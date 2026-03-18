#CMake file for Rakarrack-plus & LV2

cmake_minimum_required(VERSION 3.5.1...3.31.1)
project (rakarrack-plus)

set (PACKAGE_VERSION "1.4.0")

set (CMAKE_SKIP_RULE_DEPENDENCY OFF)

add_definitions(-D'VERSION="${PACKAGE_VERSION}"')
add_definitions(-D'WEBSITE="github.com.Stazed.rakarrack.plus"')
add_definitions(-D'PACKAGE="rakarrack-plus"')
add_definitions(-D'DATADIR="${CMAKE_INSTALL_PREFIX}/share/rakarrack-plus"')
add_definitions(-D'HELPDIR="${CMAKE_INSTALL_PREFIX}/share/doc/rakarrack-plus"')
add_definitions(-D'ICONDIR="${CMAKE_INSTALL_PREFIX}/share/pixmaps"')

include(CheckCSourceCompiles)
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-msse2" SUPPORT_SSE)
check_cxx_compiler_flag("-ftree-vectorize" SUPPORT_VECTOR)
check_cxx_compiler_flag("-maltivec" SUPPORT_ALTIVEC)

option (BUILD_RPLUS_STANDALONE "Build rakarrack-plus standalone" ON)
option (BUILD_RPLUS_LV2 "Build rakarrack-plus LV2 (Rakarrack-Plus.lv2)" ON)
option (BUILD_LV2_EFFECTS "Build rakarack-plus lv2 plugin effects (RakarrackPlus.lv2)" ON)
option (BUILD_CARLA_PRESETS "Build Carla presets for lv2 from default banks" OFF)
option (BUILD_FOR_DEBUG "Include gdb debugging support" OFF)
option (ENABLE_NTK "Use NTK toolkit for standalone build" OFF)
option (ENABLE_NSM "Enable NSM session support" ON)
option (ENABLE_SYSEX "Enable Sysex preset save support" ON)
option (ENABLE_OPTIMIZATIONS "Enable optimizations" ON)
option (ENABLE_SSE "Enable SSE (default=automatic check)" ON)
option (ENABLE_SSE2 "Enable SSE2 (default=automatic check)" ON)
option (ENABLE_VECTORIZATION "Enable vectorize" ON)
option (ENABLE_ALTIVEC "Enable altivec (default=automatic check)" ON)
option (ENABLE_PFFFT "Use the PFFFT library instead of FFTW" ON)
option (ENABLE_ZITA "Use Zita Resampler instead of libsamplerate" ON)
option (ENABLE_ALSA_MIDI "Enable alsa MIDI support" ON)


set(CMAKE_BUILD_TYPE "Release")

set (BuildOptions_SSE
    "-msse -mfpmath=sse"
    CACHE STRING "SSE compiler options"
)

set (BuildOptions_SSE2
    "-msse -msse2 -mfpmath=sse"
    CACHE STRING "SSE2 compiler options"
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "USING CLANG OPTIMIZATIONS")
    set(BuildOptions_Vectorization
        "-ftree-vectorize -fhonor-nans -fhonor-infinities -Wno-vla-cxx-extension"
        CACHE STRING "Vectorization compiler options"
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "USING GNU OPTIMIZATIONS")
    set(BuildOptions_Vectorization
        "-ftree-vectorize -fvect-cost-model"
        CACHE STRING "Vectorization compiler options"
    )

    set(BuildOptions_Altivec
        "-maltivec"
        CACHE STRING "Altivec compiler options"
    )
endif()

set (BuildOptionsBasic
    "-std=c++11 -Wno-unused-parameter -O3 -ffast-math -pipe -fsigned-char -fvisibility=hidden"
    CACHE STRING "basic X86 complier options"
)

set (BuildOptionsDebug
    "-std=c++11 -Wno-cast-function-type -O0 -g3 -ggdb -Wall -Wextra -Wpointer-arith"
    CACHE STRING "Debug build flags"
)

