/****************************************************************************
 *
 * NAME: smbPitchShift.cpp
 * VERSION: 1.2
 * HOME URL: http://www.dspdimension.com
 * KNOWN BUGS: none
 *
 * SYNOPSIS: Routine for doing pitch shifting while maintaining
 * duration using the Short Time Fourier Transform.
 *
 * DESCRIPTION: The routine takes a pitchShift factor value which is between 0.5
 * (one octave down) and 2. (one octave up). A value of exactly 1 does not change
 * the pitch. numSampsToProcess tells the routine how many samples in indata[0...
 * numSampsToProcess-1] should be pitch shifted and moved to outdata[0 ...
 * numSampsToProcess-1]. The two buffers can be identical (ie. it can process the
 * data in-place). fftFrameSize defines the FFT frame size used for the
 * processing. Typical values are 1024, 2048 and 4096. It may be any value <=
 * MAX_FRAME_LENGTH but it MUST be a power of 2. osamp is the STFT
 * oversampling factor which also determines the overlap between adjacent STFT
 * frames. It should at least be 4 for moderate scaling ratios. A value of 32 is
 * recommended for best quality. sampleRate takes the sample rate for the signal
 * in unit Hz, ie. 44100 for 44.1 kHz audio. The data passed to the routine in
 * indata[] should be in the range [-1.0, 1.0), which is also the output range
 * for the data, make sure you scale the data accordingly (for 16bit signed integers
 * you would have to divide (and multiply) by 32768).
 *
 * COPYRIGHT 1999-2006 Stephan M. Bernsee <smb [AT] dspdimension [DOT] com>
 *
 * 						The Wide Open License (WOL)
 *
 * Permission to use, copy, modify, distribute and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice and this license appear in all source copies.
 * THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY OF
 * ANY KIND. See http://www.dspguru.com/wol.htm for more information.
 *
 *****************************************************************************/

// -----------------------------------------------------------------------------------------------------------------

//void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata)

/*
        Routine smbPitchShift(). See top of file for explanation
        Purpose: doing pitch shifting while maintaining duration using the Short
        Time Fourier Transform.
        Author: (c)1999-2006 Stephan M. Bernsee <smb [AT] dspdimension [DOT] com>
 */
#include "../global.h"
#include "smbPitchShift.h"

#include <pthread.h>

static pthread_mutex_t fftw_planner_lock = PTHREAD_MUTEX_INITIALIZER;

// ============================================================================
// Constructor / Destructor
// ============================================================================

