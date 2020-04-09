/*
  RKR_Check_Button.h

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
 * File:   RKR_Check_Button.h
 * Author: sspresto
 *
 * Created on April 9, 2020, 1:06 PM
 */

#ifndef RKR_CHECK_BUTTON_H
#define RKR_CHECK_BUTTON_H

#include "RKR_Light_Button.h"

class RKR_Check_Button : public RKR_Light_Button
{
public:
    RKR_Check_Button(int X, int Y, int W, int H, const char *label=0);
private:

};

#endif /* RKR_CHECK_BUTTON_H */

