// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "Implementations/Platforms/Linux/LinuxDeviceInfo.h"

#ifdef __unix__
#include "API/SonyGamepadProxyHelpers.h"
#include "GCore/Types/ECoreGamepad.h"
#include "GCore/Types/Structs/Config/GamepadCalibration.h"
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include "GImplementations/Utils/GamepadSensors.h"
#include "SDL_hidapi.h"
#include <cstring>
#include <string>
#include <unordered_set>

static const std::uint16_t SONY_VENDOR_ID = 0x054C;
static const std::uint16_t DUALSHOCK4_PID_V1 = 0x05C4;
static const std::uint16_t DUALSHOCK4_PID_V2 = 0x09CC;
static const std::uint16_t DUALSENSE_PID = 0x0CE6;
static const std::uint16_t DUALSENSE_EDGE_PID = 0x0DF2;

void FLinuxDeviceInfo::Read(FDeviceContext* Context)
{
	if (!Context || !Context->Handle)
	{
		return;
	}
	SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);
	if (!DeviceHandle)
	{
		return;
	}

	if (Context->ConnectionType == EDSDeviceConnection::Bluetooth && Context->DeviceType == EDSDeviceType::DualShock4)
	{
		const size_t InputReportLength = 547;
		if (SDL_hid_read(DeviceHandle, Context->BufferDS4, InputReportLength) < 0)
		{
			InvalidateHandle(Context);
		}
		return;
	}

	const size_t InputReportLength = (Context->ConnectionType == EDSDeviceConnection::Bluetooth) ? 78 : 64;
	if (sizeof(Context->Buffer) < InputReportLength)
	{
		InvalidateHandle(Context);
		return;
	}

	if (SDL_hid_read(DeviceHandle, Context->Buffer, InputReportLength) < 0)
	{
		InvalidateHandle(Context);
	}
}

void FLinuxDeviceInfo::ProcessAudioHaptic(FDeviceContext* Context)
{
	if (!Context || !Context->Handle)
	{
		return;
	}

	SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);

	constexpr size_t Report = 142;
	int BytesWritten = SDL_hid_write(DeviceHandle, Context->BufferHapitcs, Report);
	if (BytesWritten < 0)
	{
	}
}

void FLinuxDeviceInfo::ConfigureFeatures(FDeviceContext* Context)
{
	SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);

	using namespace FGamepadSensors;
	FGamepadCalibration Calibration;

	if (Context->DeviceType == EDSDeviceType::DualShock4)
	{
		if (Context->ConnectionType == EDSDeviceConnection::Usb)
		{
			unsigned char FeatureBuffer[37] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x02;
			if (!SDL_hid_get_feature_report(DeviceHandle, FeatureBuffer, 41))
			{
				return;
			}

			DualShockCalibrationSensors(FeatureBuffer, Calibration, Context->ConnectionType);
		}
		else
		{

			unsigned char FeatureBuffer[41] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x05;
			if (!SDL_hid_get_feature_report(DeviceHandle, FeatureBuffer, 41))
			{
				return;
			}

			DualShockCalibrationSensors(FeatureBuffer, Calibration, Context->ConnectionType);
		}

		Context->Calibration = Calibration;
	}
	else
	{
		unsigned char FeatureBuffer[41] = {0};
		std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

		FeatureBuffer[0] = 0x05;
		if (!SDL_hid_get_feature_report(DeviceHandle, FeatureBuffer, 41))
		{
			return;
		}

		DualSenseCalibrationSensors(FeatureBuffer, Calibration);
		Context->Calibration = Calibration;
	}
	return;
}

void FLinuxDeviceInfo::Write(FDeviceContext* Context)
{
	if (!Context || !Context->Handle)
	{
		return;
	}

	SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);

	const size_t InReportLength = (Context->DeviceType == EDSDeviceType::DualShock4) ? 32 : 74;
	const size_t OutputReportLength = (Context->ConnectionType == EDSDeviceConnection::Bluetooth) ? 78 : InReportLength;

	int BytesWritten = SDL_hid_write(DeviceHandle, Context->GetRawOutputBuffer(), OutputReportLength);
	if (BytesWritten < 0)
	{
		InvalidateHandle(Context);
	}
}

