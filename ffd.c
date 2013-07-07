/*----------------------------------------------------------------------------
  
  Filename: ffd.c

  Written by:  Wangda Zuo

  Last Modified: Wangda Zuo on 7/7/2013

  Task: Main routine of Fast Fluid Dynamics

---------------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "data_structure.h"
#include "solver.h"
#include "write_data.h"
#include "initialization.h"
#include "boundary.h"
#include "read_data.h"
#include "grid.h"
#include "timing.h"
#include "sci_reader.h"

/* global variables */
static REAL dt, diff, visc;
static REAL force, source;
static int screen;

REAL **var;
int  **BINDEX;
int  *xindex,*yindex,*zindex,*fltemp;
REAL *x, *y, *z, *gx, *gy, *gz;
REAL *u, *v, *w, *u_s, *v_s, *w_s, *u_mean, *v_mean, *w_mean;
REAL *dens, *dens_s, *temp, *temp_s, *temp_mean, *p, *my_div, *pp;
REAL *tmp1, *tmp2, *tmp3;
REAL *ap, *an, *as, *aw, *ae, *b, *ab, *af, *ap0;
REAL *flagp, *flagu, *flagv, *flagw;
REAL *locmin,*locmax;
REAL *vxbc,*vybc,*vzbc,*tempbc;

static GEOM_DATA geom;
static PROB_DATA prob;
static TIME_DATA mytime;
static OUTP_DATA outp1;
static BC_DATA bc;
static SOLV_DATA solv;
static PARA_DATA para;


clock_t start, end;

