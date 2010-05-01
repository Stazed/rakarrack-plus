

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sndfile.h>
#include <getopt.h>


#define PERIOD 128

 
 

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
 int i;
 int readcount;
 int index_length=0;
 int data_length=0;
 int have_output=0;
 FILE *fn;
 SNDFILE *infile = NULL;
 SF_INFO sfinfo;
 char Outputfile[128];
 char Inputfile[128];
 char tempfile[128];
 float *buf;
 float *index, *data;


  fprintf (stderr,
   "\nrackverb convert Reverb IR wav files to the rakarrack file format.\nrackverb - Copyright (c) Josep Andreu - Ryan Billing - Douglas McClendon\n\n");


  struct option opts[] = {
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
	    strcpy(Inputfile,optarguments); 
            if(!(infile = sf_open(Inputfile, SFM_READ, &sfinfo))) return(0);

            }
          break;  
        case 'o':
           if (optarguments != NULL)
	    {
	    have_output = 1;
            strcpy(Outputfile,optarguments);
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


 buf = (float *) malloc (sizeof (float) * PERIOD);
 index = (float *) malloc (sizeof (float) * 100000); // put the max size
 data  = (float *) malloc (sizeof (float) * 1000000); // put the max size


if(!have_output)
{
 strncpy(tempfile, Inputfile, strlen(Inputfile)-4);
 sprintf(Outputfile, "%s.rvb",tempfile);
}

// open output and save headers

  if ((fn = fopen (Outputfile, "w")) == NULL)  return(0);
  bzero (wbuf, sizeof (wbuf));
  sprintf(wbuf,"%s\n",Inputfile);    
  fputs (wbuf, fn);
  bzero (wbuf, sizeof (wbuf));
  sprintf(wbuf,"%d\n",sfinfo.samplerate);    
  fputs (wbuf, fn);
  

// read file
readcount = sf_seek (infile,0, SEEK_SET);
readcount = 1;

while( readcount > 0)
{ 
readcount = sf_readf_float(infile,buf,PERIOD);  //PERIOD is defined UP ... change if its short.


//
//process the data here 
//
     tmp = 0.0f;
     lastbuf = 0.0f;
for (i = 0; i<PERIOD; i++) {
     tmp += buf[i];
     testzero = buf[i] * lastbuf; 
     if(testzero < 0.0f) {
     data[x] = tmp;
     index[x] = time;
     x++;
     }	
}
//index data in index[x]
//data data in data[x]
//index length in indexlength
//data length in datalength





//close input file when finish
}
sf_close(infile);

// save data and close

  bzero (wbuf, sizeof (wbuf));
  sprintf(wbuf,"%d\n",index_length);    
  fputs (wbuf, fn);

  bzero (wbuf, sizeof (wbuf));
  sprintf(wbuf,"%d\n",data_length);    
  fputs (wbuf, fn);


for(i=0;i<index_length;i++)
{
  bzero(wbuf,sizeof(wbuf));
  sprintf(wbuf, "%f,",index[i]);
  fputs(wbuf,fn);
}

for(i=0;i<data_length;i++)
{
  bzero(wbuf,sizeof(wbuf));
  sprintf(wbuf, "%f,",data[i]);
  fputs(wbuf,fn);
}

fclose(fn);
return(0);
}


