/*
  rakarrack - a guitar effects software

 process.C  -  mainloop functions
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

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <FL/Fl_Preferences.H>
#include "global.h"
#include "process.h"

int Pexitprogram = 0;
int preset = 0;
int commandline = 0;
int exitwithhelp = 0;
int gui = 0;
int nojack = 0;
int error_num = 0;
int stecla = 0;
int looper_lqua = 0;
int needtoloadstate = 0;
int needtoloadbank = 0;
char *s_uuid;
char *statefile;
char *filetoload = NULL;
char *banktoload;
char *jack_client_name = (char*) "rakarrack-plus";

Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);

RKR::RKR() :
    Fpre(NULL),
    efx_Rev(NULL),
    efx_Chorus(NULL),
    efx_Flanger(NULL),
    efx_Phaser(NULL),
    efx_APhaser(NULL),
    efx_EQ1(NULL),
    efx_EQ2(NULL),
    efx_Echo(NULL),
    efx_Distorsion(NULL),
    efx_Overdrive(NULL),
    efx_Compressor(NULL),
    efx_WhaWha(NULL),
    efx_Alienwah(NULL),
    efx_Cabinet(NULL),
    efx_Pan(NULL),
    efx_Har(NULL),
    efx_MusDelay(NULL),
    efx_Gate(NULL),
    efx_Derelict(NULL),
    efx_Tuner(NULL),
    efx_MIDIConverter(NULL),
    M_Metronome(NULL),
    beat(NULL),
    HarmRecNote(NULL),
    StHarmRecNote(NULL),
    RingRecNote(NULL),
    RC_Harm(NULL),
    RC_Stereo_Harm(NULL),
    efx_FLimiter(NULL),
    efx_Valve(NULL),
    efx_DFlange(NULL),
    efx_Ring(NULL),
    efx_Exciter(NULL),
    efx_DistBand(NULL),
    efx_Arpie(NULL),
    efx_Expander(NULL),
    efx_Synthfilter(NULL),
    efx_Shuffle(NULL),
    efx_VaryBand(NULL),
    efx_Convol(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    A_Resample(NULL),
    DC_Offsetl(NULL),
    DC_Offsetr(NULL),
    efx_Looper(NULL),
    efx_MuTroMojo(NULL),
    efx_Echoverse(NULL),
    efx_CoilCrafter(NULL),
    efx_ShelfBoost(NULL),
    efx_Vocoder(NULL),
    efx_Sustainer(NULL),
    efx_Sequence(NULL),
    efx_Shifter(NULL),
    efx_StompBox(NULL),
    efx_Reverbtron(NULL),
    efx_Echotron(NULL),
    efx_StereoHarm(NULL),
    efx_CompBand(NULL),
    efx_Opticaltrem(NULL),
    efx_Vibe(NULL),
    efx_Infinity(NULL),
    jackclient(NULL),
    options(),
    status(),
    jackcliname(),
    db6booster(),
    jdis(),
    jshut(),
    DC_Offset(),
    Bypass(),
    MIDIConverter_Bypass(),
    Metro_Bypass(),
    Tuner_Bypass(),
    Tap_Bypass(),
    ACI_Bypass(),
    Reverb_Bypass(),
    Chorus_Bypass(),
    Flanger_Bypass(),
    Phaser_Bypass(),
    Overdrive_Bypass(),
    Distorsion_Bypass(),
    Echo_Bypass(),
    EQ1_Bypass(),
    EQ2_Bypass(),
    Compressor_Bypass(),
    WhaWha_Bypass(),
    Alienwah_Bypass(),
    Cabinet_Bypass(),
    Pan_Bypass(),
    Harmonizer_Bypass(),
    MusDelay_Bypass(),
    Gate_Bypass(),
    Derelict_Bypass(),
    APhaser_Bypass(),
    Valve_Bypass(),
    DFlange_Bypass(),
    Ring_Bypass(),
    Exciter_Bypass(),
    DistBand_Bypass(),
    Arpie_Bypass(),
    Expander_Bypass(),
    Shuffle_Bypass(),
    Synthfilter_Bypass(),
    VaryBand_Bypass(),
    Convol_Bypass(),
    Looper_Bypass(),
    MuTroMojo_Bypass(),
    Echoverse_Bypass(),
    CoilCrafter_Bypass(),
    ShelfBoost_Bypass(),
    Vocoder_Bypass(),
    Sustainer_Bypass(),
    Sequence_Bypass(),
    Shifter_Bypass(),
    StompBox_Bypass(),
    Reverbtron_Bypass(),
    Echotron_Bypass(),
    StereoHarm_Bypass(),
    CompBand_Bypass(),
    Opticaltrem_Bypass(),
    Vibe_Bypass(),
    Infinity_Bypass(),
    Bypass_B(),
    Reverb_B(),
    Chorus_B(),
    Flanger_B(),
    Phaser_B(),
    APhaser_B(),
    DFlange_B(),
    Overdrive_B(),
    Distorsion_B(),
    Echo_B(),
    EQ1_B(),
    EQ2_B(),
    Compressor_B(),
    WhaWha_B(),
    Alienwah_B(),
    Cabinet_B(),
    Pan_B(),
    Harmonizer_B(),
    MusDelay_B(),
    Gate_B(),
    Derelict_B(),
    Valve_B(),
    Ring_B(),
    Exciter_B(),
    DistBand_B(),
    Arpie_B(),
    Expander_B(),
    Shuffle_B(),
    Synthfilter_B(),
    VaryBand_B(),
    Convol_B(),
    Looper_B(),
    MuTroMojo_B(),
    Echoverse_B(),
    CoilCrafter_B(),
    ShelfBoost_B(),
    Vocoder_B(),
    Sustainer_B(),
    Sequence_B(),
    Shifter_B(),
    StompBox_B(),
    Reverbtron_B(),
    Echotron_B(),
    StereoHarm_B(),
    CompBand_B(),
    Opticaltrem_B(),
    Vibe_B(),
    Infinity_B(),
    Selected_Preset(),
    lv(),
    saved_order(),
    efx_order(),
    new_order(),
    availables(),
    active(),
    MidiCh(),
    HarCh(),
    StereoHarCh(),
    init_state(),
    actuvol(),
    help_displayed(),
    modified(),
    autoassign(),
    comemouse(),
    aconnect_MI(),
    aconnect_JA(),
    aconnect_JIA(),
    cuan_jack(),
    cuan_ijack(),
    IsCoIn(),
    Cyoin(),
    Pyoin(),
    Ccin(),
    Pcin(),
    Mcontrol(),
    RControl(),
    ControlGet(),
    CountWait(),
    XUserMIDI(),
    eff_filter(),
    Har_Down(),
    Har_U_Q(),
    Har_D_Q(),
    Rev_Down(),
    Rev_U_Q(),
    Rev_D_Q(),
    Con_Down(),
    Con_U_Q(),
    Con_D_Q(),
    Shi_Down(),
    Shi_U_Q(),
    Shi_D_Q(),
    Seq_Down(),
    Seq_U_Q(),
    Seq_D_Q(),
    Voc_Down(),
    Voc_U_Q(),
    Voc_D_Q(),
    Ste_Down(),
    Ste_U_Q(),
    Ste_D_Q(),
    aFreq(),
    Metro_Vol(),
    M_Metro_Sound(),
    deachide(),
    scalable(0),
    change_scale(1),
    font(),
    flpos(),
    upsample(),
    UpQual(),
    DownQual(),
    UpAmo(),
    J_SAMPLE_RATE(),
    J_PERIOD(),
    m_displayed(),
    Mvalue(),
    Mnumeff(),
    OnOffC(),
    period(),
    fPeriod(),
    sample_rate(),
    fSample_rate(),
    cSample_rate(),
    interpbuf(NULL),
    Wave_res_amount(),
    Wave_up_q(),
    Wave_down_q(),
    sw_stat(),
    MIDIway(),
    NumParams(423),
    NumEffects(47),
    fontsize(C_DEFAULT_FONT_SIZE),
    swidth(),
    sheight(),
    sschema(),
    slabel_color(),
    sfore_color(),
    sback_color(),
    sleds_color(),
    have_signal(),
    OnCounter(),
    t_periods(),
    HarQual(),
    SteQual(),
    tempocnt(),
    Tap_Display(),
    Tap_Selection(),
    Tap_TempoSet(),
    Tap_SetValue(),
    t_timeout(),
    jt_state(),
    Tap_Updated(),
    note_old(),
    cents_old(),
    cpufp(),
    mess_dis(),
    numpi(),
    numpo(),
    numpa(),
    numpmi(),
    numpmo(),
    numpc(),
    midi_table(),
    a_bank(),
    new_bank_loaded(),
    Aux_Gain(),
    Aux_Threshold(),
    Aux_MIDI(),
    Aux_Minimum(),
    Aux_Maximum(),
    Aux_Source(),
    last_auxvalue(),
    ena_tool(),
    VocBands(),
    RCOpti_Harm(),
    RCOpti_Stereo(),
    RCOpti_Ring(),
    M_Metro_Tempo(),
    M_Metro_Bar(),
    mtc_counter(),
    EnableBackgroundImage(),
    ML_filter(),
    ML_clist(),
    Tap_time_Init(),
    Tap_timeB(),
    Tap_timeC(),
    jt_tempo(),
    tempobuf(),
    u_down(),
    u_up(),
    timeA(),
    booster(),
    cpuload(),
    rtrig(),
    efxoutl(NULL),
    efxoutr(NULL),
    auxdata(NULL),
    auxresampled(NULL),
    anall(NULL),
    analr(NULL),
    smpl(NULL),
    smpr(NULL),
    denormal(NULL),
    m_ticks(NULL),
    Master_Volume(0.50f),
    Input_Gain(0.50f),
    Fraction_Bypass(1.0f),
    Log_I_Gain(),
    Log_M_Volume(),
    M_Metro_Vol(),
    val_sum(),
    old_il_sum(),
    old_ir_sum(),
    old_vl_sum(),
    old_vr_sum(),
    val_vl_sum(),
    val_vr_sum(),
    val_il_sum(),
    val_ir_sum(),
    old_a_sum(),
    val_a_sum(),
    bogomips(),
    looper_size(),
    nfreq_old(),
    afreq_old(),
    tmpprefname(),
    Preset_Name(NULL),
    Author(NULL),
    Bank_Saved(NULL),
    UserRealName(NULL),
    MID(),
    BankFilename(),
    UDirFilename(),
    BackgroundImage(),
    efx_names(),
    efx_params(),
    Bank(),
    M_table(),
    B_Names(),
    midi_in(NULL),
    midi_out(NULL),
    jack_po(),
    jack_poi()

{
    char temp[256];
    error_num = 0;
    nojack = 0;
    memset(Mcontrol, 0, sizeof (Mcontrol));
    sprintf(temp, "%s", jack_client_name);

#ifdef JACK_SESSION
    jackclient = jack_client_open(temp, JackSessionID, NULL, s_uuid);
#else
    jackclient = jack_client_open(temp, options, &status, NULL);
#endif


    if (jackclient == NULL)
    {
        fprintf(stderr, "Cannot make a jack client, is jackd running?\n");
        nojack = 1;
        exitwithhelp = 1;
        return;
    }

    strcpy(jackcliname, jack_get_client_name(jackclient));

    J_SAMPLE_RATE = jack_get_sample_rate(jackclient);
    J_PERIOD = jack_get_buffer_size(jackclient);

    rakarrack.get(PrefNom("Disable Warnings"), mess_dis, 0);
    rakarrack.get(PrefNom("Filter DC Offset"), DC_Offset, 0);
    rakarrack.get(PrefNom("UpSampling"), upsample, 0);
    rakarrack.get(PrefNom("UpQuality"), UpQual, 4);
    rakarrack.get(PrefNom("DownQuality"), DownQual, 4);
    rakarrack.get(PrefNom("UpAmount"), UpAmo, 0);

    Adjust_Upsample();

    rakarrack.get(PrefNom("Looper Size"), looper_size, 1);
    rakarrack.get(PrefNom("Calibration"), aFreq, 440.0f);
    //    RecNote->update_freqs(aFreq); // cannot call here until RecNote is declared, which must be placed after efxoutl, efxoutr initialization

    rakarrack.get(PrefNom("Vocoder Bands"), VocBands, 32);
    rakarrack.get(PrefNom("Recognize Trigger"), rtrig, .6f);

    Fraction_Bypass = 1.0f;
    Master_Volume = 0.50f;
    Input_Gain = 0.50f;

    rakarrack.get(PrefNom("Harmonizer Downsample"), Har_Down, 5);
    rakarrack.get(PrefNom("Harmonizer Up Quality"), Har_U_Q, 4);
    rakarrack.get(PrefNom("Harmonizer Down Quality"), Har_D_Q, 2);

    rakarrack.get(PrefNom("StereoHarm Downsample"), Ste_Down, 5);
    rakarrack.get(PrefNom("StereoHarm Up Quality"), Ste_U_Q, 4);
    rakarrack.get(PrefNom("StereoHarm Down Quality"), Ste_D_Q, 2);

    rakarrack.get(PrefNom("Reverbtron Downsample"), Rev_Down, 5);
    rakarrack.get(PrefNom("Reverbtron Up Quality"), Rev_U_Q, 4);
    rakarrack.get(PrefNom("Reverbtron Down Quality"), Rev_D_Q, 2);

    rakarrack.get(PrefNom("Convolotron Downsample"), Con_Down, 6);
    rakarrack.get(PrefNom("Convolotron Up Quality"), Con_U_Q, 4);
    rakarrack.get(PrefNom("Convolotron Down Quality"), Con_D_Q, 2);

    rakarrack.get(PrefNom("Sequence Downsample"), Seq_Down, 5);
    rakarrack.get(PrefNom("Sequence Up Quality"), Seq_U_Q, 4);
    rakarrack.get(PrefNom("Sequence Down Quality"), Seq_D_Q, 2);

    rakarrack.get(PrefNom("Shifter Downsample"), Shi_Down, 5);
    rakarrack.get(PrefNom("Shifter Up Quality"), Shi_U_Q, 4);
    rakarrack.get(PrefNom("Shifter Down Quality"), Shi_D_Q, 2);

    rakarrack.get(PrefNom("Vocoder Downsample"), Voc_Down, 5);
    rakarrack.get(PrefNom("Vocoder Up Quality"), Voc_U_Q, 4);
    rakarrack.get(PrefNom("Vocoder Down Quality"), Voc_D_Q, 2);

    rakarrack.get(PrefNom("Waveshape Resampling"), Wave_res_amount, 2);
    rakarrack.get(PrefNom("Waveshape Up Quality"), Wave_up_q, 4);
    rakarrack.get(PrefNom("Waveshape Down Quality"), Wave_down_q, 2);

    rakarrack.get(PrefNom("Harmonizer Quality"), HarQual, 4);
    rakarrack.get(PrefNom("StereoHarm Quality"), SteQual, 4);

    rakarrack.get(PrefNom("Auto Connect Jack"), aconnect_JA, 1);
    
    /* For default on new installs - don't connect input - can cause feedback loop = 0 */
    rakarrack.get(PrefNom("Auto Connect Jack In"), aconnect_JIA, 0);

    rakarrack.get(PrefNom("Auto Connect Num"), cuan_jack, 2);
    rakarrack.get(PrefNom("Auto Connect In Num"), cuan_ijack, 1);

    memset(temp, 0, sizeof (temp));
    char j_names[128];

    static const char *jack_names[] ={"system:playback_1", "system:playback_2"};

    for (int i = 0; i < cuan_jack; i++)
    {
        memset(temp, 0, sizeof (temp));
        sprintf(temp, "Jack Port %d", i + 1);
        
        if (i < 2)
        {
            strcpy(j_names, jack_names[i]);
        }
        else
        {
            strcpy(j_names, "");
        }
        
        rakarrack.get(PrefNom(temp), jack_po[i].name, j_names, 128);
    }

    memset(j_names, 0, sizeof (j_names));

    static const char *jack_inames[] ={"system:capture_1", "system:capture_2"};

    for (int i = 0; i < cuan_ijack; i++)
    {
        memset(temp, 0, sizeof (temp));
        sprintf(temp, "Jack Port In %d", i + 1);
        
        if (i < 1)
        {
            strcpy(j_names, jack_inames[i]);
        }
        else
        {
            strcpy(j_names, "");
        }
        
        rakarrack.get(PrefNom(temp), jack_poi[i].name, j_names, 128);
    }

    bogomips = 0.0f;
    Get_Bogomips();
    //int i = Get_Bogomips();     // compiler warning i not used

    efxoutl = (float *) malloc(sizeof (float) * period);
    efxoutr = (float *) malloc(sizeof (float) * period);

    smpl = (float *) malloc(sizeof (float) * period);
    smpr = (float *) malloc(sizeof (float) * period);

    anall = (float *) malloc(sizeof (float) * period);
    analr = (float *) malloc(sizeof (float) * period);

    auxdata = (float *) malloc(sizeof (float) * period);
    auxresampled = (float *) malloc(sizeof (float) * period);

    m_ticks = (float *) malloc(sizeof (float) * period);

    //ssj
    interpbuf = (float*) malloc(sizeof (float)* period);

    memset(efxoutl, 0, sizeof (float)*period);
    memset(efxoutr, 0, sizeof (float)*period);

    memset(smpl, 0, sizeof (float)*period);
    memset(smpr, 0, sizeof (float)*period);

    memset(anall, 0, sizeof (float)*period);
    memset(analr, 0, sizeof (float)*period);

    memset(auxdata, 0, sizeof (float)*period);
    memset(auxresampled, 0, sizeof (float)*period);

    memset(m_ticks, 0, sizeof (float)*period);
    memset(interpbuf, 0, sizeof (float)*period);

    Fpre = new FPreset();

    DC_Offsetl = new AnalogFilter(1, 20, 1, 0, sample_rate, interpbuf);
    DC_Offsetr = new AnalogFilter(1, 20, 1, 0, sample_rate, interpbuf);
    M_Metronome = new metronome(fSample_rate, period);
    efx_Chorus = new Chorus(fSample_rate, period);
    efx_Flanger = new Chorus(fSample_rate, period);
    efx_Rev = new Reverb(fSample_rate, period);
    efx_Echo = new Echo(fSample_rate, period);
    efx_Phaser = new Phaser(fSample_rate, period);
    efx_APhaser = new Analog_Phaser(fSample_rate, period);
    efx_Distorsion = new Distorsion(Wave_res_amount, Wave_up_q, Wave_down_q, fSample_rate, period);
    efx_Overdrive = new Distorsion(Wave_res_amount, Wave_up_q, Wave_down_q, fSample_rate, period);
    efx_EQ2 = new EQ(EQ2_PARAMETRIC, fSample_rate, period);
    efx_EQ1 = new EQ(EQ1_REGULAR, fSample_rate, period);
    efx_Compressor = new Compressor(fSample_rate, period);
    efx_WhaWha = new WahWah(fSample_rate, period);
    efx_Alienwah = new Alienwah(fSample_rate, period);
    efx_Cabinet = new Cabinet(fSample_rate, period);
    efx_Pan = new Pan(fSample_rate, period);
    efx_Har = new Harmonizer((long) HarQual, Har_Down, Har_U_Q, Har_D_Q, fSample_rate, period);
    efx_MusDelay = new MusicDelay(fSample_rate, period);
    efx_Gate = new Gate(fSample_rate, period);
    efx_Derelict = new Derelict(Wave_res_amount, Wave_up_q, Wave_down_q, fSample_rate, period);
    efx_FLimiter = new Compressor(fSample_rate, period);
    efx_Valve = new Valve(fSample_rate, period);
    efx_DFlange = new Dflange(fSample_rate, period);
    efx_Ring = new Ring(fSample_rate, period);
    efx_Exciter = new Exciter(fSample_rate, period);
    efx_DistBand = new DistBand(Wave_res_amount, Wave_up_q, Wave_down_q, fSample_rate, period);
    efx_Arpie = new Arpie(fSample_rate, period);
    efx_Expander = new Expander(fSample_rate, period);
    efx_Shuffle = new Shuffle(fSample_rate, period);
    efx_Synthfilter = new Synthfilter(fSample_rate, period);
    efx_VaryBand = new VaryBand(fSample_rate, period);
    efx_Convol = new Convolotron(Con_Down, Con_U_Q, Con_D_Q, fSample_rate, period);
    efx_Looper = new Looper(looper_size, fSample_rate, period);
    efx_MuTroMojo = new MuTroMojo(fSample_rate, period);
    efx_Echoverse = new Echoverse(fSample_rate, period);
    efx_CoilCrafter = new CoilCrafter(fSample_rate, period);
    efx_ShelfBoost = new ShelfBoost(fSample_rate, period);
    efx_Vocoder = new Vocoder(auxresampled, VocBands, Voc_Down, Voc_U_Q, Voc_D_Q, fSample_rate, period);
    efx_Sustainer = new Sustainer(fSample_rate, period);
    efx_Sequence = new Sequence((long) HarQual, Seq_Down, Seq_U_Q, Seq_D_Q, fSample_rate, period);
    efx_Shifter = new Shifter((long) HarQual, Shi_Down, Shi_U_Q, Shi_D_Q, fSample_rate, period);
    efx_StompBox = new StompBox(Wave_res_amount, Wave_up_q, Wave_down_q, fSample_rate, period);
    efx_Reverbtron = new Reverbtron(Rev_Down, Rev_U_Q, Rev_D_Q, fSample_rate, period);
    efx_Echotron = new Echotron(fSample_rate, period);
    efx_StereoHarm = new StereoHarm((long) SteQual, Ste_Down, Ste_U_Q, Ste_D_Q, fSample_rate, period);
    efx_CompBand = new CompBand(fSample_rate, period);
    efx_Opticaltrem = new Opticaltrem(fSample_rate, period);
    efx_Vibe = new Vibe(fSample_rate, period);
    efx_Infinity = new Infinity(fSample_rate, period);

    U_Resample = new Resample(UpQual);
    D_Resample = new Resample(DownQual);
    A_Resample = new Resample(3);

    beat = new beattracker(fSample_rate, period);
    efx_Tuner = new Tuner(fSample_rate);
    efx_MIDIConverter = new MIDIConverter(jackcliname, fSample_rate, period);
    HarmRecNote = new Recognize(rtrig, aFreq, fSample_rate, period);
    StHarmRecNote = new Recognize(rtrig, aFreq, fSample_rate, period);
    RingRecNote = new Recognize(rtrig, aFreq, fSample_rate, period);
    RC_Harm = new RecChord();
    RC_Stereo_Harm = new RecChord();

    Preset_Name = (char *) malloc(sizeof (char) * 64);
    memset(Preset_Name, 0, sizeof (char) * 64);
    Author = (char *) malloc(sizeof (char) * 64);
    memset(Author, 0, sizeof (char) * 64);
    Bank_Saved = (char *) malloc(sizeof (char) * 128);
    memset(Bank_Saved, 0, sizeof (char) * 128);
    UserRealName = (char *) malloc(sizeof (char) * 128);
    memset(UserRealName, 0, sizeof (char) * 128);

    // Names

    /*
    //Filter

    1   - Distortion
    2   - Modulation
    4   - Time
    8   - Emulation
    16  - Filters
    32  - Dynamics
    64  - Processing & EQ
    128 - Synthesis
     */

    NumEffects = 47;

    {
        static const char *los_names[] = {
            "AlienWah", "11", "16",
            "Analog Phaser", "18", "2",
            "Arpie", "24", "4",
            "Cabinet", "12", "8",
            "Chorus", "5", "2",
            "Coil Crafter", "33", "8",
            "CompBand", "43", "8",
            "Compressor", "1", "32",
            "Convolotron", "29", "8",
            "Derelict", "17", "1",
            "DistBand", "23", "1",
            "Distortion", "2", "1",
            "Dual Flange", "20", "2",
            "Echo", "4", "4",
            "Echotron", "41", "4",
            "Echoverse", "32", "4",
            "EQ", "0", "64",
            "Exciter", "22", "64",
            "Expander", "25", "32",
            "Flanger", "7", "2",
            "Harmonizer", "14", "128",
            "Infinity", "46", "16",
            "Looper", "30", "128",
            "MusicalDelay", "15", "4",
            "MuTroMojo", "31", "16",
            "NoiseGate", "16", "32",
            "Opticaltrem", "44", "2",
            "Overdrive", "3", "1",
            "Pan", "13", "64",
            "Parametric EQ", "9", "64",
            "Phaser", "6", "2",
            "Reverb", "8", "4",
            "Reverbtron", "40", "4",
            "Ring", "21", "128",
            "Sequence", "37", "128",
            "ShelfBoost", "34", "64",
            "Shifter", "38", "128",
            "Shuffle", "26", "64",
            "StereoHarm", "42", "128",
            "StompBox", "39", "9",
            "Sustainer", "36", "32",
            "Synthfilter", "27", "16",
            "Valve", "19", "9",
            "VaryBand", "28", "2",
            "Vibe", "45", "2",
            "Vocoder", "35", "128",
            "WahWah", "10", "16"

        };

        for (int i = 0; i < NumEffects * 3; i += 3)
        {
            strcpy(efx_names[i / 3].Nom, los_names[i]);
            sscanf(los_names[i + 1], "%d", &efx_names[i / 3].Pos);
            sscanf(los_names[i + 2], "%d", &efx_names[i / 3].Type);
        }
    }


    NumParams = 443;

    {
        static const char *los_params[] = {

            "Alienwah Dry/Wet", "55", "11",
            "Alienwah Pan", "61", "11",
            "Alienwah Tempo", "76", "11",
            "Alienwah Random", "109", "11",
            "Alienwah LFO Type", "456", "11",
            "Alienwah Phase", "115", "11",
            "Alienwah Stereo Df.", "103", "11",
            "Alienwah Depth", "20", "11",
            "Alienwah Delay", "403", "11",
            "Alienwah Feedback", "82", "11",
            "Alienwah L/R Cross", "96", "11",
            
            "Analog Phaser Dry/Wet", "117", "18",
            "Analog Phaser LFO Type", "457", "18",
            "Analog Phaser Tempo", "119", "18",
            "Analog Phaser P. Depth", "120", "18",
            "Analog Phaser Width", "121", "18",
            "Analog Phaser Feedback", "122", "18",
            "Analog Phaser Mismatch", "123", "18",
            "Analog Phaser Distortion", "118", "18",
            "Analog Phaser Random", "404", "18",
            "Analog Phaser Stereo Df.", "124", "18",

            "Arpie Dry/Wet", "212", "24",
            "Arpie Arpe's", "213", "24",
            "Arpie Pan", "214", "24",
            "Arpie Tempo", "215", "24",
            "Arpie L/R Delay", "216", "24",
            "Arpie L/R Cross", "217", "24",
            "Arpie Feedback", "218", "24",
            "Arpie Damp", "219", "24",

            "Balance", "12", "50",
            
            "Cabinet Gain","405 ","12",

            "Chorus Dry/Wet", "52", "5",
            "Chorus Pan", "50", "5",
            "Chorus Tempo", "72", "5",
            "Chorus Random", "105", "5",
            "Chorus LFO Type", "458", "5",
            "Chorus Stereo Df", "99", "5",
            "Chorus Depth", "23", "5",
            "Chorus Delay", "406", "5",
            "Chorus Feedback", "79", "5",
            "Chorus L/R Cross", "91", "5",

            "CoilCrafter Gain", "286", "33",
            "CoilCrafter Tone", "287", "33",
            "CoilCrafter Freq 1", "288", "33",
            "CoilCrafter Q 1", "289", "33",
            "CoilCrafter Freq 2", "290", "33",
            "CoilCrafter Q 2", "291", "33",

            "CompBand Dry/Wet", "368", "43",
            "CompBand Gain", "369", "43",
            "CompBand L Ratio", "370", "43",
            "CompBand ML Ratio", "371", "43",
            "CompBand MH Ratio", "372", "43",
            "CompBand H Ratio", "373", "43",
            "CompBand L Thres", "374", "43",
            "CompBand ML Thres", "375", "43",
            "CompBand MH Thres", "376", "43",
            "CompBand H Thres", "377", "43",
            "CompBand Cross 1", "378", "43",
            "CompBand Cross 2", "379", "43",
            "CompBand Cross 3", "380", "43",

            "Compressor A.Time", "142", "1",
            "Compressor Knee", "145", "1",
            "Compressor Output", "147", "1",
            "Compressor Ratio", "144", "1",
            "Compressor R.Time", "143", "1",
            "Compressor Threshold", "146", "1",

            "Convolotron Dry/Wet", "280", "29",
            "Convolotron Pan", "281", "29",
            "Convolotron Level", "282", "29",
            "Convolotron Damp", "283", "29",
            "Convolotron Feedback", "284", "29",
            "Convolotron Length", "285", "29",

            "Derelict Dry/Wet", "125", "17",
            "Derelict L/R Cross", "127", "17",
            "Derelict Drive", "2", "17",
            "Derelict Level", "3", "17",
            "Derelict Level", "451", "17",
            "Derelict Color", "6", "17",
            "Derelict Sub Octave", "8", "17",
            "Derelict Pan", "126", "17",
            "Derelict LPF", "4", "17",
            "Derelict HPF", "5", "17",

            "DistBand Dry/Wet", "202", "23",
            "DistBand L/R Cross", "203", "23",
            "DistBand Drive", "204", "23",
            "DistBand Level", "205", "23",
            "DistBand L. Gain", "206", "23",
            "DistBand M. Gain", "207", "23",
            "DistBand H. Gain", "208", "23",
            "DistBand Cross 1", "209", "23",
            "DistBand Cross 2", "210", "23",
            "DistBand Type Low", "452", "23",
            "DistBand Type Med", "453", "23",
            "DistBand Type High", "454", "23",
            "DistBand Pan", "211", "23",
            
            "Distortion Dry/Wet", "30", "2",
            "Distortion L/R Cross", "95", "2",
            "Distortion Drive", "69", "2",
            "Distortion Level", "71", "2",
            "Distortion Type", "449", "2",
            "Distortion Pan", "48", "2",
            "Distortion Sub Octave", "9", "2",
            "Distortion LPF", "86", "2",
            "Distortion HPF", "89", "2",

            "Dual Flange Dry/Wet", "158", "20",
            "Dual Flange Pan", "159", "20",
            "Dual Flange L/R Cross", "160", "20",
            "Dual Flange Depth", "161", "20",
            "Dual Flange Width", "162", "20",
            "Dual Flange Offset", "163", "20",
            "Dual Flange Feedback", "164", "20",
            "Dual Flange LPF", "165", "20",
            "Dual Flange Tempo", "166", "20",
            "Dual Flange Stereo Df", "167", "20",
            "Dual Flange LFO Type", "460", "20",
            "Dual Flange Random", "168", "20",

            "Echo Dry/Wet", "59", "4",
            "Echo Reverse", "407", "4",
            "Echo Pan", "46", "4",
            "Echo Delay", "408", "4",
            "Echo L/R Delay", "409", "4",
            "Echo L/R Cross", "97", "4",
            "Echo Feedback", "78", "4",
            "Echo Damp", "410", "4",

            "Echotron Dry/Wet", "348", "41",
            "Echotron Pan", "349", "41",
            "Echotron Tempo", "350", "41",
            "Echotron Damp", "351", "41",
            "Echotron Feedback", "352", "41",
            "Echotron L/R Cross", "353", "41",
            "Echotron Width", "354", "41",
            "Echotron Depth", "355", "41",
            "Echotron Stereo Df", "356", "41",
            "Echotron LFO Type", "461", "41",
            "Echotron #", "357", "41",

            "Echoverse Dry/Wet", "303", "32",
            "Echoverse Reverse", "304", "32",
            "Echoverse Pan", "305", "32",
            "Echoverse Tempo", "306", "32",
            "Echoverse L/R Delay", "307", "32",
            "Echoverse Feedback", "308", "32",
            "Echoverse Damp", "309", "32",
            "Echoverse Ex Stereo", "310", "32",
            "Echoverse Angle", "311", "32",

            "EQ Gain", "130", "0",
            "EQ Q", "131", "0",
            "EQ 31 Hz", "132", "0",
            "EQ 63 Hz", "133", "0",
            "EQ 125 Hz", "134", "0",
            "EQ 250 Hz", "135", "0",
            "EQ 500 Hz", "136", "0",
            "EQ 1 Khz", "137", "0",
            "EQ 2 Khz", "138", "0",
            "EQ 4 Khz", "139", "0",
            "EQ 8 Khz", "140", "0",
            "EQ 16 Khz", "141", "0",

            "Exciter Gain", "189", "22",
            "Exciter LPF", "190", "22",
            "Exciter HPF", "191", "22",
            "Exciter Har 1", "192", "22",
            "Exciter Har 2", "193", "22",
            "Exciter Har 3", "194", "22",
            "Exciter Har 4", "195", "22",
            "Exciter Har 5", "196", "22",
            "Exciter Har 6", "197", "22",
            "Exciter Har 7", "198", "22",
            "Exciter Har 8", "199", "22",
            "Exciter Har 9", "200", "22",
            "Exciter Har 10", "201", "22",

            "Expander A.Time", "220", "25",
            "Expander R.Time", "221", "25",
            "Expander Shape", "222", "25",
            "Expander Threshold", "223", "25",
            "Expander Out Gain", "224", "25",
            "Expander LPF", "225", "25",
            "Expander HPF", "226", "25",

            "Flanger Dry/Wet", "53", "7",
            "Flanger Pan", "51", "7",
            "Flanger Tempo", "73", "7",
            "Flanger Random", "106", "7",
            "Flanger LFO Type", "459", "7",
            "Flanger Stereo Df", "100", "7",
            "Flanger Depth", "22", "7",
            "Flanger Delay", "411", "7",
            "Flanger Feedback", "80", "7",
            "Flanger L/R Cross", "92", "7",

            "Harmonizer Dry/Wet", "31", "14",
            "Harmonizer Interval", "27", "14",
            "Harmonizer Gain", "412", "14",
            "Harmonizer Pan", "49", "14",
            "Harmonizer Freq", "26", "14",
            "Harmonizer Filter Gain", "413", "14",
            "Harmonizer Filter Q", "414", "14",
            "Harmonizer SELECT", "447", "14",
            "Harmonizer Note", "444", "14",
            "Harmonizer Chord", "445", "14",

            "Infinity Dry/Wet", "395", "46",
            "Infinity Res", "396", "46",
            "Infinity Filter Band 1", "415", "46",
            "Infinity Filter Band 2", "416", "46",
            "Infinity Filter Band 3", "417", "46",
            "Infinity Filter Band 4", "418", "46",
            "Infinity Filter Band 5", "419", "46",
            "Infinity Filter Band 6", "420", "46",
            "Infinity Filter Band 7", "421", "46",
            "Infinity Filter Band 8", "422", "46",
            "Infinity AutoPan", "397", "46",
            "Infinity Stereo Df", "398", "46",
            "Infinity Start", "399", "46",
            "Infinity End", "400", "46",
            "Infinity Tempo", "401", "46",
            "Infinity Subdiv", "402", "46",

            "Input", "14", "50",

            "Looper Dry/Wet", "267", "30",
            "Looper Level 1", "268", "30",
            "Looper Level 2", "269", "30",
            "Looper Tempo", "423", "30",
            "Looper Reverse", "270", "30",
            "Looper Auto Play", " 271", "30",
            "Looper Play", "272", "30",
            "Looper Pause", "273", "30",
            "Looper Record", "274", "30",
            "Looper R1", "275", "30",
            "Looper R2", "276", "30",
            "Looper Track 1", "277", "30",
            "Looper Track 2", "278", "30",
            "Looper Clear", "279", "30",

            "Multi On/Off", "116", "50",

            "Musical Delay Dry/Wet", "56", "15",
            "Musical Delay L/R Cross", "98", "15",
            "Musical Delay Pan 1", "62", "15",
            "Musical Delay Pan 2", "65", "15",
            "Musical Delay Tempo", "424", "15",
            "Musical Delay Gain 1", "24", "15",
            "Musical Delay Gain 2", "25", "15",
            "Musical Delay FB 1", "83", "15",
            "Musical Delay FB 2", "84", "15",
            "Musical Delay Damp", "425", "15",

            "MuTroMojo Dry/Wet", "256", "31",
            "MuTroMojo LP", "257", "31",
            "MuTroMojo BP", "258", "31",
            "MuTroMojo HP", "259", "31",
            "MuTroMojo LFO Type", "462", "31",
            "MuTroMojo Depth", "260", "31",
            "MuTroMojo Tempo", "261", "31",
            "MuTroMojo Res", "262", "31",
            "MuTroMojo Range", "263", "31",
            "MuTroMojo Wah", "264", "31",
            "MuTroMojo E. Sens", "265", "31",
            "MuTroMojo Smooth", "266", "31",
            "MuTroMojo Random", "426", "31",
            "MuTroMojo Stereo Df", "427", "31",
            "MuTroMojo St. Freq", "428", "31",
            
            "NoiseGate A. Time", "429", "16",
            "NoiseGate R. Time", "430", "16",
            "NoiseGate Range", "431", "16",
            "NoiseGate Threshold", "432", "16",
            "NoiseGate Hold", "433", "16",
            "NoiseGate LPF", "434", "16",
            "NoiseGate HPF", "435", "16",

            "Opticaltrem Depth", "381", "44",
            "Opticaltrem Tempo", "382", "44",
            "Opticaltrem Random", "383", "44",
            "Opticaltrem LFO Type", "463", "44",
            "Opticaltrem Stereo Df", "384", "44",
            "Opticaltrem Pan", "385", "44",

            "Overdrive Dry/Wet", "29", "3",
            "Overdrive L/R Cross", "94", "3",
            "Overdrive Drive", "68", "3",
            "Overdrive Level", "70", "3",
            "Overdrive Type", "450", "3",
            "Overdrive Pan", "47", "3",
            "Overdrive LPF", "85", "3",
            "Overdrive HPF", "88", "3",

            "Pan Dry/Wet", "58", "13",
            "Pan Pan", "67", "13",
            "Pan Tempo", "77", "13",
            "Pan Random", "110", "13",
            "Pan LFO Type", "464", "13",
            "Pan Stereo Df", "104", "13",
            "Pan E. Stereo", "436", "13",
            
            "Parametric EQ Gain", "148", "9",
            "Parametric EQ Low Freq", "149", "9",
            "Parametric EQ Low Gain", "150", "9",
            "Parametric EQ Low Q", "151", "9",
            "Parametric EQ Mid Freq", "152", "9",
            "Parametric EQ Mid Gain", "153", "9",
            "Parametric EQ Mid Q", "154", "9",
            "Parametric EQ High Freq", "155", "9",
            "Parametric EQ High Gain", "156", "9",
            "Parametric EQ High Q", "157", "9",

            "Phaser Dry/Wet", "54", "6",
            "Phaser Pan", "60", "6",
            "Phaser Tempo", "74", "6",
            "Phaser Random", "107", "6",
            "Phaser LFO Type", "465", "6",
            "Phaser Phase", "114", "6",
            "Phaser Stereo Df", "101", "6",
            "Phaser Depth", "21", "6",
            "Phaser Feedback", "81", "6",
            "Phaser L/R Cross", "93", "6",

            "Reverb Dry/Wet", "57", "8",
            "Reverb Pan", "63", "8",
            
            "Reverb Time", "437", "8",
            "Reverb Initial Delay", "438", "8",
            "Reverb Del. E/R", "439", "8",
            "Reverb Room Size", "440", "8",
            "Reverb LPF", "87", "8",
            "Reverb HPF", "90", "8",
            "Reverb Damping", "441", "8",

            "Reverbtron Dry/Wet", "339", "40",
            "Reverbtron Pan", "340", "40",
            "Reverbtron Level", "341", "40",
            "Reverbtron Damp", "342", "40",
            "Reverbtron Feedback", "343", "40",
            "Reverbtron Length", "344", "40",
            "Reverbtron Stretch", "345", "40",
            "Reverbtron Initial Delay", "346", "40",
            "Reverbtron Fade", "347", "40",
            "Reverbtron Diffusion", "442", "40",
            "Reverbtron LPF", "443", "40",

            "Ring Dry/Wet", "178", "21",
            "Ring L/R Cross", "179", "21",
            "Ring Input", "180", "21",
            "Ring Level", "181", "21",
            "Ring Pan", "182", "21",
            "Ring Depth", "183", "21",
            "Ring Freq", "184", "21",
            "Ring Sine", "185", "21",
            "Ring Triangle", "186", "21",
            "Ring Sawtooth", "187", "21",
            "Ring Square", "188", "21",

            "Sequence Dry/Wet", "314", "37",
            "Sequence 1", "315", "37",
            "Sequence 2", "316", "37",
            "Sequence 3", "317", "37",
            "Sequence 4", "318", "37",
            "Sequence 5", "319", "37",
            "Sequence 6", "320", "37",
            "Sequence 7", "321", "37",
            "Sequence 8", "322", "37",
            "Sequence Tempo", "323", "37",
            "Sequence Q", "324", "37",
            "Sequence Stereo Df", "325", "37",

            "ShelfBoost Gain", "292", "34",
            "ShelfBoost Level", "293", "34",
            "ShelfBoost Tone", "294", "34",
            "ShelfBoost Presence", "295", "34",

            "Shifter Dry/Wet", "326", "38",
            "Shifter Interval", "327", "38",
            "Shifter Gain", "328", "38",
            "Shifter Pan", "329", "38",
            "Shifter Attack", "330", "38",
            "Shifter Decay", "331", "38",
            "Shifter Threshold", "332", "38",
            "Shifter Whammy", "333", "38",

            "Shuffle Dry/Wet", "227", "26",
            "Shuffle Low Freq", "228", "26",
            "Shuffle Low Gain", "229", "26",
            "Shuffle M.L. Freq", "230", "26",
            "Shuffle M.L. Gain", "231", "26",
            "Shuffle M.H. Freq", "232", "26",
            "Shuffle M.H. Gain", "233", "26",
            "Shuffle High Freq", "234", "26",
            "Shuffle High Gain", "235", "26",
            "Shuffle Q", "236", "26",

            "StereoHarm Dry/Wet", "358", "42",
            "StereoHarm Int L", "359", "42",
            "StereoHarm Chrm L", "360", "42",
            "StereoHarm Gain L", "361", "42",
            "StereoHarm Int R", "362", "42",
            "StereoHarm Chrm R", "363", "42",
            "StereoHarm Gain R", "364", "42",
            "StereoHarm L/R Cross", "365", "42",
            "StereoHarm SELECT", "448", "42",
            "StereoHarm Note", "366", "42",
            "StereoHarm Chord", "367", "42",

            "StompBox Level", "334", "39",
            "StompBox Gain", "335", "39",
            "StompBox Bias", "336", "39",
            "StompBox Mid", "337", "39",
            "StompBox Tone", "338", "39",
            "StompBox Mode", "455", "39",

            "Sustainer Gain", "312", "36",
            "Sustainer Sustain", "313", "36",

            "Synthfilter Dry/Wet", "237", "27",
            "Synthfilter Distort", "238", "27",
            "Synthfilter Tempo", "239", "27",
            "Synthfilter Random", "446", "27",
            "Synthfilter LFO Type", "466", "27",
            "Synthfilter Stereo Df", "240", "27",
            "Synthfilter Width", "241", "27",
            "Synthfilter Feedback", "242", "27",
            "Synthfilter Depth", "243", "27",
            "Synthfilter E.Sens", "244", "27",
            "Synthfilter A.Time", "245", "27",
            "Synthfilter R.Time", "246", "27",
            "Synthfilter Offset", "247", "27",

            "Valve Dry/Wet", "169", "19",
            "Valve L/R Cross", "170", "19",
            "Valve Pan", "171", "19",
            "Valve Level", "172", "19",
            "Valve Drive", "173", "19",
            "Valve Dist", "174", "19",
            "Valve Presence", "175", "19",
            "Valve LPF", "176", "19",
            "Valve HPF", "177", "19",

            "VaryBand Dry/Wet", "248", "28",
            "VaryBand Tempo 1", "249", "28",
            "VaryBand LFO 1 Type", "467", "28",
            "VaryBand St.df 1", "250", "28",
            "VaryBand Tempo 2", "251", "28",
            "VaryBand LFO 2 Type", "468", "28",
            "VaryBand St.df 2", "252", "28",
            "VaryBand Cross 1", "253", "28",
            "VaryBand Cross 2", "254", "28",
            "VaryBand Cross 3", "255", "28",

            "Vibe Dry/Wet", "386", "45",
            "Vibe Width", "387", "45",
            "Vibe Depth", "388", "45",
            "Vibe Tempo", "389", "45",
            "Vibe Random", "390", "45",
            "Vibe Stereo Df", "391", "45",
            "Vibe Feedback", "392", "45",
            "Vibe L/R Cross", "393", "45",
            "Vibe Pan", "394", "45",

            "Vocoder Dry/Wet", "296", "35",
            "Vocoder Pan", "297", "35",
            "Vocoder Input", "298", "35",
            "Vocoder Smear", "299", "35",
            "Vocoder Q", "300", "35",
            "Vocoder Ring", "301", "35",
            "Vocoder Level", "302", "35",

            "Volume", "7", "50",

            "WahWah Dry/Wet", "28", "10",
            "WahWah Pan", "66", "10",
            "WahWah Tempo", "75", "10",
            "WahWah Random", "108", "10",
            "WahWah Stereo Df", "102", "10",
            "WahWah Depth", "1", "10",
            "WahWah Amp S.", "111", "10",
            "WahWah Amp S.I.", "112", "10",
            "WahWah Smooth", "113", "10",
        };
        for (int i = 0; i < NumParams; i++)
        {
            strcpy(efx_params[i].Nom, los_params[i * 3]);
            sscanf(los_params[i * 3 + 1], "%d", &efx_params[i].Ato);
            sscanf(los_params[i * 3 + 2], "%d", &efx_params[i].Effect);
        }
    }

    // Init Preset
    New();

    // Init Bank
    New_Bank();
    init_rkr();

}

