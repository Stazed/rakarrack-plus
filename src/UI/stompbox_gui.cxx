// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "stompbox_gui.h"

void StompboxGui::cb_stomp_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Bypass[EFX_STOMPBOX]);
 return;
}
m_process->EFX_Bypass[EFX_STOMPBOX]=(int)o->value();
if((int) o->value()==0)
m_process->Rack_Effects[EFX_STOMPBOX]->cleanup();
m_parent->findpos(EFX_STOMPBOX,(int)o->value(),o);
}
void StompboxGui::cb_stomp_activar(RKR_Light_Button* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_activar_i(o,v);
}

void StompboxGui::cb_stomp_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_STOMPBOX))
    m_process->Rack_Effects[EFX_STOMPBOX]->setpreset((int)o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_STOMPBOX]; i++)
{
    parameter_refresh(i);
};
}
void StompboxGui::cb_stomp_preset(RKR_Choice* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_preset_i(o,v);
}

Fl_Menu_Item StompboxGui::menu_stomp_preset[] = {
 {"Amp 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Odie", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Grunger", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Hard Dist.", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Ratula", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Classic Dist", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Morbid Impalement", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Sharp Metal", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Classic Fuzz", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Gnasty", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Amp 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void StompboxGui::cb_stomp_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Stomp_Level);
 return;
}
m_process->Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Level,(int)o->value());
}
void StompboxGui::cb_stomp_WD(RKR_Slider* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_WD_i(o,v);
}

void StompboxGui::cb_stomp_gain_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Stomp_Gain);
 return;
}
m_process->Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Gain,(int)o->value());
}
void StompboxGui::cb_stomp_gain(RKR_Slider* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_gain_i(o,v);
}

void StompboxGui::cb_stomp_low_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Stomp_Bias);
 return;
}
m_process->Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Bias,(int)o->value());
}
void StompboxGui::cb_stomp_low(RKR_Slider* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_low_i(o,v);
}

void StompboxGui::cb_stomp_mid_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Stomp_Mid);
 return;
}
m_process->Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Mid,(int)o->value());
}
void StompboxGui::cb_stomp_mid(RKR_Slider* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_mid_i(o,v);
}

void StompboxGui::cb_stomp_high_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Stomp_Tone);
 return;
}
m_process->Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Tone,(int)o->value());
}
void StompboxGui::cb_stomp_high(RKR_Slider* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_high_i(o,v);
}

void StompboxGui::cb_stomp_mode_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Stomp_Mode);
 return;
}

m_process->Rack_Effects[EFX_STOMPBOX]->changepar(Stomp_Mode,(int)o->value());
}
void StompboxGui::cb_stomp_mode(RKR_Choice* o, void* v) {
  ((StompboxGui*)(o->parent()))->cb_stomp_mode_i(o,v);
}

