/*

  Reverbtron.C - Reverbtron effect
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
#include "Reverbtron.h"

Reverbtron::Reverbtron(int DS, int uq, int dq,
                       double sample_rate, uint32_t intermediate_bufsize) :
    Effect(DS, uq, dq, sample_rate, intermediate_bufsize),
    Filename(),
    File(),
    PERIOD(intermediate_bufsize),
    nPERIOD(),
    nSAMPLE_RATE((int) sample_rate),
    fSAMPLE_RATE(sample_rate),
    nfSAMPLE_RATE(sample_rate),
    Pvolume(50),
    Ppanning(64),
    Plrcross(100),
    Phidamp(60),
    Plevel(),
    Plength(20),
    Puser(),
    Pstretch(),
    Pidelay(),
    Filenum(),
    Psafe(),
    Pfb(1),
    Pfade(1),
    Pes(),
    Prv(),
    Plpf(20000),
    Pdiff(1),
    imctr(),
    imax(),
    offset(),
    hoffset(),
    maxx_size(),
    error(),
    Llength(50),
    hrtf_size(),
    hlength(),
    DS_state(DS),
    nRATIO(),
    time(NULL),
    rndtime(NULL),
    u_up(),
    u_down(),
    fstretch(1.0f),
    idelay(1.0f),
    ffade(),
    decay(),
    diffusion(),
    lpanning(),
    rpanning(),
    hidamp(0.0f),
    alpha_hidamp(1.0f - hidamp),
    convlength(10.0f),  //  max reverb time
    oldl(),
    data(NULL),
    lxn(NULL),
    imdelay(NULL),
    rnddata(NULL),
    hrtf(NULL),
    templ(NULL),
    tempr(NULL),
    level(),
    fb(),
    feedback(),
    levpanl(),
    levpanr(),
    roomsize(1.0f),
    U_Resample(NULL),
    D_Resample(NULL),
    interpbuf(NULL),
    lpfl(NULL),
    lpfr(NULL)
{
    //  default values
    adjust(DS, sample_rate);

    hrtf_size = nSAMPLE_RATE / 2;
    maxx_size = (int) (nfSAMPLE_RATE * convlength); //  just to get the max memory allocated
    time = (int *) malloc(sizeof (int) * 2000);
    rndtime = (int *) malloc(sizeof (int) * 2000);
    data = (float *) malloc(sizeof (float) * (1 + hrtf_size));
    rnddata = (float *) malloc(sizeof (float) * 2000);
    lxn = (float *) malloc(sizeof (float) * (1 + maxx_size));
    hrtf = (float *) malloc(sizeof (float) * (1 + hrtf_size));
    imax = nSAMPLE_RATE / 2;                        // 1/2 second available
    imdelay = (float *) malloc(sizeof (float) * imax);
    decay = f_exp(-1.0f / (0.2f * nfSAMPLE_RATE));  //   0.2 seconds

    initialize();
    File = loaddefault();

    U_Resample = new Resample(uq);
    D_Resample = new Resample(dq);  //Downsample, uses sinc interpolation for bandlimiting to avoid aliasing

    setpreset(Ppreset);
    cleanup();
}

Reverbtron::~Reverbtron()
{
    clear_initialize();

    free(time);
    free(rndtime);
    free(data);
    free(rnddata);
    free(lxn);
    free(hrtf);
    free(imdelay);

    delete U_Resample;
    delete D_Resample;
}

/*
 * Cleanup the effect
 */
void
Reverbtron::cleanup()
{
    memset(lxn, 0, sizeof (float)*(maxx_size + 1));
    memset(hrtf, 0, sizeof (float)*(hrtf_size + 1));

    memset(imdelay, 0, sizeof (float)*imax);
    memset(rnddata, 0, sizeof (float)* 2000);

    memset(templ, 0, sizeof (float)*PERIOD);
    memset(tempr, 0, sizeof (float)*PERIOD);

    feedback = 0.0f;
    oldl = 0.0f;
    lpfl->cleanup();
    lpfr->cleanup();
}

std::vector<int>
Reverbtron::save_parameters()
{
    std::vector<int> parameters;
    for(int i = 0; i < C_REVTRON_PARAMETERS; i++)
    {
        parameters.push_back(getpar(i));
    }
    
    return parameters;
}

