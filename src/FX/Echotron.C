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
    Effect(sample_rate, intermediate_bufsize),
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
    filterbank()
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
Echotron::set_random_parameters()
{
    for(int i = 0; i < C_ECHOTRON_PARAMETERS; i++)
    {
        switch(i)
        {
            case Echotron_DryWet:
            case Echotron_Pan:
            case Echotron_LFO_Width:
            case Echotron_Damp:
            case Echotron_LFO_Stereo:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Echotron_Tempo:
            {
                int value = (int) (RND * 600);
                changepar (i, value + 1);
            }
            break;

            case Echotron_Feedback:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Echotron_LR_Cross:
            case Echotron_Depth:
            {
                int value = (int) (RND * 129);
                changepar (i, value);
            }
            break;

            case Echotron_LFO_Type:
            {
                int value = (int) (RND * 12);
                changepar (i, value);
            }
            break;

            case Echotron_Mod_Delay:
            case Echotron_Mod_Filter:
            case Echotron_Filters:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Echotron_Set_File:
            {
                int value = (int) (RND * 11);
                changepar (i, value);
            }
            break;

            case Echotron_Taps:
            case Echotron_User_File:
                break;
        }
    }
}

void
Echotron::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    for(int i = 0; i < (C_ECHOTRON_PARAMETERS - 2); i++)    // -2 for skip user file and set file
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Echotron_LFO_Width:
            case Echotron_Tempo:
            case Echotron_Damp:
            case Echotron_LFO_Stereo:
            case Echotron_Feedback:
            case Echotron_Mod_Delay:
            case Echotron_Mod_Filter:
            case Echotron_LFO_Type:
            case Echotron_Filters:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), echotron_parameters[i * 3 + 1], echotron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Echotron_Filters )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Echotron_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Echotron_DryWet )), echotron_parameters[i * 3 + 1], echotron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Echotron_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Echotron_Depth:
            case Echotron_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ) - 64, echotron_parameters[i * 3 + 1], echotron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) - 64);
                    s_buf += ":";
                }
            }
            break;

            // Skip user file
            case Echotron_Taps:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Echotron_Taps ), echotron_parameters[i * 3 + 1], echotron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Echotron_Taps ) );
                    s_buf += ":";
                }

                param_case_offset++;    // skip user file
            }
            break;

            // Offset & skip Set file
            case Echotron_LR_Cross:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Echotron_LR_Cross ) - 64, echotron_parameters[i * 3 + 1], echotron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Echotron_LR_Cross ) - 64);
                    s_buf += ":";
                }

                param_case_offset++;    // skip set file
            }
            break;
        }

        param_case_offset++;
    }

    if(type == CARLA)
    {
        s_buf += "   <CustomData>\n";
        s_buf += "    <Type>http://lv2plug.in/ns/ext/atom#Path</Type>\n";
        s_buf += "    <Key>https://github.com/Stazed/rakarrack-plus#Echotron:dlyfile</Key>\n";
        s_buf += "    <Value>";
        s_buf += Filename;
        s_buf += "</Value>\n";
        s_buf += "   </CustomData>\n";
    }
    else
    {
        s_buf += "\" :filename \"";
        s_buf += Filename;
    }
}

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
    bool have_nans = false;

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

        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
    }

    if(have_nans)
        cleanup();

    if (initparams) init_params();
}

/*
 * Parameter control
 */
void
Echotron::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
    
    if (_Pvolume == 0)
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
#ifndef LV2_SUPPORT // Rakarrack-plus only, user files must be in User Directory
    else
    {
        // For backwards compatibility and copying to another computer,
        // we check the filename only for a match in the user directory.
        // If a match is found, then we use the User Directory path.
        std::string file_name_clean = strrchr(Filename,'/')+1;     // get the file name W/O path
        int file_found = 0;
        
        // Check if the user file is in the User Directory
        for(unsigned i = 0; i < DLY_Files.size(); i++)
        {
            if(strcmp(file_name_clean.c_str(), DLY_Files[i].User_File_Name_Clean.c_str()) == 0)
            {
                // We found the file in the user directory so replace Filename with
                // the full path of the found file. In case the User Directory name
                // was changed, or copied to another computer with a different file
                // structure, and backwards compatible if the file was not originally
                // placed in the User Directory.
                file_found = 1;
                memset(Filename, 0, sizeof (Filename));
                sprintf(Filename, "%s", DLY_Files[i].User_File_Name.c_str());
                break;
            }
        }
        
        // We did not find the file in the User Directory, so error & set defaults
        if(!file_found)
        {
            filedata = loaddefault();
            applyfile(filedata);
            error = global_error_number = 44;
            return 0;
        }
    }
