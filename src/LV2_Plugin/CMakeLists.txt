project (Rakarrack-Plus)

set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/Config_fltk.C
    ${CMAKE_SOURCE_DIR}/src/jack.C 
    ${CMAKE_SOURCE_DIR}/src/strlcpy.C
    ${CMAKE_SOURCE_DIR}/src/PresetBankStruct.C
    ${CMAKE_SOURCE_DIR}/src/process.C
    ${CMAKE_SOURCE_DIR}/src/fileio.C
    ${CMAKE_SOURCE_DIR}/src/varios.C
    ${CMAKE_SOURCE_DIR}/src/Limiter.C
    ${CMAKE_SOURCE_DIR}/src/Tuner.C
    ${CMAKE_SOURCE_DIR}/src/RecognizeNote.C
    ${CMAKE_SOURCE_DIR}/src/RecChord.C
    ${CMAKE_SOURCE_DIR}/src/rkrMIDI.C
    ${CMAKE_SOURCE_DIR}/src/metronome.C
)

include(${CMAKE_SOURCE_DIR}/cmake/common_sources.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/common_ui_sources.cmake)

set(ProgSources
    RakarrackPlusLV2.cpp
    RakarrackPlusLV2UI.cpp
    ${CMAKE_SOURCE_DIR}/src/FX/Looper.C
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${FX_SOURCES}
    ${EFX_COMMON_SOURCES}
)

set(ExternLibraries
    ${NTK_IMAGES_LIBRARIES}
    ${NTK_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${FFTW3_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    ${SNDFILE_LIBRARIES}
    ${SAMPLERATE_LIBRARIES}
    ${ZITA_RESAMPLER_LIBRARY}
    ${X11_LIBRARIES}
    ${XFT_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XPM_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${DL_LIBRARY}
    ${RT_LIBRARY}
    ${M_LIBRARY}
)

add_library(Rakarrack-Plus SHARED
    ${ProgSources}
)

target_include_directories (
    Rakarrack-Plus PRIVATE
    ${NTK_INCLUDE_DIRS}
    ${FONTCONFIG_INCLUDE_DIRS}
    ${FFTW3_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${SAMPLERATE_INCLUDE_DIRS}
    ${ZITA_RESAMPLER_INCLUDE_DIR}
    ${X11_INCLUDE_DIRS}
    ${XFT_INCLUDE_DIRS}
    ${XRENDER_INCLUDE_DIRS}
    ${XPM_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}../UI"
    "${CMAKE_CURRENT_BINARY_DIR}../UI"
)

target_link_libraries (Rakarrack-Plus PRIVATE ${ExternLibraries})

target_compile_definitions(Rakarrack-Plus PRIVATE RKR_PLUS_LV2=1)
target_compile_definitions(Rakarrack-Plus PRIVATE NTK_SUPPORT=1)
set_target_properties(Rakarrack-Plus PROPERTIES PREFIX "")
set_target_properties(Rakarrack-Plus PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE )

if (BUILD_RPLUS_LV2)
    install(TARGETS Rakarrack-Plus
      LIBRARY
      DESTINATION ${LV2_INSTALL_DIR_RKRP} )

    install (FILES Rakarrack-Plus.ttl manifest.ttl
        DESTINATION ${LV2_INSTALL_DIR_RKRP} )
endif (BUILD_RPLUS_LV2)