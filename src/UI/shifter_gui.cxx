// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "shifter_gui.h"

void ShifterGui::cb_shifter_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Active[EFX_SHIFTER]);
 return;
}
m_process->EFX_Active[EFX_SHIFTER]=(int)o->value();
if((int) o->value()==0)
m_process->Rack_Effects[EFX_SHIFTER]->cleanup();
m_parent->findpos(EFX_SHIFTER,(int)o->value(),o);
}
void ShifterGui::cb_shifter_activar(RKR_Light_Button* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_activar_i(o,v);
}

void ShifterGui::cb_shifter_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_SHIFTER))
    m_process->Rack_Effects[EFX_SHIFTER]->setpreset((int)o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_SHIFTER]; i++)
{
    parameter_refresh(i);
};
}
void ShifterGui::cb_shifter_preset(RKR_Choice* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_preset_i(o,v);
}

Fl_Menu_Item ShifterGui::menu_shifter_preset[] = {
 {"Fast", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Slow Up", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Slow Down", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Chorus", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Trig. Chorus", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Shifter 6", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void ShifterGui::cb_shifter_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_DryWet);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_DryWet,Dry_Wet((int)(o->value())));
}
void ShifterGui::cb_shifter_WD(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_WD_i(o,v);
}

void ShifterGui::cb_shifter_int_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Interval);
 return;
}
m_process->EFX_Active[EFX_SHIFTER]=0;
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Interval,(int)o->value());
if((int)shifter_activar->value())m_process->EFX_Active[EFX_SHIFTER]=1;
}
void ShifterGui::cb_shifter_int(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_int_i(o,v);
}

void ShifterGui::cb_shifter_gain_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Gain);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Gain,(int)(o->value()+64));
}
void ShifterGui::cb_shifter_gain(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_gain_i(o,v);
}

void ShifterGui::cb_shifter_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Pan);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Pan,(int)(o->value()+64));
}
void ShifterGui::cb_shifter_pan(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_pan_i(o,v);
}

void ShifterGui::cb_shifter_attack_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Attack);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Attack,(int)o->value());
}
void ShifterGui::cb_shifter_attack(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_attack_i(o,v);
}

void ShifterGui::cb_shifter_decay_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Decay);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Decay,(int)o->value());
}
void ShifterGui::cb_shifter_decay(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_decay_i(o,v);
}

void ShifterGui::cb_shifter_thre_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Threshold);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Threshold,(int)o->value());
}
void ShifterGui::cb_shifter_thre(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_thre_i(o,v);
}

void ShifterGui::cb_shifter_ud_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Shift,(int)o->value());
}
void ShifterGui::cb_shifter_ud(RKR_Check_Button* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_ud_i(o,v);
}

void ShifterGui::cb_shifter_whammy_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Shifter_Whammy);
 return;
}
m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Whammy,(int)o->value());
}
void ShifterGui::cb_shifter_whammy(RKR_Slider* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_whammy_i(o,v);
}

void ShifterGui::cb_shifter_mode_i(RKR_Choice* o, void*) {
  m_process->Rack_Effects[EFX_SHIFTER]->changepar(Shifter_Mode,(int)o->value());
}
void ShifterGui::cb_shifter_mode(RKR_Choice* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_mode_i(o,v);
}

