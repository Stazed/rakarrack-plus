/*
  rakarrack - a guitar effects software

 rkrMIDI.C  -  MIDI functions
  Copyright (C) 2008 Daniel Vidal & Josep Andreu
  Author: Daniel Vidal & Josep Andreu

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


#include "global.h"


void
RKR::InitMIDI ()
{

  // Open Alsa Seq

  int alsaport_in;

  int err = snd_seq_open (&midi_in, "default", SND_SEQ_OPEN_INPUT, 0);
  if (err < 0)
    printf ("Cannot activate ALSA seq client\n");
  snd_seq_set_client_name (midi_in, "rakarrack");
  snd_config_update_free_global ();



 char portname[50];

  // Create Alsa Seq Client

  sprintf (portname, "rakarrack IN");
  alsaport_in = snd_seq_create_simple_port (midi_in, portname,
					    SND_SEQ_PORT_CAP_WRITE |
					    SND_SEQ_PORT_CAP_SUBS_WRITE,
					    SND_SEQ_PORT_TYPE_SYNTH);


}



void
RKR::miramidi ()
{

  if (snd_seq_event_input_pending (midi_in, 1))
    {
      do
	{
	  midievents ();

	}
      while (snd_seq_event_input_pending (midi_in, 0));
    }

};








void
RKR::midievents()
{

  int i;
  snd_seq_event_t *midievent;
  midievent = NULL;
  snd_seq_event_input (midi_in, &midievent);
  if (midievent == NULL)
    return;
  if (midievent->type == 42)
    return;



  if ((midievent->type == SND_SEQ_EVENT_NOTEON)
      || (midievent->type == SND_SEQ_EVENT_NOTEOFF))
    {
      int cmdnote = midievent->data.note.note;
      int cmdvelo = midievent->data.note.velocity;

      if (midievent->data.note.channel == HarCh)
	{
	  for (i = 0; i < POLY; i++)
	    {
	      if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo != 0))
		{
		  if (note_active[i] == 0)
		    {
		      note_active[i] = 1;
		      rnote[i] = cmdnote;
		      gate[i] = 1;
		      RC->MiraChord ();
		      break;
		    }

		}


	      if ((midievent->type == SND_SEQ_EVENT_NOTEON) && (cmdvelo == 0))
		{

		  if ((note_active[i]) && (rnote[i] == cmdnote))
		    {
		      note_active[i] = 0;
		      gate[i] = 0;
		      break;
		    }

		}


	      if (midievent->type == SND_SEQ_EVENT_NOTEOFF)
		{

		  if ((note_active[i]) && (rnote[i] == cmdnote))
		    {
		      note_active[i] = 0;
		      gate[i] = 0;
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
	  if ((midievent->data.control.value > 0)
	      && (midievent->data.control.value < 61))
	    preset = midievent->data.control.value;

          if (midievent->data.control.value == 81)
          if (Selected_Preset >1) preset = Selected_Preset-1;
        
          if (midievent->data.control.value == 82)
          if (Selected_Preset<60) preset = Selected_Preset+1;
           	   


	}
    }



  if (midievent->type == SND_SEQ_EVENT_CONTROLLER)
    {
      if (midievent->data.control.channel == MidiCh)
	{

           if(RControl)
	    {
	      ControlGet = (int)midievent->data.control.param;
	      return; 
	    } 
	
          if(MIDIway)
           {
             for(i=0; i<20;i++)
               {
                 if (XUserMIDI[(int)midievent->data.control.param][i])
 	         process_midi_controller_events(XUserMIDI[(int)midievent->data.control.param][i],
					 (int)midievent->data.control.value);
                 else break;
               } 
           }
           else 
	  process_midi_controller_events((int)midievent->data.control.param,
					 (int)midievent->data.control.value);


	}

    }



};


void
RKR::ActiveUn(int value)
{

  int numef;
  int inoff=0;
  int miraque=0;


if(value < 20)
{
  numef = value / 2;
  inoff = value % 2; 
  miraque = efx_order[numef];
  Mnumeff = numef;
}

if(value > 149)
{
  numef = value -150;
  inoff = checkonoff(numef); 
  miraque = numef;
  Mnumeff = numef;
}

  switch(miraque)
   {
   case 0:
     if (inoff) EQ1_Bypass=1; else EQ1_Bypass=0;
     break;
   case 1:
     if (inoff) Compressor_Bypass=1; else Compressor_Bypass=0;  
     break;
   case 2:
     if (inoff) Distorsion_Bypass=1; else Distorsion_Bypass=0;
     break;
   case 3:
     if (inoff) Overdrive_Bypass=1; else Overdrive_Bypass=0;  
     break;
   case 4:
     if (inoff) Echo_Bypass=1; else Echo_Bypass=0;
     break;
   case 5:
     if (inoff) Chorus_Bypass=1; else Chorus_Bypass=0;  
     break;
   case 6:
     if (inoff) Phaser_Bypass=1; else Phaser_Bypass=0;
     break;
   case 7:
     if (inoff) Flanger_Bypass=1; else Flanger_Bypass=0;  
     break;
   case 8:
     if (inoff) Reverb_Bypass=1; else Reverb_Bypass=0;
     break;
   case 9:
     if (inoff) EQ2_Bypass=1; else EQ2_Bypass=0;  
     break;
   case 10:
     if (inoff) WhaWha_Bypass=1; else WhaWha_Bypass=0;
     break;
   case 11:
     if (inoff) Alienwah_Bypass=1; else Alienwah_Bypass=0;  
     break;
   case 12:
     if (inoff) Cabinet_Bypass=1; else Cabinet_Bypass=0;
     break;
   case 13:
     if (inoff) Pan_Bypass=1; else Pan_Bypass=0;  
     break;
   case 14:
     if (inoff) Harmonizer_Bypass=1; else Harmonizer_Bypass=0;
     break;
   case 15:
     if (inoff) MusDelay_Bypass=1; else MusDelay_Bypass=0;  
     break;
   case 16:
     if (inoff) Gate_Bypass=1; else Gate_Bypass=0;
     break;
   case 17:
     if (inoff) NewDist_Bypass=1; else NewDist_Bypass=0;
     break;
   case 18: 
     if (inoff) APhaser_Bypass=1; else APhaser_Bypass=0;
     break;
   case 19: 
     if (inoff) Valve_Bypass=1; else Valve_Bypass=0;
     break;
   case 20: 
     if (inoff) DFlange_Bypass=1; else DFlange_Bypass=0;
     break;
   case 21: 
     if (inoff) Ring_Bypass=1; else Ring_Bypass=0;
     break;
   case 22: 
     if (inoff) Exciter_Bypass=1; else Exciter_Bypass=0;
     break;
   case 23: 
     if (inoff) MBDist_Bypass=1; else MBDist_Bypass=0;
     break;
   case 24: 
     if (inoff) Arpie_Bypass=1; else Arpie_Bypass=0;
     break;
   case 25: 
     if (inoff) Expander_Bypass=1; else Expander_Bypass=0;
     break;
   case 26: 
     if (inoff) Synthfilter_Bypass=1; else Synthfilter_Bypass=0;
     break;
   case 27: 
     if (inoff) MBVvol_Bypass=1; else MBVvol_Bypass=0;
     break;
}   
}



int
RKR::checkonoff(int miraque)
{


  switch(miraque)
   {
   case 0:
     if(EQ1_Bypass) return(0);
     break;
   case 1:
     if (Compressor_Bypass) return(0);  
     break;
   case 2:
     if (Distorsion_Bypass) return(0);
     break;
   case 3:
     if (Overdrive_Bypass)return(0);
     break;
   case 4:
     if (Echo_Bypass)return(0);
     break;
   case 5:
     if (Chorus_Bypass) return(0);
     break;
   case 6:
     if (Phaser_Bypass) return(0);
     break;
   case 7:
     if (Flanger_Bypass) return(0);
     break;
   case 8:
     if (Reverb_Bypass) return(0);
     break;
   case 9:
     if (EQ2_Bypass) return(0);
     break;
   case 10:
     if (WhaWha_Bypass) return(0);
     break;
   case 11:
     if (Alienwah_Bypass) return(0);
     break;
   case 12:
     if (Cabinet_Bypass) return(0);
     break;
   case 13:
     if (Pan_Bypass) return(0);
     break;
   case 14:
     if (Harmonizer_Bypass) return(0);
     break;
   case 15:
     if (MusDelay_Bypass) return(0);
     break;
   case 16:
     if (Gate_Bypass) return(0);
     break;
   case 17:
     if (NewDist_Bypass) return(0);
     break;
   case 18: 
     if (APhaser_Bypass) return(0);
     break;
   case 19: 
     if (Valve_Bypass) return(0);
     break;
   case 20: 
     if (DFlange_Bypass) return(0);
     break;
   case 21: 
     if (Ring_Bypass) return(0);
     break;
   case 22: 
     if (Exciter_Bypass) return(0);
     break;
   case 23: 
     if (MBDist_Bypass) return(0);
     break;
   case 24: 
     if (Arpie_Bypass) return(0);
     break;
   case 25: 
     if (Expander_Bypass) return(0);
     break;
   case 26: 
     if (Synthfilter_Bypass) return(0);
     break;
   case 27: 
     if (MBVvol_Bypass) return(0);
     break;
}   
return(1);

}



void
RKR::Conecta ()
{

  FILE *fp;

  int client = 0;
  int puerto = 0;
  char temp[128];
  char temp1[128];
  char temp2[128];
  char *nume;

  if (IsCoIn)
    disconectaaconnect ();


  if ((fp = fopen ("/proc/asound/seq/clients", "r")) != NULL)
    {
      bzero (temp, sizeof (temp));

      while (fgets (temp, sizeof temp, fp) != NULL)
	{

	  if (strstr (temp, "Client") != NULL)

	    {

	      strcpy (temp1, temp);
	      strtok (temp1, " ");
	      nume = strtok (NULL, "\"");
	      sscanf (nume, "%d", &client);

	    }

	  if (strstr (temp, "Port") != NULL)
	    {
	      strcpy (temp2, temp);
	      strtok (temp2, " ");
	      nume = strtok (NULL, "  ");
	      sscanf (nume, "%d", &puerto);
	      if (strstr (temp, "rakarrack IN") != 0)
		{
		  Cyoin = client;
		  Pyoin = puerto;
		}
	      if (strstr (temp, MID) != 0)
		{
		  Ccin = client;
		  Pcin = puerto;
		}

	    }
	}
    }

  fclose (fp);
  conectaaconnect ();
};




void
RKR::conectaaconnect ()
{
  char tempi[128];

  if (MID != NULL)
    {
      bzero (tempi, sizeof (tempi));
      sprintf (tempi, "aconnect %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
      system (tempi);
      IsCoIn = 1;
    }

};


void
RKR::disconectaaconnect ()
{
  char tempi[128];

  if (MID != NULL)
    {
      bzero (tempi, sizeof (tempi));
      sprintf (tempi, "aconnect -d %d:%d  %d:%d", Ccin, Pcin, Cyoin, Pyoin);
      system (tempi);
      IsCoIn = 0;
    }


};



void
RKR::jack_process_midievents (jack_midi_event_t *midievent)
{

  int i;
  int type = midievent->buffer[0]>>4;

  if ((type == 8) || (type == 9))
    {
      int cmdnote = midievent->buffer[1];
      int cmdvelo = midievent->buffer[2];
      int cmdchan = midievent->buffer[0]&15;
      
      if (cmdchan == HarCh)
	{
	  for (i = 0; i < POLY; i++)
	    {
	      if ((type==9) && (cmdvelo != 0))
		{
		  if (note_active[i] == 0)
		    {
		      note_active[i] = 1;
		      rnote[i] = cmdnote;
		      gate[i] = 1;
		      RC->MiraChord ();
		      break;
		    }

		}


	      if ((type==9) && (cmdvelo == 0))
		{

		  if ((note_active[i]) && (rnote[i] == cmdnote))
		    {
		      note_active[i] = 0;
		      gate[i] = 0;
		      break;
		    }

		}

	      if (type==8)
		{

		  if ((note_active[i]) && (rnote[i] == cmdnote))
		    {
		      note_active[i] = 0;
		      gate[i] = 0;
		      break;
		    }

		}
	    }



	}


    }





  if (type == 12)
    {
      int cmdvalue= midievent->buffer[1];
      int cmdchan = midievent->buffer[0]&15;
      
      if (cmdchan == MidiCh)
	{
	  if ((cmdvalue > 0)
	      && (cmdvalue < 61))
	    preset = cmdvalue;

          if (cmdvalue==81) if (Selected_Preset>1) preset = Selected_Preset-1;
          if (cmdvalue==82) if (Selected_Preset<60) preset = Selected_Preset+1;
          


	}
    }



  if (type == 11)
    {
      int cmdcontrol = midievent->buffer[1];
      int cmdvalue= midievent->buffer[2];
      int cmdchan = midievent->buffer[0]&15;
    
    
      if (cmdchan == MidiCh)
	{
	  if(RControl)
	    {
	      ControlGet = cmdcontrol;
	      return; 
	    } 
	
	
          if(MIDIway)
           {
             for(i=0; i<20;i++)
               {
                 if (XUserMIDI[cmdcontrol][i])
 	         process_midi_controller_events(XUserMIDI[cmdcontrol][i],cmdvalue);
                 else break;
               } 
           }
           else 
           process_midi_controller_events(cmdcontrol, cmdvalue);
	}
    }

};


/*
 * process MIDI controller events
 */
