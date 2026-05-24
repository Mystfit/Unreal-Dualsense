// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026
#pragma once
#include "GCore/Templates/TGenericHardwareInfo.h"

#ifdef _WIN32
#include "Implementations/Platforms/Windows/WindowsDeviceInfo.h"
namespace FWindowsPlatform
{
	struct FWindowsHardwarePolicy;
	using FWindowsHardware = GamepadCore::TGenericHardwareInfo<FWindowsHardwarePolicy>;

	struct FWindowsHardwarePolicy
	{
	public:
		void Read(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::Read(Context);
		}

		void Write(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::Write(Context);
		}

		void Detect(std::vector<FDeviceContext>& Devices)
		{
			FWindowsDeviceInfo::Detect(Devices);
		}

		bool CreateHandle(FDeviceContext* Context)
		{
			return FWindowsDeviceInfo::CreateHandle(Context);
		}

		void InvalidateHandle(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::InvalidateHandle(Context);
		}

		void ProcessAudioHaptic(FDeviceContext* Context)
		{
			FWindowsDeviceInfo::ProcessAudioHaptic(Context);
		}
	};
} // namespace FWindowsPlatform
#endif