RKR::~RKR()
{
    /* To clean up valgrind log */

    delete DC_Offsetl;
    delete DC_Offsetr;
    delete M_Metronome;
    delete efx_Chorus;
    delete efx_Flanger;
    delete efx_Rev;
    delete efx_Echo;
    delete efx_Phaser;
    delete efx_APhaser;
    delete efx_Distorsion;
    delete efx_Overdrive;
    delete efx_EQ2;
    delete efx_EQ1;
    delete efx_Compressor;
    delete efx_WhaWha;
    delete efx_Alienwah;
    delete efx_Cabinet;
    delete efx_Pan;
    delete efx_Har;
    delete efx_MusDelay;
    delete efx_Gate;
    delete efx_Derelict;
    delete efx_FLimiter;
    delete efx_Valve;
    delete efx_DFlange;
    delete efx_Ring;
    delete efx_Exciter;
    delete efx_DistBand;
    delete efx_Arpie;
    delete efx_Expander;
    delete efx_Shuffle;
    delete efx_Synthfilter;
    delete efx_VaryBand;
    delete efx_Convol;
    delete efx_Looper;
    delete efx_MuTroMojo;
    delete efx_Echoverse;
    delete efx_CoilCrafter;
    delete efx_ShelfBoost;
    delete efx_Vocoder;
    delete efx_Sustainer;
    delete efx_Sequence;
    delete efx_Shifter;
    delete efx_StompBox;
    delete efx_Reverbtron;
    delete efx_Echotron;
    delete efx_StereoHarm;
    delete efx_CompBand;
    delete efx_Opticaltrem;
    delete efx_Vibe;
    delete efx_Infinity;

    delete U_Resample;
    delete D_Resample;
    delete A_Resample;

    delete beat;
    delete efx_Tuner;
    delete efx_MIDIConverter;
    delete HarmRecNote;
    delete StHarmRecNote;
    delete RingRecNote;
    delete RC_Harm;
    delete RC_Stereo_Harm;

    delete Fpre;

    free(efxoutl);
    free(efxoutr);
    free(auxdata);
    free(auxresampled);
    free(anall);
    free(analr);
    free(smpl);
    free(smpr);
    free(m_ticks);
    free(interpbuf);
    free(Preset_Name);
    free(Author);
    free(Bank_Saved);
    free(UserRealName);

    // alsa
    snd_seq_close(midi_in);
};

