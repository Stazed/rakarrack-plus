//spencer jackson

#ifndef PROCESS_H
#define PROCESS_H

//main class for processing engine
#include "global.h"

#include "Reverb.h"
#include "Chorus.h"
#include "Echo.h"
#include "Phaser.h"
#include "Distorsion.h"
#include "EQ.h"
#include "Compressor.h"
#include "Alienwah.h"
#include "Cabinet.h"
#include "WahWah.h"
#include "Pan.h"
#include "Harmonizer.h"
#include "MusicDelay.h"
#include "Gate.h"
#include "Tuner.h"
#include "MIDIConverter.h"
#include "RecognizeNote.h"
#include "RecChord.h"
#include "Derelict.h"
#include "APhaser.h"
#include "Valve.h"
#include "Dual_Flange.h"
#include "Ring.h"
#include "Exciter.h"
#include "DistBand.h"
#include "Arpie.h"
#include "Expander.h"
#include "Shuffle.h"
#include "Synthfilter.h"
#include "VaryBand.h"
#include "Convolotron.h"
#include "Resample.h"
#include "Looper.h"
#include "MuTroMojo.h"
#include "Echoverse.h"
#include "CoilCrafter.h"
#include "ShelfBoost.h"
#include "Vocoder.h"
#include "Sustainer.h"
#include "Sequence.h"
#include "Shifter.h"
#include "StompBox.h"
#include "Reverbtron.h"
#include "Echotron.h"
#include "StereoHarm.h"
#include "CompBand.h"
#include "Opticaltrem.h"
#include "Vibe.h"
#include "Infinity.h"
#include "ParametricEQ.h"
#include "beattracker.h"
#include <jack/jack.h>

/**
 * The effect index used by switch(): case: and order number.
 */
enum EFX_Index
{
    EFX_EQ = 0,
    EFX_COMPRESSOR,
    EFX_DISTORSION,
    EFX_OVERDRIVE,
    EFX_ECHO,
    EFX_CHORUS,
    EFX_PHASER,
    EFX_FLANGER,
    EFX_REVERB,
    EFX_PARAMETRIC,
    EFX_WAHWAH,         // 10
    EFX_ALIENWAH,
    EFX_CABINET,
    EFX_PAN,
    EFX_HARMONIZER,
    EFX_MUSICAL_DELAY,
    EFX_NOISEGATE,
    EFX_DERELICT,
    EFX_ANALOG_PHASER,
    EFX_VALVE,
    EFX_DUAL_FLANGE,    // 20
    EFX_RING,
    EFX_EXCITER,
    EFX_DISTBAND,
    EFX_ARPIE,
    EFX_EXPANDER,
    EFX_SHUFFLE,
    EFX_SYNTHFILTER,
    EFX_VARYBAND,
    EFX_CONVOLOTRON,
    EFX_LOOPER,         // 30
    EFX_MUTROMOJO,
    EFX_ECHOVERSE,
    EFX_COILCRAFTER,
    EFX_SHELFBOOST,
    EFX_VOCODER,
    EFX_SUSTAINER,
    EFX_SEQUENCE,
    EFX_SHIFTER,
    EFX_STOMPBOX,
    EFX_REVERBTRON,     // 40
    EFX_ECHOTRON,
    EFX_STEREOHARM,
    EFX_COMPBAND,
    EFX_OPTICALTREM,
    EFX_VIBE,
    EFX_INFINITY        // 46
};

/*
 * The effect order used by int lv[70][20] and Preset_Bank_Struct.lv[70][20].
 * This the same array order as the presets_default[47][19] array.
 * This index should be used when accessing the two lv[][] arrays.
 * The Preset_Bank_Struct is used for file saving, and thus this index order is
 * saved to file. I do not know what lv is supposed to refer, gonna assume something
 * like: List Vector...
 * 
 * WARNING!!!
 * Index 10, LV_ORDER contains the order based on the EFX_Index, not this index.
 */
enum LV_Index
{
    LV_REVERB = 0,
    LV_ECHO,
    LV_CHORUS,
    LV_FLANGER,
    LV_PHASER,
    LV_OVERDRIVE,
    LV_DISTORSION,
    LV_EQ,
    LV_PARAMETRIC,
    LV_COMPRESSOR,
    LV_ORDER,           // 10
    LV_WAHWAH,
    LV_ALIENWAH,
    LV_CABINET,
    LV_PAN,
    LV_HARMONIZER,
    LV_MUSICAL_DELAY,
    LV_NOISEGATE,
    LV_DERELICT,
    LV_ANALOG_PHASER,
    LV_VALVE,           // 20
    LV_DUAL_FLANGE,
    LV_RING,
    LV_EXCITER,
    LV_DISTBAND,
    LV_ARPIE,
    LV_EXPANDER,
    LV_SHUFFLE,
    LV_SYNTHFILTER,
    LV_VARYBAND,
    LV_CONVOLOTRON,     // 30
    LV_LOOPER,
    LV_MUTROMOJO,
    LV_ECHOVERSE,
    LV_COILCRAFTER,
    LV_SHELFBOOST,
    LV_VOCODER,
    LV_SUSTAINER,
    LV_SEQUENCE,
    LV_SHIFTER,
    LV_STOMPBOX,        // 40
    LV_REVERBTRON,
    LV_ECHOTRON,
    LV_STEREOHARM,
    LV_COMPBAND,
    LV_OPTICALTREM,
    LV_VIBE,
    LV_INFINITY        // 47
};

