// Copyright (c) 2026 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "Legacy/DualSenseProxy.h"
#include "API/SonyGamepadBaseProxy.h"
#include "API/SonyGamepadHapticsProxy.h"
#include "API/SonyGamepadLightsProxy.h"
#include "API/SonyGamepadProxyHelpers.h"
#include "API/SonyGamepadSettingsProxy.h"
#include "API/SonyGamepadTriggerProxy.h"
#include "Helpers/ValidateHelpers.h"

using namespace SonyGamepadProxyHelpers;
void UDualSenseProxy::DeviceSettings(int32 ControllerId, FDualSenseFeatureReport Settings)
{
	USonyGamepadSettingsProxy::DualSenseSettings(ControllerId, Settings);
}

void UDualSenseProxy::RegisterSubmixForDevice(int32 ControllerId, USoundSubmix* Submix)
{
	USonyGamepadHapticsProxy::RegisterSubmixForDevice(ControllerId, Submix);
}

void UDualSenseProxy::UnregisterSubmixForDevice(int32 ControllerId)
{
	USonyGamepadHapticsProxy::UnregisterSubmixForDevice(ControllerId);
}

void UDualSenseProxy::LedPlayerEffects(int32 ControllerId, ELedPlayerEnum Value, ELedBrightnessEnum Brightness)
{
	USonyGamepadLightsProxy::PlayerLed(ControllerId, Value, Brightness);
}

void UDualSenseProxy::SetFeedback(int32 ControllerId, int32 BeginStrength,
                                  int32 MiddleStrength, int32 EndStrength, EControllerHand Hand)
{
	if (!FValidateHelpers::ValidateMaxPosition(BeginStrength))
	{
		BeginStrength = 8;
	}
	if (!FValidateHelpers::ValidateMaxPosition(MiddleStrength))
	{
		MiddleStrength = 8;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndStrength))
	{
		EndStrength = 8;
	}

	USonyGamepadTriggerProxy::Resistance(ControllerId, MakePosition(BeginStrength), MakeStrength(BeginStrength), MakeHand(Hand));
}

void UDualSenseProxy::Resistance(int32 ControllerId, int32 StartPosition, int32 EndPosition, int32 Strength, EControllerHand Hand)
{
	if (!FValidateHelpers::ValidateMaxPosition(StartPosition))
	{
		StartPosition = 0;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndPosition))
	{
		EndPosition = 0;
	}
	if (!FValidateHelpers::ValidateMaxPosition(Strength))
	{
		Strength = 0;
	}

	USonyGamepadTriggerProxy::Resistance(ControllerId, MakePosition(StartPosition), MakeStrength(Strength), MakeHand(Hand));
}

void UDualSenseProxy::AutomaticGun(int32 ControllerId, int32 BeginStrength, int32 MiddleStrength, int32 EndStrength, EControllerHand Hand, bool KeepEffect, float Frequency)
{
	if (!FValidateHelpers::ValidateMaxPosition(BeginStrength))
	{
		BeginStrength = 6;
	}
	if (!FValidateHelpers::ValidateMaxPosition(MiddleStrength))
	{
		MiddleStrength = 8;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndStrength))
	{
		EndStrength = 8;
	}

	ETriggerEffectBehavior Behavior = ETriggerEffectBehavior::Sustained;
	if (!KeepEffect)
	{
		Behavior = ETriggerEffectBehavior::Localized;
	}
	int32 Max = FMath::Max(MiddleStrength, EndStrength);

	USonyGamepadTriggerProxy::AutomaticGun(ControllerId, MakeTPosition(BeginStrength), Behavior, MakeRecoil(Max), MakeFrequency(Frequency), MakeHand(Hand));
}

void UDualSenseProxy::GameCube(int32 ControllerId, EControllerHand Hand)
{
	USonyGamepadTriggerProxy::GameCube(ControllerId, MakeHand(Hand));
}

void UDualSenseProxy::CustomTrigger(int32 ControllerId, EControllerHand Hand, const TArray<FString>& HexBytes)
{
	if (HexBytes.Num() > 10)
	{
		return;
	}
	USonyGamepadTriggerProxy::CustomTrigger(ControllerId, HexBytes, MakeHand(Hand));
}