void
Reverbtron::reset_parameters(std::vector<int> parameters)
{
    for(int i = 0; i < C_REVTRON_PARAMETERS; i++)
    {
        changepar(i, parameters[i]);
    }
    
    cleanup();
}

#ifdef LV2_SUPPORT
void
Reverbtron::lv2_update_params(uint32_t period)
{
    if (period > PERIOD) // only re-initialize if period > intermediate_bufsize of declaration
    {
        PERIOD = period;
        adjust(DS_state, fSAMPLE_RATE);
        clear_initialize();
        initialize();
        setlpf(Plpf);
        sethidamp(Phidamp);
    }
    else
    {
        PERIOD = period;
        adjust(DS_state, fSAMPLE_RATE);
    }
}
#endif // LV2

void
Reverbtron::set_random_parameters()
{
    for(int i = 0; i < C_REVTRON_PARAMETERS; i++)
    {
        switch(i)
        {
            case Revtron_DryWet:
            case Revtron_Pan:
            case Revtron_Level:
            case Revtron_Damp:
            case Revtron_Fade:
            case Revtron_Diffusion:
            {
                int value = (int) (RND * 128);
                changepar (i, value);
            }
            break;

            case Revtron_Safe:
            case Revtron_Ex_Stereo:
            case Revtron_Shuffle:
            {
                int value = (int) (RND * 2);
                changepar (i, value);
            }
            break;

            case Revtron_I_Delay:
            {
                int value = (int) (RND * 501);
                changepar (i, value);
            }
            break;

            case Revtron_Stretch:
            case Revtron_Feedback:
            {
                int value = (int) (RND * 129);
                changepar (i, value - 64);
            }
            break;

            case Revtron_LPF:
            {
                int value = (int) (RND * 25980);
                changepar (i, value + 20);
            }
            break;

            case Revtron_Length:
            {
                int value = (int) (RND * 1480);
                changepar (i, value + 20);
            }
            break;

            case Revtron_Set_File:
            {
                int value = (int) (RND * 10);
                changepar (i, value);
            }
            break;

            case Revtron_User_File:
                break;
        }
    }
}

