/*
  rakarrack - a guitar effects software

 Compressor.h  -  Compressor Effect definitions
 Based on artscompressor.cc by Matthias Kretz <kretz@kde.org>
 Stefan Westerfeld <stefan@space.twc.de> 
 
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


#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include "global.h"

#define REALTYPE float
//define LN2                       (0.6931471805599453094172321214581765680755)  //Uncomment for att/rel half-life behavior
#define LN2                       (1.0f)  //Uncomment for att/rel to behave more like a capacitor.
//#define LN2                       (1.8378771)  //Approx. ln(2*pi) ~85% charge at attack/release times. 
#define MUG_CORR_FACT  0.4f



class Compressor
{

public:

  Compressor (REALTYPE * efxoutl_, REALTYPE * efxoutr_);
  ~Compressor ();

  void out (REALTYPE * smps_l, REALTYPE * smps_r);

  void Compressor_Change (int np, int value);
  void Compressor_Change_Preset (int npreset);
  int getpar (int npar);
  void cleanup ();

  REALTYPE *efxoutl;
  REALTYPE *efxoutr;



  // Compressor

  int tatt;			// attack time  (ms)
  int trel;			// release time (ms)
  int tratio;
  int toutput;
  int tthreshold;
  int a_out;
  int stereo;
  int tknee;

private:

  REALTYPE rvolume;
  REALTYPE lvolume;  
  REALTYPE rvolume_db;
  REALTYPE lvolume_db; 
  REALTYPE thres_db;		// threshold
  REALTYPE knee;
  REALTYPE thres_mx;
  REALTYPE kpct;			
  REALTYPE ratio;			// ratio
  REALTYPE kratio;			// ratio maximum for knee region
  REALTYPE eratio;			// dynamic ratio
  REALTYPE makeup;			// make-up gain
  REALTYPE makeuplin;

  REALTYPE outlevel;
  REALTYPE att;
  REALTYPE rel;
  
  REALTYPE rgain;
  REALTYPE rgain_old;

  REALTYPE lgain;
  REALTYPE lgain_old;

  REALTYPE lgain_t;
  REALTYPE rgain_t;

  REALTYPE coeff_kratio;
  REALTYPE coeff_ratio;
  REALTYPE coeff_knee;
  REALTYPE coeff_kk;               


};

#endif
