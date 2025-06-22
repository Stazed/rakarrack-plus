/* tuneit.c -- Detect fundamental frequency of a sound
 * Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
 *
 * Modified for rakarrack by Josep Andreu
 * MIDIConverter.C  MIDI Converter class
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software Foundation.
 * Please see the file COPYING for details.
 */
#include "MIDIConverter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef nullptr
// needed for c++98 with fltk > 1.3.9
#define nullptr 0
#endif

#include "../global.h"

#define M_PI 3.14159265358979323846
#define MAX_FFT_LENGTH 48000
#define MAX_PEAKS 8

static pthread_mutex_t fftw_planner_lock = PTHREAD_MUTEX_INITIALIZER;

MIDIConverter::MIDIConverter(char *jname, double sample_rate, uint32_t intermediate_bufsize):
    schmittBuffer(NULL),
    schmittPointer(NULL),
    notes(NULL),
    note(),
    nfreq(),
    afreq(),
    freq(),
    TrigVal(.25f),
    cents(),
    channel(),
    lanota(-1),
    nota_actual(-1),
    hay(),                          /* This is used for the red light on/off display */
    preparada(),
    ponla(),
    velocity(100),
    Moctave(),
    PERIOD(intermediate_bufsize),   /* Correct for rakarrack, may be adjusted by lv2 */
    VelVal(),
    midi_Note_Message(),
    blockSize(),
    SAMPLE_RATE(sample_rate),
    fSAMPLE_RATE((float) sample_rate),
#ifdef LV2_SUPPORT
    plug(NULL),
    Input_Gain(0.50f),
    FREQS(),
    LFREQS(),
    VAL_SUM(-50.0f),
    old_il_sum(-50.0f),
    old_ir_sum(-50.0f),
    val_il_sum(-50.0f),
    val_ir_sum(-50.0f),
    Pgain(64),
#else
#ifndef RKR_PLUS_LV2
    /* Jack */
    m_buffSize(NULL),
    m_buffMessage(NULL),
    /* Alsa */
    port(NULL),
#else
    plug(NULL),
#endif
#endif // LV2_SUPPORT
    Pmidi(),
    Poctave(),
    Ppanic(),
    Pvelocity(),
    Ptrigger(),
    Pfft(),
    fftSampleBuffer(NULL),
    fftSample(NULL),
    fftLastPhase(NULL),
    fftSize(),
    fftFrameCount(),
    fftIn(NULL),
    fftOut(NULL),
    fftPlan()
{
    static const char *englishNotes[12] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    notes = englishNotes;

    schmittInit(32); // 32 == latency (tuneit default = 10)
#ifndef RKR_PLUS_LV2
    fftInit(32); // == latency
#endif

    char portname[50]; // used by alsa - put here to avoid unused variable compiler warning on jname
    snprintf(portname, sizeof(portname), "%s MC OUT", jname); // used by alsa - put here to avoid unused variable compiler warning on jname

#ifdef LV2_SUPPORT
    update_freqs(440.0f);
#else
#ifndef RKR_PLUS_LV2
    // Open Alsa Seq
    int err = snd_seq_open(&port, "default", SND_SEQ_OPEN_OUTPUT, 0);

    if (err < 0)
        printf("Cannot activate ALSA seq client\n");

    snd_seq_set_client_name(port, jname);
    snd_config_update_free_global();

    // Create Alsa Seq Client
    snd_seq_create_simple_port(port, portname,
                               SND_SEQ_PORT_CAP_READ |
                               SND_SEQ_PORT_CAP_SUBS_READ,
                               SND_SEQ_PORT_TYPE_APPLICATION);

#endif // ifndef RKR_PLUS_LV2
#endif // LV2_SUPPORT
}

MIDIConverter::~MIDIConverter()
{
    schmittFree();
#ifndef RKR_PLUS_LV2
    fftFree();
#endif
#ifndef LV2_SUPPORT
#ifndef RKR_PLUS_LV2
    if(port)
    {
        snd_seq_close(port);
    }
#endif
#endif // LV2_SUPPORT
}

