/*
  Rakarrack   Audio FX software
  FPreset.h - Internal Preset Reader
  by Josep Andreu

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

#ifndef FPRESET_H
#define FPRESET_H

#include "../global.h"

class FPreset
{
public:

    FPreset();
    ~FPreset();
    void ReadPreset(int eff, int num, int pdata[], char *filename = NULL);
};


#endif