/**
 * This is the "MIDI Implementation Chart" listed in Rakarrack-plus help.
 * For default MIDI processing (Not MIDI learn) this index directly corresponds
 * to the MIDI Control Change values (0 - 127). When MIDI learn is used, the
 * direct correspondence is no longer valid since each index item is mapped to
 * the user selected Learn. This index plus the MIDI_Learn_Index combined is
 * the total of MIDI controllable parameters.
 * 
 * The index labels related to rack effects parameters are the same as the 
 * effect parameter index, with the added MC_ prefix. 
 * Example: WahWah_Depth >> MC_WahWah_Depth.
 */
enum MIDI_Control_Default_Index
{
    MC_Unused_0 = 0,

    MC_WahWah_Depth,
    MC_Dere_Drive,
    MC_Dere_Level,
    MC_Dere_LPF,
    MC_Dere_HPF,
    MC_Dere_Color,

    MC_Output_Volume,           // 7

    MC_Dere_Suboctave,
    MC_Dist_Suboctave,

    MC_Unused_10,
    MC_Unused_11,

    MC_Balance_FX,              // 12

    MC_Unused_13,

    MC_Input_Volume,            // 14

    MC_Unused_15,
    MC_Unused_16,
    MC_Unused_17,
    MC_Unused_18,
    MC_Unused_19,

    MC_Alien_Depth,             // 20
    MC_Phaser_Depth,
    MC_Flanger_Depth,
    MC_Chorus_Depth,
    MC_Music_Gain_1,
    MC_Music_Gain_2,
    MC_Harm_Filter_Freq,
    MC_Harm_Interval,
    MC_WahWah_DryWet,
    MC_Overdrive_DryWet,
    MC_Dist_DryWet,             // 30
    MC_Harm_DryWet,

    MC_Unused_32,
    MC_Unused_33,
    MC_Unused_34,
    MC_Unused_35,
    MC_Unused_36,
    MC_Unused_37,
    MC_Unused_38,
    MC_Unused_39,
    MC_Unused_40,
    MC_Unused_41,
    MC_Unused_42,
    MC_Unused_43,
    MC_Unused_44,
    MC_Unused_45,

    MC_Echo_Pan,
    MC_Overdrive_Pan,
    MC_Dist_Pan,
    MC_Harm_Pan,
    MC_Chorus_Pan,              // 50
    MC_Flanger_Pan,
    MC_Chorus_DryWet,
    MC_Flanger_DryWet,
    MC_Phaser_DryWet,
    MC_Alien_DryWet,
    MC_Music_DryWet,
    MC_Reverb_DryWet,
    MC_Pan_DryWet,
    MC_Echo_DryWet,
    MC_Phaser_Pan,              // 60
    MC_Alien_Pan,
    MC_Music_Pan_1,
    MC_Reverb_Pan,

    MC_Unused_64,

    MC_Music_Pan_2,
    MC_WahWah_Pan,
    MC_Pan_Pan,
    MC_Overdrive_Drive,
    MC_Dist_Drive,
    MC_Overdrive_Level,         // 70
    MC_Dist_Level,
    MC_Chorus_LFO_Tempo,
    MC_Flanger_LFO_Tempo,
    MC_Phaser_LFO_Tempo,
    MC_WahWah_LFO_Tempo,
    MC_Alien_LFO_Tempo,
    MC_Pan_LFO_Tempo,
    MC_Echo_Feedback,
    MC_Chorus_Feedback,
    MC_Flanger_Feedback,        // 80
    MC_Phaser_Feedback,
    MC_Alien_Feedback,
    MC_Music_Feedback_1,
    MC_Music_Feedback_2,
    MC_Overdrive_LPF,
    MC_Dist_LPF,
    MC_Reverb_LPF,
    MC_Overdrive_HPF,
    MC_Dist_HPF,
    MC_Reverb_HPF,              // 90
    MC_Chorus_LR_Cross,
    MC_Flanger_LR_Cross,
    MC_Phaser_LR_Cross,
    MC_Overdrive_LR_Cross,
    MC_Dist_LR_Cross,
    MC_Alien_LR_Cross,
    MC_Echo_LR_Cross,
    MC_Music_LR_Cross,
    MC_Chorus_LFO_Stereo,
    MC_Flanger_LFO_Stereo,      // 100
    MC_Phaser_LFO_Stereo,
    MC_WahWah_LFO_Stereo,
    MC_Alien_LFO_Stereo,
    MC_Pan_LFO_Stereo,
    MC_Chorus_LFO_Random,
    MC_Flanger_LFO_Random,
    MC_Phaser_LFO_Random,
    MC_WahWah_LFO_Random,
    MC_Alien_LFO_Random,
    MC_Pan_LFO_Random,          // 110
    MC_WahWah_Sense,
    MC_WahWah_ASI,
    MC_WahWah_Smooth,
    MC_Phaser_Phase,
    MC_Alien_Phase,