void
RKR::init_rkr()
{
    Tuner_Bypass = 0;
    MIDIConverter_Bypass = 0;
    Metro_Bypass = 0;
    Tap_Bypass = 0;
    Tap_Updated = 0;
    t_timeout = 0;
    ACI_Bypass = 0;
    Selected_Preset = 1;

    efx_FLimiter->setpreset(0, 4);

    val_sum = 0.0f;
    old_il_sum = -0.0f;
    old_ir_sum = -0.0f;

    old_vl_sum = -0.0f;
    old_vr_sum = -0.0f;

    old_a_sum = -0.0f;
    val_a_sum = -0.0f;

    val_il_sum = -0.0f;
    val_ir_sum = -0.0f;

    val_vl_sum = -0.0f;
    val_vr_sum = -0.0f;

    last_auxvalue = 0;
    note_old = 0;
    nfreq_old = 0;
    afreq_old = 0;
    cents_old = 0;

    MidiCh = 0;
    RControl = 0;
    ControlGet = 0;

    help_displayed = 0;
    modified = 0;

    tempocnt = 0;
    
    for (int i = 0; i < 6; i++)
    {
        tempobuf[i] = 0;
    }
    
    Tap_timeB = 0;
    Tap_Display = 0;
    Tap_Selection = 0;
    Tap_TempoSet = 90;

    // Load Preset Bank File
    char temp[128];
    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/Default.rkrb", DATADIR);
    rakarrack.get(PrefNom("Bank Filename"), BankFilename, temp, 127);
    loadnames();

    if (commandline == 0)
    {
        loadbank(BankFilename);
        a_bank = 3;
    }
    
    RC_Harm->cleanup();
    RC_Stereo_Harm->cleanup();
    HarmRecNote->reconota = -1;
    StHarmRecNote->reconota = -1;
    RingRecNote->reconota = -1;
}

