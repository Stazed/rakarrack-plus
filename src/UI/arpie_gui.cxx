// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "arpie_gui.h"

void ArpieGui::cb_arpie_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Multi_On_Off);
 o->value(rkr->EFX_Bypass[EFX_ARPIE]);
 return;
}
rkr->EFX_Bypass[EFX_ARPIE]=(int)o->value();
if((int) o->value()==0)
rkr->Rack_Effects[EFX_ARPIE]->cleanup();
rgui->findpos(EFX_ARPIE,(int)o->value(),o);
}
void ArpieGui::cb_arpie_activar(RKR_Light_Button* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_activar_i(o,v);
}

void ArpieGui::cb_arpie_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==12024))rkr->Rack_Effects[EFX_ARPIE]->setpreset((int) o->value());

for (int i = 0; i < rkr->EFX_Param_Size[EFX_ARPIE]; i++)
{
    parameter_refresh(i);
};
}
void ArpieGui::cb_arpie_preset(RKR_Choice* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_preset_i(o,v);
}

Fl_Menu_Item ArpieGui::menu_arpie_preset[] = {
 {"Tremolo", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Arpie 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Arpie 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Arpie 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Simple Arpie", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Canyon", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Panning Arpie 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Panning Arpie 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Panning Arpie 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Feedback Arpie", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void ArpieGui::cb_arpie_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_DryWet);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_DryWet,Dry_Wet((int)(o->value())));
}
void ArpieGui::cb_arpie_WD(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_WD_i(o,v);
}

void ArpieGui::cb_arpie_arpe_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_ArpeWD);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_ArpeWD,(int)o->value());
}
void ArpieGui::cb_arpie_arpe(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_arpe_i(o,v);
}

void ArpieGui::cb_arpie_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_Pan);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Pan,(int)(o->value()+64));
}
void ArpieGui::cb_arpie_pan(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_pan_i(o,v);
}

void ArpieGui::cb_arpie_delay_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_Tempo);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Tempo,(int)o->value());
}
void ArpieGui::cb_arpie_delay(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_delay_i(o,v);
}

void ArpieGui::cb_arpie_subdiv_i(RKR_Choice* o, void*) {
  rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Subdivision,(int)o->value());
}
void ArpieGui::cb_arpie_subdiv(RKR_Choice* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_subdiv_i(o,v);
}

void ArpieGui::cb_arpie_LRdl_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_LR_Delay);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_LR_Delay,(int)o->value());
}
void ArpieGui::cb_arpie_LRdl(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_LRdl_i(o,v);
}

void ArpieGui::cb_arpie_LRc_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_LR_Cross);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_LR_Cross,(int)(o->value()));
}
void ArpieGui::cb_arpie_LRc(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_LRc_i(o,v);
}

void ArpieGui::cb_arpie_fb_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_Feedback);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Feedback,(int)o->value());
}
void ArpieGui::cb_arpie_fb(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_fb_i(o,v);
}

void ArpieGui::cb_arpie_damp_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==3)
{
 rgui->getMIDIControl(MC_Arpie_Damp);
 return;
} 
rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Damp,(int)o->value());
}
void ArpieGui::cb_arpie_damp(RKR_Slider* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_damp_i(o,v);
}

void ArpieGui::cb_arpie_harm_i(RKR_Counter* o, void*) {
  rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Harm,(int)o->value());
}
void ArpieGui::cb_arpie_harm(RKR_Counter* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_harm_i(o,v);
}

void ArpieGui::cb_arpie_pattern_i(RKR_Choice* o, void*) {
  rkr->Rack_Effects[EFX_ARPIE]->changepar(Arpie_Pattern,(int)o->value());
}
void ArpieGui::cb_arpie_pattern(RKR_Choice* o, void* v) {
  ((ArpieGui*)(o->parent()))->cb_arpie_pattern_i(o,v);
}

