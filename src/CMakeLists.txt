#CMake file for Rakarrack

project (rakarrack)

set (CMAKE_SKIP_RULE_DEPENDENCY OFF)
    # Rebuild objects if rules have changed, even if source hasn't.

message (STATUS "Building Rakarrack version ${VERSION} for ${CMAKE_SYSTEM_NAME}")

#Dependency check

# fltk
find_package (FLTK REQUIRED)
if (FLTK_FOUND)
    set (FLTK_LIBRARIES "${FLTK_LIBRARIES}")
else (FLTK_FOUND)
    message (FATAL_ERROR "FLTK required but not found")
endif (FLTK_FOUND)

#aconnect
find_program(ACONNECT aconnect)
if (ACONNECT)
    message (STATUS "Found aconnect in ${ACONNECT}")
else (ACONNECT)
    message (FATAL_ERROR "Aconnect required but not found")
endif (ACONNECT)

# Alsa
pkg_check_modules (ALSA REQUIRED alsa>=1.0.17)
if (ALSA_FOUND)
    set (ALSA_LIBRARIES "${ALSA_LIBRARIES}")
else (ALSA_FOUND)
    message(FATAL_ERROR "Alsa >=1.0.17 required but not found")
endif (ALSA_FOUND)

# Jack
pkg_check_modules (JACK REQUIRED jack>=0.115.6)
if (JACK_FOUND)
    set (JACK_LIBRARIES "${JACK_LIBRARIES}")
else (JACK_FOUND)
    message(FATAL_ERROR "Jack >=0.115.6 required but not found")
endif (JACK_FOUND)

# fontconfig
pkg_check_modules (FONTCONFIG REQUIRED fontconfig>=0.22)
mark_as_advanced(FONTCONFIG_LIBRARIES)
if(FONTCONFIG_FOUND)
    set (FONTCONFIG_LIBRARIES "${FONTCONFIG_LIBRARIES}")
else(FONTCONFIG_FOUND)
    message (FATAL_ERROR "fontconfig>=0.22 required but not found")
endif(FONTCONFIG_FOUND)

# fftw3
pkg_check_modules (FFTW3 REQUIRED fftw3>=0.22 fftw3f>=0.22)
if (FFTW3_FOUND)
    set (FFTW3_LIBRARIES "${FFTW3_LIBRARIES}")
else (FFTW3_FOUND)
    message (FATAL_ERROR "fftw3 >=0.22 required but not found")
endif (FFTW3_FOUND)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET rakarrack PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET rakarrack PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

pkg_check_modules(SNDFILE REQUIRED sndfile)
if (SNDFILE_FOUND)
    set (SNDFILE_LIBRARIES "${SNDFILE_LIBRARIES}")
else (SNDFILE_FOUND)
    message (FATAL_ERROR "sndfile required but not found")
endif (SNDFILE_FOUND)

# samplerate
pkg_check_modules(SAMPLERATE REQUIRED samplerate>=0.1.5)
if (SAMPLERATE_FOUND)
    set (SAMPLERATE_LIBRARIES "${SAMPLERATE_LIBRARIES}")
else (SAMPLERATE_FOUND)
    message (FATAL_ERROR "samplerate required but not found")
endif (SAMPLERATE_FOUND)

# X11
find_package(X11 REQUIRED)
if (X11_FOUND)
    set (X11_LIBRARIES "${X11_LIBRARIES}")
else (X11_FOUND)
    message (FATAL_ERROR "x11 required but not found")
endif (X11_FOUND)

# Xft
pkg_check_modules(XFT REQUIRED xft)
if (XFT_FOUND)
    set (XFT_LIBRARIES "${XFT_LIBRARIES}")
else (XFT_FOUND)
    message (FATAL_ERROR "Xft required but not found")
endif (XFT_FOUND)

# Xrender
pkg_check_modules(XRENDER REQUIRED xrender)
if (XRENDER_FOUND)
    set (XRENDER_LIBRARIES "${XRENDER_LIBRARIES}")
else (XRENDER_FOUND)
    message (FATAL_ERROR "Xrender required but not found")
