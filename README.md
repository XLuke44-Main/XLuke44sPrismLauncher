# XLuke44's PrismLauncher

This is my own fork of PrismLauncher 8.4. I originally made it for personal use, but decided to share it with everyone.

The goal of this fork is to make prismLauncher lighter, reduce unnecessary overhead, and introduce various quality-of-life improvements. 
I also want to keep things as local as possible. Translations work entirely offline, so there's no need for an internet connection. All instances also start offline by default.

Microsoft account login is optional, so you can use the launcher without signing in. I've also made my own default theme, while keeping the original prismLauncher themes available.

This is a personal project, so development happens at my own pace and according to my own priorities.

Anyway, enjoy everyone ^w^ Have a great day/night

## Release Formats

There are two release types:

- **Portable:** Extract the release and run it. Launcher data is stored alongside the application. The Linux portable archive is built using `scripts/build-linux-portable.bash`.
- **Non-portable:** Uses the standard installation layout for the platform.

I currently only support linux, but windows support is planned for sometime in the future.

I don't maintain Flatpak or distro-specific packages.

## Online Mode Warning

The first launch of an instance with minecraft version or modloader version that hasn't been installed before will start in online mode.

## API Credentials

This fork currently uses API credentials provided by PrismLauncher. I know upstream recommends that forks use their own credentials or disable the related functionality.

For now, that includes:

- **CurseForge:** The default API key provided by PrismLauncher. I haven't obtained a separate key for this project yet.
- **Microsoft Authentication:** PrismLauncher's client ID. I haven't been able to register and configure my own application, so I'm continuing to use the existing one.

I'll replace these credentials with ones registered specifically for this project when possible. Until then, the existing credentials allow these features to remain available without requiring users to configure their own API keys or client IDs.

## Legal Disclaimer

This is an **unofficial** fork of https://github.com/PrismLauncher/PrismLauncher, based on version 8.4. It is independently maintained and is **not** affiliated with or endorsed by the original PrismLauncher project or its contributors.

All original copyright notices, attributions, and third-party licence notices have been retained.

My original modifications are licensed under the **BSD 3-Clause License**, as specified in `LICENSE.txt`. This applies to my own contributions and does not change the licences of the original PrismLauncher code or third-party components.

All original licences and notices from PrismLauncher and its third-party components remain applicable to this fork. Each component retains its respective original licence, and all applicable licence terms must be respected when using, modifying, or redistributing this project. The BSD 3-Clause License mentioned above applies only to my original contributions and does not replace or alter any of the original licences.

I don't claim ownership of the original PrismLauncher project or its code. All credit for the original work belongs to its authors and contributors.