void
RKR::Adjust_Upsample()
{
    if (upsample)
    {
        sample_rate = J_SAMPLE_RATE * (UpAmo + 2);
        period = J_PERIOD * (UpAmo + 2);
        u_up = (double) UpAmo + 2.0;
        u_down = 1.0 / u_up;
    }
    else
    {
        sample_rate = J_SAMPLE_RATE;
        period = J_PERIOD;
    }

    fSample_rate = (float) sample_rate;
    cSample_rate = 1.0f / (float) sample_rate;
    fPeriod = float(period);
    t_periods = J_SAMPLE_RATE / 12 / J_PERIOD;

}

void
RKR::ConnectMIDI()
{
    // Get config settings and init settings
    // Get MIDI IN Setting

    rakarrack.get(PrefNom("Auto Connect MIDI IN"), aconnect_MI, 0);
    rakarrack.get(PrefNom("MIDI IN Device"), MID, "", 40);
    
    if (aconnect_MI)
        Conecta();
}

void
RKR::EQ1_setpreset(int npreset)
{
    const int PRESET_SIZE = 12;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
        //Pop
        {78, 71, 64, 64, 64, 64, 64, 64, 71, 80, 64, 42},
        //Jazz
        {71, 68, 64, 64, 64, 64, 64, 64, 66, 69, 64, 40}
    };

    if (npreset >= NUM_PRESETS)
    {
        Fpre->ReadPreset(0, npreset - NUM_PRESETS + 1, pdata);
        for (int n = 0; n < 10; n++)
            efx_EQ1->changepar(n * 5 + 12, pdata[n]);
        efx_EQ1->changepar(0, pdata[10]);
        for (int n = 0; n < 10; n++)
            efx_EQ1->changepar(n * 5 + 13, pdata[11]);
    }
    else
    {
        for (int n = 0; n < 10; n++)
            efx_EQ1->changepar(n * 5 + 12, presets[npreset][n]);
        
        efx_EQ1->changepar(0, presets[npreset][10]);
        
        for (int n = 0; n < 10; n++)
            efx_EQ1->changepar(n * 5 + 13, presets[npreset][11]);
    }
}

