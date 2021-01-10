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

Echotron::Echotron(double sample_rate, uint32_t intermediate_bufsize) :
    Ppreset(),
    outvolume(0.5f),
    Filename(),
    File(),
    error(),
    fSAMPLE_RATE(sample_rate),
    PERIOD(intermediate_bufsize),
    fPERIOD(intermediate_bufsize),
    Pvolume(50),
    Ppanning(64),
    Plrcross(100),
    Phidamp(60),
    Puser(),
    Ptempo(76),
    Filenum(),
    Pfb(),
    Pdepth(64),
    Pwidth(64),
    Pfilters(),
    Pmodfilts(),
    Pmoddly(),
    Pstdiff(),
    Plength(10),
    Llength(),
    offset(),
    maxx_size((sample_rate * 6)),   //  6 Seconds delay time
    initparams(),
    ldmod(),
    rdmod(),
    oldldmod(),
    oldrdmod(),
    interpl(),
    interpr(),
    dlyrange(),
    width(),
    depth(),
    lpanning(),
    rpanning(),
    hidamp(),
    lxn(NULL),
    rxn(NULL),
    fb(),
    rfeedback(),
    lfeedback(),
    lrcross(),
    ilrcross(),
    tempo_coeff(60.0f / (float) Ptempo),
    lfo(NULL),
    dlfo(NULL),
    lpfl(NULL),
    lpfr(NULL),
    interpbuf(NULL),
    filterbank(),
    Fpre(NULL)
#ifdef LV2_SUPPORT
    ,FILENAME(NULL)
#endif // LV2
{
    File = loaddefault();

    lfo = new EffectLFO(sample_rate);
    dlfo = new EffectLFO(sample_rate);

    lxn = new delayline(6.0f, ECHOTRON_F_SIZE, sample_rate);
    rxn = new delayline(6.0f, ECHOTRON_F_SIZE, sample_rate);

    lxn->set_mix(0.0f);
    rxn->set_mix(0.0f);

    initialize();

    setpreset(Ppreset);
    init_params();
}

Echotron::~Echotron()
{
    delete lfo;
    delete dlfo;
    delete lxn;
    delete rxn;

    clear_initialize();
}

/*
 * Cleanup the effect
 */
void
Echotron::cleanup()
{
    lxn->cleanup();
    rxn->cleanup();
    lxn->set_averaging(0.05f);
    rxn->set_averaging(0.05f);

    lpfl->cleanup();
    lpfr->cleanup();
}

#ifdef LV2_SUPPORT
void
Echotron::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        fPERIOD = period;
        clear_initialize();
        initialize();

        DlyFile filedata; // need to reload the file to reset the parameters
        filedata = loadfile(FILENAME);

        applyfile(filedata);
        sethidamp(Phidamp); // set for the analog filter
    }
    else
    {
        PERIOD = period;
        fPERIOD = period;
    }

    lfo->updateparams(fPERIOD);
    dlfo->updateparams(fPERIOD);
}
#endif // LV2

void
Echotron::initialize()
{
    interpbuf = new float[PERIOD];
    lpfl = new AnalogFilter(0, 800, 1, 0, fSAMPLE_RATE, interpbuf);
    lpfr = new AnalogFilter(0, 800, 1, 0, fSAMPLE_RATE, interpbuf);

    float center, qq;
    
    for (int i = 0; i < ECHOTRON_MAXFILTERS; i++)
    {
        center = 500;
        qq = 1.0f;
        filterbank[i].sfreq = center;
        filterbank[i].sq = qq;
        filterbank[i].sLP = 0.25f;
        filterbank[i].sBP = -1.0f;
        filterbank[i].sHP = 0.5f;
        filterbank[i].sStg = 1.0f;
        filterbank[i].l = new RBFilter(0, center, qq, 0, fSAMPLE_RATE, interpbuf);
        filterbank[i].r = new RBFilter(0, center, qq, 0, fSAMPLE_RATE, interpbuf);

        filterbank[i].l->setmix(1, filterbank[i].sLP, filterbank[i].sBP, filterbank[i].sHP);
        filterbank[i].r->setmix(1, filterbank[i].sLP, filterbank[i].sBP, filterbank[i].sHP);
    }
}