PitchShifter::PitchShifter(long fftFrameSize, long osamp, float sampleRate)
    : gSynFreq(),
      gSynMagn(),
      magn(),
      phase(),
      tmp(),
      real(),
      imag(),
      qpd(),
      index()
{
    fftFrameSize2 = fftFrameSize / 2;
    FS_osamp      = fftFrameSize2 * osamp;
    stepSize      = fftFrameSize / osamp;

    dfftFrameSize       = (double)fftFrameSize;
    coef_dfftFrameSize  = 1.0 / dfftFrameSize;
    coef_dpi            = 1.0 / D_PI;
    coef_mpi            = 1.0 / M_PI;
    dpi_coef            = coef_dfftFrameSize * D_PI;

    freqPerBin = sampleRate / (double)fftFrameSize;
    coefPB     = 1.0 / freqPerBin;
    expct      = D_PI * (double)stepSize / (double)fftFrameSize;

    inFifoLatency = fftFrameSize - stepSize;
    gRover        = inFifoLatency;
    ratio         = 1.0f;

    memset(gInFIFO,        0, MAX_FRAME_LENGTH * sizeof(float));
    memset(gOutFIFO,       0, MAX_FRAME_LENGTH * sizeof(float));
    memset(gFFTworksp,     0, 2 * MAX_FRAME_LENGTH * sizeof(float));
    memset(gLastPhase,     0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
    memset(gSumPhase,      0, (MAX_FRAME_LENGTH / 2 + 1) * sizeof(float));
    memset(gOutputAccum,   0, 2 * MAX_FRAME_LENGTH * sizeof(float));
    memset(gAnaFreq,       0, MAX_FRAME_LENGTH * sizeof(float));
    memset(gAnaMagn,       0, MAX_FRAME_LENGTH * sizeof(float));
    memset(window,         0, MAX_FRAME_LENGTH * sizeof(double));

    makeWindow(fftFrameSize);

#ifdef KISSFFT_SUPPORT
    kiss_cfg_fwd = kiss_fft_alloc(fftFrameSize, 0, nullptr, nullptr);
    kiss_cfg_inv = kiss_fft_alloc(fftFrameSize, 1, nullptr, nullptr);
#else
    int nfftFrameSize = (int)fftFrameSize;

    pthread_mutex_lock(&fftw_planner_lock);
    ftPlanForward = fftw_plan_dft_1d(
        nfftFrameSize, fftw_in, fftw_out, FFTW_FORWARD, FFTW_MEASURE);
    ftPlanInverse = fftw_plan_dft_1d(
        nfftFrameSize, fftw_in, fftw_out, FFTW_BACKWARD, FFTW_MEASURE);
    pthread_mutex_unlock(&fftw_planner_lock);
#endif
}

PitchShifter::~PitchShifter()
{
#ifdef KISSFFT_SUPPORT
    free(kiss_cfg_fwd);
    free(kiss_cfg_inv);
#else
    pthread_mutex_lock(&fftw_planner_lock);
    fftw_destroy_plan(ftPlanForward);
    fftw_destroy_plan(ftPlanInverse);
    pthread_mutex_unlock(&fftw_planner_lock);
#endif
}


// ============================================================================
// Window
// ============================================================================

void PitchShifter::makeWindow(long fftFrameSize)
{
    for (k = 0; k < fftFrameSize; ++k)
    {
        double dk = (double)k;
        window[k] = 0.5 - 0.5 * cos(dpi_coef * dk);
    }
}

// ============================================================================
// Main Pitch Shift Processing
// ============================================================================

void PitchShifter::smbPitchShift(float pitchShift,
                                 long numSampsToProcess,
                                 long fftFrameSize,
                                 long osamp,
                                 float /* sampleRate */,
                                 const float* indata,
                                 float* outdata)
{
    long  i;
    float maxmag = 0.0f;

    for (i = 0; i < numSampsToProcess; ++i)
    {
        gInFIFO[gRover] = indata[i];
        outdata[i]     = gOutFIFO[gRover - inFifoLatency];
        gRover++;

        if (gRover >= fftFrameSize)
        {
            gRover = inFifoLatency;

            // Windowing
            for (k = 0; k < fftFrameSize; ++k)
            {
#ifdef KISSFFT_SUPPORT
                kiss_in[k].r = gInFIFO[k] * window[k];
                kiss_in[k].i = 0.0f;
#else
                fftw_in[k][0] = gInFIFO[k] * window[k];
                fftw_in[k][1] = 0.0;
#endif
            }

            // FFT
#ifdef KISSFFT_SUPPORT
            kiss_fft(kiss_cfg_fwd, kiss_in, kiss_out);
#else
            fftw_execute(ftPlanForward);
#endif

            // Analysis
            for (k = 0; k <= fftFrameSize2; ++k)
            {
                double dk = (double)k;

#ifdef KISSFFT_SUPPORT
                real = kiss_out[k].r;
                imag = kiss_out[k].i;
#else
                real = fftw_out[k][0];
                imag = fftw_out[k][1];
#endif
                magn  = 2.0 * sqrt(real * real + imag * imag);
                phase = atan2(imag, real);

                tmp            = phase - gLastPhase[k];
                gLastPhase[k]  = phase;
                tmp           -= dk * expct;

                qpd = lrint(tmp * coef_mpi);
                if (qpd >= 0) qpd += qpd & 1;
                else          qpd -= qpd & 1;

                tmp -= M_PI * (double)qpd;
                tmp  = osamp * tmp * coef_dpi;
                tmp  = dk * freqPerBin + tmp * freqPerBin;

                gAnaMagn[k] = magn;
                gAnaFreq[k] = tmp;

                if (magn > maxmag)
                    maxmag = magn;
            }

            // Processing
            memset(gSynMagn, 0, fftFrameSize * sizeof(float));
            memset(gSynFreq, 0, fftFrameSize * sizeof(float));

            for (k = 0; k <= fftFrameSize2; ++k)
            {
                index = (long)(k * pitchShift);
                if (index <= fftFrameSize2)
                {
                    gSynMagn[index] += gAnaMagn[k];
                    gSynFreq[index]  = gAnaFreq[k] * pitchShift;
                }
            }

            // Synthesis
            for (k = 0; k <= fftFrameSize2; ++k)
            {
                double dk = (double)k;

                magn = gSynMagn[k];
                tmp  = gSynFreq[k];

                tmp -= dk * freqPerBin;
                tmp *= coefPB;
                tmp  = D_PI * tmp / osamp;
                tmp += dk * expct;

                gSumPhase[k] += tmp;
                phase         = gSumPhase[k];

#ifdef KISSFFT_SUPPORT
                kiss_in[k].r = magn * cos(phase);
                kiss_in[k].i = magn * sin(phase);
#else
                fftw_in[k][0] = magn * cos(phase);
                fftw_in[k][1] = magn * sin(phase);
#endif
            }

            for (k = 2 + fftFrameSize2; k < fftFrameSize; ++k)
            {
#ifdef KISSFFT_SUPPORT
                kiss_in[k].r     = 0.0f;
                kiss_in[k - 1].i = 0.0f;
#else
                fftw_in[k][0]     = 0.0;
                fftw_in[k - 1][1] = 0.0;
#endif
            }

#ifdef KISSFFT_SUPPORT
            kiss_fft(kiss_cfg_inv, kiss_in, kiss_out);
#else
            fftw_execute(ftPlanInverse);
#endif

            for (k = 0; k < fftFrameSize; ++k)
            {
#ifdef KISSFFT_SUPPORT
                gOutputAccum[k] +=
                    2.0 * window[k] * kiss_out[k].r / FS_osamp;
#else
                gOutputAccum[k] +=
                    2.0 * window[k] * fftw_out[k][0] / FS_osamp;
#endif
            }

            for (k = 0; k < stepSize; ++k)
                gOutFIFO[k] = gOutputAccum[k];

            memmove(gOutputAccum,
                    gOutputAccum + stepSize,
                    fftFrameSize * sizeof(float));

            for (k = 0; k < inFifoLatency; ++k)
                gInFIFO[k] = gInFIFO[k + stepSize];
        }
    }
}

// ============================================================================
// Original Bernsee FFT (unchanged)
// ============================================================================

void PitchShifter::smbFft(float* fftBuffer, long fftFrameSize, long sign)
{
    float wr, wi, arg, *p1, *p2, temp;
    float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
    long  i, bitm, j, le, le2, k;

    for (i = 2; i < 2 * fftFrameSize - 2; i += 2)
    {
        for (bitm = 2, j = 0; bitm < 2 * fftFrameSize; bitm <<= 1)
        {
            if (i & bitm) j++;
            j <<= 1;
        }

        if (i < j)
        {
            p1   = fftBuffer + i;
            p2   = fftBuffer + j;
            temp = *p1; *(p1++) = *p2; *(p2++) = temp;
            temp = *p1; *p1     = *p2; *p2     = temp;
        }
    }

    for (k = 0, le = 2;
         k < (long)(log(fftFrameSize) / LOG_2 + .5);
         ++k)
    {
        le <<= 1;
        le2 = le >> 1;
        ur  = 1.0;
        ui  = 0.0;

        arg = M_PI / (le2 >> 1);
        wr  = cosf(arg);
        wi  = sign * sinf(arg);

        for (j = 0; j < le2; j += 2)
        {
            p1r = fftBuffer + j;
            p1i = p1r + 1;
            p2r = p1r + le2;
            p2i = p2r + 1;

            for (i = j; i < 2 * fftFrameSize; i += le)
            {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;

                *p2r = *p1r - tr;
                *p2i = *p1i - ti;
                *p1r += tr;
                *p1i += ti;

                p1r += le; p1i += le;
                p2r += le; p2i += le;
            }

            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

// ============================================================================
// Safe atan2 Replacement
// ============================================================================

double PitchShifter::smbAtan2(double x, double y)
{
    double signx;

    if (x > 0.0)      signx = 1.0;
    else              signx = -1.0;

    if (x == 0.0) return 0.0;
    if (y == 0.0) return signx * M_PI_2;

    return atan2(x, y);
}
