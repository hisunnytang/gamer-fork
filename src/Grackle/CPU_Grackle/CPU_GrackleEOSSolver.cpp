#include "GAMER.h"

#ifdef SUPPORT_GRACKLE

extern int CheIdx_CoolingTime;
extern int CheIdx_Gamma;
extern int CheIdx_MolecularWeight;
extern int CheIdx_Temperature;




//-----------------------------------------------------------------------------------------
// Function    :  CPU_GrackleEOSSolver
// Description :  Estimate Cooling Time, Adiabatic Index, Temperature from Grackle directly
//
// Note        :  1. Currently it is used even when GPU is enabled
//                2. Since these variables are not included in grackle_field_data,
//                   we can only access/ update it from the h_Che_Array

// Parameter   :  Che_FieldData : Array of Grackle "grackle_field_data" objects
//                Che_Units     : Grackle "code_units" object
//                NPatchGroup   : Number of patch groups to be evaluated
//                dt            : Time interval to advance solution
//-----------------------------------------------------------------------------------------
void CPU_GrackleEOSSolver( real h_Che_Array[], grackle_field_data *Che_FieldData, code_units Che_Units, const int NPatchGroup, const real dt )
{

   // additional hydro info from chemistry solver
   real *Ptr_CoolingTime0     = h_Che_Array + CheIdx_CoolingTime     * Size1v;
   real *Ptr_Gamma0           = h_Che_Array + CheIdx_Gamma           * Size1v;
   real *Ptr_MolecularWeight0 = h_Che_Array + CheIdx_MolecularWeight * Size1v; 
   real *Ptr_Temperature0     = h_Che_Array + CheIdx_Temperature     * Size1v;


#  pragma omp parallel
   {

// thread-private variables
   int  idx_pg, PID, PID0, offset;  // idx_pg: array indices within a patch group
   real (*fluid)[PS1][PS1][PS1]=NULL;;

   real *Ptr_CoolingTime     = NULL;
   real *Ptr_Gamma           = NULL;
   real *Ptr_MolecularWeight = NULL;
   real *Ptr_Temperature     = NULL;
   

#  pragma omp for schedule( static )
   for (int TID=0; TID<NPG; TID++)
   {
      PID0      = PID0_List[TID];
      idx_pg    = 0;
      offset    = TID*Size1pg;

      Ptr_CoolingTime     = Ptr_CoolingTime0     + offset;
      Ptr_Gamma           = Ptr_Gamma0           + offset;
      Ptr_MolecularWeight = Ptr_MolecularWeight0 + offset;
      Ptr_Temperature     = Ptr_Temperature0     + offset;

      for (int LocalID=0; LocalID<8; LocalID++)
      {
         PID   = PID0 + LocalID;
         fluid = amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid;

         for (int idx_p=0; idx_p<CUBE(PS1); idx_p++)
         {
            if ( GRACKLE_USE_REALISTIC_EOS ){
            Ptr_CoolingTime    [idx_pg] = *(fluid[Idx_CoolingTime    ][0][0] + idx_p);
            Ptr_Gamma          [idx_pg] = *(fluid[Idx_Gamma          ][0][0] + idx_p);
            Ptr_MolecularWeight[idx_pg] = *(fluid[Idx_MolecularWeight][0][0] + idx_p);
            Ptr_Temperature    [idx_pg] = *(fluid[Idx_Temperature    ][0][0] + idx_p);
            }
            idx_pg ++;
         } // for (int idx_p=0; idx_p<CUBE(PS1); idx_p++)

      } // for (int LocalID=0; LocalID<8; LocalID++)
   } // for (int TID=0; TID<NPG; TID++)

   } // end of OpenMP parallel region


    if (calculate_cooling_time(&Che_Units, &Che_FieldData, Ptr_CoolingTime ) == 0)
        Aux_Error( ERROR_INFO, "Grackle calculate_cooling_time() failed !! \n" );
    
    if (calculate_temperature(&Che_Units, &Che_FieldData, Ptr_Temperature  ) == 0)
        Aux_Error( ERROR_INFO, "Grackle calculate_temperature() failed !! \n" );

    if (calculate_gamma(&Che_Units, &Che_FieldData, Ptr_Temperature  ) == 0)
        Aux_Error( ERROR_INFO, "Grackle calculate_temperature() failed !! \n" );
    
    // There is no such functionality in Grackle
    // might do it here manually
    //if (calculate_molecular_weight(&Che_Units, &Che_FieldData, Ptr_MolecularWeight  ) == 0)
    //    Aux_Error( ERROR_INFO, "Grackle calculate_molecular_weight() failed !! \n" );


}


} // FUNCTION : CPU_GrackleEOSSolver



#endif // #ifdef SUPPORT_GRACKLE
