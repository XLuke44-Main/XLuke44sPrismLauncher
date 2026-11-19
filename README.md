# XLuke44's PrismLauncher

This is my own fork of PrismLauncher 8.4. I originally made it for personal use, but decided to share it with everyone.

The goal of this fork is to make prismLauncher lighter, reduce unnecessary overhead, and introduce various quality-of-life improvements. 
I also want to keep things as local as possible. Translations work entirely offline, so there's no need for an internet connection. All instances also start offline by default.

Microsoft account login is optional, so you can use the launcher without signing in. I've also made my own default theme, while keeping the original prismLauncher themes available.

This is a personal project, so development happens at my own pace and according to my own priorities.

Anyway, enjoy everyone ^w^ Have a great day/night

## Online Mode Warning

The first launch of an instance with minecraft version or modloader version that hasn't been installed before will start in online mode.

## **Release Formats**

This Fork supports Windows and Linux only. There are currently no plans to support other operating systems.

Releases are available in two formats:

**Portable:** Extract the archive and run the application directly. No installation is required.

    - Linux portable releases are built using scripts/build-linux-portable.bash.
    - Windows portable releases are built using scripts/build-windows-portable.bash.

**Installable:** 

    - Uses the platform's standard installation layout.

## APIs

This fork currently relies on Prism Launcher's Microsoft Application ID, as I have been unable to obtain one of my own. It will remain in use for the foreseeable future until I can secure a reliable replacement.

Starting with version 1.0.5.0, this fork will use its own CurseForge Core API key. 

Imgur support has been completely removed, along with all related code and API key configuration.

## Legal Disclaimer

This is an **independently maintained** fork of Prism Launcher, based on version 8.4. The Original Prism Launcher repository is https://github.com/PrismLauncher/PrismLauncher . It is **not** affiliated with or endorsed by the original PrismLauncher project or its contributors.

All original copyright notices, attributions, and third-party licence notices have been retained.

My original modifications are licensed under the **BSD 3-Clause License**, as specified in `LICENSE.txt`. This applies to my own contributions and does not change the licences of the original PrismLauncher code or third-party components.

All original licences and notices from PrismLauncher and its third-party components remain applicable to this fork. Each component retains its respective original licence, and all applicable licence terms must be respected when using, modifying, or redistributing this project. The BSD 3-Clause License mentioned above applies only to my original contributions and does not replace or alter any of the original licences.

I don't claim ownership of the original PrismLauncher project or its code. All credit for the original work belongs to its authors and contributors.
