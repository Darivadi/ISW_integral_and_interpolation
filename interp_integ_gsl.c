/****************************************************************************************************
NAME: interp_integ_gsl
FUNCTION: 
INPUT: 
RETURN: 
****************************************************************************************************/

double interp_integ_potdot_dx(double down_lim, double up_lim)
{
  //double down_lim, up_lim;
  double Integ_ofZ;
  gsl_interp_accel *acc;
  gsl_spline *linearInterp;

  
  down_lim = 0.0;
  up_lim = GV.BoxSize;

  acc = gsl_interp_accel_alloc();
  linearInterp = gsl_spline_alloc(gsl_interp_linear, (size_t) GV.NCELLS );
  gsl_spline_init( linearInterp, z_depth, PotDot, (size_t) GV.NCELLS ); 
  Integ_ofZ = gsl_spline_eval_integ (linearInterp, down_lim, up_lim, acc);
  
  gsl_spline_free(linearInterp);
  gsl_interp_accel_free(acc);
  
  return Integ_ofZ;
}//interp_integ_potdot_dx


/****************************************************************************************************
NAME: fill_potdot_dTdr_xy
FUNCTION: UNDER CONSTRUCTION!
INPUT: 
RETURN: 
****************************************************************************************************/
double fill_potdot_dTdr_xy(int i, int j, int k_lim)
{  
  int m, k;
  
  for(k=0; k<k_lim; k++)
    { 
      m = INDEX_C_ORDER(i,j,k);
      
      z_depth[k] = gp[m].pos[Z];
      PotDot[k]  = gp[m].potDot_r;
    }//for k 
    
  z_depth[0] = 0.0;
  z_depth[k_lim-1] = GV.BoxSize;

  return 0;
  
}//fill_potdot_xy





/****************************************************************************************************
NAME: dT_dr_gsl
FUNCTION: UNDER CONSTRUCTION!
INPUT: 
RETURN: 
****************************************************************************************************/
double *dT_dr_gsl(int i, int j)
{
  double *T_depth=NULL, *DeltaT=NULL, *dT_dr=NULL, dr;
  int m, n, k;


  T_depth  = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  DeltaT   = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  dT_dr    = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );


  for( k=(GV.NCELLS-1); k>=0; --k )
    {

      m = INDEX_C_ORDER(i,j,k);            
      
            
      if( k != (GV.NCELLS-1) )
	{
	  T_depth[k] = interp_integ_potdot_dx(gp[m].pos[Z]-GV.CellStep, GV.BoxSize); 
	  	  
	  DeltaT[k] = T_depth[k+1] - T_depth[k];
	}//if
      else
	{
	  DeltaT[k] = T_depth[k] = gp[m].potDot_r; 	  
	}//else
      
      
      dr = GV.BoxSize / (1.0 * GV.NCELLS); 
      dT_dr[k] = DeltaT[k] / dr ;
      

      if(m == 0 || m == 256)
	{
	  printf("m=%d, k=%d, z_depth=%lf, T=%lf\n", m, k, z_depth[k], T_depth[k]);
	}//if
      

      //printf("m=%d i=%d j=%d k=%d posZ=%lf T_depth=%lf DeltaT=%lf dT_dr=%lf\n", m, i, j, k, gp[m].pos[Z], T_depth[k], DeltaT[k], dT_dr[k]);
            
    }//for k

  free(T_depth);
  free(DeltaT);

  return dT_dr;
}//dT_dr