/******************************************************************************
| allocate data
******************************************************************************/
int allocate_data ( void )
{
  int size = (geom.imax+2) * (geom.jmax+2) * (geom.kmax+2);
  printf( "size=%d\n", size); 
  x         = (REAL *) malloc ( size*sizeof(REAL) );
  y         = (REAL *) malloc ( size*sizeof(REAL) );
  z         = (REAL *) malloc ( size*sizeof(REAL) );
  u         = (REAL *) malloc ( size*sizeof(REAL) );
  v         = (REAL *) malloc ( size*sizeof(REAL) );
  w         = (REAL *) malloc ( size*sizeof(REAL) );
  u_s       = (REAL *) malloc ( size*sizeof(REAL) );
  v_s       = (REAL *) malloc ( size*sizeof(REAL) );
  w_s       = (REAL *) malloc ( size*sizeof(REAL) );
  u_mean    = (REAL *) malloc ( size*sizeof(REAL) );
  v_mean    = (REAL *) malloc ( size*sizeof(REAL) );
  w_mean    = (REAL *) malloc ( size*sizeof(REAL) );
  temp      = (REAL *) malloc ( size*sizeof(REAL) );
  temp_s    = (REAL *) malloc ( size*sizeof(REAL) );
  temp_mean = (REAL *) malloc ( size*sizeof(REAL) );
  dens      = (REAL *) malloc ( size*sizeof(REAL) );
  dens_s    = (REAL *) malloc ( size*sizeof(REAL) );
  p         = (REAL *) malloc ( size*sizeof(REAL) ); 
  tmp1      = (REAL *) malloc ( size*sizeof(REAL) );  
  tmp2      = (REAL *) malloc ( size*sizeof(REAL) );  
  tmp3      = (REAL *) malloc ( size*sizeof(REAL) );  
  ap        = (REAL *) malloc ( size*sizeof(REAL) );
  an        = (REAL *) malloc ( size*sizeof(REAL) );
  as        = (REAL *) malloc ( size*sizeof(REAL) );
  aw        = (REAL *) malloc ( size*sizeof(REAL) );
  ae        = (REAL *) malloc ( size*sizeof(REAL) );
  ab        = (REAL *) malloc ( size*sizeof(REAL) );
  af        = (REAL *) malloc ( size*sizeof(REAL) );
  b         = (REAL *) malloc ( size*sizeof(REAL) );
  gx        = (REAL *) malloc ( size*sizeof(REAL) );  
  gy        = (REAL *) malloc ( size*sizeof(REAL) );
  gz        = (REAL *) malloc ( size*sizeof(REAL) );
  ap0       = (REAL *) malloc ( size*sizeof(REAL) );
  pp        = (REAL *) malloc ( size*sizeof(REAL) );
  flagp     = (REAL *) malloc ( size*sizeof(REAL) );
  flagu     = (REAL *) malloc ( size*sizeof(REAL) );
  flagv     = (REAL *) malloc ( size*sizeof(REAL) );
  flagw     = (REAL *) malloc ( size*sizeof(REAL) );
  locmin    = (REAL *) malloc ( size*sizeof(REAL) );
  locmax    = (REAL *) malloc ( size*sizeof(REAL) );
  vxbc      = (REAL *) malloc ( size*sizeof(REAL) );
  vybc      = (REAL *) malloc ( size*sizeof(REAL) );
  vzbc      = (REAL *) malloc ( size*sizeof(REAL) );
  tempbc    = (REAL *) malloc ( size*sizeof(REAL) );
  var	      = (REAL **) malloc ( 44*sizeof(REAL*) );
  
  var[X]      = x;
  var[Y]      = y;
  var[Z]      = z;
  var[VX]     = u;
  var[VY]     = v;
  var[VZ]     = w;
  var[VXS]    = u_s;
  var[VYS]    = v_s;
  var[VZS]    = w_s;
  var[VXM]    = u_mean;
  var[VYM]    = v_mean;
  var[VZM]    = w_mean;
  var[DEN]    = dens;
  var[DENS]   = dens_s;
  var[IP]     = p;
  var[TEMP]   = temp;
  var[TEMPS]  = temp_s;
  var[TEMPM]  = temp_mean;
  var[AP]     = ap;
  var[AN]     = an;
  var[AS]     = as;
  var[AW]     = aw;
  var[AE]     = ae;
  var[AB]     = ab;
  var[AF]     = af;
  var[B]      = b;
  var[TMP1]   = tmp1;
  var[TMP2]   = tmp2;
  var[TMP3]   = tmp3;
  var[GX]     = gx;
  var[GY]     = gy;
  var[GZ]     = gz;
  var[AP0]    = ap0;
  var[PP]     = pp;
  var[FLAGP]  =flagp;
  var[FLAGU]  =flagu;
  var[FLAGV]  =flagv;
  var[FLAGW]  =flagw;
  var[LOCMIN] =locmin;
  var[LOCMAX] =locmax;
  var[VXBC]   =vxbc;
  var[VYBC]   =vybc;
  var[VZBC]   =vzbc;
  var[TEMPBC] =tempbc;

  xindex = (int *) malloc ( size*sizeof(int) );
  yindex = (int *) malloc ( size*sizeof(int) );
  zindex = (int *) malloc ( size*sizeof(int) );
  fltemp = (int *) malloc ( size*sizeof(int) );
  BINDEX = (int **) malloc ( 4*sizeof(int*) );

  BINDEX[0] = xindex;
  BINDEX[1] = yindex;
  BINDEX[2] = zindex;
  BINDEX[3] = fltemp;


  if( !x || !y || !z || !u || !v || !w || !u_s || !v_s || !w_s || 
      !u_mean || !v_mean || !w_mean || 
      !dens || !dens_s || !temp || !temp_s || !temp_mean || 
      !tmp1 || !tmp2 || !tmp3 ||
      !ap || !ae || !aw || !as || !an || !ab || !af || !b || !gx || !gy || !gz || !ap0 || !pp || !flagp ||
      ! flagu || ! flagv || ! flagw || ! locmin || ! locmax ||
      ! vxbc ||! vybc ||! vzbc ||! tempbc||! xindex ||! yindex ||! zindex) 
  {
    fprintf( stderr, "cannot allocate data\n" );
    return 1;
  }

  return 0;
} /** allocate_data() **/


/******************************************************************************
   main --- main routine
******************************************************************************/
int main()
{ 
  // Initialize the parameters
  para.geom = &geom;
  para.outp = &outp1;
  para.prob = &prob;
  para.mytime = &mytime;
  para.bc     = &bc;
  para.solv   = &solv;
  if(initialize(&para)) exit(1);
  
  // Read the mesh and simulation data from SCI genreated file
  if(read_max(&para, var)) exit(1);

  printf("imax= %d\t jmax= %d\t  kmax= %d\n ", para.geom->imax,para.geom->jmax,para.geom->kmax);

  // Allocate memory for the variables
  if(allocate_data( )) exit(1);

  // Set the initial values for the simulation data
  if(set_initial_data(&para, var)) exit(1);

  if(!read_input(&para, var,BINDEX)) {printf("no file"); exit(1);}
 if(!read_zeroone(&para, var,BINDEX)) {printf("no file"); exit(1);}

  mark_cell(&para, var);

   if(para.solv->read_file==1) read_data(&para,var);

   FFD_solver(&para, var,BINDEX);


	 write_SCI(&para,  var, "output");

     free_data(var);
 	 free_index(BINDEX);

	  getchar();

  
  
	exit ( 0 );
} // End of main( )