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

RKR::RKR() :
    efx_Compressor(NULL),

    efx_Tuner(NULL),
    efx_MIDIConverter(NULL),
    M_Metronome(NULL),
    beat(NULL),
    HarmRecNote(NULL),
    StHarmRecNote(NULL),
    RingRecNote(NULL),
    RC_Harm(NULL),
    RC_Stereo_Harm(NULL),
    EFX_Bypass(),
    EFX_Bank_Bypass(),
    efx_FLimiter(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    A_Resample(NULL),
    DC_Offsetl(NULL),
    DC_Offsetr(NULL),
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
    MIDIConverter_Bypass(0),
    Metro_Bypass(0),
    Tuner_Bypass(0),
    Tap_Bypass(0),
    ACI_Bypass(0),
    Bypass_B(),
    Selected_Preset(1),
    lv(),
    saved_order(),
    efx_order(),
    new_order(),
    availables(),
    active(),
    MidiCh(0),
    HarCh(),
    StereoHarCh(),
    init_state(),
    actuvol(),
    help_displayed(0),
    modified(0),
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
    RControl(0),
    ControlGet(0),
    CountWait(),
    XUserMIDI(),
    quality_update(false),
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
    Dist_res_amount(),
    Dist_up_q(),
    Dist_down_q(),
    Ovrd_res_amount(),
    Ovrd_up_q(),
    Ovrd_down_q(),
    Dere_res_amount(),
    Dere_up_q(),
    Dere_down_q(),
    DBand_res_amount(),
    DBand_up_q(),
    DBand_down_q(),
    Stomp_res_amount(),
    Stomp_up_q(),
    Stomp_down_q(),
    sw_stat(),
    MIDIway(),
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
    SeqQual(),
    ShiQual(),
    tempocnt(0),
    Tap_Display(0),
    Tap_Selection(0),
    Tap_TempoSet(90),
    Tap_SetValue(),
    t_timeout(0),
    jt_state(),
    Tap_Updated(0),
    note_old(0),
    cents_old(0),
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
    last_auxvalue(0),
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
    Tap_timeB(0),
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
    val_sum(0.0f),
    old_il_sum(-0.0f),
    old_ir_sum(-0.0f),
    old_vl_sum(-0.0f),
    old_vr_sum(-0.0f),
    val_vl_sum(-0.0f),
    val_vr_sum(-0.0f),
    val_il_sum(-0.0f),
    val_ir_sum(-0.0f),
    old_a_sum(-0.0f),
    val_a_sum(-0.0f),
    bogomips(0.0f),
    looper_size(),
    nfreq_old(0),
    afreq_old(0),
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
    if(!jack_open_client())
    {
        return; // If we don't have a jack client then quit with message
    }

    load_user_preferences();

    Get_Bogomips();

    initialize_arrays();

    instantiate_effects();

    put_order_in_rack();
    
    MIDI_control();
    
    // Initialize Preset
    New();

    // Initialize Bank
    New_Bank();
    
    // Loads the banks preset names and information for the bank manager window.
    loadnames();

    // If no bank is listed from the command line, then load the default user bank
    // in Settings/Preferences/Bank/ Bank Filename
    if (commandline == 0)
    {
        loadbank(BankFilename);
        a_bank = 3;
    }
}

RKR::~RKR()
{
    /* To clean up valgrind log */

    delete DC_Offsetl;
    delete DC_Offsetr;
    delete M_Metronome;
    delete Rack_Effects[EFX_CHORUS];
    delete Rack_Effects[EFX_FLANGER];
    delete Rack_Effects[EFX_REVERB];
    delete Rack_Effects[EFX_ECHO];
    delete Rack_Effects[EFX_PHASER];
    delete Rack_Effects[EFX_ANALOG_PHASER];
    delete Rack_Effects[EFX_DISTORTION];
    delete Rack_Effects[EFX_OVERDRIVE];
    delete Rack_Effects[EFX_PARAMETRIC];
    delete Rack_Effects[EFX_EQ];
    delete efx_Compressor;
    delete Rack_Effects[EFX_WAHWAH];
    delete Rack_Effects[EFX_ALIENWAH];
    delete Rack_Effects[EFX_CABINET];
    delete Rack_Effects[EFX_PAN];
    delete Rack_Effects[EFX_HARMONIZER];
    delete Rack_Effects[EFX_MUSICAL_DELAY];
    delete Rack_Effects[EFX_NOISEGATE];
    delete Rack_Effects[EFX_DERELICT];
    delete efx_FLimiter;
    delete Rack_Effects[EFX_VALVE];
    delete Rack_Effects[EFX_DUAL_FLANGE];
    delete Rack_Effects[EFX_RING];
    delete Rack_Effects[EFX_EXCITER];
    delete Rack_Effects[EFX_DISTBAND];
    delete Rack_Effects[EFX_ARPIE];
    delete Rack_Effects[EFX_EXPANDER];
    delete Rack_Effects[EFX_SHUFFLE];
    delete Rack_Effects[EFX_SYNTHFILTER];
    delete Rack_Effects[EFX_VARYBAND];
    delete Rack_Effects[EFX_CONVOLOTRON];
    delete Rack_Effects[EFX_LOOPER];
    delete Rack_Effects[EFX_MUTROMOJO];
    delete Rack_Effects[EFX_ECHOVERSE];
    delete Rack_Effects[EFX_COILCRAFTER];
    delete Rack_Effects[EFX_SHELFBOOST];
    delete Rack_Effects[EFX_VOCODER];
    delete Rack_Effects[EFX_SUSTAINER];
    delete Rack_Effects[EFX_SEQUENCE];
    delete Rack_Effects[EFX_SHIFTER];
    delete Rack_Effects[EFX_STOMPBOX];
    delete Rack_Effects[EFX_REVERBTRON];
    delete Rack_Effects[EFX_ECHOTRON];
    delete Rack_Effects[EFX_STEREOHARM];
    delete Rack_Effects[EFX_COMPBAND];
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

/**
 *  Opens a jack client for this session.
 * 
 * @return 
 *      0 if client cannot be opened.
 *      1 if valid client is opened.
 */
int
RKR::jack_open_client()
{
    char temp[256];
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
        return 0;
    }

    strcpy(jackcliname, jack_get_client_name(jackclient));

    J_SAMPLE_RATE = jack_get_sample_rate(jackclient);
    J_PERIOD = jack_get_buffer_size(jackclient);
    
    return 1;
}

