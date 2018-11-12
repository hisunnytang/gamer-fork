#include "GAMER.h"
#include "TestProb.h"



// problem-specific global variables
// =======================================================================================
static double SphCol_Dens_Bg;       // background mass density
static double SphCol_Dens_Delta;    // top-hat mass density --> total density = Dens_Bg*( 1 + Dens_Delta )
static double SphCol_Engy_Bg;       // background energy density
static double SphCol_Radius;        // top-hat radius
static double SphCol_Center[3];     // top-hat center
static double GasTemp;
static double MetalMassFrac;
static double MolecularHydrogenMassFrac;
static double NFW_Rho;
static double NFW_Rs;
static double smoothing_length;
static double gas_Rs;
// =======================================================================================




//-------------------------------------------------------------------------------------------------------
// Function    :  Validate
// Description :  Validate the compilation flags and runtime parameters for this test problem
//
// Note        :  None
//
// Parameter   :  None
//
// Return      :  None
//-------------------------------------------------------------------------------------------------------
void Validate()
{

   if ( MPI_Rank == 0 )    Aux_Message( stdout, "   Validating test problem %d ...\n", TESTPROB_ID );


#  if ( MODEL != HYDRO )
   Aux_Error( ERROR_INFO, "MODEL != HYDRO !!\n" );
#  endif

#  ifndef GRAVITY
   Aux_Error( ERROR_INFO, "GRAVITY must be enabled !!\n" );
#  endif

//#  ifndef COMOVING
//   Aux_Error( ERROR_INFO, "COMOVING must be enabled !!\n" );
//#  endif

#  ifdef PARTICLE
   Aux_Error( ERROR_INFO, "PARTICLE must be disabled !!\n" );
#  endif

   if ( MPI_Rank == 0 )
   {
#     ifdef GRAVITY
      if ( OPT__GRAVITY_TYPE != GRAVITY_SELF )
         Aux_Message( stderr, "WARNING : OPT__GRAVITY_TYPE != GRAVITY_SELF ??\n" );
#     endif

#     ifndef DUAL_ENERGY
         Aux_Message( stderr, "WARNING : it's recommended to enable DUAL_ENERGY for this test\n" );
#     endif

      if ( OPT__BC_FLU[0] == BC_FLU_PERIODIC )
         Aux_Message( stderr, "WARNING : periodic BC ??\n" );

      if ( amr->BoxSize[0] != amr->BoxSize[1]  ||  amr->BoxSize[0] != amr->BoxSize[2] )
         Aux_Message( stderr, "WARNING : simulation domain is not cubic ??\n" );
   }


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "   Validating test problem %d ... done\n", TESTPROB_ID );

} // FUNCTION : Validate



