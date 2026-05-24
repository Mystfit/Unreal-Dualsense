// Copyright (c) 2026 Rafael Valoto. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2026

#pragma once

#include "GCore/Types/Structs/Context/DeviceContext.h"
#include <memory>
#include <vector>

class FLinuxDeviceInfo
{
	/**
	 * Virtual destructor for the FLinuxDeviceInfo class.
	 *
	 * This destructor ensures proper cleanup of resources associated with
	 * the FLinuxDeviceInfo class when the object is destroyed. It overrides
	 * the destructor of the IPlatformHardwareInfoInterface base class.
	 *
	 * The destructor is declared as virtual to allow correct destruction
	 * of derived class objects through base class pointers.
	 */
public:
	~FLinuxDeviceInfo()
	{
	}

	/**
	 * Processes audio haptic feedback using the given device context.
	 *
	 * This method is responsible for handling and generating audio-based
	 * haptic feedback through the provided device context. It leverages
	 * the context to access or manage the required resources for delivering
	 * the haptic feedback.
	 *
	 * @param Context A pointer to the FDeviceContext object, which provides
	 *                the necessary interface and data for processing audio
	 *                haptic feedback. Must be a valid, properly initialized
	 *                pointer.
	 */
	static void ProcessAudioHaptic(FDeviceContext* Context);
	/**
	 * Configures the features of the device using the specified device context.
	 *
	 * This static method is responsible for enabling or adjusting the features
	 * of the device associated with the provided context. It leverages the context
	 * to access and modify the underlying device's capabilities as needed.
	 *
	 * @param Context A pointer to the FDeviceContext object that represents the
	 *                context of the device being configured. Must be a valid,
	 *                non-null pointer.
	 */
	static void ConfigureFeatures(FDeviceContext* Context);
	/**
	 * Reads device information using the provided device context.
	 *
	 * This method retrieves and processes the relevant information about
	 * the device through the given device context. It is a member function
	 * of the FLinuxDeviceInfo class and is intended to handle device-specific
	 * data retrieval operations.
	 *
	 * @param Context A pointer to the FDeviceContext object, which provides
	 *        the necessary context and interface for accessing device-related
	 *        information. It must be valid and properly initialized.
	 */
	static void Read(FDeviceContext* Context);
	/**
	 * Writes device-specific information to the provided device context.
	 *
	 * This method is responsible for processing and transferring relevant
	 * device information to the supplied context. It is expected to interact
	 * with the context to store or manipulate data as needed for device operations.
	 *
	 * @param Context Pointer to the FDeviceContext object that serves as the
	 *                target for the device information. Must be a valid,
	 *                non-null pointer.
	 */
	static void Write(FDeviceContext* Context);
	/**
	 * Detects and populates a list of device contexts.
	 *
	 * This method scans for available devices and collects their information,
	 * storing it in the provided array of FDeviceContext objects.
	 *
	 * @param Devices A reference to an array of FDeviceContext objects
	 *                that will be populated with the detected device information.
	 */
	static void Detect(std::vector<FDeviceContext>& Devices);
	/**
	 * Creates a handle for the FLinuxDeviceInfo class using the provided device context.
	 *
	 * This method initializes and configures a handle specific to the device context
	 * supplied as an argument. It is expected to ensure that the device context is
	 * used properly in the creation of the handle.
	 *
	 * @param Context A pointer to the FDeviceContext instance used for handle creation.
	 * @return A boolean value indicating whether the handle was successfully created.
	 */
	static bool CreateHandle(FDeviceContext* Context);
	/**
	 * Invalidates the device handle associated with the specified device context.
	 *
	 * This method clears the association of the handle linked with the provided
	 * FDeviceContext, ensuring that it is no longer valid. It is typically used
	 * during device cleanup or reinitialization processes.
	 *
	 * @param Context A pointer to the FDeviceContext instance whose handle is to
	 *                be invalidated. This parameter must not be null.
	 */
	static void InvalidateHandle(FDeviceContext* Context);

	/**
	 * @brief Initializes the audio device associated with the given device context.
	 *
	 * This method is responsible for setting up and configuring the audio device
	 * based on the provided device context and device identifier.
	 *
	 * @param Context Pointer to the device context that holds relevant device information.
	 */
	static void InitializeAudioDevice(FDeviceContext* Context);
};