/**
 *  Loads the user preferences set in the Settings/Preferences tabs:
 *  Look, Audio, Quality, MIDI, Jack, Misc, Bank.
 *  These settings are saved in:
 *  /home/username/.fltk/github.com.Stazed.rakarrrack.plus/rakarrack-plus.prefs
 */
void
RKR::load_user_preferences()
{
    Fl_Preferences rakarrack(Fl_Preferences::USER, WEBSITE, PACKAGE);
    
    rakarrack.get(PrefNom("Disable Warnings"), mess_dis, 0);
    rakarrack.get(PrefNom("Filter DC Offset"), DC_Offset, 0);
    rakarrack.get(PrefNom("UpSampling"), upsample, 0);
    rakarrack.get(PrefNom("UpQuality"), UpQual, 4);
    rakarrack.get(PrefNom("DownQuality"), DownQual, 4);
    rakarrack.get(PrefNom("UpAmount"), UpAmo, 0);

    Adjust_Upsample();

    rakarrack.get(PrefNom("Looper Size"), looper_size, 1);
    rakarrack.get(PrefNom("Calibration"), aFreq, 440.0f);

    rakarrack.get(PrefNom("Vocoder Bands"), VocBands, 32);
    rakarrack.get(PrefNom("Recognize Trigger"), rtrig, .6f);

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

    rakarrack.get(PrefNom("Distortion Resampling"), Dist_res_amount, 2);
    rakarrack.get(PrefNom("Distortion Up Quality"), Dist_up_q, 4);
    rakarrack.get(PrefNom("Distortion Down Quality"), Dist_down_q, 2);

    rakarrack.get(PrefNom("Overdrive Resampling"), Ovrd_res_amount, 2);
    rakarrack.get(PrefNom("Overdrive Up Quality"), Ovrd_up_q, 4);
    rakarrack.get(PrefNom("Overdrive Down Quality"), Ovrd_down_q, 2);

    rakarrack.get(PrefNom("Derelict Resampling"), Dere_res_amount, 2);
    rakarrack.get(PrefNom("Derelict Up Quality"), Dere_up_q, 4);
    rakarrack.get(PrefNom("Derelict Down Quality"), Dere_down_q, 2);

    rakarrack.get(PrefNom("DistBand Resampling"), DBand_res_amount, 2);
    rakarrack.get(PrefNom("DistBand Up Quality"), DBand_up_q, 4);
    rakarrack.get(PrefNom("DistBand Down Quality"), DBand_down_q, 2);

    rakarrack.get(PrefNom("StompBox Resampling"), Stomp_res_amount, 2);
    rakarrack.get(PrefNom("StompBox Up Quality"), Stomp_up_q, 4);
    rakarrack.get(PrefNom("StompBox Down Quality"), Stomp_down_q, 2);

    rakarrack.get(PrefNom("Harmonizer Quality"), HarQual, 4);
    rakarrack.get(PrefNom("StereoHarm Quality"), SteQual, 4);
    
    rakarrack.get(PrefNom("Sequence Quality"), SeqQual, 4);
    rakarrack.get(PrefNom("Shifter Quality"), ShiQual, 4);

    rakarrack.get(PrefNom("Auto Connect Jack"), aconnect_JA, 1);
    
    // Default on new installs - don't connect input - can cause feedback loop = 0
    rakarrack.get(PrefNom("Auto Connect Jack In"), aconnect_JIA, 0);

    rakarrack.get(PrefNom("Auto Connect Num"), cuan_jack, 2);
    rakarrack.get(PrefNom("Auto Connect In Num"), cuan_ijack, 1);

    char temp[256];
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
    
    // Get user default bank file from Settings/Bank/ --Bank Filename
    memset(temp, 0, sizeof (temp));
    sprintf(temp, "%s/Default.rkrb", DATADIR);
    rakarrack.get(PrefNom("Bank Filename"), BankFilename, temp, 127);
}