if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    message(STATUS "Building on FreeBSD")
    list(PREPEND CMAKE_PREFIX_PATH "/usr/local")
    link_directories("/usr/local/lib")
    include_directories(SYSTEM "/usr/local/include")
    set (ENABLE_ALSA_MIDI "OFF")
endif()

find_package (PkgConfig REQUIRED)

#dependencies
if(BUILD_RPLUS_LV2)
    pkg_check_modules(NTK ntk REQUIRED)
    pkg_check_modules(NTK_IMAGES ntk_images REQUIRED)
endif(BUILD_RPLUS_LV2)

if(BUILD_RPLUS_STANDALONE)
    if(ENABLE_NTK)
        pkg_check_modules(NTK ntk REQUIRED)
        pkg_check_modules(NTK_IMAGES ntk_images REQUIRED)
        add_definitions(-D'BUILD_TYPE="Built with NTK Library"')
    else(ENABLE_NTK)
        SET(FLTK_SKIP_OPENGL TRUE) 
        find_package(FLTK 1.4 CONFIG)
        if(TARGET fltk::fltk-shared)
            message(STATUS "FLTK CONFIG is available")
            add_definitions(-D'FLTK_VERSION_1_4=1')
        else()
            # If FLTK version is < 1.4 the CONFIG option does not link, so re-run find_package without it.
            find_package(FLTK REQUIRED)
            message(STATUS "FLTK version is older than 1.4.0.")
        endif()
    endif(ENABLE_NTK)
endif(BUILD_RPLUS_STANDALONE)

if(ENABLE_ALSA_MIDI)
    find_package(ALSA REQUIRED alsa>=1.0.17)
    find_program(ACONNECT aconnect)
    if (ACONNECT)
        message (STATUS "Found aconnect in ${ACONNECT}")
    else (ACONNECT)
        message (FATAL_ERROR "Aconnect required but not found")
    endif (ACONNECT)
    add_definitions(-D'ALSA_SUPPORT=1')
endif()

