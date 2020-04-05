/*
  inputW.h

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
 * File:   inputW.h
 * Author: sspresto
 *
 * Created on March 30, 2020, 7:39 PM
 */

#ifndef INPUTW_H
#define INPUTW_H

#include <FL/Fl.H>
#include "RKR_Value_Input.H"

class inputW : public RKR_Value_Input {
public:
    inputW(int x, int y, int w, int h, const char *label = 0);
    int handle(int event);
private:

};

#endif /* INPUTW_H */

