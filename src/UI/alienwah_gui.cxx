// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "alienwah_gui.h"

void AlienwahGui::cb_Alienwah_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116);
 o->value(rkr->Alienwah_Bypass);
 return;
}
rkr->Alienwah_Bypass=(int)o->value();
if((int) o->value()==0)
rkr->efx_Alienwah->cleanup();
rgui->findpos(11,(int)o->value(),o);
}
void AlienwahGui::cb_Alienwah_activar(RKR_Light_Button* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_activar_i(o,v);
}

void AlienwahGui::cb_Alienwah_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12011))rkr->efx_Alienwah->setpreset((int) o->value());
Alienwah_WD->value(Dry_Wet(rkr->efx_Alienwah->getpar(Alien_DryWet)));
Alienwah_pan->value(rkr->efx_Alienwah->getpar(Alien_Pan)-64);
Alienwah_freq->value(rkr->efx_Alienwah->getpar(Alien_LFO_Tempo));
Alienwah_rnd->value(rkr->efx_Alienwah->getpar(Alien_LFO_Random));
Alienwah_lfotype->value(rkr->efx_Alienwah->getpar(Alien_LFO_Type));
Alienwah_stdf->value(rkr->efx_Alienwah->getpar(Alien_LFO_Stereo));
Alienwah_dpth->value(rkr->efx_Alienwah->getpar(Alien_Depth));
Alienwah_fb->value(rkr->efx_Alienwah->getpar(Alien_Feedback));
Alienwah_delay->value(rkr->efx_Alienwah->getpar(Alien_Delay));
Alienwah_LR->value(rkr->efx_Alienwah->getpar(Alien_LR_Cross));
Alienwah_phase->value(rkr->efx_Alienwah->getpar(Alien_Phase));
}
void AlienwahGui::cb_Alienwah_preset(RKR_Choice* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_preset_i(o,v);
}

Fl_Menu_Item AlienwahGui::menu_Alienwah_preset[] = {
 {"AlienWah1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"AlienWah2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"AlienWah3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"AlienWah4", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void AlienwahGui::cb_Alienwah_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(55);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_DryWet,Dry_Wet((int)(o->value())));
}
void AlienwahGui::cb_Alienwah_WD(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_WD_i(o,v);
}

void AlienwahGui::cb_Alienwah_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(61);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_Pan,(int)(o->value()+64));
}
void AlienwahGui::cb_Alienwah_pan(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_pan_i(o,v);
}

void AlienwahGui::cb_Alienwah_freq_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(76);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_LFO_Tempo,(int)o->value());
}
void AlienwahGui::cb_Alienwah_freq(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_freq_i(o,v);
}

void AlienwahGui::cb_Alienwah_rnd_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(109);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_LFO_Random,(int)o->value());
}
void AlienwahGui::cb_Alienwah_rnd(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_rnd_i(o,v);
}

void AlienwahGui::cb_Alienwah_lfotype_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Alien_LFO_Type);
 return;
} 

rkr->efx_Alienwah->changepar(Alien_LFO_Type,(int)o->value());
}
void AlienwahGui::cb_Alienwah_lfotype(RKR_Choice* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_lfotype_i(o,v);
}

void AlienwahGui::cb_Alienwah_phase_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(115);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_Phase,(int)o->value());
}
void AlienwahGui::cb_Alienwah_phase(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_phase_i(o,v);
}

void AlienwahGui::cb_Alienwah_stdf_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(103);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_LFO_Stereo,(int)o->value());
}
void AlienwahGui::cb_Alienwah_stdf(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_stdf_i(o,v);
}

void AlienwahGui::cb_Alienwah_dpth_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(20);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_Depth,(int)o->value());
}
void AlienwahGui::cb_Alienwah_dpth(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_dpth_i(o,v);
}

void AlienwahGui::cb_Alienwah_delay_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Alien_Delay);
 return;
} 
rkr->Alienwah_Bypass=0;
rkr->efx_Alienwah->changepar(Alien_Delay,(int)o->value());
if (Alienwah_activar->value()) rkr->Alienwah_Bypass=1;
}
void AlienwahGui::cb_Alienwah_delay(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_delay_i(o,v);
}

void AlienwahGui::cb_Alienwah_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(82);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_Feedback,(int)o->value());
}
void AlienwahGui::cb_Alienwah_fb(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_fb_i(o,v);
}

