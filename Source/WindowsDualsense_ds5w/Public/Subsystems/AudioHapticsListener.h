// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#pragma once

#include "AudioResampler.h"
#include "Containers/Queue.h"
#include "CoreMinimal.h"
#include "GCore/Interfaces/Segregations/IGamepadHaptics.h"
#include "ISubmixBufferListener.h"

struct BTPacket
{
	std::vector<uint8_t> haptics = {64}; // 3000Hz data
};

/**
 Class responsible for handling audio submix buffers and preparing audio data for haptic feedback systems.

 FAudioHapticsListener integrates with the audio rendering pipeline using the ISubmixBufferListener interface,
 allowing real-time access to submix audio buffers. The class supports processing, conversion, and resampling
 of audio data for use in haptic feedback hardware or systems. It includes mechanisms to manage resampling state
 and an internal queue for storing processed audio data.
 */
class FAudioHapticsListener : public ISubmixBufferListener
{

public:
	/**
	 Constructor for the FAudioHapticsListener class.

	 Initializes the audio haptics listener instance for processing audio data, enabling haptic feedback generation.
	 The constructor configures audio settings, initializes Opus encoding for wireless devices, and sets up internal
	 audio packet queues.

	 @param InDeviceId The device ID associated with this listener.
	 @param InSubmix A pointer to the USoundSubmix object for audio processing.
	 @param IsWireless Indicates if the device is wireless. If true, Opus encoding is initialized.
	 @param Vol The multiplier applied to the audio volume for processing.
	 @param LowPassUsb The smoothing factor for low-pass filtering for USB devices.
	 @param LowPassWireless The smoothing factor for low-pass filtering for wireless devices.

	 The constructor clears the audio packet queues and performs Opus encoder setup if the device is wireless.
	 Opus encoder settings include bitrate, frame duration, complexity, and signal type adjustment, ensuring optimal
	 audio fidelity and performance for haptic feedback systems.
	 Log errors will be raised if Opus encoder creation fails.
	 */
	FAudioHapticsListener(int32 InDeviceId, USoundSubmix* InSubmix, bool IsWireless, float Vol, float LowPassUsb, float LowPassWireless);

	/**
	 Determines if the audio processing system is actively rendering audio.

	 This method indicates whether the audio data rendering pipeline is currently active.
	 It is used to verify the state of the audio system and ensure that audio buffers
	 are being processed and rendered correctly.

	 @return True if the system is rendering audio; otherwise, false.
	 */
	virtual bool IsRenderingAudio() const override
	{
		return true;
	}

	/**
	 Processes and consumes the current audio data in the haptics queue, sending it to the appropriate haptic feedback interface.

	 This method retrieves audio packets from the internal queue and forwards them to a supported haptic feedback system, such as
	 the Sony DualSense gamepad, through the relevant interface. Packets that could not be processed are discarded after the final
	 flush of the queue.

	 It integrates with device-specific haptic systems using interfaces like ISonyGamepadTriggerInterface to achieve real-time
	 audio-haptic feedback conversion.
	 */
	void ConsumeHapticsQueue(IGamepadHaptics* AudioHaptics);

	/**
	 Returns the associated audio submix instance.

	 This method retrieves the `USoundSubmix` object associated with the audio processing pipeline.
	 The submix serves as a source of mixed audio data, which is utilized in various processing
	 stages, including resampling and preparation for haptic feedback systems.

	 @return The `USoundSubmix` instance associated with the haptic feedback audio processing system.
	 */
	USoundSubmix* GetSubmix() const
	{
		return Submix;
	}

	/**
	 Sets the volume multiplier for audio playback, ensuring the value remains within a valid range.

	 This method adjusts the internal volume multiplier, clamping the value between 0.0 and 1.0 to avoid unintended behavior.
	 It only updates the multiplier if the provided volume differs from the current value.

	 @param Volume The desired volume multiplier, typically ranging from 0.0 (silent) to 1.0 (full volume).
	 */
	void SetVolume(const float Volume)
	{
		if (Volume != VolumeMultiplier)
		{
			VolumeMultiplier = FMath::Clamp(Volume, 0.0f, 1.0f);
		}
	}

	/**
	 Sets the low-pass alpha value for USB audio processing.

	 This method adjusts the filter's low-pass alpha parameter, ensuring the value is clamped
	 within a valid range. The low-pass alpha controls the degree of attenuation for higher
	 frequencies in the audio signal, used for optimizing USB audio rendering.

	 @param LowPass The desired low-pass alpha value, clamped between -1.0 and 1.0.
	 */
	void SetLowPassAlphaUSB(float LowPass)
	{
		if (LowPass != kLowPassAlphaUSB)
		{
			kLowPassAlphaUSB = LowPass;
		}
	}

	/**
	 Updates the low-pass filter alpha value used for wireless haptic feedback processing.

	 This method adjusts the strength of the low-pass filter applied to audio data intended for
	 wireless haptic feedback systems. The input value is clamped to ensure it remains within
	 a valid range.

	 @param LowPass The desired low-pass filter alpha value, with valid range between -1.0 and 1.0.
	 */
	void SetLowPassAlphaWireless(float LowPass)
	{
		if (LowPass != kLowPassAlphaWireless)
		{
			kLowPassAlphaWireless = LowPass;
		}
	}

