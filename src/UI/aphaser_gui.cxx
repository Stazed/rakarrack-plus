// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "aphaser_gui.h"

void AphaserGui::cb_aphaser_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Active[EFX_ANALOG_PHASER]);
 return;
}
m_process->EFX_Active[EFX_ANALOG_PHASER]=(int)o->value();
if((int) o->value()==0)
m_process->Rack_Effects[EFX_ANALOG_PHASER]->cleanup();
m_parent->findpos(EFX_ANALOG_PHASER,(int)o->value(),o);
}
void AphaserGui::cb_aphaser_activar(RKR_Light_Button* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_activar_i(o,v);
}

void AphaserGui::cb_aphaser_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_ANALOG_PHASER))
    m_process->Rack_Effects[EFX_ANALOG_PHASER]->setpreset((int) o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_ANALOG_PHASER]; i++)
{
    parameter_refresh(i);
};
}
void AphaserGui::cb_aphaser_preset(RKR_Choice* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_preset_i(o,v);
}

Fl_Menu_Item AphaserGui::menu_aphaser_preset[] = {
 {"Phaser 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Phaser 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Phaser 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Phaser 4", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Phaser 5", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Phaser 6", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void AphaserGui::cb_aphaser_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_DryWet);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_DryWet,Dry_Wet((int)(o->value())));
}
void AphaserGui::cb_aphaser_WD(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_WD_i(o,v);
}

void AphaserGui::cb_aphaser_distort_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_Distortion);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Distortion,(int)o->value());
}
void AphaserGui::cb_aphaser_distort(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_distort_i(o,v);
}

void AphaserGui::cb_aphaser_freq_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_LFO_Tempo);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Tempo,(int)o->value());
}
void AphaserGui::cb_aphaser_freq(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_freq_i(o,v);
}

void AphaserGui::cb_aphaser_rnd_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_LFO_Random);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Random,(int)o->value());
}
void AphaserGui::cb_aphaser_rnd(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_rnd_i(o,v);
}

void AphaserGui::cb_aphaser_lfotype_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_LFO_Type);
 return;
} 

m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Type,(int)o->value());
}
void AphaserGui::cb_aphaser_lfotype(RKR_Choice* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_lfotype_i(o,v);
}

void AphaserGui::cb_aphaser_width_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_Width);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Width,(int)o->value());
}
void AphaserGui::cb_aphaser_width(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_width_i(o,v);
}

void AphaserGui::cb_aphaser_depth_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_Depth);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Depth,(int)o->value());
}
void AphaserGui::cb_aphaser_depth(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_depth_i(o,v);
}

void AphaserGui::cb_aphaser_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_Feedback);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Feedback,(int)(o->value()+64));
}
void AphaserGui::cb_aphaser_fb(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_fb_i(o,v);
}

void AphaserGui::cb_aphaser_mismatch_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_Mismatch);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Mismatch,(int)o->value());
}
void AphaserGui::cb_aphaser_mismatch(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_mismatch_i(o,v);
}

void AphaserGui::cb_aphaser_stdf_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_APhase_LFO_Stereo);
 return;
} 
m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_LFO_Stereo,(int)o->value());
}
void AphaserGui::cb_aphaser_stdf(RKR_Slider* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_stdf_i(o,v);
}

void AphaserGui::cb_aphaser_stages_i(RKR_Counter* o, void*) {
  m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Stages,(int)o->value());
}
void AphaserGui::cb_aphaser_stages(RKR_Counter* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_stages_i(o,v);
}

void AphaserGui::cb_aphaser_subs_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Subtract,(int)o->value());
}
void AphaserGui::cb_aphaser_subs(RKR_Check_Button* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_subs_i(o,v);
}

