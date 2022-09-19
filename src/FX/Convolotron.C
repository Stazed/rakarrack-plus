/*

  Convolotron.C - Convolotron effect
  Author: Ryam Billing & Jospe Andreu

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
#include <vector>
#include "Convolotron.h"

Convolotron::Convolotron(int DS, int uq, int dq, double sample_rate, uint16_t intermediate_bufsize) :
    Effect(DS, uq, dq, sample_rate, intermediate_bufsize),
    Filename(),
    SAMPLE_RATE((unsigned int) sample_rate),
    fSAMPLE_RATE((float) sample_rate),
    nSAMPLE_RATE(),
    nfSAMPLE_RATE(),
    PERIOD(intermediate_bufsize),
    nPERIOD(),
    fPERIOD(),
    Pvolume(50),
    Ppanning(64),
    Plrcross(100),
    Phidamp(60),
    Plevel(20),
    Psafe(1),
    Plength(50),
    Puser(),
    Filenum(),
    Pfb(),
    offset(),
    maxx_size(),
    maxx_read(),
    real_len(),
    length(),
    DS_state(),
    u_up(),
    u_down(),
    lpanning(((float) Ppanning + 0.5f) / 127.0f),
    rpanning(1.0f - lpanning),
    hidamp(1.0f - (float) Phidamp / 127.1f),
    alpha_hidamp(1.0f - hidamp),
    convlength(.5f),
    oldl(),
    rbuf(NULL),
    buf(NULL),
    lxn(NULL),
    templ(NULL),
    tempr(NULL),
    level(dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f)),
    fb(),
    feedback(),
    levpanl(lpanning * level * 2.0f),
    levpanr(rpanning * level * 2.0f),
    infile(NULL),
    sfinfo(),
    M_Resample(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    impulse()
{
    //default values
    adjust(DS, PERIOD);

    initialize();

    maxx_size = (int) (nfSAMPLE_RATE * convlength); //just to get the max memory allocated
    buf = (float *) malloc(sizeof (float) * maxx_size);
    memset(buf, 0, sizeof (float) * maxx_size);
    rbuf = (float *) malloc(sizeof (float) * maxx_size);
    memset(rbuf, 0, sizeof (float) * maxx_size);
    lxn = (float *) malloc(sizeof (float) * maxx_size);
    memset(lxn, 0, sizeof (float) * maxx_size);
    maxx_size--;
    maxx_read = maxx_size / 2;
    offset = 0;
    length = 1;
    oldl = 0.0f;

    M_Resample = new Resample(0);
    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq); //Downsample, uses sinc interpolation for bandlimiting to avoid aliasing
    
    setpreset(Ppreset);
    cleanup();
};

Convolotron::~Convolotron()
{
    clear_initialize();

    free(buf);
    free(rbuf);
    free(lxn);

    delete M_Resample;
    delete U_Resample;
    delete D_Resample;
}

/*
 * Cleanup the effect
 */
void
Convolotron::cleanup()
{
    memset(templ, 0, sizeof (float)*PERIOD);
    memset(tempr, 0, sizeof (float)*PERIOD);

    fb = 0.0f;
    feedback = 0.0f;
    oldl = 0.0f;
}


std::vector<int>
Convolotron::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_CONVO_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Convolotron::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_CONVO_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
}

#ifdef LV2_SUPPORT
void
Convolotron::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        clear_initialize();
        initialize();
    }
    else
    {
        PERIOD = period;
    }

    adjust(DS_state, period);
}
#endif // LV2

void
Convolotron::set_random_parameters()
{
    for(int i = 0; i < C_CONVO_PARAMETERS; i++)
    {
        switch(i)
        {
            case Convo_User_File:
            case Convo_SKIP_5:
            case Convo_SKIP_9:
                break;

            case Convo_DryWet:
            case Convo_Pan:
            case Convo_Level:
            case Convo_Damp:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Convo_Safe:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Convo_Feedback:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Convo_Length:
            {
                int value = (int) (RND * 245);
                changepar (i, value + 5);
            }
            break;

            case Convo_Set_File:
            {
                int value = (int) (RND * 9);
                changepar (i, value);
            }
            break;
        }
    }
}

