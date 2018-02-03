#CMake file for Rakarrack

project (rakarrack)

if (EnableJackSessionSupport)
    add_definitions(-DJACK_SESSION=1)
endif (EnableJackSessionSupport)

set (ProgSources
    jack.C 
    UI/rakarrack.cxx
    UI/analyzer.cxx
    UI/scope.cxx
    UI/sliderW.cxx
    UI/newvum.cxx
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
    "${CMAKE_CURRENT_SOURCE_DIR}/UI"
    "${CMAKE_CURRENT_BINARY_DIR}/UI"
)

target_link_libraries (rakarrack ${ExternLibraries})

set_source_files_properties(MIDIConverter.C PROPERTIES COMPILE_FLAGS -std=c++98)
target_sources(rakarrack PRIVATE
  MIDIConverter.C
)

install (TARGETS rakarrack RUNTIME DESTINATION bin)
