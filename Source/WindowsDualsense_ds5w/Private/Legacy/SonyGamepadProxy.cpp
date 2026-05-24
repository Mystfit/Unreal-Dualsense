// Copyright (c) 2026 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "Legacy/SonyGamepadProxy.h"
#include "API/SonyGamepadBaseProxy.h"
#include "API/SonyGamepadLightsProxy.h"
#include "API/SonyGamepadProxyHelpers.h"
#include "API/SonyGamepadSensorsProxy.h"
#include "API/SonyGamepadTouchProxy.h"
#include "API/Types/Enums/EDeviceConnection.h"

using namespace SonyGamepadProxyHelpers;

EDeviceType USonyGamepadProxy::GetDeviceType(int32 ControllerId)
{
	return USonyGamepadBaseProxy::GetDeviceType(ControllerId);
}

EDeviceConnection USonyGamepadProxy::GetConnectionType(int32 ControllerId)
{
	return USonyGamepadBaseProxy::GetConnectionType(ControllerId);
}

bool USonyGamepadProxy::DeviceIsConnected(int32 ControllerId)
{
	return USonyGamepadBaseProxy::DeviceIsConnected(ControllerId);
}

float USonyGamepadProxy::LevelBatteryDevice(int32 ControllerId)
{
	return USonyGamepadBaseProxy::BatteryLevelDevice(ControllerId);
}

void USonyGamepadProxy::LedColorEffects(int32 ControllerId, FColor Color, float BrightnessTime, float ToogleTime)
{
	return USonyGamepadLightsProxy::LightbarFlash(ControllerId, Color, BrightnessTime, ToogleTime);
}

void USonyGamepadProxy::LedMicEffects(int32 ControllerId, ELedMicEnum Value)
{
}

void USonyGamepadProxy::StartMotionSensorCalibration(int32 ControllerId, float Duration, float DeadZone)
{
}

void USonyGamepadProxy::ResetGyroOrientation(int32 ControllerId)
{
	USonyGamepadSensorsProxy::ResetGyroOrientation(ControllerId);
}

bool USonyGamepadProxy::GetMotionSensorCalibrationStatus(int32 ControllerId, float& Progress)
{
	return false;
}

void USonyGamepadProxy::EnableTouch(int32 ControllerId, bool bEnableTouch)
{
	USonyGamepadTouchProxy::EnableTouch(ControllerId, bEnableTouch);
}

void USonyGamepadProxy::EnableGyroscopeValues(int32 ControllerId, bool bEnableGyroscope)
{
	USonyGamepadSensorsProxy::EnableGyroscopeValues(ControllerId, bEnableGyroscope);
}
