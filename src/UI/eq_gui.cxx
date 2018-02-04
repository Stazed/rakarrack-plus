// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "eq_gui.h"

void EqGui::cb_eq_activar_i(Fl_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116);
 o->value(rkr->EQ1_Bypass);
 return;
} 
rkr->EQ1_Bypass=(int)o->value();
if((int) o->value()==0)
rkr->efx_EQ1->cleanup();
rgui->findpos(0,(int)o->value(),o);
}
void EqGui::cb_eq_activar(Fl_Light_Button* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_activar_i(o,v);
}

void EqGui::cb_eq_preset_i(Fl_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12000))rkr->EQ1_setpreset((int)o->value());
eq_1->value(rkr->efx_EQ1->getpar(12)-64);
eq_2->value(rkr->efx_EQ1->getpar(17)-64);
eq_3->value(rkr->efx_EQ1->getpar(22)-64);
eq_4->value(rkr->efx_EQ1->getpar(27)-64);
eq_5->value(rkr->efx_EQ1->getpar(32)-64);
eq_6->value(rkr->efx_EQ1->getpar(37)-64);
eq_7->value(rkr->efx_EQ1->getpar(42)-64);
eq_8->value(rkr->efx_EQ1->getpar(47)-64);
eq_9->value(rkr->efx_EQ1->getpar(52)-64);
eq_10->value(rkr->efx_EQ1->getpar(57)-64);
eq_Gain->value(rkr->efx_EQ1->getpar(0)-64);
eq_Q->value(rkr->efx_EQ1->getpar(13)-64);
}
void EqGui::cb_eq_preset(Fl_Choice* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_preset_i(o,v);
}

Fl_Menu_Item EqGui::menu_eq_preset[] = {
 {"Plain", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Pop", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Jazz", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void EqGui::cb_eq_Gain_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(130);
 return;
} 
rkr->efx_EQ1->changepar(0,(int)(o->value()+64));
}
void EqGui::cb_eq_Gain(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_Gain_i(o,v);
}

void EqGui::cb_eq_Q_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(131);
 return;
} 
int i;
for(i=0;i<10;i++) rkr->efx_EQ1->changepar(i*5+13,(int)(o->value()+64));
}
void EqGui::cb_eq_Q(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_Q_i(o,v);
}

void EqGui::cb_eq_1_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(132);
 return;
} 
rkr->efx_EQ1->changepar(12,(int)(o->value()+64));
}
void EqGui::cb_eq_1(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_1_i(o,v);
}

void EqGui::cb_eq_2_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(133);
 return;
} 
rkr->efx_EQ1->changepar(5+12,(int)(o->value()+64));
}
void EqGui::cb_eq_2(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_2_i(o,v);
}

void EqGui::cb_eq_3_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(134);
 return;
} 
rkr->efx_EQ1->changepar(10+12,(int)(o->value()+64));
}
void EqGui::cb_eq_3(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_3_i(o,v);
}

void EqGui::cb_eq_4_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(135);
 return;
} 
rkr->efx_EQ1->changepar(15+12,(int)(o->value()+64));
}
void EqGui::cb_eq_4(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_4_i(o,v);
}

void EqGui::cb_eq_5_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(136);
 return;
} 
rkr->efx_EQ1->changepar(20+12,(int)(o->value()+64));
}
void EqGui::cb_eq_5(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_5_i(o,v);
}

void EqGui::cb_eq_6_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(137);
 return;
} 
rkr->efx_EQ1->changepar(25+12,(int)(o->value()+64));
}
void EqGui::cb_eq_6(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_6_i(o,v);
}

void EqGui::cb_eq_7_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(138);
 return;
} 
rkr->efx_EQ1->changepar(30+12,(int)(o->value()+64));
}
void EqGui::cb_eq_7(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_7_i(o,v);
}

void EqGui::cb_eq_8_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(139);
 return;
} 
rkr->efx_EQ1->changepar(35+12,(int)(o->value()+64));
}
void EqGui::cb_eq_8(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_8_i(o,v);
}

