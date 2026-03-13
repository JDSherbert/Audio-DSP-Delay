// Copyright (c) 2024 JDSherbert. All rights reserved.
 
#include <cmath>
#include <iostream>
#include <vector>
 
#include "Delay.h"
 
// ======================================================================= //
 
// Generates a single sample of a sine wave at the given frequency and time.
// frequency - Hz (e.g. 440.0 = concert A)
// time      - position in seconds (e.g. sampleIndex / sampleRate)
float GenerateInputSignal(float frequency, float time)
{
    return std::sin(2.0f * static_cast<float>(M_PI) * frequency * time);
}
 
// ======================================================================= //
 
int main()
{
    // == Signal Parameters ==============================================
    const int   sampleRate = 44100;   // 44.1 kHz
    const float frequency  = 440.0f;  // Concert A
    const float duration   = 0.1f;    // 0.1 seconds
 
    const int numSamples = static_cast<int>(sampleRate * duration);
 
    // == Generate Sine Wave =============================================
    std::vector<float> sineWave(numSamples);
    for (int i = 0; i < numSamples; ++i)
        sineWave[i] = GenerateInputSignal(frequency, i / static_cast<float>(sampleRate));
 
    // == Demonstrate: Standard Delay ====================================
    // 20ms delay, equal wet/dry mix.
    // The first 20ms of output will be silence (the buffer is empty),
    // then the delayed signal begins to appear alongside the dry signal.
    Sherbert::Delay delay(20, 0.5f, static_cast<float>(sampleRate));
 
    const int printCount = 10;
 
    std::cout << "--- 20ms Delay, wetMix: 0.5 ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float dry    = sineWave[i];
        const float output = delay.ProcessSample(dry);
 
        std::cout << "Sample " << i
                  << "  |  Dry: "    << dry
                  << "  |  Output: " << output
                  << "\n";
    }
 
    // == Demonstrate: Fully Wet =========================================
    // wetMix = 1.0; only the delayed signal is heard.
    // Output is silence for the first delayInSamples frames, then the
    // original signal appears exactly delayInSamples frames later.
    delay.reset();
    delay.setWetMix(1.0f);
 
    std::cout << "\n--- 20ms Delay, wetMix: 1.0 (fully wet) ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float dry    = sineWave[i];
        const float output = delay.ProcessSample(dry);
 
        std::cout << "Sample " << i
                  << "  |  Dry: "    << dry
                  << "  |  Output: " << output
                  << "\n";
    }
 
    // == Demonstrate: Shorter Delay =====================================
    // 5ms delay - the echo arrives sooner, closer in character to the
    // Haas effect. At very short delays it begins to sound like a
    // comb filter rather than a distinct echo.
    delay.reset();
    delay.setDelayInMilliseconds(5);
    delay.setWetMix(0.5f);
 
    std::cout << "\n--- 5ms Delay, wetMix: 0.5 ---\n\n";
 
    for (int i = 0; i < printCount; ++i)
    {
        const float dry    = sineWave[i];
        const float output = delay.ProcessSample(dry);
 
        std::cout << "Sample " << i
                  << "  |  Dry: "    << dry
                  << "  |  Output: " << output
                  << "\n";
    }
 
    return 0;
}
 
// ======================================================================= //
 
