/*
  rkrlv2.h

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

#ifndef RKRLV2_H
#define RKRLV2_H

#include <stdint.h> // uint32_t etc...

enum RKRLV2_ports_
{
    INL = 0,
    INR,
    OUTL,
    OUTR,
    BYPASS,
    PARAM0,
    PARAM1,
    PARAM2,
    PARAM3,
    PARAM4,
    PARAM5,
    PARAM6,
    PARAM7,
    PARAM8,
    PARAM9,
    PARAM10,
    PARAM11,
    PARAM12,
    PARAM13,
    PARAM14,
    PARAM15,
    PARAM16,
    PARAM17,
    PARAM18,
    DBG,
    EXTRA
};


//just make sure these line up with the order in manifest.ttl
enum RKRLV2_effects_
{
    IEQ = 0,
    ICOMP,
    IDIST,
    IECHO,
    ICHORUS,
    IAPHASE,
    IHARM_NM,
    IEXCITER,
    IPAN,
    IAWAH,
    IREV,//10
    IEQP,
    ICAB,
    IMDEL,
    IWAH,
    IDERE,
    IVALVE,
    IDFLANGE,
    IRING,
    IDISTBAND,
    IARPIE,//20
    IEXPAND,
    ISHUFF,
    ISYNTH,
    IVBAND,
    IMUTRO,
    IECHOVERSE,
    ICOIL,
    ISHELF,
    IVOC,
    ISUS,//30
    ISEQ,
    ISHIFT,
    ISTOMP,
    IREVTRON,
    IECHOTRON,
    ISHARM_NM,
    IMBCOMP,
    IOPTTREM,
    IVIBE,
    IINF,//40
    IPHASE,
    IGATE,
    IMIDIC,
    ICONVO,
    IFLANGE,
    IOVERDRIVE,
    IHARM,
    ISTEREOHARM,
    IRESSOLUTION
};

struct _RKRLV2; // forward declaration

void forge_midimessage(_RKRLV2* plug,
		uint32_t tme,
		const uint8_t* const buffer,
		uint32_t size);

#endif
