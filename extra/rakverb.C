/*
  rakarrack-plus - audio effects software

  rakverb.C

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License (version 2) for more details.

  You should have received a copy of the GNU General Public License (version2)
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sndfile.h>
#include <getopt.h>
#include <string>
#include "../src/strlcpy.h"

#define PERIOD 128
#define RND ( rand()/((RAND_MAX)+1.0))

/* For file .wav extension size for removing the extension from file names */
const unsigned c_wav_ext_size = 4;

void
show_help ()
{
    fprintf (stderr, "Usage: rakverb -i [Input File] [-o] [Output File]\n\n" );
    fprintf (stderr,
        "  -h ,     --help \t\t\t display command-line help and exit\n");
    fprintf (stderr,
        "  -i ,     --input \t\t\t Input File\n");
    fprintf (stderr,
        "  -o ,     --output \t\t\t Output File\n");

    fprintf (stderr, "\n");

}

int
main(int argc, char *argv[])
{
    int option_index = 0, opt;
    int exitwithhelp = 0;
    char wbuf[2048];
    int i, j;
    int step;
    int readcount;
    int have_output = 0;
    FILE *fn;
    SNDFILE *infile = NULL;
    SF_INFO sfinfo;
    char Outputfile[156];
    char Inputfile[128];
    float *buf;
    float *index, *data;
    float sample;
    float testzero, time, tmp, iSR, lastbuf;
    float skip = 0.0f;
    int indexx;
    float chunk, incr, findex;
    float compress, quality;
    int x = 0;
    fprintf (stderr,
        "\nrackverb convert Reverb IR wav files to the rakarrack file format.\nrackverb - Copyright (c) Josep Andreu - Ryan Billing \n\n");

    struct option opts[] =
    {
        {"input", 1, NULL, 'i'},
        {"output", 1, NULL, 'o'},
        {"help", 0, NULL, 'h'},
        {0, 0, 0, 0}
    };

    while (1)
    {
        opt = getopt_long (argc, argv, "i:o:h", opts, &option_index);
        char *optarguments = optarg;

        if (opt == -1)
            break;
        switch (opt)
        {
            case 'h':
                exitwithhelp = 1;
                break;
            case 'i':
                if (optarguments != NULL)
                {
                    RKRP::strlcpy(Inputfile, optarguments, sizeof(Inputfile));
                    if(!(infile = sf_open(Inputfile, SFM_READ, &sfinfo))) return(0);

                }
                break;
            case 'o':
                if (optarguments != NULL)
                {
                    have_output = 1;
                    RKRP::strlcpy(Outputfile, optarguments, sizeof(Outputfile));
                }
                break;
        }

    }

    if (exitwithhelp != 0)
    {
        show_help ();
        return (0);
    };

    if (argc < 3)
    {
        fprintf (stderr, "Try 'rakverb --help' for usage options.\n");
        return(0);
    }

    buf = (float *) malloc (sizeof (float) * PERIOD*sfinfo.channels);
    index = (float *) malloc (sizeof (float) * sfinfo.frames*sfinfo.channels); // put the max size
    data  = (float *) malloc (sizeof (float) * sfinfo.frames*sfinfo.channels); // put the max size

    if ((buf == NULL) || (index == NULL) || (data == NULL))
    {
        fprintf(stderr, "ERRoR: Cannot allocate memory, buf, index, data\n");
        return(0);
    }

    if(!have_output)
    {
        std::string file_name = Inputfile;
        file_name = file_name.substr(0, file_name.size() - c_wav_ext_size);    // remove the file extension .wav

        snprintf(Outputfile, sizeof(Outputfile), "%s.rvb", file_name.c_str());
    }

    // open output and save headers

    if ((fn = fopen (Outputfile, "w")) == NULL)  return(0);
    memset(wbuf, 0, sizeof (wbuf));
    snprintf(wbuf, sizeof(wbuf), "%s\n", Inputfile);
    fputs (wbuf, fn);

    // read file
    int ret = sf_seek (infile, 0, SEEK_SET);

    if(ret < 0)
    {
        fprintf(stderr, "An error occurred reading the sound file %127s\n", Inputfile);
        return(0);
    }
    
    readcount = 1;
    time = 0.0f;
    tmp = 0.0f;
    lastbuf = 0.0f;
    iSR = 1.0f / ((float) sfinfo.samplerate);
    if (sfinfo.channels == 1) step = 1;
    if (sfinfo.channels == 2) step = 2;

    while( readcount > 0)
    {
        readcount = sf_readf_float(infile, buf, PERIOD); //PERIOD is defined UP ... change if its short.

        //
        //process the data here
        //

        for (i = 0; i < (PERIOD * sfinfo.channels); i += step)
        {

            if(step == 1) sample = buf[i];
            if(step == 2) sample = (buf[i] + buf[i + 1]) * .5;

            tmp += sample;
            testzero = sample * lastbuf;       //one-liner zero crossing detection
            if(testzero < 0.0f)
            {
                data[x] = tmp;
                index[x] = time;
                x++;
                tmp = 0.0f;
            }
            time += iSR;
            lastbuf = sample;
        }
        //index data in index[x]
        //data data in data[x]

    }

    incr = 1500.0f / ((float) x);
    printf("incr: %f\n", incr);
    if(x < 1500) incr = 1.0f; //Often will be longer than 1024.  Change to make final file longer or shorter.

    compress = 100.0f * ((float) x) / ((float) sfinfo.frames);
    printf("Compression: %3.2f%%\n", compress);
    quality = incr*compress;
    printf("Quality : %3.2f%%\n", quality);
    sf_close(infile);

    memset(wbuf, 0, sizeof(wbuf));
    snprintf(wbuf, sizeof(wbuf), "%f,%f\n", compress, quality);
    fputs(wbuf, fn);

    indexx = 1500;

    memset(wbuf, 0, sizeof(wbuf));
    snprintf(wbuf, sizeof(wbuf), "%d\n", indexx);
    fputs(wbuf, fn);

    skip = 0.0f;
    indexx = 0;
    double dchunk = 10.0;
    chunk = 25;
    if(x < 1500) x = 1500;
    for(i = 0; i < x; i++)
    {
        skip += incr;
        findex = (float)indexx;
        if( findex < skip)
        {
            for(j = 0; j <= chunk; j++)
            {
                if(indexx < 1500)
                {
                    memset(wbuf, 0, sizeof(wbuf));
                    snprintf(wbuf, sizeof(wbuf), "%f,%f\n", index[i + j], data[i + j]);
                    fputs(wbuf, fn);
                    indexx++;

                }
            }

            chunk = (int) (dchunk*RND);
            dchunk *= 0.99;

        }
    };

    fclose(fn);
    return(0);
}
