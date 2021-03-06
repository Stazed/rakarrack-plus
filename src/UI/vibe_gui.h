// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef vibe_gui_h
#define vibe_gui_h
#include <FL/Fl.H>
#include "RKR_Light_Button.h"
#include "RKR_Check_Button.h"
#include "RKR_Choice.h"
#include "RKR_GUI_Effect.h"
#include "RKR_Slider.h"
#include "../process.h"
#include "rakarrack.h"
#include "common_gui_menu.h"

class VibeGui : public RKR_Gui_Effect {
public:
  VibeGui(int X, int Y, int W, int H, const char *L = 0);
  RKR_Light_Button *vibe_activar;
private:
  inline void cb_vibe_activar_i(RKR_Light_Button*, void*);
  static void cb_vibe_activar(RKR_Light_Button*, void*);
public:
  RKR_Choice *vibe_preset;
private:
  inline void cb_vibe_preset_i(RKR_Choice*, void*);
  static void cb_vibe_preset(RKR_Choice*, void*);
  static Fl_Menu_Item menu_vibe_preset[];
public:
  RKR_Slider *vibe_WD;
private:
  inline void cb_vibe_WD_i(RKR_Slider*, void*);
  static void cb_vibe_WD(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_width;
private:
  inline void cb_vibe_width_i(RKR_Slider*, void*);
  static void cb_vibe_width(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_dpth;
private:
  inline void cb_vibe_dpth_i(RKR_Slider*, void*);
  static void cb_vibe_dpth(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_freq;
private:
  inline void cb_vibe_freq_i(RKR_Slider*, void*);
  static void cb_vibe_freq(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_rnd;
private:
  inline void cb_vibe_rnd_i(RKR_Slider*, void*);
  static void cb_vibe_rnd(RKR_Slider*, void*);
public:
  RKR_Choice *vibe_lfotype;
private:
  inline void cb_vibe_lfotype_i(RKR_Choice*, void*);
  static void cb_vibe_lfotype(RKR_Choice*, void*);
public:
  RKR_Slider *vibe_stdf;
private:
  inline void cb_vibe_stdf_i(RKR_Slider*, void*);
  static void cb_vibe_stdf(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_fb;
private:
  inline void cb_vibe_fb_i(RKR_Slider*, void*);
  static void cb_vibe_fb(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_LR;
private:
  inline void cb_vibe_LR_i(RKR_Slider*, void*);
  static void cb_vibe_LR(RKR_Slider*, void*);
public:
  RKR_Slider *vibe_pan;
private:
  inline void cb_vibe_pan_i(RKR_Slider*, void*);
  static void cb_vibe_pan(RKR_Slider*, void*);
public:
  RKR_Check_Button *vibe_stereo;
private:
  inline void cb_vibe_stereo_i(RKR_Check_Button*, void*);
  static void cb_vibe_stereo(RKR_Check_Button*, void*);
  CommonGuiMenu *m_lfo_menu; 
public:
  void parameter_refresh(int index);
  void tap_tempo_update();
};
#endif
