// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "echotron_gui.h"

void EchotronGui::cb_echotron_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(116);
 o->value(rkr->Echotron_Bypass);
 return;
}
rkr->Echotron_Bypass=(int)o->value();
if((int) o->value()==0) rkr->efx_Echotron->cleanup();
rgui->findpos(41,(int)o->value(),o);
}
void EchotronGui::cb_echotron_activar(RKR_Light_Button* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_activar_i(o,v);
}

void EchotronGui::cb_echotron_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12041))rkr->efx_Echotron->setpreset((int) o->value());
echotron_pan->value(rkr->efx_Echotron->getpar(Echotron_Pan)-64);
echotron_WD->value(Dry_Wet(rkr->efx_Echotron->getpar(Echotron_DryWet)));
echotron_damp->value(rkr->efx_Echotron->getpar(Echotron_Damp));
echotron_fnum->value(rkr->efx_Echotron->getpar(Echotron_Set_File));
echotron_user->value(rkr->efx_Echotron->getpar(Echotron_User_File));
echotron_user->do_callback();
echotron_fb->value(rkr->efx_Echotron->getpar(Echotron_Feedback));
echotron_tempo->value(rkr->efx_Echotron->getpar(Echotron_Tempo));
echotron_lfotype->value(rkr->efx_Echotron->getpar(Echotron_LFO_Type));
echotron_width->value(rkr->efx_Echotron->getpar(Echotron_LFO_Width));
echotron_deep->value(rkr->efx_Echotron->getpar(Echotron_Depth)-64);
echotron_lrcross->value(rkr->efx_Echotron->getpar(Echotron_LR_Cross)-64);
echotron_stdf->value(rkr->efx_Echotron->getpar(Echotron_LFO_Stereo));
echotron_af->value(rkr->efx_Echotron->getpar(Echotron_Filters));
echotron_mf->value(rkr->efx_Echotron->getpar(Echotron_Mod_Filter));
echotron_md->value(rkr->efx_Echotron->getpar(Echotron_Mod_Delay));
echotron_length->value(rkr->efx_Echotron->getpar(Echotron_Taps));
echotron_length->maximum(rkr->efx_Echotron->File.fLength);
}
void EchotronGui::cb_echotron_preset(RKR_Choice* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_preset_i(o,v);
}

Fl_Menu_Item EchotronGui::menu_echotron_preset[] = {
 {"Summer", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Ambience", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Arranjer", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Suction", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"SuctionFlange", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Soft", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void EchotronGui::cb_echotron_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_DryWet);
 return;
}
rkr->efx_Echotron->changepar(Echotron_DryWet,Dry_Wet((int)(o->value())));
}
void EchotronGui::cb_echotron_WD(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_WD_i(o,v);
}

void EchotronGui::cb_echotron_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_Pan);
 return;
}
rkr->efx_Echotron->changepar(Echotron_Pan,(int)(o->value()+64));
}
void EchotronGui::cb_echotron_pan(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_pan_i(o,v);
}

void EchotronGui::cb_echotron_tempo_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_Tempo);
 return;
}
rkr->efx_Echotron->changepar(Echotron_Tempo,(int)o->value());
}
void EchotronGui::cb_echotron_tempo(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_tempo_i(o,v);
}

void EchotronGui::cb_echotron_damp_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_Damp);
 return;
}
rkr->efx_Echotron->changepar(Echotron_Damp,(int)o->value());
}
void EchotronGui::cb_echotron_damp(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_damp_i(o,v);
}

void EchotronGui::cb_echotron_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_Feedback);
 return;
}
rkr->efx_Echotron->changepar(Echotron_Feedback,(int)o->value());
}
void EchotronGui::cb_echotron_fb(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_fb_i(o,v);
}

void EchotronGui::cb_echotron_lrcross_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_LR_Cross);
 return;
}
rkr->efx_Echotron->changepar(Echotron_LR_Cross,(int)(o->value()+64));
}
void EchotronGui::cb_echotron_lrcross(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_lrcross_i(o,v);
}

