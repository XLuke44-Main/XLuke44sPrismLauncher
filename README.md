# XLuke44's PrismLauncher

This project is my own fork of PrismLauncher 8.4. I originally made it for personal use, but decided to make it publicly accessible as well.

Anyway, enjoy everyone ^w^ Have a great day/night!

## Release formats

This fork maintains two release types:

- **Portable:** extract the release and run it without a system-wide installation. Portable mode stores launcher data alongside the application. The Linux portable archive is built with `scripts/build-linux-portable.bash`.
- **Non-portable:** use the platform's standard installation layout. CMake provides the Linux install layout and macOS application bundle, and the Windows installer definition remains in `program_info/win_install.nsi.in`.

No flatpak, distro-specific package formats are maintained by me.

## Online Mode Warning

**Warning:** The first launch of any instance with a modloader version or minecraft version that has not been installed previously will start in online mode.

## API Credentials and Third-Party Services

This fork currently uses certain API credentials provided by Prism Launcher. I am aware that the upstream project asks forks to replace these credentials with their own or disable the associated functionality.

The current arrangements are as follows:

### CurseForge API

This fork currently uses the default CurseForge API key provided by Prism Launcher.

I understand that this key was issued specifically for Prism Launcher and is not intended for use in derivative projects. However, until I am able to obtain a separate API key for this fork, I intend to continue using the existing one.

Replacing it with a key registered specifically for this project remains a priority, and I intend to make the switch as soon as I am able to do so.

### Microsoft Authentication

This fork currently uses Prism Launcher's Microsoft authentication client ID.

Unfortunately, I have been unable to successfully register and generate my own Microsoft authentication client ID. As a result, I have no alternative but to continue using the client ID provided by Prism Launcher.

I intend to retain the existing client ID for the foreseeable future, until further notice, as replacing it is currently not an option for me. This also ensures that Microsoft account authentication remains available and convenient for users without requiring them to provide or configure their own client ID.

I acknowledge the upstream project's guidance regarding the use of its API credentials and appreciate understanding of circumstances.

## Legal Disclaimer

This project is an unofficial fork of https://github.com/PrismLauncher/PrismLauncher, based on version 8.4. It is independently maintained and is not affiliated with, endorsed by, or officially associated with the Prism Launcher project or its contributors.

All original copyright notices, attributions, and applicable third-party licence notices from the upstream project have been retained.

Original modifications introduced by this fork are my own work and are licensed under the **BSD 3-Clause License**, as specified in `LICENSE.txt`. This applies to my original contributions and does not replace or alter the licences governing the original Prism Launcher code or third-party components.

The original Prism Launcher code is licensed under the **GNU General Public License v3.0 only (GPL-3.0-only)**. Redistribution of this project, including modified versions and derivative builds, must comply with the applicable licensing requirements, including those of the GPL-3.0-only licence for the original Prism Launcher code.

I do not claim authorship or ownership of the original Prism Launcher project or its underlying code. All credit for the original work belongs to its respective authors and contributors.
