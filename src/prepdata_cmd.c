/*****
  command line parser -- generated by clig
  (http://wsd.iitb.fhg.de/~kir/clighome/)

  The command line parser `clig':
  (C) 1995---2001 Harald Kirsch (kirschh@lionbioscience.com)
*****/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "prepdata_cmd.h"

char *Program;

/*@-null*/

static Cmdline cmd = {
  /***** -o: Root of the output file names */
  /* outfileP = */ 0,
  /* outfile = */ (char*)0,
  /* outfileC = */ 0,
  /***** -pkmb: Raw data in Parkes Multibeam format */
  /* pkmbP = */ 0,
  /***** -gmrt: Raw data in GMRT Phased Array format */
  /* gmrtP = */ 0,
  /***** -bcpm: Raw data in Berkeley-Caltech Pulsar Machine (BPP) format */
  /* bcpmP = */ 0,
  /***** -wapp: Raw data in Wideband Arecibo Pulsar Processor (WAPP) format */
  /* wappP = */ 0,
  /***** -numwapps: Number of WAPPs used with contiguous frequencies */
  /* numwappsP = */ 1,
  /* numwapps = */ 1,
  /* numwappsC = */ 1,
  /***** -if: A specific IF to use if available (summed IFs is the default) */
  /* ifsP = */ 0,
  /* ifs = */ (int)0,
  /* ifsC = */ 0,
  /***** -clip: Time-domain sigma to use for clipping (0.0 = no clipping, 6.0 = default */
  /* clipP = */ 1,
  /* clip = */ 6.0,
  /* clipC = */ 1,
  /***** -noclip: Do not clip the data.  (The default is to _always_ clip!) */
  /* noclipP = */ 0,
  /***** -numout: Output this many values.  If there are not enough values in the original data file, will pad the output file with the average value */
  /* numoutP = */ 0,
  /* numout = */ (int)0,
  /* numoutC = */ 0,
  /***** -downsamp: The number of neighboring bins to co-add */
  /* downsampP = */ 1,
  /* downsamp = */ 1,
  /* downsampC = */ 1,
  /***** -nobary: Do not barycenter the data */
  /* nobaryP = */ 0,
  /***** -shorts: Use short ints for the output data instead of floats */
  /* shortsP = */ 0,
  /***** -DE405: Use the DE405 ephemeris for barycentering instead of DE200 (the default) */
  /* de405P = */ 0,
  /***** -dm: The dispersion measure to de-disperse (cm^-3 pc) */
  /* dmP = */ 1,
  /* dm = */ 0,
  /* dmC = */ 1,
  /***** -mask: File containing masking information to use */
  /* maskfileP = */ 0,
  /* maskfile = */ (char*)0,
  /* maskfileC = */ 0,
  /***** uninterpreted rest of command line */
  /* argc = */ 0,
  /* argv = */ (char**)0,
  /***** the original command line concatenated */
  /* full_cmd_line = */ NULL
};

/*@=null*/

/***** let LCLint run more smoothly */
/*@-predboolothers*/
/*@-boolops*/


/******************************************************************/
/*****
 This is a bit tricky. We want to make a difference between overflow
 and underflow and we want to allow v==Inf or v==-Inf but not
 v>FLT_MAX. 

 We don't use fabs to avoid linkage with -lm.
*****/
static void
checkFloatConversion(double v, char *option, char *arg)
{
  char *err = NULL;

  if( (errno==ERANGE && v!=0.0) /* even double overflowed */
      || (v<HUGE_VAL && v>-HUGE_VAL && (v<0.0?-v:v)>(double)FLT_MAX) ) {
    err = "large";
  } else if( (errno==ERANGE && v==0.0) 
	     || (v!=0.0 && (v<0.0?-v:v)<(double)FLT_MIN) ) {
    err = "small";
  }
  if( err ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to %s to represent\n",
	    Program, arg, option, err);
    exit(EXIT_FAILURE);
  }
}

int
getIntOpt(int argc, char **argv, int i, int *value, int force)
{
  char *end;
  long v;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  v = strtol(argv[i], &end, 0);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check if it fits into an int */
  if( errno==ERANGE || v>(long)INT_MAX || v<(long)INT_MIN ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to large to represent\n",
	    Program, argv[i], argv[i-1]);
    exit(EXIT_FAILURE);
  }
  *value = (int)v;

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr, 
	  "%s: missing or malformed integer value after option `%s'\n",
	  Program, argv[i-1]);
    exit(EXIT_FAILURE);
}
/**********************************************************************/

