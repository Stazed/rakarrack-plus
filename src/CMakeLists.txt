#CMake file for Rakarrack

project (rakarrack)

#WEBSITE, PACKAGE, DATADIR, VERSION
#define WEBSITE "rakarrack.sf.net"
#define PACKAGE "rakarrack"
#define DATADIR "/usr/local/share/rakarrack"

set (VERSION "0.7.0")

set (CMAKE_SKIP_RULE_DEPENDENCY OFF)
    # Rebuild objects if rules have changed, even if source hasn't.

message (STATUS "Building Rakarrack version ${VERSION} for ${CMAKE_SYSTEM_NAME}")
include (CheckCSourceCompiles)

option (BuildForDebug "Include gdb debugging support" OFF)
option (LV2Plugin "Build rakarack lv2 plugin interface" ON)
option (optimizations "Enable optimizations" ON)
option (vectorize "Enable vectorize" ON)
option (sse "Enable sse (default=automatic check)" ON)
option (sse2 "Enable sse2 (default=automatic check)" ON)
option (altivec "Enable altivec (default=automatic check)" ON)

# Check for jack session availability
check_c_source_compiles (
    "#include <jack/session.h>
    int main(int argc, char **argv)
    {
        jack_session_event_type_t x = JackSessionSave;
        return 0;
    }" HasJackSession
)

if (HasJackSession)
    option (JackSessionSupport "Include Jack Session Support" ON)
else (HasJackSession)
    set (JackSessionSupport OFF)
endif (HasJackSession)


find_package (PkgConfig REQUIRED)
if (PKG_CONFIG_FOUND)
    message(STATUS "Found pkg-config ${PKG_CONFIG_EXECUTABLE}")
else (PKG_CONFIG_FOUND)
    message(FATAL_ERROR "pkg-config required but not found")
endif (PKG_CONFIG_FOUND)


# fltk
find_package (FLTK REQUIRED)
if (FLTK_FOUND)
    message (STATUS "Found FLTK")
    set (FLTK_LIBRARIES "${FLTK_LIBRARIES}")
else (FLTK_FOUND)
    message (FATAL_ERROR "FLTK required but not found")
endif (FLTK_FOUND)

#find_program (HAVE_ACONNECT NAMES aconnect )
#check_library_exists(aconnect "" "" HAVE_ACONNECT)
#pkg_check_modules (ACONNECT REQUIRED aconnect)
#find_package (ACONNECT REQUIRED)
#if (HAVE_ACONNECT)
#    message (STATUS "Found Aconnect")
#else (HAVE_ACONNECT)
#    message (FATAL_ERROR "Aconnect required but not found")
#endif (HAVE_ACONNECT)

# Alsa
pkg_check_modules (ALSA REQUIRED alsa>=1.0.17)
if (ALSA_FOUND)
    message(STATUS "Found Alsa ${ALSA_VERSION}")
    set (ALSA_LIBRARIES "${ALSA_LIBRARIES}")
else (ALSA_FOUND)
    message(FATAL_ERROR "Alsa >=1.0.17 required but not found")
endif (ALSA_FOUND)

# Jack
pkg_check_modules (JACK REQUIRED jack>=0.115.6)
if (JACK_FOUND)
    message(STATUS "Found jack ${JACK_VERSION}")
    set (JACK_LIBRARIES "${JACK_LIBRARIES}")
else (JACK_FOUND)
    message(FATAL_ERROR "Jack >=0.115.6 required but not found")
endif (JACK_FOUND)

# fontconfig
pkg_check_modules (FONTCONFIG REQUIRED fontconfig>=0.22)
mark_as_advanced(FONTCONFIG_LIBRARIES)
if(FONTCONFIG_FOUND)
    message (STATUS "Found fontconfig ${FONTCONFIG_VERSION}")
    set (FONTCONFIG_LIBRARIES "${FONTCONFIG_LIBRARIES}")
else(FONTCONFIG_FOUND)
    message (FATAL_ERROR "fontconfig>=0.22 required but not found")
endif(FONTCONFIG_FOUND)

# fftw3
pkg_check_modules (FFTW3 REQUIRED fftw3>=0.22)
if (FFTW3_FOUND)
    set (FFTW3_LIBRARIES "${FFTW3_LIBRARIES}")
    message (STATUS "Found fftw3 ${FFTW3_VERSION}")
else (FFTW3_FOUND)
    message (FATAL_ERROR "fftw3 >=0.22 required but not found")
endif (FFTW3_FOUND)

# fftw3f
pkg_check_modules (FFTW3F REQUIRED fftw3f>=0.22)
if (FFTW3F_FOUND)
    set (FFTW3F_LIBRARIES "${FFTW3F_LIBRARIES}")
    message (STATUS "Found fftw3f ${FFTW3F_VERSION}")
else (FFTW3F_FOUND)
    message (FATAL_ERROR "fftw3f >=0.22 required but not found")