find_package(X11 REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET rakarrack-plus PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET rakarrack-plus PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

if (ENABLE_PFFFT)
    message(STATUS "Using PFFFT library")
    add_definitions(-D'PFFFT_SUPPORT=1')
else()
    pkg_check_modules(FFTW3 REQUIRED fftw3>=0.22 fftw3f>=0.22)
endif()

if (ENABLE_ZITA)
    find_path(ZITA_RESAMPLER_INCLUDE_DIR
    NAMES resampler.h
    PATH_SUFFIXES zita-resampler
    )

    find_library(ZITA_RESAMPLER_LIBRARY
        NAMES zita-resampler
    )

    if (NOT ZITA_RESAMPLER_INCLUDE_DIR OR NOT ZITA_RESAMPLER_LIBRARY)
        message(FATAL_ERROR "zita-resampler not found")
    else()
        add_definitions(-D'ZITA_SUPPORT=1')
    endif()
else (ENABLE_ZITA)
    pkg_check_modules(SAMPLERATE REQUIRED samplerate>=0.1.5)
endif (ENABLE_ZITA)

pkg_check_modules(JACK REQUIRED jack>=0.115.6)
pkg_check_modules(FONTCONFIG REQUIRED fontconfig>=0.22)
pkg_check_modules(SNDFILE REQUIRED sndfile)
pkg_check_modules(XFT REQUIRED xft)
pkg_check_modules(XRENDER REQUIRED xrender)
pkg_check_modules(XPM REQUIRED xpm)

if (ENABLE_NSM)
    pkg_check_modules(LIBLO liblo>=0.26 REQUIRED)
    if (LIBLO_FOUND)
        mark_as_advanced (LIBLO)
    endif (LIBLO_FOUND)
endif (ENABLE_NSM)

if (BUILD_CARLA_PRESETS)
    find_package(
            Python3
            REQUIRED
            COMPONENTS Interpreter
    )
endif (BUILD_CARLA_PRESETS)


find_library(RT_LIBRARY rt REQUIRED)
find_library(DL_LIBRARY dl REQUIRED)
find_library(M_LIBRARY m REQUIRED)

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

mark_as_advanced (FONTCONFIG_LIBRARIES)
mark_as_advanced (CMAKE_BUILD_TYPE)
mark_as_advanced (CMAKE_INSTALL_PREFIX)
mark_as_advanced (ACONNECT)

if(ENABLE_NTK)
#TODO
else(ENABLE_NTK)
mark_as_advanced (FLTK_DIR)
mark_as_advanced (FLTK_MATH_LIBRARY)
endif(ENABLE_NTK)

mark_as_advanced (DL_LIBRARY)
mark_as_advanced (RT_LIBRARY)
mark_as_advanced (M_LIBRARY)

if(ENABLE_SYSEX)
    add_definitions(-D'SYSEX_SUPPORT=1')
endif (ENABLE_SYSEX)

if(BUILD_RPLUS_STANDALONE OR BUILD_RPLUS_LV2)
    install(FILES COPYING AUTHORS README README.md
        DESTINATION share/doc/rakarrack-plus)
endif(BUILD_RPLUS_STANDALONE OR BUILD_RPLUS_LV2)
#TODO move from /data to main
#install(FILES rakarrack-plus-plus.desktop
#    DESTINATION share/applications)

if (BUILD_FOR_DEBUG)
    set (CMAKE_BUILD_TYPE "Debug")
    set (CMAKE_CXX_FLAGS_DEBUG ${BuildOptionsDebug})
else (BUILD_FOR_DEBUG)
    set (CMAKE_BUILD_TYPE "Release")

    set (CMAKE_CXX_FLAGS_RELEASE ${BuildOptionsBasic})

    if(ENABLE_OPTIMIZATIONS)
        if (SUPPORT_SSE)
            if(ENABLE_SSE)
                if(ENABLE_SSE2)
                    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_SSE2}")
                    set (USE_SSE2 "${SUPPORT_SSE}")
                    set (USE_SSE "${SUPPORT_SSE}") #uses both
                else(ENABLE_SSE2)
                    set (USE_SSE "${SUPPORT_SSE}")
                    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_SSE}")
                endif(ENABLE_SSE2)
            endif(ENABLE_SSE)
        endif (SUPPORT_SSE)

        if (SUPPORT_VECTOR)
            if(ENABLE_VECTORIZATION)
                set (USE_VECTOR "${SUPPORT_VECTOR}")
                set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_Vectorization}")
            endif(ENABLE_VECTORIZATION)
        endif(SUPPORT_VECTOR)

        if (SUPPORT_ALTIVEC)
            if(ENABLE_ALTIVEC)
                set (USE_ALTIVEC "${SUPPORT_ALTIVEC}")
                set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_Altivec}")
            endif(ENABLE_ALTIVEC)
        endif (SUPPORT_ALTIVEC)
    else(ENABLE_OPTIMIZATIONS)
        set (USE_SSE2 "")
        set (USE_SSE "")
        set (USE_VECTOR "")
        set (USE_ALTIVEC "")
    endif(ENABLE_OPTIMIZATIONS)
endif (BUILD_FOR_DEBUG)

