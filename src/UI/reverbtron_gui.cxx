// generated by Fast Light User Interface Designer (fluid) version 1.0309

#include "reverbtron_gui.h"

void RevtronGui::cb_revtron_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Active[EFX_REVERBTRON]);
 return;
}
m_process->EFX_Active[EFX_REVERBTRON]=(int)o->value();
if((int) o->value()==0) m_process->Rack_Effects[EFX_REVERBTRON]->cleanup();
m_parent->findpos(EFX_REVERBTRON,(int)o->value(),o);
}
void RevtronGui::cb_revtron_activar(RKR_Light_Button* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_activar_i(o,v);
}

void RevtronGui::cb_revtron_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_REVERBTRON))
    m_process->Rack_Effects[EFX_REVERBTRON]->setpreset((int) o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_REVERBTRON]; i++)
{
    parameter_refresh(i);
};
}
void RevtronGui::cb_revtron_preset(RKR_Choice* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_preset_i(o,v);
}

Fl_Menu_Item RevtronGui::menu_revtron_preset[] = {
 {"Chamber", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Concrete Stairwell", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Hall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Medium Hall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Room", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Large Hall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Guitar", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Studio", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Cathedral", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {0,0,0,0,0,0,0,0,0}
};

void RevtronGui::cb_revtron_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_DryWet);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_DryWet,Dry_Wet((int)(o->value())));
}
void RevtronGui::cb_revtron_WD(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_WD_i(o,v);
}

void RevtronGui::cb_revtron_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Pan);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Pan,(int)(o->value()+64));
}
void RevtronGui::cb_revtron_pan(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_pan_i(o,v);
}

void RevtronGui::cb_revtron_level_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Level);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Level,(int)o->value());
}
void RevtronGui::cb_revtron_level(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_level_i(o,v);
}

void RevtronGui::cb_revtron_damp_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Damp);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Damp,(int)o->value());
}
void RevtronGui::cb_revtron_damp(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_damp_i(o,v);
}

void RevtronGui::cb_revtron_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Feedback);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Feedback,(int)o->value());
}
void RevtronGui::cb_revtron_fb(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_fb_i(o,v);
}

void RevtronGui::cb_revtron_length_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Length);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Length,(int)o->value());
}
void RevtronGui::cb_revtron_length(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_length_i(o,v);
}

void RevtronGui::cb_revtron_strech_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Stretch);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Stretch,(int)o->value());
}
void RevtronGui::cb_revtron_strech(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_strech_i(o,v);
}

void RevtronGui::cb_revtron_idelay_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_I_Delay);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_I_Delay,(int)o->value());
}
void RevtronGui::cb_revtron_idelay(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_idelay_i(o,v);
}

void RevtronGui::cb_revtron_fade_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Fade);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Fade,(int)o->value());
}
void RevtronGui::cb_revtron_fade(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_fade_i(o,v);
}

void RevtronGui::cb_revtron_diff_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_Diffusion);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Diffusion,(int)o->value());
}
void RevtronGui::cb_revtron_diff(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_diff_i(o,v);
}

void RevtronGui::cb_revtron_LPF_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Revtron_LPF);
 return;
}
m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_LPF,(int)o->value());
}
void RevtronGui::cb_revtron_LPF(RKR_Slider* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_LPF_i(o,v);
}

void RevtronGui::cb_revtron_rv_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Shuffle,(int)o->value());
}
void RevtronGui::cb_revtron_rv(RKR_Check_Button* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_rv_i(o,v);
}

void RevtronGui::cb_revtron_es_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Ex_Stereo,(int)o->value());
}
void RevtronGui::cb_revtron_es(RKR_Check_Button* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_es_i(o,v);
}

void RevtronGui::cb_revtron_safe_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Safe,(int)o->value());
}
void RevtronGui::cb_revtron_safe(RKR_Check_Button* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_safe_i(o,v);
}

void RevtronGui::cb_B_scan_i(RKR_Button*, void*) {
  scan_for_new_rvb_files();
}
void RevtronGui::cb_B_scan(RKR_Button* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_B_scan_i(o,v);
}

