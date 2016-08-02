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
  char buff[1000];
  double down_lim, up_lim, aux_z, aux_dT;
  char buffer[50];
  double SW_Temp;



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

  /*+++++ Other variables +++++*/
  GV.ZERO         = 1e-30;
  GV.NTOTALCELLS  = GV.NCELLS*GV.NCELLS*GV.NCELLS;
  GV.CellSize     = GV.BoxSize/(1.0*GV.NCELLS);
  GV.c_SL = 299792.458; // km/s
  GV.CMB_T0 = 2725480; // micro K
  GV.CellStep = GV.CellSize / 2.0;
  
  /*----- Integration limits -----*/
  down_lim = 0.0;
  up_lim = GV.BoxSize;
  
  printf("NCells=%d\n", GV.NCELLS);
  printf("--------------------------------------------------\n");
  
  /*+++++ Memory allocation +++++*/
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

  
  pf = fopen( "./../../Processed_data/SWIntegral_Exact.dat", "w" );  
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
	  SW_Temp = GV.a_SF*interp_integ_potdot_dx(down_lim, up_lim);
	  
	  if(n%10000==0)
	    {
	      printf("n=%12d SW_temp=%16.8lf\n", n, SW_Temp);
	    }//if
	  
	  fprintf( pf, 
		   "%12d %16.8f %16.8f %16.8f\n",
		   n, gp[n].pos[X], gp[n].pos[Y], SW_Temp);
	}//for j 
    }//for i
#endif
  
  fclose(pf);
  
  free(z_depth);
  free(PotDot);
  
  printf("Interpolation finished\n");
  printf("-----------------------------------------\n");
  
  printf("Performing dT/dr\n");
  printf("-----------------------------------------\n");
  

  printf("Allocating memory\n");
  z_depth = (double *) calloc(GV.NCELLS, sizeof(double));
  PotDot  = (double *) calloc(GV.NCELLS, sizeof(double));
  printf("Memory allocated\n");


  for( i=0; i<GV.NCELLS; i++ )
    {
      for( j=0; j<GV.NCELLS; j++ )
	{
	  n = INDEX_C_2D(i,j); 
	  
	  fill_potdot_xy(i, j);	
	  dT_dr = dT_dr_gsl(i, j);
	 
	  if( (i==0 && j==0) || (i==64 && j==64) || (i==128 && j==128) || (i==256 && j==256) )
	    {
	  
	      snprintf(buffer, sizeof(char)*50, "./../../Processed_data/dT_dr_i%d_j%d.txt", i, j);
	      pf = fopen(buffer, "w");
	      	      
	      for( k=0; k<GV.NCELLS; k++ )
		{
		  m = INDEX_C_ORDER(i,j,k);
		  
		  fprintf(pf, "%16.8lf %16.8lf\n", gp[m].pos[Z], dT_dr[k]);
		}//for k
	      
	      fclose(pf);

	      /*
	      snprintf(buffer, sizeof(char)*50, "./../../Processed_data/dT_dr_i%d_j%d.bin", i, j);
	      pf = fopen(buffer, "w");
	      
	      fwrite(&i, sizeof(int), 1, pf);
	      fwrite(&j, sizeof(int), 1, pf);
	      
	      for( k=0; k<GV.NCELLS; k++ )
		{
		  m = INDEX_C_ORDER(i,j,k);
		  aux_z = gp[m].pos[Z];
		  aux_dT = dT_dr[k];
		  
		  fwrite(&aux_z, sizeof(double), 1, pf);
		  fwrite(&aux_dT, sizeof(double), 1, pf);
		}//for k
	      
		fclose(pf);
	      */
	    }//if

	}//for j
    }//for i

  
    
  
  printf("Code finished!\n");  
  printf("-----------------------------------------\n");

}//main
