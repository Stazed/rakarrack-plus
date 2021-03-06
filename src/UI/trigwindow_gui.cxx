// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "trigwindow_gui.h"

void TrigWindowGui::cb_aux_source_i(RKR_Choice* o, void*) {
  m_process->Aux_Source = (int) o->value();
}
void TrigWindowGui::cb_aux_source(RKR_Choice* o, void* v) {
  ((TrigWindowGui*)(o->parent()))->cb_aux_source_i(o,v);
}

Fl_Menu_Item TrigWindowGui::menu_aux_source[] = {
 {"Aux", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"L", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"R", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void TrigWindowGui::cb_aux_gain_i(RKR_Slider* o, void*) {
  m_process->Aux_Gain = (int)o->value();
}
void TrigWindowGui::cb_aux_gain(RKR_Slider* o, void* v) {
  ((TrigWindowGui*)(o->parent()))->cb_aux_gain_i(o,v);
}

void TrigWindowGui::cb_aux_thres_i(RKR_Slider* o, void*) {
  m_process->Aux_Threshold = (int) o->value();
}
void TrigWindowGui::cb_aux_thres(RKR_Slider* o, void* v) {
  ((TrigWindowGui*)(o->parent()))->cb_aux_thres_i(o,v);
}

void TrigWindowGui::cb_aux_midi_i(RKR_Value_Input* o, void*) {
  if(o->value()> 127) o->value(127);
if(o->value()< 1) o->value(1);
m_process->Aux_MIDI = (int)o->value();
}
void TrigWindowGui::cb_aux_midi(RKR_Value_Input* o, void* v) {
  ((TrigWindowGui*)(o->parent()))->cb_aux_midi_i(o,v);
}

void TrigWindowGui::cb_aux_min_i(RKR_Slider* o, void*) {
  m_process->Aux_Minimum = (int) o->value();
}
void TrigWindowGui::cb_aux_min(RKR_Slider* o, void* v) {
  ((TrigWindowGui*)(o->parent()))->cb_aux_min_i(o,v);
}

void TrigWindowGui::cb_aux_max_i(RKR_Slider* o, void*) {
  m_process->Aux_Maximum = (int) o->value();
}
void TrigWindowGui::cb_aux_max(RKR_Slider* o, void* v) {
  ((TrigWindowGui*)(o->parent()))->cb_aux_max_i(o,v);
}
TrigWindowGui::TrigWindowGui(int X, int Y, int W, int H, const char *L)
  : Fl_Double_Window(X, Y, W, H, L) {
  _TrigWindowGui();
}

TrigWindowGui::TrigWindowGui(int W, int H, const char *L)
  : Fl_Double_Window(0, 0, W, H, L) {
  clear_flag(16);
  _TrigWindowGui();
}

TrigWindowGui::TrigWindowGui()
  : Fl_Double_Window(0, 0, 200, 180, 0) {
  clear_flag(16);
  _TrigWindowGui();
}

void TrigWindowGui::_TrigWindowGui() {
this->box(FL_FLAT_BOX);
this->color(FL_BACKGROUND_COLOR);
this->selection_color(FL_BACKGROUND_COLOR);
this->labeltype(FL_NO_LABEL);
this->labelfont(0);
this->labelsize(14);
this->labelcolor(FL_FOREGROUND_COLOR);
this->align(Fl_Align(FL_ALIGN_TOP));
this->when(FL_WHEN_RELEASE);
{ Fondo5 = new Fl_Box(0, 1, 200, 180);
} // Fl_Box* Fondo5
{ ACI_LABEL = new RKR_Box(34, 8, 135, 24, "Analog Control");
  ACI_LABEL->box(FL_NO_BOX);
  ACI_LABEL->color(FL_BACKGROUND_COLOR);
  ACI_LABEL->selection_color(FL_BACKGROUND_COLOR);
  ACI_LABEL->labeltype(FL_NORMAL_LABEL);
  ACI_LABEL->labelfont(1);
  ACI_LABEL->labelsize(14);
  ACI_LABEL->labelcolor(FL_BACKGROUND2_COLOR);
  ACI_LABEL->align(Fl_Align(FL_ALIGN_CENTER));
  ACI_LABEL->when(FL_WHEN_RELEASE);
} // RKR_Box* ACI_LABEL
{ aux_vu = new NewVum(172, 18, 16, 144);
  aux_vu->type(2);
  aux_vu->box(FL_NO_BOX);
  aux_vu->color((Fl_Color)178);
  aux_vu->selection_color((Fl_Color)90);
  aux_vu->labeltype(FL_NORMAL_LABEL);
  aux_vu->labelfont(0);
  aux_vu->labelsize(14);
  aux_vu->labelcolor(FL_FOREGROUND_COLOR);
  aux_vu->minimum(15);
  aux_vu->maximum(-48);
  aux_vu->step(1);
  aux_vu->value(-48);
  aux_vu->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));
  aux_vu->when(FL_WHEN_NEVER);
} // NewVum* aux_vu
{ aux_source = new RKR_Choice(100, 37, 50, 17, "Source");
  aux_source->box(FL_FLAT_BOX);
  aux_source->down_box(FL_BORDER_BOX);
  aux_source->color(FL_BACKGROUND_COLOR);
  aux_source->selection_color(FL_SELECTION_COLOR);
  aux_source->labeltype(FL_NORMAL_LABEL);
  aux_source->labelfont(0);
  aux_source->labelsize(10);
  aux_source->labelcolor(FL_BACKGROUND2_COLOR);
  aux_source->textsize(10);
  aux_source->textcolor(FL_BACKGROUND2_COLOR);
  aux_source->callback((Fl_Callback*)cb_aux_source);
  aux_source->align(Fl_Align(FL_ALIGN_LEFT));
  aux_source->when(FL_WHEN_RELEASE);
  aux_source->menu(menu_aux_source);
} // RKR_Choice* aux_source
{ aux_gain = new RKR_Slider(56, 59, 100, 10, "Gain");
  aux_gain->type(5);
  aux_gain->box(FL_FLAT_BOX);
  aux_gain->color((Fl_Color)178);
  aux_gain->selection_color((Fl_Color)62);
  aux_gain->labeltype(FL_NORMAL_LABEL);
  aux_gain->labelfont(0);
  aux_gain->labelsize(10);
  aux_gain->labelcolor(FL_BACKGROUND2_COLOR);
  aux_gain->minimum(1);
  aux_gain->maximum(127);
  aux_gain->step(1);
  aux_gain->textcolor(FL_BACKGROUND2_COLOR);
  aux_gain->callback((Fl_Callback*)cb_aux_gain);
  aux_gain->align(Fl_Align(FL_ALIGN_LEFT));
  aux_gain->when(FL_WHEN_CHANGED);
} // RKR_Slider* aux_gain
{ aux_thres = new RKR_Slider(55, 75, 100, 10, "Threshold");
  aux_thres->type(5);
  aux_thres->box(FL_FLAT_BOX);
  aux_thres->color((Fl_Color)178);
  aux_thres->selection_color((Fl_Color)62);
  aux_thres->labeltype(FL_NORMAL_LABEL);
  aux_thres->labelfont(0);
  aux_thres->labelsize(10);
  aux_thres->labelcolor(FL_BACKGROUND2_COLOR);
  aux_thres->minimum(-70);
  aux_thres->maximum(20);
  aux_thres->step(1);
  aux_thres->textcolor(FL_BACKGROUND2_COLOR);
  aux_thres->callback((Fl_Callback*)cb_aux_thres);
  aux_thres->align(Fl_Align(FL_ALIGN_LEFT));
  aux_thres->when(FL_WHEN_CHANGED);
} // RKR_Slider* aux_thres
{ RKR_Value_Input* o = aux_midi = new RKR_Value_Input(111, 96, 32, 24, "Midi Control");
  aux_midi->box(FL_DOWN_BOX);
  aux_midi->color(FL_BACKGROUND2_COLOR);
  aux_midi->selection_color(FL_SELECTION_COLOR);
  aux_midi->labeltype(FL_NORMAL_LABEL);
  aux_midi->labelfont(0);
  aux_midi->labelsize(14);
  aux_midi->labelcolor(FL_BACKGROUND2_COLOR);
  aux_midi->minimum(1);
  aux_midi->maximum(127);
  aux_midi->step(1);
  aux_midi->value(1);
  aux_midi->callback((Fl_Callback*)cb_aux_midi);
  aux_midi->align(Fl_Align(FL_ALIGN_LEFT));
  aux_midi->when(FL_WHEN_CHANGED);
  o->set_text_offset(4);
  o->set_label_offset(4);
} // RKR_Value_Input* aux_midi
{ aux_min = new RKR_Slider(56, 130, 100, 10, "Minimum");
  aux_min->type(5);
  aux_min->box(FL_FLAT_BOX);
  aux_min->color((Fl_Color)178);
  aux_min->selection_color((Fl_Color)62);
  aux_min->labeltype(FL_NORMAL_LABEL);
  aux_min->labelfont(0);
  aux_min->labelsize(10);
  aux_min->labelcolor(FL_BACKGROUND2_COLOR);
  aux_min->maximum(127);
  aux_min->step(1);
  aux_min->textcolor(FL_BACKGROUND2_COLOR);
  aux_min->callback((Fl_Callback*)cb_aux_min);
  aux_min->align(Fl_Align(FL_ALIGN_LEFT));
  aux_min->when(FL_WHEN_CHANGED);
} // RKR_Slider* aux_min
{ aux_max = new RKR_Slider(56, 150, 100, 10, "Maximum");
  aux_max->type(5);
  aux_max->box(FL_FLAT_BOX);
  aux_max->color((Fl_Color)178);
  aux_max->selection_color((Fl_Color)62);
  aux_max->labeltype(FL_NORMAL_LABEL);
  aux_max->labelfont(0);
  aux_max->labelsize(10);
  aux_max->labelcolor(FL_BACKGROUND2_COLOR);
  aux_max->maximum(127);
  aux_max->step(1);
  aux_max->value(127);
  aux_max->textcolor(FL_BACKGROUND2_COLOR);
  aux_max->callback((Fl_Callback*)cb_aux_max);
  aux_max->align(Fl_Align(FL_ALIGN_LEFT));
  aux_max->when(FL_WHEN_CHANGED);
} // RKR_Slider* aux_max
this->m_process = NULL;
end();
resizable(this);
}

void TrigWindowGui::initialize(RKR *_rkr) {
  m_process = _rkr;
}
