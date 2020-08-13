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
            efx_Looper->changepar(1, 1);
            stecla = 5;
        }

        if (midievent->type == SND_SEQ_EVENT_STOP)
        {
            efx_Looper->changepar(2, 1);
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

void
RKR::preserve_parm(EffectIndex effect, int param) {
    if (param > ParmCount) {
        printf("ERROR: Parameter index %d out of range!", param);
        return;
    }
    if (preset != 1000)
        preserve[effect][param] = true;
}

void
RKR::clear_preserved() {
    memset(preserve, 0, sizeof (preserve));
}

bool
RKR::canset(int effect, int parm) {
    if (effect < 0 || effect > FXCount) {
        printf("ERROR: Effect index %d out of range.", effect);
        return true;
    }
    if (parm < 0 || parm >= ParmCount) {
        printf("ERROR: Parameter index %d out of range.", parm);
        return true;
    }
    return canset(static_cast<EffectIndex>(effect),
                  static_cast<ParmIndex>(parm));
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
        preserve_parm(FX_Main, Parm_Volume);
        calculavol(2);
        break;

    case 1:
        efx_WhaWha->changepar(Parm_Depth, value);
        preserve_parm(FX_WahWah, Parm_Depth);
        break;

    case 20:
        efx_Alienwah->changepar(Parm_Depth, value);
        preserve_parm(FX_Alienwah, Parm_Depth);
        break;

    case 21:
        efx_Phaser->changepar(Parm_Depth, value);
        preserve_parm(FX_Phaser, Parm_Depth);
        break;

    case 22:
        efx_Flanger->changepar(Parm_Depth, value);
        preserve_parm(FX_Flanger, Parm_Depth);
        break;

    case 23:
        efx_Chorus->changepar(Parm_Depth, value);
        preserve_parm(FX_Chorus, Parm_Depth);
        break;

    case 24:
        efx_MusDelay->changepar(Parm_Gain1, value);
        preserve_parm(FX_MusDelay, Parm_Gain1);
        break;

    case 25:
        efx_MusDelay->changepar(Parm_Gain2, value);
        preserve_parm(FX_MusDelay, Parm_Gain2);
        break;

    case 26:
        efx_Har->changepar(Parm_Freq, ret_LPF(value));
        preserve_parm(FX_Harmonizer, Parm_Freq);
        break;

    case 27:
        if (Harmonizer_Bypass)
        {
            Harmonizer_Bypass = 0;
            efx_Har->changepar(Parm_Interval,
                               (int) ((float) value * C_MC_24_RANGE));
            preserve_parm(FX_Harmonizer, Parm_Interval);
            Harmonizer_Bypass = 1;
        }
        break;

    case 28:
        efx_WhaWha->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_WahWah, Parm_DryWet);
        break;

    case 29:
        efx_Overdrive->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Overdrive, Parm_DryWet);
        break;

    case 30:
        efx_Distorsion->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Distortion, Parm_DryWet);
        break;

    case 31:
        efx_Har->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Harmonizer, Parm_DryWet);
        break;

    case 52:
        efx_Chorus->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Chorus, Parm_DryWet);
        break;

    case 53:
        efx_Flanger->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Flanger, Parm_DryWet);
        break;

    case 54:
        efx_Phaser->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Phaser, Parm_DryWet);
        break;

    case 55:
        efx_Alienwah->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Alienwah, Parm_DryWet);
        break;

    case 56:
        efx_MusDelay->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_MusDelay, Parm_DryWet);
        break;

    case 57:
        efx_Rev->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Reverb, Parm_DryWet);
        break;

    case 58:
        efx_Pan->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Pan, Parm_DryWet);
        break;

    case 59:
        efx_Echo->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Echo, Parm_DryWet);
        break;

    case 46:
        efx_Echo->changepar(Parm_Pan, value);
        preserve_parm(FX_Echo, Parm_Pan);
        break;

    case 47:
        efx_Overdrive->changepar(Parm_Pan, value);
        preserve_parm(FX_Overdrive, Parm_Pan);
        break;

    case 48:
        efx_Distorsion->changepar(Parm_Pan, value);
        preserve_parm(FX_Distortion, Parm_Pan);
        break;

    case 49:
        efx_Har->changepar(Parm_Pan, value);
        preserve_parm(FX_Harmonizer, Parm_Pan);
        break;

    case 50:
        efx_Chorus->changepar(Parm_Pan, value);
        preserve_parm(FX_Chorus, Parm_Pan);
        break;

    case 51:
        efx_Flanger->changepar(Parm_Pan, value);
        preserve_parm(FX_Flanger, Parm_Pan);
        break;

    case 60:
        efx_Phaser->changepar(Parm_Pan, value);
        preserve_parm(FX_Phaser, Parm_Pan);
        break;

    case 61:
        efx_Alienwah->changepar(Parm_Pan, value);
        preserve_parm(FX_Alienwah, Parm_Pan);
        break;

    case 62:
        efx_MusDelay->changepar(Parm_Pan, value);
        preserve_parm(FX_MusDelay, Parm_Pan);
        break;

    case 63:
        efx_Rev->changepar(Parm_Pan, value);
        preserve_parm(FX_Reverb, Parm_Pan);
        break;

    case 65:
        efx_MusDelay->changepar(Parm_Pan2, value);
        preserve_parm(FX_MusDelay, Parm_Pan2);
        break;

    case 66:
        efx_WhaWha->changepar(Parm_Pan, value);
        preserve_parm(FX_WahWah, Parm_Pan);
        break;

    case 67:
        efx_Pan->changepar(Parm_Pan, value);
        preserve_parm(FX_Pan, Parm_Pan);
        break;

    case 68:
        efx_Overdrive->changepar(Parm_Drive, value);
        preserve_parm(FX_Overdrive, Parm_Drive);
        break;

    case 69:
        efx_Distorsion->changepar(Parm_Drive, value);
        preserve_parm(FX_Distortion, Parm_Drive);
        break;

    case 70:
        efx_Overdrive->changepar(Parm_Level, value);
        preserve_parm(FX_Overdrive, Parm_Level);
        break;

    case 71:
        efx_Distorsion->changepar(Parm_Level, value);
        preserve_parm(FX_Distortion, Parm_Level);
        break;

    case 72:
        efx_Chorus->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Chorus, Parm_LFOFreq);
        break;

    case 73:
        efx_Flanger->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Flanger, Parm_LFOFreq);
        break;

    case 74:
        efx_Phaser->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Phaser, Parm_LFOFreq);
        break;

    case 75:
        efx_WhaWha->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_WahWah, Parm_LFOFreq);
        break;

    case 76:
        efx_Alienwah->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Alienwah, Parm_LFOFreq);
        break;

    case 77:
        efx_Pan->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Pan, Parm_LFOFreq);
        break;

    case 78:
        efx_Echo->changepar(Parm_Feedback, value);
        preserve_parm(FX_Echo, Parm_Feedback);
        break;

    case 79:
        efx_Chorus->changepar(Parm_Chorus_Feedback, value);
        preserve_parm(FX_Chorus, Parm_Chorus_Feedback);
        break;

    case 80:
        efx_Flanger->changepar(Parm_Chorus_Feedback, value);
        preserve_parm(FX_Flanger, Parm_Chorus_Feedback);
        break;

    case 81:
        efx_Phaser->changepar(Parm_Phaser_Feedback, value);
        preserve_parm(FX_Phaser, Parm_Phaser_Feedback);
        break;

    case 82:
        efx_Alienwah->changepar(Parm_Phaser_Feedback, value);
        preserve_parm(FX_Alienwah, Parm_Phaser_Feedback);
        break;

    case 83:
        efx_MusDelay->changepar(Parm_Feedback, value);
        preserve_parm(FX_MusDelay, Parm_Feedback);
        break;

    case 84:
        efx_MusDelay->changepar(Parm_MusDelay_Feedback2, value);
        preserve_parm(FX_MusDelay, Parm_MusDelay_Feedback2);
        break;

    case 85:
        efx_Overdrive->changepar(Parm_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Overdrive, Parm_LowPassFilter);
        break;

    case 86:
        efx_Distorsion->changepar(Parm_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Distortion, Parm_LowPassFilter);
        break;

    case 87:
        efx_Rev->changepar(Parm_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Reverb, Parm_LowPassFilter);
        break;

    case 88:
        efx_Overdrive->changepar(Parm_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Overdrive, Parm_HighPassFilter);
        break;

    case 89:
        efx_Distorsion->changepar(Parm_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Distortion, Parm_HighPassFilter);
        break;

    case 90:
        efx_Rev->changepar(Parm_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Reverb, Parm_HighPassFilter);
        break;

    case 91:
        efx_Chorus->changepar(Parm_LRCross, value);
        preserve_parm(FX_Chorus, Parm_LRCross);
        break;

    case 92:
        efx_Flanger->changepar(Parm_LRCross, value);
        preserve_parm(FX_Flanger, Parm_LRCross);
        break;

    case 93:
        efx_Phaser->changepar(Parm_LRCross, value);
        preserve_parm(FX_Phaser, Parm_LRCross);
        break;

    case 94:
        efx_Overdrive->changepar(Parm_Dist_LRCross, value);
        preserve_parm(FX_Overdrive, Parm_Dist_LRCross);
        break;

    case 95:
        efx_Distorsion->changepar(Parm_Dist_LRCross, value);
        preserve_parm(FX_Distortion, Parm_Dist_LRCross);
        break;

    case 96:
        efx_Alienwah->changepar(Parm_LRCross, value);
        preserve_parm(FX_Alienwah, Parm_LRCross);
        break;

    case 97:
        efx_Echo->changepar(Parm_Echo_LRCross, value);
        preserve_parm(FX_Echo, Parm_Echo_LRCross);
        break;

    case 98:
        efx_MusDelay->changepar(Parm_Echo_LRCross, value);
        preserve_parm(FX_MusDelay, Parm_Echo_LRCross);
        break;

    case 99:
        efx_Chorus->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_Chorus, Parm_LFOStereo);
        break;

    case 100:
        efx_Flanger->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_Flanger, Parm_LFOStereo);
        break;

    case 101:
        efx_Phaser->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_Phaser, Parm_LFOStereo);
        break;

    case 102:
        efx_WhaWha->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_WahWah, Parm_LFOStereo);
        break;

    case 103:
        efx_Alienwah->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_Alienwah, Parm_LFOStereo);
        break;

    case 104:
        efx_Pan->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_Pan, Parm_LFOStereo);
        break;

    case 105:
        efx_Chorus->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_Chorus, Parm_LFORandomness);
        break;

    case 106:
        efx_Flanger->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_Flanger, Parm_LFORandomness);
        break;

    case 107:
        efx_Phaser->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_Phaser, Parm_LFORandomness);
        break;

    case 108:
        efx_WhaWha->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_WahWah, Parm_LFORandomness);
        break;

    case 109:
        efx_Alienwah->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_Alienwah, Parm_LFORandomness);
        break;

    case 110:
        efx_Pan->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_Pan, Parm_LFORandomness);
        break;

    case 111:
        efx_WhaWha->changepar(Parm_AmpSns, value);
        preserve_parm(FX_WahWah, Parm_AmpSns);
        break;

    case 112:
        efx_WhaWha->changepar(Parm_AmpSnsNS, value);
        preserve_parm(FX_WahWah, Parm_AmpSnsNS);
        break;

    case 113:
        efx_WhaWha->changepar(Parm_AmpSnsSmooth, value);
        preserve_parm(FX_WahWah, Parm_AmpSnsSmooth);
        break;

    case 114:
        efx_Phaser->changepar(Parm_Phase, value);
        preserve_parm(FX_Phaser, Parm_Phase);
        break;

    case 115:
        efx_Alienwah->changepar(Parm_Alien_Phase, value);
        preserve_parm(FX_Alienwah, Parm_Alien_Phase);
        break;

    case 116:
        ActiveUn(value);
        break;

    case 117:
        efx_APhaser->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_APhaser, Parm_DryWet);
        break;

    case 118:
        efx_APhaser->changepar(Parm_Distortion, value);
        preserve_parm(FX_APhaser, Parm_Distortion);
        break;

    case 119:
        efx_APhaser->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_APhaser, Parm_LFOFreq);
        break;

    case 120:
        efx_APhaser->changepar(Parm_APhaser_Depth, value);
        preserve_parm(FX_APhaser, Parm_APhaser_Depth);
        break;

    case 121:
        efx_APhaser->changepar(Parm_Width, value);
        preserve_parm(FX_APhaser, Parm_Width);
        break;

    case 122:
        efx_APhaser->changepar(Parm_Phaser_Feedback, 
                               (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_APhaser, Parm_Phaser_Feedback);
        break;

    case 123:
        efx_APhaser->changepar(Parm_Offset, value);
        preserve_parm(FX_APhaser, Parm_Offset);
        break;

    case 124:
        efx_APhaser->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_APhaser, Parm_LFOStereo);
        break;

    case 125:
        efx_Derelict->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Derelict, Parm_DryWet);
        break;

    case 126:
        efx_Derelict->changepar(Parm_Pan, value);
        preserve_parm(FX_Derelict, Parm_Pan);
        break;

    case 127:
        efx_Derelict->changepar(Parm_Dist_LRCross, value);
        preserve_parm(FX_Derelict, Parm_Dist_LRCross);
        break;

    case 2:
        efx_Derelict->changepar(Parm_Drive, value);
        preserve_parm(FX_Derelict, Parm_Drive);
        break;

    case 3:
        efx_Derelict->changepar(Parm_Level, value);
        preserve_parm(FX_Derelict, Parm_Level);
        break;

    case 4:
        efx_Derelict->changepar(Parm_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Derelict, Parm_LowPassFilter);
        break;

    case 5:
        efx_Derelict->changepar(Parm_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Derelict, Parm_HighPassFilter);
        break;

    case 6:
        efx_Derelict->changepar(Parm_RFreq, value);
        preserve_parm(FX_Derelict, Parm_RFreq);
        break;

    case 8:
        efx_Derelict->changepar(Parm_Octave, value);
        preserve_parm(FX_Derelict, Parm_Octave);
        break;

    case 9:
        efx_Distorsion->changepar(Parm_Dist_Octave, value);
        preserve_parm(FX_Distortion, Parm_Dist_Octave);
        break;

    case 12:
        Fraction_Bypass = (float) value / 127.0f;
        preserve_parm(FX_Main, Parm_FractionBypass);
        break;

    case 14:
        Input_Gain = (float) value / 128.0f;
        preserve_parm(FX_Main, Parm_InputGain);
        calculavol(1);
        break;

    case 130:
        efx_EQ1->changepar(Parm_Volume, value);
        preserve_parm(FX_EQ1, Parm_Volume);
        break;

    case 131:
        for (int i = 0; i < 10; i++) {
            efx_EQ1->changepar(i * 5 + 13, value);
            preserve_parm(FX_EQ1, i * 5 + 13);
        }
        break;

    case 132:
        efx_EQ1->changepar(12, value);
        preserve_parm(FX_EQ1, 12);
        break;

    case 133:
        efx_EQ1->changepar(5 + 12, value);
        preserve_parm(FX_EQ1, 5 + 12);
        break;

    case 134:
        efx_EQ1->changepar(10 + 12, value);
        preserve_parm(FX_EQ1, 10 + 12);
        break;

    case 135:
        efx_EQ1->changepar(15 + 12, value);
        preserve_parm(FX_EQ1, 15 + 12);
        break;

    case 136:
        efx_EQ1->changepar(20 + 12, value);
        preserve_parm(FX_EQ1, 20 + 12);
        break;

    case 137:
        efx_EQ1->changepar(25 + 12, value);
        preserve_parm(FX_EQ1, 25 + 12);
        break;

    case 138:
        efx_EQ1->changepar(30 + 12, value);
        preserve_parm(FX_EQ1, 30 + 12);
        break;

    case 139:
        efx_EQ1->changepar(35 + 12, value);
        preserve_parm(FX_EQ1, 35 + 12);
        break;

    case 140:
        efx_EQ1->changepar(40 + 12, value);
        preserve_parm(FX_EQ1, 40 + 12);
        break;

    case 141:
        efx_EQ1->changepar(45 + 12, value);
        preserve_parm(FX_EQ1, 45 + 12);
        break;

    case 142:
        efx_Compressor->changepar(Parm_AttTime, 10 + (int) ((float) value * C_MC_240_RANGE));
        preserve_parm(FX_Compressor, Parm_AttTime);
        break;

    case 143:
        efx_Compressor->changepar(Parm_RelTime, 10 + (int) ((float) value * C_MC_490_RANGE));
        preserve_parm(FX_Compressor, Parm_RelTime);
        break;

    case 144:
        efx_Compressor->changepar(Parm_Ratio, 2 + (int) ((float) value * C_MC_40_RANGE));
        preserve_parm(FX_Compressor, Parm_Ratio);
        break;

    case 145:
        efx_Compressor->changepar(Parm_Knee, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Compressor, Parm_Knee);
        break;

    case 146:
        efx_Compressor->changepar(Parm_Threshold, -60 + (int) ((float) value * C_MC_57_RANGE));
        preserve_parm(FX_Compressor, Parm_Threshold);
        break;

    case 147:
        efx_Compressor->changepar(Parm_Output, -40 + (int) ((float) value * C_MC_40_RANGE));
        preserve_parm(FX_Compressor, Parm_Output);
        break;

    case 148:
        efx_EQ2->changepar(Parm_Volume, value);
        preserve_parm(FX_EQ2, Parm_Volume);
        break;

    case 149:
        efx_EQ2->changepar(11, 20 + (int) ((float) value * C_MC_980_RANGE));
        preserve_parm(FX_EQ2, 11);
        break;

    case 150:
        efx_EQ2->changepar(12, value);
        preserve_parm(FX_EQ2, 12);
        break;

    case 151:
        efx_EQ2->changepar(13, value);
        preserve_parm(FX_EQ2, 13);
        break;

    case 152:
        efx_EQ2->changepar(16, 800 + (int) ((float) value * C_MC_7200_RANGE));
        preserve_parm(FX_EQ2, 16);
        break;

    case 153:
        efx_EQ2->changepar(17, value);
        preserve_parm(FX_EQ2, 17);
        break;

    case 154:
        efx_EQ2->changepar(18, value);
        preserve_parm(FX_EQ2, 18);
        break;

    case 155:
        efx_EQ2->changepar(21, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        preserve_parm(FX_EQ2, 21);
        break;

    case 156:
        efx_EQ2->changepar(22, value);
        preserve_parm(FX_EQ2, 22);
        break;

    case 157:
        efx_EQ2->changepar(23, value);
        preserve_parm(FX_EQ2, 23);
        break;

    case 158:
        efx_DFlange->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_DFlange, Parm_DryWet);
        break;

    case 159:
        efx_DFlange->changepar(Parm_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_DFlange, Parm_Pan);
        break;

    case 160:
        efx_DFlange->changepar(Parm_Dist_LRCross, value);
        preserve_parm(FX_DFlange, Parm_Dist_LRCross);
        break;

    case 161:
        efx_DFlange->changepar(Parm_DFlange_Depth, 20 + (int) ((float) value * C_MC_2480_RANGE));
        preserve_parm(FX_DFlange, Parm_DFlange_Depth);
        break;

    case 162:
        efx_DFlange->changepar(Parm_DFlange_Width, (int) ((float) value * C_MC_6000_RANGE));
        preserve_parm(FX_DFlange, Parm_DFlange_Width);
        break;

    case 163:
        efx_DFlange->changepar(Parm_DFlange_Offset, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_DFlange, Parm_DFlange_Offset);
        break;

    case 164:
        efx_DFlange->changepar(Parm_DFlange_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        preserve_parm(FX_DFlange, Parm_DFlange_Feedback);
        break;

    case 165:
        /* This is labeled LPF but uses same range as HPF - FIXME check DSP */
        efx_DFlange->changepar(Parm_DFlange_HiDamp, 20 + (int) ((float) value * C_MC_19980_RANGE));
        preserve_parm(FX_DFlange, Parm_DFlange_HiDamp);
        break;

    case 166:
        efx_DFlange->changepar(Parm_DFlange_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_DFlange, Parm_DFlange_LFOFreq);
        break;

    case 167:
        efx_DFlange->changepar(Parm_DFlange_LFOStereo, value);
        preserve_parm(FX_DFlange, Parm_DFlange_LFOStereo);
        break;

    case 168:
        efx_DFlange->changepar(Parm_DFlange_LFORandomness, value);
        preserve_parm(FX_DFlange, Parm_DFlange_LFORandomness);
        break;

    case 169:
        efx_Valve->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Valve, Parm_DryWet);
        break;

    case 170:
        efx_Valve->changepar(Parm_Dist_LRCross, value);
        preserve_parm(FX_Valve, Parm_Dist_LRCross);
        break;

    case 171:
        efx_Valve->changepar(Parm_Pan, value);
        preserve_parm(FX_Valve, Parm_Pan);
        break;

    case 172:
        efx_Valve->changepar(Parm_Level, value);
        preserve_parm(FX_Valve, Parm_Level);
        break;

    case 173:
        efx_Valve->changepar(Parm_Drive, value);
        preserve_parm(FX_Valve, Parm_Drive);
        break;

    case 174:
        efx_Valve->changepar(Parm_Q, value);
        preserve_parm(FX_Valve, Parm_Q);
        break;

    case 175:
        efx_Valve->changepar(Parm_Presence, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Valve, Parm_Presence);
        break;

    case 176:
        efx_Valve->changepar(Parm_Valve_LowPass, ret_LPF(value));
        preserve_parm(FX_Valve, Parm_Valve_LowPass);
        break;

    case 177:
        efx_Valve->changepar(Parm_Valve_HighPass, ret_HPF(value));
        preserve_parm(FX_Valve, Parm_Valve_HighPass);
        break;

    case 178:
        efx_Ring->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Ring, Parm_DryWet);
        break;

    case 179:
        efx_Ring->changepar(Parm_Dist_LRCross, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Ring, Parm_Dist_LRCross);
        break;

    case 180:
        efx_Ring->changepar(Parm_Input, 1 + (int) ((float) value * C_MC_126_RANGE));
        preserve_parm(FX_Ring, Parm_Input);
        break;

    case 181:
        efx_Ring->changepar(Parm_Ring_Level, value);
        preserve_parm(FX_Ring, Parm_Ring_Level);
        break;

    case 182:
        efx_Ring->changepar(Parm_Pan, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Ring, Parm_Pan);
        break;

    case 183:
        efx_Ring->changepar(Parm_Ring_Depth, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Ring, Parm_Ring_Depth);
        break;

    case 184:
        efx_Ring->changepar(Parm_Ring_Freq, 1 + (int) ((float) value * C_MC_19999_RANGE));
        preserve_parm(FX_Ring, Parm_Ring_Freq);
        break;

    case 185:
        efx_Ring->changepar(Parm_Ring_Sine, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Ring, Parm_Ring_Sine);
        break;

    case 186:
        efx_Ring->changepar(Parm_Ring_Tri, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Ring, Parm_Ring_Tri);
        break;

    case 187:
        efx_Ring->changepar(Parm_Ring_Saw, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Ring, Parm_Ring_Saw);
        break;

    case 188:
        efx_Ring->changepar(Parm_Ring_Square, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Ring, Parm_Ring_Square);
        break;

    case 189:
        efx_Exciter->changepar(Parm_Volume, value);
        preserve_parm(FX_Exciter, Parm_Volume);
        break;

    case 190:
        efx_Exciter->changepar(Parm_Exciter_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Exciter, Parm_Exciter_LowPassFilter);
        break;

    case 191:
        efx_Exciter->changepar(Parm_Exciter_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Exciter, Parm_Exciter_HighPassFilter);
        break;

    case 192:
        efx_Exciter->changepar(Parm_Exciter_H1, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H1);
        break;

    case 193:
        efx_Exciter->changepar(Parm_Exciter_H2, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H2);
        break;

    case 194:
        efx_Exciter->changepar(Parm_Exciter_H3, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H3);
        break;

    case 195:
        efx_Exciter->changepar(Parm_Exciter_H4, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H4);
        break;

    case 196:
        efx_Exciter->changepar(Parm_Exciter_H5, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H5);
        break;

    case 197:
        efx_Exciter->changepar(Parm_Exciter_H6, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H6);
        break;

    case 198:
        efx_Exciter->changepar(Parm_Exciter_H7, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H7);
        break;

    case 199:
        efx_Exciter->changepar(Parm_Exciter_H8, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H8);
        break;

    case 200:
        efx_Exciter->changepar(Parm_Exciter_H9, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H9);
        break;

    case 201:
        efx_Exciter->changepar(Parm_Exciter_H10, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Exciter, Parm_Exciter_H10);
        break;

    case 202:
        efx_DistBand->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_DistBand, Parm_DryWet);
        break;

    case 203:
        efx_DistBand->changepar(Parm_Dist_LRCross, value);
        preserve_parm(FX_DistBand, Parm_Dist_LRCross);
        break;

    case 204:
        efx_DistBand->changepar(Parm_Drive, value);
        preserve_parm(FX_DistBand, Parm_Drive);
        break;

    case 205:
        efx_DistBand->changepar(Parm_Level, value);
        preserve_parm(FX_DistBand, Parm_Level);
        break;

    case 206:
        efx_DistBand->changepar(Parm_DistBand_LowVol, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_LowVol);
        break;

    case 207:
        efx_DistBand->changepar(Parm_DistBand_MidVol, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_MidVol);
        break;

    case 208:
        efx_DistBand->changepar(Parm_DistBand_HighVol, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_HighVol);
        break;

    case 209:
        efx_DistBand->changepar(Parm_DistBand_Cross1, 20 + (int) ((float) value * C_MC_980_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_Cross1);
        break;

    case 210:
        efx_DistBand->changepar(Parm_DistBand_Cross2, 800 + (int) ((float) value * C_MC_11200_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_Cross2);
        break;

    case 211:
        efx_DistBand->changepar(Parm_Pan, value);
        preserve_parm(FX_DistBand, Parm_Pan);
        break;

    case 212:
        efx_Arpie->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Arpie, Parm_DryWet);
        break;

    case 213:
        efx_Arpie->changepar(Parm_Arpie_Reverse, value);
        preserve_parm(FX_Arpie, Parm_Arpie_Reverse);
        break;

    case 214:
        efx_Arpie->changepar(Parm_Pan, value - 64);
        preserve_parm(FX_Arpie, Parm_Pan);
        break;

    case 215:
        efx_Arpie->changepar(Parm_Arpie_Tempo, ret_Tempo(value));
        preserve_parm(FX_Arpie, Parm_Arpie_Tempo);
        break;

    case 216:
        efx_Arpie->changepar(Parm_Arpie_LRDelay, value);
        preserve_parm(FX_Arpie, Parm_Arpie_LRDelay);
        break;

    case 217:
        efx_Arpie->changepar(Parm_Echo_LRCross, value);
        preserve_parm(FX_Arpie, Parm_Echo_LRCross);
        break;

    case 218:
        efx_Arpie->changepar(Parm_Feedback, value);
        preserve_parm(FX_Arpie, Parm_Feedback);
        break;

    case 219:
        efx_Arpie->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_Arpie, Parm_Arpie_HiDamp);
        break;

    case 220:
        efx_Expander->changepar(Parm_Expander_Attack, 1 + (int) ((float) value * C_MC_4999_RANGE));
        preserve_parm(FX_Expander, Parm_Expander_Attack);
        break;

    case 221:
        efx_Expander->changepar(Parm_Expander_Decay, 10 + (int) ((float) value * C_MC_990_RANGE));
        preserve_parm(FX_Expander, Parm_Expander_Decay);
        break;

    case 222:
        efx_Expander->changepar(Parm_Expander_Shape, 1 + (int) ((float) value * C_MC_49_RANGE));
        preserve_parm(FX_Expander, Parm_Expander_Shape);
        break;

    case 223:
        efx_Expander->changepar(Parm_Expander_Threshold, (int) ((float) value * -C_MC_80_RANGE));
        preserve_parm(FX_Expander, Parm_Expander_Threshold);
        break;

    case 224:
        efx_Expander->changepar(Parm_Expander_OutGain, 1 + (int) ((float) value * C_MC_126_RANGE));
        preserve_parm(FX_Expander, Parm_Expander_OutGain);
        break;

    case 225:
        efx_Expander->changepar(Parm_Expander_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Expander, Parm_Expander_LowPassFilter);
        break;

    case 226:
        efx_Expander->changepar(Parm_Expander_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Expander, Parm_Expander_HighPassFilter);
        break;

    case 227:
        efx_Shuffle->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Shuffle, Parm_DryWet);
        break;

    case 228:
        efx_Shuffle->changepar(Parm_Shuffle_Cross1, 20 + (int) ((float) value * C_MC_980_RANGE));
        preserve_parm(FX_Shuffle, Parm_Shuffle_Cross1);
        break;

    case 229:
        efx_Shuffle->changepar(Parm_Shuffle_GainLow, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Shuffle, Parm_Shuffle_GainLow);
        break;

    case 230:
        efx_Shuffle->changepar(Parm_Shuffle_Cross2, 400 + (int) ((float) value * C_MC_3600_RANGE));
        preserve_parm(FX_Shuffle, Parm_Shuffle_Cross2);
        break;

    case 231:
        efx_Shuffle->changepar(Parm_Shuffle_GainMidLow, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Shuffle, Parm_Shuffle_GainMidLow);
        break;

    case 232:
        efx_Shuffle->changepar(Parm_Shuffle_Cross3, 1200 + (int) ((float) value * C_MC_6800_RANGE));
        preserve_parm(FX_Shuffle, Parm_Shuffle_Cross3);
        break;

    case 233:
        efx_Shuffle->changepar(Parm_Shuffle_GainMidHigh, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Shuffle, Parm_Shuffle_GainMidHigh);
        break;

    case 234:
        efx_Shuffle->changepar(Parm_Shuffle_Cross4, 6000 + (int) ((float) value * C_MC_20000_RANGE));
        preserve_parm(FX_Shuffle, Parm_Shuffle_Cross4);
        break;

    case 235:
        efx_Shuffle->changepar(Parm_Shuffle_GainHigh, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Shuffle, Parm_Shuffle_GainHigh);
        break;

    case 236:
        efx_Shuffle->changepar(Parm_Shuffle_Q, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Shuffle, Parm_Shuffle_Q);
        break;

    case 237:
        efx_Synthfilter->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Synthfilter, Parm_DryWet);
        break;

    case 238:
        efx_Synthfilter->changepar(Parm_Distortion, value);
        preserve_parm(FX_Synthfilter, Parm_Distortion);
        break;

    case 239:
        efx_Synthfilter->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Synthfilter, Parm_LFOFreq);
        break;

    case 240:
        efx_Synthfilter->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_Synthfilter, Parm_LFOStereo);
        break;

    case 241:
        efx_Synthfilter->changepar(Parm_Width, value);
        preserve_parm(FX_Synthfilter, Parm_Width);
        break;

    case 242:
        efx_Synthfilter->changepar(Parm_Phaser_Feedback, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Synthfilter, Parm_Phaser_Feedback);
        break;

    case 243:
        efx_Synthfilter->changepar(Parm_APhaser_Depth, value);
        preserve_parm(FX_Synthfilter, Parm_APhaser_Depth);
        break;

    case 244:
        efx_Synthfilter->changepar(Parm_Synthfilter_Envelope, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_Synthfilter, Parm_Synthfilter_Envelope);
        break;

    case 245:
        efx_Synthfilter->changepar(Parm_Synthfilter_Attack, 5 + (int) ((float) value * C_MC_995_RANGE));
        preserve_parm(FX_Synthfilter, Parm_Synthfilter_Attack);
        break;

    case 246:
        efx_Synthfilter->changepar(Parm_Synthfilter_Release, 5 + (int) ((float) value * C_MC_495_RANGE));
        preserve_parm(FX_Synthfilter, Parm_Synthfilter_Release);
        break;

    case 247:
        efx_Synthfilter->changepar(Parm_Synthfilter_Bandwidth, value);
        preserve_parm(FX_Synthfilter, Parm_Synthfilter_Bandwidth);
        break;

    case 248:
        efx_VaryBand->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_VaryBand, Parm_DryWet);
        break;

    case 249:
        efx_VaryBand->changepar(Parm_VaryBand_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_VaryBand, Parm_VaryBand_LFOFreq);
        break;

    case 250:
        efx_VaryBand->changepar(Parm_VaryBand_LFOStereo, value);
        preserve_parm(FX_VaryBand, Parm_VaryBand_LFOStereo);
        break;

    case 251:
        efx_VaryBand->changepar(Parm_VaryBand_LFO2Freq, ret_Tempo(value));
        preserve_parm(FX_VaryBand, Parm_VaryBand_LFO2Freq);
        break;

    case 252:
        efx_VaryBand->changepar(Parm_VaryBand_LFO2Stereo, value);
        preserve_parm(FX_VaryBand, Parm_VaryBand_LFO2Stereo);
        break;

    case 253:
        efx_VaryBand->changepar(Parm_VaryBand_Cross1, 20 + (int) ((float) value * C_MC_980_RANGE));
        preserve_parm(FX_VaryBand, Parm_VaryBand_Cross1);
        break;

    case 254:
        efx_VaryBand->changepar(Parm_VaryBand_Cross2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        preserve_parm(FX_VaryBand, Parm_VaryBand_Cross2);
        break;

    case 255:
        efx_VaryBand->changepar(Parm_VaryBand_Cross3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        preserve_parm(FX_VaryBand, Parm_VaryBand_Cross3);
        break;

    case 256:
        efx_MuTroMojo->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_MuTroMojo, Parm_DryWet);
        break;

    case 257:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_LowPassLvl, value - 64);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_LowPassLvl);
        break;

    case 258:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_BandPassLvl, value - 64);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_BandPassLvl);
        break;

    case 259:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_HighPassLvl, value - 64);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_HighPassLvl);
        break;

    case 260:
        efx_MuTroMojo->changepar(Parm_Depth, value);
        preserve_parm(FX_MuTroMojo, Parm_Depth);
        break;

    case 261:
        efx_MuTroMojo->changepar(Parm_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_MuTroMojo, Parm_LFOFreq);
        break;

    case 262:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_Q, value);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_Q);
        break;

    case 263:
        efx_MuTroMojo->changepar(Parm_Range, 10 + (int) ((float) value * C_MC_5990_RANGE));
        preserve_parm(FX_MuTroMojo, Parm_Range);
        break;

    case 264:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_AmpSnsInv, value);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_AmpSnsInv);
        break;

    case 265:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_AmpSns, value - 64);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_AmpSns);
        break;

    case 266:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_AmpSmooth, value);
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_AmpSmooth);
        break;

    case 267:
        efx_Looper->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Looper, Parm_DryWet);
        break;

    case 268:
        efx_Looper->changepar(Parm_Looper_Fade1, value);
        preserve_parm(FX_Looper, Parm_Looper_Fade1);
        break;

    case 269:
        efx_Looper->changepar(Parm_Looper_Fade2, value);
        preserve_parm(FX_Looper, Parm_Looper_Fade2);
        break;

    case 270:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Reverse, i);
        preserve_parm(FX_Looper, Parm_Looper_Reverse);
        break;
    }
    case 271:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Autoplay, i);
        preserve_parm(FX_Looper, Parm_Looper_Autoplay);
        break;
    }
    case 272:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Play, i);
        preserve_parm(FX_Looper, Parm_Looper_Play);
        break;
    }
    case 273:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Stop, i);
        preserve_parm(FX_Looper, Parm_Looper_Stop);
        break;
    }
    case 274:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Record, i);
        preserve_parm(FX_Looper, Parm_Looper_Record);
        break;
    }
    case 275:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Record1, i);
        preserve_parm(FX_Looper, Parm_Looper_Record1);
        break;
    }
    case 276:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Record2, i);
        preserve_parm(FX_Looper, Parm_Looper_Record2);
        break;
    }
    case 277:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Track1, i);
        preserve_parm(FX_Looper, Parm_Looper_Track1);
        break;
    }
    case 278:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Track2, i);
        preserve_parm(FX_Looper, Parm_Looper_Track2);
        break;
    }
    case 279:
    {
        int i = 0;
        if (value) i = 1;
        efx_Looper->changepar(Parm_Looper_Clear, i);
        preserve_parm(FX_Looper, Parm_Looper_Clear);
        break;
    }
    case 280:
        efx_Convol->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Convolotron, Parm_DryWet);
        break;

    case 281:
        efx_Convol->changepar(Parm_Pan, value);
        preserve_parm(FX_Convolotron, Parm_Pan);
        break;

    case 282:
        efx_Convol->changepar(Parm_Convolotron_Level, value);
        preserve_parm(FX_Convolotron, Parm_Convolotron_Level);
        break;

    case 283:
        efx_Convol->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_Convolotron, Parm_Arpie_HiDamp);
        break;

    case 284:
        efx_Convol->changepar(Parm_Convolotron_Feedback, ((int) (float) value * C_MC_128_RANGE) - 64);
        preserve_parm(FX_Convolotron, Parm_Convolotron_Feedback);
        break;

    case 285:
        efx_Convol->changepar(Parm_Convolotron_Length, 5 + (int) ((float) value * C_MC_245_RANGE));
        preserve_parm(FX_Convolotron, Parm_Convolotron_Length);
        break;

    case 286:
        efx_CoilCrafter->changepar(Parm_Volume, value);
        preserve_parm(FX_CoilCrafter, Parm_Volume);
        break;

    case 287:
        efx_CoilCrafter->changepar(Parm_CoilCrafter_HighPassFilter, 20 + (int) ((float) value * C_MC_4380_RANGE));
        preserve_parm(FX_CoilCrafter, Parm_CoilCrafter_HighPassFilter);
        break;

    case 288:
        efx_CoilCrafter->changepar(Parm_CoilCrafter_Freq1, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        preserve_parm(FX_CoilCrafter, Parm_CoilCrafter_Freq1);
        break;

    case 289:
        efx_CoilCrafter->changepar(Parm_CoilCrafter_Q1, 10 + (int) ((float) value * C_MC_55_RANGE));
        preserve_parm(FX_CoilCrafter, Parm_CoilCrafter_Q1);
        break;

    case 290:
        efx_CoilCrafter->changepar(Parm_CoilCrafter_Freq2, 2600 + (int) ((float) value * C_MC_1900_RANGE));
        preserve_parm(FX_CoilCrafter, Parm_CoilCrafter_Freq2);
        break;

    case 291:
        efx_CoilCrafter->changepar(Parm_CoilCrafter_Q2, 10 + (int) ((float) value * C_MC_55_RANGE));
        preserve_parm(FX_CoilCrafter, Parm_CoilCrafter_Q2);
        break;

    case 292:
        efx_ShelfBoost->changepar(Parm_Volume, value);
        preserve_parm(FX_ShelfBoost, Parm_Volume);
        break;

    case 293:
        efx_ShelfBoost->changepar(Parm_ShelfBoost_Gain, 1 + (int) ((float) value * C_MC_126_RANGE));
        preserve_parm(FX_ShelfBoost, Parm_ShelfBoost_Gain);
        break;

    case 294:
        efx_ShelfBoost->changepar(Parm_ShelfBoost_Freq, 220 + (int) ((float) value * C_MC_15780_RANGE));
        preserve_parm(FX_ShelfBoost, Parm_ShelfBoost_Freq);
        break;

    case 295:
        efx_ShelfBoost->changepar(Parm_MuTroMojo_Q, ((int) (float) value * C_MC_128_RANGE)  - 64);
        preserve_parm(FX_ShelfBoost, Parm_MuTroMojo_Q);
        break;

    case 296:
        efx_Vocoder->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Vocoder, Parm_DryWet);
        break;

    case 297:
        efx_Vocoder->changepar(Parm_Pan, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Vocoder, Parm_Pan);
        break;

    case 298:
        efx_Vocoder->changepar(Parm_Vocoder_Input, value);
        preserve_parm(FX_Vocoder, Parm_Vocoder_Input);
        break;

    case 299:
        efx_Vocoder->changepar(Parm_Vocoder_Muffle, 1 + (int) ((float) value * C_MC_126_RANGE));
        preserve_parm(FX_Vocoder, Parm_Vocoder_Muffle);
        break;

    case 300:
        efx_Vocoder->changepar(Parm_Vocoder_Q, 40 + (int) ((float) value * C_MC_130_RANGE));
        preserve_parm(FX_Vocoder, Parm_Vocoder_Q);
        break;

    case 301:
        efx_Vocoder->changepar(Parm_Vocoder_Ring, value);
        preserve_parm(FX_Vocoder, Parm_Vocoder_Ring);
        break;

    case 302:
        efx_Vocoder->changepar(Parm_Vocoder_Level, value);
        preserve_parm(FX_Vocoder, Parm_Vocoder_Level);
        break;

    case 303:
        efx_Echoverse->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Echoverse, Parm_DryWet);
        break;

    case 304:
        efx_Echoverse->changepar(Parm_Arpie_Reverse, value);
        preserve_parm(FX_Echoverse, Parm_Arpie_Reverse);
        break;

    case 305:
        efx_Echoverse->changepar(Parm_Pan, value);
        preserve_parm(FX_Echoverse, Parm_Pan);
        break;

    case 306:
        efx_Echoverse->changepar(Parm_Echoverse_Delay, ret_Tempo(value));
        preserve_parm(FX_Echoverse, Parm_Echoverse_Delay);
        break;

    case 307:
        efx_Echoverse->changepar(Parm_Arpie_LRDelay, value);
        preserve_parm(FX_Echoverse, Parm_Arpie_LRDelay);
        break;

    case 308:
        efx_Echoverse->changepar(Parm_Feedback, value);
        preserve_parm(FX_Echoverse, Parm_Feedback);
        break;

    case 309:
        efx_Echoverse->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_Echoverse, Parm_Arpie_HiDamp);
        break;

    case 310:
        efx_Echoverse->changepar(Parm_Echoverse_ExStereo, value);
        preserve_parm(FX_Echoverse, Parm_Echoverse_ExStereo);
        break;

    case 311:
        efx_Echoverse->changepar(Parm_Echo_LRCross, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Echoverse, Parm_Echo_LRCross);
        break;

    case 312:
        efx_Sustainer->changepar(Parm_Volume, value);
        preserve_parm(FX_Sustainer, Parm_Volume);
        break;

    case 313:
        efx_Sustainer->changepar(Parm_Sustain, value);
        preserve_parm(FX_Sustainer, Parm_Sustain);
        break;

    case 314:
        efx_Sequence->changepar(Parm_Sequence_DryWet, Dry_Wet(value));
        preserve_parm(FX_Sequence, Parm_Sequence_DryWet);
        break;

    case 315:
        efx_Sequence->changepar(Parm_Sequence_1, value);
        preserve_parm(FX_Sequence, Parm_Sequence_1);
        break;

    case 316:
        efx_Sequence->changepar(Parm_Sequence_2, value);
        preserve_parm(FX_Sequence, Parm_Sequence_2);
        break;

    case 317:
        efx_Sequence->changepar(Parm_Sequence_3, value);
        preserve_parm(FX_Sequence, Parm_Sequence_3);
        break;

    case 318:
        efx_Sequence->changepar(Parm_Sequence_4, value);
        preserve_parm(FX_Sequence, Parm_Sequence_4);
        break;

    case 319:
        efx_Sequence->changepar(Parm_Sequence_5, value);
        preserve_parm(FX_Sequence, Parm_Sequence_5);
        break;

    case 320:
        efx_Sequence->changepar(Parm_Sequence_6, value);
        preserve_parm(FX_Sequence, Parm_Sequence_6);
        break;

    case 321:
        efx_Sequence->changepar(Parm_Sequence_7, value);
        preserve_parm(FX_Sequence, Parm_Sequence_7);
        break;

    case 322:
        efx_Sequence->changepar(Parm_Sequence_8, value);
        preserve_parm(FX_Sequence, Parm_Sequence_8);
        break;

    case 323:
        efx_Sequence->changepar(Parm_Sequence_Tempo, ret_Tempo(value));
        preserve_parm(FX_Sequence, Parm_Sequence_Tempo);
        break;

    case 324:
        efx_Sequence->changepar(Parm_Sequence_Q, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Sequence, Parm_Sequence_Q);
        break;

    case 325:
        efx_Sequence->changepar(Parm_Sequence_StereoDiff, (int) ((float) value * C_MC_7_RANGE));
        preserve_parm(FX_Sequence, Parm_Sequence_StereoDiff);
        break;

    case 326:
        efx_Shifter->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Shifter, Parm_DryWet);
        break;

    case 327:
        efx_Shifter->changepar(Parm_Shifter_Interval, (int) ((float) value * C_MC_12_RANGE));
        preserve_parm(FX_Shifter, Parm_Shifter_Interval);
        break;

    case 328:
        efx_Shifter->changepar(Parm_InputGain, value);
        preserve_parm(FX_Shifter, Parm_InputGain);
        break;

    case 329:
        efx_Shifter->changepar(Parm_Pan, value);
        preserve_parm(FX_Shifter, Parm_Pan);
        break;

    case 330:
        efx_Shifter->changepar(Parm_Expander_Attack, 1 + (int) ((float) value * C_MC_1999_RANGE));
        preserve_parm(FX_Shifter, Parm_Expander_Attack);
        break;

    case 331:
        efx_Shifter->changepar(Parm_Expander_Decay, 1 + (int) ((float) value * C_MC_1999_RANGE));
        preserve_parm(FX_Shifter, Parm_Expander_Decay);
        break;

    case 332:
        efx_Shifter->changepar(Parm_Shifter_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        preserve_parm(FX_Shifter, Parm_Shifter_Threshold);
        break;

    case 333:
        efx_Shifter->changepar(Parm_Shifter_Whammy, value);
        preserve_parm(FX_Shifter, Parm_Shifter_Whammy);
        break;

    case 334:
        efx_StompBox->changepar(Parm_Volume, value);
        preserve_parm(FX_StompBox, Parm_Volume);
        break;

    case 335:
        efx_StompBox->changepar(Parm_ShelfBoost_Gain, value);
        preserve_parm(FX_StompBox, Parm_ShelfBoost_Gain);
        break;

    case 336:
        efx_StompBox->changepar(Parm_StompBox_Low, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_StompBox, Parm_StompBox_Low);
        break;

    case 337:
        efx_StompBox->changepar(Parm_StompBox_Mid, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_StompBox, Parm_StompBox_Mid);
        break;

    case 338:
        efx_StompBox->changepar(Parm_StompBox_High, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_StompBox, Parm_StompBox_High);
        break;

    case 339:
        efx_Reverbtron->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Reverbtron, Parm_DryWet);
        break;

    case 340:
        efx_Reverbtron->changepar(Parm_Reverbtron_Pan, value);
        preserve_parm(FX_Reverbtron, Parm_Reverbtron_Pan);
        break;

    case 341:
        efx_Reverbtron->changepar(Parm_Convolotron_Level, value);
        preserve_parm(FX_Reverbtron, Parm_Convolotron_Level);
        break;

    case 342:
        efx_Reverbtron->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_Reverbtron, Parm_Arpie_HiDamp);
        break;

    case 343:
        efx_Reverbtron->changepar(Parm_Convolotron_Feedback, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Reverbtron, Parm_Convolotron_Feedback);
        break;

    case 344:
        efx_Reverbtron->changepar(Parm_Convolotron_Length, 20 + (int) ((float) value * C_MC_1480_RANGE));
        preserve_parm(FX_Reverbtron, Parm_Convolotron_Length);
        break;

    case 345:
        efx_Reverbtron->changepar(Parm_Reverbtron_Stretch, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Reverbtron, Parm_Reverbtron_Stretch);
        break;

    case 346:
        efx_Reverbtron->changepar(Parm_Reverbtron_InitialDelay, (int) ((float) value * C_MC_500_RANGE));
        preserve_parm(FX_Reverbtron, Parm_Reverbtron_InitialDelay);
        break;

    case 347:
        efx_Reverbtron->changepar(Parm_Reverbtron_Fade, value);
        preserve_parm(FX_Reverbtron, Parm_Reverbtron_Fade);
        break;

    case 348:
        efx_Echotron->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Echotron, Parm_DryWet);
        break;

    case 349:
        efx_Echotron->changepar(Parm_Reverbtron_Pan, value);
        preserve_parm(FX_Echotron, Parm_Reverbtron_Pan);
        break;

    case 350:
        efx_Echotron->changepar(Parm_Echotron_Tempo, ret_Tempo(value));
        preserve_parm(FX_Echotron, Parm_Echotron_Tempo);
        break;

    case 351:
        efx_Echotron->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_Echotron, Parm_Arpie_HiDamp);
        break;

    case 352:
        efx_Echotron->changepar(Parm_Convolotron_Feedback, value);
        preserve_parm(FX_Echotron, Parm_Convolotron_Feedback);
        break;

    case 353:
        efx_Echotron->changepar(Parm_Echotron_LRCross, value);
        preserve_parm(FX_Echotron, Parm_Echotron_LRCross);
        break;

    case 354:
        efx_Echotron->changepar(Parm_Echotron_Width, value);
        preserve_parm(FX_Echotron, Parm_Echotron_Width);
        break;

    case 355:
        efx_Echotron->changepar(Parm_Echotron_Depth, value);
        preserve_parm(FX_Echotron, Parm_Echotron_Depth);
        break;

    case 356:
        efx_Echotron->changepar(Parm_Echotron_LFOStereo, value);
        preserve_parm(FX_Echotron, Parm_Echotron_LFOStereo);
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
        efx_Echotron->changepar(Parm_Convolotron_Length, number_taps);
        preserve_parm(FX_Echotron, Parm_Convolotron_Length);
        break;
    }
    case 358:
        efx_StereoHarm->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_StereoHarm, Parm_DryWet);
        break;

    case 359:
        efx_StereoHarm->changepar(Parm_StereoHarm_IntervalLeft, -12 + (int) ((float) value * C_MC_24_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_IntervalLeft);
        break;

    case 360:
        efx_StereoHarm->changepar(Parm_StereoHarm_ChromeLeft, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_ChromeLeft);
        break;

    case 361:
        efx_StereoHarm->changepar(Parm_StereoHarm_GainLeft, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_GainLeft);
        break;

    case 362:
        efx_StereoHarm->changepar(Parm_StereoHarm_IntervalRight, -12 + (int) ((float) value * C_MC_24_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_IntervalRight);
        break;

    case 363:
        efx_StereoHarm->changepar(Parm_StereoHarm_ChromeRight, -2000 + (int) ((float) value * C_MC_4000_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_ChromeRight);
        break;

    case 364:
        efx_StereoHarm->changepar(Parm_StereoHarm_GainRight, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_GainRight);
        break;

    case 365:
        efx_StereoHarm->changepar(Parm_StereoHarm_LRCross, value);
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_LRCross);
        break;

    case 366:
        efx_StereoHarm->changepar(Parm_StereoHarm_Note, (int) ((float) value * C_MC_23_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_Note);
        break;

    case 367:
        efx_StereoHarm->changepar(Parm_StereoHarm_Type, (int) ((float) value * C_MC_33_RANGE));
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_Type);
        break;

    case 368:
        efx_CompBand->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_CompBand, Parm_DryWet);
        break;

    case 369:
        efx_CompBand->changepar(12, value);
        break;

    case 370:
        efx_CompBand->changepar(Parm_CompBand_LowRatio, 2 + (int) ((float) value * C_MC_40_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_LowRatio);
        break;

    case 371:
        efx_CompBand->changepar(Parm_CompBand_MidLowRatio, 2 + (int) ((float) value * C_MC_40_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_MidLowRatio);
        break;

    case 372:
        efx_CompBand->changepar(Parm_CompBand_MidHighRatio, 2 + (int) ((float) value * C_MC_40_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_MidHighRatio);
        break;

    case 373:
        efx_CompBand->changepar(Parm_CompBand_HighRatio, 2 + (int) ((float) value * C_MC_40_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_HighRatio);
        break;

    case 374:
        efx_CompBand->changepar(Parm_CompBand_LowThreshold, -70 + (int) ((float) value * C_MC_94_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_LowThreshold);
        break;

    case 375:
        efx_CompBand->changepar(Parm_CompBand_MidLowThreshold, -70 + (int) ((float) value * C_MC_94_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_MidLowThreshold);
        break;

    case 376:
        efx_CompBand->changepar(Parm_CompBand_MidHighThreshold, -70 + (int) ((float) value * C_MC_94_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_MidHighThreshold);
        break;

    case 377:
        efx_CompBand->changepar(Parm_CompBand_HighThreshold, -70 + (int) ((float) value * C_MC_94_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_HighThreshold);
        break;

    case 378:
        efx_CompBand->changepar(Parm_CompBand_Cross1, 20 + (int) ((float) value * C_MC_980_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_Cross1);
        break;

    case 379:
        efx_CompBand->changepar(Parm_CompBand_Cross2, 1000 + (int) ((float) value * C_MC_7000_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_Cross2);
        break;

    case 380:
        efx_CompBand->changepar(Parm_CompBand_Cross3, 2000 + (int) ((float) value * C_MC_24000_RANGE));
        preserve_parm(FX_CompBand, Parm_CompBand_Cross3);
        break;

    case 381:
        efx_Opticaltrem->changepar(Parm_OpticalTrem_Depth, value);
        preserve_parm(FX_OpticalTrem, Parm_OpticalTrem_Depth);
        break;

    case 382:
        efx_Opticaltrem->changepar(Parm_VaryBand_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_OpticalTrem, Parm_VaryBand_LFOFreq);
        break;

    case 383:
        efx_Opticaltrem->changepar(Parm_OpticalTrem_Randomness, value);
        preserve_parm(FX_OpticalTrem, Parm_OpticalTrem_Randomness);
        break;

    case 384:
        efx_Opticaltrem->changepar(Parm_OpticalTrem_LFOStereo, value);
        preserve_parm(FX_OpticalTrem, Parm_OpticalTrem_LFOStereo);
        break;

    case 385:
        efx_Opticaltrem->changepar(Parm_OpticalTrem_Pan, value);
        preserve_parm(FX_OpticalTrem, Parm_OpticalTrem_Pan);
        break;

    case 386:
        efx_Vibe->changepar(Parm_Vibe_DryWet, Dry_Wet(value));
        preserve_parm(FX_Vibe, Parm_Vibe_DryWet);
        break;

    case 387:
        efx_Vibe->changepar(Parm_Vibe_Width, value);
        preserve_parm(FX_Vibe, Parm_Vibe_Width);
        break;

    case 388:
        efx_Vibe->changepar(Parm_Vibe_Depth, value);
        preserve_parm(FX_Vibe, Parm_Vibe_Depth);
        break;

    case 389:
        efx_Vibe->changepar(Parm_VaryBand_LFOFreq, ret_Tempo(value));
        preserve_parm(FX_Vibe, Parm_VaryBand_LFOFreq);
        break;

    case 390:
        efx_Vibe->changepar(Parm_OpticalTrem_Randomness, value);
        preserve_parm(FX_Vibe, Parm_OpticalTrem_Randomness);
        break;

    case 391:
        efx_Vibe->changepar(Parm_OpticalTrem_LFOStereo, value);
        preserve_parm(FX_Vibe, Parm_OpticalTrem_LFOStereo);
        break;

    case 392:
        efx_Vibe->changepar(Parm_Phaser_Feedback, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Vibe, Parm_Phaser_Feedback);
        break;

    case 393:
        efx_Vibe->changepar(Parm_LRCross, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Vibe, Parm_LRCross);
        break;

    case 394:
        efx_Vibe->changepar(Parm_OpticalTrem_Pan, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Vibe, Parm_OpticalTrem_Pan);
        break;

    case 395:
        efx_Infinity->changepar(Parm_DryWet, Dry_Wet(value));
        preserve_parm(FX_Infinity, Parm_DryWet);
        break;

    case 396:
        efx_Infinity->changepar(Parm_Shuffle_Q, -1000 + (int) ((float) value * C_MC_2000_RANGE));
        preserve_parm(FX_Infinity, Parm_Shuffle_Q);
        break;

    case 397:
        efx_Infinity->changepar(Parm_Infinity_Autopan, value);
        preserve_parm(FX_Infinity, Parm_Infinity_Autopan);
        break;

    case 398:
        efx_Infinity->changepar(Parm_Infinity_StereoDiff, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_StereoDiff);
        break;

    case 399:
        efx_Infinity->changepar(Parm_Infinity_StartFreq, value);
        preserve_parm(FX_Infinity, Parm_Infinity_StartFreq);
        break;

    case 400:
        efx_Infinity->changepar(Parm_Infinity_EndFreq, value);
        preserve_parm(FX_Infinity, Parm_Infinity_EndFreq);
        break;

    case 401:
        efx_Infinity->changepar(Parm_Infinity_Rate, ret_Tempo(value));
        preserve_parm(FX_Infinity, Parm_Infinity_Rate);
        break;

    case 402:
        efx_Infinity->changepar(Parm_Infinity_Subdiv, -16 + (int) ((float) value * C_MC_32_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_Subdiv);
        break;

    case 403:
        efx_Alienwah->changepar(Parm_Alien_Delay, (int) ((float) value * C_MC_100_RANGE));
        preserve_parm(FX_Alienwah, Parm_Alien_Delay);
        break;

    case 404:
        efx_APhaser->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_APhaser, Parm_LFORandomness);
        break;

    case 405:
        efx_Cabinet->changepar(Parm_Volume, value);
        preserve_parm(FX_Cabinet, Parm_Volume);
        break;

    case 406:
        efx_Chorus->changepar(Parm_Chorus_Delay, value);
        preserve_parm(FX_Chorus, Parm_Chorus_Delay);
        break;
    
    case 407:
        efx_Echo->changepar(Parm_Arpie_Reverse, value);
        preserve_parm(FX_Echo, Parm_Arpie_Reverse);
        break;

    case 408:
        efx_Echo->changepar(Parm_Echoverse_Delay, 20 + (int) ((float) value * C_MC_1980_RANGE));
        preserve_parm(FX_Echo, Parm_Echoverse_Delay);
        break;

    case 409:
        efx_Echo->changepar(Parm_Arpie_LRDelay, value);
        preserve_parm(FX_Echo, Parm_Arpie_LRDelay);
        break;

    case 410:
        efx_Echo->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_Echo, Parm_Arpie_HiDamp);
        break;

    case 411:
        efx_Flanger->changepar(Parm_Chorus_Delay, value);
        preserve_parm(FX_Flanger, Parm_Chorus_Delay);
        break;
        
    case 412:
        efx_Har->changepar(Parm_InputGain, value);
        preserve_parm(FX_Harmonizer, Parm_InputGain);
        break;

    case 413:
        efx_Har->changepar(Parm_FGain, value);
        preserve_parm(FX_Harmonizer, Parm_FGain);
        break;

    case 414:
        efx_Har->changepar(Parm_Shuffle_Q, value);
        preserve_parm(FX_Harmonizer, Parm_Shuffle_Q);
        break;

    case 415:
        efx_Infinity->changepar(Parm_Infinity_P1, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P1);
        break;

    case 416:
        efx_Infinity->changepar(Parm_Infinity_P2, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P2);
        break;

    case 417:
        efx_Infinity->changepar(Parm_Infinity_P3, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P3);
        break;

    case 418:
        efx_Infinity->changepar(Parm_Infinity_P4, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P4);
        break;

    case 419:
        efx_Infinity->changepar(Parm_Infinity_P5, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P5);
        break;

    case 420:
        efx_Infinity->changepar(Parm_Infinity_P6, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P6);
        break;

    case 421:
        efx_Infinity->changepar(Parm_Infinity_P7, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P7);
        break;

    case 422:
        efx_Infinity->changepar(Parm_Infinity_P8, (int) ((float) value * C_MC_128_RANGE));
        preserve_parm(FX_Infinity, Parm_Infinity_P8);
        break;

    case 423:
        efx_Looper->changepar(Parm_Looper_Tempo, 20 + (int) ((float) value * C_MC_360_RANGE));
        preserve_parm(FX_Looper, Parm_Looper_Tempo);
        break;

    case 424:
        efx_MusDelay->changepar(Parm_MusDelay_Tempo, 10 + (int) ((float) value * C_MC_470_RANGE));
        preserve_parm(FX_MusDelay, Parm_MusDelay_Tempo);
        break;

    case 425:
        efx_MusDelay->changepar(Parm_Arpie_HiDamp, value);
        preserve_parm(FX_MusDelay, Parm_Arpie_HiDamp);
        break;

    case 426:
        efx_MuTroMojo->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_MuTroMojo, Parm_LFORandomness);
        break;

    case 427:
        efx_MuTroMojo->changepar(Parm_LFOStereo, value);
        preserve_parm(FX_MuTroMojo, Parm_LFOStereo);
        break;

    case 428:
        efx_MuTroMojo->changepar(Parm_MuTroMojo_MinFreq, 30 + (int) ((float) value * C_MC_770_RANGE));
        preserve_parm(FX_MuTroMojo, Parm_MuTroMojo_MinFreq);
        break;

    case 429:
        efx_Gate->changepar(Parm_Expander_Attack, 1 + (int) ((float) value * C_MC_249_RANGE));
        preserve_parm(FX_Gate, Parm_Expander_Attack);
        break;

    case 430:
        efx_Gate->changepar(Parm_Expander_Decay, 2 + (int) ((float) value * C_MC_248_RANGE));
        preserve_parm(FX_Gate, Parm_Expander_Decay);
        break;

    case 431:
        efx_Gate->changepar(Parm_Gate_Range, -90 + (int) ((float) value * C_MC_90_RANGE));
        preserve_parm(FX_Gate, Parm_Gate_Range);
        break;

    case 432:
        efx_Gate->changepar(Parm_Threshold, -70 + (int) ((float) value * C_MC_90_RANGE));
        preserve_parm(FX_Gate, Parm_Threshold);
        break;

    case 433:
        efx_Gate->changepar(Parm_Hold, 2 + (int) ((float) value * C_MC_498_RANGE));
        preserve_parm(FX_Gate, Parm_Hold);
        break;

    case 434:
        efx_Gate->changepar(Parm_Expander_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Gate, Parm_Expander_LowPassFilter);
        break;

    case 435:
        efx_Gate->changepar(Parm_Expander_HighPassFilter, ret_HPF(value));
        preserve_parm(FX_Gate, Parm_Expander_HighPassFilter);
        break;

    case 436:
        efx_Pan->changepar(Parm_Extra, value);
        preserve_parm(FX_Pan, Parm_Extra);
        break;

    case 437:
        efx_Rev->changepar(Parm_Reverb_Time, value);
        preserve_parm(FX_Reverb, Parm_Reverb_Time);
        break;

    case 438:
        efx_Rev->changepar(Parm_Reverb_InitialDelay, value);
        preserve_parm(FX_Reverb, Parm_Reverb_InitialDelay);
        break;

    case 439:
        efx_Rev->changepar(Parm_Reverb_DelayFeedback, value);
        preserve_parm(FX_Reverb, Parm_Reverb_DelayFeedback);
        break;

    case 440:
        efx_Rev->changepar(Parm_Reverb_RoomSize, 1 + (int) ((float) value * C_MC_126_RANGE));
        preserve_parm(FX_Reverb, Parm_Reverb_RoomSize);
        break;

    case 441:
        efx_Rev->changepar(Parm_Reverb_LoHiDamp, 64 + (int) ((float) value * C_MC_63_RANGE));
        preserve_parm(FX_Reverb, Parm_Reverb_LoHiDamp);
        break;

    case 442:
        efx_Reverbtron->changepar(Parm_Reverbtron_Diffusion, value);
        preserve_parm(FX_Reverbtron, Parm_Reverbtron_Diffusion);
        break;

    case 443:
        efx_Reverbtron->changepar(Parm_Reverbtron_LowPassFilter, ret_LPF(value));
        preserve_parm(FX_Reverbtron, Parm_Reverbtron_LowPassFilter);
        break;

    case 444:
        efx_Har->changepar(Parm_Note, (int) ((float) value * C_MC_23_RANGE));
        preserve_parm(FX_Harmonizer, Parm_Note);
        break;

    case 445:
        efx_Har->changepar(Parm_Harmonizer_Type, (int) ((float) value * C_MC_33_RANGE));
        preserve_parm(FX_Harmonizer, Parm_Harmonizer_Type);
        break;

    case 446:
        efx_Synthfilter->changepar(Parm_LFORandomness, value);
        preserve_parm(FX_Synthfilter, Parm_LFORandomness);
        break;

    case 447:
        efx_Har->changepar(Parm_Harmonizer_Select, value);
        preserve_parm(FX_Harmonizer, Parm_Harmonizer_Select);
        break;

    case 448:
        efx_StereoHarm->changepar(Parm_StereoHarm_Select, value);
        preserve_parm(FX_StereoHarm, Parm_StereoHarm_Select);
        break;

    case 449:
        efx_Distorsion->changepar(Parm_Distortion_Type, (int) ((float) value * C_MC_30_RANGE));
        preserve_parm(FX_Distortion, Parm_Distortion_Type);
        break;

    case 450:
        efx_Overdrive->changepar(Parm_Distortion_Type, (int) ((float) value * C_MC_30_RANGE));
        preserve_parm(FX_Overdrive, Parm_Distortion_Type);
        break;

    case 451:
        efx_Derelict->changepar(Parm_Distortion_Type, (int) ((float) value * C_MC_30_RANGE));
        preserve_parm(FX_Derelict, Parm_Distortion_Type);
        break;

    case 452:
        efx_DistBand->changepar(Parm_DistBand_TypeLow, (int) ((float) value * C_MC_30_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_TypeLow);
        break;

    case 453:
        efx_DistBand->changepar(Parm_DistBand_TypeMid, (int) ((float) value * C_MC_30_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_TypeMid);
        break;

    case 454:
        efx_DistBand->changepar(Parm_DistBand_TypeHigh, (int) ((float) value * C_MC_30_RANGE));
        preserve_parm(FX_DistBand, Parm_DistBand_TypeHigh);
        break;

    case 455:
        efx_StompBox->changepar(Parm_StompBox_Mode, (int) ((float) value * C_MC_8_RANGE));
        preserve_parm(FX_StompBox, Parm_StompBox_Mode);
        break;

    case 456:
        efx_Alienwah->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Alienwah, Parm_LFOType);
        break;

    case 457:
        efx_APhaser->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_APhaser, Parm_LFOType);
        break;

    case 458:
        efx_Chorus->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Chorus, Parm_LFOType);
        break;

    case 459:
        efx_Flanger->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Flanger, Parm_LFOType);
        break;

    case 460:
        efx_DFlange->changepar(Parm_DFlange_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_DFlange, Parm_DFlange_LFOType);
        break;

    case 461:
        efx_Echotron->changepar(Parm_Echotron_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Echotron, Parm_Echotron_LFOType);
        break;

    case 462:
        efx_MuTroMojo->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_MuTroMojo, Parm_LFOType);
        break;

    case 463:
        efx_Opticaltrem->changepar(Parm_OpticalTrem_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_OpticalTrem, Parm_OpticalTrem_LFOType);
        break;

    case 464:
        efx_Pan->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Pan, Parm_LFOType);
        break;

    case 465:
        efx_Phaser->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Phaser, Parm_LFOType);
        break;

    case 466:
        efx_Synthfilter->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Synthfilter, Parm_LFOType);
        break;

    case 467:
        efx_VaryBand->changepar(Parm_VaryBand_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_VaryBand, Parm_VaryBand_LFOType);
        break;

    case 468:
        efx_VaryBand->changepar(Parm_VaryBand_LFO2Type, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_VaryBand, Parm_VaryBand_LFO2Type);
        break;

    case 469:
        efx_Vibe->changepar(Parm_OpticalTrem_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_Vibe, Parm_OpticalTrem_LFOType);
        break;

    case 470:
        efx_WhaWha->changepar(Parm_LFOType, (int) ((float) value * C_MC_11_RANGE));
        preserve_parm(FX_WahWah, Parm_LFOType);
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