    MC_Multi_On_Off,            // 116

    MC_APhase_DryWet,
    MC_APhase_Distortion,
    MC_APhase_LFO_Tempo,
    MC_APhase_Depth,            // 120
    MC_APhase_Width,
    MC_APhase_Feedback,
    MC_APhase_Mismatch,
    MC_APhase_LFO_Stereo,
    MC_Dere_DryWet,
    MC_Dere_Pan,
    MC_Dere_LR_Cross            // 127
};

enum MIDI_Learn_Index
{
    MC_Unused_128 = 128,
    MC_Unused_129,
    MC_EQ_Gain,                 // 130
    MC_EQ_Q,
    MC_EQ_31_HZ,
    MC_EQ_63_HZ,
    MC_EQ_125_HZ,
    MC_EQ_250_HZ,
    MC_EQ_500_HZ,
    MC_EQ_1_KHZ,
    MC_EQ_2_KHZ,
    MC_EQ_4_KHZ,
    MC_EQ_8_KHZ,                // 140
    MC_EQ_16_KHZ,
    MC_Compress_Attack,
    MC_Compress_Release,
    MC_Compress_Ratio,
    MC_Compress_Knee,
    MC_Compress_Threshold,
    MC_Compress_Output,
    MC_Parametric_Gain,
    MC_Parametric_Low_Freq,
    MC_Parametric_Low_Gain,     // 150
    MC_Parametric_Low_Q,
    MC_Parametric_Mid_Freq,
    MC_Parametric_Mid_Gain,
    MC_Parametric_Mid_Q,
    MC_Parametric_High_Freq,
    MC_Parametric_High_Gain,
    MC_Parametric_High_Q,
    MC_DFlange_DryWet,
    MC_DFlange_Pan,
    MC_DFlange_LR_Cross,        // 160
    MC_DFlange_Depth,
    MC_DFlange_Width,
    MC_DFlange_Offset,
    MC_DFlange_Feedback,
    MC_DFlange_LPF,
    MC_DFlange_LFO_Tempo,
    MC_DFlange_LFO_Stereo,
    MC_DFlange_LFO_Random,
    MC_Valve_DryWet,
    MC_Valve_LR_Cross,          // 170
    MC_Valve_Pan,
    MC_Valve_Level,
    MC_Valve_Drive,
    MC_Valve_Distortion,
    MC_Valve_Presence,
    MC_Valve_LPF,
    MC_Valve_HPF,
    MC_Ring_DryWet,
    MC_Ring_LR_Cross,
    MC_Ring_Input,              // 180
    MC_Ring_Level,
    MC_Ring_Pan,
    MC_Ring_Depth,
    MC_Ring_Freq,
    MC_Ring_Sine,
    MC_Ring_Triangle,
    MC_Ring_Saw,
    MC_Ring_Square,
    MC_Exciter_Gain,
    MC_Exciter_LPF,             // 190
    MC_Exciter_HPF,
    MC_Exciter_Harm_1,
    MC_Exciter_Harm_2,
    MC_Exciter_Harm_3,
    MC_Exciter_Harm_4,
    MC_Exciter_Harm_5,
    MC_Exciter_Harm_6,
    MC_Exciter_Harm_7,
    MC_Exciter_Harm_8,
    MC_Exciter_Harm_9,          // 200
    MC_Exciter_Harm_10,
    MC_DistBand_DryWet,
    MC_DistBand_LR_Cross,
    MC_DistBand_Drive,
    MC_DistBand_Level,
    MC_DistBand_Gain_Low,
    MC_DistBand_Gain_Mid,
    MC_DistBand_Gain_Hi,
    MC_DistBand_Cross_1,
    MC_DistBand_Cross_2,        // 210
    MC_DistBand_Pan,
    MC_Arpie_DryWet,
    MC_Arpie_ArpeWD,
    MC_Arpie_Pan,
    MC_Arpie_Tempo,
    MC_Arpie_LR_Delay,
    MC_Arpie_LR_Cross,
    MC_Arpie_Feedback,
    MC_Arpie_Damp,
    MC_Expander_Attack,         // 220
    MC_Expander_Release,
    MC_Expander_Shape,
    MC_Expander_Threshold,
    MC_Expander_Gain,
    MC_Expander_LPF,
    MC_Expander_HPF,
    MC_Shuffle_DryWet,
    MC_Shuffle_Freq_L,
    MC_Shuffle_Gain_L,
    MC_Shuffle_Freq_ML,         // 230
    MC_Shuffle_Gain_ML,
    MC_Shuffle_Freq_MH,
    MC_Shuffle_Gain_MH,
    MC_Shuffle_Freq_H,
    MC_Shuffle_Gain_H,
    MC_Shuffle_Width,
    MC_Synthfilter_DryWet,
    MC_Synthfilter_Distort,
    MC_Synthfilter_LFO_Tempo,
    MC_Synthfilter_LFO_Stereo,  // 240
    MC_Synthfilter_Width,
    MC_Synthfilter_Feedback,
    MC_Synthfilter_Depth,
    MC_Synthfilter_Env_Sens,
    MC_Synthfilter_Attack,
    MC_Synthfilter_Release,
    MC_Synthfilter_Offset,
    MC_VaryBand_DryWet,
    MC_VaryBand_LFO_Tempo_1,
    MC_VaryBand_LFO_Stereo_1,   // 250
    MC_VaryBand_LFO_Tempo_2,
    MC_VaryBand_LFO_Stereo_2,
    MC_VaryBand_Cross_1,
    MC_VaryBand_Cross_2,
    MC_VaryBand_Cross_3,
    MC_MuTro_DryWet,
    MC_MuTro_LowPass,
    MC_MuTro_BandPass,
    MC_MuTro_HighPass,
    MC_MuTro_Depth,             // 260
    MC_MuTro_LFO_Tempo,
    MC_MuTro_Resonance,
    MC_MuTro_Range,
    MC_MuTro_Wah,
    MC_MuTro_Env_Sens,
    MC_MuTro_Env_Smooth,
    MC_Looper_DryWet,
    MC_Looper_Level_1,
    MC_Looper_Level_2,
    MC_Looper_Reverse,          // 270
    MC_Looper_AutoPlay,
    MC_Looper_Play,
    MC_Looper_Stop,
    MC_Looper_Record,
    MC_Looper_Rec_1,
    MC_Looper_Rec_2,
    MC_Looper_Track_1,
    MC_Looper_Track_2,
    MC_Looper_Clear,
    MC_Convo_DryWet,            // 280
    MC_Convo_Pan,
    MC_Convo_Level,
    MC_Convo_Damp,
    MC_Convo_Feedback,
    MC_Convo_Length,
    MC_Coil_Gain,
    MC_Coil_Tone,
    MC_Coil_Freq_1,
    MC_Coil_Q_1,
    MC_Coil_Freq_2,             // 290
    MC_Coil_Q_2,
    MC_Shelf_Gain,
    MC_Shelf_Level,
    MC_Shelf_Tone,
    MC_Shelf_Presence,
    MC_Vocoder_DryWet,
    MC_Vocoder_Pan,
    MC_Vocoder_Input,
    MC_Vocoder_Smear,
    MC_Vocoder_Q,               // 300
    MC_Vocoder_Ring,
    MC_Vocoder_Level,
    MC_Echoverse_DryWet,
    MC_Echoverse_Reverse,
    MC_Echoverse_Pan,
    MC_Echoverse_Tempo,
    MC_Echoverse_LR_Delay,
    MC_Echoverse_Feedback,
    MC_Echoverse_Damp,
    MC_Echoverse_Ext_Stereo,    // 310
    MC_Echoverse_Angle,
    MC_Sustain_Gain,
    MC_Sustain_Sustain,
    MC_Sequence_DryWet,
    MC_Sequence_Step_1,
    MC_Sequence_Step_2,
    MC_Sequence_Step_3,
    MC_Sequence_Step_4,
    MC_Sequence_Step_5,
    MC_Sequence_Step_6,         // 320
    MC_Sequence_Step_7,
    MC_Sequence_Step_8,
    MC_Sequence_Tempo,
    MC_Sequence_Resonance,
    MC_Sequence_Stdf,
    MC_Shifter_DryWet,
    MC_Shifter_Interval,
    MC_Shifter_Gain,
    MC_Shifter_Pan,
    MC_Shifter_Attack,          // 330
    MC_Shifter_Decay,
    MC_Shifter_Threshold,
    MC_Shifter_Whammy,
    MC_Stomp_Level,
    MC_Stomp_Gain,
    MC_Stomp_Bias,
    MC_Stomp_Mid,
    MC_Stomp_Tone,
    MC_Revtron_DryWet,
    MC_Revtron_Pan,             // 340
    MC_Revtron_Level,
    MC_Revtron_Damp,
    MC_Revtron_Feedback,
    MC_Revtron_Length,
    MC_Revtron_Stretch,
    MC_Revtron_I_Delay,
    MC_Revtron_Fade,
    MC_Echotron_DryWet,
    MC_Echotron_Pan,
    MC_Echotron_Tempo,          // 350
    MC_Echotron_Damp,
    MC_Echotron_Feedback,
    MC_Echotron_LR_Cross,
    MC_Echotron_LFO_Width,
    MC_Echotron_Depth,
    MC_Echotron_LFO_Stereo,
    MC_Echotron_Taps,
    MC_Sharm_DryWet,
    MC_Sharm_L_Interval,
    MC_Sharm_L_Chroma,          // 360
    MC_Sharm_L_Gain,
    MC_Sharm_R_Interval,
    MC_Sharm_R_Chroma,
    MC_Sharm_R_Gain,
    MC_Sharm_LR_Cross,
    MC_Sharm_Note,
    MC_Sharm_Chord,
    MC_CompBand_DryWet,
    MC_CompBand_Gain,
    MC_CompBand_Low_Ratio,      // 370
    MC_CompBand_Mid_1_Ratio,
    MC_CompBand_Mid_2_Ratio,
    MC_CompBand_High_Ratio,
    MC_CompBand_Low_Thresh,
    MC_CompBand_Mid_1_Thresh,
    MC_CompBand_Mid_2_Thresh,
    MC_CompBand_High_Thresh,
    MC_CompBand_Cross_1,
    MC_CompBand_Cross_2,
    MC_CompBand_Cross_3,        // 380
    MC_Optical_Depth,
    MC_Optical_LFO_Tempo,
    MC_Optical_LFO_Random,
    MC_Optical_LFO_Stereo,
    MC_Optical_Pan,
    MC_Vibe_DryWet,
    MC_Vibe_Width,
    MC_Vibe_Depth,
    MC_Vibe_LFO_Tempo,
    MC_Vibe_LFO_Random,         // 390
    MC_Vibe_LFO_Stereo,
    MC_Vibe_Feedback,
    MC_Vibe_LR_Cross,
    MC_Vibe_Pan,
    MC_Infinity_DryWet,
    MC_Infinity_Resonance,
    MC_Infinity_AutoPan,
    MC_Infinity_LR_Delay,
    MC_Infinity_Start,
    MC_Infinity_End,            // 400
    MC_Infinity_Tempo,
    MC_Infinity_Subdivision,
    MC_Alien_Delay,
    MC_APhase_LFO_Random,
    MC_Cabinet_Gain,
    MC_Chorus_Delay,
    MC_Echo_Reverse,
    MC_Echo_Delay,
    MC_Echo_LR_Delay,
    MC_Echo_Damp,               // 410
    MC_Flanger_Delay,
    MC_Harm_Gain,
    MC_Harm_Filter_Gain,
    MC_Harm_Filter_Q,
    MC_Infinity_Band_1,
    MC_Infinity_Band_2,
    MC_Infinity_Band_3,
    MC_Infinity_Band_4,
    MC_Infinity_Band_5,
    MC_Infinity_Band_6,         // 420    
    MC_Infinity_Band_7,
    MC_Infinity_Band_8,
    MC_Looper_Tempo,
    MC_Music_Tempo,
    MC_Music_Damp,
    MC_MuTro_LFO_Random,
    MC_MuTro_LFO_Stereo,
    MC_MuTro_St_Freq,
    MC_Gate_Attack,
    MC_Gate_Release,            // 430
    MC_Gate_Range,
    MC_Gate_Threshold,
    MC_Gate_Hold,
    MC_Gate_LPF,
    MC_Gate_HPF,
    MC_Pan_Ex_St_Amt,
    MC_Reverb_Time,
    MC_Reverb_I_Delay,
    MC_Reverb_Delay_FB,
    MC_Reverb_Room,             // 440
    MC_Reverb_Damp,
    MC_Revtron_Diffusion,
    MC_Revtron_LPF,
    MC_Harm_Note,
    MC_Harm_Chord,
    MC_Synthfilter_LFO_Random,
    MC_Harm_Select,
    MC_Sharm_Select,
    MC_Dist_Type,
    MC_Overdrive_Type,          // 450
    MC_Dere_Type,
    MC_DistBand_Type_Low,
    MC_DistBand_Type_Mid,
    MC_DistBand_Type_Hi,
    MC_Stomp_Mode,
    MC_Alien_LFO_Type,
    MC_APhase_LFO_Type,
    MC_Chorus_LFO_Type,
    MC_Flanger_LFO_Type,
    MC_DFlange_LFO_Type,        // 460
    MC_Echotron_LFO_Type,
    MC_MuTro_LFO_Type,
    MC_Optical_LFO_Type,
    MC_Pan_LFO_Type,
    MC_Phaser_LFO_Type,
    MC_Synthfilter_LFO_Type,
    MC_VaryBand_LFO_Type_1,
    MC_VaryBand_LFO_Type_2,
    MC_Vibe_LFO_Type,
    MC_WahWah_LFO_Type          // 470
};

