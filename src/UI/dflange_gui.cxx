// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "dflange_gui.h"

void DflangeGui::cb_dflange_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116);
 o->value(rkr->DFlange_Bypass);
 return;
}
rkr->DFlange_Bypass=(int)o->value();
if((int) o->value()==0)
rkr->efx_DFlange->cleanup();
rgui->findpos(20,(int)o->value(),o);
}
void DflangeGui::cb_dflange_activar(RKR_Light_Button* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_activar_i(o,v);
}

void DflangeGui::cb_dflange_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12020))rkr->efx_DFlange->setpreset((int)o->value());
dflange_WD->value(Dry_Wet(rkr->efx_DFlange->getpar(DFlange_DryWet)));
dflange_pan->value(rkr->efx_DFlange->getpar(DFlange_Pan));
dflange_freq->value(rkr->efx_DFlange->getpar(DFlange_LFO_Tempo));
dflange_rnd->value(rkr->efx_DFlange->getpar(DFlange_LFO_Random));
dflange_lfotype->value(rkr->efx_DFlange->getpar(DFlange_LFO_Type));
dflange_stdf->value(rkr->efx_DFlange->getpar(DFlange_LFO_Stereo));
dflange_width->value(rkr->efx_DFlange->getpar(DFlange_Width));
dflange_depth->value(rkr->efx_DFlange->getpar(DFlange_Depth));
dflange_fb->value(rkr->efx_DFlange->getpar(DFlange_Feedback));
dflange_LR->value(rkr->efx_DFlange->getpar(DFlange_LR_Cross));
dflange_subs->value(rkr->efx_DFlange->getpar(DFlange_Subtract));
dflange_tz->value(rkr->efx_DFlange->getpar(DFlange_Zero));
dflange_offset->value(rkr->efx_DFlange->getpar(DFlange_Offset));
dflange_lpf->value(rkr->efx_DFlange->getpar(DFlange_LPF));
dflange_intense->value(rkr->efx_DFlange->getpar(DFlange_Intense));
}
void DflangeGui::cb_dflange_preset(RKR_Choice* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_preset_i(o,v);
}

Fl_Menu_Item DflangeGui::menu_dflange_preset[] = {
 {"Dual Flange 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Flange-Wah", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Feedback Flange", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Soft Flange", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Flanger", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Deep Chorus", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Bright Chorus", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Dual Flange 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void DflangeGui::cb_dflange_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_DryWet);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_DryWet,Dry_Wet((int)(o->value())));
}
void DflangeGui::cb_dflange_WD(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_WD_i(o,v);
}

void DflangeGui::cb_dflange_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_Pan);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_Pan,(int)o->value());
}
void DflangeGui::cb_dflange_pan(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_pan_i(o,v);
}

void DflangeGui::cb_dflange_LR_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_LR_Cross);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_LR_Cross,(int)o->value());
}
void DflangeGui::cb_dflange_LR(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_LR_i(o,v);
}

void DflangeGui::cb_dflange_depth_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_Depth);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_Depth,(int)o->value());
}
void DflangeGui::cb_dflange_depth(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_depth_i(o,v);
}

void DflangeGui::cb_dflange_width_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_Width);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_Width,(int)o->value());
}
void DflangeGui::cb_dflange_width(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_width_i(o,v);
}

void DflangeGui::cb_dflange_offset_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_Offset);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_Offset,(int)o->value());
}
void DflangeGui::cb_dflange_offset(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_offset_i(o,v);
}

void DflangeGui::cb_dflange_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_Feedback);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_Feedback,(int)o->value());
}
void DflangeGui::cb_dflange_fb(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_fb_i(o,v);
}

void DflangeGui::cb_dflange_lpf_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_LPF);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_LPF,(int)o->value());
}
void DflangeGui::cb_dflange_lpf(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_lpf_i(o,v);
}

void DflangeGui::cb_dflange_subs_i(RKR_Check_Button* o, void*) {
  rkr->efx_DFlange->changepar(DFlange_Subtract,(int)o->value());
}
void DflangeGui::cb_dflange_subs(RKR_Check_Button* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_subs_i(o,v);
}

void DflangeGui::cb_dflange_tz_i(RKR_Check_Button* o, void*) {
  rkr->efx_DFlange->changepar(DFlange_Zero,(int)o->value());
}
void DflangeGui::cb_dflange_tz(RKR_Check_Button* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_tz_i(o,v);
}

void DflangeGui::cb_dflange_intense_i(RKR_Check_Button* o, void*) {
  rkr->efx_DFlange->changepar(DFlange_Intense,(int)o->value());
}
void DflangeGui::cb_dflange_intense(RKR_Check_Button* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_intense_i(o,v);
}

