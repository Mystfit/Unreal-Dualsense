// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "WindowsDualsense_ds5w/Public/WindowsDualsense_ds5w.h"
#include "GCore/Interfaces/IPlatformHardware.h"
#include "Implementations/Adapters/DeviceRegistry.h"

// clang-format off
#if PLATFORM_WINDOWS
#include "Implementations/Platforms/Windows/WindowsDeviceInfo.h"
#include "Implementations/Platforms/Windows/Policies/WindowsHardwarePolicy.h"
#include "Implementations/Platforms/Windows/WasApiRegistry.h"

#elif PLATFORM_LINUX
#include "Implementations/Platforms/Linux/Policies/LinuxHardwarePolicy.h"
#include "Implementations/Platforms/Linux/LinuxDeviceInfo.h"
#include "Framework/Application/SlateApplication.h"
#include "SDL.h"
#include "Subsystems/SonyInputProcessor.h"
#endif

// clang-format on

#include "API/SonyGamepadProxyHelpers.h"
#include "DeviceManager.h"
#include "Helpers/DualSenseLog.h"
#include "InputCoreTypes.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FWindowsDualsense_ds5wModule"

void FWindowsDualsense_ds5wModule::StartupModule()
{
	IModularFeatures::Get().RegisterModularFeature(GetModularFeatureName(), this);
	RegisterCustomKeys();

#if PLATFORM_WINDOWS
	// Initialize PlatformHardware, (e.g., FLinuxHardware FWindowsHardware FMacHardware, FSonyHardware)
	std::unique_ptr<IPlatformHardware> WindowsInstance = std::make_unique<FWindowsPlatform::FWindowsHardware>();
	IPlatformHardware::SetInstance(std::move(WindowsInstance));

	// Initialize Audio Platform Interface
	std::unique_ptr<IAudioDevice> WasApiAudioDevice = std::make_unique<FWasApiRegistry>();
	IAudioDevice::SetInstance(std::move(WasApiAudioDevice));

	// Initialize FDeviceRegistry
	FDeviceRegistry::Initialize();

#elif PLATFORM_LINUX
	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) != 0)
	{
		UE_LOG(LogDualSense, Error, TEXT("Failed to initialize subsystems of SDL: %s"), UTF8_TO_TCHAR(SDL_GetError()));
	}

	if (FSlateApplication::IsInitialized())
	{
		TSharedPtr<FSonyInputProcessor> SonyInputProcessor = MakeShared<FSonyInputProcessor>();
		FSlateApplication::Get().RegisterInputPreProcessor(SonyInputProcessor);
	}

	// Initialize PlatformHardware, (e.g., FLinuxHardware FWindowsHardware FMacHardware, FSonyHardware)
	std::unique_ptr<IPlatformHardware> LinuxInstance = std::make_unique<FLinuxPlatform::FLinuxHardware>();
	IPlatformHardware::SetInstance(std::move(LinuxInstance));

	FDeviceRegistry::Initialize();
#endif
}

void FWindowsDualsense_ds5wModule::ShutdownModule()
{
#if PLATFORM_LINUX
	SDL_Quit();

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(SonyInputProcessor);
	}
#endif
}

FWindowsDualsense_ds5wModule::FCustomInputDeviceFactory FWindowsDualsense_ds5wModule::CustomInputDeviceFactory = nullptr;

TSharedPtr<IInputDevice> FWindowsDualsense_ds5wModule::CreateInputDevice(
    const TSharedRef<FGenericApplicationMessageHandler>& InCustomMessageHandler)
{
	if (CustomInputDeviceFactory)
	{
		return CustomInputDeviceFactory(InCustomMessageHandler);
	}

	return MakeShareable(new DeviceManager(InCustomMessageHandler));
}

void FWindowsDualsense_ds5wModule::SetCustomInputDeviceFactory(FCustomInputDeviceFactory Factory)
{
	CustomInputDeviceFactory = Factory;
}

void FWindowsDualsense_ds5wModule::RegisterCustomKeys()
{
	const FKey Mic("PS_Mic");
	const FKey Menu("PS_Menu");
	const FKey Shared("PS_Share");
	const FKey TouchButtom("PS_TouchButtom");
	const FKey PlayStationButton("PS_Button");
	const FKey PS_PushLeftStick("PS_PushLeftStick");
	const FKey PS_PushRightStick("PS_PushRightStick");
	const FKey PS_FunctionL("PS_FunctionL");
	const FKey PS_FunctionR("PS_FunctionR");
	const FKey PS_PaddleL("PS_PaddleL");
	const FKey PS_PaddleR("PS_PaddleR");

	EKeys::AddKey(FKeyDetails(
	    PS_FunctionL,
	    FText::FromString("PlayStation Left Function Button"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    PS_FunctionR,
	    FText::FromString("PlayStation Right Function Button"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    PS_PaddleL,
	    FText::FromString("PlayStation Left Paddle"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    PS_PaddleR,
	    FText::FromString("PlayStation Right Paddle"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    PS_PushLeftStick,
	    FText::FromString("PlayStation Left Thumbstick Button"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    PS_PushRightStick,
	    FText::FromString("PlayStation Right Thumbstick Button"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    Shared,
	    FText::FromString("PlayStation Share"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    Menu,
	    FText::FromString("PlayStation Menu"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    PlayStationButton,
	    FText::FromString("PlayStation Button"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    Mic,
	    FText::FromString("PlayStation Mic"),
	    FKeyDetails::GamepadKey));

	EKeys::AddKey(FKeyDetails(
	    TouchButtom,
	    FText::FromString("PlayStation Touchpad Button"),
	    FKeyDetails::GamepadKey));
}

IMPLEMENT_MODULE(FWindowsDualsense_ds5wModule, WindowsDualsense_ds5w)