/**
 * The total Number of MIDI controllable parameters.
 * This is the total of listed items in the MIDI learn window.
 * If any new parameters are added, this must be adjusted.
 */
const int C_MC_PARAMETER_SIZE = 445;

/**
 * Magic number 26 is the Unused cases from the default MIDI control
 * (0, 10, 11, 13, (15 > 19), (32 > 45), 64) = 24 -- Unused
 * plus 128, 129 also Unused  = 24 + 2 = 26.
 * If any of these unused parameters are used and added to
 * C_MC_PARAMETER_SIZE, this value must be adjusted.
 */
const int C_MC_UNUSED_PARAMETERS = 26;

/**
 * The total MIDI control range used in switch() case function.
 */
const int C_MC_RANGE = C_MC_PARAMETER_SIZE + C_MC_UNUSED_PARAMETERS;

/**
 * The number of rack effects.
 */
const int C_NUMBER_EFFECTS = 47;

/**
 * The number of rack effects plus one (index 10) for main window displayed effects.
 * See fileio.C presets_default[][].
 */
const int C_NUM_EFFECTS_PLUS_ORDER = C_NUMBER_EFFECTS + 1;

/**
 * The maximum effect parameter size. See fileio.C presets_default[][].
 */
const int C_MAX_PARAMETER_SIZE = 19;


