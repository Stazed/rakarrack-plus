// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "echo_gui.h"

void EchoGui::cb_echo_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Active[EFX_ECHO]);
 return;
}
m_process->EFX_Active[EFX_ECHO]=(int)o->value();
if((int) o->value()==0)
m_process->Rack_Effects[EFX_ECHO]->cleanup();
m_parent->findpos(EFX_ECHO,(int)o->value(),o);
}
void EchoGui::cb_echo_activar(RKR_Light_Button* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_activar_i(o,v);
}

void EchoGui::cb_echo_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_ECHO))
    m_process->Rack_Effects[EFX_ECHO]->setpreset((int) o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_ECHO]; i++)
{
    parameter_refresh(i);
};
}
void EchoGui::cb_echo_preset(RKR_Choice* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_preset_i(o,v);
}

Fl_Menu_Item EchoGui::menu_echo_preset[] = {
 {"Echo 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Echo 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Echo 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Simple Echo", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Canyon", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Panning Echo 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Panning Echo 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Panning Echo 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Feedback Echo", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void EchoGui::cb_echo_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_DryWet);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_DryWet,Dry_Wet((int)(o->value())));
}
void EchoGui::cb_echo_WD(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_WD_i(o,v);
}

void EchoGui::cb_echo_RV_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_Reverse);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_Reverse,(int)o->value());
}
void EchoGui::cb_echo_RV(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_RV_i(o,v);
}

void EchoGui::cb_echo_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_Pan);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_Pan,(int)(o->value()+64));
}
void EchoGui::cb_echo_pan(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_pan_i(o,v);
}

void EchoGui::cb_echo_delay_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_Delay);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_Delay,(int)o->value());
}
void EchoGui::cb_echo_delay(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_delay_i(o,v);
}

void EchoGui::cb_echo_LRdl_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_LR_Delay);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_LR_Delay,(int)o->value());
}
void EchoGui::cb_echo_LRdl(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_LRdl_i(o,v);
}

void EchoGui::cb_echo_LRc_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_LR_Cross);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_LR_Cross,(int)(o->value()));
}
void EchoGui::cb_echo_LRc(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_LRc_i(o,v);
}

void EchoGui::cb_echo_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_Feedback);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_Feedback,(int)o->value());
}
void EchoGui::cb_echo_fb(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_fb_i(o,v);
}

void EchoGui::cb_echo_direct_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_Direct,(int)o->value());
}
void EchoGui::cb_echo_direct(RKR_Check_Button* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_direct_i(o,v);
}

