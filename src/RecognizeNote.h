/* tuneit.c -- Detect fundamental frequency of a sound
* Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
*
* Modified for rakarrack by Daniel Vidal & Josep Andreu
* Recognizer.h  Recognizer Audio Note definitions
*
* This is free software, placed under the terms of the
* GNU General Public License, as published by the Free Software Foundation.
* Please see the file COPYING for details.
*/


#ifndef RECOGNIZE_H_
#define RECOGNIZE_H_

#include <math.h>
#include "global.h"
#include "AnalogFilter.h"

#define REALTYPE float

#define D_NOTE          1.059463f
#define LOG_D_NOTE      0.057762f
#define D_NOTE_SQRT     1.029302f
#define MAX_PEAKS 8


class Recognize
{
public:
  Recognize (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~Recognize ();


  REALTYPE *efxoutl;
  REALTYPE *efxoutr;
  signed short int *schmittBuffer;
  signed short int *schmittPointer;
  const char **notes;
  int note;
  float lafreq;
  float nfreq, afreq, freq;
  void schmittFloat (float *indatal, float *indatar);
  


private:

  int ultima;
  float freqs[12];
  float lfreqs[12];
  int blockSize;
  void displayFrequency (float freq);
  void schmittInit (int size);
  void schmittS16LE (signed short int *indata);
  void schmittFree ();


};

#endif