class RKR
{

public:

    RKR ();
    ~RKR ();

    // process.C
    int jack_open_client();
    void load_user_preferences();
    void instantiate_effects();
    void initialize_arrays();
    void put_order_in_rack();
    void Adjust_Upsample();
    void add_metro();
    void Vol_Efx (int NumEffect, float volume);
    void Vol2_Efx ();
    void Vol3_Efx ();
    void calculavol (int i);
    int checkforaux();
    void Control_Gain (float *origl, float *origr);
    void Control_Volume (float *origl, float *origr);
    void cleanup_efx ();
    void Alg (float *origl, float *origr ,void *);

    // rkrMIDI.C
    void MIDI_control();
    void ConnectMIDI ();
    void InitMIDI ();
    void miramidi ();
    void midievents();
    void ActOnOff();
    void ActiveUn(int value);
    int checkonoff(int value);
    void Conecta ();
    void conectaaconnect ();
    void disconectaaconnect ();
    void jack_process_midievents (jack_midi_event_t *midievent);
    void process_midi_controller_events(int parameter, int value);
    int ret_Tempo(int value);
    int ret_LPF(int value);
    int ret_HPF(int value);

    // fileio.C
    void putbuf (char *buf, int j);
    void getbuf (char *buf, int j);
    void savefile (char *filename);
    void loadfile (char *filename);
    void file_error(FILE *fn);
    void Actualizar_Audio ();
    void loadnames();
    int loadbank (char *filename);
    int savebank (char *filename);
    void New ();
    void New_Bank ();
    void Bank_to_Preset (int Num);
    void Preset_to_Bank (int i);
    int BigEndian();
    void copy_IO();
    void convert_IO();
    void fix_endianess();
    void saveskin (char *filename);
    bool loadskin (char *filename);
    void load_skin_error(FILE *fn);
    void dump_preset_names ();
    int CheckOldBank(char *filename);
    void ConvertOldFile(char *filename);
    void ConvertReverbFile(char * filename);
    void SaveIntPreset(int num, char *name);
    void DelIntPreset(int num, char *name);
    bool MergeIntPreset(char *filename);
    void savemiditable (char *filename);
    void loadmiditable (char *filename);