void DflangeGui::cb_dflange_freq_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_LFO_Tempo);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_LFO_Tempo,(int)o->value());
}
void DflangeGui::cb_dflange_freq(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_freq_i(o,v);
}

void DflangeGui::cb_dflange_stdf_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_LFO_Stereo);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_LFO_Stereo,(int)o->value());
}
void DflangeGui::cb_dflange_stdf(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_stdf_i(o,v);
}

void DflangeGui::cb_dflange_lfotype_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_LFO_Type);
 return;
} 

rkr->efx_DFlange->changepar(DFlange_LFO_Type,(int)o->value());
}
void DflangeGui::cb_dflange_lfotype(RKR_Choice* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_lfotype_i(o,v);
}

void DflangeGui::cb_dflange_rnd_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_DFlange_LFO_Random);
 return;
} 
rkr->efx_DFlange->changepar(DFlange_LFO_Random,(int)o->value());
}
void DflangeGui::cb_dflange_rnd(RKR_Slider* o, void* v) {
  ((DflangeGui*)(o->parent()))->cb_dflange_rnd_i(o,v);
}
DflangeGui::DflangeGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ dflange_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  dflange_activar->box(FL_UP_BOX);
  dflange_activar->shortcut(0x38);
  dflange_activar->color((Fl_Color)62);
  dflange_activar->selection_color((Fl_Color)1);
  dflange_activar->labeltype(FL_NORMAL_LABEL);
  dflange_activar->labelfont(0);
  dflange_activar->labelsize(10);
  dflange_activar->labelcolor(FL_FOREGROUND_COLOR);
  dflange_activar->callback((Fl_Callback*)cb_dflange_activar, (void*)(2));
  dflange_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  dflange_activar->when(FL_WHEN_CHANGED);
} // RKR_Light_Button* dflange_activar
{ dflange_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  dflange_preset->box(FL_FLAT_BOX);
  dflange_preset->down_box(FL_BORDER_BOX);
  dflange_preset->color(FL_BACKGROUND_COLOR);
  dflange_preset->selection_color(FL_FOREGROUND_COLOR);
  dflange_preset->labeltype(FL_NORMAL_LABEL);
  dflange_preset->labelfont(0);
  dflange_preset->labelsize(10);
  dflange_preset->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_preset->textsize(10);
  dflange_preset->textcolor(FL_BACKGROUND2_COLOR);
  dflange_preset->callback((Fl_Callback*)cb_dflange_preset, (void*)(12020));
  dflange_preset->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_preset->when(FL_WHEN_RELEASE_ALWAYS);
  dflange_preset->menu(menu_dflange_preset);
} // RKR_Choice* dflange_preset
{ dflange_WD = new RKR_Slider(56, 24, 100, 10, "Dry/Wet");
  dflange_WD->type(5);
  dflange_WD->box(FL_FLAT_BOX);
  dflange_WD->color((Fl_Color)178);
  dflange_WD->selection_color((Fl_Color)62);
  dflange_WD->labeltype(FL_NORMAL_LABEL);
  dflange_WD->labelfont(0);
  dflange_WD->labelsize(10);
  dflange_WD->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_WD->maximum(127);
  dflange_WD->step(1);
  dflange_WD->textcolor(FL_BACKGROUND2_COLOR);
  dflange_WD->callback((Fl_Callback*)cb_dflange_WD);
  dflange_WD->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_WD
{ dflange_pan = new RKR_Slider(56, 35, 100, 10, "Pan");
  dflange_pan->type(5);
  dflange_pan->box(FL_FLAT_BOX);
  dflange_pan->color((Fl_Color)178);
  dflange_pan->selection_color((Fl_Color)62);
  dflange_pan->labeltype(FL_NORMAL_LABEL);
  dflange_pan->labelfont(0);
  dflange_pan->labelsize(10);
  dflange_pan->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_pan->minimum(-64);
  dflange_pan->maximum(64);
  dflange_pan->step(1);
  dflange_pan->textcolor(FL_BACKGROUND2_COLOR);
  dflange_pan->callback((Fl_Callback*)cb_dflange_pan);
  dflange_pan->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_pan
{ dflange_LR = new RKR_Slider(56, 46, 100, 10, "L/R Cross");
  dflange_LR->type(5);
  dflange_LR->box(FL_FLAT_BOX);
  dflange_LR->color((Fl_Color)178);
  dflange_LR->selection_color((Fl_Color)62);
  dflange_LR->labeltype(FL_NORMAL_LABEL);
  dflange_LR->labelfont(0);
  dflange_LR->labelsize(10);
  dflange_LR->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_LR->maximum(127);
  dflange_LR->step(1);
  dflange_LR->textcolor(FL_BACKGROUND2_COLOR);
  dflange_LR->callback((Fl_Callback*)cb_dflange_LR);
  dflange_LR->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_LR->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_LR
{ dflange_depth = new RKR_Slider(56, 57, 100, 10, "Depth");
  dflange_depth->type(5);
  dflange_depth->box(FL_FLAT_BOX);
  dflange_depth->color((Fl_Color)178);
  dflange_depth->selection_color((Fl_Color)62);
  dflange_depth->labeltype(FL_NORMAL_LABEL);
  dflange_depth->labelfont(0);
  dflange_depth->labelsize(10);
  dflange_depth->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_depth->minimum(20);
  dflange_depth->maximum(2500);
  dflange_depth->step(1);
  dflange_depth->value(20);
  dflange_depth->textcolor(FL_BACKGROUND2_COLOR);
  dflange_depth->callback((Fl_Callback*)cb_dflange_depth);
  dflange_depth->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_depth->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_depth
{ dflange_width = new RKR_Slider(56, 69, 100, 10, "Width");
  dflange_width->type(5);
  dflange_width->box(FL_FLAT_BOX);
  dflange_width->color((Fl_Color)178);
  dflange_width->selection_color((Fl_Color)62);
  dflange_width->labeltype(FL_NORMAL_LABEL);
  dflange_width->labelfont(0);
  dflange_width->labelsize(10);
  dflange_width->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_width->maximum(6000);
  dflange_width->step(1);
  dflange_width->textcolor(FL_BACKGROUND2_COLOR);
  dflange_width->callback((Fl_Callback*)cb_dflange_width);
  dflange_width->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_width->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_width
{ dflange_offset = new RKR_Slider(56, 80, 100, 10, "Offset");
  dflange_offset->type(5);
  dflange_offset->box(FL_FLAT_BOX);
  dflange_offset->color((Fl_Color)178);
  dflange_offset->selection_color((Fl_Color)62);
  dflange_offset->labeltype(FL_NORMAL_LABEL);
  dflange_offset->labelfont(0);
  dflange_offset->labelsize(10);
  dflange_offset->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_offset->maximum(100);
  dflange_offset->step(1);
  dflange_offset->textcolor(FL_BACKGROUND2_COLOR);
  dflange_offset->callback((Fl_Callback*)cb_dflange_offset);
  dflange_offset->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_offset->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_offset
{ dflange_fb = new RKR_Slider(56, 91, 100, 10, "Feedback");
  dflange_fb->type(5);
  dflange_fb->box(FL_FLAT_BOX);
  dflange_fb->color((Fl_Color)178);
  dflange_fb->selection_color((Fl_Color)62);
  dflange_fb->labeltype(FL_NORMAL_LABEL);
  dflange_fb->labelfont(0);
  dflange_fb->labelsize(10);
  dflange_fb->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_fb->minimum(-64);
  dflange_fb->maximum(64);
  dflange_fb->step(1);
  dflange_fb->textcolor(FL_BACKGROUND2_COLOR);
  dflange_fb->callback((Fl_Callback*)cb_dflange_fb);
  dflange_fb->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_fb
{ dflange_lpf = new RKR_Slider(56, 102, 100, 10, "LPF");
  dflange_lpf->type(5);
  dflange_lpf->box(FL_FLAT_BOX);
  dflange_lpf->color((Fl_Color)178);
  dflange_lpf->selection_color((Fl_Color)62);
  dflange_lpf->labeltype(FL_NORMAL_LABEL);
  dflange_lpf->labelfont(0);
  dflange_lpf->labelsize(10);
  dflange_lpf->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_lpf->minimum(20);
  dflange_lpf->maximum(20000);
  dflange_lpf->step(1);
  dflange_lpf->value(20);
  dflange_lpf->textcolor(FL_BACKGROUND2_COLOR);
  dflange_lpf->callback((Fl_Callback*)cb_dflange_lpf);
  dflange_lpf->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_lpf->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_lpf
{ dflange_subs = new RKR_Check_Button(5, 113, 46, 15, "Subtr");
  dflange_subs->box(FL_NO_BOX);
  dflange_subs->down_box(FL_BORDER_BOX);
  dflange_subs->color(FL_BACKGROUND_COLOR);
  dflange_subs->selection_color(FL_FOREGROUND_COLOR);
  dflange_subs->labeltype(FL_NORMAL_LABEL);
  dflange_subs->labelfont(0);
  dflange_subs->labelsize(10);
  dflange_subs->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_subs->callback((Fl_Callback*)cb_dflange_subs, (void*)(2));
  dflange_subs->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  dflange_subs->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* dflange_subs
{ dflange_tz = new RKR_Check_Button(48, 113, 55, 15, "Th. zero");
  dflange_tz->box(FL_NO_BOX);
  dflange_tz->down_box(FL_BORDER_BOX);
  dflange_tz->color(FL_BACKGROUND_COLOR);
  dflange_tz->selection_color(FL_FOREGROUND_COLOR);
  dflange_tz->labeltype(FL_NORMAL_LABEL);
  dflange_tz->labelfont(0);
  dflange_tz->labelsize(10);
  dflange_tz->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_tz->callback((Fl_Callback*)cb_dflange_tz, (void*)(2));
  dflange_tz->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  dflange_tz->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* dflange_tz
{ dflange_intense = new RKR_Check_Button(102, 113, 52, 15, "Intense");
  dflange_intense->box(FL_NO_BOX);
  dflange_intense->down_box(FL_BORDER_BOX);
  dflange_intense->color(FL_BACKGROUND_COLOR);
  dflange_intense->selection_color(FL_FOREGROUND_COLOR);
  dflange_intense->labeltype(FL_NORMAL_LABEL);
  dflange_intense->labelfont(0);
  dflange_intense->labelsize(10);
  dflange_intense->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_intense->callback((Fl_Callback*)cb_dflange_intense, (void*)(2));
  dflange_intense->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  dflange_intense->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* dflange_intense
{ dflange_freq = new RKR_Slider(56, 128, 100, 10, "Tempo");
  dflange_freq->type(5);
  dflange_freq->box(FL_FLAT_BOX);
  dflange_freq->color((Fl_Color)178);
  dflange_freq->selection_color((Fl_Color)62);
  dflange_freq->labeltype(FL_NORMAL_LABEL);
  dflange_freq->labelfont(0);
  dflange_freq->labelsize(10);
  dflange_freq->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_freq->minimum(1);
  dflange_freq->maximum(600);
  dflange_freq->step(1);
  dflange_freq->value(100);
  dflange_freq->textcolor(FL_BACKGROUND2_COLOR);
  dflange_freq->callback((Fl_Callback*)cb_dflange_freq);
  dflange_freq->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_freq->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_freq
{ dflange_stdf = new RKR_Slider(56, 140, 100, 10, "Stereo Df");
  dflange_stdf->type(5);
  dflange_stdf->box(FL_FLAT_BOX);
  dflange_stdf->color((Fl_Color)178);
  dflange_stdf->selection_color((Fl_Color)62);
  dflange_stdf->labeltype(FL_NORMAL_LABEL);
  dflange_stdf->labelfont(0);
  dflange_stdf->labelsize(10);
  dflange_stdf->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_stdf->maximum(127);
  dflange_stdf->step(1);
  dflange_stdf->textcolor(FL_BACKGROUND2_COLOR);
  dflange_stdf->callback((Fl_Callback*)cb_dflange_stdf);
  dflange_stdf->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_stdf->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_stdf
{ RKR_Choice* o = dflange_lfotype = new RKR_Choice(78, 153, 72, 15, "LFO Type");
  dflange_lfotype->box(FL_FLAT_BOX);
  dflange_lfotype->down_box(FL_BORDER_BOX);
  dflange_lfotype->color(FL_BACKGROUND_COLOR);
  dflange_lfotype->selection_color(FL_FOREGROUND_COLOR);
  dflange_lfotype->labeltype(FL_NORMAL_LABEL);
  dflange_lfotype->labelfont(0);
  dflange_lfotype->labelsize(10);
  dflange_lfotype->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_lfotype->textsize(10);
  dflange_lfotype->textcolor(FL_BACKGROUND2_COLOR);
  dflange_lfotype->callback((Fl_Callback*)cb_dflange_lfotype);
  dflange_lfotype->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_lfotype->when(FL_WHEN_RELEASE);
  o->menu(m_lfo_menu->get_lfo_type());
} // RKR_Choice* dflange_lfotype
{ dflange_rnd = new RKR_Slider(56, 170, 100, 10, "Random");
  dflange_rnd->type(5);
  dflange_rnd->box(FL_FLAT_BOX);
  dflange_rnd->color((Fl_Color)178);
  dflange_rnd->selection_color((Fl_Color)62);
  dflange_rnd->labeltype(FL_NORMAL_LABEL);
  dflange_rnd->labelfont(0);
  dflange_rnd->labelsize(10);
  dflange_rnd->labelcolor(FL_BACKGROUND2_COLOR);
  dflange_rnd->maximum(127);
  dflange_rnd->step(1);
  dflange_rnd->textcolor(FL_BACKGROUND2_COLOR);
  dflange_rnd->callback((Fl_Callback*)cb_dflange_rnd);
  dflange_rnd->align(Fl_Align(FL_ALIGN_LEFT));
  dflange_rnd->when(FL_WHEN_CHANGED);
} // RKR_Slider* dflange_rnd
position(X, Y);
end();
}
