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
#include "global.h"
#include "process.h"

int global_error_number = 0;
char *jack_client_name = (char*) PACKAGE;

RKR::RKR(int gui) :
    efx_Tuner(NULL),
    efx_MIDIConverter(NULL),
    M_Metronome(NULL),
    beat(NULL),
    HarmRecNote(NULL),
    StHarmRecNote(NULL),
    RingRecNote(NULL),
    RC_Harm(NULL),
    RC_Stereo_Harm(NULL),
    EFX_Active(),
    efx_FLimiter(NULL),
    U_Resample(NULL),
    D_Resample(NULL),
    A_Resample(NULL),
    DC_Offsetl(NULL),
    DC_Offsetr(NULL),
    jackclient(NULL),
    options(),
    status(),
    jackcliname(),
    Jack_Shut_Down(0),
    db6booster(),
    FX_Master_Active_Reset(),
    MIDIConverter_Active(0),
    Metro_Active(0),
    Tuner_Active(0),
    Tap_Active(0),
    ACI_Active(0),
    Exit_Program(0),
    Selected_Preset(1),
    Change_Preset(C_CHANGE_PRESET_OFF),
    Change_Bank(C_BANK_CHANGE_OFF),
    Change_MIDI_Table(C_BANK_CHANGE_OFF),
    Command_Line_File(0),
    File_To_Load(),
    Gui_Shown(gui),
    Gui_Refresh(0),
    Shut_Off_Below_Mouse(0),
    No_Jack_Client(0),
    Bank_Load_Command_Line(0),
    saved_order(),
    efx_order(),
    new_order(),
    availables(),
    active(),
    help_displayed(0),
    bank_modified(0),
    comemouse(),
    IsCoIn(),
    Cyoin(),
    Pyoin(),
    Ccin(),
    Pcin(),
    Mcontrol(),
    RControl(0),
    ControlGet(0),
    CountWait(),
    quality_update(false),
    Effect_Type_Filter(Type_All),
    Alert_Count(0),
    upsample(),
    M_Metro_Sound(),
    change_scale(1),
    flpos(),
    m_displayed(),
    Mvalue(),
    Mnumeff(),
    OnOffC(),
    JACK_SAMPLE_RATE(),
    JACK_PERIOD(),
    period_master(),
    fPeriod(),
    sample_rate(),
    fSample_rate(),
    cSample_rate(),
    interpbuf(NULL),
    swidth(),
    sheight(),
    sschema(),
    slabel_color(),
    sfore_color(),
    sback_color(),
    sleds_color(),
    sfont_size(C_DEFAULT_FONT_SIZE),
    sfont_type(0),
    have_signal(),
    OnCounter(),
    t_periods(),
    tempocnt(0),
    Tap_Display(0),
    Tap_Selection(0),
    Tap_TempoSet(90),
    Tap_SetValue(),
    t_timeout(0),
    jt_state(),
    note_old(0),
    cents_old(0),
    cpufp(),
    Jack_IN_Port_Connnection_Status(),
    Jack_OUT_Port_Connnection_Status(),
    Jack_AUX_Port_Connnection_Status(),
    Jack_MIDI_IN_Port_Connnection_Status(),
    Jack_MIDI_OUT_Port_Connnection_Status(),
    Jack_Port_Connnection_Changed(),
    PG_table_modified(0),
    active_bank(0),
    new_bank_loaded(),
    Aux_Gain(),
    Aux_Threshold(),
    Aux_MIDI(),
    Aux_Minimum(),
    Aux_Maximum(),
    Aux_Source(),
    last_auxvalue(0),
    M_Metro_Tempo(),
    M_Metro_Bar(),
    mtc_counter(),
    ML_filter(),
    ML_clist(),
    Tap_timeB(0),
    Tap_timeC(),
    jt_tempo(),
    tempobuf(),
    u_down(),
    u_up(),
    timeA(),
    booster(),
    cpuload(),
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
    looper_lqua(0),
    nfreq_old(0),
    afreq_old(0),
    Bank_Saved(),
    Command_Line_Bank(),
    efx_names(),
    mc_efx_params(),
    Bank(),
    MIDI_Table(),
    MIDI_Table_Bank_Preset_Names(),
    midi_in(NULL)
#ifdef SYSEX_SUPPORT
    ,m_have_sysex_message(0),
    m_preset_name(),
    m_bank_number(0),
    m_preset_number(0)
