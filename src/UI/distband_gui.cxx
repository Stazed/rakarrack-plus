// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "distband_gui.h"

void DistBandGui::cb_distband_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Bypass[EFX_DISTBAND]);
 return;
}
m_process->EFX_Bypass[EFX_DISTBAND]=(int)o->value();
if((int) o->value()==0)
m_process->Rack_Effects[EFX_DISTBAND]->cleanup();
m_parent->findpos(EFX_DISTBAND,(int)o->value(),o);
}
void DistBandGui::cb_distband_activar(RKR_Light_Button* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_activar_i(o,v);
}

void DistBandGui::cb_distband_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_DISTBAND))
    m_process->Rack_Effects[EFX_DISTBAND]->setpreset((int)o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_DISTBAND]; i++)
{
    parameter_refresh(i);
};
}
void DistBandGui::cb_distband_preset(RKR_Choice* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_preset_i(o,v);
}

Fl_Menu_Item DistBandGui::menu_distband_preset[] = {
 {"Saturation", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Distorsion 1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Soft", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Modulated", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Crunch", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Distortion 2", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Distortion 3", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Distortion 4", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void DistBandGui::cb_distband_WD_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_DryWet);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_DryWet,Dry_Wet((int)(o->value())));
}
void DistBandGui::cb_distband_WD(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_WD_i(o,v);
}

void DistBandGui::cb_distband_LRc_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_LR_Cross);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_LR_Cross,(int)(o->value()));
}
void DistBandGui::cb_distband_LRc(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_LRc_i(o,v);
}

void DistBandGui::cb_distband_drive_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Drive);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Drive,(int)o->value());
}
void DistBandGui::cb_distband_drive(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_drive_i(o,v);
}

void DistBandGui::cb_distband_level_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Level);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Level,(int)o->value());
}
void DistBandGui::cb_distband_level(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_level_i(o,v);
}

void DistBandGui::cb_distband_volL_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Gain_Low);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Gain_Low,(int)o->value());
}
void DistBandGui::cb_distband_volL(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_volL_i(o,v);
}

void DistBandGui::cb_distband_volM_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Gain_Mid);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Gain_Mid,(int)o->value());
}
void DistBandGui::cb_distband_volM(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_volM_i(o,v);
}

void DistBandGui::cb_distband_volH_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Gain_Hi);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Gain_Hi,(int)o->value());
}
void DistBandGui::cb_distband_volH(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_volH_i(o,v);
}

void DistBandGui::cb_distband_cross1_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Cross_1);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Cross_1,(int)o->value());
}
void DistBandGui::cb_distband_cross1(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_cross1_i(o,v);
}

void DistBandGui::cb_distband_cross2_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Cross_2);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Cross_2,(int)o->value());
}
void DistBandGui::cb_distband_cross2(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_cross2_i(o,v);
}

void DistBandGui::cb_distband_tipoL_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Type_Low);
 return;
} 

m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Type_Low,(int)o->value());
}
void DistBandGui::cb_distband_tipoL(RKR_Choice* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_tipoL_i(o,v);
}

void DistBandGui::cb_distband_tipoM_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Type_Mid);
 return;
} 

m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Type_Mid,(int)o->value());
}
void DistBandGui::cb_distband_tipoM(RKR_Choice* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_tipoM_i(o,v);
}

void DistBandGui::cb_distband_tipoH_i(RKR_Choice* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Type_Hi);
 return;
} 

m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Type_Hi,(int)o->value());
}
void DistBandGui::cb_distband_tipoH(RKR_Choice* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_tipoH_i(o,v);
}

void DistBandGui::cb_distband_pan_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_DistBand_Pan);
 return;
} 
m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Pan,(int)(o->value()+64));
}
void DistBandGui::cb_distband_pan(RKR_Slider* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_pan_i(o,v);
}

void DistBandGui::cb_distband_st_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Stereo,(int)o->value());
}
void DistBandGui::cb_distband_st(RKR_Check_Button* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_st_i(o,v);
}

