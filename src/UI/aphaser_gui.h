// generated by Fast Light User Interface Designer (fluid) version 1.0309

#ifndef aphaser_gui_h
#define aphaser_gui_h
#include <FL/Fl.H>
#include "RKR_Light_Button.h"
#include "RKR_Check_Button.h"
#include "RKR_Choice.h"
#include "RKR_Counter.h"
#include "RKR_GUI_Effect.h"
#include "RKR_Slider.h"
#include "../process.h"
#include "rakarrack.h"
#include "common_gui_menu.h"

class AphaserGui : public RKR_Gui_Effect {
public:
  AphaserGui(int X, int Y, int W, int H, const char *L = 0);
  RKR_Light_Button *aphaser_activar;
private:
  inline void cb_aphaser_activar_i(RKR_Light_Button*, void*);
  static void cb_aphaser_activar(RKR_Light_Button*, void*);
public:
  RKR_Choice *aphaser_preset;
private:
  inline void cb_aphaser_preset_i(RKR_Choice*, void*);
  static void cb_aphaser_preset(RKR_Choice*, void*);
  static Fl_Menu_Item menu_aphaser_preset[];
public:
  RKR_Slider *aphaser_WD;
private:
  inline void cb_aphaser_WD_i(RKR_Slider*, void*);
  static void cb_aphaser_WD(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_distort;
private:
  inline void cb_aphaser_distort_i(RKR_Slider*, void*);
  static void cb_aphaser_distort(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_freq;
private:
  inline void cb_aphaser_freq_i(RKR_Slider*, void*);
  static void cb_aphaser_freq(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_rnd;
private:
  inline void cb_aphaser_rnd_i(RKR_Slider*, void*);
  static void cb_aphaser_rnd(RKR_Slider*, void*);
public:
  RKR_Choice *aphaser_lfotype;
private:
  inline void cb_aphaser_lfotype_i(RKR_Choice*, void*);
  static void cb_aphaser_lfotype(RKR_Choice*, void*);
public:
  RKR_Slider *aphaser_width;
private:
  inline void cb_aphaser_width_i(RKR_Slider*, void*);
  static void cb_aphaser_width(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_depth;
private:
  inline void cb_aphaser_depth_i(RKR_Slider*, void*);
  static void cb_aphaser_depth(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_fb;
private:
  inline void cb_aphaser_fb_i(RKR_Slider*, void*);
  static void cb_aphaser_fb(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_mismatch;
private:
  inline void cb_aphaser_mismatch_i(RKR_Slider*, void*);
  static void cb_aphaser_mismatch(RKR_Slider*, void*);
public:
  RKR_Slider *aphaser_stdf;
private:
  inline void cb_aphaser_stdf_i(RKR_Slider*, void*);
  static void cb_aphaser_stdf(RKR_Slider*, void*);
public:
  RKR_Counter *aphaser_stages;
private:
  inline void cb_aphaser_stages_i(RKR_Counter*, void*);
  static void cb_aphaser_stages(RKR_Counter*, void*);
public:
  RKR_Check_Button *aphaser_subs;
private:
  inline void cb_aphaser_subs_i(RKR_Check_Button*, void*);
  static void cb_aphaser_subs(RKR_Check_Button*, void*);
public:
  RKR_Check_Button *aphaser_hyper;
private:
  inline void cb_aphaser_hyper_i(RKR_Check_Button*, void*);
  static void cb_aphaser_hyper(RKR_Check_Button*, void*);
  CommonGuiMenu *m_lfo_menu; 
public:
  void parameter_refresh(int index);
  void tap_tempo_update();
};
#endif