#ifdef LV2_SUPPORT
void
MIDIConverter::out(float * efxoutl, float * efxoutr)
{
    float il_sum = 1e-12f;
    float ir_sum = 1e-12f;
    float Log_I_Gain = powf(Input_Gain * 2.0f, 4);

    for (unsigned i = 0; i < PERIOD; i++)
    {
        efxoutl[i] *= Log_I_Gain;
        efxoutr[i] *= Log_I_Gain;
        float tmp = fabsf(efxoutr[i]);

        if (tmp > ir_sum) ir_sum = tmp;

        tmp = fabsf(efxoutl[i]);

        if (tmp > il_sum) il_sum = tmp;
    }

    if (val_il_sum != old_il_sum)
    {
        old_il_sum = val_il_sum;
    }

    if (val_ir_sum != old_ir_sum)
    {
        old_ir_sum = val_ir_sum;
    }

    float temp_sum = (float) CLAMP(rap2dB(il_sum), -48.0, 15.0);
    val_il_sum = .6f * old_il_sum + .4f * temp_sum;

    temp_sum = (float) CLAMP(rap2dB(ir_sum), -48.0, 15.0);
    val_ir_sum = .6f * old_ir_sum + .4f * temp_sum;

    VAL_SUM = val_il_sum + val_ir_sum;

    if (Pfft)
        fftFloat(efxoutl, efxoutr, VAL_SUM, FREQS, LFREQS);
    else
        schmittFloat(efxoutl, efxoutr, VAL_SUM, FREQS, LFREQS);
}
#endif // LV2_SUPPORT

void
MIDIConverter::cleanup()
{
    // nothing
}

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
void
MIDIConverter::lv2_update_params(uint32_t period)
{
    PERIOD = period;
}
#endif // LV2_SUPPORT

void
MIDIConverter::displayFrequency(float ffreq, float val_sum, float *freqs, float *lfreqs)
{
    int noteoff = 0;
    int octave = 4;

    if (ffreq < 1E-15)
        ffreq = 1E-15f;

    float lfreq = logf(ffreq);

    while (lfreq < lfreqs[0] - LOG_D_NOTE * .5f)
    {
        lfreq += LOG_2;
    }

    while (lfreq >= lfreqs[0] + LOG_2 - LOG_D_NOTE * .5f)
    {
        lfreq -= LOG_2;
    }

    float mldf = LOG_D_NOTE;
    float ldf = 0.0;
    
    for (int i = 0; i < 12; i++)
    {
        ldf = fabsf(lfreq - lfreqs[i]);

        if (ldf < mldf)
        {
            mldf = ldf;
            note = i;
        }
    }

    nfreq = freqs[note];

    while (nfreq / ffreq > D_NOTE_SQRT)
    {
        nfreq *= .5f;
        octave--;

        if (octave < -2)
        {
            noteoff = 1;
            break;
        }

    }

    while (ffreq / nfreq > D_NOTE_SQRT)
    {
        nfreq *= 2.0f;
        octave++;

        if (octave > 9)
        {
            noteoff = 1;
            break;
        }
    }

    cents = lrintf(1200.0f * (logf(ffreq / nfreq) / LOG_2));
    lanota = 24 + (octave * 12) + note - 3;

    if ((noteoff) & (hay))
    {
        send_Midi_Note(nota_actual, 0, false); // false = note off: 0 is not used for note off
        hay = 0;
        nota_actual = -1;
    }

    if ((preparada == lanota) && (lanota != nota_actual))
    {
        hay = 1;

        if (nota_actual != -1)
        {
            send_Midi_Note(nota_actual, 0, false); // false = note off: 0 is not used for note off
        }

        send_Midi_Note(lanota, val_sum, true); // true = note on
        nota_actual = lanota;
    }

    if ((lanota > 0 && lanota < 128) && (lanota != nota_actual))
    {
        preparada = lanota;
    }
}

void
MIDIConverter::schmittInit(int size)
{
    blockSize = SAMPLE_RATE / size;
    schmittBuffer =  (signed short int *) malloc(sizeof (signed short int) * (blockSize + 1));

    memset(schmittBuffer, 0, sizeof (signed short int) * (blockSize + 1));
    schmittPointer = schmittBuffer;
}

