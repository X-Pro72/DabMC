<p align="left">
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="/program_info/DabMC_logo_dark.png">
  <source media="(prefers-color-scheme: light)" srcset="/program_info/DabMC_logo_light.png">
  <img alt="Prism Launcher" src="/program_info/DabMC_logo_light.png" width="50%">
</picture>
</p>


DabMC is a custom launcher for Minecraft that allows you to easily manage multiple installations of Minecraft at once as well as yarg.

This is a **fork** of the Prism Launcher and is not endorsed by Prism Launcher.
If you want to read about why this fork was created, check out https://www.minecraft.net/en-us/article/addressing-player-chat-reporting-tool.


## DISCLAIMER! 

This fork is NOT Prism Launcher and is not endorsed by or affiliated with the Prism Launcher project (https://prismlauncher.org/).
Do not complain to Prism Launcher about any issues you encounter while using this fork. Don't complain to me either.


## Development

I don't want help.


## Building

If you want to build DabMC yourself, check the [Build Instructions](https://prismlauncher.org/wiki/development/build-instructions/).
Just clone this repo instead of Prism Launcher's


## Forking/Redistributing/Custom builds policy

We don't care what you do with your fork/custom build as long as you follow the terms of the [license](LICENSE) (this is a legal responsibility), and if you made code changes rather than just packaging a custom build, please do the following as a basic courtesy:

- Make it clear that your fork is not PrismLauncher and is not endorsed by or affiliated with the PrismLauncher project (<https://prismlauncher.org>).
- Go through [CMakeLists.txt](CMakeLists.txt) and change PrismLauncher's API keys to your own or set them to empty strings (`""`) to disable them (this way the program will still compile but the functionality requiring those keys will be disabled).

If you have any questions or want any clarification on the above conditions please make an issue and ask us.

Be aware that if you build this software without removing the provided API keys in [CMakeLists.txt](CMakeLists.txt) you are accepting the following terms and conditions:

- [Microsoft Identity Platform Terms of Use](https://docs.microsoft.com/en-us/legal/microsoft-identity-platform/terms-of-use)
- [CurseForge 3rd Party API Terms and Conditions](https://support.curseforge.com/en/support/solutions/articles/9000207405-curse-forge-3rd-party-api-terms-and-conditions)

If you do not agree with these terms and conditions, then remove the associated API keys from the [CMakeLists.txt](CMakeLists.txt) file by setting them to an empty string (`""`).

## Sponsors & Partners

Destroy All Bacteria (The DAB Server)

God (The good one)