void EchoGui::cb_echo_damp_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Echo_Damp);
 return;
} 
m_process->Rack_Effects[EFX_ECHO]->changepar(Echo_Damp,(int)o->value());
}
void EchoGui::cb_echo_damp(RKR_Slider* o, void* v) {
  ((EchoGui*)(o->parent()))->cb_echo_damp_i(o,v);
}
EchoGui::EchoGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = echo_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  echo_activar->box(FL_UP_BOX);
  echo_activar->shortcut(0x35);
  echo_activar->color((Fl_Color)62);
  echo_activar->selection_color((Fl_Color)1);
  echo_activar->labeltype(FL_NORMAL_LABEL);
  echo_activar->labelfont(0);
  echo_activar->labelsize(10);
  echo_activar->labelcolor(FL_FOREGROUND_COLOR);
  echo_activar->callback((Fl_Callback*)cb_echo_activar);
  echo_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  echo_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* echo_activar
{ RKR_Choice* o = echo_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  echo_preset->box(FL_FLAT_BOX);
  echo_preset->down_box(FL_BORDER_BOX);
  echo_preset->color(FL_BACKGROUND_COLOR);
  echo_preset->selection_color(FL_FOREGROUND_COLOR);
  echo_preset->labeltype(FL_NORMAL_LABEL);
  echo_preset->labelfont(0);
  echo_preset->labelsize(10);
  echo_preset->labelcolor(FL_BACKGROUND2_COLOR);
  echo_preset->textsize(10);
  echo_preset->textcolor(FL_BACKGROUND2_COLOR);
  echo_preset->callback((Fl_Callback*)cb_echo_preset, (void*)(UD_PRESET_ECHO));
  echo_preset->align(Fl_Align(FL_ALIGN_LEFT));
  echo_preset->when(FL_WHEN_RELEASE_ALWAYS);
  echo_preset->menu(menu_echo_preset);
  preset_choice = o;
} // RKR_Choice* echo_preset
{ echo_WD = new RKR_Slider(56, 35, 100, 10, "Dry/Wet");
  echo_WD->type(5);
  echo_WD->box(FL_FLAT_BOX);
  echo_WD->color((Fl_Color)178);
  echo_WD->selection_color((Fl_Color)62);
  echo_WD->labeltype(FL_NORMAL_LABEL);
  echo_WD->labelfont(0);
  echo_WD->labelsize(10);
  echo_WD->labelcolor(FL_BACKGROUND2_COLOR);
  echo_WD->maximum(127);
  echo_WD->step(1);
  echo_WD->textcolor(FL_BACKGROUND2_COLOR);
  echo_WD->callback((Fl_Callback*)cb_echo_WD);
  echo_WD->align(Fl_Align(FL_ALIGN_LEFT));
  echo_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* echo_WD
{ echo_RV = new RKR_Slider(56, 58, 100, 10, "Reverse");
  echo_RV->type(5);
  echo_RV->box(FL_FLAT_BOX);
  echo_RV->color((Fl_Color)178);
  echo_RV->selection_color((Fl_Color)62);
  echo_RV->labeltype(FL_NORMAL_LABEL);
  echo_RV->labelfont(0);
  echo_RV->labelsize(10);
  echo_RV->labelcolor(FL_BACKGROUND2_COLOR);
  echo_RV->maximum(127);
  echo_RV->step(1);
  echo_RV->textcolor(FL_BACKGROUND2_COLOR);
  echo_RV->callback((Fl_Callback*)cb_echo_RV);
  echo_RV->align(Fl_Align(FL_ALIGN_LEFT));
  echo_RV->when(FL_WHEN_CHANGED);
} // RKR_Slider* echo_RV
{ echo_pan = new RKR_Slider(56, 73, 100, 10, "Pan");
  echo_pan->type(5);
  echo_pan->box(FL_FLAT_BOX);
  echo_pan->color((Fl_Color)178);
  echo_pan->selection_color((Fl_Color)62);
  echo_pan->labeltype(FL_NORMAL_LABEL);
  echo_pan->labelfont(0);
  echo_pan->labelsize(10);
  echo_pan->labelcolor(FL_BACKGROUND2_COLOR);
  echo_pan->minimum(-64);
  echo_pan->maximum(63);
  echo_pan->step(1);
  echo_pan->textcolor(FL_BACKGROUND2_COLOR);
  echo_pan->callback((Fl_Callback*)cb_echo_pan);
  echo_pan->align(Fl_Align(FL_ALIGN_LEFT));
  echo_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* echo_pan
{ echo_delay = new RKR_Slider(56, 88, 100, 10, "Delay");
  echo_delay->type(5);
  echo_delay->box(FL_FLAT_BOX);
  echo_delay->color((Fl_Color)178);
  echo_delay->selection_color((Fl_Color)62);
  echo_delay->labeltype(FL_NORMAL_LABEL);
  echo_delay->labelfont(0);
  echo_delay->labelsize(10);
  echo_delay->labelcolor(FL_BACKGROUND2_COLOR);
  echo_delay->minimum(20);
  echo_delay->maximum(2000);
  echo_delay->step(1);
  echo_delay->value(20);
  echo_delay->textcolor(FL_BACKGROUND2_COLOR);
  echo_delay->callback((Fl_Callback*)cb_echo_delay);
  echo_delay->align(Fl_Align(FL_ALIGN_LEFT));
  echo_delay->when(FL_WHEN_RELEASE);
} // RKR_Slider* echo_delay
{ echo_LRdl = new RKR_Slider(56, 103, 100, 10, "L/R Delay");
  echo_LRdl->type(5);
  echo_LRdl->box(FL_FLAT_BOX);
  echo_LRdl->color((Fl_Color)178);
  echo_LRdl->selection_color((Fl_Color)62);
  echo_LRdl->labeltype(FL_NORMAL_LABEL);
  echo_LRdl->labelfont(0);
  echo_LRdl->labelsize(10);
  echo_LRdl->labelcolor(FL_BACKGROUND2_COLOR);
  echo_LRdl->maximum(127);
  echo_LRdl->step(1);
  echo_LRdl->textcolor(FL_BACKGROUND2_COLOR);
  echo_LRdl->callback((Fl_Callback*)cb_echo_LRdl);
  echo_LRdl->align(Fl_Align(FL_ALIGN_LEFT));
  echo_LRdl->when(FL_WHEN_RELEASE);
} // RKR_Slider* echo_LRdl
{ echo_LRc = new RKR_Slider(56, 118, 100, 10, "L/R Cross");
  echo_LRc->type(5);
  echo_LRc->box(FL_FLAT_BOX);
  echo_LRc->color((Fl_Color)178);
  echo_LRc->selection_color((Fl_Color)62);
  echo_LRc->labeltype(FL_NORMAL_LABEL);
  echo_LRc->labelfont(0);
  echo_LRc->labelsize(10);
  echo_LRc->labelcolor(FL_BACKGROUND2_COLOR);
  echo_LRc->maximum(127);
  echo_LRc->step(1);
  echo_LRc->textcolor(FL_BACKGROUND2_COLOR);
  echo_LRc->callback((Fl_Callback*)cb_echo_LRc);
  echo_LRc->align(Fl_Align(FL_ALIGN_LEFT));
  echo_LRc->when(FL_WHEN_CHANGED);
} // RKR_Slider* echo_LRc
{ echo_fb = new RKR_Slider(56, 133, 100, 10, "Feedback");
  echo_fb->type(5);
  echo_fb->box(FL_FLAT_BOX);
  echo_fb->color((Fl_Color)178);
  echo_fb->selection_color((Fl_Color)62);
  echo_fb->labeltype(FL_NORMAL_LABEL);
  echo_fb->labelfont(0);
  echo_fb->labelsize(10);
  echo_fb->labelcolor(FL_BACKGROUND2_COLOR);
  echo_fb->maximum(127);
  echo_fb->step(1);
  echo_fb->textcolor(FL_BACKGROUND2_COLOR);
  echo_fb->callback((Fl_Callback*)cb_echo_fb);
  echo_fb->align(Fl_Align(FL_ALIGN_LEFT));
  echo_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* echo_fb
{ echo_direct = new RKR_Check_Button(51, 148, 53, 15, "Direct");
  echo_direct->box(FL_NO_BOX);
  echo_direct->down_box(FL_BORDER_BOX);
  echo_direct->color(FL_BACKGROUND_COLOR);
  echo_direct->selection_color(FL_FOREGROUND_COLOR);
  echo_direct->labeltype(FL_NORMAL_LABEL);
  echo_direct->labelfont(0);
  echo_direct->labelsize(10);
  echo_direct->labelcolor(FL_BACKGROUND2_COLOR);
  echo_direct->callback((Fl_Callback*)cb_echo_direct);
  echo_direct->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  echo_direct->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* echo_direct
{ echo_damp = new RKR_Slider(56, 167, 100, 10, "Damp");
  echo_damp->type(5);
  echo_damp->box(FL_FLAT_BOX);
  echo_damp->color((Fl_Color)178);
  echo_damp->selection_color((Fl_Color)62);
  echo_damp->labeltype(FL_NORMAL_LABEL);
  echo_damp->labelfont(0);
  echo_damp->labelsize(10);
  echo_damp->labelcolor(FL_BACKGROUND2_COLOR);
  echo_damp->maximum(127);
  echo_damp->step(1);
  echo_damp->textcolor(FL_BACKGROUND2_COLOR);
  echo_damp->callback((Fl_Callback*)cb_echo_damp);
  echo_damp->align(Fl_Align(FL_ALIGN_LEFT));
  echo_damp->when(FL_WHEN_CHANGED);
} // RKR_Slider* echo_damp
position(X, Y);
end();
}

void EchoGui::parameter_refresh(int index) {
  switch (index)
      {
      case Echo_DryWet:
          echo_WD->value(Dry_Wet(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_DryWet)));
          break;
      case Echo_Pan:
          echo_pan->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Pan)-64);
          break;
      case Echo_Delay:
          echo_delay->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Delay));
          break;
      case Echo_LR_Delay:
          echo_LRdl->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_LR_Delay));
          break;
      case Echo_LR_Cross:
          echo_LRc->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_LR_Cross));
          break;
      case Echo_Feedback:
          echo_fb->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Feedback));
          break;
      case Echo_Damp:
          echo_damp->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Damp));
          break;
      case Echo_Reverse:
          echo_RV->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Reverse));
          break;
      case Echo_Direct:
          echo_direct->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Direct));
          break;
      };
}

void EchoGui::tap_tempo_update() {
  echo_delay->value(m_process->Rack_Effects[EFX_ECHO]->getpar(Echo_Delay));
}
