// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include "Delay.h"
 
#include <algorithm>
#include <cassert>
 
// ======================================================================= //
 
Sherbert::Delay::Delay(int delayInMilliseconds, float wetMix, float sampleRate)
    : delayInMilliseconds(delayInMilliseconds)
    , delayInSamples(static_cast<size_t>(delayInMilliseconds * sampleRate / 1000.0f))
    , wetMix(wetMix)
    , sampleRate(sampleRate)
    , writeIndex(0)
{
    assert(delayInMilliseconds > 0);
    assert(wetMix >= 0.0f && wetMix <= 1.0f);
    assert(sampleRate > 0.0f);
 
    delayBuffer.resize(delayInSamples, 0.0f);
}
 
// ======================================================================= //
 
float Sherbert::Delay::ProcessSample(float input)
{
    // === HOW THE DELAY LINE WORKS ======================================
    //
    // A circular buffer of fixed size stores the incoming samples. On each
    // call, the sample that was written 'delayInSamples' frames ago is read
    // from the current write position (which is also the oldest position in
    // the buffer), and the new input sample is written in its place.
    //
    // The write index advances by one each call and wraps at the buffer end,
    // so the buffer always contains exactly 'delayInSamples' of history with
    // no allocations during processing.
    //
    // The dry signal (input) and wet signal (delayedSample) are then blended
    // according to wetMix:
    //   output = (1 - wetMix) * dry + wetMix * wet
    //
    // At wetMix = 0.0: output = dry  (delay inaudible)
    // At wetMix = 0.5: output = equal blend of dry and delayed signal
    // At wetMix = 1.0: output = wet  (only the echo is heard)
    // =======================================================================
 
    const float delayedSample = delayBuffer[writeIndex];
    delayBuffer[writeIndex] = input;
    writeIndex = (writeIndex + 1) % delayInSamples;
 
    return (1.0f - wetMix) * input + wetMix * delayedSample;
}
 
// ======================================================================= //
 
void Sherbert::Delay::reset()
{
    std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
    writeIndex = 0;
}
 
// ======================================================================= //
 
void Sherbert::Delay::setDelayInMilliseconds(int newDelayInMilliseconds)
{
    assert(newDelayInMilliseconds > 0);
    delayInMilliseconds = newDelayInMilliseconds;
    delayInSamples = static_cast<size_t>(delayInMilliseconds * sampleRate / 1000.0f);
    delayBuffer.resize(delayInSamples, 0.0f);
    reset();
}
 
// ======================================================================= //
 
void Sherbert::Delay::setWetMix(float newWetMix) noexcept
{
    assert(newWetMix >= 0.0f && newWetMix <= 1.0f);
    wetMix = newWetMix;
}
 
// ======================================================================= //
 
