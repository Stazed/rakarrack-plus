/* tuneit.c -- Detect fundamental frequency of a sound
* Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
*
* Modified for rakarrack by Josep Andreu
* MIDIConverter.h  MIDIConverter definitions
*
* This is free software, placed under the terms of the
* GNU General Public License, as published by the Free Software Foundation.
* Please see the file COPYING for details.
*/


#ifndef MIDICONVERTER_H_
#define MIDICONVERTER_H_

#include <math.h>
#include <stdlib.h>
#include <complex.h>
#include <fftw3.h>

class RKR;          // Forward declaration

#ifdef LV2RUN

#include "lv2/lv2plug.in/ns/ext/midi/midi.h"
#include"../lv2/rkrlv2.h"

struct _RKRLV2;     // Forward declaration

#else
#include <jack/midiport.h>
#include <alsa/asoundlib.h>
#include "process.h"

struct Midi_Event {
    jack_nframes_t  time;
    int             len;    /* Length of MIDI message, in bytes. */
    jack_midi_data_t  *dataloc;
} ;
#endif // LV2RUN

class MIDIConverter
{
public:
    MIDIConverter (char *jname, RKR *_rkr, double sample_rate, uint32_t intermediate_bufsize);
    ~MIDIConverter ();

    void out (float * efxoutl, float * efxoutr);

    signed short int *schmittBuffer;
    signed short int *schmittPointer;
    const char **notes;
    int note;
    float nfreq, afreq, freq;
    float TrigVal;
    int cents;
    void schmittFloat (float * efxoutl, float * efxoutr, float val_sum, float *freqs, float *lfreqs);
    void fftFloat (float * efxoutl, float * efxoutr, float val_sum, float *freqs, float *lfreqs);
    void setmidichannel (int channel);
    void panic ();
    void setTriggerAdjust (int val);
    void setVelAdjust (int val);
    void setOctAdjust(int val);
    void setGain(int val);

    void changepar (int npar, int value);
    int getpar (int npar);
    void cleanup ();
    void lv2_update_params(uint32_t period);

#ifdef LV2RUN
    void update_freqs(float val);
#endif // LV2RUN
    
    int channel;
    int lanota;
    int nota_actual;
    int hay;
    int preparada;
    int ponla;
    int velocity;
    int Moctave;
    uint32_t PERIOD;

    float VelVal;
    uint8_t        midi_ON_msg[3];
    uint8_t        midi_OFF_msg[3];
    RKR *rkr;       // for passed jack port buffer

#ifdef LV2RUN
    _RKRLV2* plug; // for access to forge_midimessage()
#else
    jack_midi_data_t  moutdata_ON[2048];
    jack_midi_data_t  moutdata_OFF[2048];
    Midi_Event Midi_event_ON[2048];
    Midi_Event Midi_event_OFF[2048];
    snd_seq_t *port;
    void *dataout_ON, *dataout_OFF;
#endif // LV2RUN

private:

    void displayFrequency (float freq, float val_sum, float *freqs, float *lfreqs);
    void schmittInit (int size);
    void schmittS16LE (signed short int *indata, float val_sum, float *freqs, float *lfreqs);
    void schmittFree ();

    //FFT
    void fftInit (int size);
    void fftMeasure (int overlap, float *indata, float val_sum, float *freqs, float *lfreqs);
    void fftS16LE (signed short int *indata, float val_sum, float *freqs, float *lfreqs);
    void fftFree ();

    void MIDI_Send_Note_On (int note, float val_sum);
    void MIDI_Send_Note_Off (int note);

    unsigned int blockSize;

    unsigned int SAMPLE_RATE;
    float fSAMPLE_RATE;
    
    float Input_Gain;       // lv2 only
#ifdef LV2RUN
    float FREQS[12];
    float LFREQS[12];
    float VAL_SUM;
    float old_il_sum; // -50.0
    float old_ir_sum; // -50.0
    float val_il_sum;
    float val_ir_sum;
#endif // LV2RUN
    
    //Parametrii
    int Pgain;              // lv2 only
    int Pmidi;
    int Poctave;
    int Ppanic;
    int Pvelocity;
    int Ptrigger;
    int Pfft;

    float *fftSampleBuffer;
    float *fftSample;
    float *fftLastPhase;
    int fftSize;
    int fftFrameCount;
    float *fftIn;
    fftwf_complex *fftOut;
    fftwf_plan fftPlan;

    typedef struct {
        double freq;
        double db;
    } Peak;

};

#endif /*MIDICONVERTER_H_ */