#endif
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
    new_preset();

    // Initialize Bank
    new_bank(Bank);
    
    // Loads all Banks, default and any in Settings/Preferences/Bank - User Directory
    load_bank_vector();

    // Custom MIDI table file loading
    load_MIDI_table_vector();

    // Either default or last used table
    load_default_midi_table();

    // The Preset scroll items in Settings/Preferences/Midi - MIDI Program Change Table
    load_custom_MIDI_table_preset_names();

    // Related user files in User Directory
    load_convolotron_vector();
    load_echotron_vector();
    load_reverbtron_vector();
}

RKR::~RKR()
{
    /* To clean up valgrind log */

    delete DC_Offsetl;
    delete DC_Offsetr;
    delete M_Metronome;
    delete efx_FLimiter;
    
    for(int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        if(Rack_Effects[i])
            delete Rack_Effects[i];
    }

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

    // alsa
    if(midi_in)
    {
        snd_seq_close(midi_in);
    }
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

    jackclient = jack_client_open(temp, options, &status, NULL);

    if (jackclient == NULL)
    {
        fprintf(stderr, "Cannot make a jack client, is jackd running?\n");
        No_Jack_Client = 1;
        return 0;
    }

    strcpy(jackcliname, jack_get_client_name(jackclient));
    strcpy(Config.jackcliname, jack_get_client_name(jackclient));

    JACK_SAMPLE_RATE = jack_get_sample_rate(jackclient);
    JACK_PERIOD = jack_get_buffer_size(jackclient);
    
    return 1;
}

/**
 *  Loads the user preferences set in the Settings/Preferences tabs:
 *  Look, Audio, Quality, MIDI, Jack, Misc, Bank.
 *  These settings are saved in:
 *  /home/username/.fltk/github.com.Stazed.rakarrrack.plus/rakarrack-plus.prefs
 * 
 *  This loads only items needed for audio/MIDI processing. Since the program
 *  can be run without GUI, the Look and other user settings not related to audio/MIDI
 *  do not need to be loaded here. This is for initial process setup. 
 * 
 *  The other non audio/MIDI user settings are loaded by the load_previous_state()
 *  function that is called when the RKRGUI class is created.
 */
void
RKR::load_user_preferences()
{
    Config.load_previous_state();
    upsample = Config.upsample;
    Adjust_Upsample();
}

