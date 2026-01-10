/*
    Resample - Resample

    Resample.h  - headers.
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
 (version2)  along with this program; if not, write to the Free Software
 Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

*/

#ifndef RESAMPLE_H
#define RESAMPLE_H

#include "../global.h"

#ifdef ZITA_SUPPORT
  // zita-resampler (e.g. v1.11.2)
  // Common Linux include path:
  //   /usr/include/zita-resampler/vresampler.h
  #include <zita-resampler/vresampler.h>
  #include <vector>
  #include <mutex>

  // Keep the public API intact: the class exposes SRC_DATA publicly.
  // Provide a compatible SRC_DATA when building without libsamplerate.
  typedef struct
  {
      float *data_in;
      float *data_out;
      long  input_frames;
      long  output_frames;
      long  input_frames_used;
      long  output_frames_gen;
      int   end_of_input;
      double src_ratio;
  } SRC_DATA;

  // Keep the documented "type" values usable by callers (same numeric values).
  enum
  {
      SRC_SINC_BEST_QUALITY   = 0,
      SRC_SINC_MEDIUM_QUALITY = 1,
      SRC_SINC_FASTEST        = 2,
      SRC_ZERO_ORDER_HOLD     = 3,
      SRC_LINEAR              = 4
  };
#else
  #include <samplerate.h>
#endif



class Resample
{
public:
    explicit Resample(int type);
    /*
    Types:
              SRC_SINC_BEST_QUALITY       = 0,
              SRC_SINC_MEDIUM_QUALITY     = 1,
              SRC_SINC_FASTEST            = 2,
              SRC_ZERO_ORDER_HOLD         = 3,
              SRC_LINEAR                  = 4
    */

    ~Resample();
    void cleanup();
    void out(float *inl, float *inr, float *outl, float *outr, int frames, double ratio);
    void mono_out(float *inl, float *outl, int frames, double ratio, int o_frames);

    // ratio Equal to output_sample_rate / input_sample_rate.


    SRC_DATA srcinfor;
    SRC_DATA srcinfol;


private:

    int errorl,errorr;

#ifdef ZITA_SUPPORT
    struct ZitaPair
    {
        double ratio;
        VResampler l;
        VResampler r;
        bool ok;
        ZitaPair(double ra) : ratio(ra), l(), r(), ok(false) {}
    };

    // quality mapping
    unsigned int _z_hlen;

    // “standard SR” ratios to snap to (derived from supported rates list)
    std::vector<double> _std_ratios;

    // lazily created resamplers (created only before RT starts)
    std::vector<ZitaPair*> _pairs;
    ZitaPair* _cur;

    // once true, we will never call setup() again (prevents setup on audio thread)
    bool _rt_started;

    // protects pool creation (intended for non-RT only)
    mutable std::mutex _pool_mtx;

    static std::vector<double> _build_std_ratios();
    static double _ratio_tol_from_frames(int frames);
    double _snap_to_std_ratio(double ratio, int frames) const;

    ZitaPair* _find_existing(double ratio, double tol) const;
    ZitaPair* _get_or_create(double ratio);
    void _reset_pair(ZitaPair* p);
#else
     SRC_STATE *statel;
     SRC_STATE *stater;
#endif
};

#endif
