// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "convo_gui.h"

void ConvoGui::cb_convo_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Bypass[EFX_CONVOLOTRON]);
 return;
}
m_process->EFX_Bypass[EFX_CONVOLOTRON]=(int)o->value();
if((int) o->value()==0)
m_process->Rack_Effects[EFX_CONVOLOTRON]->cleanup();
m_parent->findpos(EFX_CONVOLOTRON,(int)o->value(),o);
}
void ConvoGui::cb_convo_activar(RKR_Light_Button* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_activar_i(o,v);
}

void ConvoGui::cb_convo_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_CONVOLOTRON))
    m_process->Rack_Effects[EFX_CONVOLOTRON]->setpreset((int) o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_CONVOLOTRON]; i++)
{
    parameter_refresh(i);
};
}
void ConvoGui::cb_convo_preset(RKR_Choice* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_preset_i(o,v);
}

Fl_Menu_Item ConvoGui::menu_convo_preset[] = {
 {"Marshall JCM200", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Fender Superchamp", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Mesa Boogie", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Mesa Boogie 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void ConvoGui::cb_convo_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Convo_DryWet);
 return;
}
m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_DryWet,Dry_Wet((int)(o->value())));
}
void ConvoGui::cb_convo_WD(RKR_Slider* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_WD_i(o,v);
}

void ConvoGui::cb_convo_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Convo_Pan);
 return;
}
m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Pan,(int)(o->value()+64));
}
void ConvoGui::cb_convo_pan(RKR_Slider* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_pan_i(o,v);
}

void ConvoGui::cb_convo_level_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Convo_Level);
 return;
}
m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Level,(int)o->value());
}
void ConvoGui::cb_convo_level(RKR_Slider* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_level_i(o,v);
}

void ConvoGui::cb_convo_damp_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Convo_Damp);
 return;
}
m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Damp,(int)o->value());
}
void ConvoGui::cb_convo_damp(RKR_Slider* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_damp_i(o,v);
}

void ConvoGui::cb_convo_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Convo_Feedback);
 return;
}
m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Feedback,(int)o->value());
}
void ConvoGui::cb_convo_fb(RKR_Slider* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_fb_i(o,v);
}

void ConvoGui::cb_convo_length_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Convo_Length);
 return;
}
m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Length,(int)o->value());
}
void ConvoGui::cb_convo_length(RKR_Slider* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_length_i(o,v);
}

void ConvoGui::cb_convo_user_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_User_File,(int)o->value());

if((int)o->value())B_wav->activate(); else B_wav->deactivate();
}
void ConvoGui::cb_convo_user(RKR_Check_Button* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_user_i(o,v);
}

void ConvoGui::cb_convo_safe_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Safe,(int)o->value());
}
void ConvoGui::cb_convo_safe(RKR_Check_Button* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_safe_i(o,v);
}

void ConvoGui::cb_B_wav_i(RKR_Button*, void*) {
  char *filename;
filename=fl_file_chooser("Load Wav File:","(*.wav)",NULL,0);
if (filename==NULL) return;
filename=fl_filename_setext(filename,".wav");

Convolotron *Efx_Convolotron = static_cast<Convolotron*>(m_process->Rack_Effects[EFX_CONVOLOTRON]);
strcpy(Efx_Convolotron->Filename,filename);
if(!Efx_Convolotron->setfile(USERFILE))
{
    fl_alert("Error loading %s file!\n", filename);
};
}
void ConvoGui::cb_B_wav(RKR_Button* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_B_wav_i(o,v);
}

void ConvoGui::cb_convo_fnum_i(RKR_Choice* o, void*) {
  m_process->Rack_Effects[EFX_CONVOLOTRON]->changepar(Convo_Set_File,(int)o->value());
}
void ConvoGui::cb_convo_fnum(RKR_Choice* o, void* v) {
  ((ConvoGui*)(o->parent()))->cb_convo_fnum_i(o,v);
}

