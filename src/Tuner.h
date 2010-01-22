/* tuneit.c -- Detect fundamental frequency of a sound
 * Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
 *
 * Modified for rakarrack by Daniel Vidal & Josep Andreu
 * tune.h  Tuner definitions
 * This is free software, placed under the terms of the
 * GNU General Public License, as published by the Free Software Foundation.
 * Please see the file COPYING for details.
 */



#ifndef TUNER_H_
#define TUNER_H_

#include <math.h>
#include <stdlib.h>

#define REALTYPE float

#define D_NOTE          1.059463f
#define LOG_D_NOTE      0.057762f
#define D_NOTE_SQRT     1.029302f
#define MAX_PEAKS 8


class Tuner
{
public:
  Tuner ();
  ~Tuner ();

  REALTYPE *efxoutl;
  REALTYPE *efxoutr;
  signed short int *schmittBuffer;
  signed short int *schmittPointer;
  const char **notes;
  int note;
  int preparada;
  int note_actual;
  float nfreq, afreq, freq;
  int cents;
  void schmittFloat (int nframes, float *indatal, float *indatar);
private:


  float freqs[12];
  float lfreqs[12];
  int blockSize;
  void displayFrequency (float freq);
  void schmittInit (int size);
  void schmittS16LE (int nframes, signed short int *indata);
  void schmittFree ();
};

#endif /*TUNER_H_ */
