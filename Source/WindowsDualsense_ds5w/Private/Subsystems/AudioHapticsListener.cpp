// Copyright (c) 2026 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026
// Acknowledgement: USB Audio Haptics logic based on research and code shared by yncat (https://github.com/yncat)
// in Issue #105: https://github.com/rafaelvaloto/Unreal-Dualsense/issues/105

#include "Subsystems/AudioHapticsListener.h"
#include "AudioResampler.h"
#include "GCore/Interfaces/Segregations/IGamepadHaptics.h"

FAudioHapticsListener::FAudioHapticsListener(int32 InDeviceId, USoundSubmix* InSubmix, bool IsWireless, float Vol, float LowPassUsb, float LowPassWireless)
    : Submix(InSubmix)
    , DeviceId(InDeviceId)
    , bIsWireless(IsWireless)
    , VolumeMultiplier(Vol)
    , kLowPassAlphaUSB(LowPassUsb)
    , kLowPassAlphaWireless(LowPassWireless)
{
	AudioPacketQueue.Empty();
	AudioPacketQueueUSB.Empty();
}

void FAudioHapticsListener::OnNewSubmixBuffer(const USoundSubmix* OwningSubmix, float* AudioData, int32 NumSamples,
                                              int32 NumChannels, const int32 SampleRate, double AudioClock)
{
	if (Submix != OwningSubmix || !Submix)
	{
		return;
	}

	if (!bIsWireless) // USB
	{
		if (NumSamples <= 0)
		{
			return;
		}

		if (!AudioData)
		{
			return;
		}

		int inFrames = NumSamples / NumChannels;
		std::vector<float> AudioDataResampled;
		AudioDataResampled.reserve(NumSamples);
		for (int32 OutFrame = 0; OutFrame < inFrames; OutFrame++)
		{
			int32 LeftIdx = OutFrame * NumChannels;
			int32 RightIdx = LeftIdx + 1;

			if (LeftIdx >= NumSamples || RightIdx >= NumSamples)
			{
				break;
			}

			float AudioLeft = AudioData[LeftIdx];
			float AudioRight = AudioData[RightIdx];

			AudioLeft *= VolumeMultiplier;
			AudioRight *= VolumeMultiplier;

			// Low-pass filter
			LowPassState_Left = (1.0f - kLowPassAlphaUSB) * AudioLeft + kLowPassAlphaUSB * LowPassState_Left;
			LowPassState_Right = (1.0f - kLowPassAlphaUSB) * AudioRight + kLowPassAlphaUSB * LowPassState_Right;

			// High-pass effect
			float LeftHaptic = AudioLeft - LowPassState_Left;
			float RightHaptic = AudioRight - LowPassState_Right;

			AudioLeft = FMath::Clamp(AudioLeft, -1.0f, 1.0f);
			AudioRight = FMath::Clamp(AudioRight, -1.0f, 1.0f);
			LeftHaptic = FMath::Clamp(LeftHaptic, -1.0f, 1.0f);
			RightHaptic = FMath::Clamp(RightHaptic, -1.0f, 1.0f);

			AudioDataResampled.push_back(AudioLeft);
			AudioDataResampled.push_back(AudioRight);
			AudioDataResampled.push_back(LeftHaptic);
			AudioDataResampled.push_back(RightHaptic);
		}

		if (AudioDataResampled.size() > 0)
		{
			AudioPacketQueueUSB.Enqueue(AudioDataResampled);
			AudioDataResampled.clear();
		}

		return;
	}

	BTPacket btPack1;
	BTPacket btPack2;

	const int32 RatioHaptics = SampleRate / 3000; // 48000 / 3000 = 16
	const int32 TargetSamples = NumSamples / RatioHaptics;
	std::vector<int8_t> ResampledDataL;
	std::vector<int8_t> ResampledDataR;
	ResampledDataL.reserve(TargetSamples / 2);
	ResampledDataR.reserve(TargetSamples / 2);

	for (int32 i = 0; i < TargetSamples / 2; i++)
	{
		// Pula 16 samples de cada vez (assumindo áudio interleaved L/R/L/R)
		const int32 SourceIndex = i * RatioHaptics * 2;

		if (SourceIndex + 1 >= NumSamples)
		{
			break;
		}

		float InLeft = AudioData[SourceIndex];
		float InRight = AudioData[SourceIndex + 1];

		// Aplica o filtro (Seu High-pass/Low-pass)
		LowPassState_Left = (1.0f - kLowPassAlphaWireless) * InLeft + kLowPassAlphaWireless * LowPassState_Left;
		LowPassState_Right = (1.0f - kLowPassAlphaWireless) * InRight + kLowPassAlphaWireless * LowPassState_Right;

		float OutLeft = InLeft - LowPassState_Left;
		float OutRight = InRight - LowPassState_Right;

		// Converte direto para int8 (-128 a 127)
		ResampledDataL.push_back(static_cast<int8_t>(FMath::Clamp(OutLeft * 127.0f, -128.0f, 127.0f)));
		ResampledDataR.push_back(static_cast<int8_t>(FMath::Clamp(OutRight * 127.0f, -128.0f, 127.0f)));
	}

	// 2. Montagem dos Pacotes (Exemplo para pacotes de 64 bytes)
	std::vector<uint8_t> hapticsPacket1(64, 0);
	std::vector<uint8_t> hapticsPacket2(64, 0);

	for (int32 i = 0; i < 32; i++)
	{
		if (i < ResampledDataL.size())
		{
			hapticsPacket1[i * 2] = ResampledDataL[i];
			hapticsPacket1[i * 2 + 1] = ResampledDataR[i];
		}

		int32 secondHalfIndex = i + 32;
		if (secondHalfIndex < ResampledDataL.size())
		{
			hapticsPacket2[i * 2] = ResampledDataL[secondHalfIndex];
			hapticsPacket2[i * 2 + 1] = ResampledDataR[secondHalfIndex];
		}
	}

	const int32 InFrames = 1024;
	const int32 OutFrames = 960;
	const float Ratio = static_cast<float>(InFrames) / static_cast<float>(OutFrames);

	std::vector<float> AudioDataResampled;
	AudioDataResampled.reserve(OutFrames * NumChannels);
	for (int32 i = 0; i < OutFrames; i++)
	{
		float SourceIndex = i * Ratio;
		int32 IndexLow = (int32)SourceIndex;
		int32 IndexHigh = IndexLow + 1;

		if (IndexHigh >= InFrames)
		{
			IndexHigh = IndexLow;
		}

		float Fraction = SourceIndex - IndexLow;
		for (int32 Channel = 0; Channel < NumChannels; Channel++)
		{
			float SampleLow = AudioData[IndexLow * NumChannels + Channel];
			float SampleHigh = AudioData[IndexHigh * NumChannels + Channel];

			float InterpolatedSample = SampleLow + Fraction * (SampleHigh - SampleLow);
			InterpolatedSample *= VolumeMultiplier;
			AudioDataResampled.push_back(InterpolatedSample);
		}
	}

	for (int32 i = 0; i < 2; i++)
	{
		std::vector<uint8_t> hapticsData(64, 0);

		int32 HalfSize = AudioDataResampled.size() / 2;
		if (i > 0)
		{
			std::vector<float> SecondPacket(AudioDataResampled.begin() + HalfSize, AudioDataResampled.end());
			btPack2.haptics = hapticsPacket2;
			AudioPacketQueue.Enqueue(btPack2);
			return;
		}

		std::vector<float> FirstPacket(AudioDataResampled.begin(), AudioDataResampled.begin() + HalfSize);
		btPack1.haptics = hapticsPacket1;
		AudioPacketQueue.Enqueue(btPack1);
	}
}

void FAudioHapticsListener::ConsumeHapticsQueue(IGamepadHaptics* AudioHaptics)
{

	if (AudioHaptics && bIsWireless)
	{
		BTPacket btPack;
		while (AudioPacketQueue.Dequeue(btPack))
		{
			if (btPack.haptics.empty())
			{
				break;
			}
			AudioHaptics->AudioHapticUpdate(btPack.haptics);
		}
		AudioPacketQueue.Empty();
	}
	else if (AudioHaptics && !bIsWireless)
	{
		std::vector<float> QSampleQuad;
		QSampleQuad.reserve(1024 * 2);
		while (AudioPacketQueueUSB.Dequeue(QSampleQuad))
		{
			if (QSampleQuad.empty())
			{
				break;
			}

			if (QSampleQuad.size() >= 2048) // 40ms de áudio a 48kHz stereo
			{
				AudioHaptics->AudioHapticUpdate(QSampleQuad);
				QSampleQuad.clear();
			}
		}
		AudioPacketQueueUSB.Empty();
	}
}
