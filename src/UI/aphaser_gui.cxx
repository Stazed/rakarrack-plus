// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "aphaser_gui.h"

void AphaserGui::cb_aphaser_activar_i(Fl_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116);
 o->value(rkr->APhaser_Bypass);
 return;
}
rkr->APhaser_Bypass=(int)o->value();
if((int) o->value()==0)
rkr->efx_APhaser->cleanup();
rgui->findpos(18,(int)o->value(),o);
}
void AphaserGui::cb_aphaser_activar(Fl_Light_Button* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_activar_i(o,v);
}

void AphaserGui::cb_aphaser_preset_i(Fl_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12018))rkr->efx_APhaser->setpreset((int) o->value());
aphaser_WD->value(Dry_Wet(rkr->efx_APhaser->getpar(0)));
aphaser_distort->value(rkr->efx_APhaser->getpar(1));
aphaser_freq->value(rkr->efx_APhaser->getpar(2));
aphaser_rnd->value(rkr->efx_APhaser->getpar(3));
aphaser_lfotype->value(rkr->efx_APhaser->getpar(4));
aphaser_stdf->value(rkr->efx_APhaser->getpar(5));
aphaser_width->value(rkr->efx_APhaser->getpar(6));
aphaser_stages->value(rkr->efx_APhaser->getpar(8));
aphaser_fb->value(rkr->efx_APhaser->getpar(7)-64);
aphaser_mismatch->value(rkr->efx_APhaser->getpar(9));
aphaser_subs->value(rkr->efx_APhaser->getpar(10));
aphaser_phase->value(rkr->efx_APhaser->getpar(11));
aphaser_hyper->value(rkr->efx_APhaser->getpar(12));
}
void AphaserGui::cb_aphaser_preset(Fl_Choice* o, void* v) {
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

void AphaserGui::cb_aphaser_WD_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(117);
 return;
} 
rkr->efx_APhaser->changepar(0,Dry_Wet((int)(o->value())));
}
void AphaserGui::cb_aphaser_WD(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_WD_i(o,v);
}

void AphaserGui::cb_aphaser_distort_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(118);
 return;
} 
rkr->efx_APhaser->changepar(1,(int)o->value());
}
void AphaserGui::cb_aphaser_distort(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_distort_i(o,v);
}

void AphaserGui::cb_aphaser_freq_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(119);
 return;
} 
rkr->efx_APhaser->changepar(2,(int)o->value());
}
void AphaserGui::cb_aphaser_freq(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_freq_i(o,v);
}

void AphaserGui::cb_aphaser_rnd_i(SliderW* o, void*) {
  // No midi control
//if(Fl::event_button()==3)
//{
// rgui->getMIDIControl(121);
// return;
//} 
rkr->efx_APhaser->changepar(3,(int)o->value());
}
void AphaserGui::cb_aphaser_rnd(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_rnd_i(o,v);
}

void AphaserGui::cb_aphaser_lfotype_i(Fl_Choice* o, void*) {
  rkr->efx_APhaser->changepar(4,(int)o->value());
}
void AphaserGui::cb_aphaser_lfotype(Fl_Choice* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_lfotype_i(o,v);
}

void AphaserGui::cb_aphaser_width_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(121);
 return;
} 
rkr->efx_APhaser->changepar(6,(int)o->value());
}
void AphaserGui::cb_aphaser_width(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_width_i(o,v);
}

void AphaserGui::cb_aphaser_phase_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(120);
 return;
} 
rkr->efx_APhaser->changepar(11,(int)o->value());
}
void AphaserGui::cb_aphaser_phase(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_phase_i(o,v);
}

void AphaserGui::cb_aphaser_fb_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(122);
 return;
} 
rkr->efx_APhaser->changepar(7,(int)(o->value()+64));
}
void AphaserGui::cb_aphaser_fb(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_fb_i(o,v);
}

void AphaserGui::cb_aphaser_mismatch_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(123);
 return;
} 
rkr->efx_APhaser->changepar(9,(int)o->value());
}
void AphaserGui::cb_aphaser_mismatch(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_mismatch_i(o,v);
}

void AphaserGui::cb_aphaser_stdf_i(SliderW* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(124);
 return;
} 
rkr->efx_APhaser->changepar(5,(int)o->value());
}
void AphaserGui::cb_aphaser_stdf(SliderW* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_stdf_i(o,v);
}

