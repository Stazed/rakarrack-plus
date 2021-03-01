// generated by Fast Light User Interface Designer (fluid) version 1.0304

#include "compress_gui.h"

void CompressGui::cb_compress_activar_i(RKR_Light_Button* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Multi_On_Off);
 o->value(m_process->EFX_Bypass[EFX_COMPRESSOR]);
 return;
}
m_process->EFX_Bypass[EFX_COMPRESSOR]=(int)o->value();
m_process->Rack_Effects[EFX_COMPRESSOR]->cleanup();
m_parent->findpos(EFX_COMPRESSOR,(int)o->value(),o);
}
void CompressGui::cb_compress_activar(RKR_Light_Button* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_activar_i(o,v);
}

void CompressGui::cb_compress_preset_i(RKR_Choice* o, void* v) {
  long long ud= (long long) v;
if((ud==0)||(ud==UD_PRESET_COMPRESSOR))
    m_process->Rack_Effects[EFX_COMPRESSOR]->setpreset((int) o->value());

for (int i = 0; i < m_process->EFX_Param_Size[EFX_COMPRESSOR]; i++)
{
    parameter_refresh(i);
};
}
void CompressGui::cb_compress_preset(RKR_Choice* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_preset_i(o,v);
}

Fl_Menu_Item CompressGui::menu_compress_preset[] = {
 {"2:1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"4:1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"8:1", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {"Limiter", 0,  0, 0, 0, (uchar)FL_NORMAL_LABEL, 0, 10, 0},
 {0,0,0,0,0,0,0,0,0}
};

void CompressGui::cb_compress_ATime_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Compress_Attack);
 return;
} 
m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Attack,(int)o->value());
}
void CompressGui::cb_compress_ATime(RKR_Slider* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_ATime_i(o,v);
}

void CompressGui::cb_compress_RTime_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Compress_Release);
 return;
} 
m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Release, (int) o->value());
}
void CompressGui::cb_compress_RTime(RKR_Slider* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_RTime_i(o,v);
}

void CompressGui::cb_compress_Ratio_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Compress_Ratio);
 return;
} 
m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Ratio, (int) o->value());
}
void CompressGui::cb_compress_Ratio(RKR_Slider* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_Ratio_i(o,v);
}

void CompressGui::cb_compress_Knee_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Compress_Knee);
 return;
} 
m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Knee, (int) o->value());
}
void CompressGui::cb_compress_Knee(RKR_Slider* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_Knee_i(o,v);
}

void CompressGui::cb_compress_threshold_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Compress_Threshold);
 return;
} 
m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Threshold, (int)o->value());
}
void CompressGui::cb_compress_threshold(RKR_Slider* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_threshold_i(o,v);
}

void CompressGui::cb_compress_output_i(RKR_Slider* o, void*) {
  if(Fl::event_button()==FL_RIGHT_MOUSE)
{
 m_parent->getMIDIControl(MC_Compress_Output);
 return;
} 
m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Output,(int)o->value());
}
void CompressGui::cb_compress_output(RKR_Slider* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_compress_output_i(o,v);
}

void CompressGui::cb_Auto_Output_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Auto_Out,(int)o->value());
}
void CompressGui::cb_Auto_Output(RKR_Check_Button* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_Auto_Output_i(o,v);
}

void CompressGui::cb_Stereo_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Stereo,(int)o->value());
}
void CompressGui::cb_Stereo(RKR_Check_Button* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_Stereo_i(o,v);
}