void RevtronGui::cb_revtron_fnum_i(RKR_Choice* o, void*) {
  int user_file_selected = 0;
    Fl_Menu_Item *m = const_cast<Fl_Menu_Item*>  (o->menu ());
    std::string file = m[o->value()].label();
    
    // See if this is a User provided file by checking the vector of user files
    for(unsigned i = 0; i < m_process->Reverbtron_RVB_Files.size (); i++)
    {
        // String compare by menu name
        if(strcmp(file.c_str(), m_process->Reverbtron_RVB_Files[i].User_File_Menu_Name.c_str ()) == 0)
        {
            user_file_selected = 1;  // found
            file = m_process->Reverbtron_RVB_Files[i].User_File_Name;  // full path
            break;
        }
    }

    if(user_file_selected)
    {
        // Copy the file name to the EFX
        Reverbtron *Efx_Reverbtron = static_cast<Reverbtron*>(m_process->Rack_Effects[EFX_REVERBTRON]);
        strlcpy(Efx_Reverbtron->Filename, file.c_str(), sizeof(Efx_Reverbtron->Filename));
        
        // Tell the EFX that this is a user supplied file
        m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_User_File, 1);

        // Try to load the user file
        if(!Efx_Reverbtron->setfile(USERFILE))
        {
            m_process->Handle_Message(14, file);
            
            // The user file did not work, un-set the user file
            m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_User_File, 0);
        }
        else
        {
            // User file was loaded so set the gui user check box
            revtron_user->color (global_leds_color);
            revtron_user->redraw ();
        }
    }
    else
    {
        // They selected a program provided file, so un-set the User file
        m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_User_File, 0);
        
        // Tell the EFX which program file to use
        m_process->Rack_Effects[EFX_REVERBTRON]->changepar(Revtron_Set_File,(int)o->value());

        // Not a user file so un-check the gui user check box
        revtron_user->color (global_fore_color);
        revtron_user->redraw ();
    };
}
void RevtronGui::cb_revtron_fnum(RKR_Choice* o, void* v) {
  ((RevtronGui*)(o->parent()))->cb_revtron_fnum_i(o,v);
}

