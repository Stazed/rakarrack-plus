/*
  rakarrack - a guitar effects software

 varios.C  -  some extra functions
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

#include <FL/Fl_Pixmap.H>
#include <FL/Fl_Group.H>
#include <FL/fl_ask.H>
#include <FL/Fl_Widget.H>
#include "icono_rakarrack_32x32.xpm"
#include "global.h"
#include "process.h"

int
RKR::Message(int prio, const char *labelwin, const char *message_text)
{
    if ((mess_dis) && (prio == 0)) return (0);

    Fl_Widget *w = fl_message_icon();

    Fl_Image *a = new Fl_Pixmap(icono_rakarrack_32x32_xpm);
    w->color(FL_WHITE);
    w->label("");
    w->image(a);
    w->align(FL_ALIGN_TOP | FL_ALIGN_INSIDE);
    w->parent()->copy_label(labelwin);
    fl_message("%s", message_text);
    return (0);

};

void
RKR::Error_Handle(int num)
{
    char meslabel[70];
    char error_msg[256];
    memset(meslabel, 0, sizeof (meslabel));
    sprintf(meslabel, "%s %s", jackcliname, VERSION);

    error_num = 0;
    memset(error_msg, 0, sizeof (error_msg));

    switch (num)
    {
    case 1:
        sprintf(error_msg, "%s", "Convolotron is unable to open the audio .wav file");
        break;
    case 2:
        sprintf(error_msg, "%s", "Reverbtron is unable to open the IR .rvb file");
        break;
    case 3:
        sprintf(error_msg, "%s", "Error writing the file. Do you have permission to write to this directory?");
        break;
    case 4:
        sprintf(error_msg, "%s", "Echotron is unable to open the .dly file");
        break;
    case 5:
        sprintf(error_msg, "%s", "Some Pan parameter is bad in the .dly file");
        break;
    case 6:
        sprintf(error_msg, "%s", "Some Time parameter is bad in the .dly file");
        break;
    case 7:
        sprintf(error_msg, "%s", "Some Level parameter is bad in the .dly file");
        break;
    case 8:
        sprintf(error_msg, "%s", "Some LP parameter is bad in the .dly file");
        break;
    case 9:
        sprintf(error_msg, "%s", "Some BP parameter is bad in the .dly file");
        break;
    case 10:
        sprintf(error_msg, "%s", "Some HP parameter is bad in the .dly file");
        break;
    case 11:
        sprintf(error_msg, "%s", "Some Freq parameter is bad in the .dly file");
        break;
    case 12:
        sprintf(error_msg, "%s", "Some Q parameter bad in the .dly file");
        break;
    case 13:
        sprintf(error_msg, "%s", "Some Stages parameter bad in the .dly file");
        break;
    }

    Message(1, meslabel, error_msg);

}

char *
RKR::PrefNom(const char *dato)
{
    memset(tmpprefname, 0, sizeof (tmpprefname));
    sprintf(tmpprefname, "%s %s", jackcliname, dato);
    return (tmpprefname);
}

int
RKR::Get_Bogomips()
{
    char temp[256];
    char *tmp = NULL;
    char *tmp2 = NULL;
    FILE *fp;

    if ((fp = fopen("/proc/cpuinfo", "r")) != NULL)
    {
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "bogomips") != NULL)
            {
                tmp = strdup(temp);
                tmp2 = tmp; // needed since strsep alters original == memory leak
                strsep(&tmp2, ":");
                sscanf(tmp2, "%f", &bogomips);
                break;
            }
        }

        int maxx_len = lrintf(150.0f / 4800.0f * bogomips);
        
        if (upsample)
        {
            maxx_len /= (UpAmo + 8);
            maxx_len /= (6 - ((UpQual + DownQual) / 2));
            // printf("Max Len: %d\n",maxx_len);

        }
        
        if (maxx_len < 5)
        {
            if (maxx_len < 2) maxx_len = 2;
            
            Message(0, "!! Rakarrack-plus CPU Usage Warning !!", "It appears your CPU will not easily handle convolution with the current settings.  Be careful with the Convolotron effect settings.\nPlease read Help (F1) for more information.");
        }

        fclose(fp);
        free(tmp);
        return (1);
    }

    return (0);
}

int
RKR::TapTempo()
{
    double latency = 0.04;
    gettimeofday(&timeA, NULL);

    double Aseconds = ((double) timeA.tv_sec + (double) timeA.tv_usec * 0.000001);
    
    if (Tap_Selection == 0)
    {
        Aseconds += latency;
    }
    
    double timediff = Aseconds - Tap_timeB;

    if (timediff < 3.0f)
    {
        if (timediff > 0.01f)
        {
            tempobuf[tempocnt] = 60.0f / ((double) timediff);
        }
        
        if ((++tempocnt) >= 5)
        {
            tempocnt = 0;
        }
    }

    double AvTempo = 0.0;
    
    for (int i = 0; i < 4; i++)
    {
        AvTempo += tempobuf[i];
    }

    AvTempo *= 0.25;
    Tap_TempoSet = lrint(AvTempo);
    Tap_timeB = Aseconds;
    Tap_timeC = Aseconds;
    
    if (Tap_Selection)
    {
        Tap_Display = 1;
    }
    
    Update_tempo();
    
    return (Tap_TempoSet);

}

void
RKR::TapTempo_Timeout(int state)
{
    gettimeofday(&timeA, NULL);
    double Aseconds = (double) timeA.tv_sec + (double) timeA.tv_usec * 0.000001;
    
    if (!state)
    {
        Tap_timeC = Aseconds;
        return;
    }
    
    double timediff = Aseconds - Tap_timeC;
    
    if (timediff > 8.0f)
    {
        Tap_Display = 2;
    }
}

void
RKR::Update_tempo()
{
    if ((Tap_TempoSet < 1) || (Tap_TempoSet > 600))
        return;

    int Tap_TempoSetD = Tap_TempoSet;
    int Tap_TempoSetL = Tap_TempoSet;

    switch (Tap_SetValue)
    {
    case 0:
        Tap_TempoSetD = Tap_TempoSetL = Tap_TempoSet;
        break;
    case 1:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 2:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 3:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 4:
        Tap_TempoSetD = Tap_TempoSet;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 5:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 6:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 7:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 8:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 9:
        Tap_TempoSetD = Tap_TempoSet / 2;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 10:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 11:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 12:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 13:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 14:
        Tap_TempoSetD = Tap_TempoSet / 3;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 15:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 16:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 17:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 18:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 19:
        Tap_TempoSetD = Tap_TempoSet / 4;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    case 20:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet;
        break;
    case 21:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 2;
        break;
    case 22:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 3;
        break;
    case 23:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 4;
        break;
    case 24:
        Tap_TempoSetD = Tap_TempoSet / 8;
        Tap_TempoSetL = Tap_TempoSet / 8;
        break;
    }

    if ((Tap_TempoSetL < 1) || (Tap_TempoSetL > 600))
    {
        Tap_TempoSetL = Tap_TempoSet;
    }
    
    if ((Tap_TempoSetD < 1) || (Tap_TempoSetD > 600))
    {
        Tap_TempoSetD = Tap_TempoSet;
    }

    if (Looper_Bypass) efx_Looper->settempo(Tap_TempoSet);

    if (Chorus_Bypass) efx_Chorus->changepar(Chorus_LFO_Tempo, Tap_TempoSetL);
    if (Flanger_Bypass) efx_Flanger->changepar(Chorus_LFO_Tempo, Tap_TempoSetL);
    if (Phaser_Bypass) efx_Phaser->changepar(Phaser_LFO_Tempo, Tap_TempoSetL);
    if (Pan_Bypass) efx_Pan->changepar(Pan_LFO_Tempo, Tap_TempoSetL);
    if (WhaWha_Bypass) efx_WhaWha->changepar(2, Tap_TempoSetL);
    if (Alienwah_Bypass) efx_Alienwah->changepar(Alien_LFO_Tempo, Tap_TempoSetL);
    if (MusDelay_Bypass) efx_MusDelay->changepar(Music_Tempo, Tap_TempoSetD);
    if (APhaser_Bypass) efx_APhaser->changepar(APhase_LFO_Tempo, Tap_TempoSetL);
    if (DFlange_Bypass) efx_DFlange->changepar(DFlange_LFO_Tempo, Tap_TempoSetL);
    if (Synthfilter_Bypass) efx_Synthfilter->changepar(Synthfilter_LFO_Tempo, Tap_TempoSetL);
    if (MuTroMojo_Bypass) efx_MuTroMojo->changepar(MuTro_LFO_Tempo, Tap_TempoSetL);
    if (VaryBand_Bypass) efx_VaryBand->changepar(VaryBand_LFO_Tempo_1, Tap_TempoSetL);
    if (VaryBand_Bypass) efx_VaryBand->changepar(VaryBand_LFO_Tempo_2, Tap_TempoSetL);
    if (Arpie_Bypass) efx_Arpie->changepar(Arpie_Tempo, Tap_TempoSetD);
    if (Echoverse_Bypass) efx_Echoverse->changepar(Echoverse_Tempo, Tap_TempoSetD);
    if (Sequence_Bypass) efx_Sequence->changepar(Sequence_Tempo, Tap_TempoSetD);
    if (Echotron_Bypass) efx_Echotron->changepar(Echotron_Tempo, Tap_TempoSetD);
    if (Opticaltrem_Bypass) efx_Opticaltrem->changepar(Optical_LFO_Tempo, Tap_TempoSetL);
    if (Vibe_Bypass) efx_Vibe->changepar(Vibe_LFO_Tempo, Tap_TempoSetL);
    if (Infinity_Bypass) efx_Infinity->changepar(Infinity_Tempo, Tap_TempoSetL);

    if (Echo_Bypass)
    {
        efx_Echo->Tempo2Delay(Tap_TempoSetD);
    }

}