void
MIDIConverter::schmittS16LE(signed short int *indata, float val_sum, float *freqs, float *lfreqs)
{
    unsigned int j = 0;
    float trigfact = 0.6f;

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        *schmittPointer++ = indata[i];

        if (schmittPointer - schmittBuffer >= blockSize)
        {
            schmittPointer = schmittBuffer;
            
            int  A1, A2; A1 = A2 = 0;            
            for (j = 0, A1 = 0, A2 = 0; j < blockSize; j++)
            {
                if (schmittBuffer[j] > 0 && A1 < schmittBuffer[j])
                {
                    A1 = schmittBuffer[j];
                }

                if (schmittBuffer[j] < 0 && A2 < -schmittBuffer[j])
                {
                    A2 = -schmittBuffer[j];
                }
            }

            int t1 = lrintf((float) A1 * trigfact + 0.5f);
            int t2 = -lrintf((float) A2 * trigfact + 0.5f);

            for (j = 1; j < blockSize && schmittBuffer[j] <= t1; j++);

            for (; j < blockSize && !(schmittBuffer[j] >= t2 &&
                 schmittBuffer[j + 1] < t2) && j; j++);

            int startpoint = j;
            int schmittTriggered = 0;
            int endpoint = startpoint + 1;
            int tc = 0;

            for (j = startpoint, tc = 0; j < blockSize; j++)
            {
                if (!schmittTriggered)
                {
                    schmittTriggered = (schmittBuffer[j] >= t1);
                }
                else if (schmittBuffer[j] >= t2 && schmittBuffer[j + 1] < t2)
                {
                    endpoint = j;
                    tc++;
                    schmittTriggered = 0;
                }
            }

            if (endpoint > startpoint)
            {
                afreq = fSAMPLE_RATE * ((float) tc / (float) (endpoint - startpoint));
                displayFrequency(afreq, val_sum, freqs, lfreqs);
            }
        }
    }
}

void
MIDIConverter::schmittFree()
{
    free(schmittBuffer);
}

void
MIDIConverter::schmittFloat(float * efxoutl, float * efxoutr, float val_sum, float *freqs, float *lfreqs)
{
    signed short int buf[PERIOD];

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        buf[i] = (short) ((TrigVal * efxoutl[i] + TrigVal * efxoutr[i]) * 32768);
    }
    schmittS16LE(buf, val_sum, freqs, lfreqs);
}

void
MIDIConverter::fftInit(int size)
{
    fftSize = 2 + (SAMPLE_RATE / size);
    fftIn = (float*) fftwf_malloc(sizeof (float) * 2 * (fftSize / 2 + 1));
    fftOut = (fftwf_complex *) fftIn;
    pthread_mutex_lock (&fftw_planner_lock);
    fftPlan = fftwf_plan_dft_r2c_1d(fftSize, fftIn, fftOut, FFTW_MEASURE);
    pthread_mutex_unlock (&fftw_planner_lock);

    fftSampleBuffer = (float *) malloc(fftSize * sizeof (float));
    fftSample = NULL;
    fftLastPhase = (float *) malloc((fftSize / 2 + 1) * sizeof (float));
    memset(fftSampleBuffer, 0, fftSize * sizeof (float));
    memset(fftLastPhase, 0, (fftSize / 2 + 1) * sizeof (float));
}

