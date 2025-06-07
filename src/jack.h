/*
  rakarrak - a guitar efects software

  jack.h  -  some jack definitions
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License
  (version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/
#ifndef JACK_OUTPUT_H
#define JACK_OUTPUT_H

#ifndef RKR_PLUS_LV2

#include "global.h"
#include "process.h"


int JACKstart (RKR * rkr_);
int jackprocess(jack_nframes_t nframes, void *arg);
void JACKfinish (RKR * JackOUT);
void jackshutdown (void *arg);
int timebase(jack_transport_state_t state, jack_position_t *pos, void *arg);
void actualiza_tap(double val, RKR * JackOUT);

#endif  // #ifndef RKR_PLUS_LV2
#endif