void
Echotron::clear_initialize()
{
    delete lpfl;
    delete lpfr;
    
    for (int i = 0; i < ECHOTRON_MAXFILTERS; i++)
    {
        delete filterbank[i].l;
        delete filterbank[i].r;
    }
    
    delete[] interpbuf;
}

/*
 * Effect output
 */
void
Echotron::out(float * efxoutl, float * efxoutr)
{
    int length = Plength;

    if ((Pmoddly) || (Pmodfilts)) modulate_delay();
    else interpl = interpr = 0;

    float tmpmodl = oldldmod;
    float tmpmodr = oldrdmod;

    for (unsigned i = 0; i < PERIOD; i++)
    {
        tmpmodl += interpl;
        tmpmodr += interpr;

        float l = lxn->delay((lpfl->filterout_s(efxoutl[i] + lfeedback)), 0.0f, 0, 1, 0); //High Freq damping
        float r = rxn->delay((lpfr->filterout_s(efxoutr[i] + rfeedback)), 0.0f, 0, 1, 0);

        //Convolve
        float lyn = 0.0f;
        float ryn = 0.0f;

        if (Pfilters)
        {
            int j = 0;
            
            for (int k = 0; k < length; k++)
            {
                float lxindex = File.ltime[k] + tmpmodl;
                float rxindex = File.rtime[k] + tmpmodr;

                if ((File.iStages[k] >= 0)&&(j < ECHOTRON_MAXFILTERS))
                {
                    lyn += filterbank[j].l->filterout_s(lxn->delay(l, lxindex, k, 0, 0)) * File.ldata[k]; //filter each tap specified
                    ryn += filterbank[j].r->filterout_s(rxn->delay(r, rxindex, k, 0, 0)) * File.rdata[k];
                    j++;
                }
                else
                {
                    lyn += lxn->delay(l, lxindex, k, 0, 0) * File.ldata[k];
                    ryn += rxn->delay(r, rxindex, k, 0, 0) * File.rdata[k];
                }
            }
        }
        else
        {
            for (int k = 0; k < length; k++)
            {
                float lxindex = File.ltime[k] + tmpmodl;
                float rxindex = File.rtime[k] + tmpmodr;

                lyn += lxn->delay(l, lxindex, k, 0, 0) * File.ldata[k];
                ryn += rxn->delay(r, rxindex, k, 0, 0) * File.rdata[k];
            }
        }

        lfeedback = (lrcross * ryn + ilrcross * lyn) * lpanning;
        rfeedback = (lrcross * lyn + ilrcross * ryn) * rpanning;
        efxoutl[i] = lfeedback;
        efxoutr[i] = rfeedback;
        lfeedback *= fb;
        rfeedback *= fb;

    }

    if (initparams) init_params();
}

/*
 * Parameter control
 */
void
Echotron::setvolume(int Pvolume)
{
    this->Pvolume = Pvolume;
    outvolume = (float) Pvolume / 127.0f;
    
    if (Pvolume == 0)
        cleanup();
}

void
Echotron::setpanning(int value)
{
    Ppanning = value;
    rpanning = ((float) Ppanning) / 64.0f;
    lpanning = 2.0f - rpanning;
    lpanning = 10.0f * powf(lpanning, 4);
    rpanning = 10.0f * powf(rpanning, 4);
    lpanning = 1.0f - 1.0f / (lpanning + 1.0f);
    rpanning = 1.0f - 1.0f / (rpanning + 1.0f);
    lpanning *= 1.1f;
    rpanning *= 1.1f;
    
    if (lpanning > 1.0f) lpanning = 1.0f;
    if (rpanning > 1.0f) rpanning = 1.0f;
}

