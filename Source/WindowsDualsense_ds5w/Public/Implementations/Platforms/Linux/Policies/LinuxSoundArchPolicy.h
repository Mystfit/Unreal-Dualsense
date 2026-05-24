// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#pragma once
#include "GCore/Types/Structs/Context/DeviceContext.h"
#include <vector>

class FLinuxSoundArchPolicy
{
public:
	using Policy = FLinuxSoundArchPolicy;

	using DevicePathType = std::string;
	using AudioDeviceType = FLinuxSoundArchPolicy;
	using AudioDeviceIdType = std::string;
	using ContextType = FDeviceContext;

	// Send-only path: Unreal already provides samples from submix; this policy only writes to pcm audio device channel output.
	using AudioRingBufferType = std::vector<float>;
	using AudioFrameCountType = int;

	int NumChannels = 2;
	int SampleRate = 48000;
	bool bInitialized = false;
	bool bHasDeviceId = false;
	bool bRingBufferInitialized = false;
	bool bFoundDevice = false;
	bool bComInitialized = false;
	bool bAudioStarted = false;

	DevicePathType DevicePath;
	AudioDeviceIdType DeviceId;
	AudioRingBufferType RingBuffer;

	void Close() {}
	[[nodiscard]] bool IsValid() const { return false; }
	bool InitializeWithDeviceId(const AudioDeviceIdType& InDeviceId) { return false; }
	bool InitializeWithDeviceId(const AudioDeviceIdType* InDeviceId, int InSampleRate = 48000, int InNumChannels = 4) { return false; }
	void RegisterAudioDevice(const DevicePathType& InDevicePath, const AudioDeviceIdType* InDeviceId = nullptr) {}
	void UnregisterAudioDevice(const DevicePathType& InDevicePath) {}
	bool WriteHapticData(const std::vector<float>& InterleavedData) { return false; }
	bool WriteHapticData(const std::vector<float>& HapticsData, const std::vector<float>& AudioData) { return false; }
	bool InitializeAudioContainer(const ContextType* Context) { return false; }
};
