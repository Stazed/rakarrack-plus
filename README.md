Rakarack-plus
=============

Screenshot
----------

![screenshot](https://raw.github.com/Stazed/rakarrack-plus/wip/doc/rakarrack-plus-1.0.0.png "Rakarrack-plus Gleam Theme")


About
-----
This project is a merging of original rakarrack (http://rakarrack.sourceforge.net)
and the program's effects ported to LV2 from (https://github.com/ssj71/rkrlv2).
In addition there are many bug fixes and enhancements to rakarrack and the LV2s.
Rakarrack-plus-1.0.0 is the first version under the new name.

Install
-------

The dependencies are:

*   libfltk1.3
*   libxpm
*   libjack100.0
*   libasound2
*   libsamplerate0
*   libsndfile1
*   aconnect  (part of Debian Package alsa-utils, name may vary on other distributions)
*   jackd
*   libxft2
*   libfftw3

If you want to compile you will also need the development packages:

*   libxpm-dev 
*   libfltk1.3-dev
*   libjack-dev
*   libsndfile1-dev
*   libsamplerate0-dev
*   libasound2-dev
*   libxft-dev
*   libfftw3-dev


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
    
To build LV2s only set "BuildRakarrackPlus" to OFF

    cmake -DBuildRakarrackPlus=OFF ..


Rakarrack-plus-1.0.0
--------------------

First release under the new name "Rakarrack-plus".
Important changes from earlier versions - Rakarrack-plus:
*   FLTK version required >= 1.3
*   All text fonts are auto resized for the GUI
*   "Wet/Dry" sliders are now "Dry/Wet" (backwards compatible except Dual Flange and Ring)
*   Settings window can now be resized
*   New "Quality" tab in settings for individual effect settings
*   All changes on "Quality" tab no longer require restart
*   Expanded quality settings to allow individual settings for Waveshape Resampling, Sequence and Shifter.
*   MIDI learn expanded for many missing parameters including distortion Type and LFO Type
*   Keyboard bindings added to value adjusters to mimic slider behavior (Infinity, MuTroMojo, Synthfilter)
*   MuTroMojo, Synthfilter redesigned for better spacing
*   Some parameter ranges were changed for consistency (Stereo Df. and LFO L/R Delay) backwards compatible
*   Many additional individual efx presets ware added from the rkrlv2 project
*   Additional tooltips added
*   Labels and sizing adjustments for better display
*   Gleam theme added (default)
*   F12 fullscreen toggle now resizes fonts correctly
*   Updated program help for key changes and .jpg

LV2s - RakarrackPlus
--------------------
*   The URI's were changed and the plugin names and jack names will now indicate "RakarrackPlus"
*   "Wet/Dry" is now "Dry/Wet"
*   Some parameter ranges were changed to be consistent with the GUI program
*   "Overdrive" LV2 added
*   "Flanger" LV2 added
*   MIDI versions of the Harmonizer and StereoHarm were added
*   The "StompBox Fuzz" LV2 was removed. The presets and labels from the LV2 were adopted by the regular "StompBox"

Under The Hood
--------------
*   Fix Bankwindow memory leak
*   Fix Echotron, AlienWah intermittent crashes
*   Fix distortion Type "Super Soft"
*   Use initializer lists on all class/structures to eliminate intermittent uninitialized crashes
*   Add RKR override classes for text resizing

Additional Information
----------------------
*   File bug reports at (https://github.com/Stazed/rakarrack-plus)
*   See the "rakarrack-plut.txt" file for individual effect and LV2 changes
*   See README.legacy for previous version information
*   You may find additional information on the official rakarrack site (http://rakarrack.sourceforge.net)
*   This project was forked from (https://github.com/ssj71/rakarrack) >> (https://github.com/Stazed/rakarrack)
*   Special thanks to Spencer Jackson (https://github.com/ssj71/rkrlv2)
