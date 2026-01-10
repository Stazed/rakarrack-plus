/*

  Resample.C  -  Class
  Using Erik de Castro Lopo libsamplerate
  Copyright (C) 2008-2009 Josep Andreu (Holborn)
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <algorithm>
#include "Resample.h"

#ifdef ZITA_SUPPORT
static inline double dabs(double x) { return x < 0.0 ? -x : x; }

// Debug logging / asserts for unexpected ratios.
// Enabled in debug builds (no NDEBUG) or if RESAMPLE_DEBUG is defined.
#if !defined(NDEBUG) || defined(RESAMPLE_DEBUG)
  #define RESAMPLE_ZITA_DEBUG 1
#else
  #define RESAMPLE_ZITA_DEBUG 0
#endif

// “Hard” epsilon to handle tiny FP noise; real tolerance is frame-size based.
static const double kHardEps = 1e-9;

// Map libsamplerate "type" to zita-resampler filter half-length (hlen).
static unsigned int
zita_hlen_from_type(int type)
{
    switch (type)
    {
        case SRC_SINC_BEST_QUALITY:   return 96;
        case SRC_SINC_MEDIUM_QUALITY: return 48;
        case SRC_SINC_FASTEST:        return 32;
        case SRC_ZERO_ORDER_HOLD:     return 16;
        case SRC_LINEAR:              return 24;
        default:                      return 48;
    }
}

// Standard sample rates to include for “snap-to-SR-ratio”.
static const double kRates[] =
{
    4000.0,
    8000.0,
    12000.0,
    16000.0,
    22050.0,
    32000.0,
    44100.0,
    48000.0,
    88200.0,
    96000.0,
    176400.0,
    192000.0
};
static const int kNumRates = (int)(sizeof(kRates) / sizeof(kRates[0]));

std::vector<double>
Resample::_build_std_ratios()
{
    // Build all directional ratios from the supported SR list.
    // This is only used to *snap* “period-quantized” ratios to their intended SR ratios.
    std::vector<double> r;
    r.reserve((size_t)kNumRates * (size_t)(kNumRates - 1));
    for (int i = 0; i < kNumRates; ++i)
    {
        for (int j = 0; j < kNumRates; ++j)
        {
            if (i == j) continue;
            r.push_back(kRates[j] / kRates[i]);
        }
    }

    std::sort(r.begin(), r.end());
    std::vector<double> u;
    u.reserve(r.size());
    for (size_t i = 0; i < r.size(); ++i)
    {
        if (u.empty() || dabs(u.back() - r[i]) > kHardEps)
            u.push_back(r[i]);
    }
    return u;
}

double
Resample::_ratio_tol_from_frames(int frames)
{
    // Your callers often do:
    //   o_frames = lrint(frames * true_ratio)
    //   ratio_used = o_frames / frames
    //
    // The quantization error vs true_ratio is bounded by about 0.5/frames.
    // Use a slightly generous tolerance so we can snap to the intended SR ratio.
    if (frames <= 0) return 1e-3;
    const double q = 0.5 / (double)frames;
    // Make it a bit forgiving + include a hard epsilon floor.
    const double tol = std::max(4.0 * q, 1e-6);
    return std::max(tol, kHardEps);
}

Resample::ZitaPair*
Resample::_find_existing(double ratio, double tol) const
{
    for (size_t i = 0; i < _pairs.size(); ++i)
    {
        ZitaPair* p = _pairs[i];
        if (!p || !p->ok) continue;
        if (dabs(p->ratio - ratio) <= tol)
            return p;
    }
    return NULL;
}

void
Resample::_reset_pair(ZitaPair* p)
{
    if (!p) return;
    p->l.reset();
    p->r.reset();
}

Resample::ZitaPair*
Resample::_get_or_create(double ratio)
{
    // We only create pairs before RT starts.
    // After _rt_started is true, we will only return an existing pair.

    std::lock_guard<std::mutex> g(_pool_mtx);

    // Return existing if already created (within hard eps).
    if (ZitaPair* e = _find_existing(ratio, kHardEps))
        return e;

    if (_rt_started)
        return NULL; // no setup allowed once RT started

    // Create lazily (non-RT only).
    ZitaPair* np = new ZitaPair(ratio);
    const int el = np->l.setup(ratio, 1, _z_hlen);
    const int er = np->r.setup(ratio, 1, _z_hlen);
    np->ok = (el == 0 && er == 0);
    if (!np->ok)
    {
        delete np;
        return NULL;
    }
    _pairs.push_back(np);
    return np;
}
#endif


Resample::Resample(int type) :
    srcinfor(),
    srcinfol(),
    errorl(),
    errorr(),
#ifdef ZITA_SUPPORT
    _z_hlen(zita_hlen_from_type(type)),
    _std_ratios(_build_std_ratios()),
    _pairs(),
    _cur(NULL),
    _rt_started(false),
    _pool_mtx()
#else
    statel(NULL),
    stater(NULL)
#endif
{
#ifdef ZITA_SUPPORT
    // Lazy pool: no setup() here.
#else
    statel = src_new(type, 1, &errorl);
    stater = src_new(type, 1, &errorr);
#endif
}

Resample::~Resample()
{
#ifdef ZITA_SUPPORT
    std::lock_guard<std::mutex> g(_pool_mtx);
    for (size_t i = 0; i < _pairs.size(); ++i)
    {
        if (_pairs[i])
        {
            _pairs[i]->l.clear();
            _pairs[i]->r.clear();
            delete _pairs[i];
        }
    }
    _pairs.clear();
    _cur = NULL;
#else
    if(statel)
    {
        src_delete(statel);
    }
    
    if(stater)
    {
        src_delete(stater);
    }
#endif
}

void
Resample::cleanup()
{
#ifdef ZITA_SUPPORT
    if (_cur)
        _reset_pair(_cur);
#else
    if(statel)
    {
        src_reset(statel);
    }
    
    if(stater)
    {
        src_reset(stater);
    }
#endif
}

void
Resample::out(float *inl, float *inr, float *outl, float *outr, int frames, double ratio)
{
#ifdef ZITA_SUPPORT
    if (frames <= 0)
        return;

    // Keep existing behavior: compute requested output frames from passed ratio.
    long int o_frames = lrint((double) frames * ratio);

    // Fill SRC_DATA for compatibility/diagnostics.
    srcinfol.data_in = inl;
    srcinfol.input_frames = frames;
    srcinfol.data_out = outl;
    srcinfol.output_frames = o_frames;
    srcinfol.src_ratio = ratio;
    srcinfol.end_of_input = 0;
    srcinfol.input_frames_used = 0;
    srcinfol.output_frames_gen = 0;

    srcinfor.data_in = inr;
    srcinfor.input_frames = frames;
    srcinfor.data_out = outr;
    srcinfor.output_frames = o_frames;
    srcinfor.src_ratio = ratio;
    srcinfor.end_of_input = 0;
    srcinfor.input_frames_used = 0;
    srcinfor.output_frames_gen = 0;

    // IMPORTANT:
    // Your callers often size output buffers from integer periods:
    //   o_frames = lrint(frames * ratio) and then pass ratio ~ o_frames/frames.
    // To avoid discontinuities, always use the exact buffer-consistent ratio:
    const double use_ratio = (double)o_frames / (double)frames;

    // First, try to use an existing pair (no setup).
    const double tol = _ratio_tol_from_frames(frames);
    ZitaPair* p = _find_existing(use_ratio, tol);

    // If none exists yet, create it only if RT hasn't started.
    if (!p)
        p = _get_or_create(use_ratio);

    if (!p)
    {
        errorl = -1;
        errorr = -1;

#if RESAMPLE_ZITA_DEBUG
        fprintf(stderr,
                "Resample(ZITA): ratio unavailable: in_ratio=%.15g quant=%.15g use=%.15g "
                "(frames=%d o_frames=%ld rt_started=%d)\n",
                ratio, (double)o_frames/(double)frames, use_ratio, frames, (long)o_frames, _rt_started ? 1 : 0);
        assert(!"Resample(ZITA): ratio not available (needs prewarm before RT started)");
#endif
        return;
    }

    // Mark RT started after first successful selection. From now on, no setup() is allowed.
    _rt_started = true;

    if (_cur != p)
    {
        _cur = p;
        _reset_pair(_cur);
    }

    // Left
    _cur->l.inp_data  = inl;
    _cur->l.inp_count = (unsigned int)frames;
    _cur->l.out_data  = outl;
    _cur->l.out_count = (unsigned int)o_frames;
    errorl = _cur->l.process();
    const long l_gen = o_frames - (long)_cur->l.out_count;

    // Right
    _cur->r.inp_data  = inr;
    _cur->r.inp_count = (unsigned int)frames;
    _cur->r.out_data  = outr;
    _cur->r.out_count = (unsigned int)o_frames;
    errorr = _cur->r.process();
    const long r_gen = o_frames - (long)_cur->r.out_count;

    srcinfol.input_frames_used  = frames - (long)_cur->l.inp_count;
    srcinfol.output_frames_gen  = l_gen;
    srcinfor.input_frames_used  = frames - (long)_cur->r.inp_count;
    srcinfor.output_frames_gen  = r_gen;

    // zita can legitimately generate fewer than requested frames at startup.
    // Your callers usually assume the full buffer is valid; zero-fill remainder to avoid garbage/static.
    if (l_gen < o_frames && l_gen >= 0)
        memset(outl + l_gen, 0, (size_t)(o_frames - l_gen) * sizeof(float));
    if (r_gen < o_frames && r_gen >= 0)
        memset(outr + r_gen, 0, (size_t)(o_frames - r_gen) * sizeof(float));
#else
    if(!statel)
        return;
    if(!stater)
        return;
    
    long int o_frames = lrint((double) frames * ratio);
    srcinfol.data_in = inl;
    srcinfol.input_frames = frames;
    srcinfol.data_out = outl;
    srcinfol.output_frames = o_frames;
    srcinfol.src_ratio = ratio;
    srcinfol.end_of_input = 0;

    srcinfor.data_in = inr;
    srcinfor.input_frames = frames;
    srcinfor.data_out = outr;
    srcinfor.output_frames = o_frames;
    srcinfor.src_ratio = ratio;
    srcinfor.end_of_input = 0;

    errorl = src_process(statel, &srcinfol);
    errorr = src_process(stater, &srcinfor);
#endif
}

void
Resample::mono_out(float *inl, float *outl, int frames, double ratio, int o_frames)
{
#ifdef ZITA_SUPPORT
    if (frames <= 0 || o_frames < 0)
        return;

    srcinfol.data_in = inl;
    srcinfol.input_frames = frames;
    srcinfol.data_out = outl;
    srcinfol.output_frames = o_frames;
    srcinfol.src_ratio = ratio;
    srcinfol.end_of_input = 0;
    srcinfol.input_frames_used = 0;
    srcinfol.output_frames_gen = 0;

    // For mono_out, o_frames is explicit; use the exact buffer-consistent ratio.
    const double use_ratio = (double)o_frames / (double)frames;

    const double tol = _ratio_tol_from_frames(frames);
    ZitaPair* p = _find_existing(use_ratio, tol);
    if (!p)
        p = _get_or_create(use_ratio);

    if (!p)
    {
        errorl = -1;

#if RESAMPLE_ZITA_DEBUG
        fprintf(stderr,
                "Resample(ZITA): mono ratio unavailable: in_ratio=%.15g quant=%.15g use=%.15g "
                "(frames=%d o_frames=%d rt_started=%d)\n",
                ratio, (double)o_frames/(double)frames, use_ratio, frames, o_frames, _rt_started ? 1 : 0);
        assert(!"Resample(ZITA): mono ratio not available (needs prewarm before RT started)");
#endif
        return;
    }

    _rt_started = true;

    if (_cur != p)
    {
        _cur = p;
        _reset_pair(_cur);
    }

    _cur->l.inp_data  = inl;
    _cur->l.inp_count = (unsigned int)frames;
    _cur->l.out_data  = outl;
    _cur->l.out_count = (unsigned int)o_frames;
    errorl = _cur->l.process();
    const long gen = o_frames - (long)_cur->l.out_count;

    srcinfol.input_frames_used = frames - (long)_cur->l.inp_count;
    srcinfol.output_frames_gen = gen;

    // Zero-fill remainder to avoid garbage/static if fewer frames were generated.
    if (gen < o_frames && gen >= 0)
        memset(outl + gen, 0, (size_t)(o_frames - gen) * sizeof(float));

#else
    if(!statel)
        return;
    
    srcinfol.data_in = inl;
    srcinfol.input_frames = frames;
    srcinfol.data_out = outl;
    srcinfol.output_frames = o_frames;
    srcinfol.src_ratio = ratio;
    srcinfol.end_of_input = 0;

    errorl = src_process(statel, &srcinfol);
#endif
}




