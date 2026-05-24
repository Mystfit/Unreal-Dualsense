<h1 align="center">Middleware Unreal-Dualsense</h1>

<p align="center">
Integrate all the features of Sony's DualSense™ and DualShock 4® controllers into your Unreal Engine project.
<br />
<br />
<a href="https://github.com/rafaelvaloto/Unreal-Dualsense/issues">Report Bug</a>
·
<a href="https://github.com/rafaelvaloto/Unreal-Dualsense/issues">Suggest a Feature
</a>
·
<a href="https://github.com/rafaelvaloto/Unreal-Dualsense/wiki">Documentation
</a>
</p>

<p align="center">
<a href="https://github.com/rafaelvaloto/Unreal-Dualsense/releases">
    <img src="https://img.shields.io/github/v/release/rafaelvaloto/Unreal-Dualsense?style=for-the-badge&logo=github" alt="Latest Release">
</a>
<a href="https://github.com/rafaelvaloto/Unreal-Dualsense/blob/main/LICENSE">
    <img src="https://img.shields.io/github/license/rafaelvaloto/Unreal-Dualsense?style=for-the-badge" alt="License">
</a>
<img src="https://img.shields.io/badge/Unreal%20Engine-5.2+-blue?style=for-the-badge&logo=unrealengine" alt="Unreal Engine 5.x">
<br/>
<img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Platform: Windows">
<img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="Platform: Linux">
<img src="https://img.shields.io/badge/macOS-000000?style=for-the-badge&logo=apple&logoColor=white" alt="Platform: macOS">
<img src="https://img.shields.io/badge/PlayStation-0070D1?style=for-the-badge&logo=playstation&logoColor=white" alt="Platform: PlayStation">
</p>

<p align="center">
  <a href="#-getting-started">Getting Started</a> •
  <a href="#-basic-usage">Basic Usage</a> •
  <a href="#-example-project-arena-shooter-ue-56">Example Project</a>
  <br>
</p>

---

## 📖 About the Project

Built with a cross-platform architecture, this Unreal Engine Middleware provides a unified solution for integrating DualSense™ (PlayStation 5) and DualShock 4® (PlayStation 4) controllers. It delivers native support on PC while being architected for easy compilation across other Unreal Engine platforms, including PlayStation®. This asset provides direct API access to the complete feature set of each controller, including the revolutionary Haptic Feedback and Adaptive Triggers of the DualSense™. All features are exposed through a clean and well-documented function library for both Blueprint and C++.

Designed to bridge the gap left by generic controller support, this asset empowers developers to implement the high-fidelity, immersive feedback that makes Sony's controllers unique.

## ✨ Features
* 🏗️ **Extensible Multi-Platform Architecture**: Its flexible design enables compilation across all Unreal Engine platforms. Supporting new hardware is as simple as implementing the connection interface.
* 🔌 **Dynamic Connection (Hot-Swap)**: Automatically detects controller connection and disconnection, even during gameplay.
* ⚡ **Optimized for Multiplayer**: High-performance architecture with minimal impact on network games.
* 🎮 **Seamless Input Integration**: Coexists perfectly with Unreal Engine's native input managers (like Enhanced Input) and other gamepad Middlewares, preventing conflicts between devices.
* 🎧 **Audio Haptics (USB & Wireless)**: Haptic feedback based on in-game audio.
* 🎯 **Adaptive Triggers**: Full control over resistance, effect, and vibration on R2/L2 triggers.
* 💡 **Lightbar Control**: Dynamically change the controller's LED color.
* 🎤 **Smart Mute Detection** — Automatic mute LED control, no coding required
* ⚙️ **Force Feedback**: Native integration with Unreal Engine's Force Feedback system for standard motor vibration.
* 🎮 **Multi-Controller Support**: Manage up to 4 controllers simultaneously.

## 🚀 Getting Started

### Quick Installation

