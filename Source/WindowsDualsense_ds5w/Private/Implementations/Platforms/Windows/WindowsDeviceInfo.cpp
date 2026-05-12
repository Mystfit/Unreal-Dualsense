// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025

#include "Implementations/Platforms/Windows/WindowsDeviceInfo.h"
#if PLATFORM_WINDOWS

// clang-format off
#include "GCore/Types/Structs/Config/GamepadCalibration.h"
#include "GImplementations/Utils/GamepadSensors.h"
#include "Helpers/DualSenseLog.h"
#include "Implementations/Managers/HapticsDeviceRegistry.h"
#include <filesystem>
#include <hidsdi.h>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <setupapi.h>
#include <Functiondiscoverykeys_devpkey.h>


// If you already have UE's Windows wrapper includes, keep them.
// The important bit is: include initguid.h BEFORE devpkey.h in ONE .cpp.

#ifndef INITGUID
#define INITGUID

#include <initguid.h>
#include <devpkey.h>

#endif // INITGUID
// clang-format on

void FWindowsDeviceInfo::Detect(std::vector<FDeviceContext>& Devices)
{
	Devices.clear();

	GUID HidGuid;
	HidD_GetHidGuid(&HidGuid);
	const HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(&HidGuid, nullptr, nullptr,
	                                                   DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return;
	}

	SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = {};
	DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	for (std::int32_t DeviceIndex = 0; SetupDiEnumDeviceInterfaces(DeviceInfoSet, nullptr, &HidGuid, DeviceIndex,
	                                                               &DeviceInterfaceData);
	     DeviceIndex++)
	{
		DWORD RequiredSize = 0;
		SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, nullptr, 0, &RequiredSize, nullptr);

		const auto DetailDataBuffer = static_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(malloc(RequiredSize));
		if (!DetailDataBuffer)
		{
			UE_LOG(LogDualSense, Error, TEXT("HIDManager: Failed to allocate memory for device details."));
			continue;
		}

		DetailDataBuffer->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (SetupDiGetDeviceInterfaceDetail(DeviceInfoSet, &DeviceInterfaceData, DetailDataBuffer, RequiredSize,
		                                    nullptr, nullptr))
		{
			std::string InPath = std::filesystem::path(DetailDataBuffer->DevicePath).string();
			const HANDLE TempDeviceHandle = CreateFileW(
			    DetailDataBuffer->DevicePath,
			    GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);

			if (TempDeviceHandle != INVALID_HANDLE_VALUE)
			{
				HIDD_ATTRIBUTES Attributes = {};
				Attributes.Size = sizeof(HIDD_ATTRIBUTES);
				if (HidD_GetAttributes(TempDeviceHandle, &Attributes))
				{
					if (
					    Attributes.VendorID == 0x054C &&
					    (Attributes.ProductID == 0x0CE6 ||
					     Attributes.ProductID == 0x0DF2 ||
					     Attributes.ProductID == 0x05C4 ||
					     Attributes.ProductID == 0x09CC))
					{
						FDeviceContext Context = {};
						Context.Handle = nullptr;
						Context.Path = std::filesystem::path(DetailDataBuffer->DevicePath).string();
						switch (Attributes.ProductID)
						{
							case 0x05C4:
							case 0x09CC:
								Context.DeviceType = EDSDeviceType::DualShock4;
								break;
							case 0x0CE6:
								Context.DeviceType = EDSDeviceType::DualSense;
								break;
							case 0x0DF2:
								Context.DeviceType = EDSDeviceType::DualSenseEdge;
								break;
							default:
								Context.DeviceType = EDSDeviceType::NotFound;
						}

						if (Context.DeviceType != EDSDeviceType::NotFound)
						{
							Context.IsConnected = true;
							Context.ConnectionType = EDSDeviceConnection::Usb;
							// Match both BT-Classic HID (0x1124) and BLE HID-over-GATT (0x1812).
							const std::string BluetoothClassicHIDGUID = "{00001124-0000-1000-8000-00805f9b34fb}";
							const std::string BluetoothLowEnergyHIDGUID = "{00001812-0000-1000-8000-00805f9b34fb}";
							if (Context.Path.find(BluetoothClassicHIDGUID) != std::string::npos ||
							    Context.Path.find(BluetoothLowEnergyHIDGUID) != std::string::npos)
							{
								Context.ConnectionType = EDSDeviceConnection::Bluetooth;
							}
							Devices.push_back(Context);
						}
					}
				}
				if (TempDeviceHandle != INVALID_HANDLE_VALUE)
				{
					CloseHandle(TempDeviceHandle);
				}
			}
		}
		free(DetailDataBuffer);
	}
	SetupDiDestroyDeviceInfoList(DeviceInfoSet);
}

void FWindowsDeviceInfo::Read(FDeviceContext* Context)
{
	if (!Context)
	{
		return;
	}

	if (Context->Handle == INVALID_PLATFORM_HANDLE)
	{
		return;
	}

	if (!Context->IsConnected)
	{
		return;
	}

	DWORD BytesRead = 0;
	if (Context->ConnectionType == EDSDeviceConnection::Bluetooth && Context->DeviceType == EDSDeviceType::DualShock4)
	{
		constexpr size_t InputReportLength = 547;
		EPollResult Result = PollTick(Context->Handle, Context->BufferDS4, InputReportLength, BytesRead);
		if (Result != EPollResult::ReadOk)
		{
			InvalidateHandle(Context);
		}
	}
	else
	{
		const size_t InputBufferSize = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : 64;

		// Flush all queued reports from HID buffer to prevent input lag.
		if (Context->ConnectionType == EDSDeviceConnection::Usb)
		{
			HidD_FlushQueue(Context->Handle);
		}

		EPollResult Result = PollTick(Context->Handle, Context->Buffer, InputBufferSize, BytesRead);
		if (Result != EPollResult::ReadOk)
		{
			InvalidateHandle(Context);
		}
	}
}

