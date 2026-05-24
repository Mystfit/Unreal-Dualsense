// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "API/SonyGamepadLightsProxy.h"
#include "API/SonyGamepadProxyHelpers.h"
#include "GCore/Types/DSCoreTypes.h"

using namespace DSCoreTypes;
using namespace SonyGamepadProxyHelpers;

void USonyGamepadLightsProxy::Lightbar(int32 ControllerId, FColor Color)
{
	auto* Gamepad = GetGamepad(ControllerId);
	if (!Gamepad)
	{
		return;
	}

	if (auto* Lights = Gamepad->GetIGamepadLightbar())
	{
		FDSColor CastColor = {Color.R, Color.G, Color.B, Color.A};
		Lights->SetLightbar(CastColor);
	}
}

void USonyGamepadLightsProxy::LightbarFlash(int32 ControllerId, FColor Color, float BrightnessTime, float ToggleTime)
{
	auto* Gamepad = GetGamepad(ControllerId);
	if (!Gamepad)
	{
		return;
	}

	if (auto* Lights = Gamepad->GetIGamepadLightbar())
	{
		FDSColor CastColor = {Color.R, Color.G, Color.B, Color.A};
		Lights->SetLightbarFlash(CastColor, BrightnessTime, ToggleTime);
	}
}

void USonyGamepadLightsProxy::PlayerLed(int32 ControllerId, ELedPlayerEnum Value, ELedBrightnessEnum Brightness)
{
	auto* Gamepad = GetGamepad(ControllerId);
	if (!Gamepad)
	{
		return;
	}

	if (auto* Lights = Gamepad->GetIGamepadLightbar())
	{
		Lights->SetPlayerLed(static_cast<EDSPlayer>(Value), static_cast<std::uint8_t>(Brightness));
	}
}