void
RKR::instantiate_effects()
{
    DC_Offsetl = new AnalogFilter(1, 20, 1, 0, sample_rate, interpbuf);
    DC_Offsetr = new AnalogFilter(1, 20, 1, 0, sample_rate, interpbuf);
    M_Metronome = new metronome(fSample_rate, period_master);
    efx_FLimiter = new Limiter(fSample_rate, period_master);

    Rack_Effects[EFX_CHORUS] = new Chorus(fSample_rate, period_master);
    Rack_Effects[EFX_FLANGER] = new Flanger(fSample_rate, period_master);
    Rack_Effects[EFX_REVERB] = new Reverb(fSample_rate, period_master);
    Rack_Effects[EFX_ECHO] = new Echo(fSample_rate, period_master);
    Rack_Effects[EFX_PHASER] = new Phaser(fSample_rate, period_master);
    Rack_Effects[EFX_ANALOG_PHASER] = new Analog_Phaser(fSample_rate, period_master);
    Rack_Effects[EFX_DISTORTION] = new Distorsion(Config.Dist_res_amount, Config.Dist_up_q, Config.Dist_down_q, fSample_rate, period_master);
    Rack_Effects[EFX_OVERDRIVE] = new Overdrive(Config.Ovrd_res_amount, Config.Ovrd_up_q, Config.Ovrd_down_q, fSample_rate, period_master);
    Rack_Effects[EFX_PARAMETRIC] = new ParametricEQ(fSample_rate, period_master);
    Rack_Effects[EFX_EQ] = new EQ( fSample_rate, period_master);
    Rack_Effects[EFX_COMPRESSOR] = new Compressor(fSample_rate, period_master);
    Rack_Effects[EFX_WAHWAH] = new WahWah(fSample_rate, period_master);
    Rack_Effects[EFX_ALIENWAH] = new Alienwah(fSample_rate, period_master);
    Rack_Effects[EFX_CABINET] = new Cabinet(fSample_rate, period_master);
    Rack_Effects[EFX_PAN] = new Pan(fSample_rate, period_master);
    Rack_Effects[EFX_HARMONIZER] = new Harmonizer((long) Config.HarQual, Config.Har_Down, Config.Har_U_Q, Config.Har_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_MUSICAL_DELAY] = new MusicDelay(fSample_rate, period_master);
    Rack_Effects[EFX_NOISEGATE] = new Gate(fSample_rate, period_master);
    Rack_Effects[EFX_DERELICT] = new Derelict(Config.Dere_res_amount, Config.Dere_up_q, Config.Dere_down_q, fSample_rate, period_master);
    Rack_Effects[EFX_VALVE] = new Valve(fSample_rate, period_master);
    Rack_Effects[EFX_DUAL_FLANGE] = new Dflange(fSample_rate, period_master);
    Rack_Effects[EFX_RING] = new Ring(fSample_rate, period_master);
    Rack_Effects[EFX_EXCITER] = new Exciter(fSample_rate, period_master);
    Rack_Effects[EFX_DISTBAND] = new DistBand(Config.DBand_res_amount, Config.DBand_up_q, Config.DBand_down_q, fSample_rate, period_master);
    Rack_Effects[EFX_ARPIE] = new Arpie(fSample_rate, period_master);
    Rack_Effects[EFX_EXPANDER] = new Expander(fSample_rate, period_master);
    Rack_Effects[EFX_SHUFFLE] = new Shuffle(fSample_rate, period_master);
    Rack_Effects[EFX_SYNTHFILTER] = new Synthfilter(fSample_rate, period_master);
    Rack_Effects[EFX_VARYBAND] = new VaryBand(fSample_rate, period_master);
    Rack_Effects[EFX_CONVOLOTRON] = new Convolotron(Config.Con_Down, Config.Con_U_Q, Config.Con_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_LOOPER] = new Looper(Config.looper_size, fSample_rate, period_master);
    Rack_Effects[EFX_MUTROMOJO] = new MuTroMojo(fSample_rate, period_master);
    Rack_Effects[EFX_ECHOVERSE] = new Echoverse(fSample_rate, period_master);
    Rack_Effects[EFX_COILCRAFTER] = new CoilCrafter(fSample_rate, period_master);
    Rack_Effects[EFX_SHELFBOOST] = new ShelfBoost(fSample_rate, period_master);
    Rack_Effects[EFX_VOCODER] = new Vocoder(auxresampled, Config.VocBands, Config.Voc_Down, Config.Voc_U_Q, Config.Voc_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_SUSTAINER] = new Sustainer(fSample_rate, period_master);
    Rack_Effects[EFX_SEQUENCE] = new Sequence((long) Config.SeqQual, Config.Seq_Down, Config.Seq_U_Q, Config.Seq_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_SHIFTER] = new Shifter((long) Config.ShiQual, Config.Shi_Down, Config.Shi_U_Q, Config.Shi_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_STOMPBOX] = new StompBox(Config.Stomp_res_amount, Config.Stomp_up_q, Config.Stomp_down_q, fSample_rate, period_master);
    Rack_Effects[EFX_REVERBTRON] = new Reverbtron(Config.Rev_Down, Config.Rev_U_Q, Config.Rev_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_ECHOTRON] = new Echotron(fSample_rate, period_master);
    Rack_Effects[EFX_STEREOHARM] = new StereoHarm((long) Config.SteQual, Config.Ste_Down, Config.Ste_U_Q, Config.Ste_D_Q, fSample_rate, period_master);
    Rack_Effects[EFX_COMPBAND] = new CompBand(fSample_rate, period_master);
    Rack_Effects[EFX_OPTICALTREM] = new Opticaltrem(fSample_rate, period_master);
    Rack_Effects[EFX_VIBE] = new Vibe(fSample_rate, period_master);
    Rack_Effects[EFX_INFINITY] = new Infinity(fSample_rate, period_master);

    U_Resample = new Resample(Config.UpQual);
    D_Resample = new Resample(Config.DownQual);
    A_Resample = new Resample(3);

    beat = new beattracker(fSample_rate, period_master);
    efx_Tuner = new Tuner(fSample_rate);
    efx_MIDIConverter = new MIDIConverter(jackcliname, fSample_rate, period_master);
    HarmRecNote = new Recognize(Config.rtrig, Config.aFreq, fSample_rate, period_master);
    StHarmRecNote = new Recognize(Config.rtrig, Config.aFreq, fSample_rate, period_master);
    RingRecNote = new Recognize(Config.rtrig, Config.aFreq, fSample_rate, period_master);
    RC_Harm = new RecChord();
    RC_Stereo_Harm = new RecChord();
    
    // Defaults and cleanup
    efx_FLimiter->setpreset(0);
    RC_Harm->cleanup();
    RC_Stereo_Harm->cleanup();
    HarmRecNote->reconota = -1;
    StHarmRecNote->reconota = -1;
    RingRecNote->reconota = -1;
    
    for (int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        EFX_Param_Size[i] = Rack_Effects[i]->get_number_efx_parameters();
    }
}