Fl_Menu_Item StompboxGui::menu_stomp_mode[] = {
 {"Amp", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Grunge", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Rat", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Fat Cat", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Dist+", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Death", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Mid Elves Own", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Fuzz", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};
StompboxGui::StompboxGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = stomp_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  stomp_activar->box(FL_UP_BOX);
  stomp_activar->shortcut(0x30);
  stomp_activar->color((Fl_Color)62);
  stomp_activar->selection_color((Fl_Color)1);
  stomp_activar->labeltype(FL_NORMAL_LABEL);
  stomp_activar->labelfont(0);
  stomp_activar->labelsize(10);
  stomp_activar->labelcolor(FL_FOREGROUND_COLOR);
  stomp_activar->callback((Fl_Callback*)cb_stomp_activar);
  stomp_activar->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  stomp_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* stomp_activar
{ RKR_Choice* o = stomp_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  stomp_preset->box(FL_FLAT_BOX);
  stomp_preset->down_box(FL_BORDER_BOX);
  stomp_preset->color(FL_BACKGROUND_COLOR);
  stomp_preset->selection_color(FL_FOREGROUND_COLOR);
  stomp_preset->labeltype(FL_NORMAL_LABEL);
  stomp_preset->labelfont(0);
  stomp_preset->labelsize(10);
  stomp_preset->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_preset->textsize(10);
  stomp_preset->textcolor(FL_BACKGROUND2_COLOR);
  stomp_preset->callback((Fl_Callback*)cb_stomp_preset, (void*)(UD_PRESET_STOMPBOX));
  stomp_preset->align(Fl_Align(FL_ALIGN_LEFT));
  stomp_preset->when(FL_WHEN_RELEASE_ALWAYS);
  stomp_preset->menu(menu_stomp_preset);
  preset_choice = o;
} // RKR_Choice* stomp_preset
{ stomp_WD = new RKR_Slider(56, 30, 100, 10, "Level");
  stomp_WD->type(5);
  stomp_WD->box(FL_FLAT_BOX);
  stomp_WD->color((Fl_Color)178);
  stomp_WD->selection_color((Fl_Color)62);
  stomp_WD->labeltype(FL_NORMAL_LABEL);
  stomp_WD->labelfont(0);
  stomp_WD->labelsize(10);
  stomp_WD->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_WD->maximum(127);
  stomp_WD->step(1);
  stomp_WD->textcolor(FL_BACKGROUND2_COLOR);
  stomp_WD->callback((Fl_Callback*)cb_stomp_WD);
  stomp_WD->align(Fl_Align(FL_ALIGN_LEFT));
  stomp_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* stomp_WD
{ stomp_gain = new RKR_Slider(56, 58, 100, 10, "Gain");
  stomp_gain->type(5);
  stomp_gain->box(FL_FLAT_BOX);
  stomp_gain->color((Fl_Color)178);
  stomp_gain->selection_color((Fl_Color)62);
  stomp_gain->labeltype(FL_NORMAL_LABEL);
  stomp_gain->labelfont(0);
  stomp_gain->labelsize(10);
  stomp_gain->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_gain->maximum(127);
  stomp_gain->step(1);
  stomp_gain->textcolor(FL_BACKGROUND2_COLOR);
  stomp_gain->callback((Fl_Callback*)cb_stomp_gain);
  stomp_gain->align(Fl_Align(FL_ALIGN_LEFT));
  stomp_gain->when(FL_WHEN_CHANGED);
} // RKR_Slider* stomp_gain
{ stomp_low = new RKR_Slider(56, 75, 100, 10, "Bias");
  stomp_low->type(5);
  stomp_low->box(FL_FLAT_BOX);
  stomp_low->color((Fl_Color)178);
  stomp_low->selection_color((Fl_Color)62);
  stomp_low->labeltype(FL_NORMAL_LABEL);
  stomp_low->labelfont(0);
  stomp_low->labelsize(10);
  stomp_low->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_low->minimum(-64);
  stomp_low->maximum(64);
  stomp_low->step(1);
  stomp_low->textcolor(FL_BACKGROUND2_COLOR);
  stomp_low->callback((Fl_Callback*)cb_stomp_low);
  stomp_low->align(Fl_Align(FL_ALIGN_LEFT));
  stomp_low->when(FL_WHEN_CHANGED);
} // RKR_Slider* stomp_low
{ stomp_mid = new RKR_Slider(56, 92, 100, 10, "Mid");
  stomp_mid->type(5);
  stomp_mid->box(FL_FLAT_BOX);
  stomp_mid->color((Fl_Color)178);
  stomp_mid->selection_color((Fl_Color)62);
  stomp_mid->labeltype(FL_NORMAL_LABEL);
  stomp_mid->labelfont(0);
  stomp_mid->labelsize(10);
  stomp_mid->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_mid->minimum(-64);
  stomp_mid->maximum(64);
  stomp_mid->step(1);
  stomp_mid->textcolor(FL_BACKGROUND2_COLOR);
  stomp_mid->callback((Fl_Callback*)cb_stomp_mid);
  stomp_mid->align(Fl_Align(36));
  stomp_mid->when(FL_WHEN_CHANGED);
} // RKR_Slider* stomp_mid
{ stomp_high = new RKR_Slider(56, 109, 100, 10, "Tone");
  stomp_high->type(5);
  stomp_high->box(FL_FLAT_BOX);
  stomp_high->color((Fl_Color)178);
  stomp_high->selection_color((Fl_Color)62);
  stomp_high->labeltype(FL_NORMAL_LABEL);
  stomp_high->labelfont(0);
  stomp_high->labelsize(10);
  stomp_high->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_high->minimum(-64);
  stomp_high->maximum(64);
  stomp_high->step(1);
  stomp_high->textcolor(FL_BACKGROUND2_COLOR);
  stomp_high->callback((Fl_Callback*)cb_stomp_high);
  stomp_high->align(Fl_Align(FL_ALIGN_LEFT));
  stomp_high->when(FL_WHEN_CHANGED);
} // RKR_Slider* stomp_high
{ stomp_mode = new RKR_Choice(60, 139, 78, 18, "Mode");
  stomp_mode->box(FL_FLAT_BOX);
  stomp_mode->down_box(FL_BORDER_BOX);
  stomp_mode->color(FL_BACKGROUND_COLOR);
  stomp_mode->selection_color(FL_SELECTION_COLOR);
  stomp_mode->labeltype(FL_NORMAL_LABEL);
  stomp_mode->labelfont(0);
  stomp_mode->labelsize(10);
  stomp_mode->labelcolor(FL_BACKGROUND2_COLOR);
  stomp_mode->textsize(10);
  stomp_mode->textcolor(FL_BACKGROUND2_COLOR);
  stomp_mode->callback((Fl_Callback*)cb_stomp_mode, (void*)(12));
  stomp_mode->align(Fl_Align(FL_ALIGN_LEFT));
  stomp_mode->when(FL_WHEN_RELEASE);
  stomp_mode->menu(menu_stomp_mode);
} // RKR_Choice* stomp_mode
position(X, Y);
end();
}

void StompboxGui::parameter_refresh(int index) {
  switch (index)
      {
      case Stomp_Level:
          stomp_WD->value(m_process->Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Level));
          break;
      case Stomp_Tone:
          stomp_high->value(m_process->Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Tone));
          break;
      case Stomp_Mid:
          stomp_mid->value(m_process->Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Mid));
          break;
      case Stomp_Bias:
          stomp_low->value(m_process->Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Bias));
          break;
      case Stomp_Gain:
          stomp_gain->value(m_process->Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Gain));
          break;
      case Stomp_Mode:
          stomp_mode->value(m_process->Rack_Effects[EFX_STOMPBOX]->getpar(Stomp_Mode));
          break;
      }
}
