// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "API/SonyGamepadSettingsProxy.h"
#include "API/SonyGamepadProxyHelpers.h"

using namespace SonyGamepadProxyHelpers;
void USonyGamepadSettingsProxy::DualSenseSettings(int32 ControllerId, FDualSenseFeatureReport Value)
{
	FDualSenseLibrary* Gamepad = static_cast<FDualSenseLibrary*>(GetTriggerInterface(ControllerId));
	if (!Gamepad)
	{
		return;
	}

	uint8_t Speaker = 0;
	uint8_t Headset = 0;
	uint8_t Mute = 0;
	if (Value.AudioHeadset == EDualSenseAudioFeatureReport::On)
	{
		Headset = 1;
	}
	if (Value.AudioSpeaker == EDualSenseAudioFeatureReport::On)
	{
		Speaker = 1;
	}
	if (Value.MicStatus == EDualSenseAudioFeatureReport::On)
	{
		Mute = 1;
	}

	Gamepad->DualSenseSettings(
	    Mute,
	    Headset,
	    Speaker,
	    static_cast<std::uint8_t>(Value.MicVolume),
	    static_cast<std::uint8_t>(Value.AudioVolume),
	    static_cast<std::uint8_t>(Value.VibrationMode),
	    static_cast<std::uint8_t>(Value.SoftRumbleReduce),
	    static_cast<std::uint8_t>(Value.TriggerSoftnessLevel));
}

void USonyGamepadSettingsProxy::MadgwickBeta(const float Value)
{
	PluginSettings::MadgwickBeta = Value;
}

void USonyGamepadSettingsProxy::PollInterval(const float Value)
{
	PluginSettings::PollInterval = Value;
}

void USonyGamepadSettingsProxy::SetLowPassAlphaUSB(const float Value)
{
	if (Value > 0)
	{
		PluginSettings::kLowPassAlphaUSB = Value;
	}
}

void USonyGamepadSettingsProxy::SetLowPassAlphaWireless(const float Value)
{
	if (Value > 0)
	{
		PluginSettings::kLowPassAlphaWireless = Value;
	}
}