Fl_Menu_Item RevtronGui::menu_revtron_fnum[] = {
 {"Chamber", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Conc. Stair", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Hall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Med Hall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Large Room", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Large Hall", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Guitar Ambience", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Studio", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Twilight", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Santa Lucia", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};
RevtronGui::RevtronGui(int X, int Y, int W, int H, const char *L)
  : RKR_Gui_Effect(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
this->when(FL_WHEN_RELEASE);
{ RKR_Light_Button* o = revtron_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  revtron_activar->box(FL_UP_BOX);
  revtron_activar->shortcut(0x35);
  revtron_activar->color((Fl_Color)62);
  revtron_activar->selection_color((Fl_Color)1);
  revtron_activar->labeltype(FL_NORMAL_LABEL);
  revtron_activar->labelfont(0);
  revtron_activar->labelsize(10);
  revtron_activar->labelcolor(FL_FOREGROUND_COLOR);
  revtron_activar->callback((Fl_Callback*)cb_revtron_activar);
  revtron_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  revtron_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* revtron_activar
{ RKR_Choice* o = revtron_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  revtron_preset->box(FL_FLAT_BOX);
  revtron_preset->down_box(FL_BORDER_BOX);
  revtron_preset->color(FL_BACKGROUND_COLOR);
  revtron_preset->selection_color(FL_FOREGROUND_COLOR);
  revtron_preset->labeltype(FL_NORMAL_LABEL);
  revtron_preset->labelfont(0);
  revtron_preset->labelsize(10);
  revtron_preset->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_preset->textsize(10);
  revtron_preset->textcolor(FL_BACKGROUND2_COLOR);
  revtron_preset->callback((Fl_Callback*)cb_revtron_preset, (void*)(UD_PRESET_REVERBTRON));
  revtron_preset->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_preset->when(FL_WHEN_RELEASE_ALWAYS);
  revtron_preset->menu(menu_revtron_preset);
  preset_choice = o;
} // RKR_Choice* revtron_preset
{ revtron_WD = new RKR_Slider(56, 25, 100, 10, "Dry/Wet");
  revtron_WD->type(5);
  revtron_WD->box(FL_FLAT_BOX);
  revtron_WD->color((Fl_Color)178);
  revtron_WD->selection_color((Fl_Color)62);
  revtron_WD->labeltype(FL_NORMAL_LABEL);
  revtron_WD->labelfont(0);
  revtron_WD->labelsize(10);
  revtron_WD->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_WD->maximum(127);
  revtron_WD->step(1);
  revtron_WD->textcolor(FL_BACKGROUND2_COLOR);
  revtron_WD->callback((Fl_Callback*)cb_revtron_WD);
  revtron_WD->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* revtron_WD
{ revtron_pan = new RKR_Slider(56, 37, 100, 10, "Pan");
  revtron_pan->type(5);
  revtron_pan->box(FL_FLAT_BOX);
  revtron_pan->color((Fl_Color)178);
  revtron_pan->selection_color((Fl_Color)62);
  revtron_pan->labeltype(FL_NORMAL_LABEL);
  revtron_pan->labelfont(0);
  revtron_pan->labelsize(10);
  revtron_pan->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_pan->minimum(-64);
  revtron_pan->maximum(63);
  revtron_pan->step(1);
  revtron_pan->textcolor(FL_BACKGROUND2_COLOR);
  revtron_pan->callback((Fl_Callback*)cb_revtron_pan);
  revtron_pan->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* revtron_pan
{ revtron_level = new RKR_Slider(56, 49, 100, 10, "Level");
  revtron_level->type(5);
  revtron_level->box(FL_FLAT_BOX);
  revtron_level->color((Fl_Color)178);
  revtron_level->selection_color((Fl_Color)62);
  revtron_level->labeltype(FL_NORMAL_LABEL);
  revtron_level->labelfont(0);
  revtron_level->labelsize(10);
  revtron_level->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_level->maximum(127);
  revtron_level->step(1);
  revtron_level->textcolor(FL_BACKGROUND2_COLOR);
  revtron_level->callback((Fl_Callback*)cb_revtron_level);
  revtron_level->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_level->when(FL_WHEN_CHANGED);
} // RKR_Slider* revtron_level
{ revtron_damp = new RKR_Slider(56, 61, 100, 10, "Damp");
  revtron_damp->type(5);
  revtron_damp->box(FL_FLAT_BOX);
  revtron_damp->color((Fl_Color)178);
  revtron_damp->selection_color((Fl_Color)62);
  revtron_damp->labeltype(FL_NORMAL_LABEL);
  revtron_damp->labelfont(0);
  revtron_damp->labelsize(10);
  revtron_damp->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_damp->maximum(127);
  revtron_damp->step(1);
  revtron_damp->textcolor(FL_BACKGROUND2_COLOR);
  revtron_damp->callback((Fl_Callback*)cb_revtron_damp);
  revtron_damp->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_damp->when(FL_WHEN_CHANGED);
} // RKR_Slider* revtron_damp
{ revtron_fb = new RKR_Slider(56, 73, 100, 10, "Feedback");
  revtron_fb->type(5);
  revtron_fb->box(FL_FLAT_BOX);
  revtron_fb->color((Fl_Color)178);
  revtron_fb->selection_color((Fl_Color)62);
  revtron_fb->labeltype(FL_NORMAL_LABEL);
  revtron_fb->labelfont(0);
  revtron_fb->labelsize(10);
  revtron_fb->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_fb->minimum(-64);
  revtron_fb->maximum(64);
  revtron_fb->step(1);
  revtron_fb->textcolor(FL_BACKGROUND2_COLOR);
  revtron_fb->callback((Fl_Callback*)cb_revtron_fb);
  revtron_fb->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* revtron_fb
{ revtron_length = new RKR_Slider(56, 85, 100, 10, "Length");
  revtron_length->type(5);
  revtron_length->box(FL_FLAT_BOX);
  revtron_length->color((Fl_Color)178);
  revtron_length->selection_color((Fl_Color)62);
  revtron_length->labeltype(FL_NORMAL_LABEL);
  revtron_length->labelfont(0);
  revtron_length->labelsize(10);
  revtron_length->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_length->minimum(20);
  revtron_length->maximum(1500);
  revtron_length->step(1);
  revtron_length->value(100);
  revtron_length->textcolor(FL_BACKGROUND2_COLOR);
  revtron_length->callback((Fl_Callback*)cb_revtron_length);
  revtron_length->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_length->when(FL_WHEN_RELEASE);
} // RKR_Slider* revtron_length
{ revtron_strech = new RKR_Slider(56, 97, 100, 10, "Stretch");
  revtron_strech->tooltip("Time Stretch");
  revtron_strech->type(5);
  revtron_strech->box(FL_FLAT_BOX);
  revtron_strech->color((Fl_Color)178);
  revtron_strech->selection_color((Fl_Color)62);
  revtron_strech->labeltype(FL_NORMAL_LABEL);
  revtron_strech->labelfont(0);
  revtron_strech->labelsize(10);
  revtron_strech->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_strech->minimum(-64);
  revtron_strech->maximum(64);
  revtron_strech->step(1);
  revtron_strech->textcolor(FL_BACKGROUND2_COLOR);
  revtron_strech->callback((Fl_Callback*)cb_revtron_strech);
  revtron_strech->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_strech->when(FL_WHEN_RELEASE);
} // RKR_Slider* revtron_strech
{ revtron_idelay = new RKR_Slider(56, 109, 100, 10, "I. Del");
  revtron_idelay->tooltip("Initial Delay");
  revtron_idelay->type(5);
  revtron_idelay->box(FL_FLAT_BOX);
  revtron_idelay->color((Fl_Color)178);
  revtron_idelay->selection_color((Fl_Color)62);
  revtron_idelay->labeltype(FL_NORMAL_LABEL);
  revtron_idelay->labelfont(0);
  revtron_idelay->labelsize(10);
  revtron_idelay->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_idelay->maximum(500);
  revtron_idelay->step(1);
  revtron_idelay->textcolor(FL_BACKGROUND2_COLOR);
  revtron_idelay->callback((Fl_Callback*)cb_revtron_idelay);
  revtron_idelay->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_idelay->when(FL_WHEN_RELEASE);
} // RKR_Slider* revtron_idelay
{ revtron_fade = new RKR_Slider(56, 121, 100, 10, "Fade");
  revtron_fade->type(5);
  revtron_fade->box(FL_FLAT_BOX);
  revtron_fade->color((Fl_Color)178);
  revtron_fade->selection_color((Fl_Color)62);
  revtron_fade->labeltype(FL_NORMAL_LABEL);
  revtron_fade->labelfont(0);
  revtron_fade->labelsize(10);
  revtron_fade->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_fade->maximum(127);
  revtron_fade->step(1);
  revtron_fade->textcolor(FL_BACKGROUND2_COLOR);
  revtron_fade->callback((Fl_Callback*)cb_revtron_fade);
  revtron_fade->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_fade->when(FL_WHEN_RELEASE);
} // RKR_Slider* revtron_fade
{ revtron_diff = new RKR_Slider(56, 133, 100, 10, "Diffusion");
  revtron_diff->type(5);
  revtron_diff->box(FL_FLAT_BOX);
  revtron_diff->color((Fl_Color)178);
  revtron_diff->selection_color((Fl_Color)62);
  revtron_diff->labeltype(FL_NORMAL_LABEL);
  revtron_diff->labelfont(0);
  revtron_diff->labelsize(10);
  revtron_diff->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_diff->maximum(127);
  revtron_diff->step(1);
  revtron_diff->textcolor(FL_BACKGROUND2_COLOR);
  revtron_diff->callback((Fl_Callback*)cb_revtron_diff);
  revtron_diff->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_diff->when(FL_WHEN_RELEASE);
} // RKR_Slider* revtron_diff
{ revtron_LPF = new RKR_Slider(56, 145, 100, 10, "LPF");
  revtron_LPF->tooltip("Lowpass Filter");
  revtron_LPF->type(5);
  revtron_LPF->box(FL_FLAT_BOX);
  revtron_LPF->color((Fl_Color)178);
  revtron_LPF->selection_color((Fl_Color)62);
  revtron_LPF->labeltype(FL_NORMAL_LABEL);
  revtron_LPF->labelfont(0);
  revtron_LPF->labelsize(10);
  revtron_LPF->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_LPF->minimum(20);
  revtron_LPF->maximum(26000);
  revtron_LPF->step(1);
  revtron_LPF->value(20);
  revtron_LPF->textcolor(FL_BACKGROUND2_COLOR);
  revtron_LPF->callback((Fl_Callback*)cb_revtron_LPF);
  revtron_LPF->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_LPF->when(FL_WHEN_RELEASE);
} // RKR_Slider* revtron_LPF
{ revtron_rv = new RKR_Check_Button(2, 151, 30, 15, "Sh");
  revtron_rv->tooltip("Shuffle");
  revtron_rv->box(FL_NO_BOX);
  revtron_rv->down_box(FL_BORDER_BOX);
  revtron_rv->color(FL_BACKGROUND_COLOR);
  revtron_rv->selection_color(FL_FOREGROUND_COLOR);
  revtron_rv->labeltype(FL_NORMAL_LABEL);
  revtron_rv->labelfont(0);
  revtron_rv->labelsize(10);
  revtron_rv->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_rv->callback((Fl_Callback*)cb_revtron_rv);
  revtron_rv->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  revtron_rv->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* revtron_rv
{ revtron_es = new RKR_Check_Button(2, 164, 29, 15, "ES");
  revtron_es->tooltip("Extra stereo");
  revtron_es->box(FL_NO_BOX);
  revtron_es->down_box(FL_BORDER_BOX);
  revtron_es->color(FL_BACKGROUND_COLOR);
  revtron_es->selection_color(FL_FOREGROUND_COLOR);
  revtron_es->labeltype(FL_NORMAL_LABEL);
  revtron_es->labelfont(0);
  revtron_es->labelsize(10);
  revtron_es->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_es->callback((Fl_Callback*)cb_revtron_es);
  revtron_es->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  revtron_es->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* revtron_es
{ revtron_safe = new RKR_Check_Button(30, 154, 41, 15, "Safe");
  revtron_safe->tooltip("Safe mode");
  revtron_safe->box(FL_NO_BOX);
  revtron_safe->down_box(FL_BORDER_BOX);
  revtron_safe->color(FL_BACKGROUND_COLOR);
  revtron_safe->selection_color(FL_FOREGROUND_COLOR);
  revtron_safe->labeltype(FL_NORMAL_LABEL);
  revtron_safe->labelfont(0);
  revtron_safe->labelsize(10);
  revtron_safe->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_safe->callback((Fl_Callback*)cb_revtron_safe);
  revtron_safe->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  revtron_safe->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* revtron_safe
{ RKR_Box* o = revtron_user = new RKR_Box(67, 154, 12, 12, "User");
  revtron_user->box(FL_DOWN_BOX);
  revtron_user->color(FL_BACKGROUND_COLOR);
  revtron_user->selection_color(FL_BACKGROUND_COLOR);
  revtron_user->labeltype(FL_NORMAL_LABEL);
  revtron_user->labelfont(0);
  revtron_user->labelsize(10);
  revtron_user->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_user->align(Fl_Align(FL_ALIGN_RIGHT));
  revtron_user->when(FL_WHEN_RELEASE);
  o->set_box_type(BOX_LIGHT);
  o->set_label_offset(1);
} // RKR_Box* revtron_user
{ B_scan = new RKR_Button(106, 156, 46, 10, "Scan");
  B_scan->tooltip("Scan the User Directory for .rvb files added after program start.");
  B_scan->box(FL_UP_BOX);
  B_scan->color(FL_BACKGROUND_COLOR);
  B_scan->selection_color(FL_BACKGROUND_COLOR);
  B_scan->labeltype(FL_NORMAL_LABEL);
  B_scan->labelfont(0);
  B_scan->labelsize(10);
  B_scan->labelcolor(FL_FOREGROUND_COLOR);
  B_scan->callback((Fl_Callback*)cb_B_scan);
  B_scan->align(Fl_Align(FL_ALIGN_CENTER));
  B_scan->when(FL_WHEN_RELEASE);
} // RKR_Button* B_scan
{ revtron_fnum = new RKR_Choice(51, 168, 101, 14, "File");
  revtron_fnum->tooltip("Select the .rvb file to be used. File preceded by a \'*\' are user supplied f\
iles in the User Directory.");
  revtron_fnum->box(FL_FLAT_BOX);
  revtron_fnum->down_box(FL_BORDER_BOX);
  revtron_fnum->color(FL_BACKGROUND_COLOR);
  revtron_fnum->selection_color(FL_FOREGROUND_COLOR);
  revtron_fnum->labeltype(FL_NORMAL_LABEL);
  revtron_fnum->labelfont(0);
  revtron_fnum->labelsize(10);
  revtron_fnum->labelcolor(FL_BACKGROUND2_COLOR);
  revtron_fnum->textsize(10);
  revtron_fnum->textcolor(FL_BACKGROUND2_COLOR);
  revtron_fnum->callback((Fl_Callback*)cb_revtron_fnum);
  revtron_fnum->align(Fl_Align(FL_ALIGN_LEFT));
  revtron_fnum->when(FL_WHEN_RELEASE);
  revtron_fnum->menu(menu_revtron_fnum);
} // RKR_Choice* revtron_fnum
position(X, Y);
end();
}

void RevtronGui::parameter_refresh(int index) {
  switch (index)
      {
      case Revtron_DryWet:
          revtron_WD->value(Dry_Wet(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_DryWet)));
          break;
      case Revtron_Fade:
          revtron_fade->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Fade));
          break;
      case Revtron_Safe:
          revtron_safe->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Safe));
          break;
      case Revtron_Length:
          revtron_length->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Length));
          break;
      case Revtron_User_File:
      {
          if(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_User_File))
          {
              revtron_user->color (global_leds_color);
              revtron_user->redraw ();
          }
          else
          {
              revtron_user->color (global_fore_color);
              revtron_user->redraw ();
          }
      }
          break;
      case Revtron_I_Delay:
          revtron_idelay->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_I_Delay));
          break;
      case Revtron_Damp:
          revtron_damp->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Damp));
          break;
      case Revtron_Level:
          revtron_level->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Level));
          break;
      case Revtron_Set_File:
          revtron_fnum->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Set_File));
          break;
      case Revtron_Stretch:
          revtron_strech->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Stretch));
          break;
      case Revtron_Feedback:
          revtron_fb->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Feedback));
          break;
      case Revtron_Pan:
          revtron_pan->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Pan)-64);
          break;
      case Revtron_Ex_Stereo:
          revtron_es->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Ex_Stereo));
          break;
      case Revtron_Shuffle:
          revtron_rv->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Shuffle));
          break;
      case Revtron_LPF:
          revtron_LPF->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_LPF));
          break;
      case Revtron_Diffusion:
          revtron_diff->value(m_process->Rack_Effects[EFX_REVERBTRON]->getpar(Revtron_Diffusion));
          break;
      }
}

