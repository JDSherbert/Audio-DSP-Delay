// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#pragma once
 
#include <vector>
 
// ======================================================================= //
// Delay
//
// A simple single-tap digital delay line. The input signal is stored in a
// circular buffer and read back after a configurable delay time, producing
// a single echo. The wet/dry mix controls the balance between the delayed
// (wet) and original (dry) signal in the output.
//
// This is the foundational building block for more complex time-based
// effects — the Haas effect, chorus, flanger, and reverb all use variations
// of this same circular buffer delay structure.
//
// Usage:
//   Sherbert::Delay delay(500, 0.6f, 44100.0f);
//   float output = delay.ProcessSample(input);
//
// ======================================================================= //
 
namespace Sherbert
{ 
    class Delay
    {

    public:
 
        // ------------------------------------------------------------------
        // Constructor
        //
        // delayInMilliseconds - Time between the dry signal and the echo.
        //                       Must be > 0.
        //
        // wetMix              - Balance between dry and wet signal in output.
        //                       Range: [0.0, 1.0]
        //                       0.0 = fully dry (no delay heard)
        //                       1.0 = fully wet (only delayed signal heard)
        //                       0.5 = equal mix of dry and delayed signal
        //
        // sampleRate          - Audio sample rate in Hz (e.g. 44100).
        //                       Used to convert milliseconds to samples.
        //                       Must be > 0.
        // ------------------------------------------------------------------
 
        Delay(int delayInMilliseconds, float wetMix, float sampleRate);
 
        // ------------------------------------------------------------------
        // ProcessSample
        //
        // Process one sample through the delay line.
        // Call this once per sample in your audio callback loop.
        // ------------------------------------------------------------------
 
        [[nodiscard]] float ProcessSample(float input);
 
        // ------------------------------------------------------------------
        // reset
        //
        // Clears the delay buffer and resets the write index.
        // Call when playback stops or the effect is bypassed to prevent
        // stale samples bleeding into the next session.
        // ------------------------------------------------------------------
 
        void reset();
 
        // ------------------------------------------------------------------
        // Getters / Setters
        // ------------------------------------------------------------------
 
        int   getDelayInMilliseconds() const noexcept { return delayInMilliseconds; }
        float getWetMix()              const noexcept { return wetMix; }
        float getSampleRate()          const noexcept { return sampleRate; }
 
        // Note: setDelayInMilliseconds calls reset() to avoid artefacts
        // from the buffer containing samples at the old delay length.
        void setDelayInMilliseconds(int newDelayInMilliseconds);
        void setWetMix(float newWetMix) noexcept;
 
    private:
 
        int   delayInMilliseconds;
        size_t delayInSamples;
        float wetMix;
        float sampleRate;
 
        std::vector<float> delayBuffer;
        size_t             writeIndex = 0;
 
    };
 
}
 
// ======================================================================= //
