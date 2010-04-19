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
  Recognize (float * efxoutl_, float * efxoutr_);
  ~Recognize ();


  float *efxoutl;
  float *efxoutr;
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