void
RKR::EQ2_setpreset(int npreset)
{
    const int PRESET_SIZE = 10;
    const int NUM_PRESETS = 3;
    int pdata[MAX_PDATA_SIZE];
    int presets[NUM_PRESETS][PRESET_SIZE] = {
        //Plain
        {200, 64, 64, 800, 64, 64, 12000, 64, 64, 64},
        //Pop
        {72, 73, 45, 1077, 64, 64, 8111, 69, 38, 64},
        //Jazz
        {72, 71, 38, 1077, 64, 64, 10580, 69, 38, 64}
    };


    if (npreset >= NUM_PRESETS)
    {
        Fpre->ReadPreset(9, npreset - NUM_PRESETS + 1, pdata);
        
        for (int n = 0; n < 3; n++)
        {
            efx_EQ2->changepar(n * 5 + 11, pdata[n * 3]);
            efx_EQ2->changepar(n * 5 + 12, pdata[n * 3 + 1]);
            efx_EQ2->changepar(n * 5 + 13, pdata[n * 3 + 2]);
        }
        
        efx_EQ2->changepar(0, pdata[9]);
    }
    else
    {
        for (int n = 0; n < 3; n++)
        {
            efx_EQ2->changepar(n * 5 + 11, presets[npreset][n * 3]);
            efx_EQ2->changepar(n * 5 + 12, presets[npreset][n * 3 + 1]);
            efx_EQ2->changepar(n * 5 + 13, presets[npreset][n * 3 + 2]);
        }
        
        efx_EQ2->changepar(0, presets[npreset][9]);
    }
}

