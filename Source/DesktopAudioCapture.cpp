#include "DesktopAudioCapture.h"

#ifndef JUCE_MAC

struct DesktopAudioCapture::Impl {};

DesktopAudioCapture::DesktopAudioCapture() : impl(std::make_unique<Impl>()) {
    ringBuffer.assign(ringBufferSize, 0.0f);
}

DesktopAudioCapture::~DesktopAudioCapture() {}

void DesktopAudioCapture::start(double) {}

void DesktopAudioCapture::stop() {}

void DesktopAudioCapture::pushAudio(const float* data, int numSamples) {
    const auto scope = fifo.write(numSamples);

    if (scope.blockSize1 > 0) {
        std::copy(data, data + scope.blockSize1, ringBuffer.begin() + scope.startIndex1);
    }

    if (scope.blockSize2 > 0) {
        std::copy(data + scope.blockSize1, data + scope.blockSize1 + scope.blockSize2, ringBuffer.begin() + scope.startIndex2);
    }
}

#endif