    // varios.C
    int Message (int prio, const char *labelwin, const char *message_text);
    void Error_Handle(int num);
    char *PrefNom (const char *dato);
    int Get_Bogomips();
    int TapTempo();
    void TapTempo_Timeout(int state);
    void Update_tempo();

    // class pointers
    class Reverb *efx_Rev;
    class Chorus *efx_Chorus;
    class Chorus *efx_Flanger;
    class Phaser *efx_Phaser;
    class Analog_Phaser *efx_APhaser;
    class EQ *efx_EQ1;
    class ParametricEQ *efx_EQ2;
    class Echo *efx_Echo;
    class Distorsion *efx_Distorsion;
    class Distorsion *efx_Overdrive;
    class Compressor *efx_Compressor;
    class WahWah *efx_WahWah;
    class Alienwah *efx_Alienwah;
    class Cabinet *efx_Cabinet;
    class Pan *efx_Pan;
    class Harmonizer *efx_Har;
    class MusicDelay *efx_MusDelay;
    class Gate *efx_Gate;
    class Derelict *efx_Derelict;
    class Tuner *efx_Tuner;
    class MIDIConverter *efx_MIDIConverter;
    class metronome *M_Metronome;
    class beattracker *beat;

    class Recognize *HarmRecNote;
    class Recognize *StHarmRecNote;
    class Recognize *RingRecNote;
    class RecChord *RC_Harm;
    class RecChord *RC_Stereo_Harm;
    class Compressor *efx_FLimiter;
    class Valve *efx_Valve;
    class Dflange *efx_DFlange;
    class Ring *efx_Ring;
    class Exciter *efx_Exciter;
    class DistBand *efx_DistBand;
    class Arpie *efx_Arpie;
    class Expander *efx_Expander;
    class Synthfilter *efx_Synthfilter;
    class Shuffle *efx_Shuffle;
    class VaryBand *efx_VaryBand;
    class Convolotron *efx_Convol;
    class Resample *U_Resample;
    class Resample *D_Resample;
    class Resample *A_Resample;
    class AnalogFilter *DC_Offsetl;
    class AnalogFilter *DC_Offsetr;
    class Looper *efx_Looper;
    class MuTroMojo *efx_MuTroMojo;
    class Echoverse *efx_Echoverse;
    class CoilCrafter *efx_CoilCrafter;
    class ShelfBoost *efx_ShelfBoost;
    class Vocoder *efx_Vocoder;
    class Sustainer *efx_Sustainer;
    class Sequence *efx_Sequence;
    class Shifter *efx_Shifter;
    class StompBox *efx_StompBox;
    class Reverbtron *efx_Reverbtron;
    class Echotron *efx_Echotron;
    class StereoHarm *efx_StereoHarm;
    class CompBand *efx_CompBand;
    class Opticaltrem *efx_Opticaltrem;
    class Vibe *efx_Vibe;
    class Infinity *efx_Infinity;

    jack_client_t *jackclient;
    jack_options_t options;
    jack_status_t status;
    char jackcliname[64];

