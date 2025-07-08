Rakarrack-plus
=============

Screenshot
----------

![screenshot](https://raw.github.com/Stazed/rakarrack-plus/wip/doc/rakarrack-plus-1.1.0.png "Rakarrack-plus release-1.1.0")


About
-----
This project is a merging of original rakarrack (http://rakarrack.sourceforge.net)
and the program's effects ported to LV2 from (https://github.com/ssj71/rkrlv2).
In addition there are many bug fixes and enhancements to rakarrack and the LV2s.
Rakarrack-plus-1.0.0 is the first version under the new name.

Install
-------

The dependencies are:

*   cmake
*   libfltk1.3
*   libxpm
*   libasound2
*   libsamplerate0
*   libsndfile1
*   aconnect  (part of Debian Package alsa-utils, name may vary on other distributions)
*   jack (One of the following jack1, jack2, pipewire-jack, name may vary with different distributions)
*   libxft2
*   libfftw3
*   lv2     (LV2 support)
*   liblo   (NSM support)
*   ntk     (X11 embedded GUI support or optional NTK standalone)

If you want to compile you will also need the development packages:

*   libxpm-dev 
*   libfltk1.3-dev
*   jack (The development package for jack1, jack2 or pipewire-jack, name may vary with different distributions)
*   libsndfile1-dev
*   libsamplerate0-dev
*   libasound2-dev
*   libxft-dev
*   libfftw3-dev
*   lv2-dev  (LV2 support)
*   liblo-dev   (NSM support)

Getting NTK:
------------

Your distribution may have NTK available. If not, you can get an "unofficial" updated NTK at:

```bash
    git clone https://github.com/Stazed/ntk-unofficial.git
```

To build both rakarrack-plus and LV2s, from top checkout directory:
```bash
    mkdir build
    cd build
    cmake ..
    make
    make install (as root)
```
To remove:
```bash
    make uninstall (as root)
```
Carla presets can be generated and installed by setting the "BuildCarlaPresets"
flag (OFF by default). Use:

    cmake -DBuildCarlaPresets=ON ..

To build Rakarrack-plus only set "BuildLV2Plugins" to OFF:

    cmake -DBuildLV2Plugins=OFF ..
    
To build LV2s only set "BuildRakarrackPlus" to OFF:

    cmake -DBuildRakarrackPlus=OFF ..

To disable NTK embedded LV2 build set "BuildRakarrackPlusLV2" to OFF:

    cmake -DBuildRakarrackPlusLV2=OFF ..

To enable NTK standalone build set "EnableNTK" to ON:

    cmake -DEnableNTK=ON ..

Rakarrack-plus-1.3.0
--------------------
Additional features, enhancements:
*   A new NTK embedded LV2 of rakarrack-plus standalone is now available.
*   Re-initialization on 'Master Upsampling' is added so a restart is not longer necessary.
*   An NTK version of the standalone rakarrack-plus is available (disabled by default).
*   The LV2 uses the NTK library which has some differences compared to FLTK and standalone.
    - The Settings/Preferences/Look 'Schema' selector is not valid for NTK.
    - To change the theme for NTK you must launch ntk-chtheme which is installed with NTK. A restart is required for LV2 to apply
  the changes.
    - If you use the original NTK library, the font selector is de-activated.
    - If you use ntk-unofficial (git clone https://github.com/Stazed/ntk-unofficial.git), then the font selector is active.
    - F12 fullscreen does not work correctly with NTK and is de-activated.
    - For LV2, the Settings/Preferences/Audio 'FX On at start' button is de-activated and is always active.
    - The LV2 uses a separate configure file than the standalone and each instance can have different settings.
    - The LV2 Settings/Preferences/Look items are global for all instances. Other tab items can be different.
    - For LV2 the Settings/Preferences/MIDI 'Auto Connect MIDI in' and 'ALSA Sequencer Ports' are de-activated since they do not apply.
    - For LV2 all items in the Settings/Preferences/Jack tab are de-activated as they do not apply.
    - For LV2 the AUX port is disabled to allow for insertion with LV2 host stereo tracks.
    - For LV2 the Tap Tempo can be synced to the host by selecting 'Input' to be 'Jack Transport (LV2)'
    - For LV2 all main window rack effects and 'Settings' are saved for each instance.
    - For LV2 the MIDI Converter FFT option is disabled do to fftw library incompatibility.

Rakarrack-plus-1.2.0
--------------------
Additional features, enhancements:
*   Three new SFX banks are now included with the installation. Select from the
    Bankwindow User Bank choice menu.
*   Echotron delay file editor added. Right mouse on Echotron delay file choice button.
*   The main window "Random" button is now configurable. Right mouse on the "Random" button.
*   All main rack effect parameters can be randomly set. Right mouse on the main rack name label.
*   Added option for NSM single state mode. Settings/Preferences/Misc. Only active in NSM session.
*   And more, see Help, release notes and git commits for additional information.

Rakarrack-plus-1.1.0
--------------------
Important changes from previous versions:
*   All user files must now be located in the User Directory.
*   MIDI Bank select CC 0, now available for program default and all user banks.
*   "Insert" preset file changed to user directory. From the main window File/Import Inserted Presets
    to import from older versions.
*   Some key binding has changed.
*   Added NSM session support.
*   Custom MIDI program table can now be used without the gui.
*   Custom MIDI program files must be in the User Directory.
*   Custom MIDI program files are MIDI selectable, CC 32, MIDI learnable.
*   File handling changed for Convolotron, Echotron and Reverbtron. See F1 help for additional information.
*   Error handling expanded.
*   Help updated.
*   Extensive re-factoring of MIDI control, Audio processing, and gui.
*   And many, many more... to numerous to list. See git commits for details.


Rakarrack-plus-1.0.0
--------------------

First release under the new name "Rakarrack-plus".
Important changes from earlier versions:
*   FLTK version required >= 1.3
*   All text fonts are auto resized for the GUI.
*   "Wet/Dry" sliders are now "Dry/Wet" (backwards compatible except Dual Flange and Ring).
*   Settings window can now be resized.
*   New "Quality" tab in settings for individual effect settings.
*   All changes on "Quality" tab no longer require restart
*   Expanded quality settings to allow individual settings for Waveshape Resampling, Sequence and Shifter.
*   MIDI learn expanded for many missing parameters including distortion Type and LFO Type.
*   Keyboard bindings added to value adjusters to mimic slider behavior (Infinity, MuTroMojo, Synthfilter).
*   MuTroMojo, Synthfilter redesigned for better spacing.
*   Some parameter ranges were changed for consistency (Stereo Df. and LFO L/R Delay) backwards compatible.
*   Many additional individual efx presets ware added from the rkrlv2 project.
*   Additional tooltips added.
*   Labels and sizing adjustments for better display.
*   Gleam theme added (default).
*   F12 fullscreen toggle now resizes fonts correctly.
*   Updated program help for key changes and .jpg.

LV2s - RakarrackPlus
--------------------
*   The URI's were changed and the plugin names and jack names will now indicate "RakarrackPlus".
*   "Wet/Dry" is now "Dry/Wet".
*   Some parameter ranges were changed to be consistent with the GUI program.
*   "Overdrive" LV2 added.
*   "Flanger" LV2 added.
*   MIDI versions of the Harmonizer and StereoHarm were added.
*   The "StompBox Fuzz" LV2 was removed. The presets and labels from the LV2 were adopted by the regular "StompBox".

Under The Hood
--------------
*   Fix Bankwindow memory leak.
*   Fix Echotron, AlienWah intermittent crashes.
*   Fix distortion Type "Super Soft".
*   Use initializer lists on all class/structures to eliminate intermittent uninitialized crashes.
*   Add RKR override classes for text resizing.

Additional Information
----------------------
*   File bug reports at (https://github.com/Stazed/rakarrack-plus).
*   See the "rakarrack-plus.txt" file for individual effect and LV2 changes.
*   See README.legacy for previous version information.
*   You may find additional information on the official rakarrack site (http://rakarrack.sourceforge.net).
*   This project was forked from (https://github.com/ssj71/rakarrack) >> (https://github.com/Stazed/rakarrack).
*   Special thanks to Spencer Jackson (https://github.com/ssj71/rkrlv2).

## SAST Tools

[PVS-Studio](https://pvs-studio.com/en/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.