Go to the official page on the Unreal Engine Marketplace (FAB): **[Middleware Gold Edition - FAB](https://www.fab.com/listings/d0836635-91a8-42cb-a78f-d34456cc042c)**

### 🔒 V2 Gold Version Access (Exclusive for Buyers)

If you have purchased the Gold version, your purchase verification grants you full access to the private GitHub repository, which includes:

* **Full & Detailed Documentation:** Comprehensive guides to get you started quickly.
* **Practical Examples:** Hands-on integration samples for features like high-fidelity audio and audio-based haptics (vibration) over both Bluetooth and USB.

### ⚠️ Important: How to Request Access

Click the link below to provide your GitHub username and Order ID:
👉 **[Fill out the Access Form](https://forms.gle/bis7RqWbweis1UyZA)**

* **Already bought the Gold Edition?** Just fill out the form with your Gold Order ID.
* **Upgrading from Controller Support v2?** If you purchased the v2 version on FAB up until **May 16th, 2026**, you are eligible for a free migration. Simply fill out the form using your v2 Order ID.

*Access is usually granted within 24 hours. You will receive a GitHub invitation via email or directly in your platform notifications.*

### Manual Installation

To ensure the Middleware compiles correctly within Unreal Engine, you must configure the [GamepadCore](https://github.com/rafaelvaloto/GamepadCore_) submodule.

Please run the following commands in your terminal (Git Bash, PowerShell, or CMD):
```bash
# Clone the repository at version (tag)
git clone -b [tag] --single-branch https://github.com/rafaelvaloto/Unreal-Dualsense.git
```
```bash
# Clone the repository at master branch
git clone https://github.com/rafaelvaloto/Unreal-Dualsense.git

# Enter the repository folder
cd Unreal-Dualsense

# Init the submodule to the latest version
git submodule update --init

```
    
## 💻 Basic Usage

The Middleware exposes all functionality through static Blueprint function libraries, meaning you can call methods from anywhere without needing to add components.

### Blueprint Function Libraries

The functions are divided into two main categories for easy access:

* **Sony Gamepad**: Contains management methods common to Sony controllers (DualShock and DualSense), such as LED control, gyroscope, battery level, etc.
* **DualSense Effects**: Contains methods specific to DualSense exclusive features, such as Adaptive Triggers configuration.

Call functions directly to control DualSense features. Some available effects include:

### 📚 For the full documentation, please see the **[Wiki](https://github.com/rafaelvaloto/WindowsDualsenseUnreal/wiki)**.


## 🎮 Example Project: Arena Shooter UE 5.6

To demonstrate the practical use of the **Dualsense Unreal Middleware**, a sample project has been developed using the [*Arena Shooter* template](https://www.unrealengine.com/marketplace/en-US/product/arena-shooter-template) and upgraded to Unreal Engine 5.6. This project integrates key features of the DualSense controller to enhance the player's experience.

🎬 **Gyroscope Demo:** Watch the new aiming mechanics in action on **[YouTube](https://youtu.be/r_NhoODeELU?si=IdqRFx8_aimeIToD)**.
  
![Arena Shooter UE 5.6](Images/ArenaBanner.jpg)

### Implemented Features

In this sample, the following DualSense functionalities were integrated to provide a more immersive gameplay experience:

* 🔫 **Automatic Gun**: Experience fully automatic firing with appropriate haptic feedback, simulating realistic weapon recoil and vibration.
* 🎯 **Semi-Automatic Gun**: Engage in precise shooting with semi-automatic weapons, where each shot provides distinct haptic sensations and trigger resistance.
* 💥 **Vibration on Player Hit & Visual Feedback**: Feel the impact! The controller vibrates dynamically when the player takes damage, complemented by on-screen visual feedback for enhanced immersion and immediate awareness.
* 💡 **LED Color Change on Player Hit**: The DualSense controller's LED light dynamically changes color when the player is hit, offering an immediate and intuitive visual cue of damage taken, enhancing situational awareness.

### Where to Download

You can download the *Arena Shooter UE 5.6* with the DualSense integration directly from link.

- [**Download the example project v2.0.0 for the editor here**](https://drive.google.com/file/d/11iUQuWwA4zkFI_eP0roYbTDh0ss8614m/view?usp=drive_link)

### 🎓 Hands-On Tutorial

We've created a detailed, step-by-step tutorial that breaks down the entire implementation within the example project. It's the perfect guide to get you started.

* 🎯 **Gyroscope Aiming**: A complete tutorial on how to implement a precise and responsive Aim Down Sights (ADS) gyro aiming system using the Arena Shooter template.
    ➡️ **[[Read the Gyroscope Aiming Tutorial]](https://github.com/rafaelvaloto/WindowsDualsenseUnreal/wiki/Example-Project:-Gyroscope-Aiming)**

* 🔫 **Arena Shooter**: An example using the Arena Shooter template that implements adaptive triggers for automatic/semi-automatic weapons and haptic feedback for player damage.
    ➡️ **[[Read the Arena Shooter Tutorial]](https://github.com/rafaelvaloto/WindowsDualsenseUnreal/wiki/Example-Project:-Arena-Shooter-Tutorial)**


## 🎮 Example Project: Audio and Audio Haptics (USB & Wireless)

Take your immersion to the next level! This update enhances the advanced Audio Haptics feature, allowing it to work seamlessly via both USB and wirelessly via Bluetooth (previously USB-only). This new example project demonstrates how to harness this power, featuring a complete implementation for real-time haptic feedback based on in-game audio.

## [🎥 Watch the video on YouTube](https://youtu.be/JJFxJJESFUo?si=lhIcLWosU6UYANz9)
- [**Download the example project compiled "Win64-Shipping" here.**](https://github.com/rafaelvaloto/Unreal-Dualsense-Gold-Edition)

![Audio Haptics (USB & Wireless)](https://github.com/user-attachments/assets/7ceea1f5-4955-40e6-9dda-3fa9e3ab9808)



## 💉 Injecting Custom Device Logic (Custom DeviceManager)
Since version 2.0.0, you can also inject a custom implementation of the `DeviceManager`. This is useful if you want to implement your own input buffering, custom button mapping, or specialized haptic logic without modifying the Middleware source.

> [!TIP]
> To ensure that your custom implementation works with native Unreal Engine assets (like **Haptic Feedback Effects**, **Force Feedback Assets**, and **Device Properties**), your class must correctly implement or override the methods from `IInputDevice` and `IHapticDevice`.

### Required Interfaces for Native Assets

If you want your custom manager to support native Unreal features, ensure it implements/overrides:

*   **`IHapticDevice`**: haptic assets.
    *   `SetHapticFeedbackValues`: Processes frequency and amplitude values from assets.
    *   `GetHapticFrequencyRange`: Determines the valid frequency range supported by the device.
    *   `GetHapticAmplitudeScale`: Returns the scaling factor for amplitude mapping.
*   **`IInputDevice`**: Required for standard vibration, light color, and properties.
    *   `SetChannelValues` / `SetChannelValue`: Essential for `UForceFeedbackEffect` assets.
    *   `SetLightColor` / `ResetLightColor`: Controls the controller's LED.
    *   `SetDeviceProperty`: Handles `UInputDeviceProperty` (e.g., Adaptive Triggers via Unreal 5.1+ system).
    *   `GetHapticDevice`: Returns the `IHapticDevice*` interface (usually `return this;`).
    *   `IsGamepadAttached`: Returns whether the device is currently connected.

## 🤝 How to Contribute

Contributions are welcome! If you have ideas, suggestions, or bug fixes, feel free to open an *Issue* or submit a *Pull Request*.

## ⭐ Special Thanks

A special thanks to the following contributors who help maintain and improve this repository:

* **[Marat Radchenko (slonopotamus)](https://github.com/slonopotamus)** - For his invaluable help and dedication to maintaining this repository.
* **[yncat](https://github.com/yncat)**: For the extensive research and implementation logic regarding **USB Audio Haptics**, which was crucial for supporting high-fidelity haptics via USB ([Issue #105](https://github.com/rafaelvaloto/Unreal-Dualsense/issues/105)).


## ⭐ Credits and Acknowledgments

The foundation of this Middleware was built upon the research and code from several amazing projects in the community:

* [DualSense on Windows API](https://github.com/Ohjurot/DualSense-Windows) - Initial DS5 implementation logic.
* [SAxense](https://github.com/egormanga/SAxense) - Base for Bluetooth Audio Haptics.
* [Awalol/DS5Dongle](https://github.com/Awalol/DS5Dongle) - Reference Bluetooth Audio (Headset/Speaker) opus codec and buffer sizes.
* [Ryochan7/DS4Windows](https://github.com/Ryochan7/DS4Windows) - Industry standard for DualShock/DualSense on Windows.
* [linux/drivers/hid/hid-playstation.c](https://github.com/torvalds/linux/blob/master/drivers/hid/hid-playstation.c#L1709) - Reference for calibration, gyroscope, and Linux driver standards.

A special thanks to the Unreal Engine team for providing the Arena Shooter templates, which served as an excellent foundation for the example project demonstrating this Middleware's features.

---

## ⚖️ Disclaimer and Trademarks

This software is an independent and unofficial project. It is **not** affiliated, associated, authorized, endorsed by, or in any way officially connected with Sony Interactive Entertainment Inc., Microsoft Corporation, Apple Inc., Epic Games, Unity Technologies, the Godot Engine project, or the Open 3D Foundation.

**Trademarks belong to their respective owners:**

* **Sony:** "PlayStation", "PlayStation Family Mark", "PS5 logo", "PS5", "DualSense", and "DualShock" are registered trademarks or trademarks of Sony Interactive Entertainment Inc. "SONY" is a registered trademark of Sony Corporation.
* **Microsoft:** "Windows" and "Xbox" are registered trademarks of Microsoft Corporation.
* **Apple:** "Mac" and "macOS" are registered trademarks of Apple Inc.
* **Linux:** "Linux" is the registered trademark of Linus Torvalds in the U.S. and other countries.
* **Epic Games:** "Unreal" and "Unreal Engine" are trademarks or registered trademarks of Epic Games, Inc. in the United States of America and elsewhere.
* **Unity:** "Unity", Unity logos, and other Unity trademarks are trademarks or registered trademarks of Unity Technologies or its affiliates in the U.S. and elsewhere.
* **Godot:** "Godot" and the Godot logo are trademarks of the Godot Engine project.
* **O3DE:** "O3DE" and the O3DE logo are trademarks of the Open 3D Foundation.
