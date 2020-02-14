#CMake file for Rakarrack & LV2

cmake_minimum_required(VERSION 3.1)

set (PACKAGE_VERSION "0.7.1")

set (CMAKE_SKIP_RULE_DEPENDENCY OFF)

add_definitions (-D'VERSION="${PACKAGE_VERSION}"')
add_definitions(-D'WEBSITE="https://gitlab.com/stazed/rakarrack-plus"')
add_definitions(-D'PACKAGE="rakarrack-plus"')
add_definitions(-D'DATADIR="${CMAKE_INSTALL_PREFIX}/share/rakarrack-plus"')
add_definitions(-D'HELPDIR="${CMAKE_INSTALL_PREFIX}/share/doc/rakarrack-plus"')

include (CheckCSourceCompiles)
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-msse2" SUPPORT_SSE)
check_cxx_compiler_flag("-ftree-vectorize" SUPPORT_VECTOR)
check_cxx_compiler_flag("-maltivec" SUPPORT_ALTIVEC)

option (BuildRakarrackPlus "Build rakarrack-plus" ON)
option (BuildLV2Plugins "Build rakarack lv2 plugins (rkrlv2)" ON)
option (BuildCarlaPresets "Build Carla presets for lv2 from default banks" OFF)
option (BuildForDebug "Include gdb debugging support" OFF)
option (EnableOptimizations "Enable optimizations" ON)
option (EnableSSE "Enable SSE (default=automatic check)" ON)
option (EnableSSE2 "Enable SSE2 (default=automatic check)" ON)
option (EnableVectorization "Enable vectorize" ON)
option (EnableAltivec "Enable altivec (default=automatic check)" ON)


set(CMAKE_BUILD_TYPE "Release")

set (BuildOptions_SSE
    "-msse -mfpmath=sse"
    CACHE STRING "SSE compiler options"
)

set (BuildOptions_SSE2
    "-msse -msse2 -mfpmath=sse"
    CACHE STRING "SSE2 compiler options"
)

set(BuildOptions_Vectorization
    "-ftree-vectorize -fvect-cost-model"
    CACHE STRING "Vectorization compiler options"
)

set(BuildOptions_Altivec
    "-maltivec"
    CACHE STRING "Altivec compiler options"
)

set (BuildOptionsBasic
    "-std=c++11 -Wno-unused-parameter -O3 -ffast-math -pipe -fsigned-char"
    CACHE STRING "basic X86 complier options"
)

set (BuildOptionsDebug
    "-std=c++11 -O0 -g3 -ggdb -Wall -Wextra -Wpointer-arith"
    CACHE STRING "Debug build flags"
)


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
    option (EnableJackSessionSupport "Include Jack Session Support" ON)
else (HasJackSession)
    set (EnableJackSessionSupport OFF)
endif (HasJackSession)

find_package (PkgConfig REQUIRED)

#dependencies
find_package (FLTK REQUIRED)
find_package (ALSA REQUIRED alsa>=1.0.17)
find_package(X11 REQUIRED)
find_package(Freetype REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET rakarrack-plus PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET rakarrack-plus PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

pkg_check_modules (JACK REQUIRED jack>=0.115.6)
pkg_check_modules (FONTCONFIG REQUIRED fontconfig>=0.22)
pkg_check_modules (FFTW3 REQUIRED fftw3>=0.22 fftw3f>=0.22)
pkg_check_modules(SNDFILE REQUIRED sndfile)
pkg_check_modules(SAMPLERATE REQUIRED samplerate>=0.1.5)
pkg_check_modules(XFT REQUIRED xft)
pkg_check_modules(XRENDER REQUIRED xrender)
pkg_check_modules(XPM REQUIRED xpm)

find_program(ACONNECT aconnect)
if (ACONNECT)
    message (STATUS "Found aconnect in ${ACONNECT}")
else (ACONNECT)
    message (FATAL_ERROR "Aconnect required but not found")
endif (ACONNECT)

find_library (LIBRT rt)
if (LIBRT)
    message(STATUS "Found rt in ${LIBRT}")
else (LIBRT)
    message (FATAL_ERROR "rt required but not found")
endif (LIBRT)

find_library (LIBDL dl)
if (LIBDL)
    message(STATUS "Found dl in ${LIBDL}")
else (LIBDL)
    message(FATAL_ERROR "dl required but not found")
endif (LIBDL)

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
mark_as_advanced (FLTK_DIR)
mark_as_advanced (FLTK_MATH_LIBRARY)
mark_as_advanced (LIBDL)
mark_as_advanced (LIBRT)

if(BuildRakarrackPlus)
    install(FILES COPYING AUTHORS ChangeLog NEWS README PACKAGERS.README
        DESTINATION share/doc/rakarrack-plus)
endif(BuildRakarrackPlus)
#TODO move from /data to main
#install(FILES rakarrack-plus-plus.desktop
#    DESTINATION share/applications)

if (BuildForDebug)
    set (CMAKE_BUILD_TYPE "Debug")
    set (CMAKE_CXX_FLAGS_DEBUG ${BuildOptionsDebug})
else (BuildForDebug)
    set (CMAKE_BUILD_TYPE "Release")

    set (CMAKE_CXX_FLAGS_RELEASE ${BuildOptionsBasic})

    if(EnableOptimizations)
        if (SUPPORT_SSE)
            if(EnableSSE)
                if(EnableSSE2)
                    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_SSE2}")
                    set (USE_SSE2 "${SUPPORT_SSE}")
                    set (USE_SSE "${SUPPORT_SSE}") #uses both
                else(EnableSSE2)
                    set (USE_SSE "${SUPPORT_SSE}")
                    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_SSE}")
                endif(EnableSSE2)
            endif(EnableSSE)
        endif (SUPPORT_SSE)

        if (SUPPORT_VECTOR)
            if(EnableVectorization)
                set (USE_VECTOR "${SUPPORT_VECTOR}")
                set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${BuildOptions_Vectorization}")
            endif(EnableVectorization)
        endif(SUPPORT_VECTOR)

        if (SUPPORT_ALTIVEC)
            if(EnableAltivec)
                set (USE_ALTIVEC "${SUPPORT_ALTIVEC}")
                set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} $[BuildOptions_Altivec}")
            endif(EnableAltivec)
        endif (SUPPORT_ALTIVEC)
    else(EnableOptimizations)
        set (USE_SSE2 "")
        set (USE_SSE "")
        set (USE_VECTOR "")
        set (USE_ALTIVEC "")
    endif(EnableOptimizations)
