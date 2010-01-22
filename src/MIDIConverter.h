/* tuneit.c -- Detect fundamental frequency of a sound
* Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
*
* Modified for rakarrack by Daniel Vidal & Josep Andreu
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
#include <jack/midiport.h>
#include <alsa/asoundlib.h>

#define REALTYPE float

#define D_NOTE          1.059463f
#define LOG_D_NOTE      0.057762f
#define D_NOTE_SQRT     1.029302f
#define MAX_PEAKS 8


struct Midi_Event
{ 
 jack_nframes_t  time;
 int             len;    /* Length of MIDI message, in bytes. */
 jack_midi_data_t  *dataloc;
} ;



class MIDIConverter
{
public:
  MIDIConverter ();
  ~MIDIConverter ();


  REALTYPE *efxoutl;
  REALTYPE *efxoutr;
  signed short int *schmittBuffer;
  signed short int *schmittPointer;
  const char **notes;
  int note;
  float nfreq, afreq, freq;
  float TrigVal;
  int cents;
  void schmittFloat (int nframes, float *indatal, float *indatar);
  void setmidichannel (int channel);
  void panic ();
  void setTriggerAdjust (int val);
  void setVelAdjust (int val);

  float VelVal;
  int channel;
  snd_seq_t *port;
  int lanota;
  int nota_actual;
  int hay;
  int preparada;
  int ponla;
  int velocity;

  int moutdatasize;
  jack_midi_data_t  moutdata[2048];  
  Midi_Event Midi_event[2048];
  int ev_count; 

private:


  float freqs[12];
  float lfreqs[12];
  int blockSize;
  void displayFrequency (float freq);
  void schmittInit (int size);
  void schmittS16LE (int nframes, signed short int *indata);
  void schmittFree ();
  void MIDI_Send_Note_On (int note);
  void MIDI_Send_Note_Off (int note);


};

#endif /*MIDICONVERTER_H_ */
