#include "DesktopAudioCapture.h"

#ifndef JUCE_MAC

struct DesktopAudioCapture::Impl {};

DesktopAudioCapture::DesktopAudioCapture() : impl(std::make_unique<Impl>()) {}

DesktopAudioCapture::~DesktopAudioCapture() {}

bool DesktopAudioCapture::isSupported() { return false; }

void DesktopAudioCapture::start(double) {}

void DesktopAudioCapture::stop() {}

void DesktopAudioCapture::pushAudio(const float*, int) {}

#endif