void
RKR::initialize_arrays()
{
    efxoutl = (float *) malloc(sizeof (float) * period_master);
    efxoutr = (float *) malloc(sizeof (float) * period_master);

    smpl = (float *) malloc(sizeof (float) * period_master);
    smpr = (float *) malloc(sizeof (float) * period_master);

    anall = (float *) malloc(sizeof (float) * period_master);
    analr = (float *) malloc(sizeof (float) * period_master);

    auxdata = (float *) malloc(sizeof (float) * period_master);
    auxresampled = (float *) malloc(sizeof (float) * period_master);

    m_ticks = (float *) malloc(sizeof (float) * period_master);

    interpbuf = (float*) malloc(sizeof (float)* period_master);

    memset(efxoutl, 0, sizeof (float)*period_master);
    memset(efxoutr, 0, sizeof (float)*period_master);

    memset(smpl, 0, sizeof (float)*period_master);
    memset(smpr, 0, sizeof (float)*period_master);

    memset(anall, 0, sizeof (float)*period_master);
    memset(analr, 0, sizeof (float)*period_master);

    memset(auxdata, 0, sizeof (float)*period_master);
    memset(auxresampled, 0, sizeof (float)*period_master);

    memset(m_ticks, 0, sizeof (float)*period_master);
    memset(interpbuf, 0, sizeof (float)*period_master);

}

/**
 *  The effect names, position, and type used by the 
 *  "Put Order in your Rack" window.
 */