void EchotronGui::cb_echotron_width_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_LFO_Width);
 return;
}
rkr->efx_Echotron->changepar(Echotron_LFO_Width,(int)o->value());
}
void EchotronGui::cb_echotron_width(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_width_i(o,v);
}

void EchotronGui::cb_echotron_deep_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_Depth);
 return;
}
rkr->efx_Echotron->changepar(Echotron_Depth,((int)o->value()+64));
}
void EchotronGui::cb_echotron_deep(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_deep_i(o,v);
}

void EchotronGui::cb_echotron_stdf_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_LFO_Stereo);
 return;
}
rkr->efx_Echotron->changepar(Echotron_LFO_Stereo,(int)o->value());
}
void EchotronGui::cb_echotron_stdf(RKR_Slider* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_stdf_i(o,v);
}

void EchotronGui::cb_echotron_af_i(RKR_Check_Button* o, void*) {
  rkr->efx_Echotron->changepar(Echotron_Filters,(int)o->value());
}
void EchotronGui::cb_echotron_af(RKR_Check_Button* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_af_i(o,v);
}

void EchotronGui::cb_echotron_lfotype_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_LFO_Type);
 return;
}

rkr->efx_Echotron->changepar(Echotron_LFO_Type,(int)o->value());
}
void EchotronGui::cb_echotron_lfotype(RKR_Choice* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_lfotype_i(o,v);
}

void EchotronGui::cb_echotron_mf_i(RKR_Check_Button* o, void*) {
  rkr->efx_Echotron->changepar(Echotron_Mod_Filter,(int)o->value());
}
void EchotronGui::cb_echotron_mf(RKR_Check_Button* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_mf_i(o,v);
}

void EchotronGui::cb_echotron_md_i(RKR_Check_Button* o, void*) {
  rkr->efx_Echotron->changepar(Echotron_Mod_Delay,(int)o->value());
}
void EchotronGui::cb_echotron_md(RKR_Check_Button* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_md_i(o,v);
}

void EchotronGui::cb_echotron_user_i(RKR_Check_Button* o, void*) {
  rkr->efx_Echotron->changepar(Echotron_User_File,(int)o->value());

if((int)o->value())B_ech->activate(); else B_ech->deactivate();
}
void EchotronGui::cb_echotron_user(RKR_Check_Button* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_user_i(o,v);
}

void EchotronGui::cb_B_ech_i(RKR_Button*, void*) {
  char *filename;
filename=fl_file_chooser("Load dly File:","(*.dly)",NULL,0);
if (filename==NULL) return;
filename=fl_filename_setext(filename,".dly");
strcpy(rkr->efx_Echotron->Filename,filename);

if(!rkr->efx_Echotron->setfile(USERFILE))
{
    fl_alert("Error loading %s file!\n", filename);
}

echotron_length->value(rkr->efx_Echotron->getpar(Echotron_Taps));
echotron_length->maximum(rkr->efx_Echotron->File.fLength);
}
void EchotronGui::cb_B_ech(RKR_Button* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_B_ech_i(o,v);
}

void EchotronGui::cb_echotron_length_i(RKR_Counter* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Echotron_Taps);
 return;
}
rkr->efx_Echotron->changepar(Echotron_Taps,(int)o->value());
}
void EchotronGui::cb_echotron_length(RKR_Counter* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_length_i(o,v);
}

void EchotronGui::cb_echotron_fnum_i(RKR_Choice* o, void*) {
  rkr->efx_Echotron->changepar(Echotron_Set_File,(int)o->value());
echotron_length->value(rkr->efx_Echotron->getpar(Echotron_Taps));
echotron_length->maximum(rkr->efx_Echotron->File.fLength);
}
void EchotronGui::cb_echotron_fnum(RKR_Choice* o, void* v) {
  ((EchotronGui*)(o->parent()))->cb_echotron_fnum_i(o,v);
}

