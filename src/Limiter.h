/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Limiter.h
 * Author: sspresto
 *
 * Created on January 29, 2021, 10:02 PM
 */

#ifndef LIMITER_H
#define LIMITER_H

#include "Compressor.h"

class Limiter : public Compressor
{
public:
    Limiter (double sample_rate, uint32_t intermediate_bufsize);
    
    void setpreset (int npreset);

};

#endif /* LIMITER_H */