    int db6booster;
    int jdis;
    int jshut;
    int DC_Offset;
    int Bypass;
    int MIDIConverter_Bypass;
    int Metro_Bypass;
    int Tuner_Bypass;
    int Tap_Bypass;
    int ACI_Bypass;
    int Reverb_Bypass;
    int Chorus_Bypass;
    int Flanger_Bypass;
    int Phaser_Bypass;
    int Overdrive_Bypass;
    int Distorsion_Bypass;
    int Echo_Bypass;
    int EQ1_Bypass;
    int EQ2_Bypass;
    int Compressor_Bypass;
    int WahWah_Bypass;
    int Alienwah_Bypass;
    int Cabinet_Bypass;
    int Pan_Bypass;
    int Harmonizer_Bypass;
    int MusDelay_Bypass;
    int Gate_Bypass;
    int Derelict_Bypass;
    int APhaser_Bypass;
    int Valve_Bypass;
    int DFlange_Bypass;
    int Ring_Bypass;
    int Exciter_Bypass;
    int DistBand_Bypass;
    int Arpie_Bypass;
    int Expander_Bypass;
    int Shuffle_Bypass;
    int Synthfilter_Bypass;
    int VaryBand_Bypass;
    int Convol_Bypass;
    int Looper_Bypass;
    int MuTroMojo_Bypass;
    int Echoverse_Bypass;
    int CoilCrafter_Bypass;
    int ShelfBoost_Bypass;
    int Vocoder_Bypass;
    int Sustainer_Bypass;
    int Sequence_Bypass;
    int Shifter_Bypass;
    int StompBox_Bypass;
    int Reverbtron_Bypass;
    int Echotron_Bypass;
    int StereoHarm_Bypass;
    int CompBand_Bypass;
    int Opticaltrem_Bypass;
    int Vibe_Bypass;
    int Infinity_Bypass;
    int Bypass_B;
    int Reverb_B;
    int Chorus_B;
    int Flanger_B;
    int Phaser_B;
    int APhaser_B;
    int DFlange_B;
    int Overdrive_B;
    int Distorsion_B;
    int Echo_B;
    int EQ1_B;
    int EQ2_B;
    int Compressor_B;
    int WahWah_B;
    int Alienwah_B;
    int Cabinet_B;
    int Pan_B;
    int Harmonizer_B;
    int MusDelay_B;
    int Gate_B;
    int Derelict_B;
    int Valve_B;
    int Ring_B;
    int Exciter_B;
    int DistBand_B;
    int Arpie_B;
    int Expander_B;
    int Shuffle_B;
    int Synthfilter_B;
    int VaryBand_B;
    int Convol_B;
    int Looper_B;
    int MuTroMojo_B;
    int Echoverse_B;
    int CoilCrafter_B;
    int ShelfBoost_B;
    int Vocoder_B;
    int Sustainer_B;
    int Sequence_B;
    int Shifter_B;
    int StompBox_B;
    int Reverbtron_B;
    int Echotron_B;
    int StereoHarm_B;
    int CompBand_B;
    int Opticaltrem_B;
    int Vibe_B;
    int Infinity_B;

    int Selected_Preset;
    int lv[70][20];
    int saved_order[16];
    int efx_order[16];
    int new_order[16];
    int availables[60];
    int active[12];
    int MidiCh;
    int HarCh;
    int StereoHarCh;
    int init_state;
    int actuvol;
    int help_displayed;
    int modified;
    int autoassign;
    int comemouse;
    int aconnect_MI;
    int aconnect_JA;
    int aconnect_JIA;

    int cuan_jack;
    int cuan_ijack;
    int IsCoIn;
    int Cyoin;
    int Pyoin;
    int Ccin;
    int Pcin;

    // bank of flags telling GUI which midi controlled items to update
    int Mcontrol[C_MC_RANGE];
    // flag telling GUI that Mcontrol has at least one set flag
    int RControl;
    int ControlGet;
    int CountWait;
    int XUserMIDI[128][20];

    bool quality_update;
    int eff_filter;
    int Har_Down;
    int Har_U_Q;
    int Har_D_Q;
    int Rev_Down;
    int Rev_U_Q;
    int Rev_D_Q;
    int Con_Down;
    int Con_U_Q;
    int Con_D_Q;
    int Shi_Down;
    int Shi_U_Q;
    int Shi_D_Q;
    int Seq_Down;
    int Seq_U_Q;
    int Seq_D_Q;
    int Voc_Down;
    int Voc_U_Q;
    int Voc_D_Q;
    int Ste_Down;
    int Ste_U_Q;
    int Ste_D_Q;
    float aFreq;

    int Metro_Vol;
    int M_Metro_Sound;
    int deachide;
    int scalable;
    int change_scale;
    int font;
    int flpos;
    int upsample;
    int UpQual;
    int DownQual;
    int UpAmo;
    int J_SAMPLE_RATE;
    int J_PERIOD;
    int m_displayed;
    int Mvalue;
    int Mnumeff[32];
    int OnOffC;

    //ssj
    int period;
    float fPeriod;
    int sample_rate;
    float fSample_rate;
    float cSample_rate;
    float *interpbuf;//buffer for analog filters to interpolate (shared)

    /* WaveShape Resampling */
    int Dist_res_amount;
    int Dist_up_q;
    int Dist_down_q;
    
    int Ovrd_res_amount;
    int Ovrd_up_q;
    int Ovrd_down_q;