void FWindowsDeviceInfo::Write(FDeviceContext* Context)
{
	if (Context->Handle == INVALID_PLATFORM_HANDLE)
	{
		return;
	}

	size_t InReportLength = Context->DeviceType == EDSDeviceType::DualShock4 ? 32 : 74;
	size_t OutputReportLength = Context->ConnectionType == EDSDeviceConnection::Bluetooth ? 78 : InReportLength;

	DWORD BytesWritten = 0;
	if (!WriteFile(Context->Handle, Context->GetRawOutputBuffer(), OutputReportLength, &BytesWritten, nullptr))
	{
		InvalidateHandle(Context);
	}
}

bool FWindowsDeviceInfo::CreateHandle(FDeviceContext* DeviceContext)
{
	std::string Source = DeviceContext->Path;
	std::wstring MyStdString = std::filesystem::path(Source).wstring();
	const HANDLE DeviceHandle = CreateFileW(
	    MyStdString.data(),
	    GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, NULL, nullptr);

	if (DeviceHandle == INVALID_HANDLE_VALUE)
	{
		DeviceContext->Handle = INVALID_PLATFORM_HANDLE;
		return false;
	}

	HANDLE DuplicatedHandle = INVALID_HANDLE_VALUE;
	if (DuplicateHandle(GetCurrentProcess(), DeviceHandle, GetCurrentProcess(), &DuplicatedHandle, 0, 0, DUPLICATE_SAME_ACCESS))
	{
		CloseHandle(DeviceHandle);
		DeviceContext->Handle = DuplicatedHandle;
	}
	else
	{
		DeviceContext->Handle = DeviceHandle;
	}

	ConfigureFeatures(DeviceContext);
	return true;
}

void FWindowsDeviceInfo::InvalidateHandle(FDeviceContext* Context)
{
	if (!Context)
	{
		return;
	}

	if (Context->Handle != INVALID_PLATFORM_HANDLE)
	{
		CloseHandle(Context->Handle);
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

EPollResult FWindowsDeviceInfo::PollTick(HANDLE Handle, unsigned char* Buffer, std::int32_t Length, DWORD& OutBytesRead)
{
	std::int32_t Err = ERROR_SUCCESS;
	PingOnce(Handle, &Err);

	OutBytesRead = 0;
	if (!ReadFile(Handle, Buffer, Length, &OutBytesRead, nullptr))
	{
		return EPollResult::Disconnected;
	}

	return EPollResult::ReadOk;
}

bool FWindowsDeviceInfo::PingOnce(HANDLE Handle, std::int32_t* OutLastError)
{
	FILE_STANDARD_INFO Info{};
	if (!GetFileInformationByHandleEx(Handle, FileStandardInfo, &Info, sizeof(Info)))
	{
		if (OutLastError)
		{
			*OutLastError = GetLastError();
		}
		return false;
	}
	if (OutLastError)
	{
		*OutLastError = ERROR_SUCCESS;
	}
	return true;
}

void FWindowsDeviceInfo::ProcessAudioHaptic(FDeviceContext* Context)
{
	if (!Context || !Context->Handle)
	{
		return;
	}

	if (Context->Handle == INVALID_PLATFORM_HANDLE)
	{
		return;
	}

	if (Context->ConnectionType != EDSDeviceConnection::Bluetooth)
	{
		return;
	}

	unsigned long BytesWritten = 0;
	constexpr size_t BufferSize = sizeof(Context->BufferHapitcs);
	if (!WriteFile(Context->Handle, Context->BufferHapitcs, BufferSize, &BytesWritten, nullptr))
	{
		const unsigned long Error = GetLastError();
		if (Error != ERROR_IO_PENDING)
		{
		}
	}
}

void FWindowsDeviceInfo::ConfigureFeatures(FDeviceContext* Context)
{
	using namespace FGamepadSensors;
	FGamepadCalibration Calibration;

	if (Context->DeviceType == EDSDeviceType::DualShock4)
	{
		if (Context->ConnectionType == EDSDeviceConnection::Usb)
		{
			unsigned char FeatureBuffer[37] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x02;
			if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 37))
			{
				const unsigned long Error = GetLastError();
				return;
			}

			DualShockCalibrationSensors(FeatureBuffer, Calibration, Context->ConnectionType);
		}
		else
		{

			unsigned char FeatureBuffer[41] = {0};
			std::memset(FeatureBuffer, 0, sizeof(FeatureBuffer));

			FeatureBuffer[0] = 0x05;
			if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 41))
			{
				const unsigned long Error = GetLastError();
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
		if (!HidD_GetFeature(Context->Handle, FeatureBuffer, 41))
		{
			const unsigned long Error = GetLastError();
			return;
		}

		DualSenseCalibrationSensors(FeatureBuffer, Calibration);
		Context->Calibration = Calibration;
	}
}

#endif PLATFORM_WINDOWS