#endif

    //    printf("Filename %s\n",Filename);
    filedata = loadfile(Filename);
    applyfile(filedata);
    
    if (error)
        return 0;
    
    return 1;
}

bool
Echotron::check_delay_file_ranges(double value, int item)
{
    bool ret = true;
    error = 0;

    switch(item)
    {
        case Dly_Pan:
        {
            if ((value < -1.0f) || (value > 1.0f))
            {
                error = Dly_Pan;
                ret = false;
            }
            break;
        }
        case Dly_Time:
        {
            if ((value < -6.0f) || (value > 6.0f))
            {
                error = Dly_Time;
                ret = false;
            }
            break;
        }
        case Dly_Level:
        {
            if ((value < -10.0f) || (value > 10.0f))
            {
                error = Dly_Level;
                ret = false;
            }
            break;
        }
        case Dly_LP:
        case Dly_BP:
        case Dly_HP:
        {
            if ((value < -2.0f) || (value > 2.0f))
            {
                error = item;
                ret = false;
            }
            break;
        }
        case Dly_Freq:
        {
            if ((value < 20.0f) || (value > 26000.0f))
            {
                error = Dly_Freq;
                ret = false;
            }
            break;
        }
        case Dly_Q:
        {
            if ((value < 0.0f) || (value > 300.0f))
            {
                error = Dly_Q;
                ret = false;
            }
            break;
        }
        case Dly_Stages:
        {
            if ((value < 0.0f) || (value > MAX_FILTER_STAGES))
            {
                error = Dly_Stages;
                ret = false;
            }
            break;
        }
    }

#ifndef LV2_SUPPORT
    if (!ret)
        global_error_number = error;
#endif

    return ret;
}

DlyFile
Echotron::loadfile(char* Filename)
{
#ifdef LV2_SUPPORT
    FILENAME = Filename; // For lv2 if need to re-initialize and reload file
#endif // LV2
    
    double tPan = 0.0f;
    double tTime = 0.0f;
    double tLevel = 0.0f;
    double tLP = 0.0f;
    double tBP = 0.0f;
    double tHP = 0.0f;
    double tFreq = 20.0f;
    double tQ = 1.0f;
    int tiStages = 0;

    FILE *fs;
    DlyFile f;

    char wbuf[128];

    error = 0;

    if ((fs = fopen(Filename, "r")) == NULL)
    {
        error = Dly_Open;
#ifndef LV2_SUPPORT
        global_error_number = error;
#else
        fprintf(stderr, "Echotron file open error #%d - %s\n", error, Filename);
#endif
        return loaddefault();
    }

    strcpy(f.Filename, Filename); // Must copy the file name here for lv2 save when if(plug->file_changed) in rkr.lv2.C

    int first_line = 0;
    while (fgets(wbuf, sizeof wbuf, fs) != NULL)
    {
        // Get the description
        if ( (wbuf[0] == '#') && (first_line == 0) )
        {
            f.Description = wbuf;
            if (!f.Description.empty())
            {
                f.Description.pop_back();   // remove \n
                if (!f.Description.empty())
                {
                    f.Description.erase(f.Description.begin()); // remove #
                }
            }
        }
        
        ++first_line;   // only check for description on first line

        if (wbuf[0] != '#') break;
        
        memset(wbuf, 0, sizeof (wbuf));
    }

    sscanf(wbuf, "%lf\t%lf\t%d", &f.subdiv_fmod, &f.subdiv_dmod, &f.f_qmode); //Second line has tempo subdivision
//    printf("subdivs:%f\t%f\t%d\n",f.subdiv_fmod,f.subdiv_dmod, f.f_qmode);

    int count = 0;
    memset(f.iStages, 0, sizeof (f.iStages));

    while ((fgets(wbuf, sizeof wbuf, fs) != NULL) && (count < ECHOTRON_F_SIZE))
    {
        if (wbuf[0] == 10) break; // Check Carriage Return
        
        if (wbuf[0] == '#') continue;
        
        sscanf(wbuf, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%d", &tPan, &tTime, &tLevel,
               &tLP, &tBP, &tHP, &tFreq, &tQ, &tiStages);
        //printf("params:\n%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d\n",tPan, tTime, tLevel,
        //tLP,  tBP,  tHP,  tFreq,  tQ,  tiStages);
        
        if ( !check_delay_file_ranges( tPan, Dly_Pan ) ) break;
        else f.fPan[count] = tPan;
        
        if ( !check_delay_file_ranges( tTime, Dly_Time ) ) break;
        else f.fTime[count] = fabs(tTime);

        if ( !check_delay_file_ranges( tLevel, Dly_Level ) ) break;
        else f.fLevel[count] = tLevel;

        if ( !check_delay_file_ranges( tLP, Dly_LP ) ) break;
        else f.fLP[count] = tLP;

        if ( !check_delay_file_ranges( tBP, Dly_BP ) ) break;
        else f.fBP[count] = tBP;

        if ( !check_delay_file_ranges( tHP, Dly_HP ) ) break;
        else f.fHP[count] = tHP;

        if ( !check_delay_file_ranges( tFreq, Dly_Freq ) ) break;
        else f.fFreq[count] = tFreq;

        if ( !check_delay_file_ranges( tQ, Dly_Q ) ) break;
        else f.fQ[count] = tQ;

        if ( !check_delay_file_ranges( (double) tiStages, Dly_Stages ) ) break;
        else f.iStages[count] = tiStages - 1; //check in main loop if < 0, then skip filter

        memset(wbuf, 0, sizeof (wbuf));
        count++;
    }
    
    fclose(fs);

    f.fLength = count;
    
    // Set Taps (Plength) to lessor of fLength or 127
    Plength = f.fLength > 127 ? 127 : f.fLength;

    if(error)
    {
#ifdef LV2_SUPPORT
    fprintf(stderr, "Echotron file loading error #%d - %s\n", error, Filename);
#endif
        return loaddefault();
    }    

    return f;
}

