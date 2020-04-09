/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RKR_Box.h
 * Author: sspresto
 *
 * Created on April 8, 2020, 12:29 PM
 */

#ifndef RKR_BOX_H
#define RKR_BOX_H

#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#define BOX_USER_DATA   99997
#define BOX_LED_DATA    5

class RKR_Box : public Fl_Box {
public:
    RKR_Box(int X, int Y, int W, int H, const char *label=0);
    void draw();
    void resize(int,int,int,int);
    void set_font_adjustment(){m_need_font_update = true;}
    int m_start_height;
    int m_start_font_offset;
private:
    
    bool m_need_font_update;
    void font_resize(int,int,int,int);

};

#endif /* RKR_BOX_H */