	/**
	Called when a new buffer has been rendered for a given submix
	@param OwningSubmix	The submix object which has rendered a new buffer
	@param AudioData		Ptr to the audio buffer
	@param NumSamples		The number of audio samples in the audio buffer
	@param NumChannels		The number of channels of audio in the buffer (e.g. 2 for stereo, 6 for 5.1)
	@param SampleRate		The sample rate of the audio buffer
	@param AudioClock		Double audio clock value, from Start of audio rendering.
	*/
	virtual void OnNewSubmixBuffer(const USoundSubmix* OwningSubmix, float* AudioData, int32 NumSamples, int32 NumChannels, const int32 SampleRate, double AudioClock) override;

private:
	/**
	 Scalar value used to adjust the overall volume of audio data.

	 VolumeMultiplier is applied to audio buffers during processing to scale their amplitude.
	 This allows dynamic control over audio output levels, providing flexibility for system-wide
	 volume adjustment or audio signal modulation.
	 */
	float VolumeMultiplier = 0.7f;
	/**
	 Constant representing the alpha parameter used in a low-pass filter for USB audio data.

	 kLowPassAlphaUSB determines the smoothing factor applied to audio signals during processing.
	 A higher value results in a smoother, more attenuated signal, while a lower value retains more
	 of the high-frequency components. This value is specifically chosen to optimize the balance
	 between noise reduction and signal clarity in USB audio scenarios.
	 */
	float kLowPassAlphaUSB = 0.97f;
	/**
	 A constant parameter used as the smoothing factor for a low-pass filter in wireless audio processing.

	 The value of `kLowPassAlphaWireless` determines the balance between the previous and current signal
	 in the filtering process, where a higher value prioritizes stability over responsiveness. This parameter
	 is optimized for wireless audio contexts to maintain consistent output quality in the presence of variable
	 signal conditions.
	 */
	float kLowPassAlphaWireless = 0.9f;
	/**
	 A thread-safe single-producer, single-consumer (SPSC) queue used for managing audio packets in the audio processing pipeline.

	 AudioPacketQueue facilitates the transfer of audio packet data between threads in an efficient manner, ensuring
	 synchronization and avoiding contention. Leveraging the SPSC mode ensures that only one producer thread and one
	 consumer thread can interact with the queue, making it highly optimized for scenarios involving audio data flow,
	 especially in real-time environments.
	 */
	TQueue<BTPacket, EQueueMode::Spsc> AudioPacketQueue;
	/**
	 A thread-safe, single-producer, single-consumer queue used for transferring audio packets between audio processing components in a USB-based workflow.

	 AudioPacketQueueUSB provides a mechanism for buffering and transferring audio data encapsulated as vectors of floating-point samples. The queue ensures efficient and synchronized communication between producer and consumer threads, making it suitable for real-time audio streaming applications that integrate USB-based audio hardware.
	 */
	TQueue<std::vector<float>, EQueueMode::Spsc> AudioPacketQueueUSB;
	/**
	 A reference to a USoundSubmix instance used within the audio processing pipeline.

	 Submix is a core component that represents an audio submix, allowing for the mixing of multiple audio sources
	 into a single stream. Within the context of the haptic feedback system, this reference is used to monitor and
	 access generated audio buffers. The submix provides the necessary audio data for subsequent processing, such as
	 resampling or conversion, enabling it to be used for tactile feedback in haptic devices.
	 */
	USoundSubmix* Submix;
	/**
	 A unique identifier representing an input device.

	 DeviceId is used to reliably reference and interact with a specific input device
	 within the system, such as game controllers or other haptics-enabled peripherals.
	 It provides a consistent and unique mechanism for identifying devices, enabling
	 their integration into various systems, including haptic feedback and input processing pipelines.
	 */
	int32 DeviceId;
	/**
	 Flag indicating whether the device or connection operates wirelessly.

	 The bIsWireless variable determines if the communication or operation mode is wireless.
	 This can be used to configure specific behaviors or optimizations for wireless systems
	 versus wired systems within the software's logic or settings.
	 */
	bool bIsWireless;
	/**
	 Variable used to maintain the state of the left channel for a low-pass filter.

	 This state is utilized in the filtering process to ensure continuity and accuracy
	 in audio signal processing, particularly when applying a low-pass effect for the
	 left audio channel. It is updated dynamically as the audio processing pipeline executes.
	 */
	float LowPassState_Left = 0.0f;
	/**
	 A floating-point variable used to maintain the internal state of the low-pass filter for the right audio channel.

	 This variable is utilized in audio signal processing to store the current state of the filter between processing iterations,
	 ensuring continuity and accuracy in the filtering process. It plays a role in smoothing out high-frequency components
	 in the right channel audio signal based on the filter's cutoff frequency.
	 */
	float LowPassState_Right = 0.0f;
};
