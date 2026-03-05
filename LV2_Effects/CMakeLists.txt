#CMake file for LV2 Versions of Rakarrack-plus Effects

project (rakarrack-plus-lv2)

# config libraries

set(LV2_SOURCES
    rkrlv2.C
    ${CMAKE_SOURCE_DIR}/src/strlcpy.C
    ${CMAKE_SOURCE_DIR}/src/Limiter.C
    ${CMAKE_SOURCE_DIR}/src/RecognizeNote.C
    ${CMAKE_SOURCE_DIR}/src/RecChord.C
)

include(${CMAKE_SOURCE_DIR}/cmake/common_sources.cmake)

add_library(rakarrack-plus-lv2 SHARED
    ${LV2_SOURCES}
    ${FX_SOURCES}
    ${EFX_COMMON_SOURCES}
)

target_compile_definitions(rakarrack-plus-lv2 PRIVATE LV2_SUPPORT=1)

set_target_properties(rakarrack-plus-lv2 PROPERTIES PREFIX "")

set_target_properties(rakarrack-plus-lv2 PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

if (ENABLE_PFFFT)
    target_include_directories (rakarrack-plus-lv2 PRIVATE ${LV2_INCLUDE_DIRS} ${SAMPLERATE_INCLUDE_DIRS} ${ZITA_RESAMPLER_INCLUDE_DIR} ${SNDFILE_INCLUDE_DIRS} ../src)
    target_link_libraries(rakarrack-plus-lv2 PRIVATE ${LV2_LIBRARIES} ${SAMPLERATE_LIBRARIES} ${ZITA_RESAMPLER_LIBRARY} ${SNDFILE_LIBRARIES} )
else (ENABLE_PFFFT)
    target_include_directories (rakarrack-plus-lv2 PRIVATE ${LV2_INCLUDE_DIRS} ${SAMPLERATE_INCLUDE_DIRS} ${ZITA_RESAMPLER_INCLUDE_DIR} ${FFTW3_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ../src)
    target_link_libraries(rakarrack-plus-lv2 PRIVATE ${LV2_LIBRARIES} ${SAMPLERATE_LIBRARIES} ${ZITA_RESAMPLER_LIBRARY} ${FFTW3_LIBRARIES} ${SNDFILE_LIBRARIES} )
endif (ENABLE_PFFFT)

# config install
install(TARGETS rakarrack-plus-lv2
  LIBRARY
  DESTINATION ${LV2_INSTALL_DIR}
)

install (DIRECTORY ttl/
  DESTINATION ${LV2_INSTALL_DIR}
)

#Data files needed for default settings and presets. They must be included in the lv2 bundle folder.
#We copy all the data files here, but only those indicated by the presets & defaults are actually needed.
#Rakarrack has a pop up 'preset' selector that gives access to all the data files. This feature is
#not implemented in the lv2s. Instead the lv2s must use the user file lookup.
install (DIRECTORY ${CMAKE_BINARY_DIR}/lv2data/
  DESTINATION ${LV2_INSTALL_DIR}
)

#This is a convenience for the user to easily access the data files from the user file lookup
#which are also be copied to the lv2 bundle folder above. 
install (DIRECTORY ${CMAKE_BINARY_DIR}/lv2data/
  DESTINATION share/RakarrackPlus.lv2
)

if(BUILD_CARLA_PRESETS)
    install (DIRECTORY presets/
        DESTINATION share/RakarrackPlus.lv2
    )
endif(BUILD_CARLA_PRESETS)
