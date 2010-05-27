/*
 
  Echotron.C - Echotron effect
  Author: Ryan Billing & Josep Andreu
  
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

#include "Echotron.h"

Echotron::Echotron (float * efxoutl_, float * efxoutr_)
{
  efxoutl = efxoutl_;
  efxoutr = efxoutr_;

  //default values
  Ppreset = 0;
  Pvolume = 50;
  Ppanning = 64;
  Plrcross = 100;
  Phidamp = 60;
  Filenum = 0;
  Plength = 50;
  Puser = 0;
  Psafe = 0;
  fb = 0.0f;
  feedback = 0.0f;
  maxtime = 0.0f;

  hrtf_size = SAMPLE_RATE/2;
  maxx_size = (SAMPLE_RATE * 6);   //6 Seconds delay time

  lxn = (float *) malloc (sizeof (float) * (1 + maxx_size)); 
  rxn = (float *) malloc (sizeof (float) * (1 + maxx_size));    

  imax = SAMPLE_RATE/2;  // 1/2 second available
  imdelay = (float *) malloc (sizeof (float) * imax);
  offset = 0;
  hoffset = 0;
  data_length=0;
  hlength = 0;
  fstretch = 1.0f;
  idelay = 0.0f;
  decay = expf(-1.0f/(0.2f*fSAMPLE_RATE));  //0.2 seconds

  lpfl =  new AnalogFilter (0, 800, 1, 0);;
  lpfr =  new AnalogFilter (0, 800, 1, 0);;

   float center, qq;
  for (int i = 0; i < 8; i++)
    {
      center = 500;
      qq = 1.0f;
      filterbank[i].sfreq = center;
      filterbank[i].sq = qq;
      filterbank[i].sLP = 0.25f;  
      filterbank[i].sBP = -1.0f;
      filterbank[i].sHP = 0.5f;   
      filterbank[i].sStg = 1.0f;               
      filterbank[i].l = new RBFilter (0, center, qq, 0);
      filterbank[i].r = new RBFilter (0, center, qq, 0);
      
      filterbank[i].l->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);
      filterbank[i].r->setmix (1,filterbank[i].sLP , filterbank[i].sBP,filterbank[i].sHP);      
    };
    
   setpreset (Ppreset);
  cleanup ();
};

Echotron::~Echotron ()
{
};

/*
 * Cleanup the effect
 */
void
Echotron::cleanup ()
{
memset(lxn,0,sizeof(float)*maxx_size);
feedback = 0.0f;
oldl = 0.0f;
lpfl->cleanup ();
lpfr->cleanup ();

};

/*
 * Effect output
 */
void
Echotron::out (float * smpsl, float * smpsr)
{
  int i, j, xindex;
  float l,r,lyn, ryn, hyn;
  int length = Plength;

  int numfilters = 2; //just keep the compiler happy for now.
  
  for (i = 0; i < PERIOD; i++)
    {

      l = lpfl->filterout_s(smpsl[i] + lfeedback);  //High Freq damping 
      r = lpfr->filterout_s(smpsr[i] + rfeedback);

      lxn[offset] = l;
      rxn[offset] = r;
      
      //Convolve 
      lyn = 0.0f;
      ryn = 0.0f;
      xindex = offset;
      
      for (j =0; j<numfilters; j++)
      {
      xindex = offset + time[j];
      if(xindex>=maxx_size) xindex -= maxx_size;
      lyn += filterbank[i].l->filterout_s(lxn[xindex]) * ldata[j];		//filter each tap
      ryn += filterbank[i].r->filterout_s(rxn[xindex]) * rdata[j];
      }
      
      for (j =numfilters; j<length; j++)
      {
      xindex = offset + time[j];
      if(xindex>=maxx_size) xindex -= maxx_size;
      lyn += lxn[xindex] * ldata[j];		//unfiltered reflections
      ryn += rxn[xindex] * rdata[j];
      }


      efxoutl[i] = (lrcross*ryn + ilrcross*lyn) * levpanl;
      efxoutr[i] = (lrcross*lyn + ilrcross*ryn) * levpanr;       
      lfeedback = fb * lyn;
      rfeedback = fb * ryn;      
      efxoutl[i] = lyn * levpanl;
      efxoutr[i] = ryn * levpanr;         

      offset--;
      if (offset<0) offset = maxx_size;   
     
    };



};


