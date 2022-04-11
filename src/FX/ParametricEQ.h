/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version 2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

/* 
 * File:   ParametricEQ.h
 * Author: sspresto
 *
 * Created on January 11, 2021, 7:27 PM
 */

#ifndef PARAMETRICEQ_H
#define PARAMETRICEQ_H

#include "EQ.h"

#define EQPLV2_URI "https://github.com/Stazed/rakarrack-plus#eqp"

const int C_PARAMETRIC_EQ_PARAMETERS = 10;

enum ParametricEQ_Index
{
    Parametric_Low_Freq = 0,
    Parametric_Low_Gain,
    Parametric_Low_Q,
    Parametric_Mid_Freq,
    Parametric_Mid_Gain,
    Parametric_Mid_Q,
    Parametric_High_Freq,
    Parametric_High_Gain,
    Parametric_High_Q,
    Parametric_Gain         // 9
};


class ParametricEQ :public EQ
{
public:
    ParametricEQ (double sample_frequency, uint32_t intermediate_bufsize);
    void setpreset(int npreset);
    void changepar (int npar, int value);
    int getpar (int npar);
    int get_number_efx_parameters () {return C_PARAMETRIC_EQ_PARAMETERS;};
    void set_random_parameters();
    
    void initialize();

    virtual void LV2_parameters(std::string &s_buf);
    virtual std::string get_URI() { return EQPLV2_URI; };

private:

};

#endif /* PARAMETRICEQ_H */

