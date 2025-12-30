#CMake file for Rakarrack-plus

project (rakarrack-plus)

set(UI_SOURCES
    UI/rakarrack.cxx
    UI/analyzer.cxx
    UI/scope.cxx
    UI/newvum.cxx
    UI/tunerbar.cxx
    UI/common_gui_menu.cxx
    UI/RKR_GUI_Effect.cxx
    UI/RKR_Input.cxx
    UI/RKR_Box.cxx
    UI/RKR_Browser.cxx
    UI/RKR_Button.cxx
    UI/RKR_Check_Button.cxx
    UI/RKR_Choice.cxx
    UI/RKR_Counter.cxx
    UI/RKR_File_Input.cxx
    UI/RKR_Fl_Slider.cxx
    UI/RKR_Float_Input.cxx
    UI/RKR_Group.cxx
    UI/RKR_Key_Input.cxx
    UI/RKR_Light_Button.cxx
    UI/RKR_Menu_Bar.cxx
    UI/RKR_Slider.cxx
    UI/RKR_Scroll.cxx
    UI/RKR_Tabs.cxx
    UI/RKR_Value_Input.cxx
    UI/eq_gui.cxx
    UI/compress_gui.cxx
    UI/dist_gui.cxx
    UI/ovrd_gui.cxx
    UI/echo_gui.cxx
    UI/chorus_gui.cxx
    UI/phaser_gui.cxx
    UI/flanger_gui.cxx
    UI/reverb_gui.cxx
    UI/peq_gui.cxx
    UI/wahwah_gui.cxx
    UI/alienwah_gui.cxx
    UI/cabinet_gui.cxx
    UI/pan_gui.cxx
    UI/har_gui.cxx
    UI/musdelay_gui.cxx
    UI/gate_gui.cxx
    UI/derelict_gui.cxx
    UI/aphaser_gui.cxx
    UI/valve_gui.cxx
    UI/dflange_gui.cxx
    UI/ring_gui.cxx
    UI/exciter_gui.cxx
    UI/distband_gui.cxx
    UI/arpie_gui.cxx
    UI/expander_gui.cxx
    UI/shuffle_gui.cxx
    UI/synthfilter_gui.cxx
    UI/varyband_gui.cxx
    UI/convo_gui.cxx
    UI/looper_gui.cxx
    UI/mutromojo_gui.cxx
    UI/echoverse_gui.cxx
    UI/coil_gui.cxx
    UI/shelf_gui.cxx
    UI/vocoder_gui.cxx
    UI/sustain_gui.cxx
    UI/sequence_gui.cxx
    UI/shifter_gui.cxx
    UI/stompbox_gui.cxx
    UI/reverbtron_gui.cxx
    UI/ressol_gui.cxx
    UI/echotron_gui.cxx
    UI/stereoharm_gui.cxx
    UI/compband_gui.cxx
    UI/otrem_gui.cxx
    UI/vibe_gui.cxx
    UI/infinity_gui.cxx
    UI/midiconv_gui.cxx
    UI/bankwindow_gui.cxx
    UI/orderwindow_gui.cxx
    UI/settingswindow_gui.cxx
    UI/midilearnwindow_gui.cxx
    UI/aboutwindow_gui.cxx
    UI/trigwindow_gui.cxx
    UI/delayfilewindow_gui.cxx
    UI/randomedit_gui.cxx
    UI/rkrprocess_gui.cxx
    UI/Reset_Window.cpp
)

set(FX_SOURCES
    FX/Reverb.C
    FX/Chorus.C
    FX/Flanger.C
    FX/Effect.C
    FX/Echo.C
    FX/Phaser.C
    FX/Distorsion.C
    FX/Overdrive.C
    FX/EQ.C
    FX/Compressor.C
    FX/Alienwah.C
    FX/WahWah.C
    FX/Pan.C
    FX/Harmonizer.C
    FX/MusicDelay.C
    FX/Gate.C
    FX/Derelict.C
    FX/APhaser.C
    FX/Dual_Flange.C
    FX/Exciter.C
    FX/DistBand.C
    FX/Valve.C
    FX/Arpie.C
    FX/Expander.C
    FX/Ring.C
    FX/Shuffle.C
    FX/VaryBand.C
    FX/Convolotron.C
    FX/Synthfilter.C
    FX/Looper.C
    FX/MuTroMojo.C
    FX/Echoverse.C
    FX/CoilCrafter.C
    FX/ShelfBoost.C
    FX/Vocoder.C
    FX/Sustainer.C
    FX/Sequence.C
    FX/Shifter.C
    FX/StompBox.C
    FX/Reverbtron.C
    FX/ResSolution.C
    FX/Echotron.C
    FX/StereoHarm.C
    FX/CompBand.C
    FX/Opticaltrem.C
    FX/Vibe.C
    FX/Infinity.C
    FX/Cabinet.C
    FX/ParametricEQ.C
    FX/MIDIConverter.C
)

set(EFX_COMMON_SOURCES
    EFX_common/FPreset.C
    EFX_common/EffectLFO.C
    EFX_common/AnalogFilter.C
    EFX_common/beattracker.C
    EFX_common/delayline.C
    EFX_common/Filter.C
    EFX_common/HarmonicEnhancer.C
    EFX_common/SVFilter.C
    EFX_common/RBFilter.C
    EFX_common/FormantFilter.C
    EFX_common/FilterParams.C
    EFX_common/smbPitchShift.C
    EFX_common/pffft.C
    EFX_common/mayer_fft.C
    EFX_common/Resample.C
    EFX_common/Waveshaper.C
)

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

set(ProgSources
    ${CORE_SOURCES}
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
    ${X11_LIBRARIES}
    ${XFT_LIBRARIES}
    ${XRENDER_LIBRARIES}
    ${XPM_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${ZLIB_LIBRARIES}
    ${LIBLO_LIBRARIES}
    dl
    rt
    m
)

if (EnableKISSfft)
    list(APPEND ExternLibraries libkissfft-float.a)
endif (EnableKISSfft)

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
