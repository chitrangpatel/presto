#ifndef __rfifind_cmd__
#define __rfifind_cmd__
/*****
  command line parser interface -- generated by clig 
  (http://wsd.iitb.fhg.de/~kir/clighome/)

  The command line parser `clig':
  (C) 1995---2001 Harald Kirsch (kirschh@lionbioscience.com)
*****/

typedef struct s_Cmdline {
  /***** -o: Root of the output file names */
  char outfileP;
  char* outfile;
  int outfileC;
  /***** -pkmb: Raw data in Parkes Multibeam format */
  char pkmbP;
  /***** -gmrt: Raw data in GMRT Phased Array format */
  char gmrtP;
  /***** -bcpm: Raw data in Berkeley-Caltech Pulsar Machine (BPP) format */
  char bcpmP;
  /***** -wapp: Raw data in Wideband Arecibo Pulsar Processor (WAPP) format */
  char wappP;
  /***** -numwapps: Number of WAPPs used with contiguous frequencies */
  char numwappsP;
  int numwapps;
  int numwappsC;
  /***** -if: A specific IF to use if available (summed IFs is the default) */
  char ifsP;
  int ifs;
  int ifsC;
  /***** -clip: Time-domain sigma to use for clipping (0.0 = no clipping, 6.0 = default */
  char clipP;
  float clip;
  int clipC;
  /***** -noclip: Do not clip the data.  (The default is to _always_ clip!) */
  char noclipP;
  /***** -xwin: Draw plots to the screen as well as a PS file */
  char xwinP;
  /***** -nocompute: Just plot and remake the mask */
  char nocomputeP;
  /***** -rfixwin: Show the RFI instances on screen */
  char rfixwinP;
  /***** -rfips: Plot the RFI instances in a PS file */
  char rfipsP;
  /***** -time: Seconds to integrate for stats and FFT calcs */
  char timeP;
  double time;
  int timeC;
  /***** -timesig: The +/-sigma cutoff to reject time-domain chunks */
  char timesigmaP;
  float timesigma;
  int timesigmaC;
  /***** -freqsig: The +/-sigma cutoff to reject freq-domain chunks */
  char freqsigmaP;
  float freqsigma;
  int freqsigmaC;
  /***** -chanfrac: The fraction of bad channels that will mask a full interval */
  char chantrigfracP;
  float chantrigfrac;
  int chantrigfracC;
  /***** -intfrac: The fraction of bad intervals that will mask a full channel */
  char inttrigfracP;
  float inttrigfrac;
  int inttrigfracC;
  /***** -zapchan: Channels to explicitly remove from analysis (zero-offset) */
  char zapchanP;
  int *zapchan;
  int zapchanC;
  /***** -zapints: Intervals to explicitly remove from analysis (zero-offset) */
  char zapintsP;
  int *zapints;
  int zapintsC;
  /***** -mask: File containing masking information to use */
  char maskfileP;
  char* maskfile;
  int maskfileC;
  /***** uninterpreted command line parameters */
  int argc;
  /*@null*/char **argv;
  /***** the whole command line concatenated */
  char *full_cmd_line;
} Cmdline;


extern char *Program;
extern void usage(void);
extern /*@shared*/Cmdline *parseCmdline(int argc, char **argv);

extern void showOptionValues(void);

#endif

