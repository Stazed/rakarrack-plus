/*
  RKR_Input.h

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

#ifndef RKR_Input_H
#define RKR_Input_H

#include <FL/Fl_Input.H>


class FL_EXPORT RKR_Input : public Fl_Input
{
protected:
public:
    int handle(int);
    RKR_Input(int,int,int,int,const char * = 0);
};

#endif 

