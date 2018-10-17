#include "GAMER.h"

#ifdef SUPPORT_DENGO




//-----------------------------------------------------------------------------------------
// Function    :  CPU_DengoSolver
// Description :  Update the internal energy by the various cooling and heating mechanisms
//                in the original Dengo library
//
// Note        :  1. Currently it is used even when GPU is enabled
//
// Parameter   :  Che_FieldData : Array of Dengo "dengo_field_data" objects
//                Che_Units     : Dengo "code_units" object
//                NPatchGroup   : Number of patch groups to be evaluated
//                dt            : Time interval to advance solution
//-----------------------------------------------------------------------------------------
void CPU_DengoSolver( dengo_field_data *Che_FieldData, code_units Che_Units, const int NPatchGroup, const real dt )
{

// set grid_dimension, grid_start, and grid_end
    
   Che_FieldData->ncells = (unsigned long int) CUBE(PS2) * NPatchGroup;

// invoke Dengo
// --> note that we use the OpenMP implementation in Dengo directly, which applies the parallelization to the first two
//     dimensiones of the input grid
// --> this approach is found to be much more efficient than parallelizing different patches or patch groups here
//
// cvklu = solver_name
   if ( cvklu_solve_chemistry_dt( &Che_Units, Che_FieldData, dt ) == 1  )
      Aux_Error( ERROR_INFO, "Dengo solve_chemistry() failed !!\n" );

} // FUNCTION : CPU_DengoSolver



#endif // #ifdef SUPPORT_DENGO
