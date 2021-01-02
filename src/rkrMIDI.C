/*
  rakarrack - a guitar effects software

 rkrMIDI.C  -  MIDI functions
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


#include "process.h"
#include <FL/fl_ask.H>  // for error pop up

/* MIDI control defines (Max - Min) / 127 - parameter ranges  */
const float C_MC_7_RANGE        = 0.05511811f;      /* 7 / 127 = 0.055118110236 */
const float C_MC_8_RANGE        = 0.06299f;         /* 8 / 127 = 0.062992125984252 */
const float C_MC_11_RANGE       = 0.0866142f;       /* 11 / 127 = 0.0866141732283465*/
const float C_MC_12_RANGE       = 0.094488189f;     /* 12 / 127 = 0.094488188976 */
const float C_MC_23_RANGE       = 0.18110236f;      /* 23 / 127 = 0.1811023622047 */
const float C_MC_24_RANGE       = 0.18897638f;      /* 24 / 127 = 0.188976377953  */
const float C_MC_30_RANGE       = 0.236220472441f;  /* 30 / 127 = 0.2362204724409449 */
const float C_MC_32_RANGE       = 0.25196850393701f;  /* 32 / 127 = 0.2519685039370079 */
const float C_MC_33_RANGE       = 0.25984252f;      /* 33 / 127 = 0.2598425196850 */
const float C_MC_40_RANGE       = 0.31496063f;      /* (42 - 2) / 127 = 0.314960629921 */
const float C_MC_49_RANGE       = 0.385827f;        /* (50 - 1) / 127 = 0.385826771654 */
const float C_MC_55_RANGE       = 0.43307087f;      /* (65 - 10) / 127 = 0.433070866142 */
const float C_MC_57_RANGE       = 0.448818898f;     /* (-60 - -3) / 127 = 0.448818897638 */
const float C_MC_63_RANGE       = 0.49606299f;      /* (127 - 64) / 127 = 0.4960629921259843 */
const float C_MC_80_RANGE       = 0.62992126f;      /* 80 / 127 = 0.629921259843 */
const float C_MC_90_RANGE       = 0.708661417f;     /* 90 / 127 = 0.7086614173228 */
const float C_MC_94_RANGE       = 0.74015748f;      /* (-70 + 24) / 127 = 0.740157480315 */
const float C_MC_100_RANGE      = 0.7874016f;       /* 100 / 127 = 0.787401574803 */
const float C_MC_126_RANGE      = 0.99212598f;      /* (127 - 1) / 127 = 0.992125984252 */
const float C_MC_128_RANGE      = 1.007874015748f;  /* 128 / 127 = 1.007874015748 */
const float C_MC_130_RANGE      = 1.023622f;        /* (170 - 40) / 127 = 1.0236220472441 */
const float C_MC_240_RANGE      = 1.890f;           /* (250 - 10) / 127 = 1.889763779528 */
const float C_MC_245_RANGE      = 1.9291339f;       /* (250 - 5) / 127 = 1.929133858268 */
const float C_MC_248_RANGE      = 1.9527559055f;    /* (250 - 2) / 127 = 1.9527559055118 */
const float C_MC_249_RANGE      = 1.9606299212598f; /* (250 - 1) / 127 = 1.9606299212598 */
const float C_MC_360_RANGE      = 2.83464567f;      /* (380 - 20) / 127 = 2.8346456692913 */
const float C_MC_470_RANGE      = 3.7007874f;       /* (480 - 10) / 127 = 3.7007874015748 */
const float C_MC_490_RANGE      = 3.85826772f;      /* (500 - 10) / 127 = 3.858267716535 */
const float C_MC_495_RANGE      = 3.8976378f;       /* (500 - 5) / 127 = 3.8976377952756 */
const float C_MC_498_RANGE      = 3.92125984f;      /* (500 - 2) / 127 = 3.921259842519685 */
const float C_MC_500_RANGE      = 3.9370079f;       /* 500 / 127 = 3.937007874 */
const float C_MC_600_RANGE      = 4.724f;           /* 600  / 127 = 4.724409448819 */
const float C_MC_770_RANGE      = 6.062992126f;     /* (800 - 30) / 127 = 6.062992125984 */
const float C_MC_980_RANGE      = 7.716535433f;     /* (1000 - 20) / 127 = 7.716535433071 */
const float C_MC_990_RANGE      = 7.7952756f;       /* (1000 - 10) / 127 = 7.795275590551 */
const float C_MC_995_RANGE      = 7.8346457f;       /* (1000 - 5) / 127 = 7.8346456692913 */
const float C_MC_1480_RANGE     = 11.653543f;       /* (1500 - 20) / 127 = 11.6535433071 */
const float C_MC_1900_RANGE     = 14.96063f;        /* (4500 - 2600) / 127 = 14.96062992126 */
const float C_MC_1980_RANGE     = 15.59055118f;     /* (2000 - 20) / 127 = 15.5905511811 */
const float C_MC_1999_RANGE     = 15.748031f;       /* (2000 - 1) / 127 = 15.740157480315 */
const float C_MC_2000_RANGE     = 15.7480315f;      /* 2000 / 127 = 15.748031496063 */
const float C_MC_2480_RANGE     = 19.52756f;        /* (2500 - 20) / 127 =  19.527559055118 */
const float C_MC_3600_RANGE     = 28.34645669f;     /* (4000 - 400) / 127 = 28.3464566929134 */
const float C_MC_4000_RANGE     = 31.496063f;       /* 4000 / 127 = 31.496062992126 */
const float C_MC_4380_RANGE     = 34.488189f;       /* (4400 - 20) / 127 = 34.488188976378 */
const float C_MC_4999_RANGE     = 39.362205f;       /* (5000 - 1) / 127 = 39.362204724409 */
const float C_MC_5990_RANGE     = 47.165354f;       /* (6000 - 10) / 127 = 47.1653543307087 */
const float C_MC_6000_RANGE     = 47.2441f;         /* 6000 / 127 = 47.244094488189 */
const float C_MC_6800_RANGE     = 53.54330709f;     /* (8000 - 1200) / 127 = 53.5433070866142 */
const float C_MC_7000_RANGE     = 55.11811f;        /* (8000 - 1000) / 127 = 55.11811023622 */
const float C_MC_7200_RANGE     = 56.6929134f;      /* (8000 - 800) / 127 = 56.6929133858 */
const float C_MC_11200_RANGE    = 88.18898f;        /* (12000 - 800) / 127 = 88.188976377953 */
const float C_MC_15780_RANGE    = 124.25197f;       /* (16000 - 220) / 127 = 124.251968503937 */
const float C_MC_19980_RANGE    = 157.322835f;      /* (20000 - 20) / 127 = 157.322834645669 */
const float C_MC_19999_RANGE    = 157.472441f;      /* (20000 - 1) / 127 = 157.4724409448819 */
const float C_MC_20000_RANGE    = 157.480315f;      /* (26000 - 6000) / 127 = 157.480314961 */
const float C_MC_24000_RANGE    = 188.97638f;       /* (26000 - 2000) / 127 = 188.976377952756 */
const float C_MC_25980_RANGE    = 204.566929f;      /* (26000 - 20) / 127 = 204.5669291338583 */

void
RKR::InitMIDI()
{
    // Open Alsa Seq
    int err = snd_seq_open(&midi_in, "default", SND_SEQ_OPEN_INPUT, 0);
    if (err < 0)
        printf("Cannot activate ALSA seq client\n");
    snd_seq_set_client_name(midi_in, jackcliname);
    snd_config_update_free_global();

    char portname[70];

    // Create Alsa Seq Client
    sprintf(portname, "%s IN", jackcliname);
    snd_seq_create_simple_port(midi_in, portname,
                               SND_SEQ_PORT_CAP_WRITE |
                               SND_SEQ_PORT_CAP_SUBS_WRITE,
                               SND_SEQ_PORT_TYPE_SYNTH);

}

void
RKR::miramidi()
{
    if (snd_seq_event_input_pending(midi_in, 1))
    {
        do
        {
            midievents();
        }
        while (snd_seq_event_input_pending(midi_in, 0));
    }
}