#if ( MODEL == HYDRO )
//-------------------------------------------------------------------------------------------------------
// Function    :  SetParameter
// Description :  Load and set the problem-specific runtime parameters
//
// Note        :  1. Filename is set to "Input__TestProb" by default
//                2. Major tasks in this function:
//                   (1) load the problem-specific runtime parameters
//                   (2) set the problem-specific derived parameters
//                   (3) reset other general-purpose parameters if necessary
//                   (4) make a note of the problem-specific parameters
//
// Parameter   :  None
//
// Return      :  None
//-------------------------------------------------------------------------------------------------------
void SetParameter()
{

   if ( MPI_Rank == 0 )    Aux_Message( stdout, "   Setting runtime parameters ...\n" );


// (1) load the problem-specific runtime parameters
   const char FileName[] = "Input__TestProb";
   ReadPara_t *ReadPara  = new ReadPara_t;

// add parameters in the following format:
// --> note that VARIABLE, DEFAULT, MIN, and MAX must have the same data type
// --> some handy constants (e.g., NoMin_int, Eps_float, ...) are defined in "include/ReadPara.h"
// ********************************************************************************************************************************
// ReadPara->Add( "KEY_IN_THE_FILE",   &VARIABLE,              DEFAULT,       MIN,              MAX               );
// ********************************************************************************************************************************
   ReadPara->Add( "SphCol_Dens_Bg",    &SphCol_Dens_Bg,        -1.0,          Eps_double,       NoMax_double      );
   ReadPara->Add( "SphCol_Dens_Delta", &SphCol_Dens_Delta,     -1.0,          Eps_double,       NoMax_double      );
   ReadPara->Add( "SphCol_Engy_Bg",    &SphCol_Engy_Bg,        -1.0,          Eps_double,       NoMax_double      );
   ReadPara->Add( "SphCol_Radius",     &SphCol_Radius,         -1.0,          Eps_double,       NoMax_double      );
   ReadPara->Add( "SphCol_Center_X",   &SphCol_Center[0],      -1.0,          NoMin_double,     NoMax_double      );
   ReadPara->Add( "SphCol_Center_Y",   &SphCol_Center[1],      -1.0,          NoMin_double,     NoMax_double      );
   ReadPara->Add( "SphCol_Center_Z",   &SphCol_Center[2],      -1.0,          NoMin_double,     NoMax_double      );

// READ PASSIVE METAL FIELD mass fraction
   ReadPara->Add( "MetalMassFrac",     &MetalMassFrac,          -1.0,          0.0,              1.0               );
   ReadPara->Add( "MolecularHydrogenMassFrac",     &MolecularHydrogenMassFrac,          -1.0,          0.0,              1.0               );
// REDA initial temperature
   ReadPara->Add( "GasTemp",           &GasTemp,               -1.0,          Eps_double,        NoMax_double      );
   ReadPara->Add( "NFW_Rho",           &NFW_Rho,               -1.0,          Eps_double,        NoMax_double      );
   ReadPara->Add( "NFW_Rs",            &NFW_Rs,                -1.0,          Eps_double,        NoMax_double      );
   ReadPara->Add( "smoothing_length",  &smoothing_length,      -1.0,          Eps_double,        NoMax_double      );
   ReadPara->Add( "gas_Rs",            &gas_Rs,                -1.0,          Eps_double,        NoMax_double      );

   ReadPara->Read( FileName );

   delete ReadPara;

// set the default values
   for (int d=0; d<3; d++)
      if ( SphCol_Center[d] < 0.0 )    SphCol_Center[d] = 0.5*amr->BoxSize[d];


// (2) set the problem-specific derived parameters


// (3) reset other general-purpose parameters
//     --> a helper macro PRINT_WARNING is defined in TestProb.h
   const long   End_Step_Default = __INT_MAX__;
   const double End_T_Default    = 5.0e-2;

   if ( END_STEP < 0 ) {
      END_STEP = End_Step_Default;
      PRINT_WARNING( "END_STEP", END_STEP, FORMAT_LONG );
   }

   if ( END_T < 0.0 ) {
      END_T = End_T_Default;
      PRINT_WARNING( "END_T", END_T, FORMAT_REAL );
   }


// (4) make a note
   if ( MPI_Rank == 0 )
   {
      Aux_Message( stdout, "=============================================================================\n" );
      Aux_Message( stdout, "  test problem ID           = %d\n",     TESTPROB_ID       );
      Aux_Message( stdout, "  background mass density   = %13.7e\n", SphCol_Dens_Bg    );
      Aux_Message( stdout, "  top-hat over-density      = %13.7e\n", SphCol_Dens_Delta );
      Aux_Message( stdout, "  background energy density = %13.7e\n", SphCol_Engy_Bg    );
      Aux_Message( stdout, "  top-hat radius            = %13.7e\n", SphCol_Radius     );
      Aux_Message( stdout, "  top-hat center x          = %13.7e\n", SphCol_Center[0]  );
      Aux_Message( stdout, "  ...            y          = %13.7e\n", SphCol_Center[1]  );
      Aux_Message( stdout, "  ...            z          = %13.7e\n", SphCol_Center[2]  );

      Aux_Message( stdout, "  MolecularMassFrac         = %13.7e\n", MolecularHydrogenMassFrac  );
      Aux_Message( stdout, "  GasTemp                   = %13.7e\n", GasTemp  );

      Aux_Message( stdout, "=============================================================================\n" );
   }


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "   Setting runtime parameters ... done\n" );

} // FUNCTION : SetParameter