int
Echotron::setfile(int value)
{
#ifdef LV2_SUPPORT
    /* LV2 will only call setfile() directly with Puser set and the value will be always USERFILE.
       This single case occurs upon initialization before Puser is set because setpreset() is called
       by initializer. So we just ignore the initial call for LV2 since the value will be incorrect
       and immediately replaced by the default file set in the ttl */
    if (value != USERFILE)
    {
        return (0);
    }
#endif // LV2_SUPPORT

    DlyFile filedata;

    if (!Puser)
    {
        Filenum = value;
        memset(Filename, 0, sizeof (Filename));
        sprintf(Filename, "%s/%d.dly", DATADIR, Filenum + 1);
    }

    //    printf("Filename %s\n",Filename);
    filedata = loadfile(Filename);
    applyfile(filedata);
    
    if (error)
        return 0;
    
    return 1;
    /*

    if ((fs = fopen (Filename, "r")) == NULL) {
        loaddefault();
        return(0);
    }

    while (fgets(wbuf,sizeof wbuf,fs) != NULL) {
        //fgets(wbuf,sizeof wbuf,fs);
        if(wbuf[0]!='#') break;
        memset(wbuf,0,sizeof(wbuf));
    }

    sscanf(wbuf,"%f\t%f\t%d",&subdiv_fmod,&subdiv_dmod,&f_qmode); //Second line has tempo subdivision
//printf("subdivs:\t%f\t%f\n",subdiv_fmod,subdiv_dmod);

    int count = 0;
    memset(iStages,0,sizeof(iStages));



    while ((fgets(wbuf,sizeof wbuf,fs) != NULL) && (count<ECHOTRON_F_SIZE)) {
        if(wbuf[0]==10) break;  // Check Carriage Return
        if(wbuf[0]=='#') continue;
        sscanf(wbuf,"%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d",&tPan, &tTime, &tLevel,
               &tLP,  &tBP,  &tHP,  &tFreq,  &tQ,  &tiStages);
        //printf("params:\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",tPan, tTime, tLevel,
        //tLP,  tBP,  tHP,  tFreq,  tQ,  tiStages);
        if((tPan<-1.0f) || (tPan>1.0f)) {
            error_num=5;
            break;
        } else fPan[count]=tPan;

        if((tTime <-6.0) || (tTime>6.0f)) {
            error_num=6;
            break;
        } else fTime[count]=fabs(tTime);

        if((tLevel <-10.0f) || (tLevel>10.0f)) {
            error_num=7;
            break;
        } else fLevel[count]=tLevel;

        if((tLP <-2.0f) || (tLP>2.0f)) {
            error_num=8;
            break;
        } else fLP[count]=tLP;

        if((tBP<-2.0f) || (tBP>2.0f)) {
            error_num=9;
            break;
        } else fBP[count]=tBP;

        if((tHP<-2.0f) || (tHP>2.0f)) {
            error_num=10;
            break;
        } else fHP[count]=tHP;

        if((tFreq <20.0f) || (tFreq>26000.0f)) {
            error_num=11;
            break;
        } else fFreq[count]=tFreq;

        if((tQ <0.0) || (tQ>300.0f)) {
            error_num=12;
            break;
        } else fQ[count]=tQ;

        if((tiStages<0) || (tiStages>MAX_FILTER_STAGES)) {
            error_num=13;
            break;
        } else iStages[count]=tiStages-1;   //check in main loop if <0, then skip filter


        memset(wbuf,0,sizeof(wbuf));
        count++;
    }
    fclose(fs);

    if(!Pchange) Plength=count;
    cleanup();
    init_params();
    return(1);
     */
}