void
MIDIConverter::fftMeasure(int overlap, float *indata, float val_sum, float *freqs, float *lfreqs)
{
    int stepSize = fftSize / overlap;
    double freqPerBin = SAMPLE_RATE / (double) fftSize,
            phaseDifference = 2. * M_PI * (double) stepSize / (double) fftSize;

    if (!fftSample) fftSample = fftSampleBuffer + (fftSize - stepSize);

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        *fftSample++ = indata[i];

        if (fftSample - fftSampleBuffer >= fftSize)
        {
            int k = 0;
            Peak peaks[MAX_PEAKS];

            for (k = 0; k < MAX_PEAKS; k++)
            {
                peaks[k].db = -200.;
                peaks[k].freq = 0.;
            }

            fftSample = fftSampleBuffer + (fftSize - stepSize);

            for (k = 0; k < fftSize; k++)
            {
                double window = -.5 * cos(2. * M_PI * (double) k / (double) fftSize) + .5;
                fftIn[k] = fftSampleBuffer[k] * window;
            }

            fftwf_execute(fftPlan);

            for (k = 0; k <= fftSize / 2; k++)
            {
                float real = creal(fftOut[k]), // This requires -std=gnu++98 
                        imag = cimag(fftOut[k]), // This requires -std=gnu++98 
                        magnitude = 20. * log10(2. * sqrt(real * real + imag * imag) / fftSize),
                        phase = atan2(imag, real),
                        tmp, freq;

                /* compute phase difference */
                tmp = phase - fftLastPhase[k];
                fftLastPhase[k] = phase;

                /* subtract expected phase difference */
                tmp -= (double) k*phaseDifference;

                /* map delta phase into +/- Pi interval */
                long qpd = tmp / M_PI;

                if (qpd >= 0) qpd += qpd & 1;
                else qpd -= qpd & 1;

                tmp -= M_PI * (double) qpd;

                /* get deviation from bin frequency from the +/- Pi interval */
                tmp = overlap * tmp / (2. * M_PI);

                /* compute the k-th partials' true frequency */
                freq = (double) k * freqPerBin + tmp*freqPerBin;

                if (freq > 0.0 && magnitude > peaks[0].db)
                {
                    memmove(peaks + 1, peaks, sizeof (Peak)*(MAX_PEAKS - 1));
                    peaks[0].freq = freq;
                    peaks[0].db = magnitude;
                }
            }

            fftFrameCount++;

            if (fftFrameCount > 0 && fftFrameCount % overlap == 0)
            {
                int maxharm = 0;
                k = 0;

                for (int l = 1; l < MAX_PEAKS && peaks[l].freq > 0.0; l++)
                {
                    int harmonic;

                    for (harmonic = 5; harmonic > 1; harmonic--)
                    {
                        if (peaks[0].freq / peaks[l].freq < harmonic + .02 &&
                            peaks[0].freq / peaks[l].freq > harmonic - .02)
                        {
                            if (harmonic > maxharm &&
                                peaks[0].db < peaks[l].db / 2)
                            {
                                maxharm = harmonic;
                                k = l;
                            }
                        }
                    }
                }
                displayFrequency(peaks[k].freq, val_sum, freqs, lfreqs);
            }
            memmove(fftSampleBuffer, fftSampleBuffer + stepSize, (fftSize - stepSize) * sizeof (float));
        }
    }
}

void
MIDIConverter::fftFloat(float *efxoutl, float *efxoutr, float val_sum, float *freqs, float *lfreqs)
{
    signed short int buf[PERIOD];

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        buf[i] =  (short) ((TrigVal * efxoutl[i] + TrigVal * efxoutr[i]) * 32768);
    }

    fftS16LE(buf, val_sum, freqs, lfreqs);
}

void
MIDIConverter::fftS16LE(signed short int *indata, float val_sum, float *freqs, float *lfreqs)
{
    float buf[PERIOD];

    for (unsigned int i = 0; i < PERIOD; i++)
    {
        buf[i] = indata[i] / 32768.;
    }

    fftMeasure(4, buf, val_sum, freqs, lfreqs);
}

void
MIDIConverter::fftFree()
{
    pthread_mutex_lock (&fftw_planner_lock);
    fftwf_destroy_plan(fftPlan);
    pthread_mutex_unlock (&fftw_planner_lock);
    fftwf_free(fftIn);
    free(fftSampleBuffer);
    free(fftLastPhase);
}

