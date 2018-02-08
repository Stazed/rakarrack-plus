// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef shuffle_gui_h
#define shuffle_gui_h
#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include "sliderW.h"
#include "../process.h"
#include "rakarrack.h"
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Check_Button.H>

class ShuffleGui : public Fl_Group {
public:
  ShuffleGui(int X, int Y, int W, int H, const char *L = 0);
  Fl_Light_Button *shuffle_activar;
private:
  inline void cb_shuffle_activar_i(Fl_Light_Button*, void*);
  static void cb_shuffle_activar(Fl_Light_Button*, void*);
public:
  Fl_Choice *shuffle_preset;
private:
  inline void cb_shuffle_preset_i(Fl_Choice*, void*);
  static void cb_shuffle_preset(Fl_Choice*, void*);
  static Fl_Menu_Item menu_shuffle_preset[];
public:
  SliderW *shuffle_WD;
private:
  inline void cb_shuffle_WD_i(SliderW*, void*);
  static void cb_shuffle_WD(SliderW*, void*);
public:
  SliderW *shuffle_cross1;
private:
  inline void cb_shuffle_cross1_i(SliderW*, void*);
  static void cb_shuffle_cross1(SliderW*, void*);
public:
  SliderW *shuffle_volL;
private:
  inline void cb_shuffle_volL_i(SliderW*, void*);
  static void cb_shuffle_volL(SliderW*, void*);
public:
  SliderW *shuffle_cross2;
private:
  inline void cb_shuffle_cross2_i(SliderW*, void*);
  static void cb_shuffle_cross2(SliderW*, void*);
public:
  SliderW *shuffle_volML;
private:
  inline void cb_shuffle_volML_i(SliderW*, void*);
  static void cb_shuffle_volML(SliderW*, void*);
public:
  SliderW *shuffle_cross3;
private:
  inline void cb_shuffle_cross3_i(SliderW*, void*);
  static void cb_shuffle_cross3(SliderW*, void*);
public:
  SliderW *shuffle_volMH;
private:
  inline void cb_shuffle_volMH_i(SliderW*, void*);
  static void cb_shuffle_volMH(SliderW*, void*);
public:
  SliderW *shuffle_cross4;
private:
  inline void cb_shuffle_cross4_i(SliderW*, void*);
  static void cb_shuffle_cross4(SliderW*, void*);
public:
  SliderW *shuffle_volH;
private:
  inline void cb_shuffle_volH_i(SliderW*, void*);
  static void cb_shuffle_volH(SliderW*, void*);
public:
  SliderW *shuffle_Q;
private:
  inline void cb_shuffle_Q_i(SliderW*, void*);
  static void cb_shuffle_Q(SliderW*, void*);
public:
  Fl_Check_Button *shuffle_E;
private:
  inline void cb_shuffle_E_i(Fl_Check_Button*, void*);
  static void cb_shuffle_E(Fl_Check_Button*, void*);
};
#endif