Fl_Menu_Item ArpieGui::menu_arpie_pattern[] = {
 {"Ascending", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Descending", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"UpDown", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Stutter", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Interrupted Descent", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Double Descend ", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};
ArpieGui::ArpieGui(int X, int Y, int W, int H, const char *L)
  : RKR_Gui_Effect(0, 0, W, H, L) {
this->box(FL_UP_BOX);
this->color(FL_FOREGROUND_COLOR);
this->selection_color(FL_FOREGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->user_data((void*)(1));
this->align(Fl_Align(96|FL_ALIGN_INSIDE));
this->when(FL_WHEN_RELEASE);
{ RKR_Light_Button* o = arpie_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  arpie_activar->box(FL_UP_BOX);
  arpie_activar->shortcut(0x35);
  arpie_activar->color((Fl_Color)62);
  arpie_activar->selection_color((Fl_Color)1);
  arpie_activar->labeltype(FL_NORMAL_LABEL);
  arpie_activar->labelfont(0);
  arpie_activar->labelsize(10);
  arpie_activar->labelcolor(FL_FOREGROUND_COLOR);
  arpie_activar->callback((Fl_Callback*)cb_arpie_activar, (void*)(2));
  arpie_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  arpie_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* arpie_activar
{ RKR_Choice* o = arpie_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  arpie_preset->box(FL_FLAT_BOX);
  arpie_preset->down_box(FL_BORDER_BOX);
  arpie_preset->color(FL_BACKGROUND_COLOR);
  arpie_preset->selection_color(FL_FOREGROUND_COLOR);
  arpie_preset->labeltype(FL_NORMAL_LABEL);
  arpie_preset->labelfont(0);
  arpie_preset->labelsize(10);
  arpie_preset->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_preset->textsize(10);
  arpie_preset->textcolor(FL_BACKGROUND2_COLOR);
  arpie_preset->callback((Fl_Callback*)cb_arpie_preset, (void*)(12024));
  arpie_preset->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_preset->when(FL_WHEN_RELEASE_ALWAYS);
  arpie_preset->menu(menu_arpie_preset);
  preset_choice = o;
} // RKR_Choice* arpie_preset
{ arpie_WD = new RKR_Slider(56, 28, 100, 10, "Dry/Wet");
  arpie_WD->type(5);
  arpie_WD->box(FL_FLAT_BOX);
  arpie_WD->color((Fl_Color)178);
  arpie_WD->selection_color((Fl_Color)62);
  arpie_WD->labeltype(FL_NORMAL_LABEL);
  arpie_WD->labelfont(0);
  arpie_WD->labelsize(10);
  arpie_WD->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_WD->maximum(127);
  arpie_WD->step(1);
  arpie_WD->textcolor(FL_BACKGROUND2_COLOR);
  arpie_WD->callback((Fl_Callback*)cb_arpie_WD);
  arpie_WD->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* arpie_WD
{ arpie_arpe = new RKR_Slider(56, 42, 100, 10, "Arpe\'s");
  arpie_arpe->type(5);
  arpie_arpe->box(FL_FLAT_BOX);
  arpie_arpe->color((Fl_Color)178);
  arpie_arpe->selection_color((Fl_Color)62);
  arpie_arpe->labeltype(FL_NORMAL_LABEL);
  arpie_arpe->labelfont(0);
  arpie_arpe->labelsize(10);
  arpie_arpe->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_arpe->maximum(127);
  arpie_arpe->step(1);
  arpie_arpe->textcolor(FL_BACKGROUND2_COLOR);
  arpie_arpe->callback((Fl_Callback*)cb_arpie_arpe);
  arpie_arpe->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_arpe->when(FL_WHEN_CHANGED);
} // RKR_Slider* arpie_arpe
{ arpie_pan = new RKR_Slider(56, 56, 100, 10, "Pan");
  arpie_pan->type(5);
  arpie_pan->box(FL_FLAT_BOX);
  arpie_pan->color((Fl_Color)178);
  arpie_pan->selection_color((Fl_Color)62);
  arpie_pan->labeltype(FL_NORMAL_LABEL);
  arpie_pan->labelfont(0);
  arpie_pan->labelsize(10);
  arpie_pan->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_pan->minimum(-64);
  arpie_pan->maximum(63);
  arpie_pan->step(1);
  arpie_pan->textcolor(FL_BACKGROUND2_COLOR);
  arpie_pan->callback((Fl_Callback*)cb_arpie_pan);
  arpie_pan->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* arpie_pan
{ arpie_delay = new RKR_Slider(56, 70, 100, 10, "Tempo");
  arpie_delay->type(5);
  arpie_delay->box(FL_FLAT_BOX);
  arpie_delay->color((Fl_Color)178);
  arpie_delay->selection_color((Fl_Color)62);
  arpie_delay->labeltype(FL_NORMAL_LABEL);
  arpie_delay->labelfont(0);
  arpie_delay->labelsize(10);
  arpie_delay->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_delay->minimum(1);
  arpie_delay->maximum(600);
  arpie_delay->step(1);
  arpie_delay->value(1);
  arpie_delay->textcolor(FL_BACKGROUND2_COLOR);
  arpie_delay->callback((Fl_Callback*)cb_arpie_delay);
  arpie_delay->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_delay->when(FL_WHEN_RELEASE);
} // RKR_Slider* arpie_delay
{ RKR_Choice* o = arpie_subdiv = new RKR_Choice(78, 82, 76, 18, "SubDivision");
  arpie_subdiv->box(FL_FLAT_BOX);
  arpie_subdiv->down_box(FL_BORDER_BOX);
  arpie_subdiv->color(FL_BACKGROUND_COLOR);
  arpie_subdiv->selection_color(FL_FOREGROUND_COLOR);
  arpie_subdiv->labeltype(FL_NORMAL_LABEL);
  arpie_subdiv->labelfont(0);
  arpie_subdiv->labelsize(10);
  arpie_subdiv->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_subdiv->textsize(10);
  arpie_subdiv->textcolor(FL_BACKGROUND2_COLOR);
  arpie_subdiv->callback((Fl_Callback*)cb_arpie_subdiv, (void*)(12));
  arpie_subdiv->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_subdiv->when(FL_WHEN_RELEASE_ALWAYS);
  o->menu(m_subdiv_menu->get_subdiv_type());
} // RKR_Choice* arpie_subdiv
{ arpie_LRdl = new RKR_Slider(56, 103, 100, 10, "L/R Delay");
  arpie_LRdl->type(5);
  arpie_LRdl->box(FL_FLAT_BOX);
  arpie_LRdl->color((Fl_Color)178);
  arpie_LRdl->selection_color((Fl_Color)62);
  arpie_LRdl->labeltype(FL_NORMAL_LABEL);
  arpie_LRdl->labelfont(0);
  arpie_LRdl->labelsize(10);
  arpie_LRdl->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_LRdl->maximum(127);
  arpie_LRdl->step(1);
  arpie_LRdl->textcolor(FL_BACKGROUND2_COLOR);
  arpie_LRdl->callback((Fl_Callback*)cb_arpie_LRdl);
  arpie_LRdl->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_LRdl->when(FL_WHEN_RELEASE);
} // RKR_Slider* arpie_LRdl
{ arpie_LRc = new RKR_Slider(56, 118, 100, 10, "L/R Cross");
  arpie_LRc->type(5);
  arpie_LRc->box(FL_FLAT_BOX);
  arpie_LRc->color((Fl_Color)178);
  arpie_LRc->selection_color((Fl_Color)62);
  arpie_LRc->labeltype(FL_NORMAL_LABEL);
  arpie_LRc->labelfont(0);
  arpie_LRc->labelsize(10);
  arpie_LRc->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_LRc->maximum(127);
  arpie_LRc->step(1);
  arpie_LRc->textcolor(FL_BACKGROUND2_COLOR);
  arpie_LRc->callback((Fl_Callback*)cb_arpie_LRc);
  arpie_LRc->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_LRc->when(FL_WHEN_CHANGED);
} // RKR_Slider* arpie_LRc
{ arpie_fb = new RKR_Slider(56, 133, 100, 10, "Feedback");
  arpie_fb->type(5);
  arpie_fb->box(FL_FLAT_BOX);
  arpie_fb->color((Fl_Color)178);
  arpie_fb->selection_color((Fl_Color)62);
  arpie_fb->labeltype(FL_NORMAL_LABEL);
  arpie_fb->labelfont(0);
  arpie_fb->labelsize(10);
  arpie_fb->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_fb->maximum(127);
  arpie_fb->step(1);
  arpie_fb->textcolor(FL_BACKGROUND2_COLOR);
  arpie_fb->callback((Fl_Callback*)cb_arpie_fb);
  arpie_fb->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_fb->when(FL_WHEN_CHANGED);
} // RKR_Slider* arpie_fb
{ arpie_damp = new RKR_Slider(56, 167, 100, 10, "Damp");
  arpie_damp->type(5);
  arpie_damp->box(FL_FLAT_BOX);
  arpie_damp->color((Fl_Color)178);
  arpie_damp->selection_color((Fl_Color)62);
  arpie_damp->labeltype(FL_NORMAL_LABEL);
  arpie_damp->labelfont(0);
  arpie_damp->labelsize(10);
  arpie_damp->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_damp->maximum(127);
  arpie_damp->step(1);
  arpie_damp->textcolor(FL_BACKGROUND2_COLOR);
  arpie_damp->callback((Fl_Callback*)cb_arpie_damp);
  arpie_damp->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_damp->when(FL_WHEN_CHANGED);
} // RKR_Slider* arpie_damp
{ RKR_Counter* o = arpie_harm = new RKR_Counter(20, 150, 52, 12, "H");
  arpie_harm->tooltip("Number of steps in the pattern");
  arpie_harm->type(1);
  arpie_harm->box(FL_THIN_UP_BOX);
  arpie_harm->color(FL_BACKGROUND_COLOR);
  arpie_harm->selection_color(FL_INACTIVE_COLOR);
  arpie_harm->labeltype(FL_NORMAL_LABEL);
  arpie_harm->labelfont(0);
  arpie_harm->labelsize(10);
  arpie_harm->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_harm->minimum(1);
  arpie_harm->maximum(8);
  arpie_harm->step(1);
  arpie_harm->value(1);
  arpie_harm->textsize(9);
  arpie_harm->callback((Fl_Callback*)cb_arpie_harm);
  arpie_harm->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_harm->when(FL_WHEN_CHANGED);
  o->set_text_offset(-1);
} // RKR_Counter* arpie_harm
{ arpie_pattern = new RKR_Choice(77, 147, 76, 18);
  arpie_pattern->box(FL_FLAT_BOX);
  arpie_pattern->down_box(FL_BORDER_BOX);
  arpie_pattern->color(FL_BACKGROUND_COLOR);
  arpie_pattern->selection_color(FL_FOREGROUND_COLOR);
  arpie_pattern->labeltype(FL_NORMAL_LABEL);
  arpie_pattern->labelfont(0);
  arpie_pattern->labelsize(10);
  arpie_pattern->labelcolor(FL_BACKGROUND2_COLOR);
  arpie_pattern->textsize(10);
  arpie_pattern->textcolor(FL_BACKGROUND2_COLOR);
  arpie_pattern->callback((Fl_Callback*)cb_arpie_pattern, (void*)(12));
  arpie_pattern->align(Fl_Align(FL_ALIGN_LEFT));
  arpie_pattern->when(FL_WHEN_RELEASE_ALWAYS);
  arpie_pattern->menu(menu_arpie_pattern);
} // RKR_Choice* arpie_pattern
position(X, Y);
end();
}

void ArpieGui::parameter_refresh(int index) {
  switch (index)
      {
      case Arpie_DryWet:
          arpie_WD->value(Dry_Wet(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_DryWet)));
          break;
      case Arpie_Pan:
          arpie_pan->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Pan)-64);
          break;
      case Arpie_Tempo:
          arpie_delay->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Tempo));
          break;
      case Arpie_LR_Delay:
          arpie_LRdl->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_LR_Delay));
          break;
      case Arpie_LR_Cross:
          arpie_LRc->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_LR_Cross));
          break;
      case Arpie_Feedback:
          arpie_fb->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Feedback));
          break;
      case Arpie_Damp:
          arpie_damp->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Damp));
          break;
      case Arpie_ArpeWD:
          arpie_arpe->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_ArpeWD));
          break;
      case Arpie_Harm:
          arpie_harm->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Harm));
          break;
      case Arpie_Pattern:
          arpie_pattern->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Pattern));
          break;
      case Arpie_Subdivision:
          arpie_subdiv->value(rkr->Rack_Effects[EFX_ARPIE]->getpar(Arpie_Subdivision));
          break;
      }
}