void
MIDIConverter::send_Midi_Note(uint nota, float val_sum, bool is_On)
{
    const uint8_t anota = (uint8_t) nota + (Moctave * 12);

    if (anota > 127) return;

    if (is_On) // Note On
    {
        int k = lrintf((val_sum + 96) * 2);

        if (k > 127)
            k = 127;

        if (k < 1)
            k = 1;

        velocity = lrintf((float) k / VelVal);
    }
    else // Note Off
    {
        velocity = 64;
    }

    if (is_On)
        midi_Note_Message[0] = EVENT_NOTE_ON + channel;
    else
        midi_Note_Message[0] = EVENT_NOTE_OFF + channel;

    midi_Note_Message[1] = anota;
    midi_Note_Message[2] = velocity;

#if defined LV2_SUPPORT || defined RKR_PLUS_LV2
    if(plug)
        forge_midimessage(plug, 0, midi_Note_Message, 3);
#else
    // ALSA
    snd_seq_event_t ev;
    snd_seq_ev_clear(&ev);

    if (is_On)
        snd_seq_ev_set_noteon(&ev, channel, anota, velocity);
    else
        snd_seq_ev_set_noteoff(&ev, channel, anota, velocity);

    snd_seq_ev_set_subs(&ev);
    snd_seq_ev_set_direct(&ev);
    snd_seq_event_output_direct(port, &ev);

    // JACK
    size_t size = 3;
    int nBytes = static_cast<int> (size);

    // Write full message to buffer
    jack_ringbuffer_write(m_buffMessage, (const char *) midi_Note_Message,
                          nBytes);
    jack_ringbuffer_write(m_buffSize, (char *) &nBytes, sizeof ( nBytes));

#endif // LV2_SUPPORT || RKR_PLUS_LV2
}

#ifdef LV2_SUPPORT
void
MIDIConverter::setGain(int value)
{
    Input_Gain = (float) ((value + 50) / 100.0);
}
#endif

void
MIDIConverter::panic()
{
    for (int i = 0; i < 127; i++)
    {
        send_Midi_Note(i, 0, false); // false = note off: 0 is not used for note off
    }
    
    hay = 0;
    nota_actual = -1;
}

void
MIDIConverter::setmidichannel(int chan)
{
    channel = chan;
}

void
MIDIConverter::setTriggerAdjust(int val)
{
    TrigVal = 1.0f / (float) val;
}

void
MIDIConverter::setVelAdjust(int val)
{
    VelVal = 100.0f / (float) val;
}

void
MIDIConverter::setOctAdjust(int val)
{
    Moctave = val;
}

void
MIDIConverter::changepar(int npar, int value)
{
    switch (npar)
    {
    case MIDIConv_Gain:
#ifdef LV2_SUPPORT
        Pgain = value; // lv2 only
        setGain(value);
#endif
        break;
    case MIDIConv_Trigger:
        Ptrigger = value;
        setTriggerAdjust(value);
        break;
    case MIDIConv_Velocity:
        Pvelocity = value;
        setVelAdjust(value);
        break;
    case MIDIConv_Midi:
        Pmidi = value;
        setmidichannel(value - 1); // offset
        break;
    case MIDIConv_Octave:
        Poctave = value;
        setOctAdjust(value);
        break;
    case MIDIConv_FFT:
        Pfft = value;
        break;
    case MIDIConv_Panic:
        Ppanic = value;
        panic();
        break;
    }
}

int
MIDIConverter::getpar(int npar)
{
    switch (npar)
    {
    case MIDIConv_Gain:
#ifdef LV2_SUPPORT
        return (Pgain); // lv2 only
#else // LV2_SUPPORT
        return 0;
#endif

    case MIDIConv_Trigger:
        return (Ptrigger);

    case MIDIConv_Velocity:
        return (Pvelocity);

    case MIDIConv_Midi:
        return (Pmidi);

    case MIDIConv_Octave:
        return (Poctave);

    case MIDIConv_FFT:
        return (Pfft);

    case MIDIConv_Panic:
        return (Ppanic);

    default:
        return (0);
    }
}

#ifdef LV2_SUPPORT

void
MIDIConverter::update_freqs(float val) // val = 440.0f - user default settings
{
    int i;

    float aFreq = val;

    FREQS[0] = aFreq;
    LFREQS[0] = logf(FREQS[0]);

    for (i = 1; i < 12; i++)
    {
        FREQS[i] = FREQS[i - 1] * D_NOTE;
        LFREQS[i] = LFREQS[i - 1] + LOG_D_NOTE;
    }
}
#endif // LV2_SUPPORT