DlyFile
Echotron::loadfile(char* Filename)
{
#ifdef LV2_SUPPORT
    FILENAME = Filename; // For lv2 if need to re-initialize and reload file
#endif // LV2
    
    float tPan = 0.0f;
    float tTime = 0.0f;
    float tLevel = 0.0f;
    float tLP = 0.0f;
    float tBP = 0.0f;
    float tHP = 0.0f;
    float tFreq = 20.0f;
    float tQ = 1.0f;
    int tiStages = 0;

    FILE *fs;
    DlyFile f;

    char wbuf[128];

    error = 0;

    if ((fs = fopen(Filename, "r")) == NULL)
    {
        error = 1;
        return loaddefault();
    }

    strcpy(f.Filename, Filename); // Must copy the file name here for lv2 save when if(plug->file_changed) in rkr.lv2.C

    while (fgets(wbuf, sizeof wbuf, fs) != NULL)
    {
        //fgets(wbuf,sizeof wbuf,fs);
        if (wbuf[0] != '#') break;
        
        memset(wbuf, 0, sizeof (wbuf));
    }

    sscanf(wbuf, "%f\t%f\t%d", &f.subdiv_fmod, &f.subdiv_dmod, &f.f_qmode); //Second line has tempo subdivision
//    printf("subdivs:%f\t%f\t%d\n",f.subdiv_fmod,f.subdiv_dmod, f.f_qmode);

    int count = 0;
    memset(f.iStages, 0, sizeof (f.iStages));

    while ((fgets(wbuf, sizeof wbuf, fs) != NULL) && (count < ECHOTRON_F_SIZE))
    {
        if (wbuf[0] == 10) break; // Check Carriage Return
        
        if (wbuf[0] == '#') continue;
        
        sscanf(wbuf, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d", &tPan, &tTime, &tLevel,
               &tLP, &tBP, &tHP, &tFreq, &tQ, &tiStages);
        //printf("params:\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",tPan, tTime, tLevel,
        //tLP,  tBP,  tHP,  tFreq,  tQ,  tiStages);
        if ((tPan<-1.0f) || (tPan > 1.0f))
        {
            error = 5;
            break;
        }
        else f.fPan[count] = tPan;

        if ((tTime <-6.0) || (tTime > 6.0f))
        {
            error = 6;
            break;
        }
        else f.fTime[count] = fabs(tTime);

        if ((tLevel <-10.0f) || (tLevel > 10.0f))
        {
            error = 7;
            break;
        }
        else f.fLevel[count] = tLevel;

        if ((tLP <-2.0f) || (tLP > 2.0f))
        {
            error = 8;
            break;
        }
        else f.fLP[count] = tLP;

        if ((tBP<-2.0f) || (tBP > 2.0f))
        {
            error = 9;
            break;
        }
        else f.fBP[count] = tBP;

        if ((tHP<-2.0f) || (tHP > 2.0f))
        {
            error = 10;
            break;
        }
        else f.fHP[count] = tHP;

        if ((tFreq < 20.0f) || (tFreq > 26000.0f))
        {
            error = 11;
            break;
        }
        else f.fFreq[count] = tFreq;

        if ((tQ < 0.0) || (tQ > 300.0f))
        {
            error = 12;
            break;
        }
        else f.fQ[count] = tQ;

        if ((tiStages < 0) || (tiStages > MAX_FILTER_STAGES))
        {
            error = 13;
            break;
        }
        else f.iStages[count] = tiStages - 1; //check in main loop if <0, then skip filter

        memset(wbuf, 0, sizeof (wbuf));
        count++;
    }
    
    fclose(fs);

    f.fLength = count;
    
    /* Limit max taps to file length */
    if (Plength > f.fLength)
    {
        Plength = f.fLength;
    }


#ifdef LV2_SUPPORT
    // what to do here - FIXME how does lv2 handle file errors?
#else
    error_num = error; // error_num is used by rakarrack to pop up a warning dialog
#endif     

    return f;
}

void
Echotron::applyfile(DlyFile f)
{
    File = f;
    init_params();
}

DlyFile
Echotron::loaddefault()
{
    Plength = 1;
    DlyFile f;
    strcpy(f.Filename, "default");
    f.fLength = 1;
    f.fPan[0] = 0.0f; //
    f.fTime[0] = 1.0f; //default 1 measure delay
    f.fLevel[0] = 0.7f;
    f.fLP[0] = 1.0f;
    f.fBP[0] = -1.0f;
    f.fHP[0] = 1.0f;
    f.fFreq[0] = 800.0f;
    ;
    f.fQ[0] = 2.0f;
    f.iStages[0] = 1;
    f.subdiv_dmod = 1.0f;
    f.subdiv_fmod = 1.0f;
    f.f_qmode = 0;
    return f;
}

