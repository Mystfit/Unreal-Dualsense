// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#include "Implementations/Adapters/HapticsDeviceRegistry.h"
#include "API/SonyGamepadProxyHelpers.h"
#include "API/SonyGamepadSettingsProxy.h"
#include "Async/Async.h"
#include "Audio.h"
#include "AudioDevice.h"
#include "AudioMixer.h"
#include "AudioMixerDevice.h"
#include "Misc/App.h"
#include "Runtime/Launch/Resources/Version.h"
#include "Sound/SoundSubmix.h"

TSharedPtr<FHapticsDeviceRegistry> FHapticsDeviceRegistry::Instance;

FHapticsDeviceRegistry::~FHapticsDeviceRegistry()
{
	RemoveAllListeners();
	if (Instance->GameThreadTickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(Instance->GameThreadTickerHandle);
	}
}

bool FHapticsDeviceRegistry::HasListenerForDevice(int32 DeviceId) const
{
	return ControllerListeners.Contains(DeviceId);
}

TSharedPtr<FHapticsDeviceRegistry> FHapticsDeviceRegistry::Get()
{
	if (!Instance)
	{
		check(IsInGameThread());
		Instance = MakeShared<FHapticsDeviceRegistry>();

		Instance->GameThreadTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		    FTickerDelegate::CreateSP(Instance.Get(), &FHapticsDeviceRegistry::Tick));
	}
	return Instance;
}

void FHapticsDeviceRegistry::OnAudioDevicesObtained(const TArray<FAudioOutputDeviceInfo>& AvailableDevices)
{
}

void FHapticsDeviceRegistry::CreateListenerForDevice(int32 DeviceId, USoundSubmix* Submix)
{
	if (!Submix)
	{
		return;
	}

	using namespace SonyGamepadProxyHelpers;
	auto* Gamepad = GetGamepad(DeviceId);
	if (!Gamepad)
	{
		return;
	}

	if (Gamepad->GetConnectionType() == EDSDeviceConnection::Usb)
	{
		if (Gamepad->GetIGamepadHaptics())
		{
			FDeviceContext* Ctx = Gamepad->GetMutableDeviceContext();
			if (!Ctx)
			{
				return;
			}

			if (FAudioDeviceManager* DeviceManager = GEngine->GetAudioDeviceManager())
			{
				TArray<FAudioDevice*> AllDevices = DeviceManager->GetAudioDevices();
				for (const auto& Device : AllDevices)
				{
					if (Audio::FMixerDevice* MixerDevice = static_cast<Audio::FMixerDevice*>(Device))
					{
						if (Audio::IAudioMixerPlatformInterface* MixerPlatform = MixerDevice->GetAudioMixerPlatform())
						{
							uint32 NumDevices = 0;
							MixerPlatform->GetNumOutputDevices(NumDevices);
							for (uint32 i = 0; i < NumDevices; ++i)
							{
								Audio::FAudioPlatformDeviceInfo DeviceInfo;
								MixerPlatform->GetOutputDeviceInfo(i, DeviceInfo);
								if (DeviceInfo.Name.Contains(TEXT("DualSense")))
								{
									FString Command = FString::Printf(TEXT("AudioMixer.SetAudioDevice %s"), *DeviceInfo.DeviceId);
									GEngine->Exec(GEngine->GetWorld(), *Command);

									IAudioDevice::Get().InitializeAudioContainer(Ctx);
									UE_LOG(LogDualSense, Log, TEXT("Controller listener registered. %s"), *DeviceInfo.Name);
									break;
								}
							}
						}
					}
					break;
				}
			}
		}
	}

	if (ControllerListeners.Contains(DeviceId))
	{
		UE_LOG(LogDualSense, Warning, TEXT("Controller %d already has a listener registered."), DeviceId);
		return;
	}

	bool IsWireless = Gamepad->GetConnectionType() == EDSDeviceConnection::Bluetooth;
	const TSharedPtr<FAudioHapticsListener> Listener = MakeShared<FAudioHapticsListener>(DeviceId, Submix, IsWireless, 0.7f, 0.97f, 0.9f);
	if (FAudioDeviceHandle AudioDevice = GEngine->GetActiveAudioDevice())
	{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 3
		AudioDevice->RegisterSubmixBufferListener(Listener.ToSharedRef(), *Submix);
#else
		AudioDevice->RegisterSubmixBufferListener(Listener.Get(), Submix);
#endif

		UE_LOG(LogDualSense, Log, TEXT("Controller %d Listener %s"), DeviceId, *Submix->GetName());
		ControllerListeners.Add(DeviceId, Listener);
	}
}

