// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "API/SonyGamepadHapticsProxy.h"
#include "API/SonyGamepadProxyHelpers.h"
#include "Implementations/Adapters/HapticsDeviceRegistry.h"

using namespace SonyGamepadProxyHelpers;
void USonyGamepadHapticsProxy::RegisterSubmixForDevice(int32 ControllerId, USoundSubmix* Submix)
{
	if (GetAudioHapticsInterface(ControllerId))
	{
		FHapticsDeviceRegistry::Get()->CreateListenerForDevice(ControllerId, Submix);
	}
}

void USonyGamepadHapticsProxy::UnregisterSubmixForDevice(int32 ControllerId)
{
	if (GetAudioHapticsInterface(ControllerId))
	{
		FHapticsDeviceRegistry::Get()->RemoveListenerForDevice(ControllerId);
	}
}
