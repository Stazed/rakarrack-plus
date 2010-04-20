/*
  rakarrack - a guitar effects software

  pan.h  -  Auto/Pan - Extra stereo definitions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

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

#ifndef AUTOPAN_H
#define AUTOPAN_H

#include "global.h"
#include "EffectLFO.h"

class Pan
{

public:
  Pan (float *efxoutl_, float *efxoutr_);
   ~Pan ();
  void out (float *smpsl, float *smpsr);
  void setpreset (unsigned char npreset);
  void changepar (int npar, unsigned char value);
  unsigned char getpar (int npar);
  void cleanup ();


  unsigned char Ppreset;
  float *efxoutl;
  float *efxoutr;

  float outvolume;


private:
    EffectLFO lfo;
  unsigned char Pvolume;
  unsigned char Ppanning;
  unsigned char Pextra;
  unsigned char PAutoPan;
  unsigned char PextraON;

  void setvolume (unsigned char Pvolume);
  void setpanning (unsigned char Ppanning);
  void setextra (unsigned char Pdepth);

  float dvalue;
  float panning, mul;
  float lfol, lfor;
  float ll, lr;
};

#endif