void
Echotron::applyfile(const DlyFile &f)
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
    f.fPan[0] = 0.0f;
    f.fTime[0] = 1.0f; //default 1 measure delay
    f.fLevel[0] = 0.7f;
    f.fLP[0] = 1.0f;
    f.fBP[0] = -1.0f;
    f.fHP[0] = 1.0f;
    f.fFreq[0] = 800.0f;
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
Echotron::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = 1.0f - (float) _Phidamp / 127.1f;
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
        Fpre->ReadPreset(EFX_ECHOTRON, npreset - NUM_PRESETS + 1, pdata, Filename);
        
        for (int n = 0; n < PRESET_SIZE; n++)
        {
            changepar(n, pdata[n]);
        }

        // We have to reset the Taps because we limit it to get_file_length().
        // But, it is set before the loaded file and would be limited by the 
        // previous file. So, reset it again after the requested file is loaded.
        changepar(Echotron_Taps, pdata[Echotron_Taps]);
    }
    else
    {
        for (int n = 0; n < PRESET_SIZE; n++)
        {
            changepar(n, presets[npreset][n]);
        }

        // We have to reset the Taps because we limit it to get_file_length().
        // But, it is set before the loaded file and would be limited by the 
        // previous file. So, reset it again after the requested file is loaded.
        changepar(Echotron_Taps, presets[npreset][Echotron_Taps]);
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
    {
        // Limit Tap to file length and max of 127
        int Taps = value > get_file_length() ? get_file_length() : value;
        Taps = Taps > 127 ? 127 : Taps;
        Plength = Taps;
        initparams = 1;
        break;
    }
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
    {
#ifdef LV2_SUPPORT
        setfile(value); // This will only be called by changepar() upon initialization for lv2 and is ignored.
#else
        if (!setfile(value))
        {
            error = 0;
        }
#endif
        break;
    }
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
    default:
        return;
    }
}

int
Echotron::getpar(int npar)
{
    switch (npar)
    {
    case Echotron_DryWet:
        return (Pvolume);

    case Echotron_Depth:
        return (Pdepth);

    case Echotron_LFO_Width:
        return (Pwidth);

    case Echotron_Taps:
        return (Plength);

    case Echotron_User_File:
        return (Puser);

    case Echotron_Tempo:
        return (Ptempo);

    case Echotron_Damp:
        return (Phidamp);

    case Echotron_LR_Cross:
        return (Plrcross);

    case Echotron_Set_File:
        return (Filenum);

    case Echotron_LFO_Stereo:
        return (lfo->Pstereo);

    case Echotron_Feedback:
        return (Pfb);

    case Echotron_Pan:
        return (Ppanning);

    case Echotron_Mod_Delay:
        return (Pmoddly); //modulate delays

    case Echotron_Mod_Filter:
        return (Pmodfilts); //modulate filters

    case Echotron_LFO_Type:
        return (lfo->PLFOtype);

    case Echotron_Filters:
        return (Pfilters); //Filter delay line on/off

    }
    return (0); //in case of bogus parameter number
}