/*
 * Parameter control
 */
void
Echotron::setvolume (int Pvolume)
{
  this->Pvolume = Pvolume;
  outvolume = (float)Pvolume / 127.0f;
  if (Pvolume == 0)
    cleanup ();

};

void
Echotron::setpanning (int value)
{
  Ppanning = value;
  rpanning = ((float)Ppanning) / 64.0f;
  lpanning = 2.0f - rpanning;
  lpanning = 10.0f * powf(lpanning, 4);
  rpanning = 10.0f * powf(rpanning, 4);
  lpanning = 1.0f - 1.0f/(lpanning + 1.0f);
  rpanning = 1.0f - 1.0f/(rpanning + 1.0f); 
  lpanning *= 1.1f;
  rpanning *= 1.1f; 
  levpanl=level*lpanning;
  levpanr=level*rpanning;
};

int
Echotron::setfile(int value)
{

char wbuf[128];

if(!Puser)
{
Filenum = value;
memset(Filename,NULL,sizeof(Filename));
sprintf(Filename, "%s/%d.dly",DATADIR,Filenum+1);
}

string header;
string Time[ECHOTRON_F_SIZE];
string Level[ECHOTRON_F_SIZE];
string LP[ECHOTRON_MAXFILTERS];
string BP[ECHOTRON_MAXFILTERS];
string HP[ECHOTRON_MAXFILTERS];
string Freq[ECHOTRON_MAXFILTERS];
string Q[ECHOTRON_MAXFILTERS];
string Stages[ECHOTRON_MAXFILTERS];
string trash;

int count = 0;

  ifstream myfile (Filename);
  if (myfile.is_open())
  {
      getline (myfile,header); 
      cout << header << endl;
    while ( (! myfile.eof()) && (count<ECHOTRON_F_SIZE) )
    {  
      getline (myfile,Time[count], '\t');
      getline (myfile,Level[count], '\t');  
      if(count<ECHOTRON_MAXFILTERS)
      {    
      getline (myfile,LP[count], '\t');     
      getline (myfile,BP[count], '\t'); 
      getline (myfile,HP[count], '\t');           
      getline (myfile,Freq[count], '\t');  
      getline (myfile,Q[count], '\t');
      getline (myfile,Stages[count]);  
      }
      else
      {
      getline(myfile,trash);  //something we can ignore, but will set pointer to next line for proper reading of next Time & Level
      }
    count++;
  
    }
    
    myfile.close();  
      
    for(int i =0; i<count; i++) 
    {
    from_string<float>(fTime[i],Time[i],dec);
    from_string<float>(fLevel[i],Level[i],dec);
      if(i<ECHOTRON_MAXFILTERS)
      {     
    from_string<float>(fLP[i],LP[i],dec);
    from_string<float>(fBP[i],BP[i],dec);
    from_string<float>(fHP[i],HP[i],dec);
    from_string<float>(fFreq[i],Freq[i],dec);
    from_string<float>(fQ[i],Q[i],dec);
    from_string<float>(fStages[i],Stages[i],dec);
      }
    
   /* //Print results to confirm
    cout<<fTime[i] <<"\t" <<fLevel[i] <<"\t";
    if(i<ECHOTRON_MAXFILTERS) cout<<fLP[i] <<"\t" <<fBP[i]<<"\t"<<fHP[i] <<"\t" <<fFreq[i] <<"\t" <<fQ[i] <<"\t" <<fStages[i] << endl;
    else cout<<endl;*/
    
    }


  }

  else return(0);


cleanup();


cleanup();
convert_time();
return(1);
};

void Echotron::convert_time()
{

  Plength = 2; //just so something happens for now.
 

};


void
Echotron::setlpf (int value)
{
  Plpf = value;
  float fr = (float)Plpf;
  lpfl->setfreq (fr);
  lpfr->setfreq (fr);
    
};



void
Echotron::sethidamp (int Phidamp)
{
  this->Phidamp = Phidamp;
  hidamp = 1.0f - (float)Phidamp / 127.1f;
  alpha_hidamp = 1.0f - hidamp;
};

