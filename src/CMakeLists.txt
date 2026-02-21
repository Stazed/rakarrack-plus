#CMake file for Rakarrack-plus

project (rakarrack-plus)

set(CORE_SOURCES
    Config_fltk.C
    jack.C 
    strlcpy.C
    PresetBankStruct.C
    process.C
    fileio.C
    varios.C
    Limiter.C
    Tuner.C
    RecognizeNote.C
    RecChord.C
    rkrMIDI.C
    metronome.C
)

include(${CMAKE_SOURCE_DIR}/cmake/common_sources.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/common_ui_sources.cmake)

set(ProgSources
    ${CORE_SOURCES}
    FX/Looper.C
    ${UI_SOURCES}
    ${FX_SOURCES}
    ${EFX_COMMON_SOURCES}
)

set(ExternLibraries
    ${ALSA_LIBRARIES}
    ${JACK_LINK_LIBRARIES}
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
    ${LIBLO_LIBRARIES}
    ${DL_LIBRARY}
    ${RT_LIBRARY}
    ${M_LIBRARY}
)

add_executable (rakarrack-plus ${ProgSources} main.C)

if (EnableNSM)
    if (LIBLO_FOUND)
        target_compile_definitions(rakarrack-plus PRIVATE NSM_SUPPORT=1)
    endif (LIBLO_FOUND)
endif (EnableNSM)

if(EnableNTK)
    target_include_directories (
        rakarrack-plus PRIVATE
        ${NTK_INCLUDE_DIRS}
        ${ALSA_INCLUDE_DIRS}
        ${JACK_INCLUDE_DIRS}
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
        ${LIBLO_INCLUDE_DIRS}
        "${CMAKE_CURRENT_SOURCE_DIR}/UI"
        "${CMAKE_CURRENT_BINARY_DIR}/UI"
    )

else(EnableNTK) # Modern cmake
    if(TARGET fltk::fltk-shared)
        target_include_directories (
            rakarrack-plus PRIVATE
            ${ALSA_INCLUDE_DIRS}
            ${JACK_INCLUDE_DIRS}
            ${FFTW3_INCLUDE_DIRS}
            ${SNDFILE_INCLUDE_DIRS}
            ${SAMPLERATE_INCLUDE_DIRS}
            ${ZITA_RESAMPLER_INCLUDE_DIR}
            ${ZLIB_INCLUDE_DIRS}
            ${LIBLO_INCLUDE_DIRS}
            "${CMAKE_CURRENT_SOURCE_DIR}/UI"
            "${CMAKE_CURRENT_BINARY_DIR}/UI"
        )
    else()  # Old cmake
        target_include_directories (
            rakarrack-plus PRIVATE
            ${FLTK_INCLUDE_DIR}
            ${ALSA_INCLUDE_DIRS}
            ${JACK_INCLUDE_DIRS}
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
            ${LIBLO_INCLUDE_DIRS}
            "${CMAKE_CURRENT_SOURCE_DIR}/UI"
            "${CMAKE_CURRENT_BINARY_DIR}/UI"
    )
    endif()
endif(EnableNTK)

if(EnableNTK)
    target_link_libraries (rakarrack-plus PRIVATE ${NTK_IMAGES_LIBRARIES} ${NTK_LIBRARIES} ${ExternLibraries})
else(EnableNTK)
    if(TARGET fltk::fltk-shared)    # Modern cmake
        target_link_libraries (rakarrack-plus PRIVATE fltk::fltk-shared fltk::images ${ExternLibraries})
    else()                          # Old cmake
        target_link_libraries (rakarrack-plus PRIVATE ${FLTK_LIBRARIES} ${ExternLibraries})
    endif()
endif(EnableNTK)

if(EnableNTK)
    target_compile_definitions(rakarrack-plus PRIVATE NTK_SUPPORT=1)
endif(EnableNTK)

install (TARGETS rakarrack-plus RUNTIME DESTINATION bin)