void AphaserGui::cb_aphaser_hyper_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_ANALOG_PHASER]->changepar(APhase_Hyper,(int)o->value());
}
void AphaserGui::cb_aphaser_hyper(RKR_Check_Button* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_hyper_i(o,v);
}
AphaserGui::AphaserGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = aphaser_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  aphaser_activar->box(FL_UP_BOX);
  aphaser_activar->shortcut(0x37);
  aphaser_activar->color((Fl_Color)62);
  aphaser_activar->selection_color((Fl_Color)1);
  aphaser_activar->labeltype(FL_NORMAL_LABEL);
  aphaser_activar->labelfont(0);
  aphaser_activar->labelsize(10);
  aphaser_activar->labelcolor(FL_FOREGROUND_COLOR);
  aphaser_activar->callback((Fl_Callback*)cb_aphaser_activar);
  aphaser_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  aphaser_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* aphaser_activar
{ RKR_Choice* o = aphaser_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  aphaser_preset->box(FL_FLAT_BOX);
  aphaser_preset->down_box(FL_BORDER_BOX);
  aphaser_preset->color(FL_BACKGROUND_COLOR);
  aphaser_preset->selection_color(FL_FOREGROUND_COLOR);
  aphaser_preset->labeltype(FL_NORMAL_LABEL);
  aphaser_preset->labelfont(0);
  aphaser_preset->labelsize(10);
  aphaser_preset->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_preset->textsize(10);
  aphaser_preset->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_preset->callback((Fl_Callback*)cb_aphaser_preset, (void*)(UD_PRESET_ANALOG_PHASER));
  aphaser_preset->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_preset->when(FL_WHEN_RELEASE_ALWAYS);
  aphaser_preset->menu(menu_aphaser_preset);
  preset_choice = o;
} // RKR_Choice* aphaser_preset
{ aphaser_WD = new RKR_Slider(56, 25, 100, 10, "Dry/Wet");
  aphaser_WD->type(5);
  aphaser_WD->box(FL_FLAT_BOX);
  aphaser_WD->color((Fl_Color)178);
  aphaser_WD->selection_color((Fl_Color)62);
  aphaser_WD->labeltype(FL_NORMAL_LABEL);
  aphaser_WD->labelfont(0);
  aphaser_WD->labelsize(10);
  aphaser_WD->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_WD->maximum(127);
  aphaser_WD->step(1);
  aphaser_WD->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_WD->callback((Fl_Callback*)cb_aphaser_WD);
  aphaser_WD->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_WD
{ aphaser_distort = new RKR_Slider(56, 115, 100, 10, "Distort");
  aphaser_distort->type(5);
  aphaser_distort->box(FL_FLAT_BOX);
  aphaser_distort->color((Fl_Color)178);
  aphaser_distort->selection_color((Fl_Color)62);
  aphaser_distort->labeltype(FL_NORMAL_LABEL);
  aphaser_distort->labelfont(0);
  aphaser_distort->labelsize(10);
  aphaser_distort->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_distort->maximum(127);
  aphaser_distort->step(1);
  aphaser_distort->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_distort->callback((Fl_Callback*)cb_aphaser_distort);
  aphaser_distort->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_distort->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_distort
{ aphaser_freq = new RKR_Slider(56, 56, 100, 10, "Tempo");
  aphaser_freq->type(5);
  aphaser_freq->box(FL_FLAT_BOX);
  aphaser_freq->color((Fl_Color)178);
  aphaser_freq->selection_color((Fl_Color)62);
  aphaser_freq->labeltype(FL_NORMAL_LABEL);
  aphaser_freq->labelfont(0);
  aphaser_freq->labelsize(10);
  aphaser_freq->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_freq->minimum(1);
  aphaser_freq->maximum(600);
  aphaser_freq->step(1);
  aphaser_freq->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_freq->callback((Fl_Callback*)cb_aphaser_freq);
  aphaser_freq->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_freq->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_freq
{ aphaser_rnd = new RKR_Slider(56, 127, 100, 10, "Random");
  aphaser_rnd->type(5);
  aphaser_rnd->box(FL_FLAT_BOX);
  aphaser_rnd->color((Fl_Color)178);
  aphaser_rnd->selection_color((Fl_Color)62);
  aphaser_rnd->labeltype(FL_NORMAL_LABEL);
  aphaser_rnd->labelfont(0);
  aphaser_rnd->labelsize(10);
  aphaser_rnd->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_rnd->maximum(127);
  aphaser_rnd->step(1);
  aphaser_rnd->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_rnd->callback((Fl_Callback*)cb_aphaser_rnd);
  aphaser_rnd->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_rnd->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_rnd
{ RKR_Choice* o = aphaser_lfotype = new RKR_Choice(78, 37, 72, 16, "LFO Type");
  aphaser_lfotype->box(FL_FLAT_BOX);
  aphaser_lfotype->down_box(FL_BORDER_BOX);
  aphaser_lfotype->color(FL_BACKGROUND_COLOR);
  aphaser_lfotype->selection_color(FL_FOREGROUND_COLOR);
  aphaser_lfotype->labeltype(FL_NORMAL_LABEL);
  aphaser_lfotype->labelfont(0);
  aphaser_lfotype->labelsize(10);
  aphaser_lfotype->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_lfotype->textsize(10);
  aphaser_lfotype->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_lfotype->callback((Fl_Callback*)cb_aphaser_lfotype);
  aphaser_lfotype->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_lfotype->when(FL_WHEN_RELEASE);
  o->menu(m_lfo_menu->get_lfo_type());
} // RKR_Choice* aphaser_lfotype
{ aphaser_width = new RKR_Slider(56, 79, 100, 10, "Width");
  aphaser_width->type(5);
  aphaser_width->box(FL_FLAT_BOX);
  aphaser_width->color((Fl_Color)178);
  aphaser_width->selection_color((Fl_Color)62);
  aphaser_width->labeltype(FL_NORMAL_LABEL);
  aphaser_width->labelfont(0);
  aphaser_width->labelsize(10);
  aphaser_width->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_width->maximum(127);
  aphaser_width->step(1);
  aphaser_width->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_width->callback((Fl_Callback*)cb_aphaser_width);
  aphaser_width->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_width->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_width
{ aphaser_depth = new RKR_Slider(56, 67, 100, 10, "P. Depth");
  aphaser_depth->tooltip("Phase Depth");
  aphaser_depth->type(5);
  aphaser_depth->box(FL_FLAT_BOX);
  aphaser_depth->color((Fl_Color)178);
  aphaser_depth->selection_color((Fl_Color)62);
  aphaser_depth->labeltype(FL_NORMAL_LABEL);
  aphaser_depth->labelfont(0);
  aphaser_depth->labelsize(10);
  aphaser_depth->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_depth->maximum(127);
  aphaser_depth->step(1);
  aphaser_depth->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_depth->callback((Fl_Callback*)cb_aphaser_depth);
  aphaser_depth->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_depth->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_depth
{ aphaser_fb = new RKR_Slider(56, 91, 100, 10, "Feedback");
  aphaser_fb->type(5);
  aphaser_fb->box(FL_FLAT_BOX);
  aphaser_fb->color((Fl_Color)178);
  aphaser_fb->selection_color((Fl_Color)62);
  aphaser_fb->labeltype(FL_NORMAL_LABEL);
  aphaser_fb->labelfont(0);
  aphaser_fb->labelsize(10);
  aphaser_fb->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_fb->minimum(-64);
  aphaser_fb->maximum(64);
  aphaser_fb->step(1);
  aphaser_fb->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_fb->callback((Fl_Callback*)cb_aphaser_fb);
  aphaser_fb->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_fb
{ aphaser_mismatch = new RKR_Slider(56, 103, 100, 10, "Mismatch");
  aphaser_mismatch->type(5);
  aphaser_mismatch->box(FL_FLAT_BOX);
  aphaser_mismatch->color((Fl_Color)178);
  aphaser_mismatch->selection_color((Fl_Color)62);
  aphaser_mismatch->labeltype(FL_NORMAL_LABEL);
  aphaser_mismatch->labelfont(0);
  aphaser_mismatch->labelsize(10);
  aphaser_mismatch->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_mismatch->maximum(127);
  aphaser_mismatch->step(1);
  aphaser_mismatch->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_mismatch->callback((Fl_Callback*)cb_aphaser_mismatch);
  aphaser_mismatch->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_mismatch->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_mismatch
{ aphaser_stdf = new RKR_Slider(56, 138, 100, 10, "Stereo Df");
  aphaser_stdf->tooltip("LFO L/R Delay");
  aphaser_stdf->type(5);
  aphaser_stdf->box(FL_FLAT_BOX);
  aphaser_stdf->color((Fl_Color)178);
  aphaser_stdf->selection_color((Fl_Color)62);
  aphaser_stdf->labeltype(FL_NORMAL_LABEL);
  aphaser_stdf->labelfont(0);
  aphaser_stdf->labelsize(10);
  aphaser_stdf->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_stdf->maximum(127);
  aphaser_stdf->step(1);
  aphaser_stdf->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_stdf->callback((Fl_Callback*)cb_aphaser_stdf);
  aphaser_stdf->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_stdf->when(FL_WHEN_CHANGED);
} // RKR_Slider* aphaser_stdf
{ RKR_Counter* o = aphaser_stages = new RKR_Counter(80, 151, 52, 12, "Stages");
  aphaser_stages->type(1);
  aphaser_stages->box(FL_THIN_UP_BOX);
  aphaser_stages->color(FL_BACKGROUND_COLOR);
  aphaser_stages->selection_color(FL_INACTIVE_COLOR);
  aphaser_stages->labeltype(FL_NORMAL_LABEL);
  aphaser_stages->labelfont(0);
  aphaser_stages->labelsize(10);
  aphaser_stages->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_stages->minimum(1);
  aphaser_stages->maximum(12);
  aphaser_stages->step(1);
  aphaser_stages->value(1);
  aphaser_stages->textsize(9);
  aphaser_stages->callback((Fl_Callback*)cb_aphaser_stages);
  aphaser_stages->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_stages->when(FL_WHEN_CHANGED);
  o->set_text_offset(-1);
} // RKR_Counter* aphaser_stages
{ aphaser_subs = new RKR_Check_Button(18, 165, 64, 15, "Subtract");
  aphaser_subs->box(FL_NO_BOX);
  aphaser_subs->down_box(FL_BORDER_BOX);
  aphaser_subs->color(FL_BACKGROUND_COLOR);
  aphaser_subs->selection_color(FL_FOREGROUND_COLOR);
  aphaser_subs->labeltype(FL_NORMAL_LABEL);
  aphaser_subs->labelfont(0);
  aphaser_subs->labelsize(10);
  aphaser_subs->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_subs->callback((Fl_Callback*)cb_aphaser_subs);
  aphaser_subs->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  aphaser_subs->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* aphaser_subs
{ aphaser_hyper = new RKR_Check_Button(90, 165, 53, 15, "Hyper");
  aphaser_hyper->box(FL_NO_BOX);
  aphaser_hyper->down_box(FL_BORDER_BOX);
  aphaser_hyper->color(FL_BACKGROUND_COLOR);
  aphaser_hyper->selection_color(FL_FOREGROUND_COLOR);
  aphaser_hyper->labeltype(FL_NORMAL_LABEL);
  aphaser_hyper->labelfont(0);
  aphaser_hyper->labelsize(10);
  aphaser_hyper->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_hyper->callback((Fl_Callback*)cb_aphaser_hyper);
  aphaser_hyper->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  aphaser_hyper->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* aphaser_hyper
position(X, Y);
end();
}

void AphaserGui::parameter_refresh(int index) {
  switch (index)
      {
      case APhase_DryWet:
          aphaser_WD->value(Dry_Wet(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_DryWet)));
          break;
      case APhase_Distortion:
          aphaser_distort->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Distortion));
          break;
      case APhase_LFO_Tempo:
          aphaser_freq->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Tempo));
          break;
      case APhase_LFO_Random:
          aphaser_rnd->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Random));
          break;
      case APhase_LFO_Type:
          aphaser_lfotype->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Type));
          break;
      case APhase_LFO_Stereo:
          aphaser_stdf->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Stereo));
          break;
      case APhase_Width:
          aphaser_width->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Width));
          break;
      case APhase_Feedback:
          aphaser_fb->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Feedback)-64);
          break;
      case APhase_Stages:
          aphaser_stages->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Stages));
          break;
      case APhase_Mismatch:
          aphaser_mismatch->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Mismatch));
          break;
      case APhase_Subtract:
          aphaser_subs->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Subtract));
          break;
      case APhase_Depth:
          aphaser_depth->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Depth));
          break;
      case APhase_Hyper:
          aphaser_hyper->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_Hyper));
          break;
      }
}

void AphaserGui::tap_tempo_update() {
  aphaser_freq->value(m_process->Rack_Effects[EFX_ANALOG_PHASER]->getpar(APhase_LFO_Tempo));
}