Fl_Menu_Item EchotronGui::menu_echotron_fnum[] = {
 {"SwingPong", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Short Delays", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Flange + Echo", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Comb", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"EchoFlange", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Filtered Echo", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Notch-Wah", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Multi-Chorus", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"PingPong", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"90-Shifter", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Basic LR Delay", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};
EchotronGui::EchotronGui(int X, int Y, int W, int H, const char *L)
  : Fl_Group(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
{ echotron_activar = new RKR_Light_Button(6, 4, 34, 18, "On");
  echotron_activar->box(FL_UP_BOX);
  echotron_activar->shortcut(0x35);
  echotron_activar->color((Fl_Color)62);
  echotron_activar->selection_color((Fl_Color)1);
  echotron_activar->labeltype(FL_NORMAL_LABEL);
  echotron_activar->labelfont(0);
  echotron_activar->labelsize(10);
  echotron_activar->labelcolor(FL_FOREGROUND_COLOR);
  echotron_activar->callback((Fl_Callback*)cb_echotron_activar, (void*)(2));
  echotron_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  echotron_activar->when(FL_WHEN_CHANGED);
} // RKR_Light_Button* echotron_activar
{ echotron_preset = new RKR_Choice(79, 4, 76, 18, "Preset");
  echotron_preset->box(FL_FLAT_BOX);
  echotron_preset->down_box(FL_BORDER_BOX);
  echotron_preset->color(FL_BACKGROUND_COLOR);
  echotron_preset->selection_color(FL_FOREGROUND_COLOR);
  echotron_preset->labeltype(FL_NORMAL_LABEL);
  echotron_preset->labelfont(0);
  echotron_preset->labelsize(10);
  echotron_preset->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_preset->textsize(10);
  echotron_preset->textcolor(FL_BACKGROUND2_COLOR);
  echotron_preset->callback((Fl_Callback*)cb_echotron_preset, (void*)(12041));
  echotron_preset->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_preset->when(FL_WHEN_RELEASE_ALWAYS);
  echotron_preset->menu(menu_echotron_preset);
} // RKR_Choice* echotron_preset
{ echotron_WD = new RKR_Slider(56, 25, 100, 10, "Dry/Wet");
  echotron_WD->type(5);
  echotron_WD->box(FL_FLAT_BOX);
  echotron_WD->color((Fl_Color)178);
  echotron_WD->selection_color((Fl_Color)62);
  echotron_WD->labeltype(FL_NORMAL_LABEL);
  echotron_WD->labelfont(0);
  echotron_WD->labelsize(10);
  echotron_WD->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_WD->maximum(127);
  echotron_WD->step(1);
  echotron_WD->textcolor(FL_BACKGROUND2_COLOR);
  echotron_WD->callback((Fl_Callback*)cb_echotron_WD);
  echotron_WD->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* echotron_WD
{ echotron_pan = new RKR_Slider(56, 37, 100, 10, "Pan");
  echotron_pan->type(5);
  echotron_pan->box(FL_FLAT_BOX);
  echotron_pan->color((Fl_Color)178);
  echotron_pan->selection_color((Fl_Color)62);
  echotron_pan->labeltype(FL_NORMAL_LABEL);
  echotron_pan->labelfont(0);
  echotron_pan->labelsize(10);
  echotron_pan->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_pan->minimum(-64);
  echotron_pan->maximum(63);
  echotron_pan->step(1);
  echotron_pan->textcolor(FL_BACKGROUND2_COLOR);
  echotron_pan->callback((Fl_Callback*)cb_echotron_pan);
  echotron_pan->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* echotron_pan
{ echotron_tempo = new RKR_Slider(56, 49, 100, 10, "Tempo");
  echotron_tempo->type(5);
  echotron_tempo->box(FL_FLAT_BOX);
  echotron_tempo->color((Fl_Color)178);
  echotron_tempo->selection_color((Fl_Color)62);
  echotron_tempo->labeltype(FL_NORMAL_LABEL);
  echotron_tempo->labelfont(0);
  echotron_tempo->labelsize(10);
  echotron_tempo->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_tempo->minimum(1);
  echotron_tempo->maximum(600);
  echotron_tempo->step(1);
  echotron_tempo->value(60);
  echotron_tempo->textcolor(FL_BACKGROUND2_COLOR);
  echotron_tempo->callback((Fl_Callback*)cb_echotron_tempo);
  echotron_tempo->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_tempo->when(FL_WHEN_RELEASE);
} // RKR_Slider* echotron_tempo
{ echotron_damp = new RKR_Slider(56, 61, 100, 10, "Damp");
  echotron_damp->type(5);
  echotron_damp->box(FL_FLAT_BOX);
  echotron_damp->color((Fl_Color)178);
  echotron_damp->selection_color((Fl_Color)62);
  echotron_damp->labeltype(FL_NORMAL_LABEL);
  echotron_damp->labelfont(0);
  echotron_damp->labelsize(10);
  echotron_damp->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_damp->maximum(127);
  echotron_damp->step(1);
  echotron_damp->textcolor(FL_BACKGROUND2_COLOR);
  echotron_damp->callback((Fl_Callback*)cb_echotron_damp);
  echotron_damp->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_damp->when(FL_WHEN_CHANGED);
} // RKR_Slider* echotron_damp
{ echotron_fb = new RKR_Slider(56, 73, 100, 10, "Feedback");
  echotron_fb->type(5);
  echotron_fb->box(FL_FLAT_BOX);
  echotron_fb->color((Fl_Color)178);
  echotron_fb->selection_color((Fl_Color)62);
  echotron_fb->labeltype(FL_NORMAL_LABEL);
  echotron_fb->labelfont(0);
  echotron_fb->labelsize(10);
  echotron_fb->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_fb->minimum(-64);
  echotron_fb->maximum(64);
  echotron_fb->step(1);
  echotron_fb->textcolor(FL_BACKGROUND2_COLOR);
  echotron_fb->callback((Fl_Callback*)cb_echotron_fb);
  echotron_fb->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* echotron_fb
{ echotron_lrcross = new RKR_Slider(56, 85, 100, 10, "L/R Cross");
  echotron_lrcross->type(5);
  echotron_lrcross->box(FL_FLAT_BOX);
  echotron_lrcross->color((Fl_Color)178);
  echotron_lrcross->selection_color((Fl_Color)62);
  echotron_lrcross->labeltype(FL_NORMAL_LABEL);
  echotron_lrcross->labelfont(0);
  echotron_lrcross->labelsize(10);
  echotron_lrcross->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_lrcross->minimum(-64);
  echotron_lrcross->maximum(64);
  echotron_lrcross->step(1);
  echotron_lrcross->textcolor(FL_BACKGROUND2_COLOR);
  echotron_lrcross->callback((Fl_Callback*)cb_echotron_lrcross);
  echotron_lrcross->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_lrcross->when(FL_WHEN_CHANGED);
} // RKR_Slider* echotron_lrcross
{ echotron_width = new RKR_Slider(56, 97, 100, 10, "Width");
  echotron_width->type(5);
  echotron_width->box(FL_FLAT_BOX);
  echotron_width->color((Fl_Color)178);
  echotron_width->selection_color((Fl_Color)62);
  echotron_width->labeltype(FL_NORMAL_LABEL);
  echotron_width->labelfont(0);
  echotron_width->labelsize(10);
  echotron_width->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_width->maximum(127);
  echotron_width->step(1);
  echotron_width->textcolor(FL_BACKGROUND2_COLOR);
  echotron_width->callback((Fl_Callback*)cb_echotron_width);
  echotron_width->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_width->when(FL_WHEN_RELEASE);
} // RKR_Slider* echotron_width
{ echotron_deep = new RKR_Slider(56, 109, 100, 10, "Depth");
  echotron_deep->type(5);
  echotron_deep->box(FL_FLAT_BOX);
  echotron_deep->color((Fl_Color)178);
  echotron_deep->selection_color((Fl_Color)62);
  echotron_deep->labeltype(FL_NORMAL_LABEL);
  echotron_deep->labelfont(0);
  echotron_deep->labelsize(10);
  echotron_deep->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_deep->minimum(-64);
  echotron_deep->maximum(64);
  echotron_deep->step(1);
  echotron_deep->textcolor(FL_BACKGROUND2_COLOR);
  echotron_deep->callback((Fl_Callback*)cb_echotron_deep);
  echotron_deep->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_deep->when(FL_WHEN_RELEASE);
} // RKR_Slider* echotron_deep
{ echotron_stdf = new RKR_Slider(56, 121, 100, 10, "Stereo Df");
  echotron_stdf->type(5);
  echotron_stdf->box(FL_FLAT_BOX);
  echotron_stdf->color((Fl_Color)178);
  echotron_stdf->selection_color((Fl_Color)62);
  echotron_stdf->labeltype(FL_NORMAL_LABEL);
  echotron_stdf->labelfont(0);
  echotron_stdf->labelsize(10);
  echotron_stdf->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_stdf->maximum(127);
  echotron_stdf->step(1);
  echotron_stdf->textcolor(FL_BACKGROUND2_COLOR);
  echotron_stdf->callback((Fl_Callback*)cb_echotron_stdf);
  echotron_stdf->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_stdf->when(FL_WHEN_CHANGED);
} // RKR_Slider* echotron_stdf
{ echotron_af = new RKR_Check_Button(2, 136, 32, 15, "AF");
  echotron_af->box(FL_NO_BOX);
  echotron_af->down_box(FL_BORDER_BOX);
  echotron_af->color(FL_BACKGROUND_COLOR);
  echotron_af->selection_color(FL_FOREGROUND_COLOR);
  echotron_af->labeltype(FL_NORMAL_LABEL);
  echotron_af->labelfont(0);
  echotron_af->labelsize(10);
  echotron_af->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_af->callback((Fl_Callback*)cb_echotron_af, (void*)(2));
  echotron_af->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  echotron_af->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* echotron_af
{ RKR_Choice* o = echotron_lfotype = new RKR_Choice(88, 135, 64, 16, "LFO Type");
  echotron_lfotype->box(FL_FLAT_BOX);
  echotron_lfotype->down_box(FL_BORDER_BOX);
  echotron_lfotype->color(FL_BACKGROUND_COLOR);
  echotron_lfotype->selection_color(FL_FOREGROUND_COLOR);
  echotron_lfotype->labeltype(FL_NORMAL_LABEL);
  echotron_lfotype->labelfont(0);
  echotron_lfotype->labelsize(10);
  echotron_lfotype->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_lfotype->textsize(10);
  echotron_lfotype->textcolor(FL_BACKGROUND2_COLOR);
  echotron_lfotype->callback((Fl_Callback*)cb_echotron_lfotype);
  echotron_lfotype->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_lfotype->when(FL_WHEN_RELEASE);
  o->menu(m_lfo_menu->get_lfo_type());
} // RKR_Choice* echotron_lfotype
{ echotron_mf = new RKR_Check_Button(2, 151, 33, 15, "MF");
  echotron_mf->box(FL_NO_BOX);
  echotron_mf->down_box(FL_BORDER_BOX);
  echotron_mf->color(FL_BACKGROUND_COLOR);
  echotron_mf->selection_color(FL_FOREGROUND_COLOR);
  echotron_mf->labeltype(FL_NORMAL_LABEL);
  echotron_mf->labelfont(0);
  echotron_mf->labelsize(10);
  echotron_mf->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_mf->callback((Fl_Callback*)cb_echotron_mf, (void*)(2));
  echotron_mf->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  echotron_mf->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* echotron_mf
{ echotron_md = new RKR_Check_Button(33, 151, 34, 15, "MD");
  echotron_md->box(FL_NO_BOX);
  echotron_md->down_box(FL_BORDER_BOX);
  echotron_md->color(FL_BACKGROUND_COLOR);
  echotron_md->selection_color(FL_FOREGROUND_COLOR);
  echotron_md->labeltype(FL_NORMAL_LABEL);
  echotron_md->labelfont(0);
  echotron_md->labelsize(10);
  echotron_md->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_md->callback((Fl_Callback*)cb_echotron_md, (void*)(2));
  echotron_md->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  echotron_md->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* echotron_md
{ echotron_user = new RKR_Check_Button(67, 154, 39, 15, "User");
  echotron_user->box(FL_NO_BOX);
  echotron_user->down_box(FL_BORDER_BOX);
  echotron_user->color(FL_BACKGROUND_COLOR);
  echotron_user->selection_color(FL_FOREGROUND_COLOR);
  echotron_user->labeltype(FL_NORMAL_LABEL);
  echotron_user->labelfont(0);
  echotron_user->labelsize(10);
  echotron_user->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_user->callback((Fl_Callback*)cb_echotron_user, (void*)(2));
  echotron_user->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  echotron_user->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* echotron_user
{ B_ech = new RKR_Button(106, 156, 46, 10, "Browse");
  B_ech->box(FL_UP_BOX);
  B_ech->color(FL_BACKGROUND_COLOR);
  B_ech->selection_color(FL_BACKGROUND_COLOR);
  B_ech->labeltype(FL_NORMAL_LABEL);
  B_ech->labelfont(0);
  B_ech->labelsize(10);
  B_ech->labelcolor(FL_FOREGROUND_COLOR);
  B_ech->callback((Fl_Callback*)cb_B_ech, (void*)(2));
  B_ech->align(Fl_Align(FL_ALIGN_CENTER));
  B_ech->when(FL_WHEN_RELEASE);
  B_ech->deactivate();
} // RKR_Button* B_ech
{ RKR_Counter* o = echotron_length = new RKR_Counter(16, 167, 48, 12, "#");
  echotron_length->type(1);
  echotron_length->box(FL_UP_BOX);
  echotron_length->color(FL_BACKGROUND_COLOR);
  echotron_length->selection_color(FL_INACTIVE_COLOR);
  echotron_length->labeltype(FL_NORMAL_LABEL);
  echotron_length->labelfont(0);
  echotron_length->labelsize(10);
  echotron_length->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_length->minimum(1);
  echotron_length->maximum(127);
  echotron_length->step(1);
  echotron_length->value(1);
  echotron_length->textsize(9);
  echotron_length->callback((Fl_Callback*)cb_echotron_length);
  echotron_length->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_length->when(FL_WHEN_RELEASE);
  o->set_text_offset(-1);
} // RKR_Counter* echotron_length
{ echotron_fnum = new RKR_Choice(88, 168, 64, 14, "File");
  echotron_fnum->box(FL_FLAT_BOX);
  echotron_fnum->down_box(FL_BORDER_BOX);
  echotron_fnum->color(FL_BACKGROUND_COLOR);
  echotron_fnum->selection_color(FL_FOREGROUND_COLOR);
  echotron_fnum->labeltype(FL_NORMAL_LABEL);
  echotron_fnum->labelfont(0);
  echotron_fnum->labelsize(10);
  echotron_fnum->labelcolor(FL_BACKGROUND2_COLOR);
  echotron_fnum->textsize(10);
  echotron_fnum->textcolor(FL_BACKGROUND2_COLOR);
  echotron_fnum->callback((Fl_Callback*)cb_echotron_fnum, (void*)(12));
  echotron_fnum->align(Fl_Align(FL_ALIGN_LEFT));
  echotron_fnum->when(FL_WHEN_RELEASE);
  echotron_fnum->menu(menu_echotron_fnum);
} // RKR_Choice* echotron_fnum
position(X, Y);
end();
}