endif (XRENDER_FOUND)

# Xpm
pkg_check_modules(XPM REQUIRED xpm)
if (XPM_FOUND)
    set (XPM_LIBRARIES "${XPM_LIBRARIES}")
else (XPM_FOUND)
    message (FATAL_ERROR "Xpm required but not found")
endif (XPM_FOUND)

# freetype
find_package(Freetype REQUIRED )
if (FREETYPE_FOUND)
    set (FREETYPE_LIBRARIES "${FREETYPE_LIBRARIES}")
else (FREETYPE_FOUND)
    message (FATAL_ERROR "freetype required but not found")
endif (FREETYPE_FOUND)

# libz
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    set (ZLIB_LIBRARIES "${ZLIB_LIBRARIES}")
else (ZLIB_FOUND)
    message(FATAL_ERROR "libz required but not found")
endif (ZLIB_FOUND)

# librt
check_library_exists(rt nanosleep "" HAVE_LIBRT)
if (HAVE_LIBRT)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} rt)
else (HAVE_RT)
    message (FATAL_ERROR "rt required but not found")
endif (HAVE_LIBRT)
#To link-in librt.a or librt.so, which is the Realtime Extensions library.


if (JackSessionSupport)
    message(STATUS "With jack session support")
    add_definitions(-DJACK_SESSION=1)
else (JackSessionSupport)
    message(STATUS "Without jack session support")
endif (JackSessionSupport)

set (ProgSources
    jack.C 
    rakarrack.cxx
    process.C
    fileio.C
    varios.C
    FPreset.C
    Reverb.C
    Chorus.C
    EffectLFO.C
    Echo.C
    Phaser.C
    Distorsion.C
    EQ.C
    Compressor.C
    AnalogFilter.C
    Alienwah.C
    beattracker.C
    delayline.C
    Filter.C
    HarmonicEnhancer.C
    DynamicFilter.C
    SVFilter.C
    RBFilter.C
    FormantFilter.C
    FilterParams.C
    Pan.C
    Harmonizer.C
    smbPitchShift.C
    Tuner.C
    MusicDelay.C
    Gate.C
    RecognizeNote.C
    RecChord.C
    rkrMIDI.C
    NewDist.C
    APhaser.C
    Dual_Flange.C
    Exciter.C
    MBDist.C
    Valve.C
    Arpie.C
    Expander.C
    Ring.C
    Shuffle.C
    mayer_fft.C
    MBVvol.C
    metronome.C
    Convolotron.C
    Synthfilter.C
    Resample.C
    Looper.C
    RyanWah.C
    RBEcho.C
    CoilCrafter.C
    ShelfBoost.C
    Vocoder.C
    Waveshaper.C
    Sustainer.C
    Sequence.C
    Shifter.C
    StompBox.C
    Reverbtron.C
    Echotron.C
    StereoHarm.C
    CompBand.C
    Opticaltrem.C
    Vibe.C
    Infinity.C
    Cabinet.C
)


set(ExternLibraries
    ${FLTK_LIBRARIES}
    ${ALSA_LIBRARIES}
    ${JACK_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${FFTW3_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SNDFILE_LIBRARIES}
    ${SAMPLERATE_LIBRARIES}
    ${X11_LIBRARIES}
    ${XFT_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XPM_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${ZLIB_LIBRARIES}
    dl
    rt
    m
)

add_executable (rakarrack ${ProgSources} main.C)

target_include_directories (
    rakarrack PRIVATE
    ${FLTK_INCLUDE_DIRS}
    ${ALSA_INCLUDE_DIRS}
    ${JACK_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIRS}
    ${FFTW3_INCLuDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${SAMPLERATE_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${XFT_INCLUDE_DIRS}
    ${XRENDER_INCLUDE_DIRS}
    ${XPM_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
)

target_link_libraries (rakarrack ${ExternLibraries})

set_source_files_properties(MIDIConverter.C PROPERTIES COMPILE_FLAGS -std=gnu++98)
target_sources(rakarrack PRIVATE
  MIDIConverter.C
)

install (TARGETS rakarrack RUNTIME DESTINATION bin)

