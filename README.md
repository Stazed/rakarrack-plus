Rakarack-plus
=============

Rakarrack plus LV2s.

Screenshot
----------

![screenshot](https://raw.github.com/Stazed/rakarrack-plus/wip/doc/rakarrack-plus-1.0.0.png "Rakarrack-plus Gleam Theme")


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

If you want to compile you will also need the development packages:

*   libxpm-dev 
*   libfltk1.3-dev
*   libjack-dev
*   libsndfile1-dev
*   libsamplerate0-dev
*   libasound2-dev
*   libxft-dev


To build both rakarrack-plus and LV2s:
From top checkout directory:
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
    
To build LV2s only set "BuildRakarrack-plus" to OFF

    cmake -DBuildRakarrackPlus=OFF ..