void
Echotron::setfb(int value)
{

   if(Pfb<=0) 
   fb = (float)value/64.0f * 0.3f;
   else
   fb = (float)value/64.0f * 0.15f; 
   
   fb*=((1627.0f-(float)Pdiff-(float)Plength)/1627.0f);
   fb*=(1.0f-((float)Plevel/127.0f));  
   fb*=(1.0f-diffusion)*.5f;

}


void
Echotron::setpreset (int npreset)
{
  const int PRESET_SIZE = 16;
  const int NUM_PRESETS = 9;
  int presets[NUM_PRESETS][PRESET_SIZE] = {
    //Spring
    {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64, 0, 0, 20000, 0},
    //Concrete Stair
    {64, 0, 1, 500, 0, 0, 0, 40, 1, 0, 0, 64, 0, 0, 20000, 0},
    //Nice Hall
    {64, 0, 1, 500, 0, 0, 60, 15, 2, 0, 0, 64, 0, 0, 20000, 0},
    //Hall
    {64, 16, 1, 500, 0, 0, 0, 22, 3, -17, 0, 64, 0, 0, 20000, 0},
    //Room
    {64, 0, 1, 1500, 0, 0, 48, 20, 4, 0, 0, 64, 0, 0, 20000, 0},
    //Hall
    {88, 0, 1, 1500, 0, 0, 88, 14, 5, 0, 0, 64, 0, 0, 20000, 0},
    //Guitar
    {64, 0, 1, 1500, 0, 0, 30, 34, 6, 0, 0, 64, 0, 0, 20000, 0},
    //Studio
    {64, 0, 1, 1500, 0, 0, 30, 20, 7, 0, 0, 64, 0, 0, 20000, 0},
    //Cathedral
    {64, 0, 1, 1500, 0, 30, 0, 40, 9, 0, 0, 64, 0, 0, 20000, 0}

  };

  for (int n = 0; n < PRESET_SIZE; n++)
    changepar (n, presets[npreset][n]);
  Ppreset = npreset;
};


void
Echotron::changepar (int npar, int value)
{
  switch (npar)
    {
    case 0:
      setvolume (value);
      break;
    case 1:
      Pfade=value;
      ffade = ((float) value)/127.0f;
      convert_time();
      break;
    case 2:
      Psafe=value;
      break;
    case 3:
     Plength = value;
     if((Psafe) && (Plength>400)) Plength = 400;
     convert_time(); 
     break;
    case 4:
      Puser = value;
      break;
    case 5:
      Pidelay = value;
      idelay = ((float) value)/1000.0f;
      convert_time();
      break;
    case 6:
      sethidamp (value);
      break;
    case 7:
      Plevel = value;
      level =  2.0f * dB2rap (60.0f * (float)Plevel / 127.0f - 40.0f);
      levpanl=level*lpanning;
      levpanr=level*rpanning;
      break;
    case 8:
      if(!setfile(value)) error_num=2;
      break;
    case 9:
      Pstretch = value;
      fstretch = ((float) value)/64.0f;   
      convert_time();
      break;
    case 10:
      Pfb = value;
      setfb(value);
      break;
    case 11:
      setpanning (value);
      break;
    case 12:
      Pes = value;
      break;
    case 13:
      Prv = value;
      break;     
    case 14:
      setlpf (value);
      break;
    case 15:
      Pdiff=value;
      diffusion = ((float) value)/127.0f;
      convert_time();      
      break;

   };
};

int
Echotron::getpar (int npar)
{
  switch (npar)
    {
    case 0:
      return (Pvolume);
      break;
    case 1:
      return (Pfade);
      break;
    case 2:
      return(Psafe);
      break;
    case 3:
      return(Plength); 
      break;  
    case 8:
      return (Filenum);
      break;
    case 5:
      return (Pidelay);
      break;
    case 6:
      return (Phidamp);
      break;
    case 7:
      return(Plevel);
      break;
    case 4:
      return(Puser);
      break;
    case 9:
      return(Pstretch);
      break;   
    case 10:
      return(Pfb);
      break; 
    case 11:
      return(Ppanning);
      break;
    case 12:
      return(Pes);
      break;
    case 13:
      return(Prv);
      break;  
    case 14:
      return(Plpf);
      break;  
    case 15:
      return(Pdiff);
      break;  


    };
  return (0);			//in case of bogus parameter number
};