void UDualSenseProxy::ContinuousResistance(int32 ControllerId, int32 StartPosition, int32 Strength, EControllerHand Hand)
{
	if (!FValidateHelpers::ValidateMaxPosition(StartPosition))
	{
		StartPosition = 0;
	}
	if (!FValidateHelpers::ValidateMaxPosition(Strength))
	{
		Strength = 8;
	}

	USonyGamepadTriggerProxy::Resistance(ControllerId, MakePosition(StartPosition), MakeStrength(Strength), MakeHand(Hand));
}

void UDualSenseProxy::Galloping(
    int32 ControllerId, int32 StartPosition, int32 EndPosition, int32 FirstFoot,
    int32 SecondFoot, float Frequency, EControllerHand Hand)
{
	if (!FValidateHelpers::ValidateMaxPosition(StartPosition, 8, 1))
	{
		StartPosition = 1;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndPosition, 9, StartPosition + 1))
	{
		EndPosition = 9;
	}
	if (!FValidateHelpers::ValidateMaxPosition(FirstFoot, 8))
	{
		FirstFoot = 1;
	}
	if (!FValidateHelpers::ValidateMaxPosition(SecondFoot, 9, FirstFoot + 1))
	{
		SecondFoot = 9;
	}

	// USonyGamepadTriggerProxy::Galloping(ControllerId);
}

void UDualSenseProxy::Machine(int32 ControllerId, int32 StartPosition, int32 EndPosition, int32 FirstFoot,
                              int32 LasFoot, float Frequency, float Period, EControllerHand Hand)
{
}
void UDualSenseProxy::MachineAdvanced(int32 ControllerId, ETriggerPosition StartZone, ETriggerEffectBehavior Behavior, ETriggerForceIntensity ForceIntensity, EDualSenseTriggerAmplitude Amplitude, int32 Period, int32 Frequency, EControllerHand Hand)
{
}

void UDualSenseProxy::Weapon(int32 ControllerId, int32 StartPosition, int32 EndPosition, int32 Strength,
                             EControllerHand Hand)
{
	if (!FValidateHelpers::ValidateMaxPosition(StartPosition))
	{
		StartPosition = 0;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndPosition))
	{
		EndPosition = 8;
	}
	if (!FValidateHelpers::ValidateMaxPosition(Strength))
	{
		Strength = 8;
	}

	EDualSenseWeaponTrigger Weapon = EDualSenseWeaponTrigger::Max;
	ETriggerEffectBehavior Behavior = ETriggerEffectBehavior::Sustained;
	USonyGamepadTriggerProxy::Weapon(ControllerId, MakeTPosition(StartPosition), MakeAmplitude(EndPosition), Behavior, Weapon, MakeHand(Hand));
}

void UDualSenseProxy::Bow(int32 ControllerId, int32 StartPosition, int32 EndPosition, int32 BeginStrength, int32 EndStrength,
                          EControllerHand Hand)
{
	if (!FValidateHelpers::ValidateMaxPosition(StartPosition))
	{
		StartPosition = 2;
	}
	if (!FValidateHelpers::ValidateMaxPosition(BeginStrength))
	{
		BeginStrength = 8;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndPosition))
	{
		EndPosition = 8;
	}
	if (!FValidateHelpers::ValidateMaxPosition(EndStrength))
	{
		EndStrength = 8;
	}

	USonyGamepadTriggerProxy::Bow(ControllerId, MakeTPosition(StartPosition), MakeSnap(), MakeHand(Hand));
}

void UDualSenseProxy::NoResistance(int32 ControllerId, EControllerHand Hand)
{
	USonyGamepadTriggerProxy::StopTrigger(ControllerId, MakeHand(Hand));
}

void UDualSenseProxy::StopTriggerEffect(const int32 ControllerId, EControllerHand HandStop)
{
	USonyGamepadTriggerProxy::StopTrigger(ControllerId, MakeHand(HandStop));
}

void UDualSenseProxy::StopAllTriggersEffects(const int32 ControllerId)
{
	USonyGamepadTriggerProxy::StopTrigger(ControllerId, MakeHand(EControllerHand::AnyHand));
}

void UDualSenseProxy::ResetEffects(const int32 ControllerId)
{
	USonyGamepadTriggerProxy::StopTrigger(ControllerId, MakeHand(EControllerHand::AnyHand));
}