void
Convolotron::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;

    // -4 for: Convo_User_File, Convo_SKIP_5, Convo_Set_File, Convo_SKIP_9
    for(int i = 0; i < (C_CONVO_PARAMETERS - 4); i++)
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Convo_Safe:
            case Convo_Damp:
            case Convo_Feedback:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), convo_parameters[i * 3 + 1], convo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Convo_Feedback )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Convo_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Convo_DryWet )), convo_parameters[i * 3 + 1], convo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Convo_DryWet )) );
                    s_buf += ":";
                }
            }
            break;
            
            // Offset
            case Convo_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Convo_Pan ) - 64, convo_parameters[i * 3 + 1], convo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Convo_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;
            
            // Skip two after these
            case Convo_Length:
            case Convo_Level:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), convo_parameters[i * 3 + 1], convo_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) );
                    s_buf += ":";
                }

                // increment for two skipped parameters
                param_case_offset += 2;
            }
            break;
        }
        param_case_offset++;
    }

    if(type == CARLA)
    {
        s_buf += "   <CustomData>\n";
        s_buf += "    <Type>http://lv2plug.in/ns/ext/atom#Path</Type>\n";
        s_buf += "    <Key>https://github.com/Stazed/rakarrack-plus#Convolotron:sndfile</Key>\n";
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
Convolotron::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);

    memset(templ, 0, sizeof (float)*PERIOD);
    memset(tempr, 0, sizeof (float)*PERIOD);
}

void
Convolotron::clear_initialize()
{
    free(templ);
    free(tempr);
}

