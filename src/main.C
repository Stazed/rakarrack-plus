/*
  rakarrack - a guitar efects software

  main.C  -  Main file of the organ
  Copyright (C) 2008-2010 Josep Andreu
  Author: Josep Andreu & Douglas McClendon

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


#include <getopt.h>
#include <sched.h>
#include <sys/mman.h>
#include "global.h"
#include "UI/rakarrack.h"
#include "process.h"
#include "jack.h"

void
show_help()
{
    fprintf(stderr, "Usage: rakarrack-plus [OPTION]\n\n");
    fprintf(stderr,
            "  -h ,     --help \t\t\t display command-line help and exit\n");
    fprintf(stderr, "  -n ,     --no-gui \t\t\t disable GUI\n");
    fprintf(stderr, "  -l File, --load=File \t\t\t loads preset\n");
    fprintf(stderr, "  -b File, --bank=File \t\t\t loads bank\n");
    fprintf(stderr, "  -p #,    --preset=# \t\t\t set preset\n");
    fprintf(stderr, "  -j Name, --jack-client-name=Name \t set alternate jack/alsa name: default=rakarrack-plus\n");
    fprintf(stderr, "  -x, --dump-preset-names \t\t prints bank of preset names and IDs\n\n");
    fprintf(stderr, "FLTK options are:\n\n");
    fprintf(stderr, "  -bg2 color\n");
    fprintf(stderr, "  -bg color\n");
    fprintf(stderr, "  -di[splay] host:n.n\n");
    fprintf(stderr, "  -dn[d]\n");
    fprintf(stderr, "  -fg color\n");
    fprintf(stderr, "  -g[eometry] WxH+X+Y\n");
    fprintf(stderr, "  -i[conic]\n");
    fprintf(stderr, "  -k[bd]\n");
    fprintf(stderr, "  -na[me] classname\n");
    fprintf(stderr, "  -nod[nd]\n");
    fprintf(stderr, "  -nok[bd]\n");
    fprintf(stderr, "  -not[ooltips]\n");
    fprintf(stderr, "  -s[cheme] scheme (plastic,none,gtk+)\n");
    fprintf(stderr, "  -ti[tle] windowtitle\n");
    fprintf(stderr, "  -to[oltips]\n");
    fprintf(stderr, "\n");
}

int
main(int argc, char *argv[])
{
    // Read command Line

    fprintf
        (
            stderr,
            "\n%s %s - Copyright (c) Josep Andreu - Ryan Billing - Douglas McClendon - Arnout Engelen\n",
            PACKAGE, VERSION
        );

    if (argc == 1)
    {
        fprintf(stderr, "Try 'rakarrack-plus --help' for command-line options.\n");
    }

    struct option opts[] =
    {
        {"load", 1, NULL, 'l'},
        {"bank", 1, NULL, 'b'},
        {"preset", 1, NULL, 'p'},
        {"no-gui", 0, NULL, 'n'},
        {"dump-preset-names", 0, NULL, 'x'},
        {"help", 0, NULL, 'h'},
        {"jack-client-name", 1, NULL, 'j'},
        {0, 0, 0, 0}
    };


    RKRGUI *rgui = NULL;
    int needtoloadfile = 0;
    int needtodump = 0;
    int preset = C_CHANGE_PRESET_OFF;
    int option_index = 0, opt;
    int commandline = 0;
    int gui = 1;
    int exitwithhelp = 0;
    int needtoloadbank = 0;
    std::string banktoload;
    std::string filetoload;

    // globals
    opterr = 0;
    option_index = 0;

    while (1)
    {
        opt = getopt_long(argc, argv, "l:b:p:nxhj:", opts, &option_index);
        char *optarguments = optarg;

        if (opt == -1)
            break;

        switch (opt)
        {
        case 'h':
            exitwithhelp = 1;
            break;
        case 'n':
            gui = 0;
            break;
        case 'l':
            if (optarguments != NULL)
            {
                commandline = 1;
                needtoloadfile = 1;
                filetoload = strdup(optarguments);
            }
            break;
        case 'b':
            if (optarguments != NULL)
            {
                needtoloadbank = 1;
                banktoload = strdup(optarguments);
            }
            break;
        case 'p':
            if (optarguments != NULL)
            {
                preset = atoi(optarguments);
            }
            break;
        case 'x':
            needtodump = 1;
            break;

        case 'j':
            if (optarguments != NULL)
            {
                jack_client_name = strdup(optarguments);
            }
            break;
        }
    }


    if (exitwithhelp)
    {
        show_help();
        return (0);
    };


    RKR process;
    
    // Set flag to indicate if in CLI vs Gui mode
    process.Gui_Shown = gui;

    if (process.No_Jack_Client)
    {
        show_help();
        
        process.Error_Handle(34);
        return (0);
    }

    if (needtodump)
    {
        process.dump_preset_names();
        exit(1);
    }

    JACKstart(&process);
    process.InitMIDI();
    process.ConnectMIDI();

    if (needtoloadfile)
    {
        process.File_To_Load = filetoload;
        
        // This will clear process.File_To_Load if bad file
        process.load_preset(filetoload.c_str());
    }
    
    // Set command line bank file, if any
    if (needtoloadbank)
    {
        // If valid bank then copy the Bank name and set the command line flag
        if(process.load_bank(banktoload.c_str()))
        {
            process.Bank_Load_Command_Line = needtoloadbank;
            process.Command_Line_Bank = banktoload;
        }
    }
    
    // Set preset index from command line, if any
    process.Change_Preset = preset;

    // Set flag to indicate a file from the command line, if any
    process.Command_Line_File = commandline;

    // Launch GUI
    if (process.Gui_Shown)
    {
        rgui = new RKRGUI(argc, argv, &process);
    }
    else
    {
        (void)rgui; // To suppress unused variable compiler warning
    }

    if (!process.Gui_Shown)
    {
        process.Bypass = 1;
        process.calculavol(1);
        process.calculavol(2);
        process.booster = 1.0f;
    }

    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    // For keeping the message about disconnected from repeating
    int jack_disconnected = 0;

    // Main Loop - run until exit requested
    while (!process.Exit_Program)
    {
        // Refresh GUI
        if (process.Gui_Shown)
        {
            Fl::wait();
        }
        else
        {
            usleep(1500);
            if (process.Change_Preset != C_CHANGE_PRESET_OFF)
            {
                if ((process.Change_Preset > 0) && (process.Change_Preset < 61))
                {
                    process.bank_to_preset(process.Change_Preset);
                }
                process.Change_Preset = C_CHANGE_PRESET_OFF;
            }
        }

        if ((!jack_disconnected) && (process.Jack_Shut_Down))
        {
            jack_disconnected = 1;
            process.Error_Handle(33);
        }

        process.miramidi();
    }

    // free memory etc.
    JACKfinish(&process);

    return (0);

}

