// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026
#pragma once
#include "GCore/Templates/TGenericHardwareInfo.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "Implementations/Platforms/Linux/LinuxDeviceInfo.h"

// Sample Linux hardware policy adapter template
//
// This example satisfies the `IsHardwarePolicy` concept used by
// `GamepadCore::TGenericHardwareInfo`. Replace the bodies with calls to your
// concrete Linux implementation in
// `Source/Private/Implementations/Platforms/Commons/CommonsDeviceInfo.cpp`
// (e.g., forward to your FLinuxDeviceInfo logic that uses SDL HID).
namespace FLinuxPlatform
{
	struct FLinuxHardwarePolicy;
	using FLinuxHardware = GamepadCore::TGenericHardwareInfo<FLinuxHardwarePolicy>;

	struct FLinuxHardwarePolicy
	{
		FLinuxHardwarePolicy() = default;

		void Read(FDeviceContext* Context)
		{
			FLinuxDeviceInfo::Read(Context);
		}

		void Write(FDeviceContext* Context)
		{
			FLinuxDeviceInfo::Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			FLinuxDeviceInfo::Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			return FLinuxDeviceInfo::CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			FLinuxDeviceInfo::InvalidateHandle(Context);
		}

		void ProcessAudioHaptic(FDeviceContext* Context)
		{
			FLinuxDeviceInfo::ProcessAudioHaptic(Context);
		}

		void InitializeAudioDevice(FDeviceContext* Context)
		{
			FLinuxDeviceInfo::InitializeAudioDevice(Context);
		}
	};
} // namespace FLinuxPlatform