endif (FFTW3F_FOUND)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET rakarrack PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET rakarrack PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
#if(CMAKE_THREAD_LIBS_INIT)
#  target_link_libraries(rakarrack "${CMAKE_THREAD_LIBS_INIT}")
#endif()

pkg_check_modules(SNDFILE REQUIRED sndfile)
if (SNDFILE_FOUND)
    set (SNDFILE_LIBRARIES "${SNDFILE_LIBRARIES}")
    message (STATUS "Found sndfile ${SNDFILE_VERSION}")
else (SNDFILE_FOUND)
    message (FATAL_ERROR "sndfile required but not found")
endif (SNDFILE_FOUND)

# samplerate
pkg_check_modules(SAMPLERATE REQUIRED samplerate>=0.1.5)
if (SAMPLERATE_FOUND)
    set (SAMPLERATE_LIBRARIES "${SAMPLERATE_LIBRARIES}")
    message (STATUS "Found samplerate ${SAMPLERATE_VERSION}")
else (SAMPLERATE_FOUND)
    message (FATAL_ERROR "samplerate required but not found")
endif (SAMPLERATE_FOUND)

# X11
pkg_check_modules(X11 REQUIRED x11)
if (X11_FOUND)
    set (X11_LIBRARIES "${X11_LIBRARIES}")
    message (STATUS "Found x11 ${X11_VERSION}")
else (X11_FOUND)
    message (FATAL_ERROR "x11 required but not found")
endif (X11_FOUND)

# Xft
pkg_check_modules(XFT REQUIRED xft)
if (XFT_FOUND)
    set (XFT_LIBRARIES "${XFT_LIBRARIES}")
    message (STATUS "Found Xft ${XFT_VERSION}")
else (XFT_FOUND)
    message (FATAL_ERROR "Xft required but not found")
endif (XFT_FOUND)

# Xrender
pkg_check_modules(XRENDER REQUIRED xrender)
if (XRENDER_FOUND)
    set (XRENDER_LIBRARIES "${XRENDER_LIBRARIES}")
    message (STATUS "Found xrender ${XRENDER_VERSION}")
else (XRENDER_FOUND)
    message (FATAL_ERROR "Xrender required but not found")
endif (XRENDER_FOUND)

# Xpm
pkg_check_modules(XPM REQUIRED xpm)
if (XPM_FOUND)
    set (XPM_LIBRARIES "${XPM_LIBRARIES}")
    message (STATUS "Found Xpm ${XPM_VERSION}")
else (XPM_FOUND)
    message (FATAL_ERROR "Xpm required but not found")
endif (XPM_FOUND)

# freetype
pkg_check_modules(FREETYPE REQUIRED freetype2)
if (FREETYPE_FOUND)
    set (FREETYPE_LIBRARIES "${FREETYPE_LIBRARIES}")
    message (STATUS "Found freetype ${FREETYPE_VERSION}")
else (FREETYPE_FOUND)
    message (FATAL_ERROR "freetype required but not found")
endif (FREETYPE_FOUND)

# libz
set (CMAKE_REQUIRED_LIBRARIES z)
check_c_source_compiles (
    "#include <zlib.h>
     int main(int argc, char **argv) {
         gzFile zzz  = gzopen(\"/dev/null\", \"rb\");
         if (NULL != zzz)
            gzclose(zzz);
         return 0;
     }" HAS_LIBZ
)
if (HAS_LIBZ)
    message(STATUS "Found libz")
else (HAS_LIBZ)
    message(FATAL_ERROR "libz required but not found: ${HAS_LIBZ}")
endif (HAS_LIBZ)

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
    ${FFTW3F_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SNDFILE_LIBRARIES}
    ${SAMPLERATE_LIBRARIES}
    ${X11_LIBRARIES}
    ${XFT_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XPM_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    z
    dl
    rt
    m
)

add_executable (rakarrack ${ProgSources} main.C)

target_link_libraries (rakarrack ${ExternLibraries})

set_source_files_properties(MIDIConverter.C PROPERTIES COMPILE_FLAGS -std=gnu++98)
target_sources(rakarrack PRIVATE
  MIDIConverter.C
)

install (TARGETS rakarrack RUNTIME DESTINATION bin)

if (LV2Plugin)
    if (EXISTS $ENV{LV2_PATH})
        set(LV2_INSTALL_DIR $ENV{LV2_PATH} CACHE PATH
        "Specifies where the LV2 libraries should be installed")
    else()
        set(LV2_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/lv2 CACHE PATH
        "Specifies where the LV2 libraries should be installed")
    endif()

    pkg_check_modules(LV2 REQUIRED lv2>=1.0.0)
    if (LV2_FOUND)
        message (STATUS "Found lv2 package ${LV2_VERSION}")
    else (LV2_FOUND)
        message (FATAL_ERROR "lv2 package required but not found (version 1.0.0 needed)")
    endif (LV2_FOUND)
#    add_subdirectory(../lv2)
endif (LV2Plugin)