void
RKR::instantiate_effects()
{
    DC_Offsetl = new AnalogFilter(1, 20, 1, 0, sample_rate, interpbuf);
    DC_Offsetr = new AnalogFilter(1, 20, 1, 0, sample_rate, interpbuf);
    M_Metronome = new metronome(fSample_rate, period);
    Rack_Effects[EFX_CHORUS] = new Chorus(fSample_rate, period);
    Rack_Effects[EFX_FLANGER] = new Flanger(fSample_rate, period);
    Rack_Effects[EFX_REVERB] = new Reverb(fSample_rate, period);
    Rack_Effects[EFX_ECHO] = new Echo(fSample_rate, period);
    Rack_Effects[EFX_PHASER] = new Phaser(fSample_rate, period);
    Rack_Effects[EFX_ANALOG_PHASER] = new Analog_Phaser(fSample_rate, period);
    Rack_Effects[EFX_DISTORTION] = new Distorsion(Dist_res_amount, Dist_up_q, Dist_down_q, fSample_rate, period);
    Rack_Effects[EFX_OVERDRIVE] = new Overdrive(Ovrd_res_amount, Ovrd_up_q, Ovrd_down_q, fSample_rate, period);
    Rack_Effects[EFX_PARAMETRIC] = new ParametricEQ(fSample_rate, period);
    Rack_Effects[EFX_EQ] = new EQ( fSample_rate, period);
    efx_Compressor = new Compressor(fSample_rate, period);
    Rack_Effects[EFX_WAHWAH] = new WahWah(fSample_rate, period);
    Rack_Effects[EFX_ALIENWAH] = new Alienwah(fSample_rate, period);
    Rack_Effects[EFX_CABINET] = new Cabinet(fSample_rate, period);
    Rack_Effects[EFX_PAN] = new Pan(fSample_rate, period);
    Rack_Effects[EFX_HARMONIZER] = new Harmonizer((long) HarQual, Har_Down, Har_U_Q, Har_D_Q, fSample_rate, period);
    Rack_Effects[EFX_MUSICAL_DELAY] = new MusicDelay(fSample_rate, period);
    Rack_Effects[EFX_NOISEGATE] = new Gate(fSample_rate, period);
    Rack_Effects[EFX_DERELICT] = new Derelict(Dere_res_amount, Dere_up_q, Dere_down_q, fSample_rate, period);
    efx_FLimiter = new Compressor(fSample_rate, period);
    Rack_Effects[EFX_VALVE] = new Valve(fSample_rate, period);
    Rack_Effects[EFX_DUAL_FLANGE] = new Dflange(fSample_rate, period);
    Rack_Effects[EFX_RING] = new Ring(fSample_rate, period);
    Rack_Effects[EFX_EXCITER] = new Exciter(fSample_rate, period);
    Rack_Effects[EFX_DISTBAND] = new DistBand(DBand_res_amount, DBand_up_q, DBand_down_q, fSample_rate, period);
    Rack_Effects[EFX_ARPIE] = new Arpie(fSample_rate, period);
    Rack_Effects[EFX_EXPANDER] = new Expander(fSample_rate, period);
    Rack_Effects[EFX_SHUFFLE] = new Shuffle(fSample_rate, period);
    Rack_Effects[EFX_SYNTHFILTER] = new Synthfilter(fSample_rate, period);
    Rack_Effects[EFX_VARYBAND] = new VaryBand(fSample_rate, period);
    Rack_Effects[EFX_CONVOLOTRON] = new Convolotron(Con_Down, Con_U_Q, Con_D_Q, fSample_rate, period);
    Rack_Effects[EFX_LOOPER] = new Looper(looper_size, fSample_rate, period);
    Rack_Effects[EFX_MUTROMOJO] = new MuTroMojo(fSample_rate, period);
    Rack_Effects[EFX_ECHOVERSE] = new Echoverse(fSample_rate, period);
    Rack_Effects[EFX_COILCRAFTER] = new CoilCrafter(fSample_rate, period);
    Rack_Effects[EFX_SHELFBOOST] = new ShelfBoost(fSample_rate, period);
    Rack_Effects[EFX_VOCODER] = new Vocoder(auxresampled, VocBands, Voc_Down, Voc_U_Q, Voc_D_Q, fSample_rate, period);
    Rack_Effects[EFX_SUSTAINER] = new Sustainer(fSample_rate, period);
    Rack_Effects[EFX_SEQUENCE] = new Sequence((long) SeqQual, Seq_Down, Seq_U_Q, Seq_D_Q, fSample_rate, period);
    Rack_Effects[EFX_SHIFTER] = new Shifter((long) ShiQual, Shi_Down, Shi_U_Q, Shi_D_Q, fSample_rate, period);
    Rack_Effects[EFX_STOMPBOX] = new StompBox(Stomp_res_amount, Stomp_up_q, Stomp_down_q, fSample_rate, period);
    Rack_Effects[EFX_REVERBTRON] = new Reverbtron(Rev_Down, Rev_U_Q, Rev_D_Q, fSample_rate, period);
    Rack_Effects[EFX_ECHOTRON] = new Echotron(fSample_rate, period);
    Rack_Effects[EFX_STEREOHARM] = new StereoHarm((long) SteQual, Ste_Down, Ste_U_Q, Ste_D_Q, fSample_rate, period);
    Rack_Effects[EFX_COMPBAND] = new CompBand(fSample_rate, period);
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
    
    // Defaults and cleanup
    efx_FLimiter->setpreset(0, 4);
    RC_Harm->cleanup();
    RC_Stereo_Harm->cleanup();
    HarmRecNote->reconota = -1;
    StHarmRecNote->reconota = -1;
    RingRecNote->reconota = -1;
}

