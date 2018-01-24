
add_subdirectory(css)

install (FILES help.html
    aci.html 
    general.html
    effects.html
    midiconverter.html
    tuner.html 
    midilearn.html
    taptempo.html
    metronome.html
    hardware.html
    extra.html
    credits.html
    presetlist.html
    midiic.html
    license.html
    DESTINATION share/doc/rakarrack/html)


install (DIRECTORY imagenes/
    DESTINATION share/doc/rakarrack/html/imagenes)