void DistBandGui::cb_distband_neg_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_DISTBAND]->changepar(DistBand_Negate,(int)o->value());
}
void DistBandGui::cb_distband_neg(RKR_Check_Button* o, void* v) {
  ((DistBandGui*)(o->parent()))->cb_distband_neg_i(o,v);
}
DistBandGui::DistBandGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = distband_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  distband_activar->box(FL_UP_BOX);
  distband_activar->shortcut(0x33);
  distband_activar->color((Fl_Color)62);
  distband_activar->selection_color((Fl_Color)1);
  distband_activar->labeltype(FL_NORMAL_LABEL);
  distband_activar->labelfont(0);
  distband_activar->labelsize(10);
  distband_activar->labelcolor(FL_FOREGROUND_COLOR);
  distband_activar->callback((Fl_Callback*)cb_distband_activar);
  distband_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  distband_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* distband_activar
{ RKR_Choice* o = distband_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  distband_preset->box(FL_FLAT_BOX);
  distband_preset->down_box(FL_BORDER_BOX);
  distband_preset->color(FL_BACKGROUND_COLOR);
  distband_preset->selection_color(FL_FOREGROUND_COLOR);
  distband_preset->labeltype(FL_NORMAL_LABEL);
  distband_preset->labelfont(0);
  distband_preset->labelsize(10);
  distband_preset->labelcolor(FL_BACKGROUND2_COLOR);
  distband_preset->textsize(10);
  distband_preset->textcolor(FL_BACKGROUND2_COLOR);
  distband_preset->callback((Fl_Callback*)cb_distband_preset, (void*)(UD_PRESET_DISTBAND));
  distband_preset->align(Fl_Align(FL_ALIGN_LEFT));
  distband_preset->when(FL_WHEN_RELEASE_ALWAYS);
  distband_preset->menu(menu_distband_preset);
  preset_choice = o;
} // RKR_Choice* distband_preset
{ distband_WD = new RKR_Slider(56, 26, 100, 10, "Dry/Wet");
  distband_WD->type(5);
  distband_WD->box(FL_FLAT_BOX);
  distband_WD->color((Fl_Color)178);
  distband_WD->selection_color((Fl_Color)62);
  distband_WD->labeltype(FL_NORMAL_LABEL);
  distband_WD->labelfont(0);
  distband_WD->labelsize(10);
  distband_WD->labelcolor(FL_BACKGROUND2_COLOR);
  distband_WD->maximum(127);
  distband_WD->step(1);
  distband_WD->textcolor(FL_BACKGROUND2_COLOR);
  distband_WD->callback((Fl_Callback*)cb_distband_WD);
  distband_WD->align(Fl_Align(FL_ALIGN_LEFT));
  distband_WD->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_WD
{ distband_LRc = new RKR_Slider(56, 38, 100, 10, "L/R Cross");
  distband_LRc->type(5);
  distband_LRc->box(FL_FLAT_BOX);
  distband_LRc->color((Fl_Color)178);
  distband_LRc->selection_color((Fl_Color)62);
  distband_LRc->labeltype(FL_NORMAL_LABEL);
  distband_LRc->labelfont(0);
  distband_LRc->labelsize(10);
  distband_LRc->labelcolor(FL_BACKGROUND2_COLOR);
  distband_LRc->maximum(127);
  distband_LRc->step(1);
  distband_LRc->textcolor(FL_BACKGROUND2_COLOR);
  distband_LRc->callback((Fl_Callback*)cb_distband_LRc);
  distband_LRc->align(Fl_Align(FL_ALIGN_LEFT));
  distband_LRc->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_LRc
{ distband_drive = new RKR_Slider(56, 50, 100, 10, "Drive");
  distband_drive->type(5);
  distband_drive->box(FL_FLAT_BOX);
  distband_drive->color((Fl_Color)178);
  distband_drive->selection_color((Fl_Color)62);
  distband_drive->labeltype(FL_NORMAL_LABEL);
  distband_drive->labelfont(0);
  distband_drive->labelsize(10);
  distband_drive->labelcolor(FL_BACKGROUND2_COLOR);
  distband_drive->maximum(127);
  distband_drive->step(1);
  distband_drive->textcolor(FL_BACKGROUND2_COLOR);
  distband_drive->callback((Fl_Callback*)cb_distband_drive);
  distband_drive->align(Fl_Align(FL_ALIGN_LEFT));
  distband_drive->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_drive
{ distband_level = new RKR_Slider(56, 62, 100, 10, "Level");
  distband_level->type(5);
  distband_level->box(FL_FLAT_BOX);
  distband_level->color((Fl_Color)178);
  distband_level->selection_color((Fl_Color)62);
  distband_level->labeltype(FL_NORMAL_LABEL);
  distband_level->labelfont(0);
  distband_level->labelsize(10);
  distband_level->labelcolor(FL_BACKGROUND2_COLOR);
  distband_level->maximum(127);
  distband_level->step(1);
  distband_level->textcolor(FL_BACKGROUND2_COLOR);
  distband_level->callback((Fl_Callback*)cb_distband_level);
  distband_level->align(Fl_Align(FL_ALIGN_LEFT));
  distband_level->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_level
{ distband_volL = new RKR_Slider(56, 76, 100, 10, "L. Gain");
  distband_volL->tooltip("Low Gain");
  distband_volL->type(5);
  distband_volL->box(FL_FLAT_BOX);
  distband_volL->color((Fl_Color)178);
  distband_volL->selection_color((Fl_Color)62);
  distband_volL->labeltype(FL_NORMAL_LABEL);
  distband_volL->labelfont(0);
  distband_volL->labelsize(10);
  distband_volL->labelcolor(FL_BACKGROUND2_COLOR);
  distband_volL->maximum(100);
  distband_volL->step(1);
  distband_volL->textcolor(FL_BACKGROUND2_COLOR);
  distband_volL->callback((Fl_Callback*)cb_distband_volL);
  distband_volL->align(Fl_Align(FL_ALIGN_LEFT));
  distband_volL->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_volL
{ distband_volM = new RKR_Slider(56, 89, 100, 10, "M. Gain");
  distband_volM->tooltip("Mid Gain");
  distband_volM->type(5);
  distband_volM->box(FL_FLAT_BOX);
  distband_volM->color((Fl_Color)178);
  distband_volM->selection_color((Fl_Color)62);
  distband_volM->labeltype(FL_NORMAL_LABEL);
  distband_volM->labelfont(0);
  distband_volM->labelsize(10);
  distband_volM->labelcolor(FL_BACKGROUND2_COLOR);
  distband_volM->maximum(100);
  distband_volM->step(1);
  distband_volM->textcolor(FL_BACKGROUND2_COLOR);
  distband_volM->callback((Fl_Callback*)cb_distband_volM);
  distband_volM->align(Fl_Align(FL_ALIGN_LEFT));
  distband_volM->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_volM
{ distband_volH = new RKR_Slider(56, 102, 100, 10, "H. Gain");
  distband_volH->tooltip("High Gain");
  distband_volH->type(5);
  distband_volH->box(FL_FLAT_BOX);
  distband_volH->color((Fl_Color)178);
  distband_volH->selection_color((Fl_Color)62);
  distband_volH->labeltype(FL_NORMAL_LABEL);
  distband_volH->labelfont(0);
  distband_volH->labelsize(10);
  distband_volH->labelcolor(FL_BACKGROUND2_COLOR);
  distband_volH->maximum(100);
  distband_volH->step(1);
  distband_volH->textcolor(FL_BACKGROUND2_COLOR);
  distband_volH->callback((Fl_Callback*)cb_distband_volH);
  distband_volH->align(Fl_Align(FL_ALIGN_LEFT));
  distband_volH->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_volH
{ distband_cross1 = new RKR_Slider(56, 115, 100, 10, "Cross 1");
  distband_cross1->type(5);
  distband_cross1->box(FL_FLAT_BOX);
  distband_cross1->color((Fl_Color)178);
  distband_cross1->selection_color((Fl_Color)62);
  distband_cross1->labeltype(FL_NORMAL_LABEL);
  distband_cross1->labelfont(0);
  distband_cross1->labelsize(10);
  distband_cross1->labelcolor(FL_BACKGROUND2_COLOR);
  distband_cross1->minimum(20);
  distband_cross1->maximum(1000);
  distband_cross1->step(1);
  distband_cross1->textcolor(FL_BACKGROUND2_COLOR);
  distband_cross1->callback((Fl_Callback*)cb_distband_cross1);
  distband_cross1->align(Fl_Align(FL_ALIGN_LEFT));
  distband_cross1->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_cross1
{ distband_cross2 = new RKR_Slider(56, 127, 100, 10, "Cross 2");
  distband_cross2->type(5);
  distband_cross2->box(FL_FLAT_BOX);
  distband_cross2->color((Fl_Color)178);
  distband_cross2->selection_color((Fl_Color)62);
  distband_cross2->labeltype(FL_NORMAL_LABEL);
  distband_cross2->labelfont(0);
  distband_cross2->labelsize(10);
  distband_cross2->labelcolor(FL_BACKGROUND2_COLOR);
  distband_cross2->minimum(800);
  distband_cross2->maximum(12000);
  distband_cross2->step(1);
  distband_cross2->textcolor(FL_BACKGROUND2_COLOR);
  distband_cross2->callback((Fl_Callback*)cb_distband_cross2);
  distband_cross2->align(Fl_Align(FL_ALIGN_LEFT));
  distband_cross2->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_cross2
{ RKR_Choice* o = distband_tipoL = new RKR_Choice(3, 139, 50, 16);
  distband_tipoL->tooltip("Distortion Type Low");
  distband_tipoL->box(FL_FLAT_BOX);
  distband_tipoL->down_box(FL_BORDER_BOX);
  distband_tipoL->color(FL_BACKGROUND_COLOR);
  distband_tipoL->selection_color(FL_FOREGROUND_COLOR);
  distband_tipoL->labeltype(FL_NORMAL_LABEL);
  distband_tipoL->labelfont(0);
  distband_tipoL->labelsize(10);
  distband_tipoL->labelcolor(FL_BACKGROUND2_COLOR);
  distband_tipoL->textsize(10);
  distband_tipoL->textcolor(FL_BACKGROUND2_COLOR);
  distband_tipoL->callback((Fl_Callback*)cb_distband_tipoL);
  distband_tipoL->align(Fl_Align(FL_ALIGN_LEFT));
  distband_tipoL->when(FL_WHEN_RELEASE);
  o->menu(m_dist_menu->get_distortion_type());
} // RKR_Choice* distband_tipoL
{ RKR_Choice* o = distband_tipoM = new RKR_Choice(54, 139, 50, 16);
  distband_tipoM->tooltip("Distortion Type Medium");
  distband_tipoM->box(FL_FLAT_BOX);
  distband_tipoM->down_box(FL_BORDER_BOX);
  distband_tipoM->color(FL_BACKGROUND_COLOR);
  distband_tipoM->selection_color(FL_FOREGROUND_COLOR);
  distband_tipoM->labeltype(FL_NORMAL_LABEL);
  distband_tipoM->labelfont(0);
  distband_tipoM->labelsize(10);
  distband_tipoM->labelcolor(FL_BACKGROUND2_COLOR);
  distband_tipoM->textsize(10);
  distband_tipoM->textcolor(FL_BACKGROUND2_COLOR);
  distband_tipoM->callback((Fl_Callback*)cb_distband_tipoM);
  distband_tipoM->align(Fl_Align(FL_ALIGN_LEFT));
  distband_tipoM->when(FL_WHEN_RELEASE);
  o->menu(m_dist_menu->get_distortion_type());
} // RKR_Choice* distband_tipoM
{ RKR_Choice* o = distband_tipoH = new RKR_Choice(105, 139, 50, 16);
  distband_tipoH->tooltip("Distortion Type High");
  distband_tipoH->box(FL_FLAT_BOX);
  distband_tipoH->down_box(FL_BORDER_BOX);
  distband_tipoH->color(FL_BACKGROUND_COLOR);
  distband_tipoH->selection_color(FL_FOREGROUND_COLOR);
  distband_tipoH->labeltype(FL_NORMAL_LABEL);
  distband_tipoH->labelfont(0);
  distband_tipoH->labelsize(10);
  distband_tipoH->labelcolor(FL_BACKGROUND2_COLOR);
  distband_tipoH->textsize(10);
  distband_tipoH->textcolor(FL_BACKGROUND2_COLOR);
  distband_tipoH->callback((Fl_Callback*)cb_distband_tipoH);
  distband_tipoH->align(Fl_Align(FL_ALIGN_LEFT));
  distband_tipoH->when(FL_WHEN_RELEASE);
  o->menu(m_dist_menu->get_distortion_type());
} // RKR_Choice* distband_tipoH
{ distband_pan = new RKR_Slider(56, 158, 100, 10, "Pan");
  distband_pan->type(5);
  distband_pan->box(FL_FLAT_BOX);
  distband_pan->color((Fl_Color)178);
  distband_pan->selection_color((Fl_Color)62);
  distband_pan->labeltype(FL_NORMAL_LABEL);
  distband_pan->labelfont(0);
  distband_pan->labelsize(10);
  distband_pan->labelcolor(FL_BACKGROUND2_COLOR);
  distband_pan->minimum(-64);
  distband_pan->maximum(63);
  distband_pan->step(1);
  distband_pan->textcolor(FL_BACKGROUND2_COLOR);
  distband_pan->callback((Fl_Callback*)cb_distband_pan);
  distband_pan->align(Fl_Align(FL_ALIGN_LEFT));
  distband_pan->when(FL_WHEN_CHANGED);
} // RKR_Slider* distband_pan
{ distband_st = new RKR_Check_Button(32, 168, 54, 15, "Stereo");
  distband_st->box(FL_NO_BOX);
  distband_st->down_box(FL_BORDER_BOX);
  distband_st->color(FL_BACKGROUND_COLOR);
  distband_st->selection_color(FL_FOREGROUND_COLOR);
  distband_st->labeltype(FL_NORMAL_LABEL);
  distband_st->labelfont(0);
  distband_st->labelsize(10);
  distband_st->labelcolor(FL_BACKGROUND2_COLOR);
  distband_st->callback((Fl_Callback*)cb_distband_st);
  distband_st->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  distband_st->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* distband_st
{ distband_neg = new RKR_Check_Button(101, 167, 44, 15, "Neg.");
  distband_neg->box(FL_NO_BOX);
  distband_neg->down_box(FL_BORDER_BOX);
  distband_neg->color(FL_BACKGROUND_COLOR);
  distband_neg->selection_color(FL_FOREGROUND_COLOR);
  distband_neg->labeltype(FL_NORMAL_LABEL);
  distband_neg->labelfont(0);
  distband_neg->labelsize(10);
  distband_neg->labelcolor(FL_BACKGROUND2_COLOR);
  distband_neg->callback((Fl_Callback*)cb_distband_neg);
  distband_neg->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  distband_neg->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* distband_neg
position(X, Y);
end();
}

void DistBandGui::parameter_refresh(int index) {
  switch (index)
      {
      case DistBand_DryWet:
          distband_WD->value(Dry_Wet(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_DryWet)));
          break;
      case DistBand_Pan:
          distband_pan->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Pan)-64);
          break;
      case DistBand_LR_Cross:
          distband_LRc->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_LR_Cross));
          break;
      case DistBand_Drive:
          distband_drive->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Drive));
          break;
      case DistBand_Level:
          distband_level->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Level));
          break;
      case DistBand_Type_Low:
          distband_tipoL->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Type_Low));
          break;
      case DistBand_Type_Mid:
          distband_tipoM->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Type_Mid));
          break;
      case DistBand_Type_Hi:
          distband_tipoH->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Type_Hi));
          break;
      case DistBand_Gain_Low:
          distband_volL->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Gain_Low));
          break;
      case DistBand_Gain_Mid:
          distband_volM->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Gain_Mid));
          break;
      case DistBand_Gain_Hi:
          distband_volH->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Gain_Hi));
          break;
      case DistBand_Negate:
          distband_neg->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Negate));
          break;
      case DistBand_Cross_1:
          distband_cross1->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Cross_1));
          break;
      case DistBand_Cross_2:
          distband_cross2->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Cross_2));
          break;
      case DistBand_Stereo:
          distband_st->value(m_process->Rack_Effects[EFX_DISTBAND]->getpar(DistBand_Stereo));
          break;
      }
}