void Echotron::init_params()
{
    cleanup(); // should always do this on init or junk left over

    float hSR = fSAMPLE_RATE * 0.5f;
    int tfcnt = 0;

    initparams = 0;
    depth = ((float) (Pdepth - 64)) / 64.0f;
    dlyrange = 0.008 * f_pow2(4.5f * depth);
    width = ((float) Pwidth) / 127.0f;

    float tmptempo = (float) Ptempo;
    lfo->Pfreq = lrintf(File.subdiv_fmod * tmptempo);
    dlfo->Pfreq = lrintf(File.subdiv_dmod * tmptempo);

    /* Need to process full file length or have uninitialized values if user increased Plength*/
    for (int i = 0; i < File.fLength; i++)
    {
        // tmp_time=lrintf(fTime[i]*tempo_coeff*fSAMPLE_RATE);
        // if(tmp_time<maxx_size) rtime[i]=tmp_time; else rtime[i]=maxx_size;

        File.ltime[i] = File.rtime[i] = File.fTime[i] * tempo_coeff;

        float tpanl, tpanr; // initialize o.k.
        if (File.fPan[i] >= 0.0f)
        {
            tpanr = 1.0;
            tpanl = 1.0f - File.fPan[i];
        }
        else
        {
            tpanl = 1.0;
            tpanr = 1.0f + File.fPan[i];
        }

        File.ldata[i] = File.fLevel[i] * tpanl;
        File.rdata[i] = File.fLevel[i] * tpanr;

        if ((tfcnt < ECHOTRON_MAXFILTERS)&&(File.iStages[i] >= 0))
        {
            int Freq = File.fFreq[i] * f_pow2(depth * 4.5f);
            
            if (Freq < 20.0) Freq = 20.0f;
            if (Freq > hSR) Freq = hSR;
            
            filterbank[tfcnt].l->setfreq_and_q(Freq, File.fQ[i]);
            filterbank[tfcnt].r->setfreq_and_q(Freq, File.fQ[i]);
            filterbank[tfcnt].l->setstages(File.iStages[i]);
            filterbank[tfcnt].r->setstages(File.iStages[i]);
            filterbank[tfcnt].l->setmix(1, File.fLP[i], File.fBP[i], File.fHP[i]);
            filterbank[tfcnt].r->setmix(1, File.fLP[i], File.fBP[i], File.fHP[i]);
            filterbank[tfcnt].l->setmode(File.f_qmode);
            filterbank[tfcnt].r->setmode(File.f_qmode);
            tfcnt++;
        }
    }
}

void Echotron::modulate_delay()
{
    float fperiod = 1.0f / fPERIOD;

    float lfol, lfor, dlfol, dlfor; // initialize o.k.
    lfo->effectlfoout(&lfol, &lfor);
    dlfo->effectlfoout(&dlfol, &dlfor);
    
    if (Pmodfilts)
    {
        float lfmod = f_pow2((lfol * width + 0.25f + depth)*4.5f);
        float rfmod = f_pow2((lfor * width + 0.25f + depth)*4.5f);
        
        for (int i = 0; i < ECHOTRON_MAXFILTERS; i++)
        {
            if (i < File.fLength)
            {
                filterbank[i].l->setfreq(lfmod * File.fFreq[i]);
                filterbank[i].r->setfreq(rfmod * File.fFreq[i]);
            }
        }
    }

    if (Pmoddly)
    {
        oldldmod = ldmod;
        oldrdmod = rdmod;
        ldmod = width*dlfol;
        rdmod = width*dlfor;

        // ldmod=lrintf(dlyrange*tempo_coeff*fSAMPLE_RATE*ldmod);
        // rdmod=lrintf(dlyrange*tempo_coeff*fSAMPLE_RATE*rdmod);
        ldmod = dlyrange * tempo_coeff*ldmod;
        rdmod = dlyrange * tempo_coeff*rdmod;
        interpl = (ldmod - oldldmod) * fperiod;
        interpr = (rdmod - oldrdmod) * fperiod;
    }
    else
    {
        oldldmod = 0.0f;
        oldrdmod = 0.0f;
        ldmod = 0.0f;
        rdmod = 0.0f;
        interpl = 0.0f;
        interpr = 0.0f;
    }
}

void
Echotron::sethidamp(int Phidamp)
{
    this->Phidamp = Phidamp;
    hidamp = 1.0f - (float) Phidamp / 127.1f;
    float fr = 20.0f * f_pow2(hidamp * 10.0f);
    lpfl->setfreq(fr);
    lpfr->setfreq(fr);
}

void
Echotron::setfb(int value)
{
    fb = (float) value / 64.0f;
}

void
Echotron::setpreset(int npreset)
{
    const int PRESET_SIZE = C_ECHOTRON_PARAMETERS;
    const int NUM_PRESETS = 6;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Summer
        {64, 45, 34, 4, 0, 76, 3, 41, 0, 96, -13, 64, 1, 1, 1, 1},
        //Ambience
        {96, 64, 16, 3, 0, 180, 50, 64, 1, 96, -4, 64, 1, 0, 0, 0},
        //Arranjer
        {64, 64, 10, 3, 0, 400, 32, 64, 1, 96, -8, 64, 1, 0, 0, 0},
        //Suction
        {0, 47, 28, 8, 0, 92, 0, 64, 3, 32, 0, 64, 1, 1, 1, 1},
        //SucFlange
        {64, 36, 93, 8, 0, 81, 0, 64, 3, 32, 0, 64, 1, 0, 1, 1},
        //Soft
        {64, 64, 41, 4, 0, 64, 99, 84, 0, 64, 0, 64, 0, 0, 0, 0},
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(41, npreset - NUM_PRESETS + 1, pdata, Filename);
        
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, pdata[n]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
            changepar(n, presets[npreset][n]);
    }
    
    Ppreset = npreset;
}