Fl_Menu_Item ConvoGui::menu_convo_fnum[] = {
 {"Marshall JCM200", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Fender Superchamp", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Mesa Boogie", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Mesa Boogie 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Marshall Plexi", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Bassman", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"JCM2000", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Ampeg", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Marshall2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};
ConvoGui::ConvoGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = convo_activar = new RKR_Light_Button(6, 4, 34, 18, "On");
  convo_activar->box(FL_UP_BOX);
  convo_activar->shortcut(0x35);
  convo_activar->color((Fl_Color)62);
  convo_activar->selection_color((Fl_Color)1);
  convo_activar->labeltype(FL_NORMAL_LABEL);
  convo_activar->labelfont(0);
  convo_activar->labelsize(10);
  convo_activar->labelcolor(FL_FOREGROUND_COLOR);
  convo_activar->callback((Fl_Callback*)cb_convo_activar);
  convo_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  convo_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* convo_activar
{ RKR_Choice* o = convo_preset = new RKR_Choice(79, 4, 76, 18, "Preset");
  convo_preset->box(FL_FLAT_BOX);
  convo_preset->down_box(FL_BORDER_BOX);
  convo_preset->color(FL_BACKGROUND_COLOR);
  convo_preset->selection_color(FL_FOREGROUND_COLOR);
  convo_preset->labeltype(FL_NORMAL_LABEL);
  convo_preset->labelfont(0);
  convo_preset->labelsize(10);
  convo_preset->labelcolor(FL_BACKGROUND2_COLOR);
  convo_preset->textsize(10);
  convo_preset->textcolor(FL_BACKGROUND2_COLOR);
  convo_preset->callback((Fl_Callback*)cb_convo_preset, (void*)(UD_PRESET_CONVOLOTRON));
  convo_preset->align(Fl_Align(FL_ALIGN_LEFT));
  convo_preset->when(FL_WHEN_RELEASE_ALWAYS);
  convo_preset->menu(menu_convo_preset);
  preset_choice = o;
} // RKR_Choice* convo_preset
{ convo_WD = new RKR_Slider(56, 30, 100, 10, "Dry/Wet");
  convo_WD->type(5);
  convo_WD->box(FL_FLAT_BOX);
  convo_WD->color((Fl_Color)178);
  convo_WD->selection_color((Fl_Color)62);
  convo_WD->labeltype(FL_NORMAL_LABEL);
  convo_WD->labelfont(0);
  convo_WD->labelsize(10);
  convo_WD->labelcolor(FL_BACKGROUND2_COLOR);
  convo_WD->maximum(127);
  convo_WD->step(1);
  convo_WD->textcolor(FL_BACKGROUND2_COLOR);
  convo_WD->callback((Fl_Callback*)cb_convo_WD);
  convo_WD->align(Fl_Align(FL_ALIGN_LEFT));
  convo_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* convo_WD
{ convo_pan = new RKR_Slider(56, 46, 100, 10, "Pan");
  convo_pan->type(5);
  convo_pan->box(FL_FLAT_BOX);
  convo_pan->color((Fl_Color)178);
  convo_pan->selection_color((Fl_Color)62);
  convo_pan->labeltype(FL_NORMAL_LABEL);
  convo_pan->labelfont(0);
  convo_pan->labelsize(10);
  convo_pan->labelcolor(FL_BACKGROUND2_COLOR);
  convo_pan->minimum(-64);
  convo_pan->maximum(63);
  convo_pan->step(1);
  convo_pan->textcolor(FL_BACKGROUND2_COLOR);
  convo_pan->callback((Fl_Callback*)cb_convo_pan);
  convo_pan->align(Fl_Align(FL_ALIGN_LEFT));
  convo_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* convo_pan
{ convo_level = new RKR_Slider(56, 62, 100, 10, "Level");
  convo_level->type(5);
  convo_level->box(FL_FLAT_BOX);
  convo_level->color((Fl_Color)178);
  convo_level->selection_color((Fl_Color)62);
  convo_level->labeltype(FL_NORMAL_LABEL);
  convo_level->labelfont(0);
  convo_level->labelsize(10);
  convo_level->labelcolor(FL_BACKGROUND2_COLOR);
  convo_level->maximum(127);
  convo_level->step(1);
  convo_level->textcolor(FL_BACKGROUND2_COLOR);
  convo_level->callback((Fl_Callback*)cb_convo_level);
  convo_level->align(Fl_Align(FL_ALIGN_LEFT));
  convo_level->when(FL_WHEN_CHANGED);
} // RKR_Slider* convo_level
{ convo_damp = new RKR_Slider(56, 78, 100, 10, "Damp");
  convo_damp->type(5);
  convo_damp->box(FL_FLAT_BOX);
  convo_damp->color((Fl_Color)178);
  convo_damp->selection_color((Fl_Color)62);
  convo_damp->labeltype(FL_NORMAL_LABEL);
  convo_damp->labelfont(0);
  convo_damp->labelsize(10);
  convo_damp->labelcolor(FL_BACKGROUND2_COLOR);
  convo_damp->maximum(127);
  convo_damp->step(1);
  convo_damp->textcolor(FL_BACKGROUND2_COLOR);
  convo_damp->callback((Fl_Callback*)cb_convo_damp);
  convo_damp->align(Fl_Align(FL_ALIGN_LEFT));
  convo_damp->when(FL_WHEN_CHANGED);
} // RKR_Slider* convo_damp
{ convo_fb = new RKR_Slider(56, 94, 100, 10, "Feedback");
  convo_fb->type(5);
  convo_fb->box(FL_FLAT_BOX);
  convo_fb->color((Fl_Color)178);
  convo_fb->selection_color((Fl_Color)62);
  convo_fb->labeltype(FL_NORMAL_LABEL);
  convo_fb->labelfont(0);
  convo_fb->labelsize(10);
  convo_fb->labelcolor(FL_BACKGROUND2_COLOR);
  convo_fb->minimum(-64);
  convo_fb->maximum(64);
  convo_fb->step(1);
  convo_fb->textcolor(FL_BACKGROUND2_COLOR);
  convo_fb->callback((Fl_Callback*)cb_convo_fb);
  convo_fb->align(Fl_Align(FL_ALIGN_LEFT));
  convo_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* convo_fb
{ convo_length = new RKR_Slider(56, 110, 100, 10, "Length");
  convo_length->type(5);
  convo_length->box(FL_FLAT_BOX);
  convo_length->color((Fl_Color)178);
  convo_length->selection_color((Fl_Color)62);
  convo_length->labeltype(FL_NORMAL_LABEL);
  convo_length->labelfont(0);
  convo_length->labelsize(10);
  convo_length->labelcolor(FL_BACKGROUND2_COLOR);
  convo_length->minimum(5);
  convo_length->maximum(250);
  convo_length->step(1);
  convo_length->value(100);
  convo_length->textcolor(FL_BACKGROUND2_COLOR);
  convo_length->callback((Fl_Callback*)cb_convo_length);
  convo_length->align(Fl_Align(FL_ALIGN_LEFT));
  convo_length->when(FL_WHEN_RELEASE);
} // RKR_Slider* convo_length
{ convo_user = new RKR_Check_Button(110, 126, 43, 15, "User");
  convo_user->box(FL_NO_BOX);
  convo_user->down_box(FL_BORDER_BOX);
  convo_user->color(FL_BACKGROUND_COLOR);
  convo_user->selection_color(FL_FOREGROUND_COLOR);
  convo_user->labeltype(FL_NORMAL_LABEL);
  convo_user->labelfont(0);
  convo_user->labelsize(10);
  convo_user->labelcolor(FL_BACKGROUND2_COLOR);
  convo_user->callback((Fl_Callback*)cb_convo_user);
  convo_user->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  convo_user->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* convo_user
{ convo_safe = new RKR_Check_Button(19, 141, 73, 15, "Safe Mode");
  convo_safe->box(FL_NO_BOX);
  convo_safe->down_box(FL_BORDER_BOX);
  convo_safe->color(FL_BACKGROUND_COLOR);
  convo_safe->selection_color(FL_FOREGROUND_COLOR);
  convo_safe->labeltype(FL_NORMAL_LABEL);
  convo_safe->labelfont(0);
  convo_safe->labelsize(10);
  convo_safe->labelcolor(FL_BACKGROUND2_COLOR);
  convo_safe->callback((Fl_Callback*)cb_convo_safe);
  convo_safe->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  convo_safe->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* convo_safe
{ B_wav = new RKR_Button(106, 143, 46, 12, "Browse");
  B_wav->box(FL_UP_BOX);
  B_wav->color(FL_BACKGROUND_COLOR);
  B_wav->selection_color(FL_BACKGROUND_COLOR);
  B_wav->labeltype(FL_NORMAL_LABEL);
  B_wav->labelfont(0);
  B_wav->labelsize(10);
  B_wav->labelcolor(FL_FOREGROUND_COLOR);
  B_wav->callback((Fl_Callback*)cb_B_wav);
  B_wav->align(Fl_Align(FL_ALIGN_CENTER));
  B_wav->when(FL_WHEN_RELEASE);
  B_wav->deactivate();
} // RKR_Button* B_wav
{ convo_fnum = new RKR_Choice(51, 159, 101, 16, "Preset");
  convo_fnum->box(FL_FLAT_BOX);
  convo_fnum->down_box(FL_BORDER_BOX);
  convo_fnum->color(FL_BACKGROUND_COLOR);
  convo_fnum->selection_color(FL_FOREGROUND_COLOR);
  convo_fnum->labeltype(FL_NORMAL_LABEL);
  convo_fnum->labelfont(0);
  convo_fnum->labelsize(10);
  convo_fnum->labelcolor(FL_BACKGROUND2_COLOR);
  convo_fnum->textsize(10);
  convo_fnum->textcolor(FL_BACKGROUND2_COLOR);
  convo_fnum->callback((Fl_Callback*)cb_convo_fnum, (void*)(12));
  convo_fnum->align(Fl_Align(FL_ALIGN_LEFT));
  convo_fnum->when(FL_WHEN_RELEASE);
  convo_fnum->menu(menu_convo_fnum);
} // RKR_Choice* convo_fnum
position(X, Y);
end();
}

void ConvoGui::parameter_refresh(int index) {
  switch (index)
      {
      case Convo_DryWet:
          convo_WD->value(Dry_Wet(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_DryWet)));
          break;
      case Convo_Pan:
          convo_pan->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Pan)-64);
          break;
      case Convo_Safe:
          convo_safe->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Safe));
          break;
      case Convo_Length:
          convo_length->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Length));
          break;
      case Convo_User_File:
          convo_user->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_User_File));
          convo_user->do_callback();
          break;
      case Convo_SKIP_5:
          break;
      case Convo_Damp:
          convo_damp->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Damp));
          break;
      case Convo_Level:
          convo_level->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Level));
          break;
      case Convo_Set_File:
          convo_fnum->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Set_File));
          break;
      case Convo_SKIP_9:
          break;
      case Convo_Feedback:
          convo_fb->value(m_process->Rack_Effects[EFX_CONVOLOTRON]->getpar(Convo_Feedback));
          break;
      }
}