void
RKR::add_metro()
{
    for (int i = 0; i < period; i++)
    {
        efxoutl[i] += m_ticks[i] * M_Metro_Vol;
        efxoutr[i] += m_ticks[i] * M_Metro_Vol;
    }
}

void
RKR::Vol2_Efx()
{
    memcpy(smpl, efxoutl, period * sizeof (float));
    memcpy(smpr, efxoutr, period * sizeof (float));
}

void
RKR::Vol3_Efx()
{
    float att = 2.0f;

    for (int i = 0; i < period; i++)
    {
        efxoutl[i] *= att;
        efxoutr[i] *= att;
    }

    Vol2_Efx();
}

void
RKR::Vol_Efx(int NumEffect, float volume)
{
    float v1, v2; v1 = v2 = 1.0f;

    if (volume < 0.5f)
    {
        v1 = 1.0f;
        v2 = volume * 2.0f;
    }
    else
    {
        v1 = (1.0f - volume) * 2.0f;
        v2 = 1.0f;
    }

    if ((NumEffect == 8) || (NumEffect == 15))
    {
        v2 *= v2;
    }

    for (int i = 0; i < period; i++)
    {
        efxoutl[i] = smpl[i] * v2 + efxoutl[i] * v1;
        efxoutr[i] = smpr[i] * v2 + efxoutr[i] * v1;
    }

    Vol2_Efx();
}

void
RKR::calculavol(int i)
{
    if (i == 1)
    {
        Log_I_Gain = powf(Input_Gain * 2.0f, 4);
    }
    
    if (i == 2)
    {
        Log_M_Volume = powf(Master_Volume * 2.0f, 4);
    }
}

int
RKR::checkforaux()
{
    for (int i = 0; i < 10; i++)
    {
        if (efx_order[i] == 35)
        {
            if (Vocoder_Bypass) return (1);
        }
    }

    return (0);
}

