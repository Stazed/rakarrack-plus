// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef bankwindow_gui_h
#define bankwindow_gui_h
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include "rakarrack.h"
#include <FL/Fl_Box.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Group.H>

class BankWindowGui : public Fl_Window {
  void _BankWindowGui();
public:
  BankWindowGui(int X, int Y, int W, int H, const char *L = 0);
  BankWindowGui(int W, int H, const char *L = 0);
  BankWindowGui();
  Fl_Box *Fondo3;
  Fl_Menu_Bar *MenuB;
  static Fl_Menu_Item menu_MenuB[];
  static Fl_Menu_Item *ArchivoB;
  static Fl_Menu_Item *NewB;
private:
  inline void cb_NewB_i(Fl_Menu_*, void*);
  static void cb_NewB(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Load_Bank;
private:
  inline void cb_Load_Bank_i(Fl_Menu_*, void*);
  static void cb_Load_Bank(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Save_Bank;
private:
  inline void cb_Save_Bank_i(Fl_Menu_*, void*);
  static void cb_Save_Bank(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Convert_Old_Bank;
private:
  inline void cb_Convert_Old_Bank_i(Fl_Menu_*, void*);
  static void cb_Convert_Old_Bank(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *salirB;
private:
  inline void cb_salirB_i(Fl_Menu_*, void*);
  static void cb_salirB(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *AyudaB;
  static Fl_Menu_Item *ContenidoB;
private:
  inline void cb_ContenidoB_i(Fl_Menu_*, void*);
  static void cb_ContenidoB(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Acerca_deB;
private:
  inline void cb_Acerca_deB_i(Fl_Menu_*, void*);
  static void cb_Acerca_deB(Fl_Menu_*, void*);
public:
  Fl_Button *B_B1;
private:
  inline void cb_B_B1_i(Fl_Button*, void*);
  static void cb_B_B1(Fl_Button*, void*);
public:
  Fl_Button *B_B2;
private:
  inline void cb_B_B2_i(Fl_Button*, void*);
  static void cb_B_B2(Fl_Button*, void*);
public:
  Fl_Button *B_B3;
private:
  inline void cb_B_B3_i(Fl_Button*, void*);
  static void cb_B_B3(Fl_Button*, void*);
public:
  Fl_Button *B_B4;
private:
  inline void cb_B_B4_i(Fl_Button*, void*);
  static void cb_B_B4(Fl_Button*, void*);
public:
  Fl_Choice *CH_UB;
private:
  inline void cb_CH_UB_i(Fl_Choice*, void*);
  static void cb_CH_UB(Fl_Choice*, void*);
public:
  Fl_Group *ob;
  void set_bank_CH_UB(char* nombre, char* nombank);
  void make_window_banks();
  void light_preset(int npreset);
  void unlight_preset(int npreset);
};
#endif
