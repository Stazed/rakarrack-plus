// generated by Fast Light User Interface Designer (fluid) version 1.0304

#ifndef rakarrack_h
#define rakarrack_h
#include <FL/Fl.H>
#include "../process.h"
#include "../global.h"
#include "RKR_Input.h"
#include "RKR_Slider.h"
#include "newvum.h"
#include "tunerbar.h"
#include "RKR_Box.h"
#include "RKR_Button.h"
#include "RKR_Choice.h"
#include "RKR_Counter.h"
#include "RKR_Light_Button.h"
#include "RKR_Menu_Bar.h"
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Tooltip.H>
#include <FL/x.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Tiled_Image.H>
#include <FL/Fl_Help_Dialog.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Color_Chooser.H>
#include <X11/xpm.h>
class RKR;  // forward declaration
class Scope;  // forward declaration
class NewVum;  // forward declaration
class Analyzer;  // forward declaration
class TunerBar;  // forward declaration
class EqGui; // forward declaration
class CompressGui; // forward declaration
class DistGui; // forward declaration
class OvrdGui; // forward declaration
class EchoGui; // forward declaration
class ChorusGui; // forward declaration
class PhaserGui; // forward declaration
class FlangerGui; // forward declaration
class ReverbGui; // forward declaration
class PeqGui; // forward declaration
class WahwahGui; // forward declaration
class AlienwahGui; // forward declaration
class CabinetGui; // forward declaration
class PanGui; // forward declaration
class HarGui; // forward declaration
class MusdelayGui; // forward declaration
class GateGui; // forward declaration
class DerelictGui; // forward declaration
class AphaserGui; // forward declaration
class ValveGui; // forward declaration
class DflangeGui; // forward declaration
class RingGui; // forward declaration
class ExciterGui; // forward declaration
class DistBandGui; // forward declaration
class ArpieGui; // forward declaration
class ExpanderGui; // forward declaration
class ShuffleGui; // forward declaration
class SynthfilterGui; // forward declaration
class VarybandGui; // forward declaration
class ConvoGui; // forward declaration
class LooperGui; // forward declaration
class MutromojoGui; // forward declaration
class EchoverseGui; // forward declaration
class CoilGui; // forward declaration
class ShelfGui; // forward declaration
class VocoderGui; // forward declaration
class SustainGui; // forward declaration
class SequenceGui; // forward declaration
class ShifterGui; // forward declaration
class StompboxGui; // forward declaration
class RevtronGui; // forward declaration
class EchotronGui; // forward declaration
class SharGui; // forward declaration
class CbandGui; // forward declaration
class OtremGui; // forward declaration
class VibeGui; // forward declaration
class InfinityGui; // forward declaration
class MidiGui; // forward declaration
class BankWindowGui; // forward declaration
class OrderWindowGui; // forward declaration
class SettingsWindowGui; // forward declaration
class MidiLearnWindowGui; // forward declaration
class AboutWindowGui; // forward declaration
class TrigWindowGui; // forward declaration
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include "scope.h"
#include "eq_gui.h"
#include "compress_gui.h"
#include "dist_gui.h"
#include "ovrd_gui.h"
#include "echo_gui.h"
#include "chorus_gui.h"
#include "phaser_gui.h"
#include "flanger_gui.h"
#include "reverb_gui.h"
#include "peq_gui.h"
#include "wahwah_gui.h"
#include "alienwah_gui.h"
#include "cabinet_gui.h"
#include "pan_gui.h"
#include "har_gui.h"
#include "musdelay_gui.h"
#include "gate_gui.h"
#include "derelict_gui.h"
#include "aphaser_gui.h"
#include "valve_gui.h"
#include "dflange_gui.h"
#include "ring_gui.h"
#include "exciter_gui.h"
#include "distband_gui.h"
#include "arpie_gui.h"
#include "expander_gui.h"
#include "shuffle_gui.h"
#include "synthfilter_gui.h"
#include "varyband_gui.h"
#include "convo_gui.h"
#include "looper_gui.h"
#include "mutromojo_gui.h"
#include "echoverse_gui.h"
#include "coil_gui.h"
#include "shelf_gui.h"
#include "vocoder_gui.h"
#include "sustain_gui.h"
#include "sequence_gui.h"
#include "shifter_gui.h"
#include "stompbox_gui.h"
#include "reverbtron_gui.h"
#include "echotron_gui.h"
#include "stereoharm_gui.h"
#include "compband_gui.h"
#include "otrem_gui.h"
#include "vibe_gui.h"
#include "infinity_gui.h"
#include <FL/Fl_Group.H>
#include "midiconv_gui.h"
#include "analyzer.h"
#include "bankwindow_gui.h"
#include "orderwindow_gui.h"
#include "settingswindow_gui.h"
#include "midilearnwindow_gui.h"
#include "aboutwindow_gui.h"
#include "trigwindow_gui.h"

