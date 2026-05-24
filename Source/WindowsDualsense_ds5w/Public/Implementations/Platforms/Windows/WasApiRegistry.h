// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026
#pragma once

// clang-format off
#include "GCore/Templates/TAudioDeviceRegistry.h"
#include "Implementations/Platforms/Windows/Policies/WasApiPolicy.h"
using AudioHapticsHardwarePolicy = FWasApiPolicy;
// clang-format on

using namespace GCAudio;
class FWasApiRegistry : public IAudioDevice
{
public:
	virtual ~FWasApiRegistry() override = default;

	/**
	 * Unregisters an audio device and removes its associated policy from the registry.
	 *
	 * This method removes the audio device policy associated with the given device path
	 * from the internal collection. If a policy for the specified path exists, it is
	 * removed to deregister the audio configuration for that device. A log entry is generated
	 * indicating the unregistration process.
	 *
	 * @param Path The unique path identifying the audio device whose policy is to be unregistered.
	 */
	virtual void UnregisterAudioDevice(std::string Path) override;
	/**
	 * Initializes the audio container for the specified device context.
	 *
	 * This method checks if a valid device context is provided and ensures that a policy
	 * for the device's path does not already exist. If no policy exists, a new
	 * audio device registry policy is created and initialized for the given context.
	 * The policy is then added to the internal device policies collection.
	 * Logs are generated to indicate the success or failure of the initialization process.
	 *
	 * @param Context The device context containing the path and associated information
	 *                required for initializing the audio container.
	 */
	virtual void InitializeAudioContainer(FDeviceContext* Context) override;
	/**
	 * Processes audio data to generate haptic feedback for a specific device context.
	 *
	 * This method checks the validity of the provided device context and audio data,
	 * ensuring they meet necessary conditions for processing. Only devices connected
	 * via USB are supported for audio haptic generation. If a haptic policy is found
	 * and is valid for the device, the method writes the provided audio data to generate
	 * haptic feedback on the corresponding device.
	 *
	 * @param Context The device context that includes the device path, connection type,
	 *                and other necessary metadata for the target device.
	 * @param AudioData A vector of float audio samples used as input to generate
	 *                  haptic feedback for the associated device.
	 */
	virtual void ProcessAudioHaptic(FDeviceContext* Context, const std::vector<float>& AudioData) override;

private:
	/**
	 * Maintains a collection of device-specific haptics policies mapped by device identifiers.
	 *
	 * This data structure is used for managing and storing policy configurations
	 * associated with individual haptics devices. Each device identifier (string) acts
	 * as a unique key, and its corresponding value is a shared pointer to a
	 * HapticsDevicePolicy instance that encapsulates the policy for that specific device.
	 *
	 * The unordered map allows efficient access to policies based on device identifiers,
	 * enabling quick retrieval and modification of configuration settings for specific devices.
	 */
	std::unordered_map<std::string, std::shared_ptr<GamepadCore::TAudioDeviceRegistry<AudioHapticsHardwarePolicy>>> DevicePolicies;
};
