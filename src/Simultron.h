/*
  Simultron.h - Cabinet Simulation Effect

  Author: Jospe Andreu
  
  Adapted effect structure of ZynAddSubFX - a software synthesizer
  Author: Nasca Octavian Paul
   
  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License 
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef SIMULTRON_H
#define SIMULTRON_H

#include "global.h"
#include "AnalogFilter.h"

class Simultron
{
public:
  Simultron (float * efxoutl_, float * efxoutr_);
  ~Simultron ();
  void out (float * smpsl, float * smpr);
  void setpreset (int npreset);
  void changepar (int npar, int value);
  int getpar (int npar);
  void cleanup ();
  int setfile (int value);

  int Ppreset;

  float *efxoutl;
  float *efxoutr;

  float outvolume;

  char Filename[128];


private:
  //Parametrii
  int Pvolume;	//This is master wet/dry mix like other FX...but I am finding it is not useful
  int Ppanning;	//Panning
  int Plrcross;	// L/R Mixing  // This is a mono effect, so lrcross and panning are pointless
  int Plevel;	
  int Filenum;
  void setvolume (int Pvolume);
  void setpanning (int Ppanning);
  void process_file(int num);
  int search(float val);

  //Parametrii reali
  float lpanning, rpanning;
  float gdata[200][32];
  float genv[200];
  float minenv,env,level;
  int numenv;  
  
  AnalogFilter *l[32];

};


#endif
