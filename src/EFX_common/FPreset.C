/*
  Rakarrack   Audio FX software
  FPreset.C - Internal Preset Reader
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

#include "FPreset.h"

std::string global_user_directory = "";
const std::string C_INSERT_PRESET_FILE = "InsertPresets.rkis";

FPreset::FPreset()
{
}

FPreset::~FPreset()
{
}

void
FPreset::ReadPreset(int eff, int num, int pdata[], char *filename)
{
    std::string insert_preset_location = "";
    
    // Did the user set a User Directory
    if( (strcmp(global_user_directory.c_str(), DATADIR) != 0) && (strcmp(global_user_directory.c_str(), UD_NOT_SET) != 0) )
    {
        insert_preset_location = global_user_directory;
        if(insert_preset_location[insert_preset_location.size() - 1] != '/')
            insert_preset_location += "/";

        insert_preset_location += C_INSERT_PRESET_FILE;
    }
    else
    {
        printf("No User Directory Set. Cannot load insert presets!\n");
        return;
    }

    FILE *fn;
    char buf[256];
    char *sbuf;
    int reff = 0;
    memset(pdata, 0, sizeof (int)*MAX_PDATA_SIZE);

    if ((fn = fopen(insert_preset_location.c_str(), "r")) != NULL)
    {
        int k = 0;

        if (eff != EFX_CONVOLOTRON && eff != EFX_REVERBTRON && eff != EFX_ECHOTRON)
        {
            while (fgets(buf, sizeof buf, fn) != NULL)
            {
                sbuf = buf;
                sscanf(buf, "%d", &reff);

                if (reff == eff) k++;

                if (k == num)
                {
                    strsep(&sbuf, ",");
                    strsep(&sbuf, ",");
                    sscanf(sbuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d.%d.%d\n",
                           &pdata[0], &pdata[1], &pdata[2], &pdata[3], &pdata[4], &pdata[5], &pdata[6], &pdata[7], &pdata[8], &pdata[9],
                           &pdata[10], &pdata[11], &pdata[12], &pdata[13], &pdata[14], &pdata[15], &pdata[16], &pdata[17], &pdata[18], &pdata[19],
                           &pdata[20], &pdata[21], &pdata[22], &pdata[23], &pdata[24], &pdata[25], &pdata[26], &pdata[27], &pdata[28], &pdata[29]);
                    break;
                }
            }
        }
        else if (eff == EFX_CONVOLOTRON)
        {
            char *cfilename;
            cfilename = (char *) malloc(sizeof (char) * 128);

            if(cfilename == NULL)
            {
                fprintf(stderr, "Cannot allocate memory for cfilename\n");
                goto ERROR_QUIT;
            }

            memset(cfilename, 0, sizeof (char) * 128);
            while (fgets(buf, sizeof buf, fn) != NULL)
            {
                sbuf = buf;
                sscanf(buf, "%d", &reff);
                
                if (reff == eff) k++;
                
                if (k == num)
                {
                    strsep(&sbuf, ",");
                    strsep(&sbuf, ",");
                    sscanf(sbuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%127s\n",
                           &pdata[0], &pdata[1], &pdata[2], &pdata[3], &pdata[4], &pdata[5],
                           &pdata[6], &pdata[7], &pdata[8], &pdata[9], &pdata[10], &pdata[11],
                           cfilename);
                    break;
                }
            }

            if (filename != NULL)
            {
                memset(filename, 0, sizeof (*filename));
                strcpy(filename, cfilename);
            }
            
            free(cfilename);
        }
        else if (eff == EFX_REVERBTRON)
        {
            char *cfilename;
            cfilename = (char *) malloc(sizeof (char) * 128);

            if(cfilename == NULL)
            {
                fprintf(stderr, "Cannot allocate memory for cfilename\n");
                goto ERROR_QUIT;
            }

            memset(cfilename, 0, sizeof (char) * 128);
            while (fgets(buf, sizeof buf, fn) != NULL)
            {
                sbuf = buf;
                sscanf(buf, "%d", &reff);
                if (reff == eff) k++;
                if (k == num)
                {
                    strsep(&sbuf, ",");
                    strsep(&sbuf, ",");
                    sscanf(sbuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%127s\n",
                           &pdata[0], &pdata[1], &pdata[2], &pdata[3], &pdata[4], &pdata[5],
                           &pdata[6], &pdata[7], &pdata[8], &pdata[9], &pdata[10], &pdata[11],
                           &pdata[12], &pdata[13], &pdata[14], &pdata[15], &pdata[16],
                           cfilename);
                    break;
                }
            }

            if (filename != NULL)
            {
                memset(filename, 0, sizeof (*filename));
                strcpy(filename, cfilename);
            }
            
            free(cfilename);
        }
        else if (eff == EFX_ECHOTRON)
        {
            char *cfilename;
            cfilename = (char *) malloc(sizeof (char) * 128);

            if(cfilename == NULL)
            {
                fprintf(stderr, "Cannot allocate memory for cfilename\n");
                goto ERROR_QUIT;
            }

            memset(cfilename, 0, sizeof (char) * 128);
            while (fgets(buf, sizeof buf, fn) != NULL)
            {
                sbuf = buf;
                sscanf(buf, "%d", &reff);
                
                if (reff == eff) k++;
                
                if (k == num)
                {
                    strsep(&sbuf, ",");
                    strsep(&sbuf, ",");
                    sscanf(sbuf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%127s\n",
                           &pdata[0], &pdata[1], &pdata[2], &pdata[3], &pdata[4], &pdata[5],
                           &pdata[6], &pdata[7], &pdata[8], &pdata[9], &pdata[10], &pdata[11],
                           &pdata[12], &pdata[13], &pdata[14], &pdata[15], &pdata[16],
                           cfilename);
                    break;
                }
            }

            if (filename != NULL)
            {
                memset(filename, 0, sizeof (*filename));
                strcpy(filename, cfilename);
            }
            
            free(cfilename);
        }

ERROR_QUIT:
        fclose(fn);
    }
    else
    {
        fprintf(stderr,"Cannot load insert preset file: %s\n", insert_preset_location.c_str());
    }
}