void AphaserGui::cb_aphaser_stages_i(Fl_Counter* o, void*) {
  rkr->efx_APhaser->changepar(8,(int)o->value());
}
void AphaserGui::cb_aphaser_stages(Fl_Counter* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_stages_i(o,v);
}

void AphaserGui::cb_aphaser_subs_i(Fl_Check_Button* o, void*) {
  rkr->efx_APhaser->changepar(10,(int)o->value());
}
void AphaserGui::cb_aphaser_subs(Fl_Check_Button* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_subs_i(o,v);
}

void AphaserGui::cb_aphaser_hyper_i(Fl_Check_Button* o, void*) {
  rkr->efx_APhaser->changepar(12,(int)o->value());
}
void AphaserGui::cb_aphaser_hyper(Fl_Check_Button* o, void* v) {
  ((AphaserGui*)(o->parent()))->cb_aphaser_hyper_i(o,v);
}
AphaserGui::AphaserGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ aphaser_activar = new Fl_Light_Button(5, 4, 34, 18, "On");
  aphaser_activar->shortcut(0x37);
  aphaser_activar->color((Fl_Color)62);
  aphaser_activar->selection_color((Fl_Color)1);
  aphaser_activar->labelsize(10);
  aphaser_activar->callback((Fl_Callback*)cb_aphaser_activar, (void*)(2));
  aphaser_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  aphaser_activar->when(FL_WHEN_CHANGED);
} // Fl_Light_Button* aphaser_activar
{ aphaser_preset = new Fl_Choice(77, 4, 76, 18, "Preset");
  aphaser_preset->down_box(FL_BORDER_BOX);
  aphaser_preset->selection_color(FL_FOREGROUND_COLOR);
  aphaser_preset->labelsize(10);
  aphaser_preset->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_preset->textsize(10);
  aphaser_preset->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_preset->callback((Fl_Callback*)cb_aphaser_preset, (void*)(12018));
  aphaser_preset->when(FL_WHEN_RELEASE_ALWAYS);
  aphaser_preset->menu(menu_aphaser_preset);
} // Fl_Choice* aphaser_preset
{ aphaser_WD = new SliderW(53, 25, 100, 10, "Dry/Wet");
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
} // SliderW* aphaser_WD
{ aphaser_distort = new SliderW(53, 115, 100, 10, "Distort");
  aphaser_distort->type(5);
  aphaser_distort->box(FL_FLAT_BOX);
  aphaser_distort->color((Fl_Color)178);
  aphaser_distort->selection_color((Fl_Color)62);
  aphaser_distort->labeltype(FL_NORMAL_LABEL);
  aphaser_distort->labelfont(0);
  aphaser_distort->labelsize(10);
  aphaser_distort->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_distort->maximum(100);
  aphaser_distort->step(1);
  aphaser_distort->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_distort->callback((Fl_Callback*)cb_aphaser_distort);
  aphaser_distort->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_distort->when(FL_WHEN_CHANGED);
} // SliderW* aphaser_distort
{ aphaser_freq = new SliderW(53, 56, 100, 10, "Tempo");
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
} // SliderW* aphaser_freq
{ aphaser_rnd = new SliderW(53, 127, 100, 10, "Random");
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
} // SliderW* aphaser_rnd
{ Fl_Choice* o = aphaser_lfotype = new Fl_Choice(78, 37, 72, 16, "LFO Type");
  aphaser_lfotype->down_box(FL_BORDER_BOX);
  aphaser_lfotype->selection_color(FL_FOREGROUND_COLOR);
  aphaser_lfotype->labelsize(10);
  aphaser_lfotype->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_lfotype->textsize(10);
  aphaser_lfotype->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_lfotype->callback((Fl_Callback*)cb_aphaser_lfotype);
  o->menu(m_lfo_menu->get_lfo_type());
} // Fl_Choice* aphaser_lfotype
{ aphaser_width = new SliderW(53, 79, 100, 10, "Width");
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
} // SliderW* aphaser_width
{ aphaser_phase = new SliderW(53, 67, 100, 10, "P. Depth");
  aphaser_phase->tooltip("Phase Depth");
  aphaser_phase->type(5);
  aphaser_phase->box(FL_FLAT_BOX);
  aphaser_phase->color((Fl_Color)178);
  aphaser_phase->selection_color((Fl_Color)62);
  aphaser_phase->labeltype(FL_NORMAL_LABEL);
  aphaser_phase->labelfont(0);
  aphaser_phase->labelsize(10);
  aphaser_phase->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_phase->maximum(127);
  aphaser_phase->step(1);
  aphaser_phase->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_phase->callback((Fl_Callback*)cb_aphaser_phase);
  aphaser_phase->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_phase->when(FL_WHEN_CHANGED);
} // SliderW* aphaser_phase
{ aphaser_fb = new SliderW(53, 91, 100, 10, "Feedback");
  aphaser_fb->type(5);
  aphaser_fb->box(FL_FLAT_BOX);
  aphaser_fb->color((Fl_Color)178);
  aphaser_fb->selection_color((Fl_Color)62);
  aphaser_fb->labeltype(FL_NORMAL_LABEL);
  aphaser_fb->labelfont(0);
  aphaser_fb->labelsize(8);
  aphaser_fb->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_fb->minimum(-64);
  aphaser_fb->maximum(64);
  aphaser_fb->step(1);
  aphaser_fb->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_fb->callback((Fl_Callback*)cb_aphaser_fb);
  aphaser_fb->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_fb->when(FL_WHEN_CHANGED);
} // SliderW* aphaser_fb
{ aphaser_mismatch = new SliderW(53, 103, 100, 10, "Mismatch");
  aphaser_mismatch->type(5);
  aphaser_mismatch->box(FL_FLAT_BOX);
  aphaser_mismatch->color((Fl_Color)178);
  aphaser_mismatch->selection_color((Fl_Color)62);
  aphaser_mismatch->labeltype(FL_NORMAL_LABEL);
  aphaser_mismatch->labelfont(0);
  aphaser_mismatch->labelsize(8);
  aphaser_mismatch->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_mismatch->maximum(100);
  aphaser_mismatch->step(1);
  aphaser_mismatch->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_mismatch->callback((Fl_Callback*)cb_aphaser_mismatch);
  aphaser_mismatch->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_mismatch->when(FL_WHEN_CHANGED);
} // SliderW* aphaser_mismatch
{ aphaser_stdf = new SliderW(53, 138, 100, 10, "Stereo Df.");
  aphaser_stdf->tooltip("LFO L/R Delay");
  aphaser_stdf->type(5);
  aphaser_stdf->box(FL_FLAT_BOX);
  aphaser_stdf->color((Fl_Color)178);
  aphaser_stdf->selection_color((Fl_Color)62);
  aphaser_stdf->labeltype(FL_NORMAL_LABEL);
  aphaser_stdf->labelfont(0);
  aphaser_stdf->labelsize(8);
  aphaser_stdf->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_stdf->maximum(127);
  aphaser_stdf->step(1);
  aphaser_stdf->textcolor(FL_BACKGROUND2_COLOR);
  aphaser_stdf->callback((Fl_Callback*)cb_aphaser_stdf);
  aphaser_stdf->align(Fl_Align(FL_ALIGN_LEFT));
  aphaser_stdf->when(FL_WHEN_CHANGED);
} // SliderW* aphaser_stdf
{ aphaser_stages = new Fl_Counter(80, 151, 52, 12, "Stages");
  aphaser_stages->type(1);
  aphaser_stages->box(FL_THIN_UP_BOX);
  aphaser_stages->labelsize(10);
  aphaser_stages->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_stages->minimum(1);
  aphaser_stages->maximum(12);
  aphaser_stages->step(1);
  aphaser_stages->value(1);
  aphaser_stages->textsize(9);
  aphaser_stages->callback((Fl_Callback*)cb_aphaser_stages);
  aphaser_stages->align(Fl_Align(FL_ALIGN_LEFT));
} // Fl_Counter* aphaser_stages
{ aphaser_subs = new Fl_Check_Button(10, 165, 64, 15, "Subtract");
  aphaser_subs->down_box(FL_BORDER_BOX);
  aphaser_subs->labelsize(10);
  aphaser_subs->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_subs->callback((Fl_Callback*)cb_aphaser_subs, (void*)(2));
} // Fl_Check_Button* aphaser_subs
{ aphaser_hyper = new Fl_Check_Button(75, 165, 53, 15, "Hyper");
  aphaser_hyper->down_box(FL_BORDER_BOX);
  aphaser_hyper->labelsize(10);
  aphaser_hyper->labelcolor(FL_BACKGROUND2_COLOR);
  aphaser_hyper->callback((Fl_Callback*)cb_aphaser_hyper, (void*)(2));
} // Fl_Check_Button* aphaser_hyper
position(X, Y);
end();
}
