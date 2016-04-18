/****************************************************************************************************
                       HEADERS
****************************************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_sort_float.h>


/*************************************************************************************
                           DEFINITION OF GLOBAL VARIABLES
*************************************************************************************/
extern double *z_depth=NULL, *PotDot=NULL;


/*************************************************************************************
                       INCLUDING SUPPORT FILES
*************************************************************************************/
#include "variables.c"
#include "reading.c"
#include "interp_PotDot_of_Z.c"
#include "interp_integ_gsl.c"


/*******************************************************************
NAME: main
FUNCTION: executes the read_data(), interpolation() and SWintegral() functions
INPUT: data file
RETURN: none
******************************************************************/

int main(int argc, char *argv[])
{
  int i, j, k, n, m;
  double z, *dT_dr=NULL; 
  char *infile=NULL;
  FILE *pf=NULL;
  FILE *pf1=NULL;
  char buff[1000];


  if(argc < 2)
    {
      printf("Error: Incomplete number of parameters. Execute as follows:\n");
      printf("%s Parameters_file\n", argv[0]);
      exit(0);      
    }//if
    
  infile = argv[1];
  
  /*+++++ Reading parameters +++++*/
  printf("Reading parameters file\n");
  printf("-----------------------------------------\n");
  read_parameters( infile );

  /*+++ Other variables +++*/
  GV.ZERO         = 1e-30;
  GV.NTOTALCELLS  = GV.NCELLS*GV.NCELLS*GV.NCELLS;
  GV.CellSize     = GV.BoxSize/(1.0*GV.NCELLS);
  GV.c_SL = 299792.458; // km/s
  GV.CMB_T0 = 2725480; // micro K
  GV.CellStep = GV.CellSize / 2.0;
  
  printf("NCells=%d\n", GV.NCELLS);
  printf("--------------------------------------------------\n");
  
  /*+++ Memory allocation +++*/
  gp     = (struct grid *) malloc((size_t) GV.NTOTALCELLS*sizeof(struct grid));
  printf("Memory allocated!\n");
  printf("--------------------------------------------------\n");
  

  /*+++++ Reading datafile +++++*/
  printf("Reading the file...\n");
  printf("-----------------------------------------\n");
#ifdef BINARYDATA
  read_binary();
#endif

#ifdef ASCIIDATA
  read_data(GV.FILENAME);
#endif
  

  printf("File read!\n");
  printf("--------------------------------------------------\n");
      

  //---------------------------------------------------------
  /*Interpolation of values from exact PotDot*/
  //---------------------------------------------------------  
  printf("Allocating memory\n");
  //z_depth = (double *) malloc((size_t) GV.NCELLS*sizeof(double));
  //PotDot  = (double *) malloc((size_t) GV.NCELLS*sizeof(double));
  z_depth = (double *) calloc(GV.NCELLS, sizeof(double));
  PotDot  = (double *) calloc(GV.NCELLS, sizeof(double));
  printf("Memory allocated\n");

  
#ifdef POTDOTEXACT
  pf = fopen( "./../Processed_data/SWIntegral_Exact.dat", "w" );
#endif
  
  
#ifdef POTDOTAPP1
  pf = fopen( "./../Processed_data/SWIntegral_App1.dat", "w" );
#endif
  
  
#ifdef POTDOTAPP2
  pf = fopen( "./../Processed_data/SWIntegral_App2.dat", "w" );
#endif
    
  fprintf(pf, "#n\t x\t y\t SW_Integral\n");


  printf("Interpolation\n");
#ifdef SIMPSON  
  printf("Beginning interpolation and integration with Simpson method");
  for(i=0; i<GV.NCELLS; i++)
    {                                                                                                          
      for(j=0; j<GV.NCELLS; j++)                                                                                
	{                                                                                                       
	  n = INDEX_C_2D(i,j);                                                                                  
	  fill_potdot_xy(i, j); // this one builds pot_dot(z)
	  
	   fprintf( pf, 
		    "%12d %16.8f %16.8f %16.8f\n", 
		    n, gp[n].pos[X], gp[n].pos[Y], GV.a_SF*SW_integral() ); 
	}//for j 
    }//for i
#endif 

  
#ifdef GSLINTERPINTEG
  printf("Beginning interpolation and integration with gsl methods\n");
  for(i=0; i<GV.NCELLS; i++)
    {                                                                                                          
      for(j=0; j<GV.NCELLS; j++)                                                                               
	{                                                                                                      
	  n = INDEX_C_2D(i,j);                                                                                 
	  fill_potdot_xy(i, j); // this one builds pot_dot(z)	 
	  fprintf( pf, 
		   "%12d %16.8f %16.8f %16.8f\n",
		   n, gp[n].pos[X], gp[n].pos[Y], GV.a_SF*interp_integ_potdot_dx() ); 
	}//for j 
    }//for i
#endif
  

  fclose(pf);
  
  free(z_depth);
  free(PotDot);
  
  printf("Interpolation finished\n");
  printf("-----------------------------------------\n");
  
    
  
  printf("Code finished!\n");  
  printf("-----------------------------------------\n");

}//main
