/* tuneit.c -- Detect fundamental frequency of a sound
* Copyright (C) 2004, 2005  Mario Lang <mlang@delysid.org>
*
* Modified for rakarrack by Josep Andreu
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

class Recognize
{
public:
  Recognize (float * efxoutl_, float * efxoutr_, float trig);
  ~Recognize ();

  void schmittFloat (float *indatal, float *indatar);
  void sethpf(int value);
  void setlpf(int value);
  int note;

  signed short int *schmittBuffer;
  signed short int *schmittPointer;
  const char **notes;
  float trigfact;
  float lafreq;
  float nfreq, afreq, freq;
  float *efxoutl;
  float *efxoutr;
  


private:

  void displayFrequency (float freq);
  void schmittInit (int size);
  void schmittS16LE (signed short int *indata);
  void schmittFree ();

  int ultima;
  int blockSize;

  AnalogFilter *lpfl, *lpfr, *hpfl, *hpfr;

  class Sustainer *Sus;

};

#endif
