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
/*
  rakarrack - a guitar effects software

  RecognizeNote.C  -  Recognize Audio Notess
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License (version 2) for more details.

 You should have received a copy of the GNU General Public License
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/


#include "RecognizeNote.h"



Recognize::Recognize (float *efxoutl_, float *efxoutr_)
{



  efxoutl = efxoutl_;
  efxoutr = efxoutr_;


  float aFreq = 440.0f;
  static const char *englishNotes[12] =
    { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
  notes = englishNotes;
  ultima = -1;
  note = 0;
  nfreq = 0;
  afreq = 0;
  freqs[0] = aFreq;
  lfreqs[0] = logf (freqs[0]);
  for (int i = 1; i < 12; i++)
    {
      freqs[i] = freqs[i - 1] * D_NOTE;
      lfreqs[i] = lfreqs[i - 1] + LOG_D_NOTE;
    } schmittInit (PERIOD);


}

Recognize::~Recognize ()

{

  schmittFree ();

}


void
Recognize::schmittInit (int size)
{
  blockSize = SAMPLE_RATE / size;
  schmittBuffer =
    (signed short int *) malloc (blockSize * sizeof (signed short int));
  schmittPointer = schmittBuffer;
};


void
Recognize::schmittS16LE (signed short int *indata)
{
  int i, j;
  float trigfact = 0.6f;


  for (i = 0; i < PERIOD; i++)
    {
      *schmittPointer++ = indata[i];
      if (schmittPointer - schmittBuffer >= blockSize)
	{
	  int endpoint, startpoint, t1, t2, A1, A2, tc, schmittTriggered;

	  schmittPointer = schmittBuffer;

	  for (j = 0, A1 = 0, A2 = 0; j < blockSize; j++)
	    {
	      if (schmittBuffer[j] > 0 && A1 < schmittBuffer[j])
		A1 = schmittBuffer[j];
	      if (schmittBuffer[j] < 0 && A2 < -schmittBuffer[j])
		A2 = -schmittBuffer[j];
	    }
	  t1 = lrintf ((float)A1 * trigfact + 0.5f);
	  t2 = -lrintf((float)A2 * trigfact + 0.5f);
	  startpoint = 0;
	  for (j = 1; schmittBuffer[j] <= t1 && j < blockSize; j++);
	  for (; !(schmittBuffer[j] >= t2 &&
		   schmittBuffer[j + 1] < t2) && j < blockSize; j++);
	  startpoint = j;
	  schmittTriggered = 0;
	  endpoint = startpoint + 1;
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
	      afreq =
		(float)SAMPLE_RATE *((float)tc / (float) (endpoint - startpoint));
	      displayFrequency (afreq);

	    }
	}
    }
};


void
Recognize::schmittFree ()
{
  free (schmittBuffer);
};

void
Recognize::schmittFloat (float *indatal, float *indatar)
{
  signed short int buf[PERIOD];
  int i;

  for (i = 0; i < PERIOD; i++)
    {
      buf[i] = (short) (((indatal[i]+indatar[i])*.1) * 32768);
    }
  schmittS16LE (buf);
};



void
Recognize::displayFrequency (float freq)
{
  float ldf, mldf;
  float lfreq;
  int i;
  int noteoff = 0;
  int octave = 4;

  if (freq < 1E-15)
    freq = 1E-15f;
  lfreq = logf (freq);
  while (lfreq < lfreqs[0] - LOG_D_NOTE * .5f)
    lfreq += LOG_2;
  while (lfreq >= lfreqs[0] + LOG_2 - LOG_D_NOTE * .5f)
    lfreq -= LOG_2;
  mldf = LOG_D_NOTE;
  for (i = 0; i < 12; i++)
    {
      ldf = fabsf (lfreq - lfreqs[i]);
      if (ldf < mldf)
	{
	  mldf = ldf;
	  note = i;
	}
    }

  nfreq = freqs[note];
  while (nfreq / freq > D_NOTE_SQRT)
    {
      nfreq *= .5f;
      octave--;
      if (octave < -1)
	{
	  noteoff = 1;
	  break;
	}

    }
  while (freq / nfreq > D_NOTE_SQRT)
    {
      nfreq *= 2.0f;
      octave++;
      if (octave > 7)
	{
	  noteoff = 1;
	  break;
	}
    }


  if (!noteoff)
    reconota = 24 + (octave * 12) + note - 3;
   
};