void AlienwahGui::cb_Alienwah_LR_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(96);
 return;
} 
rkr->efx_Alienwah->changepar(Alien_LR_Cross,(int)(o->value()));
}
void AlienwahGui::cb_Alienwah_LR(RKR_Slider* o, void* v) {
  ((AlienwahGui*)(o->parent()))->cb_Alienwah_LR_i(o,v);
}
AlienwahGui::AlienwahGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ Alienwah_activar = new RKR_Light_Button(4, 4, 34, 18, "On");
  Alienwah_activar->box(FL_UP_BOX);
  Alienwah_activar->shortcut(0x36);
  Alienwah_activar->color((Fl_Color)62);
  Alienwah_activar->selection_color((Fl_Color)1);
  Alienwah_activar->labeltype(FL_NORMAL_LABEL);
  Alienwah_activar->labelfont(0);
  Alienwah_activar->labelsize(10);
  Alienwah_activar->labelcolor(FL_FOREGROUND_COLOR);
  Alienwah_activar->callback((Fl_Callback*)cb_Alienwah_activar, (void*)(2));
  Alienwah_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  Alienwah_activar->when(FL_WHEN_CHANGED);
} // RKR_Light_Button* Alienwah_activar
{ Alienwah_preset = new RKR_Choice(76, 4, 76, 18, "Preset");
  Alienwah_preset->box(FL_FLAT_BOX);
  Alienwah_preset->down_box(FL_BORDER_BOX);
  Alienwah_preset->color(FL_BACKGROUND_COLOR);
  Alienwah_preset->selection_color(FL_FOREGROUND_COLOR);
  Alienwah_preset->labeltype(FL_NORMAL_LABEL);
  Alienwah_preset->labelfont(0);
  Alienwah_preset->labelsize(10);
  Alienwah_preset->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_preset->textsize(10);
  Alienwah_preset->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_preset->callback((Fl_Callback*)cb_Alienwah_preset, (void*)(12011));
  Alienwah_preset->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_preset->when(FL_WHEN_RELEASE_ALWAYS);
  Alienwah_preset->menu(menu_Alienwah_preset);
} // RKR_Choice* Alienwah_preset
{ Alienwah_WD = new RKR_Slider(56, 29, 100, 10, "Dry/Wet");
  Alienwah_WD->type(5);
  Alienwah_WD->box(FL_FLAT_BOX);
  Alienwah_WD->color((Fl_Color)178);
  Alienwah_WD->selection_color((Fl_Color)62);
  Alienwah_WD->labeltype(FL_NORMAL_LABEL);
  Alienwah_WD->labelfont(0);
  Alienwah_WD->labelsize(10);
  Alienwah_WD->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_WD->maximum(127);
  Alienwah_WD->step(1);
  Alienwah_WD->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_WD->callback((Fl_Callback*)cb_Alienwah_WD);
  Alienwah_WD->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_WD
{ Alienwah_pan = new RKR_Slider(56, 41, 100, 10, "Pan");
  Alienwah_pan->type(5);
  Alienwah_pan->box(FL_FLAT_BOX);
  Alienwah_pan->color((Fl_Color)178);
  Alienwah_pan->selection_color((Fl_Color)62);
  Alienwah_pan->labeltype(FL_NORMAL_LABEL);
  Alienwah_pan->labelfont(0);
  Alienwah_pan->labelsize(10);
  Alienwah_pan->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_pan->minimum(-64);
  Alienwah_pan->maximum(63);
  Alienwah_pan->step(1);
  Alienwah_pan->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_pan->callback((Fl_Callback*)cb_Alienwah_pan);
  Alienwah_pan->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_pan
{ Alienwah_freq = new RKR_Slider(56, 53, 100, 10, "Tempo");
  Alienwah_freq->type(5);
  Alienwah_freq->box(FL_FLAT_BOX);
  Alienwah_freq->color((Fl_Color)178);
  Alienwah_freq->selection_color((Fl_Color)62);
  Alienwah_freq->labeltype(FL_NORMAL_LABEL);
  Alienwah_freq->labelfont(0);
  Alienwah_freq->labelsize(10);
  Alienwah_freq->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_freq->minimum(1);
  Alienwah_freq->maximum(600);
  Alienwah_freq->step(1);
  Alienwah_freq->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_freq->callback((Fl_Callback*)cb_Alienwah_freq);
  Alienwah_freq->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_freq->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_freq
{ Alienwah_rnd = new RKR_Slider(56, 65, 100, 10, "Random");
  Alienwah_rnd->type(5);
  Alienwah_rnd->box(FL_FLAT_BOX);
  Alienwah_rnd->color((Fl_Color)178);
  Alienwah_rnd->selection_color((Fl_Color)62);
  Alienwah_rnd->labeltype(FL_NORMAL_LABEL);
  Alienwah_rnd->labelfont(0);
  Alienwah_rnd->labelsize(10);
  Alienwah_rnd->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_rnd->maximum(127);
  Alienwah_rnd->step(1);
  Alienwah_rnd->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_rnd->callback((Fl_Callback*)cb_Alienwah_rnd);
  Alienwah_rnd->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_rnd->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_rnd
{ RKR_Choice* o = Alienwah_lfotype = new RKR_Choice(78, 79, 72, 16, "LFO Type");
  Alienwah_lfotype->box(FL_FLAT_BOX);
  Alienwah_lfotype->down_box(FL_BORDER_BOX);
  Alienwah_lfotype->color(FL_BACKGROUND_COLOR);
  Alienwah_lfotype->selection_color(FL_FOREGROUND_COLOR);
  Alienwah_lfotype->labeltype(FL_NORMAL_LABEL);
  Alienwah_lfotype->labelfont(0);
  Alienwah_lfotype->labelsize(10);
  Alienwah_lfotype->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_lfotype->textsize(10);
  Alienwah_lfotype->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_lfotype->callback((Fl_Callback*)cb_Alienwah_lfotype);
  Alienwah_lfotype->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_lfotype->when(FL_WHEN_RELEASE);
  o->menu(m_lfo_menu->get_lfo_type());
} // RKR_Choice* Alienwah_lfotype
{ Alienwah_phase = new RKR_Slider(56, 105, 100, 10, "Phase");
  Alienwah_phase->type(5);
  Alienwah_phase->box(FL_FLAT_BOX);
  Alienwah_phase->color((Fl_Color)178);
  Alienwah_phase->selection_color((Fl_Color)62);
  Alienwah_phase->labeltype(FL_NORMAL_LABEL);
  Alienwah_phase->labelfont(0);
  Alienwah_phase->labelsize(10);
  Alienwah_phase->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_phase->maximum(127);
  Alienwah_phase->step(1);
  Alienwah_phase->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_phase->callback((Fl_Callback*)cb_Alienwah_phase);
  Alienwah_phase->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_phase->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_phase
{ Alienwah_stdf = new RKR_Slider(56, 117, 100, 10, "Stereo Df");
  Alienwah_stdf->type(5);
  Alienwah_stdf->box(FL_FLAT_BOX);
  Alienwah_stdf->color((Fl_Color)178);
  Alienwah_stdf->selection_color((Fl_Color)62);
  Alienwah_stdf->labeltype(FL_NORMAL_LABEL);
  Alienwah_stdf->labelfont(0);
  Alienwah_stdf->labelsize(10);
  Alienwah_stdf->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_stdf->maximum(127);
  Alienwah_stdf->step(1);
  Alienwah_stdf->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_stdf->callback((Fl_Callback*)cb_Alienwah_stdf);
  Alienwah_stdf->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_stdf->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_stdf
{ Alienwah_dpth = new RKR_Slider(56, 129, 100, 10, "Depth");
  Alienwah_dpth->type(5);
  Alienwah_dpth->box(FL_FLAT_BOX);
  Alienwah_dpth->color((Fl_Color)178);
  Alienwah_dpth->selection_color((Fl_Color)62);
  Alienwah_dpth->labeltype(FL_NORMAL_LABEL);
  Alienwah_dpth->labelfont(0);
  Alienwah_dpth->labelsize(10);
  Alienwah_dpth->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_dpth->maximum(127);
  Alienwah_dpth->step(1);
  Alienwah_dpth->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_dpth->callback((Fl_Callback*)cb_Alienwah_dpth);
  Alienwah_dpth->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_dpth->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_dpth
{ Alienwah_delay = new RKR_Slider(56, 141, 100, 10, "Delay");
  Alienwah_delay->type(5);
  Alienwah_delay->box(FL_FLAT_BOX);
  Alienwah_delay->color((Fl_Color)178);
  Alienwah_delay->selection_color((Fl_Color)62);
  Alienwah_delay->labeltype(FL_NORMAL_LABEL);
  Alienwah_delay->labelfont(0);
  Alienwah_delay->labelsize(10);
  Alienwah_delay->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_delay->maximum(100);
  Alienwah_delay->step(1);
  Alienwah_delay->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_delay->callback((Fl_Callback*)cb_Alienwah_delay);
  Alienwah_delay->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_delay->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_delay
{ Alienwah_fb = new RKR_Slider(56, 153, 100, 10, "Feedback");
  Alienwah_fb->type(5);
  Alienwah_fb->box(FL_FLAT_BOX);
  Alienwah_fb->color((Fl_Color)178);
  Alienwah_fb->selection_color((Fl_Color)62);
  Alienwah_fb->labeltype(FL_NORMAL_LABEL);
  Alienwah_fb->labelfont(0);
  Alienwah_fb->labelsize(10);
  Alienwah_fb->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_fb->maximum(127);
  Alienwah_fb->step(1);
  Alienwah_fb->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_fb->callback((Fl_Callback*)cb_Alienwah_fb);
  Alienwah_fb->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_fb
{ Alienwah_LR = new RKR_Slider(56, 166, 100, 10, "L/R Cross");
  Alienwah_LR->type(5);
  Alienwah_LR->box(FL_FLAT_BOX);
  Alienwah_LR->color((Fl_Color)178);
  Alienwah_LR->selection_color((Fl_Color)62);
  Alienwah_LR->labeltype(FL_NORMAL_LABEL);
  Alienwah_LR->labelfont(0);
  Alienwah_LR->labelsize(10);
  Alienwah_LR->labelcolor(FL_BACKGROUND2_COLOR);
  Alienwah_LR->maximum(127);
  Alienwah_LR->step(1);
  Alienwah_LR->textcolor(FL_BACKGROUND2_COLOR);
  Alienwah_LR->callback((Fl_Callback*)cb_Alienwah_LR);
  Alienwah_LR->align(Fl_Align(FL_ALIGN_LEFT));
  Alienwah_LR->when(FL_WHEN_CHANGED);
} // RKR_Slider* Alienwah_LR
position(X, Y);
end();
}
