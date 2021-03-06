// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef compress_gui_h
#define compress_gui_h
#include <FL/Fl.H>
#include "RKR_Light_Button.h"
#include "RKR_Check_Button.h"
#include "RKR_Choice.h"
#include "RKR_GUI_Effect.h"
#include "RKR_Slider.h"
#include "../process.h"
#include "rakarrack.h"

class CompressGui : public RKR_Gui_Effect {
public:
  CompressGui(int X, int Y, int W, int H, const char *L = 0);
  RKR_Light_Button *compress_activar;
private:
  inline void cb_compress_activar_i(RKR_Light_Button*, void*);
  static void cb_compress_activar(RKR_Light_Button*, void*);
public:
  RKR_Choice *compress_preset;
private:
  inline void cb_compress_preset_i(RKR_Choice*, void*);
  static void cb_compress_preset(RKR_Choice*, void*);
  static Fl_Menu_Item menu_compress_preset[];
public:
  RKR_Slider *compress_ATime;
private:
  inline void cb_compress_ATime_i(RKR_Slider*, void*);
  static void cb_compress_ATime(RKR_Slider*, void*);
public:
  RKR_Slider *compress_RTime;
private:
  inline void cb_compress_RTime_i(RKR_Slider*, void*);
  static void cb_compress_RTime(RKR_Slider*, void*);
public:
  RKR_Slider *compress_Ratio;
private:
  inline void cb_compress_Ratio_i(RKR_Slider*, void*);
  static void cb_compress_Ratio(RKR_Slider*, void*);
public:
  RKR_Slider *compress_Knee;
private:
  inline void cb_compress_Knee_i(RKR_Slider*, void*);
  static void cb_compress_Knee(RKR_Slider*, void*);
public:
  RKR_Slider *compress_threshold;
private:
  inline void cb_compress_threshold_i(RKR_Slider*, void*);
  static void cb_compress_threshold(RKR_Slider*, void*);
public:
  RKR_Slider *compress_output;
private:
  inline void cb_compress_output_i(RKR_Slider*, void*);
  static void cb_compress_output(RKR_Slider*, void*);
public:
  RKR_Check_Button *Auto_Output;
private:
  inline void cb_Auto_Output_i(RKR_Check_Button*, void*);
  static void cb_Auto_Output(RKR_Check_Button*, void*);
public:
  RKR_Check_Button *Stereo;
private:
  inline void cb_Stereo_i(RKR_Check_Button*, void*);
  static void cb_Stereo(RKR_Check_Button*, void*);
public:
  RKR_Check_Button *Peak;
private:
  inline void cb_Peak_i(RKR_Check_Button*, void*);
  static void cb_Peak(RKR_Check_Button*, void*);
public:
  void parameter_refresh(int index);
};
#endif