//-------------------------------------------------------------------------------------------------------
// Function    :  SetGridIC
// Description :  Set the problem-specific initial condition on grids
//
// Note        :  1. This function may also be used to estimate the numerical errors when OPT__OUTPUT_USER is enabled
//                   --> In this case, it should provide the analytical solution at the given "Time"
//                2. This function will be invoked by multiple OpenMP threads when OPENMP is enabled
//                   --> Please ensure that everything here is thread-safe
//                3. Even when DUAL_ENERGY is adopted for HYDRO, one does NOT need to set the dual-energy variable here
//                   --> It will be calculated automatically
//
// Parameter   :  fluid    : Fluid field to be initialized
//                x/y/z    : Physical coordinates
//                Time     : Physical time
//                lv       : Target refinement level
//                AuxArray : Auxiliary array
//
// Return      :  fluid
//-------------------------------------------------------------------------------------------------------
void SetGridIC( real fluid[], const double x, const double y, const double z, const double Time,
                const int lv, double AuxArray[] )
{

   const double r = sqrt( SQR(x-SphCol_Center[0]) + SQR(y-SphCol_Center[1]) + SQR(z-SphCol_Center[2]) );
   
   const bool CheckMinPres_Yes = true;
   double GasPres;
   GasTemp = 1000.0;
   GasTemp *= Const_kB / UNIT_E;
   
    // ISOTHERMAL gas profile (Yoshida 2006)
   fluid[DENS]  = NFW_Rho * 0.05 * pow( 1.0 + pow( (r / NFW_Rs ) , 2.0) , - 1);
    
   if (r < gas_Rs){
    fluid[DENS] = NFW_Rho * 0.05;
   }

   //const double PI          = 3.14159265359;
   //fluid[DENS]  = sound_speed*sound_speed / 2.0 / PI / NEWTON_G / ( r*r + NFW_Rs*NFW_Rs ) ;  
   fluid[DENS] /= UNIT_D; 
   
   // Equilibrium profile assuming the gravity is primarily dominated by Dark Matter!
   //fluid[DENS]  = NFW_Rho * 0.1 * EXP( -r / NFW_Rs / 1.5 ) ;
   //fluid[DENS] /= UNIT_D; 
   
   GasPres = CPU_Temperature2Pressure( fluid[DENS], GasTemp, MOLECULAR_WEIGHT, 
           Const_mH/UNIT_M, CheckMinPres_Yes, MIN_PRES );
   fluid[ENGY] = GasPres / ( GAMMA - 1 );  

   //double sound_speed = SQRT( fluid[ENGY] / fluid[DENS] ) ;

   fluid[MOMX] = 0.0; //fluid[DENS] * sound_speed * ( (double)rand() / (double)RAND_MAX - 0.5);
   fluid[MOMY] = 0.0; //fluid[DENS] * sound_speed * ( (double)rand() / (double)RAND_MAX - 0.5);
   fluid[MOMZ] = 0.0; //fluid[DENS] * sound_speed * ( (double)rand() / (double)RAND_MAX - 0.5);
   
   fluid[ENGY] += 0.5*( SQR(fluid[MOMX]) + SQR(fluid[MOMY]) + SQR(fluid[MOMZ]) ) / fluid[DENS];



   fluid[Idx_HI]    = fluid[DENS]*0.76;
   fluid[Idx_HII]   = fluid[DENS]*1e-20;
   fluid[Idx_HeI]   = fluid[DENS]*0.24;
   fluid[Idx_HeII]  = fluid[DENS]*1e-20;
   fluid[Idx_HeIII] = fluid[DENS]*1e-20;
   fluid[Idx_HM]    = fluid[DENS]*1e-20;
   fluid[Idx_H2I]   = fluid[DENS]* MolecularHydrogenMassFrac;
   fluid[Idx_H2II]  = fluid[DENS]*1e-20;
   fluid[Idx_e]     = fluid[DENS]*1e-20;

 

} // FUNCTION : SetGridIC
#endif // #if ( MODEL == HYDRO )



void Init_NFWAcc()
{

// ExtAcc_AuxArray has the size of EXT_ACC_NAUX_MAX defined in CUPOT.h (default = 10)
// --> by default we set
//     ExtAcc_AuxArray[0] = x coordinate of the external acceleration center
//     ExtAcc_AuxArray[1] = y ...
//     ExtAcc_AuxArray[2] = z ..
//     ExtAcc_AuxArray[3] = gravitational_constant*point_source_mass
//     ExtAcc_AuxArray[4] = soften_length (<=0.0 --> disable)
// --> to change the this default behavior, please edit "GPU_Gravity/CUPOT_ExternalAcc.cu"

   const double PI          = 3.14159265359;
   const double FourPiGRho  = NEWTON_G*4.0*PI*NFW_Rho / UNIT_D;
   ExtAcc_AuxArray[0] = 0.5*amr->BoxSize[0];
   ExtAcc_AuxArray[1] = 0.5*amr->BoxSize[1];
   ExtAcc_AuxArray[2] = 0.5*amr->BoxSize[2];
   ExtAcc_AuxArray[3] = FourPiGRho;
   ExtAcc_AuxArray[4] = NFW_Rs;
   ExtAcc_AuxArray[5] = smoothing_length;
} // FUNCTION : Init_ExternalAcc



//-------------------------------------------------------------------------------------------------------
// Function    :  Init_TestProb_Hydro_Grackle_Non_Equil
// Description :  Test problem initializer
//
// Note        :  None
//
// Parameter   :  None
//
// Return      :  None
//-------------------------------------------------------------------------------------------------------
void Init_TestProb_Hydro_Grackle_Non_Equil()
{

   if ( MPI_Rank == 0 )    Aux_Message( stdout, "%s ...\n", __FUNCTION__ );


// validate the compilation flags and runtime parameters
   Validate();


#  if ( MODEL == HYDRO )
// set the problem-specific runtime parameters
   SetParameter();


// set the function pointers of various problem-specific routines
   Init_Function_User_Ptr   = SetGridIC;
   Output_User_Ptr          = NULL;
   Flag_User_Ptr            = NULL;
   Mis_GetTimeStep_User_Ptr = NULL;
   Aux_Record_User_Ptr      = NULL;
   BC_User_Ptr              = NULL;
   Flu_ResetByUser_Func_Ptr = NULL;
   End_User_Ptr             = NULL;
#  ifdef GRAVITY
   Init_ExternalAcc_Ptr     = Init_NFWAcc;
   Init_ExternalPot_Ptr     = NULL;
#  endif
#  endif // #if ( MODEL == HYDRO )


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "%s ... done\n", __FUNCTION__ );

} // FUNCTION : Init_TestProb_Hydro_Grackle_Non_Equil
