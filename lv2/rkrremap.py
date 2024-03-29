#!/usr/bin/env python3

# this function returns SPECIAL mappings
# in a tuple of format (nparams, index1, val1, index2, val2, ...)
# index and values must be run through remap again for port data
def remap_special(efxindex, paramindex, value):
    if value < 10000: #legacy method
        if (efxindex,paramindex) == (28,10): #varyband "combi"
            return{
                #v1 v2 1 0
                0 : (4, 12,0 ,13,0 ,14,1 ,15,1 ) ,
                1 : (4, 12,0 ,13,1 ,14,1 ,15,0 ) ,
                2 : (4, 12,0 ,13,1 ,14,0 ,15,1 ) ,
                3 : (4, 12,2 ,13,0 ,14,0 ,15,2 ) ,
                4 : (4, 12,2 ,13,0 ,14,1 ,15,2 ) ,
                5 : (4, 12,3 ,13,0 ,14,0 ,15,3 ) ,
                6 : (4, 12,3 ,13,0 ,14,1 ,15,3 ) ,
                7 : (4, 12,0 ,13,2 ,14,2 ,15,0 ) ,
                8 : (4, 12,0 ,13,2 ,14,2 ,15,1 ) ,
                9 : (4, 12,0 ,13,3 ,14,3 ,15,0 ) ,
                10 : (4,12,0 ,13,3 ,14,3 ,15,1 ) ,
            }[value]
    else: #rkr.lv2 method
        if (efxindex,paramindex) == (28,10):
            value -= 10000;
            PsL  = int(value / 1000);
            value %= 1000;
            PsML = int(value / 100);
            value %= 100;
            PsMH = int(value / 10);
            value %= 10;
            PsH  = int(value);
            return{
                0 : (4, 12,PsL ,13,PsML ,14,PsMH ,15,PsH ) ,
            }[0]
            
 #convo files
 #0{"Marshall JCM200", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #1"Fender Superchamp", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #2"Mesa Boogie", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #3"Mesa Boogie 2", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #4"Marshall Plexi", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #5"Bassman", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #6"JCM2000", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #7"Ampeg", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #8"Marshall2", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},

 #cab presets
 #0{"Marshall-4-12", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #1"Celestion G12M", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #2"Jensen Alnico P12N", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #3"Jensen Alnico P15N", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #4"Delta Demon", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #5"Celestion-EVH12", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #6"Eminence Copperhead", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #7"Mesa Boogie", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #8"Jazz-Chorus", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #9"Vox-Bright", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
 #10"Marshall-I", 0,  0, 0, 0, FL_NORMAL_LABEL, 0, 10, 0},
#    if (efxindex,paramindex) == (29,8): #convolutron approximated by cab preset
#        return{
#            0 : (1, 10,10) ,
#            1 : (1, 10,1) ,
#            2 : (1, 10,7) ,
#            3 : (1, 10,7) ,
#            4 : (1, 10,0) ,
#            5 : (1, 10,7) ,
#            6 : (1, 10,7) ,
#            7 : (1, 10,4) ,
#            8 : (1, 10,0) ,
#        }[value]
    if (efxindex,paramindex) == (31,17): #mutron "mode"
        return{
            0 : (2, 20,0, 21,0) ,
            1 : (2, 20,1, 21,0) ,
            2 : (2, 20,0, 21,1) ,
            3 : (2, 20,1, 21,1) ,
            4 : (2, 20,0, 21,0) ,
        }[value]
    return (0,)        