void
Reverbtron::LV2_parameters(std::string &s_buf, int type)
{
    int param_case_offset = 0;
    for(int i = 0; i < (C_REVTRON_PARAMETERS - 2); i++) // -2 Revtron_User_File, Revtron_Set_File
    {
        switch(param_case_offset)
        {
            // Normal processing
            case Revtron_Fade:
            case Revtron_Safe:
            case Revtron_I_Delay:
            case Revtron_Damp:
            case Revtron_Stretch:
            case Revtron_Feedback:
            case Revtron_Ex_Stereo:
            case Revtron_Shuffle:
            case Revtron_LPF:
            case Revtron_Diffusion:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), revtron_parameters[i * 3 + 1], revtron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ));

                    if ( param_case_offset !=  Revtron_Diffusion )   // last one no need for delimiter
                        s_buf += ":";
                }
            }
            break;

            // Special cases
            // wet/dry -> dry/wet reversal
            case Revtron_DryWet:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, Dry_Wet(getpar( Revtron_DryWet )), revtron_parameters[i * 3 + 1], revtron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( Dry_Wet(getpar( Revtron_DryWet )) );
                    s_buf += ":";
                }
            }
            break;

            // Offset
            case Revtron_Pan:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( Revtron_Pan ) - 64, revtron_parameters[i * 3 + 1], revtron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( Revtron_Pan ) - 64);
                    s_buf += ":";
                }
            }
            break;

            // Skip parameter
            case Revtron_Length:
            case Revtron_Level:
            {
                if(type == CARLA)
                {
                    Carla_LV2_port(s_buf, i + 1, getpar( param_case_offset ), revtron_parameters[i * 3 + 1], revtron_parameters[i * 3 + 2]);
                }
                else
                {
                    s_buf += NTS( getpar( param_case_offset ) );
                    s_buf += ":";
                }

                param_case_offset++;    // skips Revtron_User_File, Revtron_Set_File
            }
            break;
        }

        param_case_offset++;
    }

    if(type == CARLA)
    {
        s_buf += "   <CustomData>\n";
        s_buf += "    <Type>http://lv2plug.in/ns/ext/atom#Path</Type>\n";
        s_buf += "    <Key>https://github.com/Stazed/rakarrack-plus#Reverbtron:rvbfile</Key>\n";
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
Reverbtron::initialize()
{
    templ = (float *) malloc(sizeof (float) * PERIOD);
    tempr = (float *) malloc(sizeof (float) * PERIOD);

    interpbuf = new float[PERIOD];
    lpfl = new AnalogFilter(0, 800, 1, 0, nfSAMPLE_RATE, interpbuf);
    lpfr = new AnalogFilter(0, 800, 1, 0, nfSAMPLE_RATE, interpbuf);
    lpfl->setSR(nSAMPLE_RATE);
    lpfr->setSR(nSAMPLE_RATE);
}

void
Reverbtron::clear_initialize()
{
    free(templ);
    free(tempr);

    delete[] interpbuf;
    delete lpfl;
    delete lpfr;
}

/*
 * Effect output
 */
void
Reverbtron::out(float * efxoutl, float * efxoutr)
{
    bool have_nans = false;

    int length = Llength;
    hlength = Pdiff;

    if (DS_state != 0)
    {
        memcpy(templ, efxoutl, sizeof (float)*PERIOD);
        memcpy(tempr, efxoutr, sizeof (float)*PERIOD);
        U_Resample->out(templ, tempr, efxoutl, efxoutr, PERIOD, u_up);
    }


    for (int i = 0; i < nPERIOD; i++)
    {
        if(isnan(efxoutl[i]) || isnan(efxoutr[i]))
        {
            efxoutl[i] = efxoutr[i] = 0.0;
            have_nans = true;
        }
        
        float l = 0.5f * (efxoutr[i] + efxoutl[i]);
        oldl = l * hidamp + oldl * (alpha_hidamp); //apply damping while I'm in the loop
        if (Prv)
        {
            oldl = 0.5f * oldl - efxoutl[i];
        }

        lxn[offset] = oldl;

        //Convolve
        float lyn = 0.0f;
        int xindex = offset;

        for (int j = 0; j < length; j++)
        {
            xindex = offset + time[j];
            if (xindex >= maxx_size) xindex -= maxx_size;
            lyn += lxn[xindex] * data[j]; //this is all of the magic
        }

        hrtf[hoffset] = lyn;

        if (Pdiff > 0)
        {
            //Convolve again with approximated hrtf
            float hyn = 0.0f;
            int hindex = hoffset;

            for (int j = 0; j < hlength; j++)
            {
                hindex = hoffset + rndtime[j];
                if (hindex >= hrtf_size) hindex -= hrtf_size;
                hyn += hrtf[hindex] * rnddata[j]; //more magic
            }
            
            lyn = hyn + (1.0f - diffusion) * lyn;
        }

        if (Pes)
        { // just so I have the code to get started

            float ldiff = lyn;
            float rdiff = imdelay[imctr];

            ldiff = lpfl->filterout_s(ldiff);
            rdiff = lpfr->filterout_s(rdiff);

            imdelay[imctr] = decay*ldiff;
            imctr--;
            
            if (imctr < 0) imctr = roomsize;

            templ[i] = (lyn + ldiff) * levpanl;
            tempr[i] = (lyn + rdiff) * levpanr;

            feedback = fb * rdiff*decay;
        }
        else
        {
            feedback = fb * lyn;
            templ[i] = lyn * levpanl;
            tempr[i] = lyn * levpanr;
        }

        offset--;
        
        if (offset < 0)
        {
            offset = maxx_size;
        }

        int doffset = (offset + roomsize);
        
        if (doffset > maxx_size)
        {
            doffset -= maxx_size;
        }

        hoffset--;
        
        if (hoffset < 0)
        {
            hoffset = hrtf_size;
        }

        lxn[doffset] += feedback;

        xindex = offset + roomsize;
    }
    
    if(have_nans)
        convert_time();

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
Reverbtron::setvolume(int _Pvolume)
{
    Pvolume = _Pvolume;
    outvolume = (float) _Pvolume / 127.0f;
    
    if (_Pvolume == 0)
    {
        cleanup();
    }
}

void
Reverbtron::setpanning(int value)
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
    levpanl = level*lpanning;
    levpanr = level*rpanning;
}

//legacy

int
Reverbtron::setfile(int value)
{
    RvbFile filedata;

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

    /* !Puser means one of the default files, so set the Filenum to get it */
    if (!Puser)
    {
        Filenum = value;
        memset(Filename, 0, sizeof (Filename));
        sprintf(Filename, "%s/%d.rvb", DATADIR, Filenum + 1); // DATADIR comes from CMakeLists.txt
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
        for(unsigned i = 0; i < RVB_Files.size(); i++)
        {
            if(strcmp(file_name_clean.c_str(), RVB_Files[i].User_File_Name_Clean.c_str()) == 0)
            {
                // We found the file in the user directory so replace Filename with
                // the full path of the found file. In case the User Directory name
                // was changed, or copied to another computer with a different file
                // structure, and backwards compatible if the file was not originally
                // placed in the User Directory.
                file_found = 1;
                memset(Filename, 0, sizeof (Filename));
                sprintf(Filename, "%s", RVB_Files[i].User_File_Name.c_str());
                break;
            }
        }
        
        // We did not find the file in the User Directory, so error
        if(!file_found)
        {
            filedata = loaddefault();
            applyfile(filedata);
            error = 45;
            return (0);
        }
    }
#endif

//    printf("Reverbtron Filename %s\n",Filename);
    
    filedata = loadfile(Filename);
    applyfile(filedata);
    
    if (error)
    {
        return 0;
    }
    
    return 1;

    /*
    if ((fs = fopen (Filename, "r")) == NULL) {
        File = loaddefault();
        cleanup();
        convert_time();
        return(0);
    }
    cleanup();
    memset(File.tdata, 0, sizeof(float)*2000);
    memset(File.ftime, 0, sizeof(float)*2000);


//Name
    memset(wbuf,0, sizeof(wbuf));
    fgets(wbuf,sizeof wbuf,fs);

// Subsample Compresion Skip
    memset(wbuf,0, sizeof(wbuf));
    fgets(wbuf,sizeof wbuf,fs);
    sscanf(wbuf,"%f,%f\n",&compresion,&quality);

//Length
    memset(wbuf,0,sizeof(wbuf));
    fgets(wbuf,sizeof wbuf,fs);
    sscanf(wbuf, "%d\n", &File.data_length);
    if(File.data_length>2000) File.data_length = 2000;
//Time Data
    for(i=0; i<File.data_length; i++) {
        memset(wbuf,0, sizeof(wbuf));
        fgets(wbuf,sizeof wbuf,fs);
        sscanf(wbuf,"%f,%f\n",&File.ftime[i],&File.tdata[i]);
    }

    fclose(fs);

    File.maxtime = 0.0f;
    File.maxdata = 0.0f;
    float averaget = 0.0f;
    float tempor = 0.0f;
    for(i=0; i<File.data_length; i++) {
        if(File.ftime[i] > File.maxtime) File.maxtime = File.ftime[i];
        if(File.tdata[i] > File.maxdata) File.maxdata = File.tdata[i];  //used to normalize so feedback is more predictable
        if(i>0) {
            tempor = File.ftime[i] - File.ftime[i-1];
            if(tempor>averaget) averaget = tempor;
        }
    }

    cleanup();
    convert_time();
    return(1);
     */
};

RvbFile
Reverbtron::loadfile(char* filename)
{
    FILE *fs;

    RvbFile f;
    error = 0;

    if ((fs = fopen(filename, "r")) == NULL)
    {
#ifdef LV2_SUPPORT
        fprintf(stderr, "Reverbtron - file open error %s\n", filename);
#endif
        f = loaddefault();
        error = 2;
        return (f);
    }
    
    strcpy(f.Filename, filename);
    memset(f.tdata, 0, sizeof (float)*2000);
    memset(f.ftime, 0, sizeof (float)*2000);

    //  Name
    char wbuf[128];
    memset(wbuf, 0, sizeof (wbuf));
    
    if (fgets(wbuf, sizeof wbuf, fs) == NULL)
    {
        f = loaddefault();
        error = 2;
        fclose(fs);
        return (f);
    }

    //  Subsample Compression Skip
    memset(wbuf, 0, sizeof (wbuf));
    
    if (fgets(wbuf, sizeof wbuf, fs) == NULL)
    {
        f = loaddefault();
        error = 2;
        fclose(fs);
        return (f);
    }

    float compression = 0.0f;
    float quality = 0.0f;
    sscanf(wbuf, "%f,%f\n", &compression, &quality);

    //  Length
    memset(wbuf, 0, sizeof (wbuf));
    
    if (fgets(wbuf, sizeof wbuf, fs) == NULL)
    {
        f = loaddefault();
        error = 2;
        fclose(fs);
        return (f);
    }
    
    sscanf(wbuf, "%d\n", &f.data_length);
    
    if (f.data_length > 2000) f.data_length = 2000;
    
    //  Time Data
    for (int i = 0; i < f.data_length; i++)
    {
        memset(wbuf, 0, sizeof (wbuf));
        
        if (fgets(wbuf, sizeof wbuf, fs) == NULL)
        {
            f = loaddefault();
            error = 2;
            fclose(fs);
            return (f);
        }
        sscanf(wbuf, "%f,%f\n", &f.ftime[i], &f.tdata[i]);
    }

    fclose(fs);

    f.maxtime = 0.0f;
    f.maxdata = 0.0f;
    //    float averaget = 0.0f;
    //    float tempor = 0.0f;
    for (int i = 0; i < f.data_length; i++)
    {
        if (f.ftime[i] > f.maxtime) f.maxtime = f.ftime[i];
        if (f.tdata[i] > f.maxdata) f.maxdata = f.tdata[i]; //used to normalize so feedback is more predictable
        //        if(i>0) {
        //            tempor = f.ftime[i] - f.ftime[i-1];       // f.ftime does not get altered here 
        //            if(tempor>averaget) averaget = tempor;    // averaget & tempor do not get used elsewhere - this has no effect
        //        }
    }

    return f;
}

void
Reverbtron::applyfile(const RvbFile &file)
{
    File = file;
    convert_time();
}

RvbFile Reverbtron::loaddefault()
{
    RvbFile f;
    strcpy(f.Filename, "default");
    f.data_length = Llength = 2;
    f.ftime[0] = 1.0f;
    f.ftime[1] = 1.25f;
    f.tdata[0] = 0.75f;
    f.tdata[1] = 0.5f;
    f.maxtime = 1.25f;
    f.maxdata = 0.75f;
    return f;
}

void Reverbtron::convert_time()
{

    memset(data, 0, sizeof (float)*(1 + hrtf_size));
    memset(time, 0, sizeof (int)*2000);
    memset(rndtime, 0, sizeof (int)*2000);

    if (Llength >= File.data_length)
    {
        Llength = File.data_length;
    }

    if (Llength == 0)
    {
        Llength = 400;
    }
    
    float incr = ((float) Llength) / ((float) File.data_length);

    float tmpstretch = 1.0f;
    if (fstretch > 0.0)
    {
        tmpstretch = 1.0f + fstretch * (convlength / File.maxtime);
    }
    else
    {
        tmpstretch = 1.0f + 0.95f * fstretch;
    }

    float skip = 0.0f;
    int index = 0;
    float normal = 0.9999f / File.maxdata;

    if (File.data_length > Llength)
    {
        for (int i = 0; i < File.data_length; i++)
        {
            skip += incr;
            float findex = (float) index;
            if (findex < skip)
            {
                if (index < Llength)
                {
                    if ((tmpstretch * (idelay + File.ftime[i])) > 9.9f)
                    {
                        File.ftime[i] = 0.0f; //TODO: why are we destroying the file data?
                        data[i] = 0.0f;
                    }
                    time[index] = lrintf(tmpstretch * (idelay + File.ftime[i]) * nfSAMPLE_RATE); // Add initial delay to all the samples
                    data[index] = normal * File.tdata[i];
                    index++;
                }
            }
        };
        Llength = index;
    } //    endif
    else
    {
        int i;
        for (i = 0; i < File.data_length; i++)
        {
            if ((idelay + File.ftime[i]) > 5.9f)
            {
                File.ftime[i] = 5.9f;
            }
            
            time[i] = lrintf(tmpstretch * (idelay + File.ftime[i]) * nfSAMPLE_RATE); // Add initial delay to all the samples
            data[i] = normal * File.tdata[i];
        }
        
        Llength = i;
    }

    //  generate an approximated randomized hrtf for diffusing reflections:
    int hrtf_tmp = Pdiff;
    
    if (hrtf_tmp > File.data_length)
    {
        hrtf_tmp = File.data_length - 1;
    }
    
    if (hlength > File.data_length)
    {
        hlength = File.data_length - 1;
    }
    
    for (int i = 0; i < hrtf_tmp; i++)
    {
        int tmptime = (int) (RND * hrtf_size);
        rndtime[i] = tmptime; //    randomly jumble the head of the transfer function
        rnddata[i] = 3.0f * (0.5f - RND) * data[tmptime];
    }

    if (Pfade > 0)
    {
        int count = lrintf(ffade * ((float) index));
        float tmp = 0.0f;
        
        for (int i = 0; i < count; i++)
        { //    head fader

            tmp = ((float) i) / ((float) count);
            data[i] *= tmp;
            //  fade the head here
        }
    }

    //  guess at room size
    roomsize = time[0] + (time[1] - time[0]) / 2; //    to help stagger left/right reflection times
    
    if (roomsize > imax)
    {
        roomsize = imax;
    }
    
    setfb(Pfb);

    cleanup();
}

void
Reverbtron::setlpf(int value)
{
    Plpf = value;
    float fr = (float) Plpf;
    lpfl->setfreq(fr);
    lpfr->setfreq(fr);
}

void
Reverbtron::sethidamp(int _Phidamp)
{
    Phidamp = _Phidamp;
    hidamp = 1.0f - (float) _Phidamp / 127.1f;
    alpha_hidamp = 1.0f - hidamp;
}

void
Reverbtron::setfb(int value)
{
    if (Pfb <= 0)
    {
        fb = (float) value / 64.0f * 0.3f;
    }
    else
    {
        fb = (float) value / 64.0f * 0.15f;
    }

    fb *= ((1627.0f - (float) Pdiff - (float) Llength) / 1627.0f);
    fb *= (1.0f - ((float) Plevel / 127.0f));
    fb *= (1.0f - diffusion)*.5f;
}

void
Reverbtron::adjust(int DS, double fSAMPLE_RATE)
{
    DS_state = DS;

    switch (DS)
    {
    case 0:
        nRATIO = 1;
        nSAMPLE_RATE = fSAMPLE_RATE;
        nfSAMPLE_RATE = fSAMPLE_RATE;
        break;

    case 1:
        nRATIO = 96000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 96000;
        nfSAMPLE_RATE = 96000.0f;
        break;


    case 2:
        nRATIO = 48000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 48000;
        nfSAMPLE_RATE = 48000.0f;
        break;

    case 3:
        nRATIO = 44100.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 44100;
        nfSAMPLE_RATE = 44100.0f;
        break;

    case 4:
        nRATIO = 32000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 32000;
        nfSAMPLE_RATE = 32000.0f;
        break;

    case 5:
        nRATIO = 22050.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 22050;
        nfSAMPLE_RATE = 22050.0f;
        break;

    case 6:
        nRATIO = 16000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 16000;
        nfSAMPLE_RATE = 16000.0f;
        break;

    case 7:
        nRATIO = 12000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 12000;
        nfSAMPLE_RATE = 12000.0f;
        break;

    case 8:
        nRATIO = 8000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 8000;
        nfSAMPLE_RATE = 8000.0f;
        break;

    case 9:
        nRATIO = 4000.0 / fSAMPLE_RATE;
        nSAMPLE_RATE = 4000;
        nfSAMPLE_RATE = 4000.0f;
        break;
    }

    nPERIOD = lrintf((float) PERIOD * nRATIO);
    u_up = (double) nPERIOD / (double) PERIOD;
    u_down = (double) PERIOD / (double) nPERIOD;
}

void
Reverbtron::setpreset(int npreset)
{
    const int PRESET_SIZE = C_REVTRON_PARAMETERS;
    const int NUM_PRESETS = 9;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Chamber
        {64, 0, 1, 500, 0, 0, 99, 70, 0, 0, 0, 64, 0, 0, 20000, 0},
        //Concrete Stair
        {64, 0, 1, 500, 0, 0, 0, 40, 1, 0, 0, 64, 0, 0, 20000, 0},
        //Hall
        {64, 0, 1, 500, 0, 0, 60, 15, 2, 0, 0, 64, 0, 0, 20000, 0},
        //Medium Hall
        {64, 16, 1, 500, 0, 0, 0, 22, 3, -17, 0, 64, 0, 0, 20000, 0},
        //Room
        {64, 0, 1, 1500, 0, 0, 48, 20, 4, 0, 0, 64, 0, 0, 20000, 0},
        //Large Hall
        {88, 0, 1, 1500, 0, 0, 88, 14, 5, 0, 0, 64, 0, 0, 20000, 0},
        //Guitar
        {64, 0, 1, 1500, 0, 0, 30, 34, 6, 0, 0, 64, 0, 0, 20000, 0},
        //Studio
        {64, 0, 1, 1500, 0, 0, 30, 20, 7, 0, 0, 64, 0, 0, 20000, 0},
        //Cathedral
        {64, 0, 1, 1500, 0, 30, 0, 40, 9, 0, 0, 64, 0, 0, 20000, 0}

    };

    if (npreset > NUM_PRESETS - 1)
    {
        Fpre->ReadPreset(EFX_REVERBTRON, npreset - NUM_PRESETS + 1, pdata, Filename);
        
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
Reverbtron::changepar(int npar, int value)
{
    switch (npar)
    {
    case Revtron_DryWet:
        setvolume(value);
        break;
    case Revtron_Fade:
        Pfade = value;
        ffade = ((float) value) / 127.0f;
        convert_time();
        break;
    case Revtron_Safe:
        Psafe = value;
        break;
    case Revtron_Length:
        Llength = Plength = value;
        if ((Psafe) && (Llength > 400)) Llength = 400;
        convert_time();
        break;
    case Revtron_User_File:
        Puser = value;
        break;
    case Revtron_I_Delay:
        Pidelay = value;
        idelay = ((float) value) / 1000.0f;
        convert_time();
        break;
    case Revtron_Damp:
        sethidamp(value);
        break;
    case Revtron_Level:
        Plevel = value;
        level = 2.0f * dB2rap(60.0f * (float) Plevel / 127.0f - 40.0f);
        levpanl = level*lpanning;
        levpanr = level*rpanning;
        break;
    case Revtron_Set_File:
#ifdef LV2_SUPPORT
        setfile(value); // This will only be called from changepar() upon initialization for lv2 and is ignored.
#else
        if (!setfile(value))
        {
            global_error_number = error;
            error = 0;
        }
#endif 
        break;
    case Revtron_Stretch:
        Pstretch = value;
        fstretch = ((float) value) / 64.0f;
        convert_time();
        break;
    case Revtron_Feedback:
        Pfb = value;
        setfb(value);
        break;
    case Revtron_Pan:
        setpanning(value);
        break;
    case Revtron_Ex_Stereo:
        Pes = value;
        break;
    case Revtron_Shuffle:
        Prv = value;
        break;
    case Revtron_LPF:
        setlpf(value);
        break;
    case Revtron_Diffusion:
        Pdiff = value;
        diffusion = ((float) value) / 127.0f;
        convert_time();
        break;
    default:
        return;
    }
}

int
Reverbtron::getpar(int npar)
{
    switch (npar)
    {
    case Revtron_DryWet:
        return (Pvolume);

    case Revtron_Fade:
        return (Pfade);

    case Revtron_Safe:
        return (Psafe);

    case Revtron_Length:
        return (Plength);

    case Revtron_User_File:
        return (Puser);

    case Revtron_I_Delay:
        return (Pidelay);

    case Revtron_Damp:
        return (Phidamp);

    case Revtron_Level:
        return (Plevel);

    case Revtron_Set_File:
        return (Filenum);

    case Revtron_Stretch:
        return (Pstretch);

    case Revtron_Feedback:
        return (Pfb);

    case Revtron_Pan:
        return (Ppanning);

    case Revtron_Ex_Stereo:
        return (Pes);

    case Revtron_Shuffle:
        return (Prv);

    case Revtron_LPF:
        return (Plpf);

    case Revtron_Diffusion:
        return (Pdiff);

    }
    return (0); //in case of bogus parameter number
}
