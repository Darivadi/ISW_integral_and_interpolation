/****************************************************************************************************
NAME: conf2dump
FUNCTION: Reads the input file with parameters
INPUT: Parameters file
RETURN: 0
****************************************************************************************************/

int conf2dump( char filename[] )
{
  int nread;
  char cmd[1000];
  /*
  sprintf( cmd, "grep -v \"#\" %s | grep -v \"^$\" | gawk -F\"=\" '{print $2}' > %s.dump", 
	   filename, filename );
  */
  sprintf( cmd, "grep -v \"#\" %s | grep -v \"^$\" | awk -F\"=\" '{print $2}' > %s.dump", 
	   filename, filename );
  nread = system( cmd );
  return 0;
}


/****************************************************************************************************
NAME: read_parameters
FUNCTION: Reads the parameters
INPUT: Parameters file
RETURN: 0
****************************************************************************************************/
int read_parameters( char filename[] )
{
  int nread;
  char cmd[1000], filenamedump[1000];
  FILE *file;
  
  /*+++++ Loading the file +++++*/
  file = fopen( filename, "r" );
  if( file==NULL )
    {
      printf( "  * The file '%s' doesn't exist!\n", filename );
      return 1;
    }
  fclose(file);
  
  /*+++++ Converting to plain text +++++*/
  conf2dump( filename );
  sprintf( filenamedump, "%s.dump", filename );
  file = fopen( filenamedump, "r" );
  
  /*+++++ Parameters for binary data +++++*/
  nread = fscanf(file, "%d", &GV.NCELLS);
  nread = fscanf(file, "%s", GV.FILENAME);    

  fclose( file );
  
  printf( "  * The file '%s' has been loaded!\n", filename );
  
  sprintf( cmd, "rm -rf %s.dump", filename );
  nread = system( cmd );
  
  return 0;
}



/**************************************************************************
NAME: read_data
FUNCTION: reads the input file 
INPUT: GV.FILENAME variable
RETURN: 0
*****************************************************************************/
/*
int read_data(char *infile)
{
  int m, nread;
  FILE *pf=NULL;
  char buff[1000];
  double dummy;
  
  printf("Reading the file!\n");
  
  pf = fopen(infile, "r");
  
  //Ignoring the first line
  nread = fgets(buff, 1000, pf);
  
  //Reading from the second line
  for(m=0; m<GV.NTOTALCELLS; m++)
    {     
      nread=fscanf(pf,"%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", 
		   &gp[m].GID, 
		   &gp[m].pos[X], &gp[m].pos[Y], &gp[m].pos[Z], 
		   &dummy, &dummy, &dummy, 
		   &dummy, &dummy,
		   &gp[m].potDot_r,
		   &gp[m].potDot_r_l_app1, &gp[m].potDot_r_l_app2);
      
      if(m%10000000==0)
	{
	  printf("\n%d %lf %lf %lf\n", 
		 gp[m].GID,
		 gp[m].pos[X], gp[m].pos[Y], gp[m].pos[Z]);
	}//if 
    }//for m
  
  fclose(pf);
  
  return 0;
}//read_data
*/


/**************************************************************************************************** 
NAME: read_binary
FUNCTION: Reads the binary data file
INPUT: None
RETURN: 0 
****************************************************************************************************/
int read_binary(void)
{
  int i, nread;
  double pos_aux[3], dummy;
  FILE *inFile=NULL;
  
  inFile = fopen(GV.FILENAME, "r");

  printf("Reading simulation parameters\n");

  
  /*+++++ Saving Simulation parameters +++++*/
  nread = fread(&GV.BoxSize,  sizeof(double), 1, inFile);  //Box Size
  nread = fread(&GV.Omega_M0, sizeof(double), 1, inFile);  //Matter density parameter
  nread = fread(&GV.Omega_L0, sizeof(double), 1, inFile);  //Cosmological constant density parameter
  nread = fread(&GV.z_RS,     sizeof(double), 1, inFile);  //Redshift
  nread = fread(&GV.H0,       sizeof(double), 1, inFile);  //Hubble parameter
  nread = fread(&GV.NCELLS,   sizeof(int),    1, inFile);  //Number or cells

  GV.a_SF = 1.0 / (1.0 + GV.z_RS);
  GV.NTOTALCELLS = GV.NCELLS * GV.NCELLS * GV.NCELLS;
  
  printf("-----------------------------------------------\n");
  printf("Cosmological parameters:\n");
  printf("OmegaM0=%lf OmegaL0=%lf redshift=%lf HubbleParam=%lf\n",
	 GV.Omega_M0,
	 GV.Omega_L0,
	 GV.z_RS,
	 GV.H0);
  printf("-----------------------------------------------\n");
  
  printf("Simulation parameters:\n");
  printf("L=%lf\n",
	 GV.BoxSize);
  printf("-----------------------------------------------\n");

    
#if defined (CIC_400) || defined (CIC_MDR)
    
  for(i=0; i<GV.NTOTALCELLS; i++ )
    {       
      nread = fread(&pos_aux[0], sizeof(double), 3, inFile);
      
      /*----- Positions -----*/
      /*
      gp[i].pos[X] = pos_aux[X];
      gp[i].pos[Y] = pos_aux[Y];
      gp[i].pos[Z] = pos_aux[Z];
      */    
      nread = fread(&gp[i].potDot_r, sizeof(double), 1, inFile);  // PotDot 
      
            
      if(i%5000000==0)
	{
	  printf("Reading i=%d x=%lf y=%lf z=%lf PD = %lf\n", 
		 i, pos_aux[X], pos_aux[Y], pos_aux[Z],
		 gp[i].potDot_r);
	}//if

    }//for i
#endif 


#ifdef SUPERCIC    
  //nread = fread(&dummy,   sizeof(int),    1, inFile);  // Hubble parameter
  
  for(i=0; i<GV.NTOTALCELLS; i++)
    {
      /*..... File app2 .....*/
        nread = fread(&gp[i].potDot_r, sizeof(double), 1, inFile);
	
	if(i%100000000==0)
	  {
	    printf("Ready for i=%d with PotDot=%lf\n", 
		   i, gp[i].potDot_r);
	  }//if
    }//for m	      

#endif  


  printf("Data read!\n");
  fclose(inFile);
  return 0;
}//read_binary