int
getIntOpts(int argc, char **argv, int i, 
	   int **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;
  long v;
  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values. It does not hurt to have room
    for a bit more values than cmax.
  *****/
  alloced = cmin + 4;
  *values = (int*)calloc((size_t)alloced, sizeof(int));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (int *) realloc(*values, alloced*sizeof(int));
      if( !*values ) goto outMem;
    }

    errno = 0;
    v = strtol(argv[used+i+1], &end, 0);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE || v>(long)INT_MAX || v<(long)INT_MIN ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to large to represent\n",
	      Program, argv[i+used+1], argv[i]);
      exit(EXIT_FAILURE);
    }

    (*values)[used] = (int)v;

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be an "
	    "integer value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getLongOpt(int argc, char **argv, int i, long *value, int force)
{
  char *end;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  *value = strtol(argv[i], &end, 0);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  if( errno==ERANGE ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to large to represent\n",
	    Program, argv[i], argv[i-1]);
    exit(EXIT_FAILURE);
  }
  return i;

nothingFound:
  /***** !force means: this parameter may be missing.*/
  if( !force ) return i-1;

  fprintf(stderr, 
	  "%s: missing or malformed value after option `%s'\n",
	  Program, argv[i-1]);
    exit(EXIT_FAILURE);
}
/**********************************************************************/

int
getLongOpts(int argc, char **argv, int i, 
	    long **values,
	    int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values. It does not hurt to have room
    for a bit more values than cmax.
  *****/
  alloced = cmin + 4;
  *values = calloc((size_t)alloced, sizeof(long));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = realloc(*values, alloced*sizeof(long));
      if( !*values ) goto outMem;
    }

    errno = 0;
    (*values)[used] = strtol(argv[used+i+1], &end, 0);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1; 
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to large to represent\n",
	      Program, argv[i+used+1], argv[i]);
      exit(EXIT_FAILURE);
    }

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be an "
	    "integer value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getFloatOpt(int argc, char **argv, int i, float *value, int force)
{
  char *end;
  double v;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  v = strtod(argv[i], &end);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  checkFloatConversion(v, argv[i-1], argv[i]);

  *value = (float)v;

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr,
	  "%s: missing or malformed float value after option `%s'\n",
	  Program, argv[i-1]);
  exit(EXIT_FAILURE);
 
}
/**********************************************************************/

int
getFloatOpts(int argc, char **argv, int i, 
	   float **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;
  double v;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values.
  *****/
  alloced = cmin + 4;
  *values = (float*)calloc((size_t)alloced, sizeof(float));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (float *) realloc(*values, alloced*sizeof(float));
      if( !*values ) goto outMem;
    }

    errno = 0;
    v = strtod(argv[used+i+1], &end);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    checkFloatConversion(v, argv[i], argv[i+used+1]);
    
    (*values)[used] = (float)v;
  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be a "
	    "floating-point value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

int
getDoubleOpt(int argc, char **argv, int i, double *value, int force)
{
  char *end;

  if( ++i>=argc ) goto nothingFound;

  errno = 0;
  *value = strtod(argv[i], &end);

  /***** check for conversion error */
  if( end==argv[i] ) goto nothingFound;

  /***** check for surplus non-whitespace */
  while( isspace((int) *end) ) end+=1;
  if( *end ) goto nothingFound;

  /***** check for overflow */
  if( errno==ERANGE ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of option `%s' to %s to represent\n",
	    Program, argv[i], argv[i-1],
	    (*value==0.0 ? "small" : "large"));
    exit(EXIT_FAILURE);
  }

  return i;

nothingFound:
  if( !force ) return i-1;

  fprintf(stderr,
	  "%s: missing or malformed value after option `%s'\n",
	  Program, argv[i-1]);
  exit(EXIT_FAILURE);
 
}
/**********************************************************************/