void
RKR::midievents()
{
    int i;
    snd_seq_event_t *midievent;
    midievent = NULL;
    snd_seq_event_input(midi_in, &midievent);
    
    if (midievent == NULL)
        return;
    
    if (midievent->type == 42)
        return;

    if ((Tap_Bypass) && (Tap_Selection == 3) && (midievent->type == SND_SEQ_EVENT_CLOCK))
    {
        mtc_counter++;
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    if ((Looper_Bypass) && (Tap_Selection == 3))
    {
        if (midievent->type == SND_SEQ_EVENT_START)
        {
            efx_Looper->changepar(Looper_Play, 1);
            stecla = 5;
        }

        if (midievent->type == SND_SEQ_EVENT_STOP)
        {
            efx_Looper->changepar(Looper_Stop, 1);
            stecla = 5;
        }
    }

    if ((midievent->type == SND_SEQ_EVENT_NOTEON)
        || (midievent->type == SND_SEQ_EVENT_NOTEOFF))
    {
        int cmdnote = midievent->data.note.note;
        int cmdvelo = midievent->data.note.velocity;

        if ((Tap_Bypass) && (Tap_Selection == 1) && (midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
            Tap_TempoSet = TapTempo();

        if (midievent->data.note.channel == HarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
                {
                    if (RC_Harm->note_active[i] == 0)
                    {
                        RC_Harm->note_active[i] = 1;
                        RC_Harm->rnote[i] = cmdnote;
                        RC_Harm->gate[i] = 1;
                        RC_Harm->MiraChord();
                        break;
                    }
                }

                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }

        if (midievent->data.note.channel == StereoHarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
                {
                    if (RC_Stereo_Harm->note_active[i] == 0)
                    {
                        RC_Stereo_Harm->note_active[i] = 1;
                        RC_Stereo_Harm->rnote[i] = cmdnote;
                        RC_Stereo_Harm->gate[i] = 1;
                        RC_Stereo_Harm->MiraChord();
                        break;
                    }
                }

                if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }
    }


    if (midievent->type == SND_SEQ_EVENT_PGMCHANGE)
    {
        if (midievent->data.control.channel == MidiCh)
        {
            if (!midi_table)
            {
                if ((midievent->data.control.value > 0)
                    && (midievent->data.control.value < 61))
                    preset = midievent->data.control.value;

                if (midievent->data.control.value == 81)
                    if (Selected_Preset > 1) preset = Selected_Preset - 1;

                if (midievent->data.control.value == 82)
                    if (Selected_Preset < 60) preset = Selected_Preset + 1;
            }
            else
                preset = midievent->data.control.value;
        }
    }

    if (midievent->type == SND_SEQ_EVENT_CONTROLLER)
    {
        if (midievent->data.control.channel == MidiCh)
        {
            if (RControl)
            {
                ControlGet = (int) midievent->data.control.param;
                return;
            }

            if (MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (XUserMIDI[(int) midievent->data.control.param][i])
                        process_midi_controller_events(XUserMIDI[(int) midievent->data.control.param][i],
                                                       (int) midievent->data.control.value);
                    else break;
                }
            }
            else
                process_midi_controller_events((int) midievent->data.control.param,
                                               (int) midievent->data.control.value);
        }
    }
}

void
RKR::ActOnOff()
{
    if (OnOffC < 63) OnOffC++;
}

void
RKR::ActiveUn(int value)
{
    int numef;
    int inoff = 0;
    int miraque = 0;

    if (value < 20)
    {
        numef = value / 2;
        inoff = checkonoff(efx_order[numef]); // value % 2;
        miraque = efx_order[numef];
        ActOnOff();
        Mnumeff[OnOffC] = numef;
    }
    else if (value < 121)
    {
        numef = value - 20;
        inoff = checkonoff(numef);
        miraque = numef;
        ActOnOff();
        Mnumeff[OnOffC] = 1000 + numef;
    }
    else
    {
        numef = value;
        inoff = checkonoff(numef);
        miraque = numef;
        ActOnOff();
        Mnumeff[OnOffC] = 2000 + numef;
    }

    switch (miraque)
    {
    case 0:
        if (inoff) EQ1_Bypass = 1;
        else EQ1_Bypass = 0;
        break;
    case 1:
        if (inoff) Compressor_Bypass = 1;
        else Compressor_Bypass = 0;
        break;
    case 2:
        if (inoff) Distorsion_Bypass = 1;
        else Distorsion_Bypass = 0;
        break;
    case 3:
        if (inoff) Overdrive_Bypass = 1;
        else Overdrive_Bypass = 0;
        break;
    case 4:
        if (inoff) Echo_Bypass = 1;
        else Echo_Bypass = 0;
        break;
    case 5:
        if (inoff) Chorus_Bypass = 1;
        else Chorus_Bypass = 0;
        break;
    case 6:
        if (inoff) Phaser_Bypass = 1;
        else Phaser_Bypass = 0;
        break;
    case 7:
        if (inoff) Flanger_Bypass = 1;
        else Flanger_Bypass = 0;
        break;
    case 8:
        if (inoff) Reverb_Bypass = 1;
        else Reverb_Bypass = 0;
        break;
    case 9:
        if (inoff) EQ2_Bypass = 1;
        else EQ2_Bypass = 0;
        break;
    case 10:
        if (inoff) WhaWha_Bypass = 1;
        else WhaWha_Bypass = 0;
        break;
    case 11:
        if (inoff) Alienwah_Bypass = 1;
        else Alienwah_Bypass = 0;
        break;
    case 12:
        if (inoff) Cabinet_Bypass = 1;
        else Cabinet_Bypass = 0;
        break;
    case 13:
        if (inoff) Pan_Bypass = 1;
        else Pan_Bypass = 0;
        break;
    case 14:
        if (inoff) Harmonizer_Bypass = 1;
        else Harmonizer_Bypass = 0;
        break;
    case 15:
        if (inoff) MusDelay_Bypass = 1;
        else MusDelay_Bypass = 0;
        break;
    case 16:
        if (inoff) Gate_Bypass = 1;
        else Gate_Bypass = 0;
        break;
    case 17:
        if (inoff) Derelict_Bypass = 1;
        else Derelict_Bypass = 0;
        break;
    case 18:
        if (inoff) APhaser_Bypass = 1;
        else APhaser_Bypass = 0;
        break;
    case 19:
        if (inoff) Valve_Bypass = 1;
        else Valve_Bypass = 0;
        break;
    case 20:
        if (inoff) DFlange_Bypass = 1;
        else DFlange_Bypass = 0;
        break;
    case 21:
        if (inoff) Ring_Bypass = 1;
        else Ring_Bypass = 0;
        break;
    case 22:
        if (inoff) Exciter_Bypass = 1;
        else Exciter_Bypass = 0;
        break;
    case 23:
        if (inoff) DistBand_Bypass = 1;
        else DistBand_Bypass = 0;
        break;
    case 24:
        if (inoff) Arpie_Bypass = 1;
        else Arpie_Bypass = 0;
        break;
    case 25:
        if (inoff) Expander_Bypass = 1;
        else Expander_Bypass = 0;
        break;
    case 26:
        if (inoff) Shuffle_Bypass = 1;
        else Shuffle_Bypass = 0;
        break;
    case 27:
        if (inoff) Synthfilter_Bypass = 1;
        else Synthfilter_Bypass = 0;
        break;
    case 28:
        if (inoff) VaryBand_Bypass = 1;
        else VaryBand_Bypass = 0;
        break;
    case 29:
        if (inoff) Convol_Bypass = 1;
        else Convol_Bypass = 0;
        break;
    case 30:
        if (inoff) Looper_Bypass = 1;
        else Looper_Bypass = 0;
        break;
    case 31:
        if (inoff) MuTroMojo_Bypass = 1;
        else MuTroMojo_Bypass = 0;
        break;
    case 32:
        if (inoff) Echoverse_Bypass = 1;
        else Echoverse_Bypass = 0;
        break;
    case 33:
        if (inoff) CoilCrafter_Bypass = 1;
        else CoilCrafter_Bypass = 0;
        break;
    case 34:
        if (inoff) ShelfBoost_Bypass = 1;
        else ShelfBoost_Bypass = 0;
        break;
    case 35:
        if (inoff) Vocoder_Bypass = 1;
        else Vocoder_Bypass = 0;
        break;
    case 36:
        if (inoff) Sustainer_Bypass = 1;
        else Sustainer_Bypass = 0;
        break;
    case 37:
        if (inoff) Sequence_Bypass = 1;
        else Sequence_Bypass = 0;
        break;
    case 38:
        if (inoff) Shifter_Bypass = 1;
        else Shifter_Bypass = 0;
        break;
    case 39:
        if (inoff) StompBox_Bypass = 1;
        else StompBox_Bypass = 0;
        break;
    case 40:
        if (inoff) Reverbtron_Bypass = 1;
        else Reverbtron_Bypass = 0;
        break;
    case 41:
        if (inoff) Echotron_Bypass = 1;
        else Echotron_Bypass = 0;
        break;
    case 42:
        if (inoff) StereoHarm_Bypass = 1;
        else StereoHarm_Bypass = 0;
        break;
    case 43:
        if (inoff) CompBand_Bypass = 1;
        else CompBand_Bypass = 0;
        break;
    case 44:
        if (inoff) Opticaltrem_Bypass = 1;
        else Opticaltrem_Bypass = 0;
        break;
    case 45:
        if (inoff) Vibe_Bypass = 1;
        else Vibe_Bypass = 0;
        break;
    case 46:
        if (inoff) Infinity_Bypass = 1;
        else Infinity_Bypass = 0;
        break;


    case 121:
        if (inoff) Tap_Bypass = 1;
        else Tap_Bypass = 0;
        break;
    case 122:
        if (inoff) MIDIConverter_Bypass = 1;
        else MIDIConverter_Bypass = 0;
        break;
    case 123:
        if (inoff) Tuner_Bypass = 1;
        else Tuner_Bypass = 0;
        break;
    case 124:
        if (inoff) Bypass = 1;
        else Bypass = 0;
        break;
    }
}

int
RKR::checkonoff(int miraque)
{
    switch (miraque)
    {
    case 0:
        if (EQ1_Bypass) return (0);
        break;
    case 1:
        if (Compressor_Bypass) return (0);
        break;
    case 2:
        if (Distorsion_Bypass) return (0);
        break;
    case 3:
        if (Overdrive_Bypass)return (0);
        break;
    case 4:
        if (Echo_Bypass)return (0);
        break;
    case 5:
        if (Chorus_Bypass) return (0);
        break;
    case 6:
        if (Phaser_Bypass) return (0);
        break;
    case 7:
        if (Flanger_Bypass) return (0);
        break;
    case 8:
        if (Reverb_Bypass) return (0);
        break;
    case 9:
        if (EQ2_Bypass) return (0);
        break;
    case 10:
        if (WhaWha_Bypass) return (0);
        break;
    case 11:
        if (Alienwah_Bypass) return (0);
        break;
    case 12:
        if (Cabinet_Bypass) return (0);
        break;
    case 13:
        if (Pan_Bypass) return (0);
        break;
    case 14:
        if (Harmonizer_Bypass) return (0);
        break;
    case 15:
        if (MusDelay_Bypass) return (0);
        break;
    case 16:
        if (Gate_Bypass) return (0);
        break;
    case 17:
        if (Derelict_Bypass) return (0);
        break;
    case 18:
        if (APhaser_Bypass) return (0);
        break;
    case 19:
        if (Valve_Bypass) return (0);
        break;
    case 20:
        if (DFlange_Bypass) return (0);
        break;
    case 21:
        if (Ring_Bypass) return (0);
        break;
    case 22:
        if (Exciter_Bypass) return (0);
        break;
    case 23:
        if (DistBand_Bypass) return (0);
        break;
    case 24:
        if (Arpie_Bypass) return (0);
        break;
    case 25:
        if (Expander_Bypass) return (0);
        break;
    case 26:
        if (Shuffle_Bypass) return (0);
        break;
    case 27:
        if (Synthfilter_Bypass) return (0);
        break;
    case 28:
        if (VaryBand_Bypass) return (0);
        break;
    case 29:
        if (Convol_Bypass) return (0);
        break;
    case 30:
        if (Looper_Bypass) return (0);
        break;
    case 31:
        if (MuTroMojo_Bypass) return (0);
        break;
    case 32:
        if (Echoverse_Bypass) return (0);
        break;
    case 33:
        if (CoilCrafter_Bypass) return (0);
        break;
    case 34:
        if (ShelfBoost_Bypass) return (0);
        break;
    case 35:
        if (Vocoder_Bypass) return (0);
        break;
    case 36:
        if (Sustainer_Bypass) return (0);
        break;
    case 37:
        if (Sequence_Bypass) return (0);
        break;
    case 38:
        if (Shifter_Bypass) return (0);
        break;
    case 39:
        if (StompBox_Bypass) return (0);
        break;
    case 40:
        if (Reverbtron_Bypass) return (0);
        break;
    case 41:
        if (Echotron_Bypass) return (0);
        break;
    case 42:
        if (StereoHarm_Bypass) return (0);
        break;
    case 43:
        if (CompBand_Bypass) return (0);
        break;
    case 44:
        if (Opticaltrem_Bypass) return (0);
        break;
    case 45:
        if (Vibe_Bypass) return (0);
        break;
    case 46:
        if (Infinity_Bypass) return (0);
        break;

    case 121:
        if (Tap_Bypass) return 0;
        break;
    case 122:
        if (MIDIConverter_Bypass) return 0;
        break;
    case 123:
        if (Tuner_Bypass) return 0;
        break;
    case 124:
        if (Bypass) return 0;
        break;
    }
    return (1);

}

void
RKR::Conecta()
{
    FILE *fp;

    int client = 0;
    int puerto = 0;
    char temp[128];
    char temp1[128];
    char temp2[128];
    char *nume;

    if (IsCoIn)
        disconectaaconnect();

    if ((fp = fopen("/proc/asound/seq/clients", "r")) != NULL)
    {
        memset(temp, 0, sizeof (temp));

        while (fgets(temp, sizeof temp, fp) != NULL)
        {
            if (strstr(temp, "Client") != NULL)
            {
                strcpy(temp1, temp);
                strtok(temp1, " ");
                nume = strtok(NULL, "\"");
                sscanf(nume, "%d", &client);
            }

            if (strstr(temp, "Port") != NULL)
            {
                strcpy(temp2, temp);
                strtok(temp2, " ");
                nume = strtok(NULL, "  ");
                sscanf(nume, "%d", &puerto);
                
                if (strstr(temp, "rakarrack IN") != 0)
                {
                    Cyoin = client;
                    Pyoin = puerto;
                }
                
                if (strstr(temp, MID) != 0)
                {
                    Ccin = client;
                    Pcin = puerto;
                }
            }
        }
    }

    fclose(fp);
    conectaaconnect();
};

void
RKR::conectaaconnect()
{
    char tempi[128];

    if (MID != NULL)
    {
        memset(tempi, 0, sizeof (tempi));
        sprintf(tempi, "aconnect %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        
        if (system(tempi) == -1)
            fl_alert("Error running aconnect!");
        else
            IsCoIn = 1;
    }
}

void
RKR::disconectaaconnect()
{
    char tempi[128];

    if (MID != NULL)
    {
        memset(tempi, 0, sizeof (tempi));
        sprintf(tempi, "aconnect -d %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
        if (system(tempi) == -1)
            fl_alert("Error running aconnect!");
        else
            IsCoIn = 0;
    }
}

void
RKR::jack_process_midievents(jack_midi_event_t *midievent)
{
    int i;
    int type = midievent->buffer[0] >> 4;

    if ((Tap_Bypass) && (Tap_Selection == 3) && (midievent->buffer[0] == 0xf8))
    {
        mtc_counter++;
        
        if (mtc_counter >= 24)
        {
            Tap_TempoSet = TapTempo();
            mtc_counter = 0;
        }
    }

    if ((type == 8) || (type == 9))
    {
        int cmdnote = midievent->buffer[1];
        int cmdvelo = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if ((Tap_Bypass) && (Tap_Selection == 1) && (type == 9) && (cmdvelo != 0)) Tap_TempoSet = TapTempo();

        if (cmdchan == HarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((type == 9) && (cmdvelo != 0))
                {
                    if (RC_Harm->note_active[i] == 0)
                    {
                        RC_Harm->note_active[i] = 1;
                        RC_Harm->rnote[i] = cmdnote;
                        RC_Harm->gate[i] = 1;
                        RC_Harm->MiraChord();
                        break;
                    }
                }

                if ((type == 9) && (cmdvelo == 0))
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (type == 8)
                {
                    if ((RC_Harm->note_active[i]) && (RC_Harm->rnote[i] == cmdnote))
                    {
                        RC_Harm->note_active[i] = 0;
                        RC_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }

        if (cmdchan == StereoHarCh)
        {
            for (i = 0; i < POLY; i++)
            {
                if ((type == 9) && (cmdvelo != 0))
                {
                    if (RC_Stereo_Harm->note_active[i] == 0)
                    {
                        RC_Stereo_Harm->note_active[i] = 1;
                        RC_Stereo_Harm->rnote[i] = cmdnote;
                        RC_Stereo_Harm->gate[i] = 1;
                        RC_Stereo_Harm->MiraChord();
                        break;
                    }
                }

                if ((type == 9) && (cmdvelo == 0))
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }

                if (type == 8)
                {
                    if ((RC_Stereo_Harm->note_active[i]) && (RC_Stereo_Harm->rnote[i] == cmdnote))
                    {
                        RC_Stereo_Harm->note_active[i] = 0;
                        RC_Stereo_Harm->gate[i] = 0;
                        break;
                    }
                }
            }
        }
    }

    if (type == 12)
    {
        int cmdvalue = midievent->buffer[1];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == MidiCh)
        {
            if (!midi_table)
            {
                if ((cmdvalue > 0)
                    && (cmdvalue < 61))
                    preset = cmdvalue;

                if (cmdvalue == 81) if (Selected_Preset > 1) preset = Selected_Preset - 1;
                
                if (cmdvalue == 82) if (Selected_Preset < 60) preset = Selected_Preset + 1;
            }
            else
                preset = cmdvalue;
        }
    }

    if (type == 11)
    {
        int cmdcontrol = midievent->buffer[1];
        int cmdvalue = midievent->buffer[2];
        int cmdchan = midievent->buffer[0]&15;

        if (cmdchan == MidiCh)
        {
            if (RControl)
            {
                ControlGet = cmdcontrol;
                return;
            }

            if (MIDIway)
            {
                for (i = 0; i < 20; i++)
                {
                    if (XUserMIDI[cmdcontrol][i])
                        process_midi_controller_events(XUserMIDI[cmdcontrol][i], cmdvalue);
                    else break;
                }
            }
            else
                process_midi_controller_events(cmdcontrol, cmdvalue);
        }
    }
}

/*
 * process MIDI controller events
 */
void
RKR::process_midi_controller_events(int parameter, int value)
{
    /* Don't process MIDI control when updating quality since
       the efx may be deleted */
    if(quality_update)
        return;
    
    // for real parameter changes, flag need for a GUI update
    if (parameter > 0)
    {
        Mcontrol[parameter] = 1;
        Mvalue = 1;
    }

    switch (parameter)
    {
    case 7:
        Master_Volume =
                (float) value / 128.0f;
        calculavol(2);
        break;

    case 1:
        efx_WhaWha->changepar(6, value);
        break;

    case 20:
        efx_Alienwah->changepar(Alien_Depth, value);
        break;

    case 21:
        efx_Phaser->changepar(Phaser_Depth, value);
        break;

    case 22:
        efx_Flanger->changepar(Chorus_Depth, value);
        break;

    case 23:
        efx_Chorus->changepar(Chorus_Depth, value);
        break;

    case 24:
        efx_MusDelay->changepar(Music_Gain_1, value);
        break;

    case 25:
        efx_MusDelay->changepar(Music_Gain_2, value);
        break;

    case 26:
        efx_Har->changepar(Harm_Filter_Freq, ret_LPF(value));
        break;

    case 27:
        if (Harmonizer_Bypass)
        {
            Harmonizer_Bypass = 0;
            efx_Har->changepar(Harm_Interval, (int) ((float) value * C_MC_24_RANGE));
            Harmonizer_Bypass = 1;
        }
        break;

    case 28:
        efx_WhaWha->changepar(0, Dry_Wet(value));
        break;

    case 29:
        efx_Overdrive->changepar(Dist_DryWet, Dry_Wet(value));
        break;

    case 30:
        efx_Distorsion->changepar(Dist_DryWet, Dry_Wet(value));
        break;

    case 31:
        efx_Har->changepar(Harm_DryWet, Dry_Wet(value));
        break;

    case 52:
        efx_Chorus->changepar(Chorus_DryWet, Dry_Wet(value));
        break;

    case 53:
        efx_Flanger->changepar(Chorus_DryWet, Dry_Wet(value));
        break;

    case 54:
        efx_Phaser->changepar(Phaser_DryWet, Dry_Wet(value));
        break;

    case 55:
        efx_Alienwah->changepar(Alien_DryWet, Dry_Wet(value));
        break;

    case 56:
        efx_MusDelay->changepar(Music_DryWet, Dry_Wet(value));
        break;

    case 57:
        efx_Rev->changepar(Reverb_DryWet, Dry_Wet(value));
        break;

    case 58:
        efx_Pan->changepar(Pan_DryWet, Dry_Wet(value));
        break;

    case 59:
        efx_Echo->changepar(Echo_DryWet, Dry_Wet(value));
        break;

    case 46:
        efx_Echo->changepar(Echo_Pan, value);
        break;

    case 47:
        efx_Overdrive->changepar(Dist_Pan, value);
        break;

    case 48:
        efx_Distorsion->changepar(Dist_Pan, value);
        break;

    case 49:
        efx_Har->changepar(Harm_Pan, value);
        break;

    case 50:
        efx_Chorus->changepar(Chorus_Pan, value);
        break;

    case 51:
        efx_Flanger->changepar(Chorus_Pan, value);
        break;

    case 60:
        efx_Phaser->changepar(Phaser_Pan, value);
        break;

    case 61:
        efx_Alienwah->changepar(Alien_Pan, value);
        break;

    case 62:
        efx_MusDelay->changepar(Music_Pan_1, value);
        break;

    case 63:
        efx_Rev->changepar(Reverb_Pan, value);
        break;

    case 65:
        efx_MusDelay->changepar(Music_Pan_2, value);
        break;

    case 66:
        efx_WhaWha->changepar(1, value);
        break;

    case 67:
        efx_Pan->changepar(Pan_Pan, value);
        break;

    case 68:
        efx_Overdrive->changepar(Dist_Drive, value);
        break;

    case 69:
        efx_Distorsion->changepar(Dist_Drive, value);
        break;

    case 70:
        efx_Overdrive->changepar(Dist_Level, value);
        break;

    case 71:
        efx_Distorsion->changepar(Dist_Level, value);
        break;

    case 72:
        efx_Chorus->changepar(Chorus_LFO_Tempo, ret_Tempo(value));
        break;

    case 73:
        efx_Flanger->changepar(Chorus_LFO_Tempo, ret_Tempo(value));
        break;

    case 74:
        efx_Phaser->changepar(Phaser_LFO_Tempo, ret_Tempo(value));
        break;

    case 75:
        efx_WhaWha->changepar(2, ret_Tempo(value));
        break;

    case 76:
        efx_Alienwah->changepar(Alien_LFO_Tempo, ret_Tempo(value));
        break;

    case 77:
        efx_Pan->changepar(Pan_LFO_Tempo, ret_Tempo(value));
        break;

    case 78:
        efx_Echo->changepar(Echo_Feedback, value);
        break;

    case 79:
        efx_Chorus->changepar(Chorus_Feedback, value);
        break;

    case 80:
        efx_Flanger->changepar(Chorus_Feedback, value);
        break;

    case 81:
        efx_Phaser->changepar(Phaser_Feedback, value);
        break;

    case 82:
        efx_Alienwah->changepar(Alien_Feedback, value);
        break;

    case 83:
        efx_MusDelay->changepar(Music_Feedback_1, value);
        break;

    case 84:
        efx_MusDelay->changepar(Music_Feedback_2, value);
        break;

    case 85:
        efx_Overdrive->changepar(Dist_LPF, ret_LPF(value));
        break;

    case 86:
        efx_Distorsion->changepar(Dist_LPF, ret_LPF(value));
        break;

    case 87:
        efx_Rev->changepar(Reverb_LPF, ret_LPF(value));
        break;

    case 88:
        efx_Overdrive->changepar(Dist_HPF, ret_HPF(value));
        break;

    case 89:
        efx_Distorsion->changepar(Dist_HPF, ret_HPF(value));
        break;

    case 90:
        efx_Rev->changepar(Reverb_HPF, ret_HPF(value));
        break;

    case 91:
        efx_Chorus->changepar(Chorus_LR_Cross, value);
        break;

    case 92:
        efx_Flanger->changepar(Chorus_LR_Cross, value);
        break;

    case 93:
        efx_Phaser->changepar(Phaser_LR_Cross, value);
        break;

    case 94:
        efx_Overdrive->changepar(Dist_LR_Cross, value);
        break;

    case 95:
        efx_Distorsion->changepar(Dist_LR_Cross, value);
        break;

    case 96:
        efx_Alienwah->changepar(Alien_LRCross, value);
        break;

    case 97:
        efx_Echo->changepar(Echo_LR_Cross, value);
        break;

    case 98:
        efx_MusDelay->changepar(Music_LR_Cross, value);
        break;

    case 99:
        efx_Chorus->changepar(Chorus_LFO_Stereo, value);
        break;

    case 100:
        efx_Flanger->changepar(Chorus_LFO_Stereo, value);
        break;

    case 101:
        efx_Phaser->changepar(Phaser_LFO_Stereo, value);
        break;

    case 102:
        efx_WhaWha->changepar(5, value);
        break;

    case 103:
        efx_Alienwah->changepar(Alien_LFO_Stereo, value);
        break;

    case 104:
        efx_Pan->changepar(Pan_LFO_Stereo, value);
        break;

    case 105:
        efx_Chorus->changepar(Chorus_LFO_Random, value);
        break;

    case 106:
        efx_Flanger->changepar(Chorus_LFO_Random, value);
        break;

    case 107:
        efx_Phaser->changepar(Phaser_LFO_Random, value);
        break;

    case 108:
        efx_WhaWha->changepar(3, value);
        break;

    case 109:
        efx_Alienwah->changepar(Alien_LFO_Rand, value);
        break;

    case 110:
        efx_Pan->changepar(Pan_LFO_Random, value);
        break;

    case 111:
        efx_WhaWha->changepar(7, value);
        break;

    case 112:
        efx_WhaWha->changepar(8, value);
        break;

    case 113:
        efx_WhaWha->changepar(9, value);
        break;

    case 114:
        efx_Phaser->changepar(Phaser_Phase, value);
        break;

    case 115:
        efx_Alienwah->changepar(Alien_Phase, value);
        break;

    case 116:
        ActiveUn(value);
        break;

    case 117:
        efx_APhaser->changepar(APhase_DryWet, Dry_Wet(value));
        break;

    case 118:
        efx_APhaser->changepar(APhase_Distortion, value);
        break;

    case 119:
        efx_APhaser->changepar(APhase_LFO_Tempo, ret_Tempo(value));
        break;

    case 120:
        efx_APhaser->changepar(APhase_Depth, value);
        break;

    case 121:
        efx_APhaser->changepar(APhase_Width, value);
        break;

    case 122:
        efx_APhaser->changepar(APhase_Feedback, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 123:
        efx_APhaser->changepar(APhase_Mismatch, value);
        break;

    case 124:
        efx_APhaser->changepar(APhase_LFO_Stereo, value);
        break;

    case 125:
        efx_Derelict->changepar(Dere_DryWet, Dry_Wet(value));
        break;

    case 126:
        efx_Derelict->changepar(Dere_Pan, value);
        break;

    case 127:
        efx_Derelict->changepar(Dere_LR_Cross, value);
        break;

    case 2:
        efx_Derelict->changepar(Dere_Drive, value);
        break;

    case 3:
        efx_Derelict->changepar(Dere_Level, value);
        break;

    case 4:
        efx_Derelict->changepar(Dere_LPF, ret_LPF(value));
        break;

    case 5:
        efx_Derelict->changepar(Dere_HPF, ret_HPF(value));
        break;

    case 6:
        efx_Derelict->changepar(Dere_Color, value);
        break;

    case 8:
        efx_Derelict->changepar(Dere_Suboctave, value);
        break;

    case 9:
        efx_Distorsion->changepar(Dist_Suboctave, value);
        break;

    case 12:
        Fraction_Bypass = (float) value / 127.0f;
        break;

    case 14:
        Input_Gain = (float) value / 128.0f;
        calculavol(1);
        break;

    case 130:
        efx_EQ1->changepar(0, value);
        break;

    case 131:
        for (int i = 0; i < 10; i++) efx_EQ1->changepar(i * 5 + 13, value);
        break;

    case 132:
        efx_EQ1->changepar(12, value);
        break;

    case 133:
        efx_EQ1->changepar(5 + 12, value);
        break;

    case 134:
        efx_EQ1->changepar(10 + 12, value);
        break;

    case 135:
        efx_EQ1->changepar(15 + 12, value);
        break;

    case 136:
        efx_EQ1->changepar(20 + 12, value);
        break;

    case 137:
        efx_EQ1->changepar(25 + 12, value);
        break;

    case 138:
        efx_EQ1->changepar(30 + 12, value);
        break;

    case 139:
        efx_EQ1->changepar(35 + 12, value);
        break;

    case 140:
        efx_EQ1->changepar(40 + 12, value);
        break;

    case 141:
        efx_EQ1->changepar(45 + 12, value);
        break;

    case 142:
        efx_Compressor->changepar(Compress_Attack, 10 + (int) ((float) value * C_MC_240_RANGE));
        break;

    case 143:
        efx_Compressor->changepar(Compress_Release, 10 + (int) ((float) value * C_MC_490_RANGE));
        break;

    case 144:
        efx_Compressor->changepar(Compress_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 145:
        efx_Compressor->changepar(Compress_Knee, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 146:
        efx_Compressor->changepar(Compress_Threshold, -60 + (int) ((float) value * C_MC_57_RANGE));
        break;

    case 147:
        efx_Compressor->changepar(Compress_Output, -40 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 148:
        efx_EQ2->changepar(0, value);
        break;

    case 149:
        efx_EQ2->changepar(11, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 150:
        efx_EQ2->changepar(12, value);
        break;

    case 151:
        efx_EQ2->changepar(13, value);
        break;

    case 152:
        efx_EQ2->changepar(16, 800 + (int) ((float) value * C_MC_7200_RANGE));
        break;

    case 153:
        efx_EQ2->changepar(17, value);
        break;

    case 154:
        efx_EQ2->changepar(18, value);
        break;

    case 155:
        efx_EQ2->changepar(21, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case 156:
        efx_EQ2->changepar(22, value);
        break;

    case 157:
        efx_EQ2->changepar(23, value);
        break;

    case 158:
        efx_DFlange->changepar(DFlange_DryWet, Dry_Wet(value));
        break;

    case 159:
        efx_DFlange->changepar(DFlange_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 160:
        efx_DFlange->changepar(DFlange_LR_Cross, value);
        break;

    case 161:
        efx_DFlange->changepar(DFlange_Depth, 20 + (int) ((float) value * C_MC_2480_RANGE));
        break;

    case 162:
        efx_DFlange->changepar(DFlange_Width, (int) ((float) value * C_MC_6000_RANGE));
        break;

    case 163:
        efx_DFlange->changepar(DFlange_Offset, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 164:
        efx_DFlange->changepar(DFlange_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        break;

    case 165:
        /* This is labeled LPF but uses same range as HPF - FIXME check DSP */
        efx_DFlange->changepar(DFlange_LPF, 20 + (int) ((float) value * C_MC_19980_RANGE));
        break;

    case 166:
        efx_DFlange->changepar(DFlange_LFO_Tempo, ret_Tempo(value));
        break;

    case 167:
        efx_DFlange->changepar(DFlange_LFO_Stereo, value);
        break;

    case 168:
        efx_DFlange->changepar(DFlange_LFO_Rand, value);
        break;

    case 169:
        efx_Valve->changepar(0, Dry_Wet(value));
        break;

    case 170:
        efx_Valve->changepar(2, value);
        break;

    case 171:
        efx_Valve->changepar(1, value);
        break;

    case 172:
        efx_Valve->changepar(4, value);
        break;

    case 173:
        efx_Valve->changepar(3, value);
        break;

    case 174:
        efx_Valve->changepar(10, value);
        break;

    case 175:
        efx_Valve->changepar(12, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 176:
        efx_Valve->changepar(6, ret_LPF(value));
        break;

    case 177:
        efx_Valve->changepar(7, ret_HPF(value));
        break;

    case 178:
        efx_Ring->changepar(Ring_DryWet, Dry_Wet(value));
        break;

    case 179:
        efx_Ring->changepar(Ring_LR_Cross, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 180:
        efx_Ring->changepar(Ring_Input, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 181:
        efx_Ring->changepar(Ring_Level, value);
        break;

    case 182:
        efx_Ring->changepar(Ring_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 183:
        efx_Ring->changepar(Ring_Depth, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 184:
        efx_Ring->changepar(Ring_Freq, 1 + (int) ((float) value * C_MC_19999_RANGE));
        break;

    case 185:
        efx_Ring->changepar(Ring_Sine, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 186:
        efx_Ring->changepar(Ring_Triangle, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 187:
        efx_Ring->changepar(Ring_Saw, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 188:
        efx_Ring->changepar(Ring_Square, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 189:
        efx_Exciter->changepar(Exciter_Gain, value);
        break;

    case 190:
        efx_Exciter->changepar(Exciter_LPF, ret_LPF(value));
        break;

    case 191:
        efx_Exciter->changepar(Exciter_HPF, ret_HPF(value));
        break;

    case 192:
        efx_Exciter->changepar(Exciter_Harm_1, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 193:
        efx_Exciter->changepar(Exciter_Harm_2, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 194:
        efx_Exciter->changepar(Exciter_Harm_3, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 195:
        efx_Exciter->changepar(Exciter_Harm_4, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 196:
        efx_Exciter->changepar(Exciter_Harm_5, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 197:
        efx_Exciter->changepar(Exciter_Harm_6, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 198:
        efx_Exciter->changepar(Exciter_Harm_7, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 199:
        efx_Exciter->changepar(Exciter_Harm_8, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 200:
        efx_Exciter->changepar(Exciter_Harm_9, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 201:
        efx_Exciter->changepar(Exciter_Harm_10, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 202:
        efx_DistBand->changepar(DistBand_DryWet, Dry_Wet(value));
        break;

    case 203:
        efx_DistBand->changepar(DistBand_LR_Cross, value);
        break;

    case 204:
        efx_DistBand->changepar(DistBand_Drive, value);
        break;

    case 205:
        efx_DistBand->changepar(DistBand_Level, value);
        break;

    case 206:
        efx_DistBand->changepar(DistBand_Gain_Low, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 207:
        efx_DistBand->changepar(DistBand_Gain_Mid, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 208:
        efx_DistBand->changepar(DistBand_Gain_Hi, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 209:
        efx_DistBand->changepar(DistBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 210:
        efx_DistBand->changepar(DistBand_Cross_2, 800 + (int) ((float) value * C_MC_11200_RANGE));
        break;

    case 211:
        efx_DistBand->changepar(DistBand_Pan, value);
        break;

    case 212:
        efx_Arpie->changepar(Arpie_DryWet, Dry_Wet(value));
        break;

    case 213:
        efx_Arpie->changepar(Arpie_ArpeWD, value);
        break;

    case 214:
        efx_Arpie->changepar(Arpie_Pan, value);
        break;

    case 215:
        efx_Arpie->changepar(Arpie_Tempo, ret_Tempo(value));
        break;

    case 216:
        efx_Arpie->changepar(Arpie_LR_Delay, value);
        break;

    case 217:
        efx_Arpie->changepar(Arpie_LR_Cross, value);
        break;

    case 218:
        efx_Arpie->changepar(Arpie_Feedback, value);
        break;

    case 219:
        efx_Arpie->changepar(Arpie_Damp, value);
        break;

    case 220:
        efx_Expander->changepar(Expander_Attack, 1 + (int) ((float) value * C_MC_4999_RANGE));
        break;

    case 221:
        efx_Expander->changepar(Expander_Release, 10 + (int) ((float) value * C_MC_990_RANGE));
        break;

    case 222:
        efx_Expander->changepar(Expander_Shape, 1 + (int) ((float) value * C_MC_49_RANGE));
        break;

    case 223:
        efx_Expander->changepar(Expander_Threshold, (int) ((float) value * -C_MC_80_RANGE));
        break;

    case 224:
        efx_Expander->changepar(Expander_Gain, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 225:
        efx_Expander->changepar(Expander_LPF, ret_LPF(value));
        break;

    case 226:
        efx_Expander->changepar(Expander_HPF, ret_HPF(value));
        break;

    case 227:
        efx_Shuffle->changepar(Shuffle_DryWet, Dry_Wet(value));
        break;

    case 228:
        efx_Shuffle->changepar(Shuffle_Freq_L, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 229:
        efx_Shuffle->changepar(Shuffle_Gain_L, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 230:
        efx_Shuffle->changepar(Shuffle_Freq_ML, 400 + (int) ((float) value * C_MC_3600_RANGE));
        break;

    case 231:
        efx_Shuffle->changepar(Shuffle_Gain_ML, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 232:
        efx_Shuffle->changepar(Shuffle_Freq_MH, 1200 + (int) ((float) value * C_MC_6800_RANGE));
        break;

    case 233:
        efx_Shuffle->changepar(Shuffle_Gain_MH, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 234:
        efx_Shuffle->changepar(Shuffle_Freq_H, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        break;

    case 235:
        efx_Shuffle->changepar(Shuffle_Gain_H, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 236:
        efx_Shuffle->changepar(Shuffle_Width, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 237:
        efx_Synthfilter->changepar(0, Dry_Wet(value));
        break;

    case 238:
        efx_Synthfilter->changepar(1, value);
        break;

    case 239:
        efx_Synthfilter->changepar(2, ret_Tempo(value));
        break;

    case 240:
        efx_Synthfilter->changepar(5, value);
        break;

    case 241:
        efx_Synthfilter->changepar(6, value);
        break;

    case 242:
        efx_Synthfilter->changepar(7, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 243:
        efx_Synthfilter->changepar(11, value);
        break;

    case 244:
        efx_Synthfilter->changepar(12, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 245:
        efx_Synthfilter->changepar(13, 5 + (int) ((float) value * C_MC_995_RANGE));
        break;

    case 246:
        efx_Synthfilter->changepar(14, 5 + (int) ((float) value * C_MC_495_RANGE));
        break;

    case 247:
        efx_Synthfilter->changepar(15, value);
        break;

    case 248:
        efx_VaryBand->changepar(0, Dry_Wet(value));
        break;

    case 249:
        efx_VaryBand->changepar(1, ret_Tempo(value));
        break;

    case 250:
        efx_VaryBand->changepar(3, value);
        break;

    case 251:
        efx_VaryBand->changepar(4, ret_Tempo(value));
        break;

    case 252:
        efx_VaryBand->changepar(6, value);
        break;

    case 253:
        efx_VaryBand->changepar(7, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 254:
        efx_VaryBand->changepar(8, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case 255:
        efx_VaryBand->changepar(9, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case 256:
        efx_MuTroMojo->changepar(MuTro_DryWet, Dry_Wet(value));
        break;

    case 257:
        efx_MuTroMojo->changepar(MuTro_LowPass, value - 64);
        break;

    case 258:
        efx_MuTroMojo->changepar(MuTro_BandPass, value - 64);
        break;

    case 259:
        efx_MuTroMojo->changepar(MuTro_HighPass, value - 64);
        break;

    case 260:
        efx_MuTroMojo->changepar(MuTro_Depth, value);
        break;

    case 261:
        efx_MuTroMojo->changepar(MuTro_LFO_Tempo, ret_Tempo(value));
        break;

    case 262:
        efx_MuTroMojo->changepar(MuTro_Resonance, value);
        break;

    case 263:
        efx_MuTroMojo->changepar(MuTro_Range, 10 + (int) ((float) value * C_MC_5990_RANGE));
        break;

    case 264:
        efx_MuTroMojo->changepar(MuTro_Wah, value);
        break;

    case 265:
        efx_MuTroMojo->changepar(MuTro_Env_Sens, value - 64);
        break;

    case 266:
        efx_MuTroMojo->changepar(MuTro_Env_Smooth, value);
        break;

    case 267:
        efx_Looper->changepar(Looper_DryWet, Dry_Wet(value));
        break;

    case 268:
        efx_Looper->changepar(Looper_Level_1, value);
        break;

    case 269:
        efx_Looper->changepar(Looper_Level_2, value);
        break;

    case 270:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Reverse, i);
        break;
    }
    case 271:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_AutoPlay, i);
        break;
    }
    case 272:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Play, i);
        break;
    }
    case 273:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Stop, i);
        break;
    }
    case 274:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Record, i);
        break;
    }
    case 275:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Rec_1, i);
        break;
    }
    case 276:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Rec_2, i);
        break;
    }
    case 277:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Track_1, i);
        break;
    }
    case 278:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Track_2, i);
        break;
    }
    case 279:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Looper_Clear, i);
        break;
    }
    case 280:
        efx_Convol->changepar(Convo_DryWet, Dry_Wet(value));
        break;

    case 281:
        efx_Convol->changepar(Convo_Pan, value);
        break;

    case 282:
        efx_Convol->changepar(Convo_Level, value);
        break;

    case 283:
        efx_Convol->changepar(Convo_Damp, value);
        break;

    case 284:
        efx_Convol->changepar(Convo_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        break;

    case 285:
        efx_Convol->changepar(Convo_Length, 5 + (int) ((float) value * C_MC_245_RANGE));
        break;

    case 286:
        efx_CoilCrafter->changepar(Coil_Gain, value);
        break;

    case 287:
        efx_CoilCrafter->changepar(Coil_Tone, 20 + (int) ((float) value * C_MC_4380_RANGE));
        break;

    case 288:
        efx_CoilCrafter->changepar(Coil_Freq_1, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case 289:
        efx_CoilCrafter->changepar(Coil_Q_1, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case 290:
        efx_CoilCrafter->changepar(Coil_Freq_2, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        break;

    case 291:
        efx_CoilCrafter->changepar(Coil_Q_2, 10 + (int) ((float) value * C_MC_55_RANGE));
        break;

    case 292:
        efx_ShelfBoost->changepar(Shelf_Gain, value);
        break;

    case 293:
        efx_ShelfBoost->changepar(Shelf_Level, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 294:
        efx_ShelfBoost->changepar(Shelf_Tone, 220 + (int) ((float) value * C_MC_15780_RANGE));
        break;

    case 295:
        efx_ShelfBoost->changepar(Shelf_Presence, ((int) (float) value * C_MC_128_RANGE)  - 64);
        break;

    case 296:
        efx_Vocoder->changepar(0, Dry_Wet(value));
        break;

    case 297:
        efx_Vocoder->changepar(1, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 298:
        efx_Vocoder->changepar(4, value);
        break;

    case 299:
        efx_Vocoder->changepar(2, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 300:
        efx_Vocoder->changepar(3, 40 + (int) ((float) value * C_MC_130_RANGE));
        break;

    case 301:
        efx_Vocoder->changepar(6, value);
        break;

    case 302:
        efx_Vocoder->changepar(5, value);
        break;

    case 303:
        efx_Echoverse->changepar(Echoverse_DryWet, Dry_Wet(value));
        break;

    case 304:
        efx_Echoverse->changepar(Echoverse_Reverse, value);
        break;

    case 305:
        efx_Echoverse->changepar(Echoverse_Pan, value);
        break;

    case 306:
        efx_Echoverse->changepar(Echoverse_Tempo, ret_Tempo(value));
        break;

    case 307:
        efx_Echoverse->changepar(Echoverse_LR_Delay, value);
        break;

    case 308:
        efx_Echoverse->changepar(Echoverse_Feedback, value);
        break;

    case 309:
        efx_Echoverse->changepar(Echoverse_Damp, value);
        break;

    case 310:
        efx_Echoverse->changepar(Echoverse_Ext_Stereo, value);
        break;

    case 311:
        efx_Echoverse->changepar(Echoverse_Angle, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 312:
        efx_Sustainer->changepar(0, value);
        break;

    case 313:
        efx_Sustainer->changepar(1, value);
        break;

    case 314:
        efx_Sequence->changepar(Sequence_DryWet, Dry_Wet(value));
        break;

    case 315:
        efx_Sequence->changepar(Sequence_Step_1, value);
        break;

    case 316:
        efx_Sequence->changepar(Sequence_Step_2, value);
        break;

    case 317:
        efx_Sequence->changepar(Sequence_Step_3, value);
        break;

    case 318:
        efx_Sequence->changepar(Sequence_Step_4, value);
        break;

    case 319:
        efx_Sequence->changepar(Sequence_Step_5, value);
        break;

    case 320:
        efx_Sequence->changepar(Sequence_Step_6, value);
        break;

    case 321:
        efx_Sequence->changepar(Sequence_Step_7, value);
        break;

    case 322:
        efx_Sequence->changepar(Sequence_Step_8, value);
        break;

    case 323:
        efx_Sequence->changepar(Sequence_Tempo, ret_Tempo(value));
        break;

    case 324:
        efx_Sequence->changepar(Sequence_Resonance, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 325:
        efx_Sequence->changepar(Sequence_Stdf, (int) ((float) value * C_MC_7_RANGE));
        break;

    case 326:
        efx_Shifter->changepar(Shifter_DryWet, Dry_Wet(value));
        break;

    case 327:
        efx_Shifter->changepar(Shifter_Interval, (int) ((float) value * C_MC_12_RANGE));
        break;

    case 328:
        efx_Shifter->changepar(Shifter_Gain, value);
        break;

    case 329:
        efx_Shifter->changepar(Shifter_Pan, value);
        break;

    case 330:
        efx_Shifter->changepar(Shifter_Attack, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case 331:
        efx_Shifter->changepar(Shifter_Decay, 1 + (int) ((float) value * C_MC_1999_RANGE));
        break;

    case 332:
        efx_Shifter->changepar(Shifter_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case 333:
        efx_Shifter->changepar(Shifter_Whammy, value);
        break;

    case 334:
        efx_StompBox->changepar(0, value);
        break;

    case 335:
        efx_StompBox->changepar(4, value);
        break;

    case 336:
        efx_StompBox->changepar(3, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 337:
        efx_StompBox->changepar(2, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 338:
        efx_StompBox->changepar(1, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 339:
        efx_Reverbtron->changepar(Revtron_DryWet, Dry_Wet(value));
        break;

    case 340:
        efx_Reverbtron->changepar(Revtron_Pan, value);
        break;

    case 341:
        efx_Reverbtron->changepar(Revtron_Level, value);
        break;

    case 342:
        efx_Reverbtron->changepar(Revtron_Damp, value);
        break;

    case 343:
        efx_Reverbtron->changepar(Revtron_Feedback, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 344:
        efx_Reverbtron->changepar(Revtron_Length, 20 + (int) ((float) value * C_MC_1480_RANGE));
        break;

    case 345:
        efx_Reverbtron->changepar(Revtron_Stretch, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 346:
        efx_Reverbtron->changepar(Revtron_I_Delay, (int) ((float) value * C_MC_500_RANGE));
        break;

    case 347:
        efx_Reverbtron->changepar(Revtron_Fade, value);
        break;

    case 348:
        efx_Echotron->changepar(Echotron_DryWet, Dry_Wet(value));
        break;

    case 349:
        efx_Echotron->changepar(Echotron_Pan, value);
        break;

    case 350:
        efx_Echotron->changepar(Echotron_Tempo, ret_Tempo(value));
        break;

    case 351:
        efx_Echotron->changepar(Echotron_Damp, value);
        break;

    case 352:
        efx_Echotron->changepar(Echotron_Feedback, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 353:
        efx_Echotron->changepar(Echotron_LR_Cross, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 354:
        efx_Echotron->changepar(Echotron_LFO_Width, value);
        break;

    case 355:
        efx_Echotron->changepar(Echotron_Depth, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 356:
        efx_Echotron->changepar(Echotron_LFO_Stereo, value);
        break;

    case 357:
    {
        /* There is a minor problem with this approach. If the user sets the delay
           file after setting the taps, then the taps will not get adjusted upward. 
           The user will have to move the MIDI control slider to set taps upward.
           The downward limit is checked and limited always on file loading. */
        int number_taps = 1 + (int) ((float) value * C_MC_126_RANGE);
        if(number_taps > efx_Echotron->File.fLength)
        {
            number_taps = efx_Echotron->File.fLength;
        }
        efx_Echotron->changepar(Echotron_Taps, number_taps);
        break;
    }
    case 358:
        efx_StereoHarm->changepar(0, Dry_Wet(value));
        break;

    case 359:
        efx_StereoHarm->changepar(2, -12 + (int) ((float) value * C_MC_24_RANGE));
        break;

    case 360:
        efx_StereoHarm->changepar(3, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case 361:
        efx_StereoHarm->changepar(1, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 362:
        efx_StereoHarm->changepar(5, -12 + (int) ((float) value * C_MC_24_RANGE));
        break;

    case 363:
        efx_StereoHarm->changepar(6, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        break;

    case 364:
        efx_StereoHarm->changepar(4, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 365:
        efx_StereoHarm->changepar(11, value);
        break;

    case 366:
        efx_StereoHarm->changepar(8, (int) ((float) value * C_MC_23_RANGE));
        break;

    case 367:
        efx_StereoHarm->changepar(9, (int) ((float) value * C_MC_33_RANGE));
        break;

    case 368:
        efx_CompBand->changepar(CompBand_DryWet, Dry_Wet(value));
        break;

    case 369:
        efx_CompBand->changepar(CompBand_Gain, value);
        break;

    case 370:
        efx_CompBand->changepar(CompBand_Low_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 371:
        efx_CompBand->changepar(CompBand_Mid_1_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 372:
        efx_CompBand->changepar(CompBand_Mid_2_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 373:
        efx_CompBand->changepar(CompBand_High_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        break;

    case 374:
        efx_CompBand->changepar(CompBand_Low_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 375:
        efx_CompBand->changepar(CompBand_Mid_1_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 376:
        efx_CompBand->changepar(CompBand_Mid_2_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 377:
        efx_CompBand->changepar(CompBand_High_Thresh, -70 + (int) ((float) value * C_MC_94_RANGE));
        break;

    case 378:
        efx_CompBand->changepar(CompBand_Cross_1, 20 + (int) ((float) value * C_MC_980_RANGE));
        break;

    case 379:
        efx_CompBand->changepar(CompBand_Cross_2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        break;

    case 380:
        efx_CompBand->changepar(CompBand_Cross_3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        break;

    case 381:
        efx_Opticaltrem->changepar(Optical_Depth, value);
        break;

    case 382:
        efx_Opticaltrem->changepar(Optical_LFO_Tempo, ret_Tempo(value));
        break;

    case 383:
        efx_Opticaltrem->changepar(Optical_LFO_Random, value);
        break;

    case 384:
        efx_Opticaltrem->changepar(Optical_LFO_Stereo, value);
        break;

    case 385:
        efx_Opticaltrem->changepar(Optical_Pan, value);
        break;

    case 386:
        efx_Vibe->changepar(6, Dry_Wet(value));
        break;

    case 387:
        efx_Vibe->changepar(0, value);
        break;

    case 388:
        efx_Vibe->changepar(8, value);
        break;

    case 389:
        efx_Vibe->changepar(1, ret_Tempo(value));
        break;

    case 390:
        efx_Vibe->changepar(2, value);
        break;

    case 391:
        efx_Vibe->changepar(4, value);
        break;

    case 392:
        efx_Vibe->changepar(7, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 393:
        efx_Vibe->changepar(9, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 394:
        efx_Vibe->changepar(5, (int) ((float) value * C_MC_128_RANGE));
        break;

    case 395:
        efx_Infinity->changepar(Infinity_DryWet, Dry_Wet(value));
        break;

    case 396:
        efx_Infinity->changepar(Infinity_Resonance, -1000 + (int) ((float) value * C_MC_2000_RANGE));
        break;

    case 397:
        efx_Infinity->changepar(Infinity_AutoPan, value);
        break;

    case 398:
        efx_Infinity->changepar(Infinity_LR_Delay, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 399:
        efx_Infinity->changepar(Infinity_Start, value);
        break;

    case 400:
        efx_Infinity->changepar(Infinity_End, value);
        break;

    case 401:
        efx_Infinity->changepar(Infinity_Tempo, ret_Tempo(value));
        break;

    case 402:
        efx_Infinity->changepar(Infinity_Subdivision, -16 + (int) ((float) value * C_MC_32_RANGE));
        break;

    case 403:
        efx_Alienwah->changepar(Alien_Delay, (int) ((float) value * C_MC_100_RANGE));
        break;

    case 404:
        efx_APhaser->changepar(APhase_LFO_Rand, value);
        break;

    case 405:
        efx_Cabinet->changepar(0, value);
        break;

    case 406:
        efx_Chorus->changepar(Chorus_Delay, value);
        break;
    
    case 407:
        efx_Echo->changepar(Echo_Reverse, value);
        break;

    case 408:
        efx_Echo->changepar(Echo_Delay, 20 + (int) ((float) value * C_MC_1980_RANGE));
        break;

    case 409:
        efx_Echo->changepar(Echo_LR_Delay, value);
        break;

    case 410:
        efx_Echo->changepar(Echo_Damp, value);
        break;

    case 411:
        efx_Flanger->changepar(Chorus_Delay, value);
        break;
        
    case 412:
        efx_Har->changepar(Harm_Gain, value);
        break;

    case 413:
        efx_Har->changepar(Harm_Filter_Gain, value);
        break;

    case 414:
        efx_Har->changepar(Harm_Filter_Q, value);
        break;

    case 415:
        efx_Infinity->changepar(Infinity_Band_1, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 416:
        efx_Infinity->changepar(Infinity_Band_2, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 417:
        efx_Infinity->changepar(Infinity_Band_3, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 418:
        efx_Infinity->changepar(Infinity_Band_4, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 419:
        efx_Infinity->changepar(Infinity_Band_5, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 420:
        efx_Infinity->changepar(Infinity_Band_6, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 421:
        efx_Infinity->changepar(Infinity_Band_7, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 422:
        efx_Infinity->changepar(Infinity_Band_8, (int) ((float) value * C_MC_128_RANGE) - 64);
        break;

    case 423:
        efx_Looper->changepar(Looper_Tempo, 20 + (int) ((float) value * C_MC_360_RANGE));
        break;

    case 424:
        efx_MusDelay->changepar(Music_Tempo, 10 + (int) ((float) value * C_MC_470_RANGE));
        break;

    case 425:
        efx_MusDelay->changepar(Music_Damp, value);
        break;

    case 426:
        efx_MuTroMojo->changepar(MuTro_LFO_Random, value);
        break;

    case 427:
        efx_MuTroMojo->changepar(MuTro_LFO_Stereo, value);
        break;

    case 428:
        efx_MuTroMojo->changepar(MuTro_St_Freq, 30 + (int) ((float) value * C_MC_770_RANGE));
        break;

    case 429:
        efx_Gate->changepar(Gate_Attack, 1 + (int) ((float) value * C_MC_249_RANGE));
        break;

    case 430:
        efx_Gate->changepar(Gate_Release, 2 + (int) ((float) value * C_MC_248_RANGE));
        break;

    case 431:
        efx_Gate->changepar(Gate_Range, -90 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case 432:
        efx_Gate->changepar(Gate_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        break;

    case 433:
        efx_Gate->changepar(Gate_Hold, 2 + (int) ((float) value * C_MC_498_RANGE));
        break;

    case 434:
        efx_Gate->changepar(Gate_LPF, ret_LPF(value));
        break;

    case 435:
        efx_Gate->changepar(Gate_HPF, ret_HPF(value));
        break;

    case 436:
        efx_Pan->changepar(Pan_Ex_St_Amt, value);
        break;

    case 437:
        efx_Rev->changepar(Reverb_Time, value);
        break;

    case 438:
        efx_Rev->changepar(Reverb_I_Delay, value);
        break;

    case 439:
        efx_Rev->changepar(Reverb_Delay_FB, value);
        break;

    case 440:
        efx_Rev->changepar(Reverb_Room, 1 + (int) ((float) value * C_MC_126_RANGE));
        break;

    case 441:
        efx_Rev->changepar(Reverb_Damp, 64 + (int) ((float) value * C_MC_63_RANGE));
        break;

    case 442:
        efx_Reverbtron->changepar(Revtron_Diffusion, value);
        break;

    case 443:
        efx_Reverbtron->changepar(Revtron_LPF, ret_LPF(value));
        break;

    case 444:
        efx_Har->changepar(Harm_Note, (int) ((float) value * C_MC_23_RANGE));
        break;

    case 445:
        efx_Har->changepar(Harm_Chord, (int) ((float) value * C_MC_33_RANGE));
        break;

    case 446:
        efx_Synthfilter->changepar(3, value);
        break;

    case 447:
        efx_Har->changepar(Harm_Select, value);
        break;

    case 448:
        efx_StereoHarm->changepar(7, value);
        break;

    case 449:
        efx_Distorsion->changepar(Dist_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 450:
        efx_Overdrive->changepar(Dist_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 451:
        efx_Derelict->changepar(Dere_Type, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 452:
        efx_DistBand->changepar(DistBand_Type_Low, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 453:
        efx_DistBand->changepar(DistBand_Type_Mid, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 454:
        efx_DistBand->changepar(DistBand_Type_Hi, (int) ((float) value * C_MC_30_RANGE));
        break;

    case 455:
        efx_StompBox->changepar(5, (int) ((float) value * C_MC_8_RANGE));
        break;

    case 456:
        efx_Alienwah->changepar(Alien_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 457:
        efx_APhaser->changepar(APhase_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 458:
        efx_Chorus->changepar(Chorus_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 459:
        efx_Flanger->changepar(Chorus_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 460:
        efx_DFlange->changepar(DFlange_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 461:
        efx_Echotron->changepar(Echotron_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 462:
        efx_MuTroMojo->changepar(MuTro_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 463:
        efx_Opticaltrem->changepar(Optical_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 464:
        efx_Pan->changepar(Pan_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 465:
        efx_Phaser->changepar(Phaser_LFO_Type, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 466:
        efx_Synthfilter->changepar(4, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 467:
        efx_VaryBand->changepar(2, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 468:
        efx_VaryBand->changepar(5, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 469:
        efx_Vibe->changepar(3, (int) ((float) value * C_MC_11_RANGE));
        break;

    case 470:
        efx_WhaWha->changepar(4, (int) ((float) value * C_MC_11_RANGE));
        break;
    }
}

int
RKR::ret_Tempo(int value)
{
    return ( 1 + (int) ((float) value * C_MC_600_RANGE));
}

int
RKR::ret_LPF(int value)
{
    return ( 20 + (int) ((float) value * C_MC_25980_RANGE));
}

int
RKR::ret_HPF(int value)
{
    return ( 20 + (int) ((float) value * C_MC_19980_RANGE));
}