    int Dere_res_amount;
    int Dere_up_q;
    int Dere_down_q;

    int DBand_res_amount;
    int DBand_up_q;
    int DBand_down_q;

    int Stomp_res_amount;
    int Stomp_up_q;
    int Stomp_down_q;
    /* End WaveShape */

    int sw_stat;
    int MIDIway;
    int fontsize;
    int swidth;
    int sheight;
    int sschema;
    int slabel_color;
    int sfore_color;
    int sback_color;
    int sleds_color;

    int have_signal;
    int OnCounter;
    int t_periods;

    // Harmonizer
    int HarQual;
    int SteQual;
    int SeqQual;
    int ShiQual;
    

    // Tap Tempo
    int tempocnt;
    int Tap_Display;
    int Tap_Selection;
    int Tap_TempoSet;
    int Tap_SetValue;
    int t_timeout;
    int jt_state;
    int Tap_Updated;
    int note_old;
    int cents_old;

    int cpufp;
    int mess_dis;
    int numpi, numpo, numpa, numpmi, numpmo;
    int numpc;
    int midi_table;
    int a_bank;
    int new_bank_loaded;

    int Aux_Gain;
    int Aux_Threshold;
    int Aux_MIDI;
    int Aux_Minimum;
    int Aux_Maximum;
    int Aux_Source;
    int last_auxvalue;
    int ena_tool;
    int VocBands;
    int RCOpti_Harm;
    int RCOpti_Stereo;
    int RCOpti_Ring;
    
    int M_Metro_Tempo;
    int M_Metro_Bar;
    int mtc_counter;
    int EnableBackgroundImage;
    int ML_filter;
    int ML_clist[150];

    long Tap_time_Init;

    double Tap_timeB;
    double Tap_timeC;
    double jt_tempo;

    double tempobuf[6];


    double u_down;
    double u_up;

    timeval timeA;

    float booster;
    float cpuload;
    float rtrig;

    float *efxoutl;
    float *efxoutr;
    float *auxdata;
    float *auxresampled;
    float *anall;
    float *analr;
    float *smpl;
    float *smpr;
    float *denormal;
    float *m_ticks;

    float Master_Volume;
    float Input_Gain;
    float Fraction_Bypass;
    float Log_I_Gain;
    float Log_M_Volume;
    float M_Metro_Vol;

    float val_sum;
    float old_il_sum;
    float old_ir_sum;
    float old_vl_sum;
    float old_vr_sum;
    float val_vl_sum;
    float val_vr_sum;
    float val_il_sum;
    float val_ir_sum;
    float old_a_sum;
    float val_a_sum;


    float bogomips;
    float looper_size;

    // Tuner
    float nfreq_old;
    float afreq_old;

    char tmpprefname[128];

    char *Preset_Name;
    char *Author;
    char *Bank_Saved;
    char *UserRealName;



    char MID[128];
    char BankFilename[128];
    char UDirFilename[128];
    char BackgroundImage[256];


    struct Effects_Names
    {
        char Nom[24];
        int Pos;
        int Type;
        
        Effects_Names():
            Nom(),
            Pos(),
            Type() {}
    } efx_names[70];

    struct Effects_Params
    {
        char Nom[32];
        int Ato;
        int Effect;
        
        Effects_Params():
            Nom(),
            Ato(),
            Effect() {}
    } efx_params[C_MC_PARAMETER_SIZE];


    struct Preset_Bank_Struct
    {
        char Preset_Name[64];
        char Author[64];
        char Classe[36];
        char Type[4];
        char ConvoFiname[128];
        char cInput_Gain[64];
        char cMaster_Volume[64];
        char cBalance[64];
        float Input_Gain;
        float Master_Volume;
        float Balance;
        int Bypass;
        char RevFiname[128];
        char EchoFiname[128];
        int lv[70][20];
        int XUserMIDI[128][20];
        int XMIDIrangeMin[128];
        int XMIDIrangeMax[128];
        
        Preset_Bank_Struct():
            Preset_Name(),
            Author(),
            Classe(),
            Type(),
            ConvoFiname(),
            cInput_Gain(),
            cMaster_Volume(),
            cBalance(),
            Input_Gain(),
            Master_Volume(),
            Balance(),
            Bypass(),
            RevFiname(),
            EchoFiname(),
            lv(),
            XUserMIDI(),
            XMIDIrangeMin(),
            XMIDIrangeMax() {}
    } Bank[62];


    struct MIDI_table
    {
        int bank;
        int preset;
        
        MIDI_table():
            bank(),
            preset() {}
    } M_table[128];

    struct Bank_Names
    {
        char Preset_Name[64];
        
        Bank_Names():
            Preset_Name()
        {
            Preset_Name[0] = 0;
        }
    } B_Names[4][62];


    // Alsa MIDI
    snd_seq_t *midi_in, *midi_out;


    struct JackPorts
    {
        char name[128];
        
        JackPorts():
            name() {}
    } jack_po[16],jack_poi[16];




};

struct list_element
{
    struct list_element *forward;
    struct list_element *backward;
    char *name;
    
    list_element():
        forward(),
        backward(),
        name() {}
};

#endif
