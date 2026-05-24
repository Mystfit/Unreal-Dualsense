// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "Implementations/Platforms/Windows/WasApiRegistry.h"
#include "Helpers/DualSenseLog.h"

void FWasApiRegistry::UnregisterAudioDevice(std::string Path)
{
	if (const auto it = DevicePolicies.find(Path); it != DevicePolicies.end())
	{
		UE_LOG(LogDualSense, Log, TEXT("Unregister audio policy for Controller path %s"), *FString(it->first.c_str()));
		DevicePolicies.erase(it);
	}
}

void FWasApiRegistry::InitializeAudioContainer(FDeviceContext* Context)
{
	if (!Context)
	{
		UE_LOG(LogDualSense, Error, TEXT("InitializeAudioContainer: Context is null"));
		return;
	}

	if (const auto it = DevicePolicies.find(Context->Path); it != DevicePolicies.end())
	{
		UE_LOG(LogDualSense, Error, TEXT("InitializeAudioContainer: Device policy already exists"));
		return;
	}

	auto Policy = std::make_shared<GamepadCore::TAudioDeviceRegistry<AudioHapticsHardwarePolicy>>();
	if (!Policy->InitializeAudioContainer(Context))
	{
		return;
	}

	DevicePolicies.emplace(Context->Path, Policy);
	UE_LOG(LogDualSense, Log, TEXT("InitializeAudioContainer: %llu Registered audio policy for path %s."), DevicePolicies.size(), *FString(Context->Path.c_str()));
}

void FWasApiRegistry::ProcessAudioHaptic(FDeviceContext* Context, const std::vector<float>& AudioData)
{
	if (!Context || AudioData.empty())
	{
		return;
	}

	// Only process USB connections for audio haptics
	if (Context->ConnectionType != EDSDeviceConnection::Usb)
	{
		return;
	}

	// Find the policy mapped to this device path
	const auto it = DevicePolicies.find(Context->Path);
	if (it == DevicePolicies.end() || !it->second)
	{
		return;
	}

	auto& Policy = it->second;
	if (!Policy->IsValid())
	{
		return;
	}

	Policy->Policy.WriteHapticData(AudioData);
}