if (BUILD_RPLUS_LV2)
    pkg_check_modules(LV2 REQUIRED lv2>=1.0.0)
    if (LV2_FOUND)
        if (DEFINED LV2_PATH)
            set(LV2_INSTALL_DIR_RKRP "${LV2_PATH}/Rakarrack-Plus.lv2" CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        else()
            set(LV2_INSTALL_DIR_RKRP lib/lv2/Rakarrack-Plus.lv2 CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        endif()
        add_subdirectory(src/LV2_Plugin)
    endif (LV2_FOUND)
endif (BUILD_RPLUS_LV2)

if (BUILD_LV2_EFFECTS)
    pkg_check_modules(LV2 REQUIRED lv2>=1.0.0)
    if (LV2_FOUND)
        if (DEFINED LV2_PATH)
            set(LV2_INSTALL_DIR "${LV2_PATH}/RakarrackPlus.lv2" CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        else()
            set(LV2_INSTALL_DIR lib/lv2/RakarrackPlus.lv2 CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        endif()

        if(BUILD_CARLA_PRESETS)
            message (STATUS "Building Carla presets...")
            execute_process(COMMAND ./convertbank.py ${CMAKE_INSTALL_PREFIX}/${LV2_INSTALL_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lv2)
        endif(BUILD_CARLA_PRESETS)

        #rename these files so they are easier to use for plugins
        SET( RVB x;Chamber;Concrete_Stairwell;Hall;Med_Hall;Large_Room;Large_Hall;Guitar_Ambience;Studio;Twilight;Santa_Lucia )
        SET( DLY x;SwingPong;Short_Delays;Flange_plus_Echo;Comb;EchoFlange;Filtered_Echo;Notch-Wah;Multi-Chorus;PingPong;90-Shifter;Basic_LR_Delay )
        SET( WAV x;Marshall_JCM200;Fender_Superchamp;Mesa_Boogie;Mesa_Boogie-2;Marshall_Plexi;Bassman;JCM2000;Ampeg;Marshall-2 )

        foreach(INDX RANGE 1 9)
            list(GET RVB ${INDX} NEWNAME)
            configure_file(data/${INDX}.rvb lv2data/${NEWNAME}.rvb COPYONLY)
            list(GET DLY ${INDX} NEWNAME)
            configure_file(data/${INDX}.dly lv2data/${NEWNAME}.dly COPYONLY)
            list(GET WAV ${INDX} NEWNAME)
            configure_file(data/${INDX}.wav lv2data/${NEWNAME}.wav COPYONLY)
        endforeach(INDX)
        # 2 more dly file 
        list(GET DLY 10 NEWNAME)
        configure_file(data/10.dly lv2data/${NEWNAME}.dly COPYONLY)
        list(GET DLY 11 NEWNAME)
        configure_file(data/11.dly lv2data/${NEWNAME}.dly COPYONLY)

        # 1 more rvb file
        list(GET RVB 10 NEWNAME)
        configure_file(data/10.rvb lv2data/${NEWNAME}.rvb COPYONLY)

        add_subdirectory(LV2_Effects)
    endif (LV2_FOUND)
else (BUILD_LV2_EFFECTS)
    set (BUILD_CARLA_PRESETS OFF)
endif (BUILD_LV2_EFFECTS)

if(BUILD_RPLUS_STANDALONE)
    add_subdirectory(src)
endif (BUILD_RPLUS_STANDALONE)

if(BUILD_RPLUS_STANDALONE OR BUILD_RPLUS_LV2)
    add_subdirectory(man)
    add_subdirectory(data)
    add_subdirectory(icons)
    add_subdirectory(doc)
    add_subdirectory(extra)
endif (BUILD_RPLUS_STANDALONE OR BUILD_RPLUS_LV2)


##Summarize The Full Configuration
message(STATUS)
message(STATUS "=========================================================")
message(STATUS "Build Configuration for Rakarrack-plus-${PACKAGE_VERSION} for ${CMAKE_SYSTEM_NAME}")
message(STATUS "=========================================================")
message(STATUS)


macro(package_status foundvar pkg)
    if(${foundvar})
        message(STATUS "${pkg} Yes")
    else()
        message(STATUS "${pkg} No")
    endif()
endmacro()

message (STATUS                    "___________________ Optimizations________________________"  )
package_status(USE_VECTOR          "Vectorization. . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(USE_ALTIVEC         "Use altiveck . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(USE_SSE             "Use sse. . . . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(USE_SSE2            "Use sse2 . . . . . . . . . . . . . . . . . . . . . . . .:"  )

message (STATUS)
message (STATUS                    "______________________Library____________________________"  )
package_status(PKG_CONFIG_FOUND    "PkgConfig. . . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(FLTK_FOUND          "FLTK toolkit . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(NTK_FOUND           "NTK toolkit. . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ALSA_FOUND          "ALSA MIDI Sequencer support. . . . . . . . . . . . . . .:"  )
package_status(X11_FOUND           "X11 support. . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(Freetype_FOUND      "Freetype support . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ZLIB_FOUND          "Zlib support . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(Threads_FOUND       "pthread support (libpthread) . . . . . . . . . . . . . .:"  )
package_status(JACK_FOUND          "JACK Audio Connection Kit library. . . . . . . . . . . .:"  )
package_status(FONTCONFIG_FOUND    "Fontconfig support . . . . . . . . . . . . . . . . . . .:"  )
package_status(SNDFILE_FOUND       "General audio file (libsndfile). . . . . . . . . . . . .:"  )
package_status(XFT_FOUND           "Xft support. . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(XRENDER_FOUND       "Xrender support. . . . . . . . . . . . . . . . . . . . .:"  )
package_status(XPM_FOUND           "Xpm support (libxpm) . . . . . . . . . . . . . . . . . .:"  )
package_status(M_LIBRARY           "libm support . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(DL_LIBRARY          "libdl support. . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(RT_LIBRARY          "librt support. . . . . . . . . . . . . . . . . . . . . .:"  )

if (NOT ENABLE_PFFFT)
    package_status(FFTW3_FOUND         "Fast Fourier Transform (fftw3) . . . . . . . . . . . . .:"  )
endif()

if (ENABLE_ZITA)
    package_status(ZITA_RESAMPLER_LIBRARY    "ZITA resampler support . . . . . . . . . . . . . . . . .:"  )
else ()
    package_status(SAMPLERATE_FOUND    "Samplerate conversion (libsamplerate). . . . . . . . . .:"  )
endif()

if (ENABLE_NSM)
    package_status(LIBLO_FOUND         "Liblo support. . . . . . . . . . . . . . . . . . . . . .:"  )
endif(ENABLE_NSM)

if(BUILD_LV2_EFFECTS)
    package_status(LV2_FOUND           "LV2 support. . . . . . . . . . . . . . . . . . . . . . .:"  )
endif(BUILD_LV2_EFFECTS)

message (STATUS)
message (STATUS                         "____________________ User Options________________________"  )
package_status(ENABLE_OPTIMIZATIONS     "Use optimizations. . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_VECTORIZATION     "Vectorization. . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_ALTIVEC           "Use altiveck . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_SSE               "Use sse. . . . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_SSE2              "Use sse2 . . . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_NTK               "Enable NTK build . . . . . . . . . . . . . . . . . . . .:"  )
package_status(BUILD_RPLUS_STANDALONE   "Build Rakarrack-plus . . . . . . . . . . . . . . . . . .:"  )
package_status(BUILD_RPLUS_LV2          "Build Rakarrack-Plus-LV2 (Rakarrack-Plus.lv2). . . . . .:"  )
package_status(BUILD_LV2_EFFECTS        "Build LV2s (RakarrackPlus.lv2) . . . . . . . . . . . . .:"  )
package_status(BUILD_CARLA_PRESETS      "Build Carla lv2 presets. . . . . . . . . . . . . . . . .:"  )
package_status(BUILD_FOR_DEBUG          "Build for debug. . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_NSM               "NSM session support. . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_SYSEX             "Sysex preset save support. . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_PFFFT             "PFFFT support. . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_ZITA              "ZITA resampler support . . . . . . . . . . . . . . . . .:"  )
package_status(ENABLE_ALSA_MIDI         "ALSA MIDI support. . . . . . . . . . . . . . . . . . . .:"  )

message (STATUS)
message (STATUS)
message (STATUS "Building for ${CMAKE_BUILD_TYPE}")
if (BUILD_FOR_DEBUG)
    message (STATUS "Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
else (BUILD_FOR_DEBUG)
    message (STATUS "Flags: ${CMAKE_CXX_FLAGS_RELEASE}")
endif (BUILD_FOR_DEBUG)
