/****************************************************************************************************
NAME: fill_potDot
FUNCTION: Fills the PotDot array in order to interpolate
INPUT: 
RETURN: 
****************************************************************************************************/
/*+++++ Along x-axis as LOS +++++*/
#ifdef XLOS
double fill_potdot_yz(int j, int k)
{  
  int m, i;
  
  for(i=0; i<GV.NCELLS; i++)
    { 
      m = INDEX_C_ORDER(i,j,k);
      PotDot[i]  = gp[m].potDot_r;
      
    }//for i
    
  z_depth[0] = 0.0;
  z_depth[GV.NCELLS-1] = GV.BoxSize;
  
  return 0;
}//fill_potdot_yz
#endif


/*+++++ Along z-axis as LOS +++++*/
#ifdef YLOS
double fill_potdot_xz(int i, int k)
{  
  int m, j;
  
  for(j=0; j<GV.NCELLS; j++)
    { 
      m = INDEX_C_ORDER(i,j,k);
      PotDot[j]  = gp[m].potDot_r;
    }//for j
    
  z_depth[0] = 0.0;
  z_depth[GV.NCELLS-1] = GV.BoxSize;

  return 0;
}//fill_potdot_xy
#endif


/*+++++ Along z-axis as LOS +++++*/
#ifdef ZLOS
double fill_potdot_xy(int i, int j)
{  
  int m, k;
  
  for(k=0; k<GV.NCELLS; k++)
    { 
      m = INDEX_C_ORDER(i,j,k);
      PotDot[k]  = gp[m].potDot_r;

      /*
      if(m%10000==0)
	{
	  printf("m=%10d z_depth=%16.8lf PotDot=%16.8lf\n", 
		 m, z_depth[k], PotDot[k]);
	}//if
      */
    }//for k 
    
  z_depth[0] = 0.0;
  z_depth[GV.NCELLS-1] = GV.BoxSize;

  return 0;
}//fill_potdot_xy
#endif


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

  
  //down_lim = 0.0;
  //up_lim = GV.BoxSize;

  acc = gsl_interp_accel_alloc();
  linearInterp = gsl_spline_alloc(gsl_interp_linear, (size_t) GV.NCELLS );
  gsl_spline_init( linearInterp, z_depth, PotDot, (size_t) GV.NCELLS ); 
  Integ_ofZ = gsl_spline_eval_integ(linearInterp, down_lim, up_lim, acc);
  
  gsl_spline_free(linearInterp);
  gsl_interp_accel_free(acc);
  
  return Integ_ofZ;
}//interp_integ_potdot_dx


/****************************************************************************************************
NAME: dT_dr_gsl
FUNCTION: UNDER CONSTRUCTION!
INPUT: 
RETURN: 
****************************************************************************************************/
/*+++++ Along x-axis as LOS +++++*/
#ifdef XLOS
double *dT_dr_gsl_yz(int j, int k)
{
  double *T_depth=NULL, *DeltaT=NULL, *dT_dr=NULL, dr;
  double down_lim, up_lim;
  int m, n, i;

  up_lim = GV.BoxSize;

  T_depth  = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  DeltaT   = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  dT_dr    = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );


  for( i=(GV.NCELLS-1); i>=0; --i )
    {
      m = INDEX_C_ORDER(i,j,k);            
                  
      if( i != (GV.NCELLS-1) )
	{
	  if(i!= 0)
	    {
	      down_lim = z_depth[i]-GV.CellStep;
	    }//if k!=0
	  else
	    {
	      down_lim = 0.0;
	    }


	  T_depth[i] = interp_integ_potdot_dx(down_lim, up_lim); 
	  	  
	  dT_dr[i] = T_depth[i+1] - T_depth[i];
	}//if
      else
	{
	  dT_dr[i] = T_depth[i] = gp[m].potDot_r;
	}//else
            
      dr = GV.BoxSize / (1.0 * GV.NCELLS); 
      dT_dr[i] /= dr ;
      /*
      if( (j==0 && k==0)  )
	{
	  printf("m=%d, i=%d, j=%d k=%d, z_depth=%lf, downlim= %lf, T=%lf\n", 
		 m, i, j, k, z_depth[i], down_lim, T_depth[i]); 
	}//if
      */               
    }//for i

  free(T_depth);
  free(DeltaT);

  return dT_dr;
}//dT_dr
#endif //XLOS


/*+++++ Along y-axis as LOS +++++*/
#ifdef YLOS
double *dT_dr_gsl_xz(int i, int k)
{
  double *T_depth=NULL, *DeltaT=NULL, *dT_dr=NULL, dr;
  double down_lim, up_lim;
  int m, n, j;

  up_lim = GV.BoxSize;

  T_depth  = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  DeltaT   = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  dT_dr    = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );

  
  for( j=(GV.NCELLS-1); j>=0; --j )
    {
      m = INDEX_C_ORDER(i,j,k);            
                  
      if( j != (GV.NCELLS-1) )
	{
	  if(j!= 0)
	    {
	      down_lim = z_depth[j]-GV.CellStep;
	    }//if k!=0
	  else
	    {
	      down_lim = 0.0;
	    }

	  T_depth[j] = interp_integ_potdot_dx(down_lim, up_lim); 
	  	  
	  dT_dr[j] = T_depth[j+1] - T_depth[j];
	}//if
      else
	{
	  dT_dr[j] = T_depth[j] = gp[m].potDot_r; 	  
	}//else
            
      dr = GV.BoxSize / (1.0 * GV.NCELLS); 
      dT_dr[j] /= dr ;
      /*
      if( (i==0 && k==0)  )
	{
	  printf("m=%d, i=%d, j=%d k=%d, z_depth=%lf, downlim= %lf, T=%lf\n", m, i, j, k, z_depth[j], down_lim, T_depth[j]); 
	}//if
      */               
    }//for k

  free(T_depth);
  free(DeltaT);

  return dT_dr;
}//dT_dr
#endif //YLOS


/*+++++ Along z-axis as LOS +++++*/
#ifdef ZLOS
double *dT_dr_gsl_xy(int i, int j)
{
  double *T_depth=NULL, *DeltaT=NULL, *dT_dr=NULL, dr;
  double down_lim, up_lim;
  int m, n, k;

  up_lim = GV.BoxSize;

  T_depth  = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  DeltaT   = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );
  dT_dr    = (double *) malloc((size_t) GV.NCELLS*sizeof(double) );


  for( k=(GV.NCELLS-1); k>=0; --k )
    {
      m = INDEX_C_ORDER(i,j,k);            
                  
      if( k != (GV.NCELLS-1) )
	{
	  if(k!= 0)
	    {
	      down_lim = z_depth[k]-GV.CellStep;
	    }//if k!=0
	  else
	    {
	      down_lim = 0.0;
	    }

	  T_depth[k] = interp_integ_potdot_dx(down_lim, up_lim); 
	  	  
	  dT_dr[k] = T_depth[k+1] - T_depth[k];
	}//if
      else
	{
	  dT_dr[k] = T_depth[k] = gp[m].potDot_r; 	  
	}//else
            
      dr = GV.BoxSize / (1.0 * GV.NCELLS); 
      dT_dr[k] /= dr ;
      /*
      if( (i==0 && j==0)  )
	{
	  printf("m=%d, i=%d, j=%d k=%d, z_depth=%lf, downlim= %lf, T=%lf\n", m, i, j,k, z_depth[k], down_lim, T_depth[k]); 
	}//if
      */               
    }//for k

  free(T_depth);
  free(DeltaT);

  return dT_dr;
}//dT_dr
#endif //ZLOS
