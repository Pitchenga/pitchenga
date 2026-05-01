#include "DesktopAudioCapture.h"
#include "Util.h"

#if PITCHENGA_USE_SCREEN_CAPTURE

#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <CoreMedia/CoreMedia.h>
#import <AVFoundation/AVFoundation.h>

@interface DesktopAudioCaptureDelegate : NSObject <SCStreamOutput>
@property (nonatomic, assign) DesktopAudioCapture* owner;
@end

@implementation DesktopAudioCaptureDelegate

- (void)stream:(SCStream *)stream didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer ofType:(SCStreamOutputType)type {
    if (type != SCStreamOutputTypeAudio) return;

    CMBlockBufferRef blockBuffer = nullptr;
    AudioBufferList audioBufferList;
    
    OSStatus status = CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(
        sampleBuffer, 
        nullptr, 
        &audioBufferList, 
        sizeof(audioBufferList),
        nullptr, 
        nullptr, 
        kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment, 
        &blockBuffer
    );

    if (status != noErr) return;

    if (audioBufferList.mNumberBuffers > 0) {
        const float* audioData = static_cast<const float*>(audioBufferList.mBuffers[0].mData);
        const int numSamples = static_cast<int>(audioBufferList.mBuffers[0].mDataByteSize / sizeof(float));
        
        if (self.owner && audioData != nullptr) {
            self.owner->pushAudio(audioData, numSamples);
        }
    }
    
    if (blockBuffer != nullptr) {
        CFRelease(blockBuffer);
    }
}

@end

struct DesktopAudioCapture::Impl {
    SCStream* stream = nil;
    DesktopAudioCaptureDelegate* delegate = nil;
    dispatch_queue_t queue = nil;
};

DesktopAudioCapture::DesktopAudioCapture() : impl(std::make_unique<Impl>()) {
    ringBuffer.assign(ringBufferSize, 0.0f);
    impl->delegate = [[DesktopAudioCaptureDelegate alloc] init];
    impl->delegate.owner = this;
    impl->queue = dispatch_queue_create("com.pitchenga.desktopcapture", DISPATCH_QUEUE_SERIAL);
}

DesktopAudioCapture::~DesktopAudioCapture() {
    stop();
    impl->delegate.owner = nullptr;
}

void DesktopAudioCapture::start(double sampleRate) {
    if (impl->stream) return;

    [SCShareableContent getShareableContentWithCompletionHandler:^(SCShareableContent *content, NSError *error) {
        if (error != nil) {
            Util::debug("SCShareableContent error: " + juce::String([error.localizedDescription UTF8String]));
            return;
        }
        
        SCDisplay *display = content.displays.firstObject;
        if (display == nil) {
            Util::debug("No display found for ScreenCaptureKit");
            return;
        }
        
        SCContentFilter *filter = [[SCContentFilter alloc] initWithDisplay:display excludingApplications:@[] exceptingWindows:@[]];
        
        SCStreamConfiguration *config = [[SCStreamConfiguration alloc] init];
        config.capturesAudio = YES;
        config.excludesCurrentProcessAudio = YES;
        config.sampleRate = static_cast<NSInteger>(sampleRate);
        config.channelCount = 1;
        config.queueDepth = 3; // Low latency
        
        impl->stream = [[SCStream alloc] initWithFilter:filter configuration:config delegate:nil];
        
        NSError* outputError = nil;
        [impl->stream addStreamOutput:impl->delegate type:SCStreamOutputTypeAudio sampleHandlerQueue:impl->queue error:&outputError];
        
        if (outputError != nil) {
            Util::debug("addStreamOutput error: " + juce::String([outputError.localizedDescription UTF8String]));
            return;
        }
        
        [impl->stream startCaptureWithCompletionHandler:^(NSError *startError) {
            if (startError != nil) {
                Util::debug("startCapture error: " + juce::String([startError.localizedDescription UTF8String]));
            } else {
                Util::debug("Desktop audio capture started");
            }
        }];
    }];
}

void DesktopAudioCapture::stop() {
    if (impl->stream) {
        [impl->stream stopCaptureWithCompletionHandler:^(NSError *error) {
            if (error != nil) {
                Util::debug("stopCapture error: " + juce::String([error.localizedDescription UTF8String]));
            }
        }];
        impl->stream = nil;
    }
}

#else

struct DesktopAudioCapture::Impl {};

DesktopAudioCapture::DesktopAudioCapture() : impl(std::make_unique<Impl>()) {
    ringBuffer.assign(ringBufferSize, 0.0f);
}

DesktopAudioCapture::~DesktopAudioCapture() {}

void DesktopAudioCapture::start(double) {}

void DesktopAudioCapture::stop() {}

#endif

void DesktopAudioCapture::pushAudio(const float* data, int numSamples) {
    const auto scope = fifo.write(numSamples);

    if (scope.blockSize1 > 0) {
        std::copy(data, data + scope.blockSize1, ringBuffer.begin() + scope.startIndex1);
    }

    if (scope.blockSize2 > 0) {
        std::copy(data + scope.blockSize1, data + scope.blockSize1 + scope.blockSize2, ringBuffer.begin() + scope.startIndex2);
    }
}