void EqGui::cb_eq_9_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(140);
 return;
} 
rkr->efx_EQ1->changepar(40+12,(int)(o->value()+64));
}
void EqGui::cb_eq_9(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_9_i(o,v);
}

void EqGui::cb_eq_10_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(141);
 return;
} 
rkr->efx_EQ1->changepar(45+12,(int)(o->value()+64));
}
void EqGui::cb_eq_10(SliderW* o, void* v) {
  ((EqGui*)(o->parent()))->cb_eq_10_i(o,v);
}
EqGui::EqGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ eq_activar = new Fl_Light_Button(5, 2, 34, 18, "On");
  eq_activar->shortcut(0x31);
  eq_activar->color((Fl_Color)62);
  eq_activar->selection_color((Fl_Color)1);
  eq_activar->labelsize(10);
  eq_activar->callback((Fl_Callback*)cb_eq_activar, (void*)(2));
  eq_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  eq_activar->when(FL_WHEN_CHANGED);
} // Fl_Light_Button* eq_activar
{ eq_preset = new Fl_Choice(77, 2, 76, 18, "Preset");
  eq_preset->down_box(FL_BORDER_BOX);
  eq_preset->selection_color(FL_FOREGROUND_COLOR);
  eq_preset->labelsize(10);
  eq_preset->labelcolor(FL_BACKGROUND2_COLOR);
  eq_preset->textsize(10);
  eq_preset->textcolor(FL_BACKGROUND2_COLOR);
  eq_preset->callback((Fl_Callback*)cb_eq_preset, (void*)(12000));
  eq_preset->when(FL_WHEN_RELEASE_ALWAYS);
  eq_preset->menu(menu_eq_preset);
} // Fl_Choice* eq_preset
{ eq_Gain = new SliderW(49, 24, 100, 10, "Gain");
  eq_Gain->type(5);
  eq_Gain->box(FL_FLAT_BOX);
  eq_Gain->color((Fl_Color)178);
  eq_Gain->selection_color((Fl_Color)62);
  eq_Gain->labeltype(FL_NORMAL_LABEL);
  eq_Gain->labelfont(0);
  eq_Gain->labelsize(10);
  eq_Gain->labelcolor(FL_BACKGROUND2_COLOR);
  eq_Gain->minimum(-64);
  eq_Gain->maximum(63);
  eq_Gain->step(1);
  eq_Gain->textcolor(FL_BACKGROUND2_COLOR);
  eq_Gain->callback((Fl_Callback*)cb_eq_Gain);
  eq_Gain->align(Fl_Align(FL_ALIGN_LEFT));
  eq_Gain->when(FL_WHEN_CHANGED);
} // SliderW* eq_Gain
{ eq_Q = new SliderW(49, 37, 100, 10, "Q");
  eq_Q->type(5);
  eq_Q->box(FL_FLAT_BOX);
  eq_Q->color((Fl_Color)178);
  eq_Q->selection_color((Fl_Color)62);
  eq_Q->labeltype(FL_NORMAL_LABEL);
  eq_Q->labelfont(0);
  eq_Q->labelsize(10);
  eq_Q->labelcolor(FL_BACKGROUND2_COLOR);
  eq_Q->minimum(-64);
  eq_Q->maximum(63);
  eq_Q->step(1);
  eq_Q->textcolor(FL_BACKGROUND2_COLOR);
  eq_Q->callback((Fl_Callback*)cb_eq_Q);
  eq_Q->align(Fl_Align(FL_ALIGN_LEFT));
  eq_Q->when(FL_WHEN_CHANGED);
} // SliderW* eq_Q
{ eq_1 = new SliderW(49, 56, 100, 10, "31 Hz");
  eq_1->type(5);
  eq_1->box(FL_FLAT_BOX);
  eq_1->color((Fl_Color)178);
  eq_1->selection_color((Fl_Color)62);
  eq_1->labeltype(FL_NORMAL_LABEL);
  eq_1->labelfont(0);
  eq_1->labelsize(10);
  eq_1->labelcolor(FL_BACKGROUND2_COLOR);
  eq_1->minimum(-64);
  eq_1->maximum(63);
  eq_1->step(1);
  eq_1->textcolor(FL_BACKGROUND2_COLOR);
  eq_1->callback((Fl_Callback*)cb_eq_1);
  eq_1->align(Fl_Align(FL_ALIGN_LEFT));
  eq_1->when(FL_WHEN_CHANGED);
} // SliderW* eq_1
{ eq_2 = new SliderW(49, 68, 100, 10, "63 Hz");
  eq_2->type(5);
  eq_2->box(FL_FLAT_BOX);
  eq_2->color((Fl_Color)178);
  eq_2->selection_color((Fl_Color)62);
  eq_2->labeltype(FL_NORMAL_LABEL);
  eq_2->labelfont(0);
  eq_2->labelsize(10);
  eq_2->labelcolor(FL_BACKGROUND2_COLOR);
  eq_2->minimum(-64);
  eq_2->maximum(63);
  eq_2->step(1);
  eq_2->textcolor(FL_BACKGROUND2_COLOR);
  eq_2->callback((Fl_Callback*)cb_eq_2);
  eq_2->align(Fl_Align(FL_ALIGN_LEFT));
  eq_2->when(FL_WHEN_CHANGED);
} // SliderW* eq_2
{ eq_3 = new SliderW(49, 80, 100, 10, "125 Hz");
  eq_3->type(5);
  eq_3->box(FL_FLAT_BOX);
  eq_3->color((Fl_Color)178);
  eq_3->selection_color((Fl_Color)62);
  eq_3->labeltype(FL_NORMAL_LABEL);
  eq_3->labelfont(0);
  eq_3->labelsize(10);
  eq_3->labelcolor(FL_BACKGROUND2_COLOR);
  eq_3->minimum(-64);
  eq_3->maximum(63);
  eq_3->step(1);
  eq_3->textcolor(FL_BACKGROUND2_COLOR);
  eq_3->callback((Fl_Callback*)cb_eq_3);
  eq_3->align(Fl_Align(FL_ALIGN_LEFT));
  eq_3->when(FL_WHEN_CHANGED);
} // SliderW* eq_3
{ eq_4 = new SliderW(49, 93, 100, 10, "250 Hz");
  eq_4->type(5);
  eq_4->box(FL_FLAT_BOX);
  eq_4->color((Fl_Color)178);
  eq_4->selection_color((Fl_Color)62);
  eq_4->labeltype(FL_NORMAL_LABEL);
  eq_4->labelfont(0);
  eq_4->labelsize(10);
  eq_4->labelcolor(FL_BACKGROUND2_COLOR);
  eq_4->minimum(-64);
  eq_4->maximum(63);
  eq_4->step(1);
  eq_4->textcolor(FL_BACKGROUND2_COLOR);
  eq_4->callback((Fl_Callback*)cb_eq_4);
  eq_4->align(Fl_Align(FL_ALIGN_LEFT));
  eq_4->when(FL_WHEN_CHANGED);
} // SliderW* eq_4
{ eq_5 = new SliderW(49, 105, 100, 10, "500 Hz");
  eq_5->type(5);
  eq_5->box(FL_FLAT_BOX);
  eq_5->color((Fl_Color)178);
  eq_5->selection_color((Fl_Color)62);
  eq_5->labeltype(FL_NORMAL_LABEL);
  eq_5->labelfont(0);
  eq_5->labelsize(10);
  eq_5->labelcolor(FL_BACKGROUND2_COLOR);
  eq_5->minimum(-64);
  eq_5->maximum(63);
  eq_5->step(1);
  eq_5->textcolor(FL_BACKGROUND2_COLOR);
  eq_5->callback((Fl_Callback*)cb_eq_5);
  eq_5->align(Fl_Align(FL_ALIGN_LEFT));
  eq_5->when(FL_WHEN_CHANGED);
} // SliderW* eq_5
{ eq_6 = new SliderW(49, 117, 100, 10, "1 Khz");
  eq_6->type(5);
  eq_6->box(FL_FLAT_BOX);
  eq_6->color((Fl_Color)178);
  eq_6->selection_color((Fl_Color)62);
  eq_6->labeltype(FL_NORMAL_LABEL);
  eq_6->labelfont(0);
  eq_6->labelsize(10);
  eq_6->labelcolor(FL_BACKGROUND2_COLOR);
  eq_6->minimum(-64);
  eq_6->maximum(63);
  eq_6->step(1);
  eq_6->textcolor(FL_BACKGROUND2_COLOR);
  eq_6->callback((Fl_Callback*)cb_eq_6);
  eq_6->align(Fl_Align(FL_ALIGN_LEFT));
  eq_6->when(FL_WHEN_CHANGED);
} // SliderW* eq_6
{ eq_7 = new SliderW(49, 130, 100, 10, "2 Khz");
  eq_7->type(5);
  eq_7->box(FL_FLAT_BOX);
  eq_7->color((Fl_Color)178);
  eq_7->selection_color((Fl_Color)62);
  eq_7->labeltype(FL_NORMAL_LABEL);
  eq_7->labelfont(0);
  eq_7->labelsize(10);
  eq_7->labelcolor(FL_BACKGROUND2_COLOR);
  eq_7->minimum(-64);
  eq_7->maximum(63);
  eq_7->step(1);
  eq_7->textcolor(FL_BACKGROUND2_COLOR);
  eq_7->callback((Fl_Callback*)cb_eq_7);
  eq_7->align(Fl_Align(FL_ALIGN_LEFT));
  eq_7->when(FL_WHEN_CHANGED);
} // SliderW* eq_7
{ eq_8 = new SliderW(49, 142, 100, 10, "4 Khz");
  eq_8->type(5);
  eq_8->box(FL_FLAT_BOX);
  eq_8->color((Fl_Color)178);
  eq_8->selection_color((Fl_Color)62);
  eq_8->labeltype(FL_NORMAL_LABEL);
  eq_8->labelfont(0);
  eq_8->labelsize(10);
  eq_8->labelcolor(FL_BACKGROUND2_COLOR);
  eq_8->minimum(-64);
  eq_8->maximum(63);
  eq_8->step(1);
  eq_8->textcolor(FL_BACKGROUND2_COLOR);
  eq_8->callback((Fl_Callback*)cb_eq_8);
  eq_8->align(Fl_Align(FL_ALIGN_LEFT));
  eq_8->when(FL_WHEN_CHANGED);
} // SliderW* eq_8
{ eq_9 = new SliderW(49, 154, 100, 10, "8 Khz");
  eq_9->type(5);
  eq_9->box(FL_FLAT_BOX);
  eq_9->color((Fl_Color)178);
  eq_9->selection_color((Fl_Color)62);
  eq_9->labeltype(FL_NORMAL_LABEL);
  eq_9->labelfont(0);
  eq_9->labelsize(10);
  eq_9->labelcolor(FL_BACKGROUND2_COLOR);
  eq_9->minimum(-64);
  eq_9->maximum(63);
  eq_9->step(1);
  eq_9->textcolor(FL_BACKGROUND2_COLOR);
  eq_9->callback((Fl_Callback*)cb_eq_9);
  eq_9->align(Fl_Align(FL_ALIGN_LEFT));
  eq_9->when(FL_WHEN_CHANGED);
} // SliderW* eq_9
{ eq_10 = new SliderW(49, 167, 100, 10, "16 Khz");
  eq_10->type(5);
  eq_10->box(FL_FLAT_BOX);
  eq_10->color((Fl_Color)178);
  eq_10->selection_color((Fl_Color)62);
  eq_10->labeltype(FL_NORMAL_LABEL);
  eq_10->labelfont(0);
  eq_10->labelsize(10);
  eq_10->labelcolor(FL_BACKGROUND2_COLOR);
  eq_10->minimum(-64);
  eq_10->maximum(63);
  eq_10->step(1);
  eq_10->textcolor(FL_BACKGROUND2_COLOR);
  eq_10->callback((Fl_Callback*)cb_eq_10);
  eq_10->align(Fl_Align(FL_ALIGN_LEFT));
  eq_10->when(FL_WHEN_CHANGED);
} // SliderW* eq_10
position(X, Y);
end();
}
