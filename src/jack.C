/*
  rakarrack - a guitar efects software

  jack.C  -   jack I/O
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
(version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */

#include <jack/jack.h>
#include <jack/midiport.h>
#include <jack/transport.h>
#include "jack.h"
#include "global.h"

jack_port_t *outport_left, *outport_right;
jack_port_t *inputport_left, *inputport_right, *inputport_aux;
jack_port_t *jack_midi_in, *jack_midi_out;


int
JACKstart(RKR * rkr_)
{
    RKR *JackOUT = rkr_;
    jack_client_t *jackclient = JackOUT->jackclient;

    // Initialize output ringbuffers for MIDIConverter
    JackOUT->efx_MIDIConverter->m_buffSize = jack_ringbuffer_create(JACK_RINGBUFFER_SIZE);
    JackOUT->efx_MIDIConverter->m_buffMessage = jack_ringbuffer_create(JACK_RINGBUFFER_SIZE);

    jack_set_sync_callback(jackclient, timebase, JackOUT);
    jack_set_process_callback(jackclient, jackprocess, JackOUT);

    jack_on_shutdown(jackclient, jackshutdown, JackOUT);


    inputport_left =
            jack_port_register(jackclient, "in_1", JACK_DEFAULT_AUDIO_TYPE,
                               JackPortIsInput, 0);
    inputport_right =
            jack_port_register(jackclient, "in_2", JACK_DEFAULT_AUDIO_TYPE,
                               JackPortIsInput, 0);

    inputport_aux =
            jack_port_register(jackclient, "aux", JACK_DEFAULT_AUDIO_TYPE,
                               JackPortIsInput, 0);

    outport_left =
            jack_port_register(jackclient, "out_1", JACK_DEFAULT_AUDIO_TYPE,
                               JackPortIsOutput, 0);
    outport_right =
            jack_port_register(jackclient, "out_2", JACK_DEFAULT_AUDIO_TYPE,
                               JackPortIsOutput, 0);

    jack_midi_in =
            jack_port_register(jackclient, "in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

    jack_midi_out =
            jack_port_register(jackclient, "MC out", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);


    if (jack_activate(jackclient))
    {
        fprintf(stderr, "Cannot activate jack client.\n");
        return (2);
    }

    if (JackOUT->Config.aconnect_JA)
    {
        for (int i = 0; i < JackOUT->Config.cuan_jack; i += 2)
        {
            jack_connect(jackclient, jack_port_name(outport_left),
                         JackOUT->Config.jack_po[i].name);
            jack_connect(jackclient, jack_port_name(outport_right),
                         JackOUT->Config.jack_po[i + 1].name);
        }
    }

    if (JackOUT->Config.aconnect_JIA)
    {
        if (JackOUT->Config.cuan_ijack == 1)
        {
            jack_connect(jackclient, JackOUT->Config.jack_poi[0].name, jack_port_name(inputport_left));
            jack_connect(jackclient, JackOUT->Config.jack_poi[0].name, jack_port_name(inputport_right));
        }
        else
        {
            for (int i = 0; i < JackOUT->Config.cuan_ijack; i += 2)
            {
                jack_connect(jackclient, JackOUT->Config.jack_poi[i].name, jack_port_name(inputport_left));
                jack_connect(jackclient, JackOUT->Config.jack_poi[i + 1].name, jack_port_name(inputport_right));
            }
        }
    }

    return 3;
}

int
jackprocess(jack_nframes_t nframes, void *arg)
{
    RKR *JackOUT = static_cast<RKR *>(arg);
    
    jack_midi_event_t midievent;
    jack_position_t pos;
    jack_transport_state_t astate;

    jack_default_audio_sample_t *outl = (jack_default_audio_sample_t *)
            jack_port_get_buffer(outport_left, nframes);
    jack_default_audio_sample_t *outr = (jack_default_audio_sample_t *)
            jack_port_get_buffer(outport_right, nframes);


    jack_default_audio_sample_t *inl = (jack_default_audio_sample_t *)
            jack_port_get_buffer(inputport_left, nframes);
    jack_default_audio_sample_t *inr = (jack_default_audio_sample_t *)
            jack_port_get_buffer(inputport_right, nframes);

    jack_default_audio_sample_t *aux = (jack_default_audio_sample_t *)
            jack_port_get_buffer(inputport_aux, nframes);


    JackOUT->cpuload = jack_cpu_load(JackOUT->jackclient);


    if ((JackOUT->Tap_Active) && (JackOUT->Tap_Selection == 2))
    {
        astate = jack_transport_query(JackOUT->jackclient, &pos);
        
        if (astate > 0)
        {
            if (JackOUT->jt_tempo != pos.beats_per_minute)
                actualiza_tap(pos.beats_per_minute, JackOUT);
        }

        if (JackOUT->EFX_Active[EFX_LOOPER])
        {
            if ((astate != JackOUT->jt_state) && (astate == 0))
            {
                JackOUT->jt_state = astate;
                JackOUT->Rack_Effects[EFX_LOOPER]->changepar(Looper_Stop, 1);
                JackOUT->Gui_Refresh = GUI_Refresh_Looper;
            }

            if ((astate != JackOUT->jt_state) && (astate == 3))
            {
                JackOUT->jt_state = astate;
                JackOUT->Rack_Effects[EFX_LOOPER]->changepar(Looper_Play, 1);
                JackOUT->Gui_Refresh = GUI_Refresh_Looper;
            }
        }
    }


    int jnumpi = jack_port_connected(inputport_left) + jack_port_connected(inputport_right);
    
    if (jnumpi != JackOUT->Jack_IN_Port_Connnection_Status)
    {
        JackOUT->Jack_IN_Port_Connnection_Status = jnumpi;
        JackOUT->Jack_Port_Connnection_Changed = 1;
    }
    
    int jnumpo = jack_port_connected(outport_left) + jack_port_connected(outport_right);
    
    if (jnumpo != JackOUT->Jack_OUT_Port_Connnection_Status)
    {
        JackOUT->Jack_OUT_Port_Connnection_Status = jnumpo;
        JackOUT->Jack_Port_Connnection_Changed = 1;
    }
    
    int jnumpa = jack_port_connected(inputport_aux);
    
    if (jnumpa != JackOUT->Jack_AUX_Port_Connnection_Status)
    {
        JackOUT->Jack_AUX_Port_Connnection_Status = jnumpa;
        JackOUT->Jack_Port_Connnection_Changed = 1;
    }

    int jnumpmi = jack_port_connected(jack_midi_in);
    
    if (jnumpmi != JackOUT->Jack_MIDI_IN_Port_Connnection_Status)
    {
        JackOUT->Jack_MIDI_IN_Port_Connnection_Status = jnumpmi;
        JackOUT->Jack_Port_Connnection_Changed = 1;
    }

    int jnumpmo = jack_port_connected(jack_midi_out);
    
    if (jnumpmo != JackOUT->Jack_MIDI_OUT_Port_Connnection_Status)
    {
        JackOUT->Jack_MIDI_OUT_Port_Connnection_Status = jnumpmo;
        JackOUT->Jack_Port_Connnection_Changed = 1;
    }


    float *data = (float *) jack_port_get_buffer(jack_midi_in, nframes);
    int count = jack_midi_get_event_count(data);

    /* For midi incoming */
    for (int i = 0; i < count; i++)
    {
        jack_midi_event_get(&midievent, data, i);
        JackOUT->jack_process_midievents(&midievent);
    }

    /* For midi outgoing - MIDIConverter */
    int space = 0;

    void *buffer = jack_port_get_buffer(jack_midi_out, nframes);
    jack_midi_clear_buffer(buffer);

    if(JackOUT->efx_MIDIConverter->m_buffSize != NULL && JackOUT->efx_MIDIConverter->m_buffMessage != NULL)
    {
        while (jack_ringbuffer_read_space(JackOUT->efx_MIDIConverter->m_buffSize) > 0)
        {
            jack_ringbuffer_read(JackOUT->efx_MIDIConverter->m_buffSize, (char *) &space, (size_t) sizeof (space));
            jack_midi_data_t *midiData = jack_midi_event_reserve(buffer, 0, space);

            jack_ringbuffer_read(JackOUT->efx_MIDIConverter->m_buffMessage, (char *) midiData, (size_t) space);
        }
    }

    if(nframes)
    {
        memcpy(JackOUT->efxoutl, inl,
               sizeof (jack_default_audio_sample_t) * nframes);
        memcpy(JackOUT->efxoutr, inr,
               sizeof (jack_default_audio_sample_t) * nframes);
        memcpy(JackOUT->auxdata, aux,
               sizeof (jack_default_audio_sample_t) * nframes);


        JackOUT->process_effects(inl, inr, 0);

        memcpy(outl, JackOUT->efxoutl,
               sizeof (jack_default_audio_sample_t) * nframes);
        memcpy(outr, JackOUT->efxoutr,
               sizeof (jack_default_audio_sample_t) * nframes);
    }
    
    return 0;

}

void
JACKfinish(RKR * JackOUT)
{
    if(JackOUT->jackclient)
    {
        jack_client_close(JackOUT->jackclient);
    }
    
    if(JackOUT->efx_MIDIConverter->m_buffSize != NULL && JackOUT->efx_MIDIConverter->m_buffMessage != NULL)
    {
        jack_ringbuffer_free(JackOUT->efx_MIDIConverter->m_buffSize);
        jack_ringbuffer_free(JackOUT->efx_MIDIConverter->m_buffMessage);
    }
    
    usleep(1000);
}

void
jackshutdown(void *arg)
{
    RKR *JackOUT = static_cast<RKR *>(arg);
    if (!JackOUT->Gui_Shown)
    {
        printf("Jack Shut Down, sorry.\n");
    }
    else
    {
        JackOUT->Jack_Shut_Down = 1;
    }
}

int
timebase(jack_transport_state_t state, jack_position_t *pos, void *arg)
{
    RKR *JackOUT = static_cast<RKR *>(arg);
    
    JackOUT->jt_state = state;

    if ((JackOUT->Tap_Active) && (JackOUT->Tap_Selection == 2))
    {
        if ((state > 0) && (pos->beats_per_minute > 0))
        {
            JackOUT->jt_tempo = pos->beats_per_minute;
            JackOUT->Tap_TempoSet = lrint(JackOUT->jt_tempo);
            JackOUT->Update_tempo();
            JackOUT->Tap_Display = 1;
            
            if ((JackOUT->EFX_Active[EFX_LOOPER]) && (state == 3))
            {
                JackOUT->Rack_Effects[EFX_LOOPER]->changepar(Looper_Play, 1);
                JackOUT->Gui_Refresh = GUI_Refresh_Looper;
            }
        }
    }

    return (1);
}

void
actualiza_tap(double val, RKR * JackOUT)
{
    JackOUT->jt_tempo = val;
    JackOUT->Tap_TempoSet = lrint(JackOUT->jt_tempo);
    JackOUT->Update_tempo();
    JackOUT->Tap_Display = 1;
}