void RevtronGui::add_reverbtron_file(const std::string &name) {
  revtron_fnum->add(name.c_str ());
      
      Fl_Menu_Item *m = const_cast<Fl_Menu_Item*>  (revtron_fnum->menu ());
      Fl_Menu_Item *p;
  
      int font_size = C_DEFAULT_FONT_SIZE;
  
      for (int i = 0; i < m->size(); i++)
      {
          p = m->next(i);
  
          if (i == 0)
          {
              font_size = p->labelsize();
          }
  
          p->labelsize(font_size);
          p->labelfont (global_font_type);
      }
}

void RevtronGui::add_user_files() {
  for(unsigned i = 0; i < m_process->Reverbtron_RVB_Files.size(); i++)
      {
          add_reverbtron_file(m_process->Reverbtron_RVB_Files[i].User_File_Menu_Name );
      }
}

void RevtronGui::scan_for_new_rvb_files() {
  // This is just to get the current font size so the scan does not change the size
      // when the menu is reloaded
      Fl_Menu_Item *m = const_cast<Fl_Menu_Item*>  (revtron_fnum->menu ());
      int font_size = m->next(0)->labelsize();
  
      // Clear the whole menu and re-add everything
      revtron_fnum->clear();
  
      // Add the default
      revtron_fnum->menu(menu_revtron_fnum);
  
      // Set the font size for the first item, others will follow
      m = const_cast<Fl_Menu_Item*>  (revtron_fnum->menu ());
      m->next(0)->labelsize(font_size);
  
      // Re scan the User Directory and reload user vector
      m_process->load_reverbtron_vector();
  
      // Add user files from vector to menu
      add_user_files();
}
