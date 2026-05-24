// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "Implementations/Platforms/Others/GamepadHardwareBridge.h"
#include "Helpers/DualSenseLog.h"
#include "Implementations/Adapters/DeviceRegistry.h"

void FGamepadHardwareBridge::InjectHardwarePlatform(std::unique_ptr<IPlatformHardware> InPlatform)
{
	if (InPlatform)
	{
		// Initialize PlatformHardware, (e.g., FMacHardware, FSonyHardware)
		IPlatformHardware::SetInstance(std::move(InPlatform));

		// Re-Initialize devices
		FDeviceRegistry::Initialize();
		UE_LOG(LogDualSense, Log, TEXT("GamepadCore: Platform swapped and Registry re-initialized."));
	}
}

void FGamepadHardwareBridge::InjectHardwarePlatform(std::unique_ptr<IPlatformHardware> InPlatform, std::unique_ptr<GCAudio::IAudioDevice> AudioDevice)
{
	if (InPlatform)
	{
		// Initialize PlatformHardware, (e.g., FMacHardware, FSonyHardware)
		IPlatformHardware::SetInstance(std::move(InPlatform));

		// Re-Initialize devices
		FDeviceRegistry::Initialize();
		UE_LOG(LogDualSense, Log, TEXT("GamepadCore: Platform swapped and Registry re-initialized."));
	}

	if (AudioDevice)
	{
		// Set the audio platform interface
		GCAudio::IAudioDevice::SetInstance(std::move(AudioDevice));
	}
}