void FLinuxDeviceInfo::Detect(std::vector<FDeviceContext>& Devices)
{
	Devices.clear();

	static const std::unordered_set<uint16_t> SupportedPIDs = {
	    DUALSHOCK4_PID_V1,
	    DUALSHOCK4_PID_V2,
	    DUALSENSE_PID,
	    DUALSENSE_EDGE_PID};

	SDL_hid_device_info* Devs = SDL_hid_enumerate(SONY_VENDOR_ID, 0);
	if (!Devs)
	{
		return;
	}

	for (SDL_hid_device_info* CurrentDevice = Devs; CurrentDevice != nullptr; CurrentDevice = CurrentDevice->next)
	{
		if (SupportedPIDs.contains(CurrentDevice->product_id))
		{
			FDeviceContext NewDeviceContext;
			NewDeviceContext.Path = std::string(CurrentDevice->path);

			switch (CurrentDevice->product_id)
			{
				case DUALSHOCK4_PID_V1:
				case DUALSHOCK4_PID_V2:
					NewDeviceContext.DeviceType = EDSDeviceType::DualShock4;
					break;
				case DUALSENSE_EDGE_PID:
					NewDeviceContext.DeviceType = EDSDeviceType::DualSenseEdge;
					break;
				case DUALSENSE_PID:
				default:
					NewDeviceContext.DeviceType = EDSDeviceType::DualSense;
					break;
			}

			NewDeviceContext.IsConnected = true;
			if (CurrentDevice->interface_number == -1)
			{
				NewDeviceContext.ConnectionType = EDSDeviceConnection::Bluetooth;
			}
			else
			{
				NewDeviceContext.ConnectionType = EDSDeviceConnection::Usb;
			}
			NewDeviceContext.Handle = nullptr;
			Devices.push_back(NewDeviceContext);
		}
	}
	SDL_hid_free_enumeration(Devs);
}

bool FLinuxDeviceInfo::CreateHandle(FDeviceContext* Context)
{
	if (!Context)
	{
		return false;
	}

	const char* Path = Context->Path.data();
	const FPlatformDeviceHandle Handle = SDL_hid_open_path(Path, true);
	if (Handle == INVALID_PLATFORM_HANDLE)
	{
		return false;
	}

	SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Handle);
	SDL_hid_set_nonblocking(DeviceHandle, 1);
	Context->Handle = Handle;

	ConfigureFeatures(Context);
	return true;
}

void FLinuxDeviceInfo::InvalidateHandle(FDeviceContext* Context)
{
	if (Context)
	{
		SDL_hid_device* DeviceHandle = static_cast<SDL_hid_device*>(Context->Handle);
		if (DeviceHandle != nullptr)
		{
			SDL_hid_close(DeviceHandle);
		}

		Context->Handle = INVALID_PLATFORM_HANDLE;
		Context->IsConnected = false;

		Context->Path.clear();
		unsigned char* RawOutput = Context->GetRawOutputBuffer();
		std::memset(RawOutput, 0, 78);
		std::memset(Context->Buffer, 0, 78);
		std::memset(Context->BufferDS4, 0, 547);
		std::memset(Context->BufferHapitcs, 0, 142);
	}
}
void FLinuxDeviceInfo::InitializeAudioDevice(FDeviceContext* Context)
{
	if (!Context)
	{
		return;
	}

	// Initialize miniaudio context for device enumeration
	ma_context maContext;
	if (ma_context_init(nullptr, 0, nullptr, &maContext) != MA_SUCCESS)
	{
		return;
	}

	// Get playback devices
	ma_device_info* pPlaybackInfos;
	ma_uint32 playbackCount;
	ma_device_info* pCaptureInfos;
	ma_uint32 captureCount;

	if (ma_context_get_devices(&maContext, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount) != MA_SUCCESS)
	{
		ma_context_uninit(&maContext);
		return;
	}

	// Search for DualSense audio device
	ma_device_id* pFoundDeviceId = nullptr;
	ma_device_id foundDeviceId;

	for (ma_uint32 i = 0; i < playbackCount; i++)
	{
		std::string deviceName(pPlaybackInfos[i].name);

		// Check if device name contains DualSense identifiers
		// DualSense appears as "Wireless Controller" or "DualSense Wireless Controller"
		if (deviceName.find("DualSense") != std::string::npos ||
		    deviceName.find("Wireless Controller") != std::string::npos)
		{
			// Verify it has 4 channels (stereo + haptic L/R)
			// DualSense audio device typically has 4 channels for haptics
			foundDeviceId = pPlaybackInfos[i].id;
			pFoundDeviceId = &foundDeviceId;
			break;
		}
	}

	// Initialize audio context with found device (or default if not found)
	Context->AudioContext = std::make_shared<FAudioDeviceContext>();

	if (pFoundDeviceId)
	{
		// Initialize with specific DualSense device
		// DualSense haptics use 4 channels at 48000 Hz
		Context->AudioContext->InitializeWithDeviceId(pFoundDeviceId, 48000, 4);
	}

	ma_context_uninit(&maContext);
}

#endif
