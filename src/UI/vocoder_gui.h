// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef vocoder_gui_h
#define vocoder_gui_h
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include "sliderW.h"
#include "../process.h"
#include "rakarrack.h"
#include "newvum.h"
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Choice.H>

class VocoderGui : public Fl_Group {
public:
  VocoderGui(int X, int Y, int W, int H, const char *L = 0);
  Fl_Light_Button *vo_activar;
private:
  inline void cb_vo_activar_i(Fl_Light_Button*, void*);
  static void cb_vo_activar(Fl_Light_Button*, void*);
public:
  Fl_Choice *vo_preset;
private:
  inline void cb_vo_preset_i(Fl_Choice*, void*);
  static void cb_vo_preset(Fl_Choice*, void*);
  static Fl_Menu_Item menu_vo_preset[];
public:
  SliderW *vo_WD;
private:
  inline void cb_vo_WD_i(SliderW*, void*);
  static void cb_vo_WD(SliderW*, void*);
public:
  SliderW *vo_pan;
private:
  inline void cb_vo_pan_i(SliderW*, void*);
  static void cb_vo_pan(SliderW*, void*);
public:
  SliderW *vo_input;
private:
  inline void cb_vo_input_i(SliderW*, void*);
  static void cb_vo_input(SliderW*, void*);
public:
  SliderW *vo_mu;
private:
  inline void cb_vo_mu_i(SliderW*, void*);
  static void cb_vo_mu(SliderW*, void*);
public:
  SliderW *vo_q;
private:
  inline void cb_vo_q_i(SliderW*, void*);
  static void cb_vo_q(SliderW*, void*);
public:
  SliderW *vo_ring;
private:
  inline void cb_vo_ring_i(SliderW*, void*);
  static void cb_vo_ring(SliderW*, void*);
public:
  SliderW *vo_level;
private:
  inline void cb_vo_level_i(SliderW*, void*);
  static void cb_vo_level(SliderW*, void*);
public:
  NewVum *vu_vu;
};
#endif
