#include "GAMER.h"

#ifdef SUPPORT_DENGO




//-------------------------------------------------------------------------------------------------------
// Function    :  Dengo_Init_FieldData
// Description :  Initialize the "Che_FieldData" dengo_field_data object of Dengo
//
// Note        :  1. Invoked by Dengo_Init()
//                   --> "Che_FieldData" is freed by End_MemFree()
//
// Parameter   :  None
//
// Return      :  None
//-------------------------------------------------------------------------------------------------------
void Dengo_Init_FieldData()
{

// nothing to do if Dengo is disabled
   if ( !DENGO_ACTIVATE )   return;


// allocate memory
   Che_FieldData = new dengo_field_data;


// initialization
// fields not evolving with time
//   Che_FieldData->nstrip = int;
    

// grid_dimension, grid_start, and grid_end are set by CPU_DengoSolver() since the number
// of patch groups advanced at a time is not a constant
   /*
   for (int d=0; d<NDim; d++)
   {
      Che_FieldData->grid_dimension[d]    = PS2;
      Che_FieldData->grid_start    [d]    = 0;
      Che_FieldData->grid_end      [d]    = PS2 - 1;
   }
   */

// fields set by Dengo_Prepare() during each time-step
   Che_FieldData->density                 = NULL;
   Che_FieldData->ge_density              = NULL;
//   Che_FieldData->grid_dx                 = NULL_REAL;
   Che_FieldData->de_density              = NULL;
   Che_FieldData->H_1_density             = NULL;
   Che_FieldData->H_2_density             = NULL;
   Che_FieldData->He_1_density            = NULL;
   Che_FieldData->He_2_density            = NULL;
   Che_FieldData->He_3_density            = NULL;
   Che_FieldData->H_m0_density            = NULL;
   Che_FieldData->H2_1_density            = NULL;
   Che_FieldData->H2_2_density            = NULL;
   
   Che_FieldData->Gamma                   = NULL;
   Che_FieldData->CoolingTime             = NULL;
   Che_FieldData->MolecularWeight         = NULL;
   Che_FieldData->temperature             = NULL;
  
   
   /*
   Che_FieldData->DI_density              = NULL;
   Che_FieldData->DII_density             = NULL;
   Che_FieldData->HDI_density             = NULL;
   Che_FieldData->metal_density           = NULL;
    */

// fields not supported yet
   /*
   Che_FieldData->x_velocity              = NULL;
   Che_FieldData->y_velocity              = NULL;
   Che_FieldData->z_velocity              = NULL;
   Che_FieldData->volumetric_heating_rate = NULL;
   Che_FieldData->specific_heating_rate   = NULL;
   Che_FieldData->RT_HI_ionization_rate   = NULL;
   Che_FieldData->RT_HeI_ionization_rate  = NULL;
   Che_FieldData->RT_HeII_ionization_rate = NULL;
   Che_FieldData->RT_H2_dissociation_rate = NULL;
   Che_FieldData->RT_heating_rate         = NULL;
    */
    
   Che_FieldData->dengo_data_file = DENGO_DATA_FILE; 

} // FUNCTION : Dengo_Init_FieldData



#endif // #ifdef SUPPORT_DENGO
