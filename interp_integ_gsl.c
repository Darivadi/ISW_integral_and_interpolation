/****************************************************************************************************
NAME: interp_integ_gsl
FUNCTION: 
INPUT: 
RETURN: 
****************************************************************************************************/

double interp_integ_potdot_dx(void)
{
  double Integ_ofZ;
  gsl_interp_accel *acc;
  gsl_spline *linearInterp;
  
  down_lim = 0.0;
  up_lim = GV.BoxSize;

  acc = gsl_interp_accel_alloc();
  linearInterp = gsl_spline_alloc(gsl_interp_linear, (size_t) GV.NCELLS );
  gsl_spline_init( linearInterp, z_depth, PotDot, (size_t) GV.NCELLS ); 
  gsl_interp_eval_integ (linearInterp, z_depth, PotDot, down_lim, up_lim,  acc);
  
  gsl_spline_free(linearInterp);
  gsl_interp_accel_free(acc);
  
  return Integ_ofZ;
}//interp_integ_potdot_dx
