#ifdef _MSC_VER
#include <windows.h>
#endif
#include <stdio.h>

#include "data_structure.h"
#include "cosimulation_interface.h"
#include "utility.h"

/******************************************************************************
  Read the data send from the other program
******************************************************************************/
int read_cosimulation_data(PARA_DATA *para, REAL **var)
{
  int j, k;
  int imax = para->geom->imax, jmax = para->geom->jmax;
  int kmax = para->geom->kmax;
  int IMAX = imax+2, IJMAX = (imax+2)*(jmax+2);
  otherSharedData data;
  REAL feak[1];

  if(read_from_shared_memory(&data))
    exit(1);
  else
    ffd_log("cosimulation.c: read data from shared memory.", FFD_NORMAL);

  feak[0] = data.arr[0];

  /*--------------------------------------------------------------------------
  | The following code is to be modified by the users
  --------------------------------------------------------------------------*/
  for(j=0; j<=jmax+1; j++)
    for(k=0; k<=kmax+1; k++)
      var[TEMPBC][IX(imax+1,j,k)]= feak[0];

   
  printf("\ntime=%f, status=%d\n", data.t, data.status);
  printf("arr[0]=%f, arr[1]=%f, arr[2]=%f\n", data.arr[0], data.arr[1], data.arr[2]);

   //printf("message=%s\n",data.message); 

  return 0;
} // End of read_cosimulation_data()


/******************************************************************************
  Write data that will be read by other program
******************************************************************************/
int write_cosimulation_data(PARA_DATA *para, REAL **var)
{
  int i;
  int imax = para->geom->imax, jmax = para->geom->jmax;
  int kmax = para->geom->kmax;
  int IMAX = imax+2, IJMAX = (imax+2)*(jmax+2);

  ffdSharedData data;


  /*--------------------------------------------------------------------------
  | The following code is to be modified by the users
  --------------------------------------------------------------------------*/
  for(i=0; i<3; i++)
    data.number[i] = var[VX][IX(1,1,1)] + i;

  data.status = 1;
  data.t = para->mytime->t;

  strcpy(data.message, "This is FFD data\0");

  if(write_to_shared_memory(&data))
    exit(1);
  else
    ffd_log("cosimulation.c: write data to shared memory.", FFD_NORMAL);

  return 0;
} // End of write_cosimulation_data()