endif (BuildForDebug)

if(BuildRakarrackPlus)
    add_subdirectory(src)
    add_subdirectory(man)
    add_subdirectory(data)
    add_subdirectory(icons)
    add_subdirectory(doc)
    add_subdirectory(extra)
endif (BuildRakarrackPlus)

if (BuildLV2Plugins)
    pkg_check_modules(LV2 REQUIRED lv2>=1.0.0)
    if (LV2_FOUND)
        if (EXISTS $ENV{LV2_PATH})
            set(LV2_INSTALL_DIR $ENV{LV2_PATH} CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        else()
            set(LV2_INSTALL_DIR lib/lv2/RakarrackPlus.lv2 CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        endif()

        if(BuildCarlaPresets)
            message (STATUS "Building Carla presets...")
            execute_process(COMMAND ./convertbank.py ${CMAKE_INSTALL_PREFIX}/${LV2_INSTALL_DIR} WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/lv2)
        endif(BuildCarlaPresets)

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
        configure_file(data/11.dly lv2data/${NEWNAME}.dly COPYONLY)
        list(GET DLY 11 NEWNAME)
        configure_file(data/11.dly lv2data/${NEWNAME}.dly COPYONLY)

        # 1 more rvb file
        list(GET RVB 10 NEWNAME)
        configure_file(data/10.rvb lv2data/${NEWNAME}.rvb COPYONLY)

        add_subdirectory(lv2)
    endif (LV2_FOUND)
else (BuildLV2Plugins)
    set (BuildCarlaPresets OFF)
endif (BuildLV2Plugins)


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
package_status(ALSA_FOUND          "ALSA MIDI Sequencer support. . . . . . . . . . . . . . .:"  )
package_status(X11_FOUND           "X11 support. . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(Freetype_FOUND      "Freetype support . . . . . . . . . . . . . . . . . . . .:"  )
package_status(ZLIB_FOUND          "Zlib support . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(Threads_FOUND       "pthread support (libpthread) . . . . . . . . . . . . . .:"  )
package_status(JACK_FOUND          "JACK Audio Connection Kit library. . . . . . . . . . . .:"  )
package_status(FONTCONFIG_FOUND    "Fontconfig support . . . . . . . . . . . . . . . . . . .:"  )
package_status(FFTW3_FOUND         "Fast Fourier Transform (fftw3) . . . . . . . . . . . . .:"  )
package_status(SNDFILE_FOUND       "General audio file (libsndfile). . . . . . . . . . . . .:"  )
package_status(SAMPLERATE_FOUND    "Samplerate conversion (libsamplerate). . . . . . . . . .:"  )
package_status(XFT_FOUND           "Xft support. . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(XRENDER_FOUND       "Xrender support. . . . . . . . . . . . . . . . . . . . .:"  )
package_status(XPM_FOUND           "Xpm support (libxpm) . . . . . . . . . . . . . . . . . .:"  )

if(BuildLV2Plugins)
    package_status(LV2_FOUND           "LV2 support. . . . . . . . . . . . . . . . . . . . . . .:"  )
endif(BuildLV2Plugins)

message (STATUS)
message (STATUS                    "____________________ User Options________________________"  )
package_status(EnableOptimizations "Use optimizations. . . . . . . . . . . . . . . . . . . .:"  )
package_status(EnableVectorization "Vectorization. . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(EnableAltivec       "Use altiveck . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(EnableSSE           "Use sse. . . . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(EnableSSE2          "Use sse2 . . . . . . . . . . . . . . . . . . . . . . . .:"  )
package_status(BuildRakarrackPlus  "Build Rakarrack-plus . . . . . . . . . . . . . . . . . .:"  )
package_status(BuildLV2Plugins     "Build LV2s (RakarrackPlus.lv2) . . . . . . . . . . . . .:"  )
package_status(BuildCarlaPresets   "Build Carla lv2 presets. . . . . . . . . . . . . . . . .:"  )
package_status(BuildForDebug       "Build for debug. . . . . . . . . . . . . . . . . . . . .:"  )
package_status(EnableJackSessionSupport "JACK Session support . . . . . . . . . . . . . . . . . .:"  )

message (STATUS)
message (STATUS)
message (STATUS "Building for ${CMAKE_BUILD_TYPE}, flags: ${CMAKE_CXX_FLAGS_RELEASE}")