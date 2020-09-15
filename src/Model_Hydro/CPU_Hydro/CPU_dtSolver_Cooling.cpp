#include "CUFLU.h"

#if ( MODEL == HYDRO )



//-----------------------------------------------------------------------------------------
// Function    :  CPU/CUFLU_dtSolver_CoolingTime
// Description :  Estimate the evolution CoolingTime 
//
// Note        :  1. This function should be applied to both physical and comoving coordinates and always
//                   return the evolution time-step (dt) actually used in various solvers
//                   --> Physical coordinates : dt = physical time interval
//                       Comoving coordinates : dt = delta(scale_factor) / ( Hubble_parameter*scale_factor^3 )
//                   --> We convert dt back to the physical time interval, which equals "delta(scale_factor)"
//                       in the comoving coordinates, in Mis_GetTimeStep()
//                2. time-step is estimated by the stability criterion from the von Neumann stability analysis
//                   --> CFL condition
//                3. Arrays with a prefix "g_" are stored in the global memory of GPU
//
// Parameter   :  g_dt_Array  : Array to store the minimum dt in each target patch
//                g_Flu_Array : Array storing the prepared fluid data of each target patch
//                NPG         : Number of target patch groups (for CPU only)
//                dh          : Cell size
//                Safety      : dt safety factor
//                Gamma       : Ratio of specific heats
//                MinPres     : Minimum allowed pressure
//
// Return      :  g_dt_Array
//-----------------------------------------------------------------------------------------
//
#ifdef SUPPORT_DENGO
void CPU_dtSolver_CoolingTime( real g_dt_Array[], real g_Cool_Array[][CUBE(PS1)], const int NPG)
{

// loop over all patches
// --> CPU/GPU solver: use different (OpenMP threads) / (CUDA thread blocks)
//                     to work on different patches

#  pragma omp parallel for schedule( runtime )
   for (int p=0; p<8*NPG; p++)
   {
      real MinCoolTime =(real)HUGE_NUMBER; // some huge number

      CGPU_LOOP( t, CUBE(PS1) )
      {
         MinCoolTime = FMIN(fabs(g_Cool_Array[p][t]) ,MinCoolTime);
      
         //fprintf(stdout, "\tCoolTime[%d][%d]: %0.5g\n", p, t, g_Cool_Array[p][t]);
      } // CGPU_LOOP( t, CUBE(PS1) )

      // cooling safe factor
      real safe_coeff = 0.1;
      g_dt_Array[p] = safe_coeff*MinCoolTime;
      
      //fprintf(stdout, "MinCoolTime[%d]: %0.5g\n", p, MinCoolTime);

   } // for (int p=0; p<8*NPG; p++)

} // FUNCTION : CPU/CUFLU_dtSolver_CoolingTime
#endif // #if ( SUPPORT_DENGO )

#ifdef SUPPORT_GRACKLE
void CPU_dtSolver_CoolingTime( real g_dt_Array[], real g_Cool_Array[][CUBE(PS1)], const int NPG)
{

// loop over all patches
// --> CPU/GPU solver: use different (OpenMP threads) / (CUDA thread blocks)
//                     to work on different patches

#  pragma omp parallel for schedule( runtime )
   for (int p=0; p<8*NPG; p++)
   {
      real MinCoolTime =(real)HUGE_NUMBER; // some huge number

      CGPU_LOOP( t, CUBE(PS1) )
      {
         MinCoolTime = FMIN(fabs(g_Cool_Array[p][t]) ,MinCoolTime);
      
         //fprintf(stdout, "\tCoolTime[%d][%d]: %0.5g\n", p, t, g_Cool_Array[p][t]);
      } // CGPU_LOOP( t, CUBE(PS1) )

      // cooling safe factor
      real safe_coeff = 0.1;
      g_dt_Array[p] = safe_coeff*MinCoolTime;
      
      //fprintf(stdout, "MinCoolTime[%d]: %0.5g\n", p, MinCoolTime);

   } // for (int p=0; p<8*NPG; p++)

} // FUNCTION : CPU/CUFLU_dtSolver_CoolingTime
#endif // #if ( SUPPORT_GRACKLE)

#endif // #if ( MODEL == HYDRO )
