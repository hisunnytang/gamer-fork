#include "GAMER.h"





//-------------------------------------------------------------------------------------------------------
// Function    :  dt_Prepare_Flu
// Description :  Fill up h_Flu_Array_T[] with fluid variables and h_Mag_Array_T[] with B field for
//                estimating the evolution time-step
//
// Note        :  1. Always prepare the latest FluSg and MagSg data
//                2. Prepare FLU_NIN_T fluid variables and NCOMP_MAG B field components
//                   --> Including/Excluding passive scalars for general/constant-gamma EoS
//                3. Use patches instead of patch groups as the basic unit
//                4. No ghost zones
//
// Parameter   :  lv            : Target refinement level
//                h_Flu_Array_T : Host array to store the prepared fluid data
//                h_Mag_Array_T : Host array to store the prepared B field data
//                NPG           : Number of patch groups prepared at a time
//                PID0_List     : List recording the target patch indices with LocalID==0
//-------------------------------------------------------------------------------------------------------
void dt_Prepare_CoolingTime( const int lv, real h_Cool_Array_T[][ CUBE(PS1) ], const int NPG, const int *PID0_List )
{

#  pragma omp parallel for schedule( static )
   for (int TID=0; TID<NPG; TID++)
   {
      const int PID0 = PID0_List[TID];
      
      real (*fluid)[PS1][PS1][PS1] = NULL; 

      for (int LocalID=0; LocalID<8; LocalID++)
      {
         const int PID = PID0 + LocalID;
         const int N   = 8*TID + LocalID;
         fluid = amr->patch[ amr->FluSg[lv] ][lv][PID]->fluid;

         // take only the cooling time
         for (int idx_p = 0; idx_p < CUBE(PS1); idx_p++)
         {
             h_Cool_Array_T[N][idx_p] = *(fluid[Idx_CoolingTime][0][0]+ idx_p );
             
             /*
            if (fabs( *(fluid[Idx_CoolingTime][0][0]+idx_p) ) < 1e-10)
                printf("dt[N=%d][idx_p=%d] at prepare: %0.5g; HI = %0.5g\n",N,idx_p, h_Cool_Array_T[N][idx_p], *(fluid[Idx_HI][0][0]+idx_p));
             */
        }

      }
   } // for (int TID=0; TID<NPG; TID++)

} // FUNCTION : dt_Prepare_Flu
