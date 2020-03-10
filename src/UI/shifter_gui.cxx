// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "shifter_gui.h"

void ShifterGui::cb_shifter_activar_i(Fl_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116);
 o->value(rkr->Shifter_Bypass);
 return;
}
rkr->Shifter_Bypass=(int)o->value();
if((int) o->value()==0)
rkr->efx_Shifter->cleanup();
rgui->findpos(38,(int)o->value(),o);
}
void ShifterGui::cb_shifter_activar(Fl_Light_Button* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_activar_i(o,v);
}

void ShifterGui::cb_shifter_preset_i(Fl_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12038))rkr->efx_Shifter->setpreset((int)o->value());
shifter_WD->value(Dry_Wet(rkr->efx_Shifter->getpar(0)));
shifter_pan->value(rkr->efx_Shifter->getpar(1)-64);
shifter_gain->value(rkr->efx_Shifter->getpar(2)-64);
shifter_int->value(rkr->efx_Shifter->getpar(6));
shifter_attack->value(rkr->efx_Shifter->getpar(3));
shifter_decay->value(rkr->efx_Shifter->getpar(4));
shifter_thre->value(rkr->efx_Shifter->getpar(5));
shifter_ud->value(rkr->efx_Shifter->getpar(7));
shifter_whammy->value(rkr->efx_Shifter->getpar(9));

shifter_mode->value(rkr->efx_Shifter->getpar(8));
}
void ShifterGui::cb_shifter_preset(Fl_Choice* o, void* v) {
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

void ShifterGui::cb_shifter_WD_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(326);
 return;
}
rkr->efx_Shifter->changepar(0,Dry_Wet((int)(o->value())));
}
void ShifterGui::cb_shifter_WD(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_WD_i(o,v);
}

void ShifterGui::cb_shifter_int_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(327);
 return;
}
rkr->Shifter_Bypass=0;
rkr->efx_Shifter->changepar(6,(int)o->value());
if((int)shifter_activar->value())rkr->Shifter_Bypass=1;
}
void ShifterGui::cb_shifter_int(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_int_i(o,v);
}

void ShifterGui::cb_shifter_gain_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(328);
 return;
}
rkr->efx_Shifter->changepar(2,(int)(o->value()+64));
}
void ShifterGui::cb_shifter_gain(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_gain_i(o,v);
}

void ShifterGui::cb_shifter_pan_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(329);
 return;
}
rkr->efx_Shifter->changepar(1,(int)(o->value()+64));
}
void ShifterGui::cb_shifter_pan(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_pan_i(o,v);
}

void ShifterGui::cb_shifter_attack_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(330);
 return;
}
rkr->efx_Shifter->changepar(3,(int)o->value());
}
void ShifterGui::cb_shifter_attack(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_attack_i(o,v);
}

void ShifterGui::cb_shifter_decay_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(331);
 return;
}
rkr->efx_Shifter->changepar(4,(int)o->value());
}
void ShifterGui::cb_shifter_decay(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_decay_i(o,v);
}

void ShifterGui::cb_shifter_thre_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(332);
 return;
}
rkr->efx_Shifter->changepar(5,(int)o->value());
}
void ShifterGui::cb_shifter_thre(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_thre_i(o,v);
}

void ShifterGui::cb_shifter_ud_i(Fl_Check_Button* o, void*) {
  rkr->efx_Shifter->changepar(7,(int)o->value());
}
void ShifterGui::cb_shifter_ud(Fl_Check_Button* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_ud_i(o,v);
}

void ShifterGui::cb_shifter_whammy_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(333);
 return;
}
rkr->efx_Shifter->changepar(9,(int)o->value());
}
void ShifterGui::cb_shifter_whammy(SliderW* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_whammy_i(o,v);
}

void ShifterGui::cb_shifter_mode_i(Fl_Choice* o, void*) {
  rkr->efx_Shifter->changepar(8,(int)o->value());
}
void ShifterGui::cb_shifter_mode(Fl_Choice* o, void* v) {
  ((ShifterGui*)(o->parent()))->cb_shifter_mode_i(o,v);
}

Fl_Menu_Item ShifterGui::menu_shifter_mode[] = {
 {"Trigger", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {"Whammy", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {"Portamento", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 9, 0},
 {0,0,0,0,0,0,0,0,0}
};
ShifterGui::ShifterGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ shifter_activar = new Fl_Light_Button(5, 4, 34, 18, "On");
  shifter_activar->shortcut(0x30);
  shifter_activar->color((Fl_Color)62);
  shifter_activar->selection_color((Fl_Color)1);
  shifter_activar->labelsize(10);
  shifter_activar->callback((Fl_Callback*)cb_shifter_activar, (void*)(2));
  shifter_activar->when(FL_WHEN_CHANGED);
} // Fl_Light_Button* shifter_activar
{ shifter_preset = new Fl_Choice(77, 4, 76, 18, "Preset");
  shifter_preset->down_box(FL_BORDER_BOX);
  shifter_preset->selection_color(FL_FOREGROUND_COLOR);
  shifter_preset->labelsize(10);
  shifter_preset->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_preset->textsize(10);
  shifter_preset->textcolor(FL_BACKGROUND2_COLOR);
  shifter_preset->callback((Fl_Callback*)cb_shifter_preset, (void*)(12038));
  shifter_preset->when(FL_WHEN_RELEASE_ALWAYS);
  shifter_preset->menu(menu_shifter_preset);
} // Fl_Choice* shifter_preset
{ shifter_WD = new SliderW(56, 30, 100, 10, "Dry/Wet");
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
} // SliderW* shifter_WD
{ shifter_int = new SliderW(56, 43, 100, 10, "Interval");
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
} // SliderW* shifter_int
{ shifter_gain = new SliderW(56, 58, 100, 10, "Gain");
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
} // SliderW* shifter_gain
{ shifter_pan = new SliderW(56, 71, 100, 10, "Pan");
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
} // SliderW* shifter_pan
{ shifter_attack = new SliderW(56, 88, 100, 10, "Attack");
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
} // SliderW* shifter_attack
{ shifter_decay = new SliderW(56, 101, 100, 10, "Decay");
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
} // SliderW* shifter_decay
{ shifter_thre = new SliderW(56, 114, 100, 10, "Threshold");
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
} // SliderW* shifter_thre
{ shifter_ud = new Fl_Check_Button(51, 128, 49, 14, "Down");
  shifter_ud->down_box(FL_BORDER_BOX);
  shifter_ud->labelsize(10);
  shifter_ud->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_ud->callback((Fl_Callback*)cb_shifter_ud, (void*)(2));
} // Fl_Check_Button* shifter_ud
{ shifter_whammy = new SliderW(56, 145, 100, 10, "Whammy");
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
} // SliderW* shifter_whammy
{ shifter_mode = new Fl_Choice(43, 165, 78, 13, "Mode");
  shifter_mode->down_box(FL_BORDER_BOX);
  shifter_mode->labelsize(9);
  shifter_mode->labelcolor(FL_BACKGROUND2_COLOR);
  shifter_mode->textsize(9);
  shifter_mode->textcolor(FL_BACKGROUND2_COLOR);
  shifter_mode->callback((Fl_Callback*)cb_shifter_mode, (void*)(12));
  shifter_mode->menu(menu_shifter_mode);
} // Fl_Choice* shifter_mode
position(X, Y);
end();
}
