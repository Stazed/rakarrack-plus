#CMake file for Rakarrack & LV2

cmake_minimum_required(VERSION 2.8)

set (VERSION "0.7.0")

add_definitions (-D'VERSION="${VERSION}"')

add_definitions(-DCMAKE_BUILD=1)
add_definitions(-D'WEBSITE="rakarrack.sf.net"')
add_definitions(-D'PACKAGE="rakarrack"')
add_definitions(-D'DATADIR="${CMAKE_INSTALL_PREFIX}/share/rakarrack"')
add_definitions(-D'HELPDIR="${CMAKE_INSTALL_PREFIX}/share/doc/rakarrack"')

include (CheckCSourceCompiles)

option (BuildForDebug "Include gdb debugging support" OFF)
option (LV2Plugin "Build rakarack lv2 plugin interface" ON)
option (BuildCarlaPresets "Build Carla .carxp for lv2 from default banks" ON)
#option (optimizations "Enable optimizations" ON)
#option (vectorize "Enable vectorize" ON)
#option (sse "Enable sse (default=automatic check)" ON)
#option (sse2 "Enable sse2 (default=automatic check)" ON)
#option (altivec "Enable altivec (default=automatic check)" ON)

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

# uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


add_subdirectory(src)
add_subdirectory(man)
add_subdirectory(data)
add_subdirectory(icons)
add_subdirectory(doc)
add_subdirectory(extra)

#include(GNUInstallDirs)
#mark_as_advanced (EXECUTABLE_OUTPUT_PATH)
#mark_as_advanced (LIBRARY_OUTPUT_PATH)
#mark_as_advanced (CMAKE_BUILD_TYPE)
#mark_as_advanced (CMAKE_INSTALL_PREFIX)

install(FILES COPYING AUTHORS ChangeLog NEWS README PACKAGERS.README
    DESTINATION share/doc/rakarrack)

#TODO move from /data to main
#install(FILES rakarrack.desktop
#    DESTINATION share/applications)


if (LV2Plugin)
    pkg_check_modules(LV2 REQUIRED lv2>=1.0.0)
    if (LV2_FOUND)
        message (STATUS "Found lv2 package ${LV2_VERSION}")

        if (EXISTS $ENV{LV2_PATH})
            set(LV2_INSTALL_DIR $ENV{LV2_PATH} CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        else()
            set(LV2_INSTALL_DIR lib/lv2/rkr.lv2 CACHE PATH
            "Specifies where the LV2 libraries should be installed")
        endif()

        if(BuildCarlaPresets)
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

    else (LV2_FOUND)
        message (FATAL_ERROR "lv2 package required but not found (version 1.0.0 needed)")
    endif (LV2_FOUND)
endif (LV2Plugin)