void
Convolotron::adjust(int DS, uint32_t period)
{
    DS_state = DS;
    fPERIOD = (float) period;

    switch (DS)
    {
    case 0:
        nPERIOD = period;
        nSAMPLE_RATE = SAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        break;

    case 1:
        nPERIOD = lrintf(fPERIOD * 96000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        break;


    case 2:
        nPERIOD = lrintf(fPERIOD * 48000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        break;

    case 3:
        nPERIOD = lrintf(fPERIOD * 44100.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        break;

    case 4:
        nPERIOD = lrintf(fPERIOD * 32000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        break;

    case 5:
        nPERIOD = lrintf(fPERIOD * 22050.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        break;

    case 6:
        nPERIOD = lrintf(fPERIOD * 16000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        break;

    case 7:
        nPERIOD = lrintf(fPERIOD * 12000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        break;

    case 8:
        nPERIOD = lrintf(fPERIOD * 8000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        break;

    case 9:
        nPERIOD = lrintf(fPERIOD * 4000.0f / fSAMPLE_RATE);
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        break;
    }
    
    u_up = (double) nPERIOD / (double) period;
    u_down = (double) period / (double) nPERIOD;
}

/*
 * Effect output
 */
void
Convolotron::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    if (DS_state != 0)
    {
        memcpy(templ, efxoutl, sizeof (float)*PERIOD);
        memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
        U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
    }

    for (int i = 0; i < nPERIOD; i++)
    {
        float l = efxoutl[i] + efxoutr[i] + feedback;
        oldl = l * hidamp + oldl * (alpha_hidamp); //apply damping while I'm in the loop
        lxn[offset] = oldl;

        //Convolve left channel
        float lyn = 0;
        int xindex = offset;

        for (int j = 0; j < length; j++)
        {
            if (--xindex < 0)
            {
                xindex = maxx_size; //length of lxn is maxx_size.
            }
            
            lyn += buf[j] * lxn[xindex]; //this is all there is to convolution
        }

        feedback = fb * lyn;
        templ[i] = lyn * levpanl;
        tempr[i] = lyn * levpanr;
        
        if(isnan(templ[i]) || isnan(tempr[i]))
        {
            templ[i] = tempr[i] = 0.0;
            have_nans = true;
        }

        if (++offset > maxx_size)
            offset = 0;
    }

    if(have_nans)
        cleanup();

    if (DS_state != 0)
    {
        D_Resample->out(templ, tempr, efxoutl, efxoutr, nPERIOD, u_down);
    }
    else
    {
        memcpy(efxoutl, templ, sizeof (float)*PERIOD);
        memcpy(efxoutr, tempr, sizeof (float)*PERIOD);
    }
}

/*
 * Parameter control
 */
void
Convolotron::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
    
    if (_Pvolume == 0)
        cleanup();
}

void
Convolotron::setpanning(int _Ppanning)
{
    Ppanning = _Ppanning;
    rpanning = ((float) _Ppanning + 0.5f) / 127.0f;
    lpanning = 1.0f - rpanning;
    levpanl = lpanning * level * 2.0f;
    levpanr = rpanning * level * 2.0f;
}

int
Convolotron::setfile(int value)
{
    double sr_ratio;

    offset = 0;
    maxx_read = maxx_size / 2;
    memset(buf, 0, sizeof (float) * maxx_size);
    memset(rbuf, 0, sizeof (float) * maxx_size);

#ifdef LV2_SUPPORT
    /* LV2 will only call setfile() directly with Puser set and the value will be always USERFILE.
       This single case occurs upon initialization before Puser is set because setpreset() is called
       by initializer. So we just ignore the initial call for LV2 since the value will be incorrect
       and immediately replaced by the default file set in the ttl */
    if (value != USERFILE)
    {
        real_len = 1;
        length = 1;
        rbuf[0] = 1.0f;
        process_rbuf();
        return (0);
    }
#endif // LV2_SUPPORT

    if (!Puser)
    {
        Filenum = value;
        memset(Filename, 0, sizeof (Filename));
        sprintf(Filename, "%s/%d.wav", DATADIR, Filenum + 1);
    }
#ifndef LV2_SUPPORT // Rakarrack-plus only, user files must be in User Directory
    else    // User file
    {
        // For backwards compatibility and copying to another computer,
        // we check the filename only for a match in the user directory.
        // If a match is found, then we use the User Directory path.
        std::string file_name_clean = strrchr(Filename,'/')+1;     // get the file name W/O path
        int file_found = 0;
        
        // Check if the user file is in the User Directory
        for(unsigned i = 0; i < WAV_Files.size(); i++)
        {
            if(strcmp(file_name_clean.c_str(), WAV_Files[i].User_File_Name_Clean.c_str()) == 0)
            {
                // We found the file in the user directory so replace Filename with
                // the full path of the found file. In case the User Directory name
                // was changed, or copied to another computer with a different file
                // structure, and backwards compatible if the file was not originally
                // placed in the User Directory.
                file_found = 1;
                memset(Filename, 0, sizeof (Filename));
                sprintf(Filename, "%s", WAV_Files[i].User_File_Name.c_str());
                break;
            }
        }
        
        // We did not find the file in the User Directory, so error
        if(!file_found)
        {
            real_len = 1;
            length = 1;
            rbuf[0] = 1.0f;
            process_rbuf();
            global_error_number = 43;
            return (0);
        }
    }
#endif


    //    printf("Convolotron Filename %s: value %d: Filenum %d\n",Filename, value,Filenum);
    sfinfo.format = 0;
    if (!(infile = sf_open(Filename, SFM_READ, &sfinfo)))
    {
        real_len = 1;
        length = 1;
        rbuf[0] = 1.0f;
        process_rbuf();
#ifdef LV2_SUPPORT
        fprintf(stderr, "Convolotron - cannot open file = %s\n", Filename);
#else
        global_error_number = 1;
#endif
        return (0);
    }

    if (sfinfo.frames > maxx_read)
    {
        real_len = maxx_read;
    }
    else
    {
        real_len = sfinfo.frames;
    }
    
    sf_seek(infile, 0, SEEK_SET);
    sf_readf_float(infile, buf, real_len);
    sf_close(infile);

    if (sfinfo.samplerate != (int) nSAMPLE_RATE)
    {
        sr_ratio = (double) nSAMPLE_RATE / ((double) sfinfo.samplerate);
        M_Resample->mono_out(buf, rbuf, real_len, sr_ratio, lrint((double) real_len * sr_ratio));
        real_len = lrintf((float) real_len * (float) sr_ratio);
    }
    else
    {
        memcpy(rbuf, buf, real_len * sizeof (float));
    }

    UpdateLength(); // this calls process_rbuf() as well

    return (1);
}

void
Convolotron::process_rbuf()
{

    memset(buf, 0, sizeof (float)*real_len);

    if (length > real_len)
        length = real_len;
    /*Blackman Window function
    wn = a0 - a1*cos(2*pi*n/(N-1)) + a2 * cos(4*PI*n/(N-1)
    a0 = (1 - alpha)/2; a1 = 0.5; a2 = alpha/2
     */
    float alpha = 0.16f;
    float a0 = 0.5f * (1.0f - alpha);
    float a1 = 0.5f;
    float a2 = 0.5 * alpha;
    int N = length;
    int N2 = length / 2;
    float Nm1p = 0.0;
    float Nm1pp = 0.0;
    
    // if length == 1 == N because of failure to load file, then we get divide by 0
    if ( N != 1 )
    {
        Nm1p = D_PI / ((float) (N - 1));
        Nm1pp = 4.0f * PI / ((float) (N - 1));
    }

    float tailfader;    // initialize o.k.
    for (int ii = 0; ii < length; ii++)
    {
        if (ii < N2)
        {
            tailfader = 1.0f;
        }
        else
        {
            tailfader = a0 - a1 * cosf(ii * Nm1p) + a2 * cosf(ii * Nm1pp); //Calculate Blackman Window for right half of IR
        }

        buf[ii] = rbuf[ii] * tailfader; //Apply window function
    }

    //memcpy(buf,rbuf,real_len*sizeof(float));

    float IRpowa, IRpowb, maxamp;
    IRpowa = IRpowb = maxamp = 0.0f;
    //compute IR signal power
    for (int j = 0; j < length; j++)
    {
        IRpowa += fabsf(rbuf[j]);
        if (maxamp < fabsf(buf[j])) maxamp = fabsf(buf[j]); //find maximum level to normalize

        IRpowb += fabsf(buf[j]);
    }

    //if(maxamp < 0.3f) maxamp = 0.3f;
    float ngain = IRpowa / IRpowb;
    
    if (ngain > maxx_read)
        ngain = maxx_read;
    
    for (int j = 0; j < length; j++) buf[j] *= ngain;

    if (Psafe)
    {
        impulse.resample_impulse(length, buf);
        length = 156;
        convlength = length / fSAMPLE_RATE;
    }
    /*
    //This section can be uncommented to make a text file you can plot
    //with something like gnuplot
    FILE * textfile;
    textfile = fopen("IR.txt", "w");
    if (textfile!=NULL)
    {
    for(j=0;j<length;j++) {
    fprintf (textfile, "%d\t%f\n",j,buf[j]);
    }
    fclose (textfile);
    }
     */
}

void
Convolotron::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = 1.0f - (float) _Phidamp / 127.1f;
    alpha_hidamp = 1.0f - hidamp;
}

void
Convolotron::UpdateLength()
{
    convlength = ((float) Plength) / 1000.0f; //time in seconds
    length = (int) (nfSAMPLE_RATE * convlength); //time in samples
    process_rbuf();
}

void
Convolotron::setpreset(int npreset)
{
    const int PRESET_SIZE = C_CONVO_PARAMETERS;
    const int NUM_PRESETS = 4;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Convolotron 1
        {67, 64, 1, 100, 0, 64, 30, 20, 0, 0, 0},
        //Convolotron 2
        {67, 64, 1, 100, 0, 64, 30, 20, 1, 0, 0},
        //Convolotron 3
        {67, 75, 1, 100, 0, 64, 30, 20, 2, 0, 0},
        //Convolotron 4
        {67, 60, 1, 100, 0, 64, 30, 20, 3, 0, 0}
    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_CONVOLOTRON, npreset - NUM_PRESETS + 1, pdata, Filename);
        
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
Convolotron::changepar(int npar, int value)
{
    switch (npar)
    {
    case Convo_DryWet:
        setvolume(value);
        break;
    case Convo_Pan:
        setpanning(value);
        break;
    case Convo_Safe:
        Psafe = value;
        UpdateLength();
        break;
    case Convo_Length:
        Plength = value;
        UpdateLength();
        break;
    case Convo_User_File:
        Puser = value;
        break;
    case Convo_SKIP_5:
        break;
    case Convo_Damp:
        sethidamp(value);
        break;
    case Convo_Level:
        Plevel = value;
        level = dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f);
        levpanl = lpanning * level * 2.0f;
        levpanr = rpanning * level * 2.0f;
        break;
    case Convo_Set_File:
        setfile(value);
        break;
    case Convo_SKIP_9:
        break;
    case Convo_Feedback:
        Pfb = value;
        if (Pfb < 0)
        {
            fb = (float) .1f * value / 250.0f * .15f;
        }
        else
        {
            fb = (float) .1f * value / 500.0f * .15f;
        }
        break;
    default:
        return;
    }
}

int
Convolotron::getpar(int npar)
{
    switch (npar)
    {
    case Convo_DryWet:
        return (Pvolume);

    case Convo_Pan:
        return (Ppanning);

    case Convo_Safe:
        return (Psafe);

    case Convo_Length:
        return (Plength);

    case Convo_User_File:
        return (Puser);

    case Convo_SKIP_5:
        return (0);

    case Convo_Damp:
        return (Phidamp);

    case Convo_Level:
        return (Plevel);

    case Convo_Set_File:
        return (Filenum);

    case Convo_SKIP_9:
        return (0);

    case Convo_Feedback:
        return (Pfb);

    }
    return (0); //in case of bogus parameter number
}
