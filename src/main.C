/*
  rakarrack - audio effects software

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

  You should have received a copy of the GNU General Public License (version2)
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

// This is necessary for NSM, so if it is empty, then we are not in NSM session
std::string nsm_preferences_file = "";
// Use when nsm is using single state mode
std::string nsm_preset_file = "";

// Signal handlers
static volatile int got_sigint = 0;
static volatile int got_sigusr1 = 0;
int save_preferences = 0;

#ifdef NSM_SUPPORT
#include "nsm.h"

int global_gui_show = 0;
static nsm_client_t *nsm = 0;
static int wait_nsm = 1;

int                                                                  
cb_nsm_open ( const char *save_file_path,   // See API Docs 2.2.2 
              const char *,                 // display_name
              const char *client_id,        // Use as JACK Client Name
              char **,                      // out_msg
              void *)                       // userdata
{
    nsm_preferences_file = save_file_path;
    nsm_preferences_file += ".prefs";

    // single state mode
    nsm_preset_file = save_file_path;
    nsm_preset_file += ".rkr";
    
    jack_client_name = strdup(client_id);
    wait_nsm = 0;
    return ERR_OK;
}                                                                    
                                                                     
int
cb_nsm_save ( char **,          // out_msg
              void *)           // userdata
{
    save_preferences = 1;
    return ERR_OK;
}

void
cb_nsm_show ( void * )          // userdata
{
    global_gui_show = CONST_GUI_SHOW;
}

void
cb_nsm_hide ( void * )          // userdata
{
    global_gui_show = CONST_GUI_HIDE;
}
                                                                     
void
poll_nsm(void *)                // userdata
{
    if ( nsm )
    {
        nsm_check_nowait( nsm );
        return;
    }
}
#endif  // NSM_SUPPORT

void sigterm_handler(int sig)
{
    // handle signal type
    if (sig == SIGUSR1)
    {
        got_sigusr1 = sig;
    }

    if (sig == SIGINT)
    {
        got_sigint = sig;
    }
}

bool install_signal_handlers()
{
    struct sigaction action;
    memset(&action, 0, sizeof (action));
    action.sa_handler = sigterm_handler;

    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        fprintf(stderr, "sigaction() failed: \n");
        return false;
    }

    if (sigaction(SIGINT, &action, NULL) == -1)
    {
        fprintf(stderr, "sigaction() failed: \n");
        return false;
    }
    
    return true;
}

void check_signals(void *usrPtr)
{
    // process signals
    RKR *process = NULL;
    process = static_cast<RKR *>(usrPtr);

    if (!process)
        return;

    if (got_sigusr1 == SIGUSR1)
    {
        fprintf(stderr, "Got SIGUSR1, saving...\n");
        save_preferences = 1;
        got_sigusr1 = 0;
        return;
    }

    if (got_sigint == SIGINT)
    {
        fprintf(stderr, "Got SIGTERM, quitting...\n");
        got_sigint = 0;
        process->Exit_Program = 1;
    }
}


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

    install_signal_handlers();

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
    std::string command_line_name = PACKAGE;

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
                command_line_name = jack_client_name = strdup(optarguments);
            }
            break;
        }
    }


    if (exitwithhelp)
    {
        show_help();
        return (0);
    };
    
#ifdef NSM_SUPPORT
    const char *nsm_url = getenv( "NSM_URL" );
    
    if ( nsm_url )
    {
        nsm = nsm_new();

        nsm_set_open_callback( nsm, cb_nsm_open, 0 );
        nsm_set_save_callback( nsm, cb_nsm_save, 0 );

        if ( 0 == nsm_init( nsm, nsm_url ) )
        {
            nsm_send_announce( nsm, command_line_name.c_str(), ":optional-gui:", argv[0] );
        }

        int timeout = 0;
        while ( wait_nsm )
        {
            nsm_check_wait( nsm, 500 );
            timeout += 1;

            if ( timeout > 200 )
                exit ( 1 );
        }

        if ( strstr( nsm_get_session_manager_features ( nsm ),
                      ":optional-gui:" ) )
        {
            nsm_set_show_callback( nsm, cb_nsm_show, 0 );
            nsm_set_hide_callback( nsm, cb_nsm_hide, 0 );

        }

        gui = 1;    // always load gui with NSM
    }

    // For NSM gui show and update from hide
    int hold_preset = C_CHANGE_PRESET_OFF;
#endif // NSM_SUPPORT


    RKR process(gui);

    if (process.No_Jack_Client)
    {
        show_help();
        
        process.Handle_Message(34);
        return (0);
    }

    if (needtodump)
    {
        process.dump_preset_names();
        exit(1);
    }

#ifdef NSM_SUPPORT
    // Startup gui status. We need to send the NSM show status here on startup
    // since the show function will not be called if the gui is already shown.
    // By default we always show, then if the gui should be hidden, we hide.
    // This is done by the main loop while, but for show on start only, since
    // the gui is already shown, the routine to show is not called, and the status
    // to send to NSM is never sent. So we send it here, and clear the flag.
    if(!nsm_preferences_file.empty())   // check if in NSM session
    {
        if(global_gui_show == CONST_GUI_SHOW)
        {
            global_gui_show = CONST_GUI_OFF;
            nsm_send_is_shown ( nsm );
        }
        
        if(process.Config.NSM_single_state)
        {
            if(!process.does_file_exist(nsm_preset_file))
            {
                process.save_preset(nsm_preset_file);
            }
            else
            {
                filetoload = nsm_preset_file;
                needtoloadfile = 1;
                commandline = 1;    // not really command line but works the same
            }
        }
    }
#endif

    JACKstart(&process);
    process.InitMIDI();
    process.ConnectMIDI();

    if (needtoloadfile)
    {
        process.File_To_Load = filetoload;
        
        // This will clear process.File_To_Load if bad file
        process.load_preset(filetoload);
    }
    
    // Set command line bank file, if any
    if (needtoloadbank)
    {
        // If valid bank then copy the Bank name and set the command line flag
        if(process.load_bank_from_vector(banktoload))
        {
            process.Bank_Load_Command_Line = needtoloadbank;
            process.Command_Line_Bank = banktoload;
        }
    }
    
    // Set preset index from command line, if any
    process.Change_Preset = preset;

    // Set flag to indicate a file from the command line, or NSM single state mode
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
        process.Active_Preset.FX_Master_Active = 1;
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
        check_signals(&process);

        // Refresh GUI
        if (process.Gui_Shown)
        {
            Fl::wait();
            
#ifdef NSM_SUPPORT
            // We could have NSM_SUPPORT without a session.
            // So the check for nsm_preferences_file.empty() is to ensure that we are
            // actually in an NSM session. The file is only loaded when within a session.
            if(!nsm_preferences_file.empty())
            {
                poll_nsm(NULL);
                if(global_gui_show == CONST_GUI_HIDE)
                {
                    rgui->NSM_gui_hide();
                    nsm_send_is_hidden ( nsm );
                }
            }
#endif
            // This could be from session SIGUSR1, so not necessarily from NSM
            if(save_preferences)
            {
                save_preferences = 0;
                rgui->save_current_state(0);
#ifdef NSM_SUPPORT
                if ( process.Config.NSM_single_state && !nsm_preset_file.empty() )
                {
                    process.save_preset( nsm_preset_file );
                }
#endif
                
                // For session use, the modified checks will not work on save and quit.
                // NSM requires that the client must quit immediately, so the 
                // shown modal windows are ignored. The user would need to do 
                // a separate save, then quit.
                rgui->is_bank_modified();
                rgui->is_PG_table_modified();      
            }
        }
        else
        {
            // This is needed for MIDI control program changes when no gui is shown.
            // If the gui is shown, then the RKRGUI timeout is used.
            usleep(1500);
            if (process.Change_Preset != C_CHANGE_PRESET_OFF)
            {
                if ((process.Change_Preset > 0) && (process.Change_Preset < 61))
                {
                    process.active_bank_preset_to_main_window(process.Change_Preset);
                    
                    // reset these if volume changed from preset
                    process.calculavol(1);
                    process.calculavol(2);
                    process.booster = 1.0f;
                }
#ifdef NSM_SUPPORT
                // hold the preset number so we can update the bank window highlight if using NSM
                hold_preset = process.Change_Preset;
#endif
                process.Change_Preset = C_CHANGE_PRESET_OFF;
            }
            
            if (global_error_number > 0)
                process.Handle_Message(global_error_number);

#ifdef NSM_SUPPORT
            // Check if within NSM session
            if(!nsm_preferences_file.empty())
            {
                poll_nsm(NULL);
                if(global_gui_show == CONST_GUI_SHOW)
                {
                    hold_preset = rgui->NSM_gui_show(hold_preset);
                    nsm_send_is_shown ( nsm );
                }
            }
#endif
            // This could be from session SIGUSR1, so not necessarily from NSM
            // We do not check for is_modified, bank or table since they cannot 
            // be changed if there is no gui. If in NSM session, then the modified
            // is checked upon gui hide request.
            if(save_preferences)
            {
                save_preferences = 0;
                rgui->save_current_state(0);
#ifdef NSM_SUPPORT
                if ( process.Config.NSM_single_state && !nsm_preset_file.empty() )
                {
                    process.save_preset( nsm_preset_file );
                }
#endif
            }
        }

        if ((!jack_disconnected) && (process.Jack_Shut_Down))
        {
            jack_disconnected = 1;
            process.Handle_Message(33);
        }

        process.miramidi();
    
#ifdef SYSEX_SUPPORT
        if(process.m_have_sysex_message)
        {
            process.m_have_sysex_message = 0;
            process.sysex_save_preset();
        }
#endif  // SYSEX_SUPPORT
    }

    // free memory etc.
    JACKfinish(&process);

#ifdef NSM_SUPPORT
    if(nsm)
    {
        nsm_free( nsm );
        nsm = NULL;
    }
#endif

    return (0);

}