void CompressGui::cb_Peak_i(RKR_Check_Button* o, void*) {
  m_process->Rack_Effects[EFX_COMPRESSOR]->changepar(Compress_Peak,(int)o->value());
}
void CompressGui::cb_Peak(RKR_Check_Button* o, void* v) {
  ((CompressGui*)(o->parent()))->cb_Peak_i(o,v);
}
CompressGui::CompressGui(int X, int Y, int W, int H, const char *L)
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
{ RKR_Light_Button* o = compress_activar = new RKR_Light_Button(5, 4, 34, 18, "On");
  compress_activar->box(FL_UP_BOX);
  compress_activar->shortcut(0x32);
  compress_activar->color((Fl_Color)62);
  compress_activar->selection_color((Fl_Color)1);
  compress_activar->labeltype(FL_NORMAL_LABEL);
  compress_activar->labelfont(0);
  compress_activar->labelsize(10);
  compress_activar->labelcolor(FL_FOREGROUND_COLOR);
  compress_activar->callback((Fl_Callback*)cb_compress_activar);
  compress_activar->align(Fl_Align(68|FL_ALIGN_INSIDE));
  compress_activar->when(FL_WHEN_CHANGED);
  activate_effect = o;
} // RKR_Light_Button* compress_activar
{ RKR_Choice* o = compress_preset = new RKR_Choice(77, 4, 76, 18, "Preset");
  compress_preset->box(FL_FLAT_BOX);
  compress_preset->down_box(FL_BORDER_BOX);
  compress_preset->color(FL_BACKGROUND_COLOR);
  compress_preset->selection_color(FL_FOREGROUND_COLOR);
  compress_preset->labeltype(FL_NORMAL_LABEL);
  compress_preset->labelfont(0);
  compress_preset->labelsize(10);
  compress_preset->labelcolor(FL_BACKGROUND2_COLOR);
  compress_preset->textsize(10);
  compress_preset->textcolor(FL_BACKGROUND2_COLOR);
  compress_preset->callback((Fl_Callback*)cb_compress_preset, (void*)(UD_PRESET_COMPRESSOR));
  compress_preset->align(Fl_Align(FL_ALIGN_LEFT));
  compress_preset->when(FL_WHEN_RELEASE_ALWAYS);
  compress_preset->menu(menu_compress_preset);
  preset_choice = o;
} // RKR_Choice* compress_preset
{ compress_ATime = new RKR_Slider(56, 39, 100, 10, "A. Time");
  compress_ATime->type(5);
  compress_ATime->box(FL_FLAT_BOX);
  compress_ATime->color((Fl_Color)178);
  compress_ATime->selection_color((Fl_Color)62);
  compress_ATime->labeltype(FL_NORMAL_LABEL);
  compress_ATime->labelfont(0);
  compress_ATime->labelsize(10);
  compress_ATime->labelcolor(FL_BACKGROUND2_COLOR);
  compress_ATime->minimum(10);
  compress_ATime->maximum(250);
  compress_ATime->step(1);
  compress_ATime->value(2);
  compress_ATime->textcolor(FL_BACKGROUND2_COLOR);
  compress_ATime->callback((Fl_Callback*)cb_compress_ATime);
  compress_ATime->align(Fl_Align(FL_ALIGN_LEFT));
  compress_ATime->when(FL_WHEN_CHANGED);
} // RKR_Slider* compress_ATime
{ compress_RTime = new RKR_Slider(56, 59, 100, 10, "R. Time");
  compress_RTime->type(5);
  compress_RTime->box(FL_FLAT_BOX);
  compress_RTime->color((Fl_Color)178);
  compress_RTime->selection_color((Fl_Color)62);
  compress_RTime->labeltype(FL_NORMAL_LABEL);
  compress_RTime->labelfont(0);
  compress_RTime->labelsize(10);
  compress_RTime->labelcolor(FL_BACKGROUND2_COLOR);
  compress_RTime->minimum(10);
  compress_RTime->maximum(500);
  compress_RTime->step(1);
  compress_RTime->value(120);
  compress_RTime->textcolor(FL_BACKGROUND2_COLOR);
  compress_RTime->callback((Fl_Callback*)cb_compress_RTime);
  compress_RTime->align(Fl_Align(FL_ALIGN_LEFT));
  compress_RTime->when(FL_WHEN_CHANGED);
} // RKR_Slider* compress_RTime
{ compress_Ratio = new RKR_Slider(56, 79, 100, 10, "Ratio");
  compress_Ratio->type(5);
  compress_Ratio->box(FL_FLAT_BOX);
  compress_Ratio->color((Fl_Color)178);
  compress_Ratio->selection_color((Fl_Color)62);
  compress_Ratio->labeltype(FL_NORMAL_LABEL);
  compress_Ratio->labelfont(0);
  compress_Ratio->labelsize(10);
  compress_Ratio->labelcolor(FL_BACKGROUND2_COLOR);
  compress_Ratio->minimum(2);
  compress_Ratio->maximum(42);
  compress_Ratio->step(1);
  compress_Ratio->value(2);
  compress_Ratio->textcolor(FL_BACKGROUND2_COLOR);
  compress_Ratio->callback((Fl_Callback*)cb_compress_Ratio);
  compress_Ratio->align(Fl_Align(FL_ALIGN_LEFT));
  compress_Ratio->when(FL_WHEN_CHANGED);
} // RKR_Slider* compress_Ratio
{ compress_Knee = new RKR_Slider(56, 99, 100, 10, "Knee");
  compress_Knee->type(5);
  compress_Knee->box(FL_FLAT_BOX);
  compress_Knee->color((Fl_Color)178);
  compress_Knee->selection_color((Fl_Color)62);
  compress_Knee->labeltype(FL_NORMAL_LABEL);
  compress_Knee->labelfont(0);
  compress_Knee->labelsize(10);
  compress_Knee->labelcolor(FL_BACKGROUND2_COLOR);
  compress_Knee->maximum(100);
  compress_Knee->step(1);
  compress_Knee->value(3);
  compress_Knee->textcolor(FL_BACKGROUND2_COLOR);
  compress_Knee->callback((Fl_Callback*)cb_compress_Knee);
  compress_Knee->align(Fl_Align(FL_ALIGN_LEFT));
  compress_Knee->when(FL_WHEN_CHANGED);
} // RKR_Slider* compress_Knee
{ compress_threshold = new RKR_Slider(56, 119, 100, 10, "Threshold");
  compress_threshold->type(5);
  compress_threshold->box(FL_FLAT_BOX);
  compress_threshold->color((Fl_Color)178);
  compress_threshold->selection_color((Fl_Color)62);
  compress_threshold->labeltype(FL_NORMAL_LABEL);
  compress_threshold->labelfont(0);
  compress_threshold->labelsize(10);
  compress_threshold->labelcolor(FL_BACKGROUND2_COLOR);
  compress_threshold->minimum(-60);
  compress_threshold->maximum(-3);
  compress_threshold->step(1);
  compress_threshold->value(-3);
  compress_threshold->textcolor(FL_BACKGROUND2_COLOR);
  compress_threshold->callback((Fl_Callback*)cb_compress_threshold);
  compress_threshold->align(Fl_Align(FL_ALIGN_LEFT));
  compress_threshold->when(FL_WHEN_CHANGED);
} // RKR_Slider* compress_threshold
{ compress_output = new RKR_Slider(56, 139, 100, 10, "Output");
  compress_output->type(5);
  compress_output->box(FL_FLAT_BOX);
  compress_output->color((Fl_Color)178);
  compress_output->selection_color((Fl_Color)62);
  compress_output->labeltype(FL_NORMAL_LABEL);
  compress_output->labelfont(0);
  compress_output->labelsize(10);
  compress_output->labelcolor(FL_BACKGROUND2_COLOR);
  compress_output->minimum(-40);
  compress_output->maximum(0);
  compress_output->step(1);
  compress_output->textcolor(FL_BACKGROUND2_COLOR);
  compress_output->callback((Fl_Callback*)cb_compress_output);
  compress_output->align(Fl_Align(FL_ALIGN_LEFT));
  compress_output->when(FL_WHEN_CHANGED);
} // RKR_Slider* compress_output
{ Auto_Output = new RKR_Check_Button(19, 166, 77, 15, "Auto Output");
  Auto_Output->box(FL_NO_BOX);
  Auto_Output->down_box(FL_BORDER_BOX);
  Auto_Output->color(FL_BACKGROUND_COLOR);
  Auto_Output->selection_color(FL_FOREGROUND_COLOR);
  Auto_Output->labeltype(FL_NORMAL_LABEL);
  Auto_Output->labelfont(0);
  Auto_Output->labelsize(10);
  Auto_Output->labelcolor(FL_BACKGROUND2_COLOR);
  Auto_Output->callback((Fl_Callback*)cb_Auto_Output);
  Auto_Output->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  Auto_Output->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* Auto_Output
{ Stereo = new RKR_Check_Button(104, 166, 50, 15, "Stereo");
  Stereo->tooltip("Level detect Left and Right separately.  Unchecked compresses according to L/\
R average.");
  Stereo->box(FL_NO_BOX);
  Stereo->down_box(FL_BORDER_BOX);
  Stereo->color(FL_BACKGROUND_COLOR);
  Stereo->selection_color(FL_FOREGROUND_COLOR);
  Stereo->labeltype(FL_NORMAL_LABEL);
  Stereo->labelfont(0);
  Stereo->labelsize(10);
  Stereo->labelcolor(FL_BACKGROUND2_COLOR);
  Stereo->callback((Fl_Callback*)cb_Stereo);
  Stereo->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  Stereo->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* Stereo
{ Peak = new RKR_Check_Button(19, 153, 45, 15, "Peak");
  Peak->tooltip("Peak Mode Compression");
  Peak->box(FL_NO_BOX);
  Peak->down_box(FL_BORDER_BOX);
  Peak->color(FL_BACKGROUND_COLOR);
  Peak->selection_color(FL_FOREGROUND_COLOR);
  Peak->labeltype(FL_NORMAL_LABEL);
  Peak->labelfont(0);
  Peak->labelsize(10);
  Peak->labelcolor(FL_BACKGROUND2_COLOR);
  Peak->callback((Fl_Callback*)cb_Peak);
  Peak->align(Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE));
  Peak->when(FL_WHEN_RELEASE);
} // RKR_Check_Button* Peak
position(X, Y);
end();
}

void CompressGui::parameter_refresh(int index) {
  switch (index)
      {
      case Compress_Threshold:
          compress_threshold->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Threshold));
          break;
      case Compress_Ratio:
          compress_Ratio->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Ratio));
          break;
      case Compress_Output:
          compress_output->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Output));
          break;
      case Compress_Attack:
          compress_ATime->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Attack));
          break;
      case Compress_Release:
          compress_RTime->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Release));
          break;
      case Compress_Auto_Out:
          Auto_Output->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Auto_Out));
          break;
      case Compress_Knee:
          compress_Knee->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Knee));
          break;
      case Compress_Stereo:
          Stereo->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Stereo));
          break;
      case Compress_Peak:
          Peak->value(m_process->Rack_Effects[EFX_COMPRESSOR]->getpar(Compress_Peak));
          break;
      }
}
