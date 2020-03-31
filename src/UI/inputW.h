/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   inputW.h
 * Author: sspresto
 *
 * Created on March 30, 2020, 7:39 PM
 */

#ifndef INPUTW_H
#define INPUTW_H

#include <FL/Fl.H>
#include <FL/Fl_Value_Input.H>

class inputW : public Fl_Value_Input {
public:
    inputW(int x, int y, int w, int h, const char *label = 0);
    int handle(int event);
private:

};

#endif /* INPUTW_H */

