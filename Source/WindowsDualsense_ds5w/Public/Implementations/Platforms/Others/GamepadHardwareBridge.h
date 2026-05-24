// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#pragma once
#include "GCore/Interfaces/IAudioDevice.h"
#include "GCore/Interfaces/IPlatformHardware.h"

class WINDOWSDUALSENSE_DS5W_API FGamepadHardwareBridge
{
	/**
	 * Injects a hardware platform into the gamepad hardware bridge. This method
	 * sets the platform hardware instance and reinitializes the device registry to
	 * accommodate the injected platform.
	 *
	 * @param InPlatform A unique pointer to the platform-specific hardware information
	 *                   to be injected (e.g., FMacHardware, FSonyHardware). It must
	 *                   not be null.
	 */
public:
	static void InjectHardwarePlatform(std::unique_ptr<IPlatformHardware> InPlatform);
	/**
	 * Injects a hardware platform and audio device into the gamepad hardware bridge.
	 * This method sets the platform hardware instance, reinitializes the device registry
	 * to accommodate the injected platform, and assigns the audio device instance.
	 *
	 * @param InPlatform A unique pointer to the platform-specific hardware information
	 *                   to be injected (e.g., FMacHardware, FSonyHardware). It is used
	 *                   to initialize the platform hardware instance and reinitialize
	 *                   the device registry. It must not be null.
	 * @param AudioDevice A unique pointer to the audio device to be injected. It sets
	 *                    the audio device instance for use in audio-related functionality.
	 *                    It can be null, in which case the audio device is not updated.
	 */
	static void InjectHardwarePlatform(std::unique_ptr<IPlatformHardware> InPlatform, std::unique_ptr<GCAudio::IAudioDevice> AudioDevice);
};
