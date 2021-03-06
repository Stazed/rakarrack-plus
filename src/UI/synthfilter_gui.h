// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef synthfilter_gui_h
#define synthfilter_gui_h
#include <FL/Fl.H>
#include "RKR_Light_Button.h"
#include "RKR_Value_Input.h"
#include "RKR_Check_Button.h"
#include "RKR_Counter.h"
#include "RKR_Choice.h"
#include "RKR_GUI_Effect.h"
#include "RKR_Slider.h"
#include "../process.h"
#include "rakarrack.h"
#include "common_gui_menu.h"

class SynthfilterGui : public RKR_Gui_Effect {
public:
  SynthfilterGui(int X, int Y, int W, int H, const char *L = 0);
  RKR_Light_Button *synthfilter_activar;
private:
  inline void cb_synthfilter_activar_i(RKR_Light_Button*, void*);
  static void cb_synthfilter_activar(RKR_Light_Button*, void*);
public:
  RKR_Choice *synthfilter_preset;
private:
  inline void cb_synthfilter_preset_i(RKR_Choice*, void*);
  static void cb_synthfilter_preset(RKR_Choice*, void*);
  static Fl_Menu_Item menu_synthfilter_preset[];
public:
  RKR_Slider *synthfilter_WD;
private:
  inline void cb_synthfilter_WD_i(RKR_Slider*, void*);
  static void cb_synthfilter_WD(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_Distort;
private:
  inline void cb_synthfilter_Distort_i(RKR_Slider*, void*);
  static void cb_synthfilter_Distort(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_freq;
private:
  inline void cb_synthfilter_freq_i(RKR_Slider*, void*);
  static void cb_synthfilter_freq(RKR_Slider*, void*);
public:
  RKR_Value_Input *synthfilter_rand;
private:
  inline void cb_synthfilter_rand_i(RKR_Value_Input*, void*);
  static void cb_synthfilter_rand(RKR_Value_Input*, void*);
public:
  RKR_Choice *synthfilter_lfotype;
private:
  inline void cb_synthfilter_lfotype_i(RKR_Choice*, void*);
  static void cb_synthfilter_lfotype(RKR_Choice*, void*);
public:
  RKR_Check_Button *synthfilter_subs;
private:
  inline void cb_synthfilter_subs_i(RKR_Check_Button*, void*);
  static void cb_synthfilter_subs(RKR_Check_Button*, void*);
public:
  RKR_Slider *synthfilter_stdf;
private:
  inline void cb_synthfilter_stdf_i(RKR_Slider*, void*);
  static void cb_synthfilter_stdf(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_width;
private:
  inline void cb_synthfilter_width_i(RKR_Slider*, void*);
  static void cb_synthfilter_width(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_fb;
private:
  inline void cb_synthfilter_fb_i(RKR_Slider*, void*);
  static void cb_synthfilter_fb(RKR_Slider*, void*);
public:
  RKR_Counter *synthfilter_Lstages;
private:
  inline void cb_synthfilter_Lstages_i(RKR_Counter*, void*);
  static void cb_synthfilter_Lstages(RKR_Counter*, void*);
public:
  RKR_Counter *synthfilter_Hstages;
private:
  inline void cb_synthfilter_Hstages_i(RKR_Counter*, void*);
  static void cb_synthfilter_Hstages(RKR_Counter*, void*);
public:
  RKR_Slider *synthfilter_dpth;
private:
  inline void cb_synthfilter_dpth_i(RKR_Slider*, void*);
  static void cb_synthfilter_dpth(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_EnvSens;
private:
  inline void cb_synthfilter_EnvSens_i(RKR_Slider*, void*);
  static void cb_synthfilter_EnvSens(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_ATime;
private:
  inline void cb_synthfilter_ATime_i(RKR_Slider*, void*);
  static void cb_synthfilter_ATime(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_RTime;
private:
  inline void cb_synthfilter_RTime_i(RKR_Slider*, void*);
  static void cb_synthfilter_RTime(RKR_Slider*, void*);
public:
  RKR_Slider *synthfilter_Offset;
private:
  inline void cb_synthfilter_Offset_i(RKR_Slider*, void*);
  static void cb_synthfilter_Offset(RKR_Slider*, void*);
  CommonGuiMenu *m_lfo_menu; 
public:
  void parameter_refresh(int index);
  void tap_tempo_update();
};
#endif
