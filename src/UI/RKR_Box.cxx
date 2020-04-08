/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RKR_Box.cxx
 * Author: sspresto
 * 
 * Created on April 8, 2020, 12:29 PM
 */

#include "RKR_Box.h"
#include "../global.h"

RKR_Box::RKR_Box(int X, int Y, int W, int H, const char *label) : Fl_Box(X, Y, W, H, label)
{
    m_need_font_update = true;
    m_start_height = H;
    this->user_data((void*)(BOX_USER_DATA));
}

void RKR_Box::draw()
{
    if(m_need_font_update)
    {
        m_need_font_update = false;
        font_resize(x(), y(), w(), h());
    }

    draw_box();
    draw_label();
}

void RKR_Box::font_resize(int X, int Y, int W, int H)
{
    float H_ratio = (float) H / m_start_height;
    float ratio = (float) (g_value_font_size * H_ratio);
    int H_size = ratio ;
    labelsize(H_size);
}

void RKR_Box::resize(int X, int Y, int W, int H)
{
    /* Resize the text and labels */
    font_resize(X, Y, W, H);

    Fl_Box::resize(X, Y, W, H);

}