def remap(efxindex, paramindex):
    return{

         (0, -1) : ('RakarrackPlus EQ', 'https://github.com/Stazed/rakarrack-plus#eql') ,
         (0, 0) : (3, '31 Hz', 'HZ31', -64) ,
         (0, 1) : (4, '63 Hz', 'HZ63', -64) ,
         (0, 2) : (5, '125 Hz', 'HZ125', -64) ,
         (0, 3) : (6, '250 Hz', 'HZ250', -64) ,
         (0, 4) : (7, '500 Hz', 'HZ500', -64) ,
         (0, 5) : (8, '1 kHz', 'KHZ1', -64) ,
         (0, 6) : (9, '2 kHz', 'KHZ2', -64) ,
         (0, 7) : (10, '4 kHz', 'KHZ4', -64) ,
         (0, 8) : (11, '8 kHz', 'KHZ8', -64) ,
         (0, 9) : (12, '16 kHz', 'KHZ16', -64) ,
         (0, 10) : (1, 'Gain', 'GAIN', -64) ,
         (0, 11) : (2, 'Q', 'Q', -64) ,
         (0, 12) : (-1,) ,

         (1, -1) : ('RakarrackPlus Compressor', 'https://github.com/Stazed/rakarrack-plus#comp') ,
         (1, 0) : (1, 'Threshold', 'THRESHOLD', 0) ,
         (1, 1) : (2, 'RATIO', 'RATIO', 0) ,
         (1, 2) : (3, 'Output Gain', 'OUTPUT', 0) ,
         (1, 3) : (4, 'Attack Time', 'ATTACK', 0) ,
         (1, 4) : (5, 'Release Time', 'RELEASE', 0) ,
         (1, 5) : (6, 'Auto Output', 'AUTOOUT', 0) ,
         (1, 6) : (7, 'Knee', 'KNEE', 0) ,
         (1, 7) : (8, 'Stereo', 'STEREO', 0) ,
         (1, 8) : (9, 'Peak', 'PEAK', 0) ,
         (1, 9) : (-1,) ,

         (2, -1) : ('RakarrackPlus Distorsion', 'https://github.com/Stazed/rakarrack-plus#dist') ,
         (2, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (2, 1) : (2, 'Panning', 'PAN', -64) ,
         (2, 2) : (3, 'Left/Right Crossover', 'LRCr', 0) ,
         (2, 3) : (4, 'Drive', 'DRIVE', 0) ,
         (2, 4) : (5, 'Level', 'LEVEL', 0) ,
         (2, 5) : (6, 'Type', 'TYPE', 0) ,
         (2, 6) : (7, 'Negate (Polarity Switch)', 'NEG', 0) ,
         (2, 7) : (8, 'Lowpass Filter', 'LPF', 0) ,
         (2, 8) : (9, 'Highpass Filter', 'HPF', 0) ,
         (2, 9) : (10, 'Stereo', 'STEREO', 0) ,
         (2, 10) : (11, 'Prefilter', 'PREFILTER', 0) ,
         (2, 11) : (0, 'SKIP', 'SKIP', 0) , #not used
         (2, 12) : (12, 'Suboctave', 'OCT', 0) ,
         (2, 13) : (-1,) ,

         (3, -1) : ('RakarrackPlus Overdrive', 'https://github.com/Stazed/rakarrack-plus#Overdrive') ,
         (3, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (3, 1) : (2, 'Panning', 'PAN', -64) ,
         (3, 2) : (3, 'Left/Right Crossover', 'LRCr', 0) ,
         (3, 3) : (4, 'Drive', 'DRIVE', 0) ,
         (3, 4) : (5, 'Level', 'LEVEL', 0) ,
         (3, 5) : (6, 'Type', 'TYPE', 0) ,
         (3, 6) : (7, 'Negate (Polarity Switch)', 'NEG', 0) ,
         (3, 7) : (8, 'Lowpass Filter', 'LPF', 0) ,
         (3, 8) : (9, 'Highpass Filter', 'HPF', 0) ,
         (3, 9) : (10, 'Stereo', 'STEREO', 0) ,
         (3, 10) : (11, 'Prefilter', 'PREFILTER', 0) ,
         (3, 11) : (0, 'SKIP', 'SKIP', 0) , #not used
         (3, 12) : (0, 'SKIP', 'SKIP', 0) , #Suboctave not used
         (3, 13) : (-1,) ,

         (4, -1) : ('RakarrackPlus Echo', 'https://github.com/Stazed/rakarrack-plus#eco') ,
         (4, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (4, 1) : (2, 'Panning', 'PAN', -64) ,
         (4, 2) : (3, 'Delay', 'DELAY', 0) ,
         (4, 3) : (4, 'Left/Right Delay Offset', 'LR_DELAY', 0) ,
         (4, 4) : (5, 'Left/Right Crossover', 'LRCr', 0) ,
         (4, 5) : (6, 'Feedback', 'FEEDBACK', 0) ,
         (4, 6) : (7, 'Damping', 'DAMP', 0) ,
         (4, 7) : (8, 'Reverse', 'REV', 0) ,
         (4, 8) : (9, 'Direct', 'DIRECT', 0) ,
         (4, 9) : (-1,) ,

         (5, -1) : ('RakarrackPlus Chorus', 'https://github.com/Stazed/rakarrack-plus#chor') ,
         (5, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (5, 1) : (2, 'Panning', 'PAN', -64) ,
         (5, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (5, 3) : (4, 'Randomness', 'RND', 0) ,
         (5, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (5, 5) : (6, 'LFO L/R Delay', 'WIDTH', 0) ,
         (5, 6) : (7, 'Depth', 'DEPTH', 0) ,
         (5, 7) : (8, 'Delay', 'DELAY', 0) ,
         (5, 8) : (9, 'Feedback', 'FB', 0) ,
         (5, 9) : (10, 'Left/Right Crossover', 'LRCR', 0) ,
         (5, 10) : (0, 'SKIP', 'SKIP', 0) ,
         (5, 11) : (11, 'Subtract', 'SUB', 0) ,
         (5, 12) : (12, 'Intense', 'INTENSE', 0) ,
         (5, 13) : (-1,) ,

         (6, -1) : ('RakarrackPlus Phaser', 'https://github.com/Stazed/rakarrack-plus#phas') ,
         (6, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (6, 1) : (2, 'Panning', 'PAN', -64) ,
         (6, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (6, 3) : (4, 'Randomness', 'RND', 0) ,
         (6, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (6, 5) : (6, 'LFO L/R Delay', 'STDL', 0) ,
         (6, 6) : (7, 'Phase Depth', 'DEPTH', 0) ,
         (6, 7) : (8, 'Feedback', 'FB', 0) ,
         (6, 8) : (9, 'Stages', 'STAGES', 0) ,
         (6, 9) : (10, 'Left/Right Crossover', 'LRCR', -64) ,
         (6, 10) : (11, 'Subtract', 'SUB', 0) ,
         (6, 11) : (12, 'Phase', 'PHASE', 0) ,
         (6, 12) : (-1,) ,

         (7, -1) : ('RakarrackPlus Flanger', 'https://github.com/Stazed/rakarrack-plus#Flange') ,
         (7, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (7, 1) : (2, 'Panning', 'PAN', -64) ,
         (7, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (7, 3) : (4, 'Randomness', 'RND', 0) ,
         (7, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (7, 5) : (6, 'LFO L/R Delay', 'WIDTH', 0) ,
         (7, 6) : (7, 'Depth', 'DEPTH', 0) ,
         (7, 7) : (8, 'Delay', 'DELAY', 0) ,
         (7, 8) : (9, 'Feedback', 'FB', 0) ,
         (7, 9) : (10, 'Left/Right Crossover', 'LRCR', 0) ,
         (7, 10) : (0, 'SKIP', 'SKIP', 0) ,
         (7, 11) : (11, 'Subtract', 'SUB', 0) ,
         (7, 12) : (12, 'Intense', 'INTENSE', 0) ,
         (7, 13) : (-1,) ,

         (8, -1) : ('RakarrackPlus Reverb', 'https://github.com/Stazed/rakarrack-plus#reve') ,
         (8, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (8, 1) : (2, 'Panning', 'PAN', -64) ,
         (8, 2) : (3, 'Time', 'TIME', 0) ,
         (8, 3) : (4, 'Initial Delay', 'IDEL', 0) ,
         (8, 4) : (5, 'Initial Delay Feedback', 'IDELFB', 0) ,
         (8, 5) : (0, 'SKIP', 'SKIP', 0) ,
         (8, 6) : (0, 'SKIP', 'SKIP', 0) ,
         (8, 7) : (6, 'Lowpass Filter', 'LPF', 0) ,
         (8, 8) : (7, 'Highpass Filter', 'HPF', 0) ,
         (8, 9) : (8, 'Damping', 'DAMP', 0) ,
         (8, 10) : (9, 'Type', 'TYPE', 0) ,
         (8, 11) : (10, 'Room Size', 'RS', 0) ,
         (8, 12) : (-1,) ,

         (9, -1) : ('RakarrackPlus Parametric EQ', 'https://github.com/Stazed/rakarrack-plus#eqp') ,
         (9, 0) : (2, 'Frequency', 'LFREQ', 0) ,
         (9, 1) : (3, 'Gain', 'LGAIN', -64) ,
         (9, 2) : (4, 'Width', 'LQ', -64) ,
         (9, 3) : (5, 'Frequency', 'MFREQ', 0) ,
         (9, 4) : (6, 'Gain', 'MGAIN', -64) ,
         (9, 5) : (7, 'Width', 'MQ', -64) ,
         (9, 6) : (8, 'Frequency', 'HFREQ', 0) ,
         (9, 7) : (9, 'Gain', 'HGAIN', -64) ,
         (9, 8) : (10, 'Width', 'HQ', -64) ,
         (9, 9) : (1, 'Gain', 'GAIN', -64) ,
         (9, 10) : (-1,) ,

         (10, -1) : ('RakarrackPlus WahWah', 'https://github.com/Stazed/rakarrack-plus#wha') ,
         (10, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (10, 1) : (2, 'Panning', 'PAN', -64) ,
         (10, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (10, 3) : (4, 'Randomness', 'RND', 0) ,
         (10, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (10, 5) : (6, 'LFO L/R Delay', 'WIDTH', 0) ,
         (10, 6) : (7, 'Depth', 'DEPTH', 0) ,
         (10, 7) : (8, 'Sensitivity', 'SENSE', 0) ,
         (10, 8) : (9, 'Invert', 'INV', 0) ,
         (10, 9) : (10, 'Smooth', 'SMOOTH', 0) ,
         (10, 10) : (11, 'Filter Type', 'MODE', 0) ,
         (10, 11) : (-1,) ,

         (11, -1) : ('RakarrackPlus AlienWah', 'https://github.com/Stazed/rakarrack-plus#awha') ,
         (11, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (11, 1) : (2, 'Panning', 'PAN', -64) ,
         (11, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (11, 3) : (4, 'Randomness', 'RND', 0) ,
         (11, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (11, 5) : (6, 'LFO L/R Delay', 'STDL', 0) ,
         (11, 6) : (7, 'Depth', 'DEPTH', 0) ,
         (11, 7) : (8, 'Feedback', 'FB', 0) ,
         (11, 8) : (9, 'Delay', 'DELAY', 0) ,
         (11, 9) : (10, 'Left/Right Crossover', 'LRCR', 0) ,
         (11, 10) : (11, 'Phase', 'PHASE', 0) ,
         (11, 11) : (-1,) ,

         (12, -1) : ('RakarrackPlus Cabinet', 'https://github.com/Stazed/rakarrack-plus#cabe') ,
         (12, 0) : (2, 'Preset', 'PRESET', 0) ,
         (12, 1) : (1, 'Gain', 'GAIN', -64) ,
         (12, 2) : (-1,) ,

         (13, -1) : ('RakarrackPlus Pan', 'https://github.com/Stazed/rakarrack-plus#pan') ,
         (13, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (13, 1) : (2, 'Panning', 'PAN', -64) ,
         (13, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (13, 3) : (4, 'Randomness', 'RND', 0) ,
         (13, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (13, 5) : (6, 'LFO L/R Delay', 'STDL', 0) ,
         (13, 6) : (7, 'Extra Stereo Amount', 'EXTRA_AMOUNT', 0) ,
         (13, 7) : (8, 'Autopan', 'AUTO', 0) ,
         (13, 8) : (9, 'Extra Stereo Enable', 'EXTRA_ON', 0) ,
         (13, 9) : (-1,) ,

         (14, -1) : ('RakarrackPlus Harmonizer', 'https://github.com/Stazed/rakarrack-plus#har') ,
         (14, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (14, 1) : (2, 'Pan', 'PAN', -64) ,
         (14, 2) : (3, 'Gain', 'GAIN', -64) ,
         (14, 3) : (4, 'Interval', 'INT', -12) ,
         (14, 4) : (5, 'Filter Frequency', 'FREQ', 0) ,
         (14, 5) : (6, 'Select Chord Mode', 'SELECT', 0) ,
         (14, 6) : (7, 'Note', 'NOTE', 0) ,
         (14, 7) : (8, 'Chord', 'CHORD', 0) ,
         (14, 8) : (9, 'Filter Gain', 'FGAIN', -64) ,
         (14, 9) : (10, 'Filter Q', 'FQ', -64) ,
         (14, 10) : (11, 'MIDI Mode', 'MIDI', 0) ,
         (14, 11) : (-1,) ,

         (15, -1) : ('RakarrackPlus Musical Delay', 'https://github.com/Stazed/rakarrack-plus#delm') ,
         (15, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (15, 1) : (2, 'Panning', 'PAN1', -64) ,
         (15, 2) : (3, 'Delay', 'DEL1', 0) ,
         (15, 3) : (4, 'Delay Offset', 'OFFS', 0) ,
         (15, 4) : (5, 'Left/Right Crossover', 'LRCr', 0) ,
         (15, 5) : (6, 'Feedback', 'FB1', 0) ,
         (15, 6) : (7, 'Damping', 'DAMP', 0) ,
         (15, 7) : (8, 'Panning', 'PAN2', -64) ,
         (15, 8) : (9, 'Delay', 'DEL2', 0) ,
         (15, 9) : (10, 'Feedback', 'FB2', 0) ,
         (15, 10) : (11, 'Tempo', 'TEMPO', 0) ,
         (15, 11) : (12, 'Gain', 'GAIN1', 0) ,
         (15, 12) : (13, 'Gain', 'GAIN2', 0) ,
         (15, 13) : (-1,) ,

         (16, -1) : ('RakarrackPlus NoiseGate', 'https://github.com/Stazed/rakarrack-plus#gate') ,
         (16, 0) : (1, 'Threshold', 'THRESHOLD', 0) ,
         (16, 1) : (2, 'Range', 'RANGE', 0) ,
         (16, 2) : (3, 'Attack Time', 'ATTACK', 0) ,
         (16, 3) : (4, 'Release Time', 'RELEASE', 0) ,
         (16, 4) : (5, 'Lowpass Filter', 'LPF', 0) ,
         (16, 5) : (6, 'Highpass Filter', 'HPF', 0) ,
         (16, 6) : (7, 'Hold', 'HOLD', 0) ,
         (16, 7) : (-1,) ,

         (17, -1) : ('RakarrackPlus Derelict', 'https://github.com/Stazed/rakarrack-plus#dere') ,
         (17, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (17, 1) : (2, 'Panning', 'PAN', -64) ,
         (17, 2) : (3, 'Left/Right Crossover', 'LRCr', 0) ,
         (17, 3) : (4, 'Drive', 'DRIVE', 0) ,
         (17, 4) : (5, 'Level', 'LEVEL', 0) ,
         (17, 5) : (6, 'Type', 'TYPE', 0) ,
         (17, 6) : (7, 'Negate (Polarity Switch)', 'NEG', 0) ,
         (17, 7) : (8, 'Lowpass Filter', 'LPF', 0) ,
         (17, 8) : (9, 'Highpass Filter', 'HPF', 0) ,
         (17, 9) : (10, 'Color', 'COLOR', 0) ,
         (17, 10) : (11, 'Prefilter', 'PREFILTER', 0) ,
         (17, 11) : (12, 'Suboctave', 'OCT', 0) ,
         (17, 12) : (-1,) ,

         (18, -1) : ('RakarrackPlus Analog Phaser', 'https://github.com/Stazed/rakarrack-plus#aphas') ,
         (18, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (18, 1) : (2, 'Distort', 'DISTORT', 0) ,
         (18, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (18, 3) : (4, 'Randomness', 'RND', 0) ,
         (18, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (18, 5) : (6, 'LFO L/R Delay', 'STDL', 0) ,
         (18, 6) : (7, 'Width', 'WIDTH', 0) ,
         (18, 7) : (8, 'Feedback', 'FB', -64) ,
         (18, 8) : (9, 'Stages', 'STAGES', 0) ,
         (18, 9) : (10, 'Mismatch', 'MISMATCH', 0) ,
         (18, 10) : (11, 'Subtract', 'SUB', 0) ,
         (18, 11) : (12, 'Phase Depth', 'DEPTH', 0) ,
         (18, 12) : (13, 'Hyper', 'HYPER', 0) ,
         (18, 13) : (-1,) ,

         (19, -1) : ('RakarrackPlus Valve', 'https://github.com/Stazed/rakarrack-plus#Valve') ,
         (19, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (19, 1) : (2, 'Panning', 'PAN', -64) ,
         (19, 2) : (3, 'Left/Right Crossover', 'LRCr', 0) ,
         (19, 3) : (4, 'Drive', 'DRIVE', 0) ,
         (19, 4) : (5, 'Level', 'LEVEL', 0) ,
         (19, 5) : (6, 'Negate (Polarity Switch)', 'NEG', 0) ,
         (19, 6) : (7, 'Lowpass Filter', 'LPF', 0) ,
         (19, 7) : (8, 'Highpass Filter', 'HPF', 0) ,
         (19, 8) : (9, 'Stereo', 'STEREO', 0) ,
         (19, 9) : (10, 'Prefilter', 'PREFILTER', 0) ,
         (19, 10) : (11, 'Distortion', 'DIST', 0) ,
         (19, 11) : (12, 'Extra Distortion', 'ED', 0) ,
         (19, 12) : (13, 'Presence', 'PRES', 0) ,
         (19, 13) : (-1,) ,

         (20, -1) : ('RakarrackPlus Dual Flange', 'https://github.com/Stazed/rakarrack-plus#Dual_Flange') ,
         (20, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (20, 1) : (2, 'Pan', 'PAN', 0) ,
         (20, 2) : (3, 'Left/Right Crossover', 'LRC', 0) ,
         (20, 3) : (4, 'Depth', 'DEPTH', 0) ,
         (20, 4) : (5, 'Sweep Width', 'WIDTH', 0) ,
         (20, 5) : (6, 'Offset Delays', 'OFFS', 0) ,
         (20, 6) : (7, 'Feedback', 'FB', 0) ,
         (20, 7) : (8, 'Lowpass Filter', 'LPF', 0) ,
         (20, 8) : (9, 'Subtract', 'SUB', 0) ,
         (20, 9) : (10, 'Sweep Through Zero', 'THZ', 0) ,
         (20, 10) : (11, 'Tempo', 'TEMPO', 0) ,
         (20, 11) : (12, 'LFO L/R Delay', 'STDL', 0) ,
         (20, 12) : (13, 'LFO Type', 'TYPE', 0) ,
         (20, 13) : (14, 'Randomness', 'RND', 0) ,
         (20, 14) : (15, 'Intense', 'INTENSE', 0) ,
         (20, 15) : (-1,) ,

         (21, -1) : ('RakarrackPlus Ring', 'https://github.com/Stazed/rakarrack-plus#Ring') ,
         (21, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (21, 1) : (2, 'Pan', 'PAN', 0) ,
         (21, 2) : (3, 'Left/Right Crossover', 'LRCr', 0) ,
         (21, 3) : (4, 'Level', 'LEVEL', 0) ,
         (21, 4) : (5, 'Depth', 'DEPTH', 0) ,
         (21, 5) : (6, 'Frequency', 'FREQ', 0) ,
         (21, 6) : (7, 'Stereo', 'STEREO', 0) ,
         (21, 7) : (8, 'Sine', 'SIN', 0) ,
         (21, 8) : (9, 'Triangle', 'TRI', 0) ,
         (21, 9) : (10, 'Sawtooth', 'SAW', 0) ,
         (21, 10) : (11, 'Square', 'SQUARE', 0) ,
         (21, 11) : (12, 'Input Gain', 'INPUT', 0) ,
         (21, 12) : (13, 'Auto-Frequency', 'AFREQ', 0) ,
         (21, 13) : (-1,) ,

         (22, -1) : ('RakarrackPlus Exciter', 'https://github.com/Stazed/rakarrack-plus#Exciter') ,
         (22, 0) : (1, 'Gain', 'GAIN', 0) ,
         (22, 1) : (2, '1st Harmonic', 'HAR1', 0) ,
         (22, 2) : (3, '2nd Harmonic', 'HAR2', 0) ,
         (22, 3) : (4, '3rd Harmonic', 'HAR3', 0) ,
         (22, 4) : (5, '4th Harmonic', 'HAR4', 0) ,
         (22, 5) : (6, '5th Harmonic', 'HAR5', 0) ,
         (22, 6) : (7, '6th Harmonic', 'HAR6', 0) ,
         (22, 7) : (8, '7th Harmonic', 'HAR7', 0) ,
         (22, 8) : (9, '8th Harmonic', 'HAR8', 0) ,
         (22, 9) : (10, '9th Harmonic', 'HAR9', 0) ,
         (22, 10) : (11, '10th Harmonic', 'HAR10', 0) ,
         (22, 11) : (12, 'Low-Pass Filter Cutoff', 'LPF', 0) ,
         (22, 12) : (13, 'High-Pass Filter Cutoff', 'HPF', 0) ,
         (22, 13) : (-1,) ,

         (23, -1) : ('RakarrackPlus DistBand', 'https://github.com/Stazed/rakarrack-plus#DistBand') ,
         (23, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (23, 1) : (2, 'Panning', 'PAN', -64) ,
         (23, 2) : (3, 'Left/Right Crossover', 'LRCr', 0) ,
         (23, 3) : (4, 'Drive', 'DRIVE', 0) ,
         (23, 4) : (5, 'Level', 'LEVEL', 0) ,
         (23, 5) : (6, 'Low Band Type', 'TYPEL', 0) ,
         (23, 6) : (7, 'Mid Band Type', 'TYPEM', 0) ,
         (23, 7) : (8, 'High Band Type', 'TYPEH', 0) ,
         (23, 8) : (9, 'Low Gain', 'LVOL', 0) ,
         (23, 9) : (10, 'Mid Gain', 'MVOL', 0) ,
         (23, 10) : (11, 'High Gain', 'HVOL', 0) ,
         (23, 11) : (12, 'Negate (Polarity Switch)', 'NEG', 0) ,
         (23, 12) : (13, 'Low/Mid Crossover', 'LMCR', 0) ,
         (23, 13) : (14, 'Mid/High Crossover', 'MHCR', 0) ,
         (23, 14) : (15, 'Stereo', 'STEREO', 0) ,
         (23, 15) : (-1,) ,

         (24, -1) : ('RakarrackPlus Arpie', 'https://github.com/Stazed/rakarrack-plus#Arpie') ,
         (24, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (24, 1) : (2, 'Panning', 'PAN', -64) ,
         (24, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (24, 3) : (4, 'Left/Right Delay Offset', 'LR_DELAY', 0) ,
         (24, 4) : (5, 'Left/Right Crossover', 'LRCr', 0) ,
         (24, 5) : (6, 'Feedback', 'FEEDBACK', 0) ,
         (24, 6) : (7, 'Damping', 'DAMP', 0) ,
         (24, 7) : (8, "Arpe's Wet/Dry", 'ARP', 0) ,
         (24, 8) : (9, 'Harmonics', 'HARM', 0) ,
         (24, 9) : (10, 'Pattern', 'PATTERN', 0) ,
         (24, 10) : (11, 'Subdivision', 'SUBD', 0) ,
         (24, 11) : (-1,) ,

         (25, -1) : ('RakarrackPlus Expander', 'https://github.com/Stazed/rakarrack-plus#Expander') ,
         (25, 0) : (1, 'Threshold', 'THRESHOLD', 0) ,
         (25, 1) : (2, 'Shape', 'SHAPE', 0) ,
         (25, 2) : (3, 'Attack Time', 'ATTACK', 0) ,
         (25, 3) : (4, 'Release Time', 'RELEASE', 0) ,
         (25, 4) : (5, 'Lowpass Filter', 'LPF', 0) ,
         (25, 5) : (6, 'Highpass Filter', 'HPF', 0) ,
         (25, 6) : (7, 'Output Gain', 'OUTPUT', 0) ,
         (25, 7) : (-1,) ,

         (26, -1) : ('RakarrackPlus Shuffle', 'https://github.com/Stazed/rakarrack-plus#Shuffle') ,
         (26, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (26, 1) : (2, 'Gain', 'LGAIN', 0) ,
         (26, 2) : (3, 'Gain', 'M1GAIN', 0) ,
         (26, 3) : (4, 'Gain', 'M2GAIN', 0) ,
         (26, 4) : (5, 'Gain', 'HGAIN', 0) ,
         (26, 5) : (6, 'Frequency', 'LFREQ', 0) ,
         (26, 6) : (7, 'Frequency', 'M1FREQ', 0) ,
         (26, 7) : (8, 'Frequency', 'M2FREQ', 0) ,
         (26, 8) : (9, 'Frequency', 'HFREQ', 0) ,
         (26, 9) : (10, 'Width', 'Q', 0) ,
         (26, 10) : (11, 'Filtered Band', 'BAND', 0) ,
         (26, 11) : (-1,) ,

         (27, -1) : ('RakarrackPlus Synthfilter', 'https://github.com/Stazed/rakarrack-plus#Synthfilter') ,
         (27, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (27, 1) : (2, 'Distort', 'DIST', 0) ,
         (27, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (27, 3) : (4, 'Randomness', 'RND', 0) ,
         (27, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (27, 5) : (6, 'LFO L/R Delay', 'STDL', 0) ,
         (27, 6) : (7, 'Width', 'WIDTH', 0) ,
         (27, 7) : (8, 'Feedback', 'FB', 0) ,
         (27, 8) : (9, 'Lowpass Stages', 'LSTAGES', 0) ,
         (27, 9) : (10, 'Highpass Stages', 'HSTAGES', 0) ,
         (27, 10) : (11, 'Subtract Output', 'SUB', 0) ,
         (27, 11) : (12, 'Depth', 'DEPTH', 0) ,
         (27, 12) : (13, 'Envelope Sensitivity', 'SENSE', 0) ,
         (27, 13) : (14, 'Attack Time', 'ATIME', 0) ,
         (27, 14) : (15, 'Release Time', 'RTIME', 0) ,
         (27, 15) : (16, 'HPF/LPF Offset', 'OFFSET', 0) ,
         (27, 16) : (-1,) ,

         (28, -1) : ('RakarrackPlus VaryBand', 'https://github.com/Stazed/rakarrack-plus#VaryBand') ,
         (28, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (28, 1) : (2, 'Tempo', 'TEMPO1', 0) ,
         (28, 2) : (3, 'LFO Type', 'TYPE1', 0) ,
         (28, 3) : (4, 'LFO L/R Delay', 'STDL1', 0) ,
         (28, 4) : (5, 'Tempo', 'TEMPO2', 0) ,
         (28, 5) : (6, 'LFO Type', 'TYPE2', 0) ,
         (28, 6) : (7, 'LFO L/R Delay', 'STDL2', 0) ,
         (28, 7) : (8, 'Low/Mid1 Crossover', 'LMCR', 0) ,
         (28, 8) : (9, 'Mid1/Mid2 Crossover', 'MMCR', 0) ,
         (28, 9) : (10, 'Mid2/High Crossover', 'MHCR', 0) ,
         (28, 10) : (0, 'SPECIAL', 'SPECIAL', 0) , #combi
         (28, 11) : (-1,) ,
         (28, 12) : (11, 'Low Band Volume', 'LB', 0) ,#plugin-only params
         (28, 13) : (12, 'Mid Band 1 Volume', 'M1B', 0) ,
         (28, 14) : (13, 'Mid Band 2 Volume', 'M2B', 0) ,
         (28, 15) : (14, 'High Band Volume', 'HB', 0) ,
         (28, 16) : (-1,) ,

         #convo
         (29, -1) : ('RakarrackPlus Convolotron', 'https://github.com/Stazed/rakarrack-plus#Convolotron') ,
         (29, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (29, 1) : (2, 'Pan', 'PAN', -64) ,
         (29, 2) : (3, 'Safe', 'SAFE', 0) ,
         (29, 3) : (4, 'Length', 'LENGTH', 0) ,
         (29, 4) : (0, 'SKIP', 'SKIP', 0) ,#user file
         (29, 5) : (0, 'SKIP', 'SKIP', 0) ,#not used
         (29, 6) : (5, 'Dampening', 'DAMP', 0) ,
         (29, 7) : (6, 'Level', 'LEVEL', 0) ,
         (29, 8) : (0, 'SKIP', 'SKIP', 0) ,#file Number
         (29, 9) : (0, 'SKIP', 'SKIP', 0) ,#not used
         (29, 10) : (7, 'Feedback', 'FB', 0) ,
         (29, 11) : (-1,) ,

         #looper
         (30,-1) : (0,) ,#not used

         (31, -1) : ('RakarrackPlus MuTroMojo', 'https://github.com/Stazed/rakarrack-plus#MuTroMojo') ,
         (31, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (31, 1) : (2, 'Resonance', 'Q', 0) ,
         (31, 2) : (3, 'Tempo', 'TEMPO', 0) ,
         (31, 3) : (4, 'LFO Randomness', 'RND', 0) ,
         (31, 4) : (5, 'LFO Type', 'TYPE', 0) ,
         (31, 5) : (6, 'LFO L/R Delay', 'STDL', -64) ,
         (31, 6) : (7, 'Depth', 'DEPTH', 0) ,
         (31, 7) : (8, 'Envelope Sensitivity', 'SENSE', 0) ,
         (31, 8) : (9, 'Wah', 'WAH', 0) ,
         (31, 9) : (10, 'Envelope Smoothing', 'SMOOTH', 0) ,
         (31, 10) : (11, 'Lowpass Level', 'LP', 0) ,
         (31, 11) : (12, 'Bandpass Level', 'BP', 0) ,
         (31, 12) : (13, 'Highpass Level', 'HP', 0) ,
         (31, 13) : (14, 'Filter Stages', 'STAGES', 0) ,
         (31, 14) : (15, 'Sweep Range', 'RANGE', 0) ,
         (31, 15) : (16, 'Starting Frequency', 'MINFREQ', 0) ,
         (31, 16) : (17, 'Modulate Resonance', 'VARIQ', 0) ,
         (31, 17) : (0, 'SPECIAL', 'SPECIAL', 0) ,#mode (sets q and a mode)
         (31, 18) : (0, 'SKIP', 'SKIP', 0) ,#preset
         (31, 19) : (-1, ) ,
         (31, 20) : (18, 'Analog Gain Mode', 'QMODE', 0) ,#plugin-only params
         (31, 21) : (19, 'Exponential Wah', 'AMODE', 0) ,
         (31, 22) : (-1,) ,

         (32, -1) : ('RakarrackPlus Echoverse', 'https://github.com/Stazed/rakarrack-plus#Echoverse') ,
         (32, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (32, 1) : (2, 'Panning', 'PAN', -64) ,
         (32, 2) : (3, 'Tempo', 'DELAY', 0) ,
         (32, 3) : (4, 'Left/Right Delay Offset', 'LR_DELAY', 0) ,
         (32, 4) : (5, 'Angle', 'ANGLE', -64) ,
         (32, 5) : (6, 'Feedback', 'FEEDBACK', 0) ,
         (32, 6) : (7, 'Damping', 'DAMP', 0) ,
         (32, 7) : (8, 'Reverse', 'REV', 0) ,
         (32, 8) : (9, 'Subdivision', 'SUBD', 0) ,
         (32, 9) : (10, 'Extra Stereo', 'ES', 0) ,
         (32, 10) : (-1,) ,

         (33, -1) : ('RakarrackPlus Coil Crafter', 'https://github.com/Stazed/rakarrack-plus#CoilCrafter') ,
         (33, 0) : (1, 'Gain', 'GAIN', 0) ,
         (33, 1) : (2, 'SKIP', 'SKIP', 0) ,#preset         
         (33, 2) : (3, 'SKIP', 'SKIP', 0) ,#origin preset
         (33, 3) : (4, 'Origin Pickup Frequency', 'F1', 0) ,
         (33, 4) : (5, 'Origin Pickup Resonance', 'Q1', 0) ,
         (33, 5) : (6, 'Destination Pickup Frequency', 'F2', 0) ,
         (33, 6) : (7, 'Destination Pickup Resonance', 'Q2', 0) ,
         (33, 7) : (8, 'Tone', 'TONE', 0) ,
         (33, 8) : (9, 'Neck Pickup', 'MODE', 0) ,
         (33, 9) : (-1,) ,

         (34, -1) : ('RakarrackPlus Shelf Boost', 'https://github.com/Stazed/rakarrack-plus#ShelfBoost') ,
         (34, 0) : (1, 'Gain', 'GAIN', 0) ,
         (34, 1) : (2, 'Presence', 'PRES', 0) ,
         (34, 2) : (3, 'Tone', 'TONE', 0) ,
         (34, 3) : (4, 'Stereo', 'STEREO', 0) ,
         (34, 4) : (5, 'Level', 'LEVEL', 0) ,
         (34, 5) : (-1,) ,

         (35, -1) : ('RakarrackPlus Vocoder', 'https://github.com/Stazed/rakarrack-plus#Vocoder') ,
         (35, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (35, 1) : (2, 'Pan', 'PAN', -64) ,
         (35, 2) : (3, 'Smear', 'MUF', 0) ,
         (35, 3) : (4, 'Q', 'Q', 0) ,
         (35, 4) : (5, 'Input Gain', 'INGAIN', 0) ,
         (35, 5) : (6, 'Level', 'LEVEL', 0) ,
         (35, 6) : (7, 'Ring Mod. Depth', 'RING', 0) ,
         (35, 7) : (8, 'Auxiliary Input', 'AUX_INPUT', 0) ,
         (35, 8) : (9, 'Auxiliary Input Level', 'INPUT', 0) ,
         (35, 9) : (-1,) ,

         (36, -1) : ('RakarrackPlus Sustainer', 'https://github.com/Stazed/rakarrack-plus#Sustainer') ,
         (36, 0) : (1, 'Gain', 'GAIN', 0) ,
         (36, 1) : (2, 'Sustain', 'SUS', 0) ,
         (36, 2) : (-1,) ,

         (37, -1) : ('RakarrackPlus Sequence', 'https://github.com/Stazed/rakarrack-plus#Sequence') ,
         (37, 0) : (1, 'Step 1', 'S1', 0) ,
         (37, 1) : (2, 'Step 2', 'S2', 0) ,
         (37, 2) : (3, 'Step 3', 'S3', 0) ,
         (37, 3) : (4, 'Step 4', 'S4', 0) ,
         (37, 4) : (5, 'Step 5', 'S5', 0) ,
         (37, 5) : (6, 'Step 6', 'S6', 0) ,
         (37, 6) : (7, 'Step 7', 'S7', 0) ,
         (37, 7) : (8, 'Step 8', 'S8', 0) ,
         (37, 8) : (9, 'Dry/Wet', 'DRYWET', 0) ,
         (37, 9) : (10, 'Tempo', 'TEMPO', 0) ,
         (37, 10) : (11, 'Q', 'Q', -64) ,
         (37, 11) : (12, 'Amplitude/Alt. Mode', 'AMP', 0) ,
         (37, 12) : (13, 'Right Channel Step Lag', 'LAG', 0) ,
         (37, 13) : (14, 'Modulation Mode', 'MODE', 0) ,
         (37, 14) : (15, 'Step Adjustment Range', 'RANGE', 0) ,
         (37, 15) : (-1,) ,

         (38, -1) : ('RakarrackPlus Shifter', 'https://github.com/Stazed/rakarrack-plus#Shifter') ,
         (38, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (38, 1) : (2, 'Pan', 'PAN', -64) ,
         (38, 2) : (3, 'Gain', 'GAIN', -64) ,
         (38, 3) : (4, 'Attack', 'ATACK', 0) ,
         (38, 4) : (5, 'Decay', 'DECAY', 0) ,
         (38, 5) : (6, 'Threshold', 'THRESH', 0) ,
         (38, 6) : (7, 'Interval', 'INT', 0) ,
         (38, 7) : (8, 'Shift Down', 'DN', 0) ,
         (38, 8) : (9, 'Mode', 'MODE', 0) ,
         (38, 9) : (10, 'Wammy', 'WHAMMY', 0) ,
         (38, 10) : (-1,) ,

         (39, -1) : ('RakarrackPlus StompBox', 'https://github.com/Stazed/rakarrack-plus#StompBox') ,
         (39, 0) : (1, 'Level', 'LEVEL', 0) ,
         (39, 1) : (2, 'Tone', 'HI', 0) ,
         (39, 2) : (3, 'Mid', 'MID', 0) ,
         (39, 3) : (4, 'Bias', 'LO', 0) ,
         (39, 4) : (5, 'Gain', 'GAIN', 0) ,
         (39, 5) : (6, 'Mode', 'MODE', 0) ,
         (39, 6) : (-1,) ,

#         (39, -1) : ('RakarrackPlus StompBox:Fuzz', 'https://github.com/Stazed/rakarrack-plus#StompBox_fuzz') ,
#         (39, 0) : (1, 'Level', 'LEVEL', 0) ,
#         (39, 1) : (2, 'Tone', 'HI', 0) ,
#         (39, 2) : (3, 'Mid', 'MID', 0) ,
#         (39, 3) : (4, 'Bias', 'LO', 0) ,
#         (39, 4) : (5, 'Gain', 'GAIN', 0) ,
#         (39, 5) : (-1,) ,

         (40, -1) : ('RakarrackPlus Reverbtron', 'https://github.com/Stazed/rakarrack-plus#Reverbtron') ,
         (40, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (40, 1) : (2, 'Fade', 'FADE', 0) ,
         (40, 2) : (3, 'Safe', 'SAFE', 0) ,
         (40, 3) : (4, 'Length', 'LENGTH', 0) ,
         (40, 4) : (0, 'SKIP', 'SKIP', 0) ,#user file
         (40, 5) : (5, 'Initial Delay', 'DELAY', 0) ,
         (40, 6) : (6, 'Dampening', 'DAMP', 0) ,
         (40, 7) : (7, 'Level', 'LEVEL', 0) ,
         (40, 8) : (0, 'SKIP', 'SKIP', 0) ,#file Number
         (40, 9) : (8, 'Time Stretch', 'STRETCH', 0) ,
         (40, 10) : (9, 'Feedback', 'FB', 0) ,
         (40, 11) : (10, 'Pan', 'PAN', -64) ,
         (40, 12) : (11, 'Extra Stereo', 'ES', 0) ,
         (40, 13) : (12, 'Shuffle', 'SH', 0) ,
         (40, 14) : (13, 'Lowpass Filter', 'LPF', 0) ,
         (40, 15) : (14, 'Diffusion', 'DIFFUSE', 0) ,
         (40, 16) : (-1,) ,

         (41, -1) : ('RakarrackPlus Echotron', 'https://github.com/Stazed/rakarrack-plus#Echotron') ,
         (41, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (41, 1) : (2, 'Filter Depth', 'DEPTH', -64) ,
         (41, 2) : (3, 'LFO Width', 'WIDTH', 0) ,
         (41, 3) : (4, 'Number of Taps', 'LENGTH', 0) ,
         (41, 4) : (0, 'SKIP', 'SKIP', 0) ,#user file
         (41, 5) : (5, 'Tempo', 'TEMPO', 0) ,
         (41, 6) : (6, 'Dampening', 'DAMP', 0) ,
         (41, 7) : (7, 'Left/Right Crossover', 'LRC', -64) ,
         (41, 8) : (0, 'SKIP', 'SKIP', 0) ,#file number
         (41, 9) : (8, 'LFO L/R Delay', 'STDF', 0) ,
         (41, 10) : (9, 'Feedback', 'FB', 0) ,
         (41, 11) : (10, 'Pan', 'PAN', -64) ,
         (41, 12) : (11, 'Modulate Delays', 'MD', 0) ,
         (41, 13) : (12, 'Modulate Filters', 'MF', 0) ,
         (41, 14) : (13, 'LFO Type', 'LFOTYPE', 0) ,
         (41, 15) : (14, 'Activate Filters', 'AF', 0) ,
         (41, 16) : (-1,) ,

         (42, -1) : ('RakarrackPlus StereoHarmonizer', 'https://github.com/Stazed/rakarrack-plus#StereoHarm') ,
         (42, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (42, 1) : (2, 'Left Gain', 'GAINL', -64) ,
         (42, 2) : (3, 'Left Interval', 'INTL', -12) ,
         (42, 3) : (4, 'Left Chroma', 'CHRML', 0) ,
         (42, 4) : (5, 'Right Gain', 'GAINR', -64) ,
         (42, 5) : (6, 'Right Interval', 'INTR', -12) ,
         (42, 6) : (7, 'Right Chroma', 'CHR', 0) ,
         (42, 7) : (8, 'Select Chord Mode', 'SELECT', 0) ,
         (42, 8) : (9, 'Note', 'NOTE', 0) ,
         (42, 9) : (10, 'Chord', 'CHORD', 0) ,
         (42, 10) : (11, 'MIDI Mode', 'MIDI', 0) ,
         (42, 11) : (11, 'Left/Right Crossover', 'LRC', 0) ,
         (42, 12) : (-1,) ,

         (43, -1) : ('RakarrackPlus CompBand', 'https://github.com/Stazed/rakarrack-plus#CompBand') ,
         (43, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (43, 1) : (2, 'Low Band Ratio', 'LR', 0) ,
         (43, 2) : (3, 'Mid Band 1 Ratio', 'M1R', 0) ,
         (43, 3) : (4, 'Mid Band 2 Ratio', 'M2R', 0) ,
         (43, 4) : (5, 'High Band Ratio', 'HR', 0) ,
         (43, 5) : (6, 'Low Band Threshold', 'LT', 0) ,
         (43, 6) : (7, 'Mid Band 1 Threshold', 'M1T', 0) ,
         (43, 7) : (8, 'Mid Band 2 Threshold', 'M2T', 0) ,
         (43, 8) : (9, 'High Threshold', 'HT', 0) ,
         (43, 9) : (10, 'Low/Mid1 Crossover', 'LMCR', 0) ,
         (43, 10) : (11, 'Mid1/Mid2 Crossover', 'MMCR', 0) ,
         (43, 11) : (12, 'Mid2/High Crossover', 'MHCR', 0) ,
         (43, 12) : (13, 'Gain', 'GAIN', 0) ,
         (43, 13) : (-1,) ,

         (44, -1) : ('RakarrackPlus OpticalTrem', 'https://github.com/Stazed/rakarrack-plus#Otrem') ,
         (44, 0) : (1, 'Depth', 'DEPTH', 0) ,
         (44, 1) : (2, 'Tempo', 'TEMPO', 0) ,
         (44, 2) : (3, 'Randomness', 'RND', 0) ,
         (44, 3) : (4, 'LFO Type', 'TYPE', 0) ,
         (44, 4) : (5, 'LFO L/R Delay', 'STDF', 0) ,
         (44, 5) : (6, 'Panning', 'PAN', -64) ,
         (44, 6) : (7, 'Invert', 'INV', 0) ,
         (44, 7) : (-1,) ,

         (45, -1) : ('RakarrackPlus Vibe', 'https://github.com/Stazed/rakarrack-plus#Vibe') ,
         (45, 0) : (1, 'Width', 'WIDTH', 0) ,
         (45, 1) : (2, 'Tempo', 'TEMPO', 0) ,
         (45, 2) : (3, 'Randomness', 'RND', 0) ,
         (45, 3) : (4, 'LFO Type', 'TYPE', 0) ,
         (45, 4) : (5, 'LFO L/R Delay', 'STDL', 0) ,
         (45, 5) : (6, 'Pan', 'PAN', -64) ,
         (45, 6) : (7, 'Dry/Wet', 'DRYWET', 0) ,
         (45, 7) : (8, 'Feedback', 'FB', -64) ,
         (45, 8) : (9, 'Depth', 'DEPTH', 0) ,
         (45, 9) : (10, 'Left/Right Crossover', 'LRCR', 0) ,
         (45, 10) : (11, 'Stereo', 'STEREO', 0) ,
         (45, 11) : (-1,) ,

         (46, -1) : ('RakarrackPlus Infinity', 'https://github.com/Stazed/rakarrack-plus#Infinity') ,
         (46, 0) : (1, 'Dry/Wet', 'DRYWET', 0) ,
         (46, 1) : (2, 'Band 1 Gain', 'B1', 0) ,
         (46, 2) : (3, 'Band 2 Gain', 'B2', 0) ,
         (46, 3) : (4, 'Band 3 Gain', 'B3', 0) ,
         (46, 4) : (5, 'Band 4 Gain', 'B4', 0) ,
         (46, 5) : (6, 'Band 5 Gain', 'B5', 0) ,
         (46, 6) : (7, 'Band 6 Gain', 'B6', 0) ,
         (46, 7) : (8, 'Band 7 Gain', 'B7', 0) ,
         (46, 8) : (9, 'Band 8 Gain', 'B8', 0) ,
         (46, 9) : (10, 'Resonance', 'RES', 0) ,
         (46, 10) : (11, 'Sweep Start', 'START', 0) ,
         (46, 11) : (12, 'Sweep End', 'END', 0) ,
         (46, 12) : (13, 'Tempo', 'TEMPO', 0) ,
         (46, 13) : (14, 'Sweep L/R Delay', 'STDF', 0) ,
         (46, 14) : (15, 'Sweep Tempo Subdivision', 'SUBD', 0) ,
         (46, 15) : (16, 'AutoPan Amount', 'AUTOPAN', 0) ,
         (46, 16) : (17, 'Reverse Left Channel Sweep', 'REV', 0) ,
         (46, 17) : (18, 'Stages', 'STAGE', 0) ,
         (46, 18) : (-1,) ,

    }[(efxindex,paramindex)]
