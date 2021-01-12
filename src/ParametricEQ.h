/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

class ParametricEQ :public EQ
{
public:
    ParametricEQ (eq_type type, double sample_frequency, uint32_t intermediate_bufsize);
    void setpreset(int npreset);
    
    void initialize(eq_type type);

private:

};

#endif /* PARAMETRICEQ_H */