void
RKR::put_order_in_rack()
{
    static const char *los_names[] =
    {
        "AlienWah", strdup( NTS(EFX_ALIENWAH).c_str()), strdup( NTS(Type_Filters).c_str()),
        "Analog Phaser", strdup( NTS(EFX_ANALOG_PHASER).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Arpie", strdup( NTS(EFX_ARPIE).c_str()), strdup( NTS(Type_Time).c_str()),
        "Cabinet", strdup( NTS(EFX_CABINET).c_str()), strdup( NTS(Type_Emulation).c_str()),
        "Chorus", strdup( NTS(EFX_CHORUS).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Coil Crafter", strdup( NTS(EFX_COILCRAFTER).c_str()), strdup( NTS(Type_Emulation).c_str()),
        "CompBand", strdup( NTS(EFX_COMPBAND).c_str()), strdup( NTS(Type_Emulation).c_str()),
        "Compressor", strdup( NTS(EFX_COMPRESSOR).c_str()), strdup( NTS(Type_Dynamics).c_str()),
        "Convolotron", strdup( NTS(EFX_CONVOLOTRON).c_str()), strdup( NTS(Type_Emulation).c_str()),
        "Derelict", strdup( NTS(EFX_DERELICT).c_str()), strdup( NTS(Type_Distortion).c_str()),
        "DistBand", strdup( NTS(EFX_DISTBAND).c_str()), strdup( NTS(Type_Distortion).c_str()),
        "Distortion", strdup( NTS(EFX_DISTORTION).c_str()), strdup( NTS(Type_Distortion).c_str()),
        "Dual Flange", strdup( NTS(EFX_DUAL_FLANGE).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Echo", strdup( NTS(EFX_ECHO).c_str()), strdup( NTS(Type_Time).c_str()),
        "Echotron", strdup( NTS(EFX_ECHOTRON).c_str()), strdup( NTS(Type_Time).c_str()),
        "Echoverse", strdup( NTS(EFX_ECHOVERSE).c_str()), strdup( NTS(Type_Time).c_str()),
        "EQ", strdup( NTS(EFX_EQ).c_str()), strdup( NTS(Type_Processing_and_EQ).c_str()),
        "Exciter", strdup( NTS(EFX_EXCITER).c_str()), strdup( NTS(Type_Processing_and_EQ).c_str()),
        "Expander", strdup( NTS(EFX_EXPANDER).c_str()), strdup( NTS(Type_Dynamics).c_str()),
        "Flanger", strdup( NTS(EFX_FLANGER).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Harmonizer", strdup( NTS(EFX_HARMONIZER).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "Infinity", strdup( NTS(EFX_INFINITY).c_str()), strdup( NTS(Type_Filters).c_str()),
        "Looper", strdup( NTS(EFX_LOOPER).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "MusicalDelay", strdup( NTS(EFX_MUSICAL_DELAY).c_str()), strdup( NTS(Type_Time).c_str()),
        "MuTroMojo", strdup( NTS(EFX_MUTROMOJO).c_str()), strdup( NTS(Type_Filters).c_str()),
        "NoiseGate", strdup( NTS(EFX_NOISEGATE).c_str()), strdup( NTS(Type_Dynamics).c_str()),
        "Opticaltrem", strdup( NTS(EFX_OPTICALTREM).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Overdrive", strdup( NTS(EFX_OVERDRIVE).c_str()), strdup( NTS(Type_Distortion).c_str()),
        "Pan", strdup( NTS(EFX_PAN).c_str()), strdup( NTS(Type_Processing_and_EQ).c_str()),
        "Parametric EQ", strdup( NTS(EFX_PARAMETRIC).c_str()), strdup( NTS(Type_Processing_and_EQ).c_str()),
        "Phaser", strdup( NTS(EFX_PHASER).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Reverb", strdup( NTS(EFX_REVERB).c_str()), strdup( NTS(Type_Time).c_str()),
        "Reverbtron", strdup( NTS(EFX_REVERBTRON).c_str()), strdup( NTS(Type_Time).c_str()),
        "Ring", strdup( NTS(EFX_RING).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "Sequence", strdup( NTS(EFX_SEQUENCE).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "ShelfBoost", strdup( NTS(EFX_SHELFBOOST).c_str()), strdup( NTS(Type_Processing_and_EQ).c_str()),
        "Shifter", strdup( NTS(EFX_SHIFTER).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "Shuffle", strdup( NTS(EFX_SHUFFLE).c_str()), strdup( NTS(Type_Processing_and_EQ).c_str()),
        "StereoHarm", strdup( NTS(EFX_STEREOHARM).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "StompBox", strdup( NTS(EFX_STOMPBOX).c_str()), strdup( NTS(Type_Distortion_and_Emulation).c_str()),
        "Sustainer", strdup( NTS(EFX_SUSTAINER).c_str()), strdup( NTS(Type_Dynamics).c_str()),
        "Synthfilter", strdup( NTS(EFX_SYNTHFILTER).c_str()), strdup( NTS(Type_Filters).c_str()),
        "Valve", strdup( NTS(EFX_VALVE).c_str()), strdup( NTS(Type_Distortion_and_Emulation).c_str()),
        "VaryBand", strdup( NTS(EFX_VARYBAND).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Vibe", strdup( NTS(EFX_VIBE).c_str()), strdup( NTS(Type_Modulation).c_str()),
        "Vocoder", strdup( NTS(EFX_VOCODER).c_str()), strdup( NTS(Type_Synthesis).c_str()),
        "WahWah", strdup( NTS(EFX_WAHWAH).c_str()), strdup( NTS(Type_Filters).c_str()),
    };

    for (int i = 0; i < C_NUMBER_EFFECTS * 3; i += 3)
    {
        strcpy(efx_names[i / 3].Nom, los_names[i]);
        sscanf(los_names[i + 1], "%d", &efx_names[i / 3].Pos);
        sscanf(los_names[i + 2], "%d", &efx_names[i / 3].Type);
    }
}

/**
 * Check if the filename is one of the Bank_Vector items.
 * If it is, then copy it to the active bank.
 * If not then error message, telling the user to put the file
 * in the user bank directory Settings/Preferences/Bank - User Directory.
 * @param filename
 *      The file name to match.
 * 
 * @return 
 *      1 =  bank was found and loaded to active Bank.
 *      0 =  bank not found in user directory.
 */
int
RKR::load_bank_from_vector(const std::string &filename)
{
    for(unsigned i = 0; i < Bank_Vector.size (); i++)
    {
        if(strcmp(filename.c_str(), Bank_Vector[i].Bank_File_Name.c_str()) == 0)
        {
            copy_bank(Bank, Bank_Vector[i].Bank);
            active_bank = i;
            return (1);
        }
    }
    
    Handle_Message(40, filename);
    
    return (0);
}

void
RKR::Adjust_Upsample()
{
    if (upsample)
    {
        sample_rate = JACK_SAMPLE_RATE * (Config.UpAmo + 2);
        period_master = JACK_PERIOD * (Config.UpAmo + 2);
        u_up = (double) Config.UpAmo + 2.0;
        u_down = 1.0 / u_up;
    }
    else
    {
        sample_rate = JACK_SAMPLE_RATE;
        period_master = JACK_PERIOD;
    }

    fSample_rate = (float) sample_rate;
    cSample_rate = 1.0f / (float) sample_rate;
    fPeriod = float(period_master);
    t_periods = JACK_SAMPLE_RATE / 12 / JACK_PERIOD;

}

void
RKR::add_metro()
{
    for (unsigned i = 0; i < period_master; i++)
    {
        efxoutl[i] += m_ticks[i] * M_Metro_Vol;
        efxoutr[i] += m_ticks[i] * M_Metro_Vol;
    }
}

void
RKR::calculavol(int i)
{
    if (i == 1)
    {
        Log_I_Gain = powf(Active_Preset.Input_Gain * 2.0f, 4);
    }
    
    if (i == 2)
    {
        Log_M_Volume = powf(Active_Preset.Master_Volume * 2.0f, 4);
    }
}

int
RKR::checkforaux()
{
    for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
    {
        if (efx_order[i] == EFX_VOCODER)
        {
            if (EFX_Active[EFX_VOCODER]) return (1);
        }
    }

    return (0);
}

void
RKR::Control_Gain(float *origl, float *origr)
{
    float il_sum = 1e-12f;
    float ir_sum = 1e-12f;
    float tmp = 0.0;

    if (upsample)
    {
        U_Resample->out(origl, origr, efxoutl, efxoutr, JACK_PERIOD, u_up);
        if ((checkforaux()) || (ACI_Active))
        {
            A_Resample->mono_out(auxdata, auxresampled, JACK_PERIOD, u_up, period_master);
        }
    }
    else if ((checkforaux()) || (ACI_Active))
    {
        memcpy(auxresampled, auxdata, sizeof (float)*JACK_PERIOD);
    }

    if (Config.DC_Offset)
    {
        DC_Offsetl->filterout(efxoutl, period_master);
        DC_Offsetr->filterout(efxoutr, period_master);
    }

    for (unsigned i = 0; i < period_master; i++)
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
    
    memcpy(smpl, efxoutl, sizeof (float)*period_master);
    memcpy(smpr, efxoutr, sizeof (float)*period_master);

    float temp_sum = (float) CLAMP(rap2dB(il_sum), -48.0, 15.0);
    val_il_sum = .6f * old_il_sum + .4f * temp_sum;

    temp_sum = (float) CLAMP(rap2dB(ir_sum), -48.0, 15.0);
    val_ir_sum = .6f * old_ir_sum + .4f * temp_sum;

    val_sum = val_il_sum + val_ir_sum;


    if ((ACI_Active) && (Aux_Source == 0))
    {
        float a_sum = 1e-12f;
        for (unsigned i = 0; i < period_master; i++)
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
RKR::Control_Volume(const float *origl, const float *origr)
{
    float il_sum = 1e-12f;
    float ir_sum = 1e-12f;

    float tmp = 0.0f;
    float Temp_M_Volume = 0.0f;

    if ((flpos)&&(have_signal))
    {
        if (db6booster)
        { // +6dB Final Limiter in settings/audio
            for (unsigned i = 0; i < period_master; i++)
            {
                efxoutl[i] *= .5f;
                efxoutr[i] *= .5f;
            }
        }

        efx_FLimiter->out(efxoutl, efxoutr);

        if (db6booster)
        {
            for (unsigned i = 0; i < period_master; i++)
            {
                efxoutl[i] *= 2.0f;
                efxoutr[i] *= 2.0f;
            }
        }
    }

    memcpy(anall, efxoutl, sizeof (float)* period_master);
    memcpy(analr, efxoutr, sizeof (float)* period_master);

    if (upsample)
    {
        D_Resample->out(anall, analr, efxoutl, efxoutr, period_master, u_down);
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

    for (unsigned i = 0; i < period_master; i++)
    { //control volume

        efxoutl[i] *= Temp_M_Volume*booster; // +10dB booster main window
        efxoutr[i] *= Temp_M_Volume*booster;

        if (Active_Preset.Fraction_Bypass < 1.0f)
        { // FX% main window
            efxoutl[i] = (origl[i] * (1.0f - Active_Preset.Fraction_Bypass) + efxoutl[i] * Active_Preset.Fraction_Bypass);
            efxoutr[i] = (origr[i] * (1.0f - Active_Preset.Fraction_Bypass) + efxoutr[i] * Active_Preset.Fraction_Bypass);
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
            for (unsigned i = 0; i < period_master; i++)
            {
                efxoutl[i] *= .5f;
                efxoutr[i] *= .5f;
            }
        }

        efx_FLimiter->out(efxoutl, efxoutr); //then limit final output

        if (db6booster)
        {
            for (unsigned i = 0; i < period_master; i++)
            {
                efxoutl[i] *= 2.0f;
                efxoutr[i] *= 2.0f;
            }
        }
    }

    for (unsigned i = 0; i < period_master; i++)
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
    for(int i = 0; i < C_NUMBER_EFFECTS; i++)
    {
        if(Rack_Effects[i])
            Rack_Effects[i]->cleanup();
    }

    RC_Harm->cleanup();
    RC_Stereo_Harm->cleanup();
    efx_FLimiter->cleanup();
}

/**
 * The main audio processing for each active effect. This includes the non rack effects
 * such as Tap Tempo, Tuner, MIDI Converter. The effects are all processed in-line
 * using the class buffers efxoutl, efxoutr. The efxoutl, efxoutr here is directly 
 * memcpy from Jack input buffers.
 * 
 * @param origl
 *      The Jack in left channel, dry signal.
 * 
 * @param origr
 *      The Jack in right channel, dry signal.
 * 
 * @param 
 *      Void pointer unused.
 */
void
RKR::process_effects(float *origl, float *origr, void *)
{
    /* Don't process when updating quality since the effect is deleted */
    if(quality_update)
        return;

    if ((t_timeout) && (Tap_Active))
    {
        TapTempo_Timeout(1);
    }

    if (Active_Preset.FX_Master_Active)
    {
        Control_Gain(origl, origr);

        if (Metro_Active)
        {
            M_Metronome->metronomeout(m_ticks, period_master);
        }

        if ((Tap_Active) && (Tap_Selection == 4))
        {
            beat->detect(efxoutl, efxoutr, period_master);
            int bt_tempo = lrintf(beat->get_tempo());
            
            if ((bt_tempo > 19) && (bt_tempo < 360) && (bt_tempo != Tap_TempoSet))
            {
                Tap_TempoSet = bt_tempo;
                Update_tempo();
                Tap_Display = 1;
            }
        }

        if (Tuner_Active)
        {
            efx_Tuner->schmittFloat(period_master, efxoutl, efxoutr, HarmRecNote->freqs, HarmRecNote->lfreqs);
        }

        if (MIDIConverter_Active)
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

        if ((EFX_Active[EFX_HARMONIZER]) && (have_signal))
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


        if ((EFX_Active[EFX_STEREOHARM]) && (have_signal))
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

        if(EFX_Active[EFX_RING])
        {
            Ring *Efx_Ring = static_cast<Ring*>(Rack_Effects[EFX_RING]);
            if (Efx_Ring->Pafreq)
            {
                RingRecNote->schmittFloat(efxoutl, efxoutr);
                if ((RingRecNote->reconota != -1) && (RingRecNote->reconota != RingRecNote->last))
                {
                    if (RingRecNote->afreq > 0.0)
                    {
                        Efx_Ring->Pfreq = lrintf(RingRecNote->lafreq);
                        RingRecNote->last = RingRecNote->reconota;
                    }
                }
            }
        }

        // Apply the main rack effects in the efx_order by EFX_Index.
        for (int i = 0; i < C_NUMBER_ORDERED_EFFECTS; i++)
        {
            // Don't process inactive effects
            if(!EFX_Active[efx_order[i]])
                continue;
            
            // The effect out()
            Rack_Effects[efx_order[i]]->out(efxoutl, efxoutr);

            // Dry/Wet mix
            Rack_Effects[efx_order[i]]->Dry_Wet_Mix(efx_order[i], Rack_Effects[efx_order[i]]->outvolume,
                    efxoutl, efxoutr, smpl, smpr);
        }

        if (Metro_Active)
        {
            add_metro();
        }

        Control_Volume(origl, origr);
    }
}