Fl_Menu_Item ShifterGui::menu_shifter_mode[] = {
 {"Trigger", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {"Whammy", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {"Portamento", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {0,0,0,0,0,0,0,0,0}
};
ShifterGui::ShifterGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = shifter_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  shifter_activar->box(FL_UP_BOX);
  shifter_activar->shortcut(0x30);
  shifter_activar->color((Fl_Color)62);
  shifter_activar->selection_color((Fl_Color)1);
  shifter_activar->labeltype(FL_NORMAL_LABEL);
  shifter_activar->labelfont(0);
  shifter_activar->labelsize(10);
  shifter_activar->labelcolor(FL_FOREGROUND_COLOR);
  shifter_activar->callback((Fl_Callback*)cb_shifter_activar);
  shifter_activar->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  shifter_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* shifter_activar
{ RKR_Choice* o = shifter_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  shifter_preset->box(FL_FLAT_BOX);
  shifter_preset->down_box(FL_BORDER_BOX);
  shifter_preset->color(FL_BACKGROUND_COLOR);
  shifter_preset->selection_color(FL_FOREGROUND_COLOR);
  shifter_preset->labeltype(FL_NORMAL_LABEL);
  shifter_preset->labelfont(0);
  shifter_preset->labelsize(10);
  shifter_preset->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_preset->textsize(10);
  shifter_preset->textcolor(FL_BACKGROUND2_COLOR);
  shifter_preset->callback((Fl_Callback*)cb_shifter_preset, (void*)(UD_PRESET_SHIFTER));
  shifter_preset->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_preset->when(FL_WHEN_RELEASE_ALWAYS);
  shifter_preset->menu(menu_shifter_preset);
  preset_choice = o;
} // RKR_Choice* shifter_preset
{ shifter_WD = new RKR_Slider(56, 30, 100, 10, "Dry/Wet");
  shifter_WD->type(5);
  shifter_WD->box(FL_FLAT_BOX);
  shifter_WD->color((Fl_Color)178);
  shifter_WD->selection_color((Fl_Color)62);
  shifter_WD->labeltype(FL_NORMAL_LABEL);
  shifter_WD->labelfont(0);
  shifter_WD->labelsize(10);
  shifter_WD->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_WD->maximum(127);
  shifter_WD->step(1);
  shifter_WD->textcolor(FL_BACKGROUND2_COLOR);
  shifter_WD->callback((Fl_Callback*)cb_shifter_WD);
  shifter_WD->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_WD
{ shifter_int = new RKR_Slider(56, 43, 100, 10, "Interval");
  shifter_int->type(5);
  shifter_int->box(FL_FLAT_BOX);
  shifter_int->color((Fl_Color)178);
  shifter_int->selection_color((Fl_Color)62);
  shifter_int->labeltype(FL_NORMAL_LABEL);
  shifter_int->labelfont(0);
  shifter_int->labelsize(10);
  shifter_int->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_int->maximum(12);
  shifter_int->step(1);
  shifter_int->textcolor(FL_BACKGROUND2_COLOR);
  shifter_int->callback((Fl_Callback*)cb_shifter_int);
  shifter_int->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_int->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_int
{ shifter_gain = new RKR_Slider(56, 58, 100, 10, "Gain");
  shifter_gain->type(5);
  shifter_gain->box(FL_FLAT_BOX);
  shifter_gain->color((Fl_Color)178);
  shifter_gain->selection_color((Fl_Color)62);
  shifter_gain->labeltype(FL_NORMAL_LABEL);
  shifter_gain->labelfont(0);
  shifter_gain->labelsize(10);
  shifter_gain->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_gain->minimum(-64);
  shifter_gain->maximum(63);
  shifter_gain->step(1);
  shifter_gain->textcolor(FL_BACKGROUND2_COLOR);
  shifter_gain->callback((Fl_Callback*)cb_shifter_gain);
  shifter_gain->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_gain->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_gain
{ shifter_pan = new RKR_Slider(56, 71, 100, 10, "Pan");
  shifter_pan->type(5);
  shifter_pan->box(FL_FLAT_BOX);
  shifter_pan->color((Fl_Color)178);
  shifter_pan->selection_color((Fl_Color)62);
  shifter_pan->labeltype(FL_NORMAL_LABEL);
  shifter_pan->labelfont(0);
  shifter_pan->labelsize(10);
  shifter_pan->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_pan->minimum(-64);
  shifter_pan->maximum(63);
  shifter_pan->step(1);
  shifter_pan->textcolor(FL_BACKGROUND2_COLOR);
  shifter_pan->callback((Fl_Callback*)cb_shifter_pan);
  shifter_pan->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_pan
{ shifter_attack = new RKR_Slider(56, 88, 100, 10, "Attack");
  shifter_attack->type(5);
  shifter_attack->box(FL_FLAT_BOX);
  shifter_attack->color((Fl_Color)178);
  shifter_attack->selection_color((Fl_Color)62);
  shifter_attack->labeltype(FL_NORMAL_LABEL);
  shifter_attack->labelfont(0);
  shifter_attack->labelsize(10);
  shifter_attack->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_attack->minimum(1);
  shifter_attack->maximum(2000);
  shifter_attack->step(1);
  shifter_attack->value(100);
  shifter_attack->textcolor(FL_BACKGROUND2_COLOR);
  shifter_attack->callback((Fl_Callback*)cb_shifter_attack);
  shifter_attack->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_attack->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_attack
{ shifter_decay = new RKR_Slider(56, 101, 100, 10, "Decay");
  shifter_decay->type(5);
  shifter_decay->box(FL_FLAT_BOX);
  shifter_decay->color((Fl_Color)178);
  shifter_decay->selection_color((Fl_Color)62);
  shifter_decay->labeltype(FL_NORMAL_LABEL);
  shifter_decay->labelfont(0);
  shifter_decay->labelsize(10);
  shifter_decay->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_decay->minimum(1);
  shifter_decay->maximum(2000);
  shifter_decay->step(1);
  shifter_decay->value(100);
  shifter_decay->textcolor(FL_BACKGROUND2_COLOR);
  shifter_decay->callback((Fl_Callback*)cb_shifter_decay);
  shifter_decay->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_decay->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_decay
{ shifter_thre = new RKR_Slider(56, 114, 100, 10, "Threshold");
  shifter_thre->tooltip("Threshold");
  shifter_thre->type(5);
  shifter_thre->box(FL_FLAT_BOX);
  shifter_thre->color((Fl_Color)178);
  shifter_thre->selection_color((Fl_Color)62);
  shifter_thre->labeltype(FL_NORMAL_LABEL);
  shifter_thre->labelfont(0);
  shifter_thre->labelsize(10);
  shifter_thre->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_thre->minimum(-70);
  shifter_thre->maximum(20);
  shifter_thre->step(1);
  shifter_thre->value(20);
  shifter_thre->textcolor(FL_BACKGROUND2_COLOR);
  shifter_thre->callback((Fl_Callback*)cb_shifter_thre);
  shifter_thre->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_thre->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_thre
{ shifter_ud = new RKR_Check_Button(51, 128, 49, 14, "Down");
  shifter_ud->box(FL_NO_BOX);
  shifter_ud->down_box(FL_BORDER_BOX);
  shifter_ud->color(FL_BACKGROUND_COLOR);
  shifter_ud->selection_color(FL_FOREGROUND_COLOR);
  shifter_ud->labeltype(FL_NORMAL_LABEL);
  shifter_ud->labelfont(0);
  shifter_ud->labelsize(10);
  shifter_ud->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_ud->callback((Fl_Callback*)cb_shifter_ud);
  shifter_ud->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  shifter_ud->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* shifter_ud
{ shifter_whammy = new RKR_Slider(56, 145, 100, 10, "Whammy");
  shifter_whammy->type(5);
  shifter_whammy->box(FL_FLAT_BOX);
  shifter_whammy->color((Fl_Color)178);
  shifter_whammy->selection_color((Fl_Color)62);
  shifter_whammy->labeltype(FL_NORMAL_LABEL);
  shifter_whammy->labelfont(0);
  shifter_whammy->labelsize(10);
  shifter_whammy->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_whammy->maximum(127);
  shifter_whammy->step(1);
  shifter_whammy->textcolor(FL_BACKGROUND2_COLOR);
  shifter_whammy->callback((Fl_Callback*)cb_shifter_whammy);
  shifter_whammy->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_whammy->when(FL_WHEN_CHANGED);
} // RKR_Slider* shifter_whammy
{ RKR_Choice* o = shifter_mode = new RKR_Choice(43, 165, 78, 13, "Mode");
  shifter_mode->box(FL_FLAT_BOX);
  shifter_mode->down_box(FL_BORDER_BOX);
  shifter_mode->color(FL_BACKGROUND_COLOR);
  shifter_mode->selection_color(FL_SELECTION_COLOR);
  shifter_mode->labeltype(FL_NORMAL_LABEL);
  shifter_mode->labelfont(0);
  shifter_mode->labelsize(9);
  shifter_mode->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_mode->textsize(9);
  shifter_mode->textcolor(FL_BACKGROUND2_COLOR);
  shifter_mode->callback((Fl_Callback*)cb_shifter_mode);
  shifter_mode->align(Fl_Align(FL_ALIGN_LEFT));
  shifter_mode->when(FL_WHEN_RELEASE);
  shifter_mode->menu(menu_shifter_mode);
  o->set_label_offset(-1);
  o->set_text_offset(-1);
} // RKR_Choice* shifter_mode
position(X, Y);
end();
}

void ShifterGui::parameter_refresh(int index) {
  switch (index)
      {
      case Shifter_DryWet:
          shifter_WD->value(Dry_Wet(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_DryWet)));
          break;
      case Shifter_Pan:
          shifter_pan->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Pan)-64);
          break;
      case Shifter_Gain:
          shifter_gain->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Gain)-64);
          break;
      case Shifter_Attack:
          shifter_attack->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Attack));
          break;
      case Shifter_Decay:
          shifter_decay->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Decay));
          break;
      case Shifter_Threshold:
          shifter_thre->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Threshold));
          break;
      case Shifter_Interval:
          shifter_int->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Interval));
          break;
      case Shifter_Shift:
          shifter_ud->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Shift));
          break;
      case Shifter_Mode:
          shifter_mode->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Mode));
          break;
      case Shifter_Whammy:
          shifter_whammy->value(m_process->Rack_Effects[EFX_SHIFTER]->getpar(Shifter_Whammy));
          break;
      }
}
