

#ifndef RKR_Input_H
#define RKR_Input_H

#include <FL/Fl_Input.H>


class FL_EXPORT RKR_Input : public Fl_Input {


protected:
//  void draw();
public:
  int handle(int);
  RKR_Input(int,int,int,int,const char * = 0);
};

#endif 