void
RKR::initialize_arrays()
{
    efxoutl = (float *) malloc(sizeof (float) * period);
    efxoutr = (float *) malloc(sizeof (float) * period);

    smpl = (float *) malloc(sizeof (float) * period);
    smpr = (float *) malloc(sizeof (float) * period);

    anall = (float *) malloc(sizeof (float) * period);
    analr = (float *) malloc(sizeof (float) * period);

    auxdata = (float *) malloc(sizeof (float) * period);
    auxresampled = (float *) malloc(sizeof (float) * period);

    m_ticks = (float *) malloc(sizeof (float) * period);

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

    Preset_Name = (char *) malloc(sizeof (char) * 64);
    memset(Preset_Name, 0, sizeof (char) * 64);
    Author = (char *) malloc(sizeof (char) * 64);
    memset(Author, 0, sizeof (char) * 64);
    Bank_Saved = (char *) malloc(sizeof (char) * 128);
    memset(Bank_Saved, 0, sizeof (char) * 128);
    UserRealName = (char *) malloc(sizeof (char) * 128);
    memset(UserRealName, 0, sizeof (char) * 128);
}

/**
 *  The effect names, position, and type used by the 
 *  "Put Order in your Rack" window.
 * 
 *  The types are:
 *      1   - Distortion
 *      2   - Modulation
 *      4   - Time
 *      8   - Emulation
 *      9   - Both Emulation & Distortion (1 + 8) : StompBox, Valve
 *      16  - Filters
 *      32  - Dynamics
 *      64  - Processing & EQ
 *      128 - Synthesis
 */
