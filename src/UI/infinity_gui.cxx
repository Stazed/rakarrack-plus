// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "infinity_gui.h"

void InfinityGui::cb_infinity_activar_i(Fl_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116); 
 o->value(rkr->Infinity_Bypass);
 return;
} 
rkr->Infinity_Bypass=(int)o->value();
if((int) o->value()==0)
rkr->efx_Infinity->cleanup();
rgui->findpos(46,(int)o->value(),o);
}
void InfinityGui::cb_infinity_activar(Fl_Light_Button* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_activar_i(o,v);
}

void InfinityGui::cb_infinity_preset_i(Fl_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12046))rkr->efx_Infinity->setpreset((int) o->value());
infinity_WD->value(rkr->efx_Infinity->getpar(0)-64);
infinity_Q->value(rkr->efx_Infinity->getpar(9));
infinity_1->value(rkr->efx_Infinity->getpar(1));
infinity_2->value(rkr->efx_Infinity->getpar(2));
infinity_3->value(rkr->efx_Infinity->getpar(3));
infinity_4->value(rkr->efx_Infinity->getpar(4));
infinity_5->value(rkr->efx_Infinity->getpar(5));
infinity_6->value(rkr->efx_Infinity->getpar(6));
infinity_7->value(rkr->efx_Infinity->getpar(7));
infinity_8->value(rkr->efx_Infinity->getpar(8));
infinity_start->value(rkr->efx_Infinity->getpar(10));
infinity_end->value(rkr->efx_Infinity->getpar(11));
infinity_rate->value(rkr->efx_Infinity->getpar(12));
infinity_stdf->value(rkr->efx_Infinity->getpar(13));
infinity_subdiv->value(rkr->efx_Infinity->getpar(14));
infinity_pan->value(rkr->efx_Infinity->getpar(15));
infinity_rev->value(rkr->efx_Infinity->getpar(16));
infinity_stages->value(rkr->efx_Infinity->getpar(17));
}
void InfinityGui::cb_infinity_preset(Fl_Choice* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_preset_i(o,v);
}

Fl_Menu_Item InfinityGui::menu_infinity_preset[] = {
 {"Basic", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Rising Comb", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Falling Comb", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Laser", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Doppler", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Detune", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Dizzy Sailor", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Stereo Phaser", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Corkscrew", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"FreqeeVox", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void InfinityGui::cb_infinity_WD_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(395);
 return;
} 
rkr->efx_Infinity->changepar(0,(int)(o->value()+64));
}
void InfinityGui::cb_infinity_WD(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_WD_i(o,v);
}

void InfinityGui::cb_infinity_Q_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(396);
 return;
}
rkr->efx_Infinity->changepar(9,(int)o->value());
}
void InfinityGui::cb_infinity_Q(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_Q_i(o,v);
}

void InfinityGui::cb_infinity_1_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(1,(int)o->value());
}
void InfinityGui::cb_infinity_1(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_1_i(o,v);
}

void InfinityGui::cb_infinity_2_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(2,(int)o->value());
}
void InfinityGui::cb_infinity_2(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_2_i(o,v);
}

void InfinityGui::cb_infinity_3_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(3,(int)o->value());
}
void InfinityGui::cb_infinity_3(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_3_i(o,v);
}

void InfinityGui::cb_infinity_4_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(4,(int)o->value());
}
void InfinityGui::cb_infinity_4(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_4_i(o,v);
}

void InfinityGui::cb_infinity_5_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(5,(int)o->value());
}
void InfinityGui::cb_infinity_5(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_5_i(o,v);
}

void InfinityGui::cb_infinity_6_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(6,(int)o->value());
}
void InfinityGui::cb_infinity_6(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_6_i(o,v);
}

void InfinityGui::cb_infinity_7_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(7,(int)o->value());
}
void InfinityGui::cb_infinity_7(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_7_i(o,v);
}

void InfinityGui::cb_infinity_8_i(Fl_Value_Input* o, void*) {
  rkr->efx_Infinity->changepar(8,(int)o->value());
}
void InfinityGui::cb_infinity_8(Fl_Value_Input* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_8_i(o,v);
}

void InfinityGui::cb_infinity_rev_i(Fl_Check_Button* o, void*) {
  rkr->efx_Infinity->changepar(16,(int)o->value());
}
void InfinityGui::cb_infinity_rev(Fl_Check_Button* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_rev_i(o,v);
}

void InfinityGui::cb_infinity_stages_i(Fl_Counter* o, void*) {
  rkr->efx_Infinity->changepar(17,(int)o->value());
}
void InfinityGui::cb_infinity_stages(Fl_Counter* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_stages_i(o,v);
}

