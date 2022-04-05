#CMake file for LV2 Versions of Rakarrack-plus Effects

project (rakarrack-plus-lv2)

# config libraries

add_library(rakarrack-plus-lv2 SHARED
  rkrlv2.C
  ../src/FX/Effect.C
  ../src/FX/EQ.C
  ../src/EFX_common/AnalogFilter.C
  ../src/FX/Compressor.C
  ../src/Limiter.C
  ../src/FX/Distorsion.C
  ../src/FX/Overdrive.C
  ../src/EFX_common/Waveshaper.C
  ../src/EFX_common/Resample.C
  ../src/EFX_common/FPreset.C
  ../src/FX/Echo.C
  ../src/EFX_common/delayline.C
  ../src/FX/Chorus.C
  ../src/FX/Flanger.C
  ../src/EFX_common/EffectLFO.C
  ../src/FX/APhaser.C
  ../src/FX/Harmonizer.C
  ../src/RecognizeNote.C
  ../src/RecChord.C
  ../src/FX/Sustainer.C
  ../src/EFX_common/smbPitchShift.C
  ../src/FX/Exciter.C
  ../src/EFX_common/HarmonicEnhancer.C
  ../src/FX/Pan.C
  ../src/FX/Alienwah.C
  ../src/FX/Reverb.C
  ../src/FX/Cabinet.C
  ../src/FX/MusicDelay.C
  ../src/FX/WahWah.C
  ../src/EFX_common/Filter.C
  ../src/EFX_common/FilterParams.C
  ../src/EFX_common/SVFilter.C
  ../src/EFX_common/FormantFilter.C
  ../src/FX/Derelict.C
  ../src/FX/Valve.C
  ../src/FX/Dual_Flange.C
  ../src/FX/Ring.C
  ../src/FX/DistBand.C
  ../src/FX/Arpie.C
  ../src/FX/Expander.C
  ../src/FX/Shuffle.C
  ../src/FX/Synthfilter.C
  ../src/FX/VaryBand.C
  ../src/EFX_common/RBFilter.C
  ../src/FX/MuTroMojo.C
  ../src/FX/Echoverse.C
  ../src/FX/CoilCrafter.C
  ../src/FX/ShelfBoost.C
  ../src/FX/Vocoder.C
  ../src/FX/Sequence.C
  ../src/EFX_common/beattracker.C
  ../src/FX/Shifter.C
  ../src/FX/StompBox.C
  ../src/FX/Reverbtron.C
  ../src/FX/Echotron.C
  ../src/FX/StereoHarm.C
  ../src/FX/CompBand.C
  ../src/FX/Opticaltrem.C
  ../src/FX/Vibe.C
  ../src/FX/Infinity.C
  ../src/FX/Phaser.C
  ../src/FX/Gate.C
  ../src/EFX_common/mayer_fft.C
  ../src/FX/Convolotron.C
  ../src/FX/ParametricEQ.C
)

set_source_files_properties(../src/FX/MIDIConverter.C PROPERTIES COMPILE_FLAGS -std=c++98)

target_sources(rakarrack-plus-lv2 PRIVATE
  ../src/FX/MIDIConverter.C
)

target_compile_definitions(rakarrack-plus-lv2 PRIVATE LV2_SUPPORT=1)

set_target_properties(rakarrack-plus-lv2 PROPERTIES PREFIX "")

set_target_properties(rakarrack-plus-lv2 PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

target_include_directories (rakarrack-plus-lv2 PRIVATE ${LV2_INCLUDE_DIRS} ${SAMPLERATE_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${SNDFILE_INCLUDE_DIRS} ../src)
target_link_libraries(rakarrack-plus-lv2 PRIVATE ${LV2_LIBRARIES} ${SAMPLERATE_LIBRARIES} ${FFTW3_LIBRARIES} ${SNDFILE_LIBRARIES} )

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

if(BuildCarlaPresets)
    install (DIRECTORY presets/
        DESTINATION share/RakarrackPlus.lv2
    )
endif(BuildCarlaPresets)