void
RKR::Control_Gain(float *origl, float *origr)
{
    float il_sum = 1e-12f;
    float ir_sum = 1e-12f;
    float a_sum = 1e-12f;
    float tmp = 0.0;

    if (upsample)
    {
        U_Resample->out(origl, origr, efxoutl, efxoutr, J_PERIOD, u_up);
        if ((checkforaux()) || (ACI_Bypass))
        {
            A_Resample->mono_out(auxdata, auxresampled, J_PERIOD, u_up, period);
        }
    }
    else if ((checkforaux()) || (ACI_Bypass))
    {
        memcpy(auxresampled, auxdata, sizeof (float)*J_PERIOD);
    }

    if (DC_Offset)
    {
        DC_Offsetl->filterout(efxoutl, period);
        DC_Offsetr->filterout(efxoutr, period);
    }

    for (int i = 0; i < period; i++)
    {
        efxoutl[i] *= Log_I_Gain;
        efxoutr[i] *= Log_I_Gain;
        tmp = fabsf(efxoutr[i]);
        
        if (tmp > ir_sum)
        {
            ir_sum = tmp;
        }
        
        tmp = fabsf(efxoutl[i]);
        
        if (tmp > il_sum)
        {
            il_sum = tmp;
        }
    }
    
    memcpy(smpl, efxoutl, sizeof (float)*period);
    memcpy(smpr, efxoutr, sizeof (float)*period);

    float temp_sum = (float) CLAMP(rap2dB(il_sum), -48.0, 15.0);
    val_il_sum = .6f * old_il_sum + .4f * temp_sum;

    temp_sum = (float) CLAMP(rap2dB(ir_sum), -48.0, 15.0);
    val_ir_sum = .6f * old_ir_sum + .4f * temp_sum;

    val_sum = val_il_sum + val_ir_sum;


    if ((ACI_Bypass) && (Aux_Source == 0))
    {
        temp_sum = 0.0;
        tmp = 0.0;
        for (int i = 0; i < period; i++)
        {
            tmp = fabsf(auxresampled[i]);
            
            if (tmp > a_sum)
            {
                a_sum = tmp;
            }
        }

        val_a_sum = .6f * old_a_sum + .4f * a_sum;
        old_a_sum = val_a_sum;
    }
}

void
RKR::Control_Volume(float *origl, float *origr)
{
    float il_sum = 1e-12f;
    float ir_sum = 1e-12f;

    float tmp = 0.0f;
    float Temp_M_Volume = 0.0f;

    if ((flpos)&&(have_signal))
    {
        if (db6booster)
        { // +6dB Final Limiter in settings/audio
            for (int i = 0; i < period; i++)
            {
                efxoutl[i] *= .5f;
                efxoutr[i] *= .5f;
            }
        }

        efx_FLimiter->out(efxoutl, efxoutr);

        if (db6booster)
        {
            for (int i = 0; i < period; i++)
            {
                efxoutl[i] *= 2.0f;
                efxoutr[i] *= 2.0f;
            }
        }
    }

    memcpy(anall, efxoutl, sizeof (float)* period);
    memcpy(analr, efxoutr, sizeof (float)* period);

    if (upsample)
    {
        D_Resample->out(anall, analr, efxoutl, efxoutr, period, u_down);
    }

    if (OnCounter < t_periods)
    {
        Temp_M_Volume = Log_M_Volume / (float) (t_periods - OnCounter);
        OnCounter++;
    }
    else
    {
        Temp_M_Volume = Log_M_Volume;
    }

    for (int i = 0; i < period; i++)
    { //control volume

        efxoutl[i] *= Temp_M_Volume*booster; // +10dB booster main window
        efxoutr[i] *= Temp_M_Volume*booster;

        if (Fraction_Bypass < 1.0f)
        { // FX% main window
            efxoutl[i] = (origl[i] * (1.0f - Fraction_Bypass) + efxoutl[i] * Fraction_Bypass);
            efxoutr[i] = (origr[i] * (1.0f - Fraction_Bypass) + efxoutr[i] * Fraction_Bypass);
        }

        tmp = fabsf(efxoutl[i]);
        
        if (tmp > il_sum)
        {
            il_sum = tmp;
        }
        
        tmp = fabsf(efxoutr[i]);
        
        if (tmp > ir_sum)
        {
            ir_sum = tmp;
        }
    }

    if ((!flpos) && (have_signal))
    {
        if (db6booster)
        {
            for (int i = 0; i < period; i++)
            {
                efxoutl[i] *= .5f;
                efxoutr[i] *= .5f;
            }
        }

        efx_FLimiter->out(efxoutl, efxoutr); //then limit final output

        if (db6booster)
        {
            for (int i = 0; i < period; i++)
            {
                efxoutl[i] *= 2.0f;
                efxoutr[i] *= 2.0f;
            }
        }
    }

    for (int i = 0; i < period; i++)
    {
        tmp = fabsf(efxoutl[i]);
        
        if (tmp > il_sum)
        {
            il_sum = tmp;
        }
        
        tmp = fabsf(efxoutr[i]);
        
        if (tmp > ir_sum)
        {
            ir_sum = tmp;
        }
    }

    float temp_sum = (float) CLAMP(rap2dB(il_sum), -48, 15);
    val_vl_sum = .6f * old_vl_sum + .4f * temp_sum;
    temp_sum = (float) CLAMP(rap2dB(ir_sum), -48, 15);
    val_vr_sum = .6f * old_vr_sum + .4f * temp_sum;

    if ((il_sum + ir_sum) > 0.0004999f)
    {
        have_signal = 1;
    }
    else
    {
        have_signal = 0;
    }

}

void
RKR::cleanup_efx()
{
    efx_EQ1->cleanup();
    efx_Rev->cleanup();
    efx_Distorsion->cleanup();
    efx_Overdrive->cleanup();
    efx_Compressor->cleanup();
    efx_Echo->cleanup();
    efx_Chorus->cleanup();
    efx_Flanger->cleanup();
    efx_Phaser->cleanup();
    efx_EQ2->cleanup();
    efx_WhaWha->cleanup();
    efx_Alienwah->cleanup();
    efx_Cabinet->cleanup();
    efx_Pan->cleanup();
    efx_Har->cleanup();
    efx_MusDelay->cleanup();
    efx_Gate->cleanup();
    efx_Derelict->cleanup();
    efx_APhaser->cleanup();
    efx_Valve->cleanup();
    efx_DFlange->cleanup();
    efx_Ring->cleanup();
    efx_Exciter->cleanup();
    efx_DistBand->cleanup();
    efx_Arpie->cleanup();
    efx_Expander->cleanup();
    efx_Shuffle->cleanup();
    efx_Synthfilter->cleanup();
    efx_VaryBand->cleanup();
    efx_Convol->cleanup();
    efx_Looper->cleanup();
    efx_MuTroMojo->cleanup();
    efx_Echoverse->cleanup();
    efx_CoilCrafter->cleanup();
    efx_ShelfBoost->cleanup();
    efx_Vocoder->cleanup();
    efx_Sustainer->cleanup();
    efx_Sequence->cleanup();
    efx_Shifter->cleanup();
    efx_StompBox->cleanup();
    efx_Reverbtron->cleanup();
    efx_Echotron->cleanup();
    efx_StereoHarm->cleanup();
    efx_CompBand->cleanup();
    efx_Opticaltrem->cleanup();
    efx_Vibe->cleanup();
    RC_Harm->cleanup();
    RC_Stereo_Harm->cleanup();
    efx_FLimiter->cleanup();
    efx_Infinity->cleanup();

}