void InfinityGui::cb_infinity_pan_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(397);
 return;
} 
rkr->efx_Infinity->changepar(15,(int)o->value());
}
void InfinityGui::cb_infinity_pan(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_pan_i(o,v);
}

void InfinityGui::cb_infinity_stdf_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(398);
 return;
} 
rkr->efx_Infinity->changepar(13,(int)o->value());
}
void InfinityGui::cb_infinity_stdf(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_stdf_i(o,v);
}

void InfinityGui::cb_infinity_start_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(399);
 return;
} 
rkr->efx_Infinity->changepar(10,(int)o->value());
}
void InfinityGui::cb_infinity_start(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_start_i(o,v);
}

void InfinityGui::cb_infinity_end_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(400);
 return;
} 
rkr->efx_Infinity->changepar(11,(int)o->value());
}
void InfinityGui::cb_infinity_end(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_end_i(o,v);
}

void InfinityGui::cb_infinity_rate_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(401);
 return;
} 
rkr->efx_Infinity->changepar(12,(int)o->value());
}
void InfinityGui::cb_infinity_rate(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_rate_i(o,v);
}

void InfinityGui::cb_infinity_subdiv_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(402);
 return;
} 
rkr->efx_Infinity->changepar(14,(int)o->value());
}
void InfinityGui::cb_infinity_subdiv(SliderW* o, void* v) {
  ((InfinityGui*)(o->parent()))->cb_infinity_subdiv_i(o,v);
}
InfinityGui::InfinityGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ infinity_activar = new Fl_Light_Button(8, 5, 34, 18, "On");
  infinity_activar->shortcut(0x31);
  infinity_activar->color((Fl_Color)62);
  infinity_activar->selection_color((Fl_Color)1);
  infinity_activar->labelsize(10);
  infinity_activar->callback((Fl_Callback*)cb_infinity_activar, (void*)(2));
  infinity_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  infinity_activar->when(FL_WHEN_CHANGED);
} // Fl_Light_Button* infinity_activar
{ infinity_preset = new Fl_Choice(80, 5, 76, 18, "Preset");
  infinity_preset->down_box(FL_BORDER_BOX);
  infinity_preset->selection_color(FL_FOREGROUND_COLOR);
  infinity_preset->labelsize(10);
  infinity_preset->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_preset->textsize(10);
  infinity_preset->textcolor(FL_BACKGROUND2_COLOR);
  infinity_preset->callback((Fl_Callback*)cb_infinity_preset, (void*)(12046));
  infinity_preset->when(FL_WHEN_RELEASE_ALWAYS);
  infinity_preset->menu(menu_infinity_preset);
} // Fl_Choice* infinity_preset
{ infinity_WD = new SliderW(52, 27, 100, 10, "Wet/Dry");
  infinity_WD->type(5);
  infinity_WD->box(FL_FLAT_BOX);
  infinity_WD->color((Fl_Color)178);
  infinity_WD->selection_color((Fl_Color)62);
  infinity_WD->labeltype(FL_NORMAL_LABEL);
  infinity_WD->labelfont(0);
  infinity_WD->labelsize(10);
  infinity_WD->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_WD->minimum(-64);
  infinity_WD->maximum(63);
  infinity_WD->step(1);
  infinity_WD->textcolor(FL_BACKGROUND2_COLOR);
  infinity_WD->callback((Fl_Callback*)cb_infinity_WD);
  infinity_WD->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_WD->when(FL_WHEN_CHANGED);
} // SliderW* infinity_WD
{ infinity_Q = new SliderW(52, 41, 100, 10, "Res");
  infinity_Q->type(5);
  infinity_Q->box(FL_FLAT_BOX);
  infinity_Q->color((Fl_Color)178);
  infinity_Q->selection_color((Fl_Color)62);
  infinity_Q->labeltype(FL_NORMAL_LABEL);
  infinity_Q->labelfont(0);
  infinity_Q->labelsize(10);
  infinity_Q->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_Q->minimum(-1000);
  infinity_Q->maximum(1000);
  infinity_Q->step(1);
  infinity_Q->textcolor(FL_BACKGROUND2_COLOR);
  infinity_Q->callback((Fl_Callback*)cb_infinity_Q);
  infinity_Q->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_Q->when(FL_WHEN_CHANGED);
} // SliderW* infinity_Q
{ infinity_1 = new Fl_Value_Input(16, 56, 20, 15, "1");
  infinity_1->labelsize(10);
  infinity_1->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_1->minimum(-64);
  infinity_1->maximum(64);
  infinity_1->step(1);
  infinity_1->textsize(10);
  infinity_1->textcolor(FL_BACKGROUND2_COLOR);
  infinity_1->callback((Fl_Callback*)cb_infinity_1);
} // Fl_Value_Input* infinity_1
{ infinity_2 = new Fl_Value_Input(52, 56, 20, 15, "2");
  infinity_2->labelsize(10);
  infinity_2->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_2->minimum(-64);
  infinity_2->maximum(64);
  infinity_2->step(1);
  infinity_2->textsize(10);
  infinity_2->textcolor(FL_BACKGROUND2_COLOR);
  infinity_2->callback((Fl_Callback*)cb_infinity_2);
} // Fl_Value_Input* infinity_2
{ infinity_3 = new Fl_Value_Input(88, 55, 20, 15, "3");
  infinity_3->labelsize(10);
  infinity_3->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_3->minimum(-64);
  infinity_3->maximum(64);
  infinity_3->step(1);
  infinity_3->textsize(10);
  infinity_3->textcolor(FL_BACKGROUND2_COLOR);
  infinity_3->callback((Fl_Callback*)cb_infinity_3);
} // Fl_Value_Input* infinity_3
{ infinity_4 = new Fl_Value_Input(124, 55, 20, 15, "4");
  infinity_4->labelsize(10);
  infinity_4->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_4->minimum(-64);
  infinity_4->maximum(64);
  infinity_4->step(1);
  infinity_4->textsize(10);
  infinity_4->textcolor(FL_BACKGROUND2_COLOR);
  infinity_4->callback((Fl_Callback*)cb_infinity_4);
} // Fl_Value_Input* infinity_4
{ infinity_5 = new Fl_Value_Input(16, 74, 20, 15, "5");
  infinity_5->labelsize(10);
  infinity_5->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_5->minimum(-64);
  infinity_5->maximum(64);
  infinity_5->step(1);
  infinity_5->textsize(10);
  infinity_5->textcolor(FL_BACKGROUND2_COLOR);
  infinity_5->callback((Fl_Callback*)cb_infinity_5);
} // Fl_Value_Input* infinity_5
{ infinity_6 = new Fl_Value_Input(52, 74, 20, 15, "6");
  infinity_6->labelsize(10);
  infinity_6->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_6->minimum(-64);
  infinity_6->maximum(64);
  infinity_6->step(1);
  infinity_6->textsize(10);
  infinity_6->textcolor(FL_BACKGROUND2_COLOR);
  infinity_6->callback((Fl_Callback*)cb_infinity_6);
} // Fl_Value_Input* infinity_6
{ infinity_7 = new Fl_Value_Input(88, 74, 20, 15, "7");
  infinity_7->labelsize(10);
  infinity_7->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_7->minimum(-64);
  infinity_7->maximum(64);
  infinity_7->step(1);
  infinity_7->textsize(10);
  infinity_7->textcolor(FL_BACKGROUND2_COLOR);
  infinity_7->callback((Fl_Callback*)cb_infinity_7);
} // Fl_Value_Input* infinity_7
{ infinity_8 = new Fl_Value_Input(124, 74, 20, 15, "8");
  infinity_8->labelsize(10);
  infinity_8->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_8->minimum(-64);
  infinity_8->maximum(64);
  infinity_8->step(1);
  infinity_8->textsize(10);
  infinity_8->textcolor(FL_BACKGROUND2_COLOR);
  infinity_8->callback((Fl_Callback*)cb_infinity_8);
} // Fl_Value_Input* infinity_8
{ infinity_rev = new Fl_Check_Button(6, 93, 15, 15, "Rev");
  infinity_rev->tooltip("Reverse left from right directions");
  infinity_rev->down_box(FL_BORDER_BOX);
  infinity_rev->labelsize(10);
  infinity_rev->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_rev->callback((Fl_Callback*)cb_infinity_rev, (void*)(2));
  infinity_rev->align(Fl_Align(FL_ALIGN_RIGHT));
} // Fl_Check_Button* infinity_rev
{ infinity_stages = new Fl_Counter(90, 93, 46, 15, "Stages");
  infinity_stages->type(1);
  infinity_stages->labelsize(10);
  infinity_stages->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_stages->minimum(1);
  infinity_stages->maximum(12);
  infinity_stages->step(1);
  infinity_stages->value(1);
  infinity_stages->textsize(10);
  infinity_stages->callback((Fl_Callback*)cb_infinity_stages);
  infinity_stages->align(Fl_Align(FL_ALIGN_LEFT));
} // Fl_Counter* infinity_stages
{ infinity_pan = new SliderW(52, 111, 100, 10, "AutoPan");
  infinity_pan->type(5);
  infinity_pan->box(FL_FLAT_BOX);
  infinity_pan->color((Fl_Color)178);
  infinity_pan->selection_color((Fl_Color)62);
  infinity_pan->labeltype(FL_NORMAL_LABEL);
  infinity_pan->labelfont(0);
  infinity_pan->labelsize(10);
  infinity_pan->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_pan->maximum(127);
  infinity_pan->step(1);
  infinity_pan->textcolor(FL_BACKGROUND2_COLOR);
  infinity_pan->callback((Fl_Callback*)cb_infinity_pan);
  infinity_pan->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_pan->when(FL_WHEN_CHANGED);
} // SliderW* infinity_pan
{ infinity_stdf = new SliderW(52, 123, 100, 10, "St.df");
  infinity_stdf->type(5);
  infinity_stdf->box(FL_FLAT_BOX);
  infinity_stdf->color((Fl_Color)178);
  infinity_stdf->selection_color((Fl_Color)62);
  infinity_stdf->labeltype(FL_NORMAL_LABEL);
  infinity_stdf->labelfont(0);
  infinity_stdf->labelsize(10);
  infinity_stdf->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_stdf->minimum(-64);
  infinity_stdf->maximum(64);
  infinity_stdf->step(1);
  infinity_stdf->textcolor(FL_BACKGROUND2_COLOR);
  infinity_stdf->callback((Fl_Callback*)cb_infinity_stdf);
  infinity_stdf->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_stdf->when(FL_WHEN_CHANGED);
} // SliderW* infinity_stdf
{ infinity_start = new SliderW(52, 135, 100, 10, "Start");
  infinity_start->type(5);
  infinity_start->box(FL_FLAT_BOX);
  infinity_start->color((Fl_Color)178);
  infinity_start->selection_color((Fl_Color)62);
  infinity_start->labeltype(FL_NORMAL_LABEL);
  infinity_start->labelfont(0);
  infinity_start->labelsize(10);
  infinity_start->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_start->maximum(127);
  infinity_start->step(1);
  infinity_start->textcolor(FL_BACKGROUND2_COLOR);
  infinity_start->callback((Fl_Callback*)cb_infinity_start);
  infinity_start->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_start->when(FL_WHEN_CHANGED);
} // SliderW* infinity_start
{ infinity_end = new SliderW(52, 147, 100, 10, "End");
  infinity_end->type(5);
  infinity_end->box(FL_FLAT_BOX);
  infinity_end->color((Fl_Color)178);
  infinity_end->selection_color((Fl_Color)62);
  infinity_end->labeltype(FL_NORMAL_LABEL);
  infinity_end->labelfont(0);
  infinity_end->labelsize(10);
  infinity_end->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_end->maximum(127);
  infinity_end->step(1);
  infinity_end->textcolor(FL_BACKGROUND2_COLOR);
  infinity_end->callback((Fl_Callback*)cb_infinity_end);
  infinity_end->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_end->when(FL_WHEN_CHANGED);
} // SliderW* infinity_end
{ infinity_rate = new SliderW(52, 159, 100, 10, "Tempo");
  infinity_rate->type(5);
  infinity_rate->box(FL_FLAT_BOX);
  infinity_rate->color((Fl_Color)178);
  infinity_rate->selection_color((Fl_Color)62);
  infinity_rate->labeltype(FL_NORMAL_LABEL);
  infinity_rate->labelfont(0);
  infinity_rate->labelsize(10);
  infinity_rate->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_rate->maximum(600);
  infinity_rate->step(1);
  infinity_rate->textcolor(FL_BACKGROUND2_COLOR);
  infinity_rate->callback((Fl_Callback*)cb_infinity_rate);
  infinity_rate->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_rate->when(FL_WHEN_CHANGED);
} // SliderW* infinity_rate
{ infinity_subdiv = new SliderW(52, 171, 100, 10, "Subdiv");
  infinity_subdiv->type(5);
  infinity_subdiv->box(FL_FLAT_BOX);
  infinity_subdiv->color((Fl_Color)178);
  infinity_subdiv->selection_color((Fl_Color)62);
  infinity_subdiv->labeltype(FL_NORMAL_LABEL);
  infinity_subdiv->labelfont(0);
  infinity_subdiv->labelsize(10);
  infinity_subdiv->labelcolor(FL_BACKGROUND2_COLOR);
  infinity_subdiv->minimum(-16);
  infinity_subdiv->maximum(16);
  infinity_subdiv->step(1);
  infinity_subdiv->textcolor(FL_BACKGROUND2_COLOR);
  infinity_subdiv->callback((Fl_Callback*)cb_infinity_subdiv);
  infinity_subdiv->align(Fl_Align(FL_ALIGN_LEFT));
  infinity_subdiv->when(FL_WHEN_CHANGED);
} // SliderW* infinity_subdiv
position(X, Y);
end();
}
