# Breath of Fire 3 Extender

<br>

## What is this?

BoF3Ext is a replacement ddraw.dll for the Chinese PC port of Breath of Fire III. Its main goals are to fix bugs, translate the game into English, and make the PC port run on modern hardware and operating systems.

## What does it do?

As of the time of writing, BoF3Ext translates the majority of the game into English and fixes numerous bugs (like missing sprites in some menus). It also allows basic texture replacement, which is currently being used to load HD textures for the main title screen, and replaces the default bitmap font with an external TrueType font that is rendered at 4x the original game's resolution. There is also partial widescreen support, but that is still very much a work-in-progress.

## What is planned for the future?

There are still a few places where the Chinese text has not been translated yet. Most noticeably, the popup bubbles that display location names in the overworld. Widescreen support is also yet to be finished, and there are a few visual bugs still to be squashed. Besides those, BoF3 still currently has some issues with certain rendering resolutions, and fullscreen mode is completely borked and will default to 640x480. Finally, after all bugs are fixed, all text is translated, and widescreen support is finished, there are plans to implement a modding API.

## Where can I download it?

You can either compile BoF3Ext yourself using Visual Studio 2022, or find pre-built releases on the Moogles & Mods Discord server.

## How do I install it?

If you are using a pre-built release of BoF3Ext, just extract the .7z archive directly into the folder where Breath of Fire III is installed.

If you choose to compile BoF3Ext yourself, you will need to copy over the compiled `ddraw.dll`, `bof3ext.dll`, `bof3ext.toml`, and `loader.cfg` files to your Breath of Fire III installation folder. You should then download the resource files that contain all of the translated text from the [BoF3Ext_Resources repo](https://github.com/TheRealBiggs/bof3ext_resources) and place them in a folder titled `/NewData/` in your BoF3 installation folder.

## How do I uninstall it?

Delete `ddraw.dll` from your Breath of Fire III installation folder, or remove the `bof3ext.dll` entry in `loader.cfg`.

## What are all these files?

* `ddraw.dll` is a proxy DLL for the original DirectDraw library that is part of Microsoft's legacy DirectX runtime. By using a local DLL file with the same name, we can force bof3.exe to load our code, allowing us to inject hooks into the game. These hooks change how the code in the game works so that we can do things like loading text from external files. `ddraw.dll` itself does not apply any hooks and is simply a loader for other DLLs.
* `bof3ext.dll` is the main part of BoF3Ext and contains all of the code and hooks.
* `bof3ext.toml` is the configuration file for BoF3Ext. With it, you can set the window size and render resolution scale, enable or disable the console in debug builds, and enable texture dumping if you plan to replace ingame textures.
* `loader.cfg` is a line-separated file that tells `ddraw.dll` which DLLs to load. By default, this file only includes `bof3ext.dll`, but other DLLs (like SpecialK) can be added to this list.
* `BOF3.CFG` is bof3.exe's configuration file. It contains 2 integer values on separate lines. The first value enables software rendering if set to 1, and the second value enables windowed mode if set to run. It is recommended to not change this file and to leave these 2 values as they originally are (0 and 1, respectively).
* `/NewData/` is a folder that contains all of the replacement data, including translated text, new HD textures for the title screen, and a new font.

## How can I help?

You can contribute to BoF3Ext in many ways!

* Tackle any of the current [issues](https://github.com/TheRealBiggs/bof3ext/issues).
* Contribute to the reverse-engineering effort that makes this work possible.
* Help with making HD versions of text-based UI elements in the [BoF3Ext_Resources repo](https://github.com/TheRealBiggs/bof3ext_resources).
* Help test development releases and report any issues you may find.
* Show your support by sharing a link this repo with your friends!