void
Echotron::changepar(int npar, int value)
{
    float tmptempo;
    switch (npar)
    {
    case Echotron_DryWet:
        setvolume(value);
        break;
    case Echotron_Depth:
        Pdepth = value;
        initparams = 1;
        break;
    case Echotron_LFO_Width:
        Pwidth = value;
        initparams = 1;
        break;
    case Echotron_Taps:
        Plength = value;
        if (Plength > 127) Plength = 127;
        initparams = 1;
        break;
    case Echotron_User_File:
        Puser = value;
        break;
    case Echotron_Tempo:
        Ptempo = value;

        tmptempo = (float) Ptempo;
        tempo_coeff = 60.0f / tmptempo;
        lfo->Pfreq = lrintf(File.subdiv_fmod * tmptempo);
        dlfo->Pfreq = lrintf(File.subdiv_dmod * tmptempo);
        lfo->updateparams(fPERIOD);
        dlfo->updateparams(fPERIOD);
        initparams = 1;
        break;
    case Echotron_Damp:
        sethidamp(value);
        break;
    case Echotron_LR_Cross:
        Plrcross = value;
        lrcross = ((float) (Plrcross) - 64) / 64.0;
        ilrcross = 1.0f - abs(lrcross);
        break;
    case Echotron_Set_File:
#ifdef LV2_SUPPORT
        setfile(value); // This will only be called by changepar() upon initialization for lv2 and is ignored.
#else
        if (!setfile(value)) error_num = 4;
#endif
        break;
    case Echotron_LFO_Stereo:
        lfo->Pstereo = value;
        dlfo->Pstereo = value;
        lfo->updateparams(fPERIOD);
        dlfo->updateparams(fPERIOD);
        break;
    case Echotron_Feedback:
        Pfb = value;
        setfb(value);
        break;
    case Echotron_Pan:
        setpanning(value);
        break;
    case Echotron_Mod_Delay:
        Pmoddly = value; //delay modulation on/off
        break;
    case Echotron_Mod_Filter:
        Pmodfilts = value; //filter modulation on/off
        if (!Pmodfilts) initparams = 1;
        break;
    case Echotron_LFO_Type:
        //LFO Type
        lfo->PLFOtype = value;
        lfo->updateparams(fPERIOD);
        dlfo->PLFOtype = value;
        dlfo->updateparams(fPERIOD);
        break;
    case Echotron_Filters:
        Pfilters = value; //Pfilters
        break;
    }
}

int
Echotron::getpar(int npar)
{
    switch (npar)
    {
    case Echotron_DryWet:
        return (Pvolume);
        break;
    case Echotron_Depth:
        return (Pdepth);
        break;
    case Echotron_LFO_Width:
        return (Pwidth);
        break;
    case Echotron_Taps:
        return (Plength);
        break;
    case Echotron_User_File:
        return (Puser);
        break;
    case Echotron_Tempo:
        return (Ptempo);
        break;
    case Echotron_Damp:
        return (Phidamp);
        break;
    case Echotron_LR_Cross:
        return (Plrcross);
        break;
    case Echotron_Set_File:
        return (Filenum);
        break;
    case Echotron_LFO_Stereo:
        return (lfo->Pstereo);
        break;
    case Echotron_Feedback:
        return (Pfb);
        break;
    case Echotron_Pan:
        return (Ppanning);
        break;
    case Echotron_Mod_Delay:
        return (Pmoddly); //modulate delays
        break;
    case Echotron_Mod_Filter:
        return (Pmodfilts); //modulate filters
        break;
    case Echotron_LFO_Type:
        return (lfo->PLFOtype);
        break;
    case Echotron_Filters:
        return (Pfilters); //Filter delay line on/off
        break;
    }
    return (0); //in case of bogus parameter number
}
