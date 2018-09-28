#include "GAMER.h"
#include <typeinfo>

#ifdef SUPPORT_DENGO




//-------------------------------------------------------------------------------------------------------
// Function    :  Dengo_Init
// Description :  Initialize the chemistry and radiative cooling library Dengo
//
// Note        :  1. Must be called AFTER Init_Load_Parameter(), Init_Unit(), and Init_OpenMP()
//                2. COMOVING is not supported yet
//
// Parameter   :  None
//
// Return      :  None
//-------------------------------------------------------------------------------------------------------
void Dengo_Init()
{

// nothing to do if Dengo is disabled
   if ( !DENGO_ACTIVATE )   return;


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "%s ...\n", __FUNCTION__ );


// check
// floating-point type (don't know how to validate it yet...)
   /*
   if ( typeid(real) != typeid(gr_float) )
      Aux_Error( ERROR_INFO, "inconsistent floating-point type (GAMER: %d, Dengo: %d) !!\n",
                 sizeof(real), sizeof(gr_float) );
                 */

// comoving frame is not supported yet
#  ifdef COMOVING
   Aux_Error( ERROR_INFO, "SUPPORT_DENGO does not work with COMOVING yet !!\n" );
#  endif
    
   /*
   if (  ( DENGO_PRIMORDIAL == DENGO_PRI_CHE_CLOUDY || DENGO_METAL || DENGO_UV )  &&
         !Aux_CheckFileExist(DENGO_CLOUDY_TABLE)  )
      Aux_Error( ERROR_INFO, "Dengo data file \"%s\" does not exist !!\n", DENGO_CLOUDY_TABLE );
    */

// enable output (only for the root rank)
//   dengo_verbose = ( MPI_Rank == 0 ) ? DENGO_VERBOSE : 0;


// units in cgs
// --> Che_Units is declared as a global variable since all Dengo solvers require that as well
#  ifdef COMOVING
   Che_Units.comoving_coordinates = 1;
   Che_Units.density_units        = NULL_REAL;  // not sure how to set the units in the comoving coordinates yet...
   Che_Units.length_units         = NULL_REAL;  // --> see http://dengo.readthedocs.io/en/latest/Integration.html
   Che_Units.time_units           = NULL_REAL;
   Che_Units.velocity_units       = NULL_REAL;
   Che_Units.a_units              = 1.0;
   Che_Units.a_value              = Time[0];

#  else
   //Che_Units.comoving_coordinates = 0;
   Che_Units.density_units        = UNIT_D;
   Che_Units.length_units         = UNIT_L;
   Che_Units.time_units           = UNIT_T;
   Che_Units.velocity_units       = UNIT_V;
   //Che_Units.a_units              = 1.0;
   //Che_Units.a_value              = 1.0;
#  endif


// set the default chemsitry
// --> note that "Che_Data" will be attached to the Dengo internal pointer "dengo_data"
//     after calling set_default_chemistry_parameters()
// --> we must NOT deallocate "Che_Data" during the simulation
// --> currently it's deallocated by Dengo_End()
//  chemistry_data *Che_Data = new chemistry_data;

//  if ( set_default_chemistry_parameters(Che_Data) == 0 )
//    Aux_Error( ERROR_INFO, "set_default_chemistry_parameters() failed !!\n" );


// set chemistry by accessing "dengo_data"
//   dengo_data->use_dengo                  = DENGO_ACTIVATE;
//   dengo_data->with_radiative_cooling     = DENGO_COOLING;
//   dengo_data->primordial_chemistry       = DENGO_PRIMORDIAL;
//   dengo_data->metal_cooling              = DENGO_METAL;
//   dengo_data->UVbackground               = DENGO_UV;
//   dengo_data->cmb_temperature_floor      = DENGO_CMB_FLOOR;
//   dengo_data->photoelectric_heating      = DENGO_PE_HEATING;
//   dengo_data->photoelectric_heating_rate = DENGO_PE_HEATING_RATE;
//   dengo_data->dengo_data_file            = DENGO_CLOUDY_TABLE;

#  ifdef OPENMP
// currently we adopt the OpenMP implementation in Dengo directly, which applies the parallelization to
// **different cells inside a patch group** instead of **different patch groups**
// --> this approach is found to be more efficient
// --> therefore, we should enable OpenMP for Dengo and disable OpenMP in CPU_DengoSolver()
//     to avoid the nested parallelization
   //dengo_data->omp_nthreads               = OMP_NTHREAD;
#  endif
/*
#  if ( MODEL == HYDRO )
   dengo_data->Gamma                      = GAMMA;

#  elif ( MODEL == MHD )
#  warning : WAIT MHD !!!
#  endif
*/

// initialize the chemistry object
//   if ( initialize_chemistry_data(&Che_Units) == 0 )
//     Aux_Error( ERROR_INFO, "initialize_chemistry_data() failed !!\n" );


// initialize the "dengo_field_data" object of Dengo
   Dengo_Init_FieldData();


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "%s ... done\n", __FUNCTION__ );

} // FUNCTION : Dengo_Init



#endif // #ifdef SUPPORT_DENGO