void
RKR::put_order_in_rack()
{
    static const char *los_names[] =
    {
        "AlienWah", ITS(EFX_ALIENWAH).c_str(), "16",
        "Analog Phaser", ITS(EFX_ANALOG_PHASER).c_str(), "2",
        "Arpie", ITS(EFX_ARPIE).c_str(), "4",
        "Cabinet", ITS(EFX_CABINET).c_str(), "8",
        "Chorus", ITS(EFX_CHORUS).c_str(), "2",
        "Coil Crafter", ITS(EFX_COILCRAFTER).c_str(), "8",
        "CompBand", ITS(EFX_COMPBAND).c_str(), "8",
        "Compressor", ITS(EFX_COMPRESSOR).c_str(), "32",
        "Convolotron", ITS(EFX_CONVOLOTRON).c_str(), "8",
        "Derelict", ITS(EFX_DERELICT).c_str(), "1",
        "DistBand", ITS(EFX_DISTBAND).c_str(), "1",
        "Distortion", ITS(EFX_DISTORTION).c_str(), "1",
        "Dual Flange", ITS(EFX_DUAL_FLANGE).c_str(), "2",
        "Echo", ITS(EFX_ECHO).c_str(), "4",
        "Echotron", ITS(EFX_ECHOTRON).c_str(), "4",
        "Echoverse", ITS(EFX_ECHOVERSE).c_str(), "4",
        "EQ", ITS(EFX_EQ).c_str(), "64",
        "Exciter", ITS(EFX_EXCITER).c_str(), "64",
        "Expander", ITS(EFX_EXPANDER).c_str(), "32",
        "Flanger", ITS(EFX_FLANGER).c_str(), "2",
        "Harmonizer", ITS(EFX_HARMONIZER).c_str(), "128",
        "Infinity", ITS(EFX_INFINITY).c_str(), "16",
        "Looper", ITS(EFX_LOOPER).c_str(), "128",
        "MusicalDelay", ITS(EFX_MUSICAL_DELAY).c_str(), "4",
        "MuTroMojo", ITS(EFX_MUTROMOJO).c_str(), "16",
        "NoiseGate", ITS(EFX_NOISEGATE).c_str(), "32",
        "Opticaltrem", ITS(EFX_OPTICALTREM).c_str(), "2",
        "Overdrive", ITS(EFX_OVERDRIVE).c_str(), "1",
        "Pan", ITS(EFX_PAN).c_str(), "64",
        "Parametric EQ", ITS(EFX_PARAMETRIC).c_str(), "64",
        "Phaser", ITS(EFX_PHASER).c_str(), "2",
        "Reverb", ITS(EFX_REVERB).c_str(), "4",
        "Reverbtron", ITS(EFX_REVERBTRON).c_str(), "4",
        "Ring", ITS(EFX_RING).c_str(), "128",
        "Sequence", ITS(EFX_SEQUENCE).c_str(), "128",
        "ShelfBoost", ITS(EFX_SHELFBOOST).c_str(), "64",
        "Shifter", ITS(EFX_SHIFTER).c_str(), "128",
        "Shuffle", ITS(EFX_SHUFFLE).c_str(), "64",
        "StereoHarm", ITS(EFX_STEREOHARM).c_str(), "128",
        "StompBox", ITS(EFX_STOMPBOX).c_str(), "9",
        "Sustainer", ITS(EFX_SUSTAINER).c_str(), "32",
        "Synthfilter", ITS(EFX_SYNTHFILTER).c_str(), "16",
        "Valve", ITS(EFX_VALVE).c_str(), "9",
        "VaryBand", ITS(EFX_VARYBAND).c_str(), "2",
        "Vibe", ITS(EFX_VIBE).c_str(), "2",
        "Vocoder", ITS(EFX_VOCODER).c_str(), "128",
        "WahWah", ITS(EFX_WAHWAH).c_str(), "16"
    };

    for (int i = 0; i < C_NUMBER_EFFECTS * 3; i += 3)
    {
        strcpy(efx_names[i / 3].Nom, los_names[i]);
        sscanf(los_names[i + 1], "%d", &efx_names[i / 3].Pos);
        sscanf(los_names[i + 2], "%d", &efx_names[i / 3].Type);
    }
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
RKR::add_metro()
{
    for (int i = 0; i < period; i++)
    {
        efxoutl[i] += m_ticks[i] * M_Metro_Vol;
        efxoutr[i] += m_ticks[i] * M_Metro_Vol;
    }
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

    if ((NumEffect == EFX_REVERB) || (NumEffect == EFX_MUSICAL_DELAY))
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
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        if (efx_order[i] == EFX_VOCODER)
        {
            if (EFX_Bypass[EFX_VOCODER]) return (1);
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
    Rack_Effects[EFX_EQ]->cleanup();
    Rack_Effects[EFX_REVERB]->cleanup();
    Rack_Effects[EFX_DISTORTION]->cleanup();
    Rack_Effects[EFX_OVERDRIVE]->cleanup();
    efx_Compressor->cleanup();
    Rack_Effects[EFX_ECHO]->cleanup();
    Rack_Effects[EFX_CHORUS]->cleanup();
    Rack_Effects[EFX_FLANGER]->cleanup();
    Rack_Effects[EFX_PHASER]->cleanup();
    Rack_Effects[EFX_PARAMETRIC]->cleanup();
    Rack_Effects[EFX_WAHWAH]->cleanup();
    Rack_Effects[EFX_ALIENWAH]->cleanup();
    Rack_Effects[EFX_CABINET]->cleanup();
    Rack_Effects[EFX_PAN]->cleanup();
    Rack_Effects[EFX_HARMONIZER]->cleanup();
    Rack_Effects[EFX_MUSICAL_DELAY]->cleanup();
    Rack_Effects[EFX_NOISEGATE]->cleanup();
    Rack_Effects[EFX_DERELICT]->cleanup();
    Rack_Effects[EFX_ANALOG_PHASER]->cleanup();
    Rack_Effects[EFX_VALVE]->cleanup();
    Rack_Effects[EFX_DUAL_FLANGE]->cleanup();
    Rack_Effects[EFX_RING]->cleanup();
    Rack_Effects[EFX_EXCITER]->cleanup();
    Rack_Effects[EFX_DISTBAND]->cleanup();
    Rack_Effects[EFX_ARPIE]->cleanup();
    Rack_Effects[EFX_EXPANDER]->cleanup();
    Rack_Effects[EFX_SHUFFLE]->cleanup();
    Rack_Effects[EFX_SYNTHFILTER]->cleanup();
    Rack_Effects[EFX_VARYBAND]->cleanup();
    Rack_Effects[EFX_CONVOLOTRON]->cleanup();
    Rack_Effects[EFX_LOOPER]->cleanup();
    Rack_Effects[EFX_MUTROMOJO]->cleanup();
    Rack_Effects[EFX_ECHOVERSE]->cleanup();
    Rack_Effects[EFX_COILCRAFTER]->cleanup();
    Rack_Effects[EFX_SHELFBOOST]->cleanup();
    Rack_Effects[EFX_VOCODER]->cleanup();
    Rack_Effects[EFX_SUSTAINER]->cleanup();
    Rack_Effects[EFX_SEQUENCE]->cleanup();
    Rack_Effects[EFX_SHIFTER]->cleanup();
    Rack_Effects[EFX_STOMPBOX]->cleanup();
    Rack_Effects[EFX_REVERBTRON]->cleanup();
    Rack_Effects[EFX_ECHOTRON]->cleanup();
    Rack_Effects[EFX_STEREOHARM]->cleanup();
    Rack_Effects[EFX_COMPBAND]->cleanup();
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
    /* Don't process when updating quality since the efx may be deleted */
    if(quality_update)
        return;

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
            if (efx_MIDIConverter->getpar(MIDIConv_FFT))
            {
                efx_MIDIConverter->fftFloat(efxoutl, efxoutr, val_sum, HarmRecNote->freqs, HarmRecNote->lfreqs);
            }
            else
            {
                efx_MIDIConverter->schmittFloat(efxoutl, efxoutr, val_sum, HarmRecNote->freqs, HarmRecNote->lfreqs);
            }
        }

        if ((EFX_Bypass[EFX_HARMONIZER]) && (have_signal))
        {
            Harmonizer *Efx_Harmonizer = static_cast <Harmonizer*> (Rack_Effects[EFX_HARMONIZER]);
            if (Efx_Harmonizer->mira)
            {
                if ((Efx_Harmonizer->PMIDI) || (Efx_Harmonizer->PSELECT))
                {
                    HarmRecNote->schmittFloat(efxoutl, efxoutr);
                    if ((HarmRecNote->reconota != -1) && (HarmRecNote->reconota != HarmRecNote->last))
                    {
                        if (HarmRecNote->afreq > 0.0)
                        {
                            RC_Harm->Vamos(0, Efx_Harmonizer->Pinterval - 12, HarmRecNote->reconota);
                            Efx_Harmonizer->r_ratio = RC_Harm->r__ratio[0]; //pass the found ratio
                            HarmRecNote->last = HarmRecNote->reconota;
                        }
                    }
                }
            }
        }


        if ((EFX_Bypass[EFX_STEREOHARM]) && (have_signal))
        {
            StereoHarm *Efx_StereoHarm = static_cast<StereoHarm*>(Rack_Effects[EFX_STEREOHARM]);

            if (Efx_StereoHarm->mira)
            {
                if ((Efx_StereoHarm->PMIDI) || (Efx_StereoHarm->PSELECT))
                {
                    StHarmRecNote->schmittFloat(efxoutl, efxoutr);
                    if ((StHarmRecNote->reconota != -1) && (StHarmRecNote->reconota != StHarmRecNote->last))
                    {
                        if (StHarmRecNote->afreq > 0.0)
                        {
                            RC_Stereo_Harm->Vamos(1, Efx_StereoHarm->Pintervall - 12, StHarmRecNote->reconota);
                            RC_Stereo_Harm->Vamos(2, Efx_StereoHarm->Pintervalr - 12, StHarmRecNote->reconota);
                            Efx_StereoHarm->r_ratiol = RC_Stereo_Harm->r__ratio[1];
                            Efx_StereoHarm->r_ratior = RC_Stereo_Harm->r__ratio[2];
                            StHarmRecNote->last = StHarmRecNote->reconota;
                        }
                    }
                }
            }
        }

        {
            Ring *Efx_Ring = static_cast<Ring*>(Rack_Effects[EFX_RING]);
            if ((EFX_Bypass[EFX_RING]) && (Efx_Ring->Pafreq))
            {
                RingRecNote->schmittFloat(efxoutl, efxoutr);
                if ((RingRecNote->reconota != -1) && (RingRecNote->reconota != RingRecNote->last))
                {
                    if (RingRecNote->afreq > 0.0)
                    {
                        Efx_Ring->Pfreq = lrintf(RingRecNote->lafreq);
                        StHarmRecNote->last = StHarmRecNote->reconota;
                    }
                }
            }
        }

        for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
        {
            switch (efx_order[i])
            {
            case EFX_EQ:
                if (EFX_Bypass[EFX_EQ])
                {
                    Rack_Effects[EFX_EQ]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_COMPRESSOR:
                if (EFX_Bypass[EFX_COMPRESSOR])
                {
                    efx_Compressor->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_DISTORTION:
                if (EFX_Bypass[EFX_DISTORTION])
                {
                    Rack_Effects[EFX_DISTORTION]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_DISTORTION, Rack_Effects[EFX_DISTORTION]->outvolume);
                }
                break;

            case EFX_OVERDRIVE:
                if (EFX_Bypass[EFX_OVERDRIVE])
                {
                    Rack_Effects[EFX_OVERDRIVE]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_OVERDRIVE, Rack_Effects[EFX_OVERDRIVE]->outvolume);
                }
                break;

            case EFX_ECHO:
                if (EFX_Bypass[EFX_ECHO])
                {
                    Rack_Effects[EFX_ECHO]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_ECHO, Rack_Effects[EFX_ECHO]->outvolume);
                }
                break;

            case EFX_CHORUS:
                if (EFX_Bypass[EFX_CHORUS])
                {
                    Rack_Effects[EFX_CHORUS]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_CHORUS, Rack_Effects[EFX_CHORUS]->outvolume);
                }
                break;

            case EFX_PHASER:
                if (EFX_Bypass[EFX_PHASER])
                {
                    Rack_Effects[EFX_PHASER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_PHASER, Rack_Effects[EFX_PHASER]->outvolume);
                }
                break;

            case EFX_FLANGER:
                if (EFX_Bypass[EFX_FLANGER])
                {
                    Rack_Effects[EFX_FLANGER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_FLANGER, Rack_Effects[EFX_FLANGER]->outvolume);
                }
                break;

            case EFX_REVERB:
                if (EFX_Bypass[EFX_REVERB])
                {
                    Rack_Effects[EFX_REVERB]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_REVERB, Rack_Effects[EFX_REVERB]->outvolume);
                }
                break;

            case EFX_PARAMETRIC:
                if (EFX_Bypass[EFX_PARAMETRIC])
                {
                    Rack_Effects[EFX_PARAMETRIC]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_WAHWAH:
                if (EFX_Bypass[EFX_WAHWAH])
                {
                    Rack_Effects[EFX_WAHWAH]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_WAHWAH, Rack_Effects[EFX_WAHWAH]->outvolume);
                }
                break;

            case EFX_ALIENWAH:
                if (EFX_Bypass[EFX_ALIENWAH])
                {
                    Rack_Effects[EFX_ALIENWAH]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_ALIENWAH, Rack_Effects[EFX_ALIENWAH]->outvolume);
                }
                break;

            case EFX_CABINET:
                if (EFX_Bypass[EFX_CABINET])
                {
                    Rack_Effects[EFX_CABINET]->out(efxoutl, efxoutr);
                    Vol3_Efx();
                }

                break;

            case EFX_PAN:
                if (EFX_Bypass[EFX_PAN])
                {
                    Rack_Effects[EFX_PAN]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_PAN, Rack_Effects[EFX_PAN]->outvolume);
                }
                break;

            case EFX_HARMONIZER:
                if (EFX_Bypass[EFX_HARMONIZER])
                {
                    Rack_Effects[EFX_HARMONIZER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_HARMONIZER, Rack_Effects[EFX_HARMONIZER]->outvolume);
                }
                break;

            case EFX_MUSICAL_DELAY:
                if (EFX_Bypass[EFX_MUSICAL_DELAY])
                {
                    Rack_Effects[EFX_MUSICAL_DELAY]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_MUSICAL_DELAY, Rack_Effects[EFX_MUSICAL_DELAY]->outvolume);
                }
                break;

            case EFX_NOISEGATE:
                if (EFX_Bypass[EFX_NOISEGATE])
                {
                    Rack_Effects[EFX_NOISEGATE]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_DERELICT:
                if (EFX_Bypass[EFX_DERELICT])
                {
                    Rack_Effects[EFX_DERELICT]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_DERELICT, Rack_Effects[EFX_DERELICT]->outvolume);
                }
                break;

            case EFX_ANALOG_PHASER:
                if (EFX_Bypass[EFX_ANALOG_PHASER])
                {
                    Rack_Effects[EFX_ANALOG_PHASER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_ANALOG_PHASER, Rack_Effects[EFX_ANALOG_PHASER]->outvolume);
                }
                break;

            case EFX_VALVE:
                if (EFX_Bypass[EFX_VALVE])
                {
                    Rack_Effects[EFX_VALVE]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_VALVE, Rack_Effects[EFX_VALVE]->outvolume);
                }
                break;

            case EFX_DUAL_FLANGE:
                if (EFX_Bypass[EFX_DUAL_FLANGE])
                {
                    Rack_Effects[EFX_DUAL_FLANGE]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_RING:
                if (EFX_Bypass[EFX_RING])
                {
                    Rack_Effects[EFX_RING]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_RING, Rack_Effects[EFX_RING]->outvolume);
                }
                break;

            case EFX_EXCITER:
                if (EFX_Bypass[EFX_EXCITER])
                {
                    Rack_Effects[EFX_EXCITER]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_DISTBAND:
                if (EFX_Bypass[EFX_DISTBAND])
                {
                    Rack_Effects[EFX_DISTBAND]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_DISTBAND, Rack_Effects[EFX_DISTBAND]->outvolume);
                }
                break;

            case EFX_ARPIE:
                if (EFX_Bypass[EFX_ARPIE])
                {
                    Rack_Effects[EFX_ARPIE]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_ARPIE, Rack_Effects[EFX_ARPIE]->outvolume);
                }
                break;

            case EFX_EXPANDER:
                if (EFX_Bypass[EFX_EXPANDER])
                {
                    Rack_Effects[EFX_EXPANDER]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_SHUFFLE:
                if (EFX_Bypass[EFX_SHUFFLE])
                {
                    Rack_Effects[EFX_SHUFFLE]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_SHUFFLE, Rack_Effects[EFX_SHUFFLE]->outvolume);
                }
                break;

            case EFX_SYNTHFILTER:
                if (EFX_Bypass[EFX_SYNTHFILTER])
                {
                    Rack_Effects[EFX_SYNTHFILTER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_SYNTHFILTER, Rack_Effects[EFX_SYNTHFILTER]->outvolume);
                }
                break;

            case EFX_VARYBAND:
                if (EFX_Bypass[EFX_VARYBAND])
                {
                    Rack_Effects[EFX_VARYBAND]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_VARYBAND, Rack_Effects[EFX_VARYBAND]->outvolume);
                }
                break;

            case EFX_CONVOLOTRON:
                if (EFX_Bypass[EFX_CONVOLOTRON])
                {
                    Rack_Effects[EFX_CONVOLOTRON]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_CONVOLOTRON, Rack_Effects[EFX_CONVOLOTRON]->outvolume);
                }
                break;

            case EFX_LOOPER:
                if (EFX_Bypass[EFX_LOOPER])
                {
                    Rack_Effects[EFX_LOOPER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_LOOPER, Rack_Effects[EFX_LOOPER]->outvolume);
                }
                break;

            case EFX_MUTROMOJO:
                if (EFX_Bypass[EFX_MUTROMOJO])
                {
                    Rack_Effects[EFX_MUTROMOJO]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_MUTROMOJO, Rack_Effects[EFX_MUTROMOJO]->outvolume);
                }
                break;

            case EFX_ECHOVERSE:
                if (EFX_Bypass[EFX_ECHOVERSE])
                {
                    Rack_Effects[EFX_ECHOVERSE]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_ECHOVERSE, Rack_Effects[EFX_ECHOVERSE]->outvolume);
                }
                break;

            case EFX_COILCRAFTER:
                if (EFX_Bypass[EFX_COILCRAFTER])
                {
                    Rack_Effects[EFX_COILCRAFTER]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_SHELFBOOST:
                if (EFX_Bypass[EFX_SHELFBOOST])
                {
                    Rack_Effects[EFX_SHELFBOOST]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_VOCODER:
                if (EFX_Bypass[EFX_VOCODER])
                {
                    Rack_Effects[EFX_VOCODER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_VOCODER, Rack_Effects[EFX_VOCODER]->outvolume);
                }
                break;

            case EFX_SUSTAINER:
                if (EFX_Bypass[EFX_SUSTAINER])
                {
                    Rack_Effects[EFX_SUSTAINER]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_SEQUENCE:
                if (EFX_Bypass[EFX_SEQUENCE])
                {
                    Rack_Effects[EFX_SEQUENCE]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_SEQUENCE, Rack_Effects[EFX_SEQUENCE]->outvolume);
                }
                break;

            case EFX_SHIFTER:
                if (EFX_Bypass[EFX_SHIFTER])
                {
                    Rack_Effects[EFX_SHIFTER]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_SHIFTER, Rack_Effects[EFX_SHIFTER]->outvolume);
                }
                break;

            case EFX_STOMPBOX:
                if (EFX_Bypass[EFX_STOMPBOX])
                {
                    Rack_Effects[EFX_STOMPBOX]->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_REVERBTRON:
                if (EFX_Bypass[EFX_REVERBTRON])
                {
                    Rack_Effects[EFX_REVERBTRON]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_REVERBTRON, Rack_Effects[EFX_REVERBTRON]->outvolume);
                }
                break;

            case EFX_ECHOTRON:
                if (EFX_Bypass[EFX_ECHOTRON])
                {
                    Rack_Effects[EFX_ECHOTRON]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_ECHOTRON, Rack_Effects[EFX_ECHOTRON]->outvolume);
                }
                break;

            case EFX_STEREOHARM:
                if (EFX_Bypass[EFX_STEREOHARM])
                {
                    Rack_Effects[EFX_STEREOHARM]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_STEREOHARM, Rack_Effects[EFX_STEREOHARM]->outvolume);
                }
                break;

            case EFX_COMPBAND:
                if (EFX_Bypass[EFX_COMPBAND])
                {
                    Rack_Effects[EFX_COMPBAND]->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_COMPBAND, Rack_Effects[EFX_COMPBAND]->outvolume);
                }
                break;

            case EFX_OPTICALTREM:
                if (EFX_Bypass[EFX_OPTICALTREM])
                {
                    efx_Opticaltrem->out(efxoutl, efxoutr);
                    Vol2_Efx();
                }
                break;

            case EFX_VIBE:
                if (EFX_Bypass[EFX_VIBE])
                {
                    efx_Vibe->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_VIBE, efx_Vibe->outvolume);
                }
                break;

            case EFX_INFINITY:
                if (EFX_Bypass[EFX_INFINITY])
                {
                    efx_Infinity->out(efxoutl, efxoutr);
                    Vol_Efx(EFX_INFINITY, efx_Infinity->outvolume);
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
