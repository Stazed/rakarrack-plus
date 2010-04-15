/*
 
  Simultron.C - Cabinet Simulation effect
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Simultron.h"

Simultron::Simultron (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ppanning = 64;
  Plrcross = 100;
  Filenum = 0;

  float xscale[] = {22.0,31.0,39.0,62.0,79.0,125.0,158.0,200.0,251.0,317.0,400.0,503.0,634.0,800.0,1000.0,1200.0,1500.0,2000.0,2500.0,3200.0,4000.0,4800.0,5500.0,6300.0,7100.0,8000.0,9000.0,10000.0,12000.0,14000.0,16000.0,20000.0};
  for (int i = 0; i < 32; i++)
 
  l[i] = new AnalogFilter (6, xscale[i], .7071f, 0);



  setpreset (Ppreset);
  cleanup ();
};

Simultron::~Simultron ()
{
  for(int i=0; i<32;i++) delete(l[i]);
};

/*
 * Cleanup the effect
 */
void
Simultron::cleanup ()
{
  for(int i=0; i<32;i++) l[i]->cleanup();
};


int
Simultron::search(float val)
{
int i;
if (val < minenv) return(numenv);

for(i=0;i<numenv; i++)
{
  if((genv[i]-val) < 0.0) return(i-1);
}

return(numenv);
}

/*
 * Effect output
 */
void
Simultron::out (float * smpsl, float * smpsr)
{
  int i;
  float sum;
  int thres;

  sum = fabsf(smpsl[i]) + fabsf(smpsr[i]);
  if(sum>env) env = sum; else  env=sum*ENV_TR+env*(1.0f-ENV_TR);

  thres = search(env);

  for(i=0;i<32;i++)
   {
    l[i]->setgain(rap2dB(gdata[thres][i]));     
    l[i]->filterout(smpsl); 
   }    

  for (i = 0; i < PERIOD; i++)
    {
      smpsr[i] = smpsl[i];
      smpsl[i] *= level * lpanning;
      smpsr[i] *= level * rpanning;  
    };



};


/*
 * Parameter control
 */
void
Simultron::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Simultron::setpanning (int Ppanning)
{
  this->Ppanning = Ppanning;
  lpanning = ((float)Ppanning + 0.5f) / 127.0f;
  rpanning = 1.0f - lpanning;
};

int
Simultron::setfile(int value)
{

FILE *fn;
char temp[1024];
int k=0;
Filenum = value;
bzero(Filename,sizeof(Filename));
sprintf(Filename, "%s/%d.data",DATADIR,Filenum+1);

if ((fn = fopen (Filename, "r")) == NULL)
return(0);

bzero(temp,sizeof(temp));

while(fgets(temp,sizeof temp, fn) != NULL)
 {
   
 sscanf (temp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n",
	  &genv[k],&gdata[k][0],&gdata[k][1],&gdata[k][2],&gdata[k][3],&gdata[k][4],&gdata[k][5],&gdata[k][6],
	  &gdata[k][7],&gdata[k][8],&gdata[k][9],&gdata[k][10],&gdata[k][11],&gdata[k][12],&gdata[k][13],&gdata[k][14],
	  &gdata[k][15],&gdata[k][16],&gdata[k][17],&gdata[k][18],&gdata[k][19],&gdata[k][20],&gdata[k][21],&gdata[k][22],
	  &gdata[k][23],&gdata[k][24],&gdata[k][25],&gdata[k][26],&gdata[k][27],&gdata[k][28],&gdata[k][29],&gdata[k][30],
	  &gdata[k][31]);


          bzero(temp,sizeof(temp));
          k++;
 }

 fclose(fn);   


 process_file(k);
 return(1);
};


void
Simultron::process_file(int num)
{
minenv = genv[num];
numenv = num;
}


void
Simultron::setpreset (int npreset)
{
  const int PRESET_SIZE = 11;
  const int NUM_PRESETS = 4;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Simultron 1
    {67, 64, 1, 0, 100},
    //Simultron 2
    {67, 64, 2, 0, 100},
    //Simultron 3
    {67, 64, 3, 0, 100},
    //Simultron 4
    {67, 64, 4, 0, 100}
  };

  
  if (npreset >= NUM_PRESETS)
    npreset = NUM_PRESETS - 1;
  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Simultron::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      setpanning (value);
      break;
    case 2:
      if(!setfile(value))
      {
       printf("shit\n"); 
      }
      break;
    case 3:
      //LRCross
      break;
    case 4:
      Plevel = value;
      level =  dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);;
      break;

   };
};

int
Simultron::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Ppanning);
      break;
    case 2:
      return (Filenum);
      break;
    case 3:
      //LRCross
      return (0);
      break;
    case 4:
      return(Plevel);
      break;
    };
  return (0);			//in case of bogus parameter number
};