class RKRGUI {
  RKR *m_process; 
  /**
   Efx_Gui_Base:
   Array to hold pointer to the rack effect base class gui.
   This is used for common effect actions to allow use in
   for() loops to eliminate redundant code.
   Array is indexed by EFX_Index.
  */
  RKR_Gui_Effect *Efx_Gui_Base[C_NUMBER_EFFECTS]; 
  CommonGuiMenu *m_looper_bar; 
  CommonGuiMenu *m_looper_ms; 
  int made; 
  Fl_Help_Dialog *visor; 
  struct list_element *mBankNameList;
  struct list_element *mBankNameListTail;
public:
  void make_window();
  Fl_Double_Window *Principal;
private:
  inline void cb_Principal_i(Fl_Double_Window*, void*);
  static void cb_Principal(Fl_Double_Window*, void*);
public:
  Fl_Box *fondo;
  RKR_Menu_Bar *MenuP;
  static Fl_Menu_Item menu_MenuP[];
  static Fl_Menu_Item *Archivo;
  static Fl_Menu_Item *Menu_New;
private:
  inline void cb_Menu_New_i(Fl_Menu_*, void*);
  static void cb_Menu_New(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Load_Preset;
private:
  inline void cb_Load_Preset_i(Fl_Menu_*, void*);
  static void cb_Load_Preset(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Save_Preset;
private:
  inline void cb_Save_Preset_i(Fl_Menu_*, void*);
  static void cb_Save_Preset(Fl_Menu_*, void*);
  inline void cb_BankWindow_i(Fl_Menu_*, void*);
  static void cb_BankWindow(Fl_Menu_*, void*);
  inline void cb_BankWindow1_i(Fl_Menu_*, void*);
  static void cb_BankWindow1(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Load_Skin;
private:
  inline void cb_Load_Skin_i(Fl_Menu_*, void*);
  static void cb_Load_Skin(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Save_Skin;
private:
  inline void cb_Save_Skin_i(Fl_Menu_*, void*);
  static void cb_Save_Skin(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Load_MTable;
private:
  inline void cb_Load_MTable_i(Fl_Menu_*, void*);
  static void cb_Load_MTable(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Save_MTable;
private:
  inline void cb_Save_MTable_i(Fl_Menu_*, void*);
  static void cb_Save_MTable(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *ConvertReverb;
private:
  inline void cb_ConvertReverb_i(Fl_Menu_*, void*);
  static void cb_ConvertReverb(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *ImportPresets;
private:
  inline void cb_ImportPresets_i(Fl_Menu_*, void*);
  static void cb_ImportPresets(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *salir;
private:
  inline void cb_salir_i(Fl_Menu_*, void*);
  static void cb_salir(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Bank_Menu;
private:
  inline void cb_Bank_Menu_i(Fl_Menu_*, void*);
  static void cb_Bank_Menu(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Ajustes;
private:
  inline void cb_Ajustes_i(Fl_Menu_*, void*);
  static void cb_Ajustes(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *ML_Menu;
private:
  inline void cb_ML_Menu_i(Fl_Menu_*, void*);
  static void cb_ML_Menu(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *ACI_Menu;
private:
  inline void cb_ACI_Menu_i(Fl_Menu_*, void*);
  static void cb_ACI_Menu(Fl_Menu_*, void*);
  inline void cb_Fullscreen_i(Fl_Menu_*, void*);
  static void cb_Fullscreen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Ayuda;
  static Fl_Menu_Item *Contenido;
private:
  inline void cb_Contenido_i(Fl_Menu_*, void*);
  static void cb_Contenido(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *Acerca_de;
private:
  inline void cb_Acerca_de_i(Fl_Menu_*, void*);
  static void cb_Acerca_de(Fl_Menu_*, void*);
public:
  Fl_Button *TITTLE_L;
private:
  inline void cb_TITTLE_L_i(Fl_Button*, void*);
  static void cb_TITTLE_L(Fl_Button*, void*);
public:
  RKR_Box *LMT_LED;
  RKR_Box *CLIP_LED;
  RKR_Box *UPS_LED;
  RKR_Box *PORT_MIDI_IN_STATE;
  RKR_Box *PORT_MIDI_OUT_STATE;
  RKR_Box *PORT_AUX_STATE;
  RKR_Box *PORT_IN_STATE;
  RKR_Box *PORT_OUT_STATE;
  RKR_Box *CPULOAD;
  Scope *Sco;
  Fl_Box *E1;
  Fl_Box *E2;
  Fl_Box *E3;
  Fl_Box *E4;
  Fl_Box *E5;
  Fl_Box *E6;
  Fl_Box *E7;
  Fl_Box *E8;
  Fl_Box *E9;
  Fl_Box *E10;
  RKR_Box *L1;
  RKR_Box *L2;
  RKR_Box *L3;
  RKR_Box *L4;
  RKR_Box *L5;
  RKR_Box *L6;
  RKR_Box *L7;
  RKR_Box *L8;
  RKR_Box *L9;
  RKR_Box *L10;
  EqGui *EQ;
  CompressGui *COMPRESS;
  DistGui *DIST;
  OvrdGui *OVRD;
  EchoGui *ECHO;
  ChorusGui *CHORUS;
  PhaserGui *PHASER;
  FlangerGui *FLANGER;
  ReverbGui *REVERB;
  PeqGui *PEQ;
  WahwahGui *WAHWAH;
  AlienwahGui *ALIENWAH;
  CabinetGui *CABINET;
  PanGui *PAN;
  HarGui *HAR;
  MusdelayGui *MUSDELAY;
  GateGui *GATE;
  DerelictGui *DERELICT;
  AphaserGui *APHASER;
  ValveGui *VALVE;
  DflangeGui *DFLANGE;
  RingGui *RING;
  ExciterGui *EXCITER;
  DistBandGui *DISTBAND;
  ArpieGui *ARPIE;
  ExpanderGui *EXPANDER;
  ShuffleGui *SHUFFLE;
  SynthfilterGui *SYNTHFILTER;
  VarybandGui *VARYBAND;
  ConvoGui *CONVOLOTRON;
  LooperGui *LOOPER;
  MutromojoGui *MUTROMOJO;
  EchoverseGui *ECHOVERSE;
  CoilGui *COILCRAFTER;
  ShelfGui *SHELFBOOST;
  VocoderGui *VOCODER;
  SustainGui *SUSTAINER;
  SequenceGui *SEQUENCE;
  ShifterGui *SHIFTER;
  StompboxGui *STOMPBOX;
  RevtronGui *REVERBTRON;
  EchotronGui *ECHOTRON;
  SharGui *SHAR;
  CbandGui *COMPBAND;
  OtremGui *OTREM;
  VibeGui *VIBE;
  InfinityGui *INFINIT;
  Fl_Group *Tuner;
  RKR_Light_Button *tuner_activar;
private:
  inline void cb_tuner_activar_i(RKR_Light_Button*, void*);
  static void cb_tuner_activar(RKR_Light_Button*, void*);
public:
  TunerBar *tuner_bar;
  RKR_Box *WNote;
  RKR_Box *WRfreq;
  RKR_Box *WNfreq;
  Fl_Box *PINCHO;
  RKR_Box *TUNER_LABEL;
  Fl_Group *InOut;
  RKR_Light_Button *ActivarGeneral;
private:
  inline void cb_ActivarGeneral_i(RKR_Light_Button*, void*);
  static void cb_ActivarGeneral(RKR_Light_Button*, void*);
public:
  RKR_Button *BostBut;
private:
  inline void cb_BostBut_i(RKR_Button*, void*);
  static void cb_BostBut(RKR_Button*, void*);
public:
  RKR_Slider *Balance;
private:
  inline void cb_Balance_i(RKR_Slider*, void*);
  static void cb_Balance(RKR_Slider*, void*);
public:
  RKR_Slider *Nivel_Entrada;
private:
  inline void cb_Nivel_Entrada_i(RKR_Slider*, void*);
  static void cb_Nivel_Entrada(RKR_Slider*, void*);
public:
  RKR_Slider *Nivel_Salida;
private:
  inline void cb_Nivel_Salida_i(RKR_Slider*, void*);
  static void cb_Nivel_Salida(RKR_Slider*, void*);
public:
  NewVum *input_vul;
  NewVum *input_vur;
  NewVum *output_vul;
  NewVum *output_vur;
  RKR_Box *LABEL_IO;
  MidiGui *MIDI;
  Fl_Group *Metro;
  RKR_Light_Button *metro_activar;
private:
  inline void cb_metro_activar_i(RKR_Light_Button*, void*);
  static void cb_metro_activar(RKR_Light_Button*, void*);
public:
  RKR_Choice *MetroBar;
private:
  inline void cb_MetroBar_i(RKR_Choice*, void*);
  static void cb_MetroBar(RKR_Choice*, void*);
public:
  RKR_Slider *Metro_Volume;
private:
  inline void cb_Metro_Volume_i(RKR_Slider*, void*);
  static void cb_Metro_Volume(RKR_Slider*, void*);
public:
  RKR_Choice *MetroSound;
private:
  inline void cb_MetroSound_i(RKR_Choice*, void*);
  static void cb_MetroSound(RKR_Choice*, void*);
public:
  RKR_Slider *Metro_Tempo;
private:
  inline void cb_Metro_Tempo_i(RKR_Slider*, void*);
  static void cb_Metro_Tempo(RKR_Slider*, void*);
public:
  Fl_Box *Metro_Led;
  RKR_Box *Metro_Label;
  Fl_Group *Presets;
  RKR_Box *PRESETS_LABEL;
  RKR_Button *L_B1;
private:
  inline void cb_L_B1_i(RKR_Button*, void*);
  static void cb_L_B1(RKR_Button*, void*);
public:
  RKR_Button *L_B2;
private:
  inline void cb_L_B2_i(RKR_Button*, void*);
  static void cb_L_B2(RKR_Button*, void*);
public:
  RKR_Button *L_B3;
private:
  inline void cb_L_B3_i(RKR_Button*, void*);
  static void cb_L_B3(RKR_Button*, void*);
public:
  RKR_Button *L_B4;
private:
  inline void cb_L_B4_i(RKR_Button*, void*);
  static void cb_L_B4(RKR_Button*, void*);
public:
  RKR_Button *S_new;
private:
  inline void cb_S_new_i(RKR_Button*, void*);
  static void cb_S_new(RKR_Button*, void*);
public:
  RKR_Button *L_preset;
private:
  inline void cb_L_preset_i(RKR_Button*, void*);
  static void cb_L_preset(RKR_Button*, void*);
public:
  RKR_Button *S_preset;
private:
  inline void cb_S_preset_i(RKR_Button*, void*);
  static void cb_S_preset(RKR_Button*, void*);
public:
  RKR_Light_Button *Compare;
private:
  inline void cb_Compare_i(RKR_Light_Button*, void*);
  static void cb_Compare(RKR_Light_Button*, void*);
public:
  RKR_Button *B_preset;
private:
  inline void cb_B_preset_i(RKR_Button*, void*);
  static void cb_B_preset(RKR_Button*, void*);
public:
  RKR_Input *WPreset_Name;
private:
  inline void cb_WPreset_Name_i(RKR_Input*, void*);
  static void cb_WPreset_Name(RKR_Input*, void*);
public:
  RKR_Counter *Preset_Counter;
private:
  inline void cb_Preset_Counter_i(RKR_Counter*, void*);
  static void cb_Preset_Counter(RKR_Counter*, void*);
public:
  RKR_Box *DAuthor;
  RKR_Button *RandomP;
private:
  inline void cb_RandomP_i(RKR_Button*, void*);
  static void cb_RandomP(RKR_Button*, void*);
public:
  RKR_Button *Open_Order;
private:
  inline void cb_Open_Order_i(RKR_Button*, void*);
  static void cb_Open_Order(RKR_Button*, void*);
public:
  RKR_Button *Etit;
private:
  inline void cb_Etit_i(RKR_Button*, void*);
  static void cb_Etit(RKR_Button*, void*);
public:
  Analyzer *Analy;
  RKR_Button *HideUE;
private:
  inline void cb_HideUE_i(RKR_Button*, void*);
  static void cb_HideUE(RKR_Button*, void*);
public:
  RKR_Button *SwitchMod;
private:
  inline void cb_SwitchMod_i(RKR_Button*, void*);
  static void cb_SwitchMod(RKR_Button*, void*);
public:
  Fl_Group *Tap;
  RKR_Light_Button *Tap_activar;
private:
  inline void cb_Tap_activar_i(RKR_Light_Button*, void*);
  static void cb_Tap_activar(RKR_Light_Button*, void*);
public:
  RKR_Choice *T_SEL;
private:
  inline void cb_T_SEL_i(RKR_Choice*, void*);
  static void cb_T_SEL(RKR_Choice*, void*);
  static Fl_Menu_Item menu_T_SEL[];
public:
  RKR_Choice *T_SET;
private:
  inline void cb_T_SET_i(RKR_Choice*, void*);
  static void cb_T_SET(RKR_Choice*, void*);
  static Fl_Menu_Item menu_T_SET[];
public:
  RKR_Button *T_Apply;
private:
  inline void cb_T_Apply_i(RKR_Button*, void*);
  static void cb_T_Apply(RKR_Button*, void*);
public:
  RKR_Button *T_BUT;
private:
  inline void cb_T_BUT_i(RKR_Button*, void*);
  static void cb_T_BUT(RKR_Button*, void*);
public:
  RKR_Box *TAP_LABEL;
  RKR_Value_Input *T_DIS;
private:
  inline void cb_T_DIS_i(RKR_Value_Input*, void*);
  static void cb_T_DIS(RKR_Value_Input*, void*);
public:
  BankWindowGui *BankWindow;
private:
  inline void cb_BankWindow2_i(BankWindowGui*, void*);
  static void cb_BankWindow2(BankWindowGui*, void*);
public:
  OrderWindowGui *Order;
private:
  inline void cb_Order_i(OrderWindowGui*, void*);
  static void cb_Order(OrderWindowGui*, void*);
public:
  SettingsWindowGui *Settings;
private:
  inline void cb_Settings_i(SettingsWindowGui*, void*);
  static void cb_Settings(SettingsWindowGui*, void*);
public:
  MidiLearnWindowGui *MIDILearn;
private:
  inline void cb_MIDILearn_i(MidiLearnWindowGui*, void*);
  static void cb_MIDILearn(MidiLearnWindowGui*, void*);
public:
  AboutWindowGui *AboutWin;
private:
  inline void cb_AboutWin_i(AboutWindowGui*, void*);
  static void cb_AboutWin(AboutWindowGui*, void*);
public:
  TrigWindowGui *Trigger;
private:
  inline void cb_Trigger_i(TrigWindowGui*, void*);
  static void cb_Trigger(TrigWindowGui*, void*);
public:
  RKRGUI(int argc, char**argv,RKR *rkr_);
private:
  static void TimeoutStatic(void* ptr);
  void GuiTimeout(void);
public:
  int increment_look_changed();
  void back_color_change(Fl_Color back_color);
  void label_color_change(Fl_Color label_color);
  void buttons_color_change(Fl_Color buttons_color);
  void leds_color_change(Fl_Color leds_color);
  void font_size_change(int font_size);
  void font_type_change(int font_type);
  void put_icon(Fl_Window* window);
  void load_previous_state();
  void save_current_state(int whati);
  void Put_Loaded();
  static void preset_click(Fl_Button* o, void* v);
  inline void preset_click_i(Fl_Button* o, void*);
  void reordena();
  void show_help();
  void show_lic();
  void MiraClientes();
  void MiraConfig();
  void BankWin_Label(const char *filename);
  void is_modified();
  void Put_Loaded_Bank();
  void Chord(int eff);
  void MIDI_control_gui_refresh();
  void ActOnOff();
  void PutBackground();
  void ChangeActives();
  void findpos(int num, int value, Fl_Widget*);
  void Put_Skin();
  void FillML(/*int type*/);
  void DisAssigns();
  void Prepare_Order();
  void Show_Next_Time();
  void update_looper();
  void update_tap_tempo_GUI();
  void ActACI();
  int Busca_Eff(int num);
  void Fill_Avail(int filter);
  void highlight_and_search_browser();
  int search_but(int x, int y);
  void Scan_Bank_Dir();
private:
  void Set_Bank(std::string directory);
public:
  static int global_shortcuts(int event);
  bool install_signal_handlers();
  static void sigterm_handler(int sig);
  static void check_signals(void *usrPtr);
  void getMIDIControl(int num);
  void PrepareML();
  inline void get_insert_preset_name(Fl_Widget *w, int effect);
  void add_insert_preset_name(Fl_Widget *w, char *name);
  Fl_Widget * find_preset_widget(int effect);
  void read_insert_presets();
  inline void delete_insert_preset(Fl_Widget *w, int effect);
  void Prep_Reorden(int source, int dest);
  static void bank_click(Fl_Choice* o, void* v);
  inline void bank_click_i(Fl_Choice* o, void*);
  static void p_click(Fl_Choice* o, void* v);
  inline void p_click_i(Fl_Choice* o, void*);
  void RandomPreset();
  void drag_effect();
  int AddBankName(const char *name);
  void ClearBankNames();
  char* get_bank_file();
  void set_save_file();
  void set_bank_made(int a_made);
  int get_bank_made();
  void Load_Midi_Program_Change_Table();
  void Save_Midi_Program_Change_Table();
};
#endif