void
RKR::process_midi_controller_events(int parameter, int value)
{
   
  int i;
  // for real parameter changes, flag need for a GUI update
  if(parameter > 0)
   {
     Mcontrol[parameter]=1;
     Mvalue = 1;
   }  
 

  switch (parameter)
    {

    case 7:
	  Master_Volume =
	    (float) value / 128.0f;
	  calculavol (2);
      break;
      
    case 1:
	  efx_WhaWha->changepar (6, value);
      break;
      
    case 20:
	  efx_Alienwah->changepar (6,value);
      break;
      
    case 21:
	  efx_Phaser->changepar (6, value);
      break;
      
    case 22:
	  efx_Flanger->changepar (6, value);
      break;
      
    case 23:
	  efx_Chorus->changepar (6, value);
      break;
      
    case 24:
	  efx_MusDelay->changepar (11, value);
      break;
      
    case 25:
	  efx_MusDelay->changepar (12, value);
      break;
      
    case 26:
	  efx_Har->changepar (4, value);
      break;
      
    case 27:
      if (Harmonizer_Bypass)
	{
	  Harmonizer_Bypass = 0;
	  efx_Har->changepar (3, value);
	  Harmonizer_Bypass = 1;
	}
      break;
      
    case 28:
	  efx_WhaWha->changepar (0, value);
      break;
      
    case 29:
	  efx_Overdrive->changepar (0, value);
      break;
      
    case 30:
	  efx_Distorsion->changepar (0, value);
      break;
      
    case 31:
	  efx_Har->changepar (0, value);
      break;
      
    case 52:
	  efx_Chorus->changepar (0, value);
      break;
      
    case 53:
	  efx_Flanger->changepar (0, value);
      break;
      
    case 54:
	  efx_Phaser->changepar (0, value);
      break;
      
    case 55:
	  efx_Alienwah->changepar (0, value);
      break;
      
    case 56:
	  efx_MusDelay->changepar (0, value);
      break;
      
    case 57:
	  efx_Rev->changepar (0, value);
      break;
      
    case 58:
	  efx_Pan->changepar (0, value);
      break;
      
    case 59:
	  efx_Echo->changepar (0, value);
      break;
      
    case 46:
	  efx_Echo->changepar (1, value);
      break;
      
    case 47:
	  efx_Overdrive->changepar (1, value);
      break;
      
    case 48:
	  efx_Distorsion->changepar (1, value);
      break;
      
    case 49:
	  efx_Har->changepar (1, value);
      break;
      
    case 50:
	  efx_Chorus->changepar (1, value);
      break;
      
    case 51:
	  efx_Flanger->changepar (1,value);
      break;
      
    case 60:
	  efx_Phaser->changepar (1, value);
      break;
      
    case 61:
	  efx_Alienwah->changepar (1, value);
      break;
      
    case 62:
	  efx_MusDelay->changepar (1, value);
      break;
      
    case 63:
	  efx_Rev->changepar (1, value);
      break;
      
    case 65:
	  efx_MusDelay->changepar (7, value);
      break;
      
    case 66:
	  efx_WhaWha->changepar (1, value);
      break;
      
    case 67:
	  efx_Pan->changepar (1, value);
      break;
      
    case 68:
	  efx_Overdrive->changepar (3, value);
      break;
      
    case 69:
	  efx_Distorsion->changepar (3, value);
      break;

    case 70:
	  efx_Overdrive->changepar (4, value);
      break;
      
    case 71:
	  efx_Distorsion->changepar (4, value);
      break;
      
    case 72:
	  efx_Chorus->changepar (2, ret_Tempo(value));
      break;
      
    case 73:
	  efx_Flanger->changepar (2,ret_Tempo(value));
      break;
      
    case 74:
	  efx_Phaser->changepar (2, ret_Tempo(value));
      break;
      
    case 75:
	  efx_WhaWha->changepar (2,ret_Tempo(value));
      break;
      
    case 76:
	  efx_Alienwah->changepar (2, ret_Tempo(value));
      break;
      
    case 77:
	  efx_Pan->changepar (2, ret_Tempo(value));
      break;
      
    case 78:
	  efx_Echo->changepar (5, value);
      break;
      
    case 79:
	  efx_Chorus->changepar (8, value);
      break;
      
    case 80:
	  efx_Flanger->changepar (8, value);
      break;
      
    case 81:
	  efx_Phaser->changepar (7, value);
      break;
      
    case 82:
	  efx_Alienwah->changepar (7, value);
      break;
      
    case 83:
	  efx_MusDelay->changepar (5, value);
      break;
      
    case 84:
	  efx_MusDelay->changepar (9, value);
      break;
      
    case 85:
	  efx_Overdrive->changepar (7, ret_LPF(value));
      break;
      
    case 86:
	  efx_Distorsion->changepar (7, ret_LPF(value));
      break;
      
    case 87:
	  efx_Rev->changepar (7, ret_LPF(value));
      break;
      
    case 88:
	  efx_Overdrive->changepar (8, ret_HPF(value));
      break;
      
    case 89:
	  efx_Distorsion->changepar (8, ret_HPF(value));
      break;
      
    case 90:
	  efx_Rev->changepar (8, ret_HPF(value));
      break;
      
    case 91:
	  efx_Chorus->changepar (9, value);
      break;
      
    case 92:
	  efx_Flanger->changepar (9, value);
      break;
      
    case 93:
	  efx_Phaser->changepar (9, value);
      break;
      
    case 94:
	  efx_Overdrive->changepar (2, value);
      break;
      
    case 95:
	  efx_Distorsion->changepar (2, value);
      break;
      
    case 96:
	  efx_Alienwah->changepar (9,value);
      break;
      
    case 97:
	  efx_Echo->changepar (4, value);
      break;
      
    case 98:
	  efx_MusDelay->changepar (4,value);
      break;
      
    case 99:
	  efx_Chorus->changepar (5, value);
      break;
      
    case 100:
	  efx_Flanger->changepar (5, value);
      break;
      
    case 101:
	  efx_Phaser->changepar (5, value);
      break;
      
    case 102:
	  efx_WhaWha->changepar (5, value);
      break;
      
    case 103:
	  efx_Alienwah->changepar (5,value);
      break;
      
    case 104:
	  efx_Pan->changepar (5, value);
      break;
      
    case 105:
	  efx_Chorus->changepar (3, value);
      break;
      
    case 106:
	  efx_Flanger->changepar (3,value);
      break;
      
    case 107:
	  efx_Phaser->changepar (3, value);
      break;
      
    case 108:
	  efx_WhaWha->changepar (3, value);
      break;
      
    case 109:
	  efx_Alienwah->changepar (3, value);
      break;
      
    case 110:
	  efx_Pan->changepar (3, value);
      break;
      
    case 111:
	  efx_WhaWha->changepar (7, value);
      break;

    case 112:
	  efx_WhaWha->changepar (8, value);
      break;
      
    case 113:
	  efx_WhaWha->changepar (9, value);
      break;
      
    case 114:
	  efx_Phaser->changepar (11, value);
      break;
      
    case 115:
	  efx_Alienwah->changepar (10, value);
      break;

    case 116:
      ActiveUn(value);
      break;
      
    case 117:
	  efx_APhaser->changepar (0, value);
      break;
      
    case 118:
	  efx_APhaser->changepar (1, value);
      break;
      
    case 119:
	  efx_APhaser->changepar (2, value);
      break;
      
    case 120:
	  efx_APhaser->changepar (11, value);
      break;
      
    case 121:
	  efx_APhaser->changepar (6, value);
      break;
      
    case 122:
	  efx_APhaser->changepar (7, value);
      break;
      
    case 123:
	  efx_APhaser->changepar (9, value);
      break;

    case 124:
	  efx_APhaser->changepar (5, value);
      break;
      
    case 125:
	  efx_NewDist->changepar (0, value);
      break;
      
    case 126:
	  efx_NewDist->changepar (1, value);
      break;
            
    case 127:
	  efx_NewDist->changepar (2, value);
      break;
      
    case 2:
	  efx_NewDist->changepar (3, value);
      break;
      
    case 3:
	  efx_NewDist->changepar (4, value);
      break;
      
    case 4:
	  efx_NewDist->changepar (7, ret_LPF(value));
      break;
      
    case 5:
	  efx_NewDist->changepar (8, ret_HPF(value));
      break;
      
    case 6:
	  efx_NewDist->changepar (9, value);
      break;
      
    case 8:
	  efx_NewDist->changepar (11, value);
      break;
      
    case 9:
	  efx_NewDist->changepar (12, value);
      break;

    case 12:
	  Fraction_Bypass = (float) value / 127.0f;
      break; 

    case 14:
	  Input_Gain =
	    (float) value / 128.0f;
	  calculavol (1);
      break; 

    case 130:
          efx_EQ1->changepar(0, value); 
       break;   
    
    case 131:       
          for(i=0;i<10;i++) efx_EQ1->changepar(i*5+13, value);
       break;
       
    case 132:   
          efx_EQ1->changepar(12, value); 
       break;   
     
    case 133:   
          efx_EQ1->changepar(5+12, value); 
       break;   
       
    case 134:   
          efx_EQ1->changepar(10+12, value); 
       break;   
       
    case 135:   
          efx_EQ1->changepar(15+12, value); 
       break;   

    case 136:   
          efx_EQ1->changepar(20+12, value); 
       break;   

    case 137:   
          efx_EQ1->changepar(25+12, value); 
       break;   

    case 138:   
          efx_EQ1->changepar(30+12, value); 
       break;   

    case 139:   
          efx_EQ1->changepar(35+12, value); 
       break;   

    case 140:   
          efx_EQ1->changepar(40+12, value); 
       break;   

    case 141:   
          efx_EQ1->changepar(45+12, value); 
       break;   
    
    case 142:
          efx_Compressor->Compressor_Change(4, 10+(int) ((float) value * 1.890f));
       break;
    
    case 143:
          efx_Compressor->Compressor_Change(5, 10+(int) ((float) value * 3.858f));
       break;
    
    case 144:
         efx_Compressor->Compressor_Change(2, 2+ (int) ((float) value * .314f));
       break;
    
    case 145:
         efx_Compressor->Compressor_Change(7,  (int) ((float) value * .787f));
       break;
    
    case 146:
         efx_Compressor->Compressor_Change(1,  -60 + (int) ((float) value * .448f));
       break;
    
    case 147:
         efx_Compressor->Compressor_Change(3,  -40 + (int) ((float) value * .314f));
       break;
       
    case 148:
         efx_EQ2->changepar(0, value);   
       break; 

    case 149:
         efx_EQ2->changepar(11, 20+(int)((float)value*7.7165f ));   
       break; 

    case 150:
         efx_EQ2->changepar(12, value);   
       break; 

    case 151:
         efx_EQ2->changepar(13, value);   
       break; 

    case 152:
         efx_EQ2->changepar(16, 800 + (int)((float)value*56.692f));   
       break; 

    case 153:
         efx_EQ2->changepar(17, value);   
       break; 

    case 154:
         efx_EQ2->changepar(18, value);   
       break; 

    case 155:
         efx_EQ2->changepar(21, 20000 + (int)((float)value*157.48f));   
       break; 

    case 156:
         efx_EQ2->changepar(22, value);   
       break; 

    case 157:
         efx_EQ2->changepar(23, value);   
       break; 
    
    case 158:
         efx_DFlange->changepar(0,value-64);       
         break;
 
    case 159:
         efx_DFlange->changepar(1,value-64);       
         break;
 
    case 160:
         efx_DFlange->changepar(2,value);       
         break;
 
    case 161:
         efx_DFlange->changepar(3,20+(int)((float)value*3.7795f));       
         break;
 
    case 162:
         efx_DFlange->changepar(4,(int)((float)value*23.622f));       
         break;
 
    case 163:
         efx_DFlange->changepar(5,(int)((float)value*0.7874f));       
         break;
 
    case 164:
         efx_DFlange->changepar(6,value-64);       
         break;
 
    case 165:
         efx_DFlange->changepar(7,20+(int)((float)value*157.32f));       
         break;
 
    case 166:
         efx_DFlange->changepar(10,1+(int)((float)value*4.716f));       
         break;
 
    case 167:
         efx_DFlange->changepar(11,value);       
         break;
 
    case 168:
         efx_DFlange->changepar(13,value);       
         break;
            
    case 169:
         efx_Valve->changepar(0,value);
         break;
 
    case 170:
         efx_Valve->changepar(2,value);
         break;

    case 171:
         efx_Valve->changepar(1,value);
         break;

    case 172:
         efx_Valve->changepar(4,value);
         break;

    case 173:
         efx_Valve->changepar(3,value);
         break;

    case 174:
         efx_Valve->changepar(10,value);
         break;

    case 175:
         efx_Valve->changepar(12,(int)((float)value*.7874f));
         break;

    case 176:
         efx_Valve->changepar(6,ret_LPF(value));
         break;

    case 177:
         efx_Valve->changepar(7,ret_HPF(value));
         break;
   
    case 178:
         efx_Ring->changepar(0,value-64);
         break;
    
    case 179:
         efx_Ring->changepar(2,value-64);
         break;
                   
    case 180:
         efx_Ring->changepar(11,value);
         break;
    
    case 181:
         efx_Ring->changepar(3,value);
         break;
                   
    case 182:
         efx_Ring->changepar(1,value-64);
         break;
    
    case 183:
         efx_Ring->changepar(4,(int)((float)value*.7874f));
         break;
                   
    case 184:
         efx_Ring->changepar(5,1 + (int)((float)value*157.472f));
         break;
    
    case 185:
         efx_Ring->changepar(7,(int)((float)value*.7874f));
         break;
                   
    case 186:
         efx_Ring->changepar(8,(int)((float)value*.7874f));
         break;
    
    case 187:
         efx_Ring->changepar(9,(int)((float)value*.7874f));
         break;
                   
    case 188:
         efx_Ring->changepar(10,(int)((float)value*.7874f));
         break;

    case 189:
         efx_Exciter->changepar(0,value);
         break;

    case 190:
         efx_Exciter->changepar(11,20+(int)((float)value *157.3228f));
         break;              

    case 191:
         efx_Exciter->changepar(12,20+(int)((float)value *157.3228f));
         break;              
 
    case 192:
         efx_Exciter->changepar(1,value-64);
         break;

    case 193:
         efx_Exciter->changepar(2,value-64);
         break;

    case 194:
         efx_Exciter->changepar(3,value-64);
         break;

    case 195:
         efx_Exciter->changepar(4,value-64);
         break;

    case 196:
         efx_Exciter->changepar(5,value-64);
         break;

    case 197:
         efx_Exciter->changepar(6,value-64);
         break;

    case 198:
         efx_Exciter->changepar(7,value-64);
         break;

    case 199:
         efx_Exciter->changepar(8,value-64);
         break;

    case 200:
         efx_Exciter->changepar(9,value-64);
         break;

    case 201:
         efx_Exciter->changepar(10,value-64);
         break;
    
    case 202:
         efx_MBDist->changepar(0,value);
         break;
             
    case 203:
         efx_MBDist->changepar(2,value);
         break;
             
    case 204:
         efx_MBDist->changepar(3,value);
         break;
             
    case 205:
         efx_MBDist->changepar(4,value);
         break;
             
    case 206:
         efx_MBDist->changepar(8,(int)((float)value*.7874f));
         break;
             
    case 207:
         efx_MBDist->changepar(9,(int)((float)value*.7874f));
         break;
             
    case 208:
         efx_MBDist->changepar(10,(int)((float)value*.7874f));
         break;
             
    case 209:
         efx_MBDist->changepar(12,40+(int)((float)value*7.559f));
         break;
             
    case 210:
         efx_MBDist->changepar(13,1000+(int)((float)value*55.1181f));
         break;
             
    case 211:
         efx_MBDist->changepar(1,value);
         break;
             
    
   } 
      
    
}


int
RKR::ret_Tempo(int value)
{
return( 1+(int)((float)value*4.724f));
}
int
RKR::ret_LPF(int value)
{
return( 20+(int)((float)value*204.566f));
}
int
RKR::ret_HPF(int value)
{
return( 20+(int)((float)value*157.322f));
}

