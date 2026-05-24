// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "API/SonyGamepadTouchProxy.h"
#include "API/SonyGamepadProxyHelpers.h"
#include "GCore/Interfaces/Segregations/IGamepadBase.h"

using namespace SonyGamepadProxyHelpers;
void USonyGamepadTouchProxy::EnableTouch(int32 ControllerId, bool bEnableTouch)
{
	IGamepadBase* Gamepad = GetGamepad(ControllerId);
	if (!Gamepad)
	{
		return;
	}

	if (auto* Touch = Gamepad->GetIGamepadTouch())
	{
		Touch->EnableTouch(bEnableTouch);
	}
}

void USonyGamepadTouchProxy::EnableGesture(int32 ControllerId, bool bEnableGesture)
{
	IGamepadBase* Gamepad = GetGamepad(ControllerId);
	if (!Gamepad)
	{
		return;
	}

	if (auto* Touch = Gamepad->GetIGamepadTouch())
	{
		Touch->EnableGesture(bEnableGesture);
	}
}