void
RKR::Alg(float *origl, float *origr, void *)
{

    if ((t_timeout) && (Tap_Bypass))
    {
        TapTempo_Timeout(1);
    }

    if (Bypass)
    {
        Control_Gain(origl, origr);

        if (Metro_Bypass)
        {
            M_Metronome->metronomeout(m_ticks, period);
        }

        if ((Tap_Bypass) && (Tap_Selection == 4))
        {
            beat->detect(efxoutl, efxoutr, period);
            int bt_tempo = lrintf(beat->get_tempo());
            
            if ((bt_tempo > 19) && (bt_tempo < 360) && (bt_tempo != Tap_TempoSet))
            {
                Tap_TempoSet = bt_tempo;
                Update_tempo();
                Tap_Display = 1;
            }
        }

        if (Tuner_Bypass)
        {
            efx_Tuner->schmittFloat(period, efxoutl, efxoutr, HarmRecNote->freqs, HarmRecNote->lfreqs);
        }

        if (MIDIConverter_Bypass)
        {
            if (efx_MIDIConverter->getpar(5))
            {
                efx_MIDIConverter->fftFloat(efxoutl, efxoutr, val_sum, HarmRecNote->freqs, HarmRecNote->lfreqs);
            }
            else
            {
                efx_MIDIConverter->schmittFloat(efxoutl, efxoutr, val_sum, HarmRecNote->freqs, HarmRecNote->lfreqs);
            }
        }

        if ((Harmonizer_Bypass) && (have_signal))
        {
            if (efx_Har->mira)
            {
                if ((efx_Har->PMIDI) || (efx_Har->PSELECT))
                {
                    HarmRecNote->schmittFloat(efxoutl, efxoutr);
                    if ((HarmRecNote->reconota != -1) && (HarmRecNote->reconota != HarmRecNote->last))
                    {
                        if (HarmRecNote->afreq > 0.0)
                        {
                            RC_Harm->Vamos(0, efx_Har->Pinterval - 12, HarmRecNote->reconota);
                            efx_Har->r_ratio = RC_Harm->r__ratio[0]; //pass the found ratio
                            HarmRecNote->last = HarmRecNote->reconota;
                        }
                    }
                }
            }
        }


        if ((StereoHarm_Bypass) && (have_signal))
        {
            if (efx_StereoHarm->mira)
            {
                if ((efx_StereoHarm->PMIDI) || (efx_StereoHarm->PSELECT))
                {
                    StHarmRecNote->schmittFloat(efxoutl, efxoutr);
                    if ((StHarmRecNote->reconota != -1) && (StHarmRecNote->reconota != StHarmRecNote->last))
                    {
                        if (StHarmRecNote->afreq > 0.0)
                        {
                            RC_Stereo_Harm->Vamos(1, efx_StereoHarm->Pintervall - 12, StHarmRecNote->reconota);
                            RC_Stereo_Harm->Vamos(2, efx_StereoHarm->Pintervalr - 12, StHarmRecNote->reconota);
                            efx_StereoHarm->r_ratiol = RC_Stereo_Harm->r__ratio[1];
                            efx_StereoHarm->r_ratior = RC_Stereo_Harm->r__ratio[2];
                            StHarmRecNote->last = StHarmRecNote->reconota;
                        }
                    }
                }
            }
        }


        if ((Ring_Bypass) && (efx_Ring->Pafreq))
        {
            RingRecNote->schmittFloat(efxoutl, efxoutr);
            if ((RingRecNote->reconota != -1) && (RingRecNote->reconota != RingRecNote->last))
            {
                if (RingRecNote->afreq > 0.0)
                {
                    efx_Ring->Pfreq = lrintf(RingRecNote->lafreq);
                    StHarmRecNote->last = StHarmRecNote->reconota;
                }
            }
        }


        for (int i = 0; i < 10; i++)
        {
            switch (efx_order[i])
            {
            case 0:
                if (EQ1_Bypass)
                {
                    efx_EQ1->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 1:
                if (Compressor_Bypass)
                {
                    efx_Compressor->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 5:
                if (Chorus_Bypass)
                {
                    efx_Chorus->out(efxoutl, efxoutr);
                    Vol_Efx(5, efx_Chorus->outvolume);
                }
                break;

            case 7:
                if (Flanger_Bypass)
                {
                    efx_Flanger->out(efxoutl, efxoutr);
                    Vol_Efx(7, efx_Flanger->outvolume);
                }
                break;

            case 6:
                if (Phaser_Bypass)
                {
                    efx_Phaser->out(efxoutl, efxoutr);
                    Vol_Efx(6, efx_Phaser->outvolume);
                }
                break;

            case 2:
                if (Distorsion_Bypass)
                {
                    efx_Distorsion->out(efxoutl, efxoutr);
                    Vol_Efx(2, efx_Distorsion->outvolume);
                }
                break;

            case 3:
                if (Overdrive_Bypass)
                {
                    efx_Overdrive->out(efxoutl, efxoutr);
                    Vol_Efx(3, efx_Overdrive->outvolume);
                }
                break;

            case 4:
                if (Echo_Bypass)
                {
                    efx_Echo->out(efxoutl, efxoutr);
                    Vol_Efx(4, efx_Echo->outvolume);
                }
                break;
            case 8:
                if (Reverb_Bypass)
                {
                    efx_Rev->out(efxoutl, efxoutr);
                    Vol_Efx(8, efx_Rev->outvolume);
                }
                break;

            case 9:
                if (EQ2_Bypass)
                {
                    efx_EQ2->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 10:
                if (WhaWha_Bypass)
                {
                    efx_WhaWha->out(efxoutl, efxoutr);
                    Vol_Efx(10, efx_WhaWha->outvolume);
                }
                break;

            case 11:
                if (Alienwah_Bypass)
                {
                    efx_Alienwah->out(efxoutl, efxoutr);
                    Vol_Efx(11, efx_Alienwah->outvolume);
                }
                break;

            case 12:
                if (Cabinet_Bypass)
                {
                    efx_Cabinet->out(efxoutl, efxoutr);
                    Vol3_Efx();
                }

                break;

            case 13:
                if (Pan_Bypass)
                {
                    efx_Pan->out(efxoutl, efxoutr);
                    Vol_Efx(13, efx_Pan->outvolume);
                }
                break;

            case 14:
                if (Harmonizer_Bypass)
                {
                    efx_Har->out(efxoutl, efxoutr);
                    Vol_Efx(14, efx_Har->outvolume);
                }
                break;

            case 15:
                if (MusDelay_Bypass)
                {
                    efx_MusDelay->out(efxoutl, efxoutr);
                    Vol_Efx(15, efx_MusDelay->outvolume);
                }
                break;

            case 16:
                if (Gate_Bypass)
                {
                    efx_Gate->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 17:
                if (Derelict_Bypass)
                {
                    efx_Derelict->out(efxoutl, efxoutr);
                    Vol_Efx(17, efx_Derelict->outvolume);
                }
                break;

            case 18:
                if (APhaser_Bypass)
                {
                    efx_APhaser->out(efxoutl, efxoutr);
                    Vol_Efx(18, efx_APhaser->outvolume);
                }
                break;

            case 19:
                if (Valve_Bypass)
                {
                    efx_Valve->out(efxoutl, efxoutr);
                    Vol_Efx(19, efx_Valve->outvolume);
                }
                break;

            case 20:
                if (DFlange_Bypass)
                {
                    efx_DFlange->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 21:
                if (Ring_Bypass)
                {
                    efx_Ring->out(efxoutl, efxoutr);
                    Vol_Efx(21, efx_Ring->outvolume);
                }
                break;

            case 22:
                if (Exciter_Bypass)
                {
                    efx_Exciter->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 23:
                if (DistBand_Bypass)
                {
                    efx_DistBand->out(efxoutl, efxoutr);
                    Vol_Efx(23, efx_DistBand->outvolume);
                }
                break;

            case 24:
                if (Arpie_Bypass)
                {
                    efx_Arpie->out(efxoutl, efxoutr);
                    Vol_Efx(24, efx_Arpie->outvolume);
                }
                break;

            case 25:
                if (Expander_Bypass)
                {
                    efx_Expander->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 26:
                if (Shuffle_Bypass)
                {
                    efx_Shuffle->out(efxoutl, efxoutr);
                    Vol_Efx(26, efx_Shuffle->outvolume);
                }
                break;

            case 27:
                if (Synthfilter_Bypass)
                {
                    efx_Synthfilter->out(efxoutl, efxoutr);
                    Vol_Efx(27, efx_Synthfilter->outvolume);
                }
                break;

            case 28:
                if (VaryBand_Bypass)
                {
                    efx_VaryBand->out(efxoutl, efxoutr);
                    Vol_Efx(28, efx_VaryBand->outvolume);
                }
                break;

            case 29:
                if (Convol_Bypass)
                {
                    efx_Convol->out(efxoutl, efxoutr);
                    Vol_Efx(29, efx_Convol->outvolume);
                }
                break;

            case 30:
                if (Looper_Bypass)
                {
                    efx_Looper->out(efxoutl, efxoutr);
                    Vol_Efx(30, efx_Looper->outvolume);
                }
                break;

            case 31:
                if (MuTroMojo_Bypass)
                {
                    efx_MuTroMojo->out(efxoutl, efxoutr);
                    Vol_Efx(31, efx_MuTroMojo->outvolume);
                }
                break;

            case 32:
                if (Echoverse_Bypass)
                {
                    efx_Echoverse->out(efxoutl, efxoutr);
                    Vol_Efx(32, efx_Echoverse->outvolume);
                }
                break;

            case 33:
                if (CoilCrafter_Bypass)
                {
                    efx_CoilCrafter->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 34:
                if (ShelfBoost_Bypass)
                {
                    efx_ShelfBoost->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 35:
                if (Vocoder_Bypass)
                {
                    efx_Vocoder->out(efxoutl, efxoutr);
                    Vol_Efx(35, efx_Vocoder->outvolume);
                }
                break;

            case 36:
                if (Sustainer_Bypass)
                {
                    efx_Sustainer->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 37:
                if (Sequence_Bypass)
                {
                    efx_Sequence->out(efxoutl, efxoutr);
                    Vol_Efx(37, efx_Sequence->outvolume);
                }
                break;

            case 38:
                if (Shifter_Bypass)
                {
                    efx_Shifter->out(efxoutl, efxoutr);
                    Vol_Efx(38, efx_Shifter->outvolume);
                }
                break;

            case 39:
                if (StompBox_Bypass)
                {
                    efx_StompBox->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 40:
                if (Reverbtron_Bypass)
                {
                    efx_Reverbtron->out(efxoutl, efxoutr);
                    Vol_Efx(40, efx_Reverbtron->outvolume);
                }
                break;

            case 41:
                if (Echotron_Bypass)
                {
                    efx_Echotron->out(efxoutl, efxoutr);
                    Vol_Efx(41, efx_Echotron->outvolume);
                }
                break;

            case 42:
                if (StereoHarm_Bypass)
                {
                    efx_StereoHarm->out(efxoutl, efxoutr);
                    Vol_Efx(42, efx_StereoHarm->outvolume);
                }
                break;

            case 43:
                if (CompBand_Bypass)
                {
                    efx_CompBand->out(efxoutl, efxoutr);
                    Vol_Efx(43, efx_CompBand->outvolume);
                }
                break;

            case 44:
                if (Opticaltrem_Bypass)
                {
                    efx_Opticaltrem->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case 45:
                if (Vibe_Bypass)
                {
                    efx_Vibe->out(efxoutl, efxoutr);
                    Vol_Efx(45, efx_Vibe->outvolume);
                }
                break;

            case 46:
                if (Infinity_Bypass)
                {
                    efx_Infinity->out(efxoutl, efxoutr);
                    Vol_Efx(46, efx_Infinity->outvolume);
                }
            }
        }

        if (Metro_Bypass)
        {
            add_metro();
        }

        Control_Volume(origl, origr);
    }
}
