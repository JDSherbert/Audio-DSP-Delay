![image](https://github.com/JDSherbert/Audio-Delay/assets/43964243/6a7b530e-8740-423b-a20b-defd88ea625b)
 
# Audio DSP: Delay
<!-- Header Start -->
<img align="right" alt="Stars Badge" src="https://img.shields.io/github/stars/jdsherbert/Audio-DSP-Delay?label=%E2%AD%90"/>
<img align="right" alt="Forks Badge" src="https://img.shields.io/github/forks/jdsherbert/Audio-DSP-Delay?label=%F0%9F%8D%B4"/>
<img align="right" alt="Watchers Badge" src="https://img.shields.io/github/watchers/jdsherbert/Audio-DSP-Delay?label=%F0%9F%91%81%EF%B8%8F"/>
<img align="right" alt="Issues Badge" src="https://img.shields.io/github/issues/jdsherbert/Audio-DSP-Delay?label=%E2%9A%A0%EF%B8%8F"/>
<a href="https://learn.microsoft.com/en-us/cpp/cpp-language">
  <img height="40" width="40" src="https://cdn.simpleicons.org/cplusplus">
</a>
<!-- Header End -->
 
-----------------------------------------------------------------------
 
<a href="">
  <img align="left" alt="Audio Processing" src="https://img.shields.io/badge/Audio%20Processing-black?style=for-the-badge&logo=audacity&logoColor=white&color=black&labelColor=black">
</a>
 
<a href="https://choosealicense.com/licenses/mit/">
  <img align="right" alt="License" src="https://img.shields.io/badge/License%20:%20MIT-black?style=for-the-badge&logo=mit&logoColor=white&color=black&labelColor=black">
</a>
 
<br></br>
 
-----------------------------------------------------------------------
 
## Overview
 
A simple single-tap digital delay line implemented in plain C++, intended as a learning resource for understanding the circular buffer; the foundational data structure behind almost every time-based audio effect.
 
A delay line stores incoming audio samples in a fixed-size buffer and reads them back after a configurable time, producing an echo. The wet/dry mix controls how much of the delayed signal is blended into the output. This same structure underpins chorus, flanger, reverb, and the Haas effect; all of which are variations on the same principle.
 
![image](https://github.com/JDSherbert/Audio-Delay/assets/43964243/696b4885-296f-4291-936e-b512fadc768c)
 
-----------------------------------------------------------------------
 
## Files
 
| File | Description |
|---|---|
| `Delay.h / .cpp` | Single-tap delay line with wet/dry mix |
| `main.cpp` | Example: applies the effect to a 440Hz sine wave at three different delay and mix settings |
 
-----------------------------------------------------------------------
 
## How It Works
 
The delay is implemented as a **circular buffer** - a fixed-size array with a write index that wraps around when it reaches the end. On every sample:
 
1. The sample at `writeIndex` is read — this is the sample that was written `delayInSamples` frames ago
2. The new input sample is written into `writeIndex`
3. `writeIndex` advances by one, wrapping at the buffer boundary
4. The dry and delayed samples are blended by `wetMix` and returned
 
```
output = (1.0 - wetMix) * input + wetMix * delayedSample
```
 
```
writeIndex:  0   1   2   3   4   5  ...  N-1   0   1  ...
             ↑
             read oldest sample here, write new sample here, then advance
```
 
Because the buffer size is fixed at construction, no memory is ever allocated or freed during processing, which is critical for real-time audio.
 
-----------------------------------------------------------------------
 
## Parameters
 
| Parameter | Type | Range | Description |
|---|---|---|---|
| `delayInMilliseconds` | `int` | > 0 | Time between dry signal and echo. |
| `wetMix` | `float` | 0.0 – 1.0 | Blend between dry and delayed signal. 0.0 = fully dry, 1.0 = fully wet. |
| `sampleRate` | `float` | > 0.0 | Audio sample rate in Hz. Used to convert ms to samples. |
 
**Delay time and perceived effect:**
 
| Delay Time | Perceptual Effect |
|---|---|
| 1 – 10ms | Phase/comb filtering — not perceived as echo |
| 10 – 40ms | Haas effect range — stereo widening (see [Audio Haas Effect](https://github.com/JDSherbert/Audio-Haas-Effect)) |
| 40 – 100ms | Pre-delay / ambience — adds space without a distinct echo |
| 100 – 500ms | Slap-back echo — common in rockabilly and vocal production |
| 500ms+ | Distinct repeating echo |
 
-----------------------------------------------------------------------
 
## Usage
 
### Basic
 
```cpp
// 500ms delay, equal wet/dry mix, at 44100Hz
Sherbert::Delay delay(500, 0.5f, 44100.0f);
 
float output = delay.ProcessSample(input);
```
 
### Fully Wet
 
```cpp
// Only the delayed signal is heard — silence for the first delayInSamples frames
delay.setWetMix(1.0f);
```
 
### Changing Parameters at Runtime
 
```cpp
// setDelayInMilliseconds calls reset() internally to avoid buffer artefacts
delay.setDelayInMilliseconds(250);
 
// setWetMix does not require a reset
delay.setWetMix(0.3f);
```
 
### Resetting State
 
```cpp
delay.reset();
```
 
-----------------------------------------------------------------------
 
## API Reference
 
| Method | Description |
|---|---|
| `Delay(delayInMs, wetMix, sampleRate)` | Construct with initial parameters. |
| `ProcessSample(input)` | Process one sample. Returns the blended dry/wet output. |
| `reset()` | Clears the delay buffer and resets the write index. |
| `setDelayInMilliseconds(value)` | Update delay time. Calls `reset()` internally. |
| `setWetMix(value)` | Update wet/dry mix. Range: 0.0 – 1.0. |
| `getDelayInMilliseconds()` | Returns current delay time in ms. |
| `getWetMix()` | Returns current wet mix value. |
| `getSampleRate()` | Returns the sample rate set at construction. |
 
-----------------------------------------------------------------------
 
## Limitations & Next Steps
 
This is an intentionally minimal single-tap delay for learning purposes. Real-world delay effects build on this foundation with additional features:
 
**Feedback**: a feedback delay feeds a portion of the output back into the input, producing multiple decaying repeats rather than a single echo. This is the most common next step and only requires one additional line: `input += feedbackSample * feedbackAmount` before writing to the buffer.
 
**Modulation** — modulating the delay time with an LFO produces chorus (very short delay, slow LFO) or flanger (very short delay, fast LFO) effects. This is the same LFO structure used in the phaser implementation.
 
**No filtering in the feedback path** — most hardware delay units apply a low-pass filter to the feedback signal, so each repeat gets progressively darker. This simulates the high-frequency absorption of real acoustic spaces.
 
If you want to explore further, the natural next steps from here are:
- Adding a `feedback` parameter (`input += output * feedback` before processing)
- Adding an LFO to modulate `delayInSamples` for chorus/flanger
- A low-pass filter in the feedback path for tape-style decay
- Multiple tap points in the buffer for multi-tap delay patterns
 
-----------------------------------------------------------------------