int
getDoubleOpts(int argc, char **argv, int i, 
	   double **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;
  char *end;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  /***** 
    alloc a bit more than cmin values.
  *****/
  alloced = cmin + 4;
  *values = (double*)calloc((size_t)alloced, sizeof(double));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory while parsing option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (double *) realloc(*values, alloced*sizeof(double));
      if( !*values ) goto outMem;
    }

    errno = 0;
    (*values)[used] = strtod(argv[used+i+1], &end);

    /***** check for conversion error */
    if( end==argv[used+i+1] ) break;

    /***** check for surplus non-whitespace */
    while( isspace((int) *end) ) end+=1;
    if( *end ) break;

    /***** check for overflow */
    if( errno==ERANGE ) {
      fprintf(stderr, 
	      "%s: parameter `%s' of option `%s' to %s to represent\n",
	      Program, argv[i+used+1], argv[i],
	      ((*values)[used]==0.0 ? "small" : "large"));
      exit(EXIT_FAILURE);
    }

  }
    
  if( used<cmin ) {
    fprintf(stderr, 
	    "%s: parameter `%s' of `%s' should be a "
	    "double value\n",
	    Program, argv[i+used+1], argv[i]);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

/**
  force will be set if we need at least one argument for the option.
*****/
int
getStringOpt(int argc, char **argv, int i, char **value, int force)
{
  i += 1;
  if( i>=argc ) {
    if( force ) {
      fprintf(stderr, "%s: missing string after option `%s'\n",
	      Program, argv[i-1]);
      exit(EXIT_FAILURE);
    } 
    return i-1;
  }
  
  if( !force && argv[i][0] == '-' ) return i-1;
  *value = argv[i];
  return i;
}
/**********************************************************************/

int
getStringOpts(int argc, char **argv, int i, 
	   char*  **values,
	   int cmin, int cmax)
/*****
  We want to find at least cmin values and at most cmax values.
  cmax==-1 then means infinitely many are allowed.
*****/
{
  int alloced, used;

  if( i+cmin >= argc ) {
    fprintf(stderr, 
	    "%s: option `%s' wants at least %d parameters\n",
	    Program, argv[i], cmin);
    exit(EXIT_FAILURE);
  }

  alloced = cmin + 4;
    
  *values = (char**)calloc((size_t)alloced, sizeof(char*));
  if( ! *values ) {
outMem:
    fprintf(stderr, 
	    "%s: out of memory during parsing of option `%s'\n",
	    Program, argv[i]);
    exit(EXIT_FAILURE);
  }

  for(used=0; (cmax==-1 || used<cmax) && used+i+1<argc; used++) {
    if( used==alloced ) {
      alloced += 8;
      *values = (char **)realloc(*values, alloced*sizeof(char*));
      if( !*values ) goto outMem;
    }

    if( used>=cmin && argv[used+i+1][0]=='-' ) break;
    (*values)[used] = argv[used+i+1];
  }
    
  if( used<cmin ) {
    fprintf(stderr, 
    "%s: less than %d parameters for option `%s', only %d found\n",
	    Program, cmin, argv[i], used);
    exit(EXIT_FAILURE);
  }

  return i+used;
}
/**********************************************************************/

void
checkIntLower(char *opt, int *values, int count, int max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%d\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkIntHigher(char *opt, int *values, int count, int min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%d\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkLongLower(char *opt, long *values, int count, long max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%ld\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkLongHigher(char *opt, long *values, int count, long min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%ld\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkFloatLower(char *opt, float *values, int count, float max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%f\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkFloatHigher(char *opt, float *values, int count, float min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%f\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkDoubleLower(char *opt, double *values, int count, double max)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]<=max ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' greater than max=%f\n",
	    Program, i+1, opt, max);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

void
checkDoubleHigher(char *opt, double *values, int count, double min)
{
  int i;

  for(i=0; i<count; i++) {
    if( values[i]>=min ) continue;
    fprintf(stderr, 
	    "%s: parameter %d of option `%s' smaller than min=%f\n",
	    Program, i+1, opt, min);
    exit(EXIT_FAILURE);
  }
}
/**********************************************************************/

static void
missingErr(char *opt)
{
  fprintf(stderr, "%s: mandatory option `%s' missing\n",
	  Program, opt);
}
/**********************************************************************/

static char *
catArgv(int argc, char **argv)
{
  int i;
  size_t l;
  char *s, *t;

  for(i=0, l=0; i<argc; i++) l += (1+strlen(argv[i]));
  s = (char *)malloc(l);
  if( !s ) {
    fprintf(stderr, "%s: out of memory\n", Program);
    exit(EXIT_FAILURE);
  }
  strcpy(s, argv[0]);
  t = s;
  for(i=1; i<argc; i++) {
    t = t+strlen(t);
    *t++ = ' ';
    strcpy(t, argv[i]);
  }
  return s;
}
/**********************************************************************/

void
showOptionValues(void)
{
  int i;

  printf("Full command line is:\n`%s'\n", cmd.full_cmd_line);

  /***** -o: Root of the output file names */
  if( !cmd.outfileP ) {
    printf("-o not found.\n");
  } else {
    printf("-o found:\n");
    if( !cmd.outfileC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%s'\n", cmd.outfile);
    }
  }

  /***** -pkmb: Raw data in Parkes Multibeam format */
  if( !cmd.pkmbP ) {
    printf("-pkmb not found.\n");
  } else {
    printf("-pkmb found:\n");
  }

  /***** -gmrt: Raw data in GMRT Phased Array format */
  if( !cmd.gmrtP ) {
    printf("-gmrt not found.\n");
  } else {
    printf("-gmrt found:\n");
  }

  /***** -bcpm: Raw data in Berkeley-Caltech Pulsar Machine (BPP) format */
  if( !cmd.bcpmP ) {
    printf("-bcpm not found.\n");
  } else {
    printf("-bcpm found:\n");
  }

  /***** -wapp: Raw data in Wideband Arecibo Pulsar Processor (WAPP) format */
  if( !cmd.wappP ) {
    printf("-wapp not found.\n");
  } else {
    printf("-wapp found:\n");
  }

  /***** -numwapps: Number of WAPPs used with contiguous frequencies */
  if( !cmd.numwappsP ) {
    printf("-numwapps not found.\n");
  } else {
    printf("-numwapps found:\n");
    if( !cmd.numwappsC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%d'\n", cmd.numwapps);
    }
  }

  /***** -if: A specific IF to use if available (summed IFs is the default) */
  if( !cmd.ifsP ) {
    printf("-if not found.\n");
  } else {
    printf("-if found:\n");
    if( !cmd.ifsC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%d'\n", cmd.ifs);
    }
  }

  /***** -clip: Time-domain sigma to use for clipping (0.0 = no clipping, 6.0 = default */
  if( !cmd.clipP ) {
    printf("-clip not found.\n");
  } else {
    printf("-clip found:\n");
    if( !cmd.clipC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.clip);
    }
  }

  /***** -noclip: Do not clip the data.  (The default is to _always_ clip!) */
  if( !cmd.noclipP ) {
    printf("-noclip not found.\n");
  } else {
    printf("-noclip found:\n");
  }

  /***** -numout: Output this many values.  If there are not enough values in the original data file, will pad the output file with the average value */
  if( !cmd.numoutP ) {
    printf("-numout not found.\n");
  } else {
    printf("-numout found:\n");
    if( !cmd.numoutC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%d'\n", cmd.numout);
    }
  }

  /***** -downsamp: The number of neighboring bins to co-add */
  if( !cmd.downsampP ) {
    printf("-downsamp not found.\n");
  } else {
    printf("-downsamp found:\n");
    if( !cmd.downsampC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%d'\n", cmd.downsamp);
    }
  }

  /***** -nobary: Do not barycenter the data */
  if( !cmd.nobaryP ) {
    printf("-nobary not found.\n");
  } else {
    printf("-nobary found:\n");
  }

  /***** -shorts: Use short ints for the output data instead of floats */
  if( !cmd.shortsP ) {
    printf("-shorts not found.\n");
  } else {
    printf("-shorts found:\n");
  }

  /***** -DE405: Use the DE405 ephemeris for barycentering instead of DE200 (the default) */
  if( !cmd.de405P ) {
    printf("-DE405 not found.\n");
  } else {
    printf("-DE405 found:\n");
  }

  /***** -dm: The dispersion measure to de-disperse (cm^-3 pc) */
  if( !cmd.dmP ) {
    printf("-dm not found.\n");
  } else {
    printf("-dm found:\n");
    if( !cmd.dmC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%.40g'\n", cmd.dm);
    }
  }

  /***** -mask: File containing masking information to use */
  if( !cmd.maskfileP ) {
    printf("-mask not found.\n");
  } else {
    printf("-mask found:\n");
    if( !cmd.maskfileC ) {
      printf("  no values\n");
    } else {
      printf("  value = `%s'\n", cmd.maskfile);
    }
  }
  if( !cmd.argc ) {
    printf("no remaining parameters in argv\n");
  } else {
    printf("argv =");
    for(i=0; i<cmd.argc; i++) {
      printf(" `%s'", cmd.argv[i]);
    }
    printf("\n");
  }
}
/**********************************************************************/

void
usage(void)
{
  fprintf(stderr, "usage: %s%s", Program, "\
 -o outfile [-pkmb] [-gmrt] [-bcpm] [-wapp] [-numwapps numwapps] [-if ifs] [-clip clip] [-noclip] [-numout numout] [-downsamp downsamp] [-nobary] [-shorts] [-DE405] [-dm dm] [-mask maskfile] [--] infile ...\n\
    Prepares a raw data file for pulsar searching or folding (conversion, de-dispersion, and barycentering).\n\
         -o: Root of the output file names\n\
             1 char* value\n\
      -pkmb: Raw data in Parkes Multibeam format\n\
      -gmrt: Raw data in GMRT Phased Array format\n\
      -bcpm: Raw data in Berkeley-Caltech Pulsar Machine (BPP) format\n\
      -wapp: Raw data in Wideband Arecibo Pulsar Processor (WAPP) format\n\
  -numwapps: Number of WAPPs used with contiguous frequencies\n\
             1 int value between 1 and 7\n\
             default: `1'\n\
        -if: A specific IF to use if available (summed IFs is the default)\n\
             1 int value between 0 and 1\n\
      -clip: Time-domain sigma to use for clipping (0.0 = no clipping, 6.0 = default\n\
             1 float value between 0 and 20.0\n\
             default: `6.0'\n\
    -noclip: Do not clip the data.  (The default is to _always_ clip!)\n\
    -numout: Output this many values.  If there are not enough values in the original data file, will pad the output file with the average value\n\
             1 int value between 1 and oo\n\
  -downsamp: The number of neighboring bins to co-add\n\
             1 int value between 1 and 32\n\
             default: `1'\n\
    -nobary: Do not barycenter the data\n\
    -shorts: Use short ints for the output data instead of floats\n\
     -DE405: Use the DE405 ephemeris for barycentering instead of DE200 (the default)\n\
        -dm: The dispersion measure to de-disperse (cm^-3 pc)\n\
             1 double value between 0 and oo\n\
             default: `0'\n\
      -mask: File containing masking information to use\n\
             1 char* value\n\
     infile: Input data file name.  If the data is not in PKMB or EBPP format, it should be a single channel of single-precision floating point data.  In this case a '.inf' file with the same root filename must also exist (Note that this means that the input data file must have a suffix that starts with a period)\n\
             1...100 values\n\
version: 02May03\n\
");
  exit(EXIT_FAILURE);
}
/**********************************************************************/
Cmdline *
parseCmdline(int argc, char **argv)
{
  int i;
  char missingMandatory = 0;

  Program = argv[0];
  cmd.full_cmd_line = catArgv(argc, argv);
  for(i=1, cmd.argc=1; i<argc; i++) {
    if( 0==strcmp("--", argv[i]) ) {
      while( ++i<argc ) argv[cmd.argc++] = argv[i];
      continue;
    }

    if( 0==strcmp("-o", argv[i]) ) {
      int keep = i;
      cmd.outfileP = 1;
      i = getStringOpt(argc, argv, i, &cmd.outfile, 1);
      cmd.outfileC = i-keep;
      continue;
    }

    if( 0==strcmp("-pkmb", argv[i]) ) {
      cmd.pkmbP = 1;
      continue;
    }

    if( 0==strcmp("-gmrt", argv[i]) ) {
      cmd.gmrtP = 1;
      continue;
    }

    if( 0==strcmp("-bcpm", argv[i]) ) {
      cmd.bcpmP = 1;
      continue;
    }

    if( 0==strcmp("-wapp", argv[i]) ) {
      cmd.wappP = 1;
      continue;
    }

    if( 0==strcmp("-numwapps", argv[i]) ) {
      int keep = i;
      cmd.numwappsP = 1;
      i = getIntOpt(argc, argv, i, &cmd.numwapps, 1);
      cmd.numwappsC = i-keep;
      checkIntLower("-numwapps", &cmd.numwapps, cmd.numwappsC, 7);
      checkIntHigher("-numwapps", &cmd.numwapps, cmd.numwappsC, 1);
      continue;
    }

    if( 0==strcmp("-if", argv[i]) ) {
      int keep = i;
      cmd.ifsP = 1;
      i = getIntOpt(argc, argv, i, &cmd.ifs, 1);
      cmd.ifsC = i-keep;
      checkIntLower("-if", &cmd.ifs, cmd.ifsC, 1);
      checkIntHigher("-if", &cmd.ifs, cmd.ifsC, 0);
      continue;
    }

    if( 0==strcmp("-clip", argv[i]) ) {
      int keep = i;
      cmd.clipP = 1;
      i = getFloatOpt(argc, argv, i, &cmd.clip, 1);
      cmd.clipC = i-keep;
      checkFloatLower("-clip", &cmd.clip, cmd.clipC, 20.0);
      checkFloatHigher("-clip", &cmd.clip, cmd.clipC, 0);
      continue;
    }

    if( 0==strcmp("-noclip", argv[i]) ) {
      cmd.noclipP = 1;
      continue;
    }

    if( 0==strcmp("-numout", argv[i]) ) {
      int keep = i;
      cmd.numoutP = 1;
      i = getIntOpt(argc, argv, i, &cmd.numout, 1);
      cmd.numoutC = i-keep;
      checkIntHigher("-numout", &cmd.numout, cmd.numoutC, 1);
      continue;
    }

    if( 0==strcmp("-downsamp", argv[i]) ) {
      int keep = i;
      cmd.downsampP = 1;
      i = getIntOpt(argc, argv, i, &cmd.downsamp, 1);
      cmd.downsampC = i-keep;
      checkIntLower("-downsamp", &cmd.downsamp, cmd.downsampC, 32);
      checkIntHigher("-downsamp", &cmd.downsamp, cmd.downsampC, 1);
      continue;
    }

    if( 0==strcmp("-nobary", argv[i]) ) {
      cmd.nobaryP = 1;
      continue;
    }

    if( 0==strcmp("-shorts", argv[i]) ) {
      cmd.shortsP = 1;
      continue;
    }

    if( 0==strcmp("-DE405", argv[i]) ) {
      cmd.de405P = 1;
      continue;
    }

    if( 0==strcmp("-dm", argv[i]) ) {
      int keep = i;
      cmd.dmP = 1;
      i = getDoubleOpt(argc, argv, i, &cmd.dm, 1);
      cmd.dmC = i-keep;
      checkDoubleHigher("-dm", &cmd.dm, cmd.dmC, 0);
      continue;
    }

    if( 0==strcmp("-mask", argv[i]) ) {
      int keep = i;
      cmd.maskfileP = 1;
      i = getStringOpt(argc, argv, i, &cmd.maskfile, 1);
      cmd.maskfileC = i-keep;
      continue;
    }

    if( argv[i][0]=='-' ) {
      fprintf(stderr, "\n%s: unknown option `%s'\n\n",
              Program, argv[i]);
      usage();
    }
    argv[cmd.argc++] = argv[i];
  }/* for i */

  if( !cmd.outfileP ) {
    missingErr("-o");
    missingMandatory = 1;
  }
  if( missingMandatory ) exit(EXIT_FAILURE);

  /*@-mustfree*/
  cmd.argv = argv+1;
  /*@=mustfree*/
  cmd.argc -= 1;

  if( 1>cmd.argc ) {
    fprintf(stderr, "%s: there should be at least 1 non-option argument(s)\n",
            Program);
    exit(EXIT_FAILURE);
  }
  if( 100<cmd.argc ) {
    fprintf(stderr, "%s: there should be at most 100 non-option argument(s)\n",
            Program);
    exit(EXIT_FAILURE);
  }
  /*@-compmempass*/  return &cmd;
}