void FHapticsDeviceRegistry::RemoveAllListeners()
{
	if (FAudioDeviceHandle AudioDevice = GEngine->GetActiveAudioDevice())
	{
		for (auto& Pair : ControllerListeners)
		{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 3
			AudioDevice->UnregisterSubmixBufferListener(Pair.Value.ToSharedRef(), *Pair.Value.ToSharedRef()->GetSubmix());
#else
			AudioDevice->UnregisterSubmixBufferListener(Pair.Value.Get());
#endif
			UE_LOG(LogDualSense, Warning, TEXT("Unregistered listener for controller %d"), Pair.Key);

			using namespace SonyGamepadProxyHelpers;
			auto* Gamepad = GetGamepad(Pair.Key);
			if (!Gamepad)
			{
				continue;
			}

			const auto* Ctx = Gamepad->GetMutableDeviceContext();
			if (!Ctx)
			{
				continue;
			}

			if (Ctx->ConnectionType == EDSDeviceConnection::Usb)
			{
				IAudioDevice::Get().UnregisterAudioDevice(Ctx->Path);
			}
		}

		ControllerListeners.Empty();
	}
}

bool FHapticsDeviceRegistry::Tick(float DeltaTime)
{
	for (auto& Pair : ControllerListeners)
	{
		if (Pair.Value.IsValid())
		{
			auto* Context = Pair.Value.Get();
			if (!Context)
			{
				return false;
			}

			float Volume = 0.7f;
			if (auto* Gamepad = SonyGamepadProxyHelpers::GetGamepad(Pair.Key))
			{
				const auto* Ctx = Gamepad->GetMutableDeviceContext();
				Volume = Ctx->Output.Audio.HeadsetVolume > 0 ? FMath::Max(Ctx->Output.Audio.HeadsetVolume, Ctx->Output.Audio.SpeakerVolume) / 100.0f : 0.0f;
				Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
			}

			float LowPassAlpha = 0.9f;
			float LowPassAlphaBT = 0.9f;
			if (PluginSettings::kLowPassAlphaUSB)
			{
				LowPassAlpha = PluginSettings::kLowPassAlphaUSB;
				LowPassAlphaBT = PluginSettings::kLowPassAlphaWireless;
			}

			Context->SetVolume(Volume);
			Context->SetLowPassAlphaUSB(LowPassAlpha);
			Context->SetLowPassAlphaWireless(LowPassAlphaBT);

			if (IGamepadHaptics* HapticsInterface = SonyGamepadProxyHelpers::GetAudioHapticsInterface(Pair.Key))
			{
				AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [NewContext = MoveTemp(Context), NewHapticsInterface = MoveTemp(HapticsInterface)]() {
					NewContext->ConsumeHapticsQueue(NewHapticsInterface);
				});
			}
		}
	}
	return true;
}

void FHapticsDeviceRegistry::RemoveListenerForDevice(int32 DeviceId)
{
	if (const TSharedPtr<FAudioHapticsListener>* ExistingListener = ControllerListeners.Find(DeviceId))
	{

		if (FAudioDeviceHandle AudioDevice = GEngine->GetActiveAudioDevice())
		{
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION > 3
			AudioDevice->UnregisterSubmixBufferListener(ExistingListener->ToSharedRef(), *ExistingListener->ToSharedRef()->GetSubmix());
#else
			AudioDevice->UnregisterSubmixBufferListener(ExistingListener->Get());
#endif
		}
		ControllerListeners.Remove(DeviceId);

		using namespace SonyGamepadProxyHelpers;
		auto* Gamepad = GetGamepad(DeviceId);
		if (!Gamepad)
		{
			return;
		}

		auto* Ctx = Gamepad->GetMutableDeviceContext();
		if (!Ctx)
		{
			return;
		}

		if (Ctx->ConnectionType == EDSDeviceConnection::Usb)
		{
			IAudioDevice::Get().UnregisterAudioDevice(Ctx->Path);
		}
	}
}
