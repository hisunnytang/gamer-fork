#include "GAMER.h"

#ifdef SUPPORT_DENGO


// global variables for accessing h_Che_Array[]
// --> declared in Init_MemAllocate_Dengo.cpp
extern int Che_NField;
extern int CheIdx_Dens;
extern int CheIdx_sEint;
extern int CheIdx_Ek;
extern int CheIdx_e;
extern int CheIdx_HI;
extern int CheIdx_HII;
extern int CheIdx_HeI;
extern int CheIdx_HeII;
extern int CheIdx_HeIII;
extern int CheIdx_HM;
extern int CheIdx_H2I;
extern int CheIdx_H2II;
extern int CheIdx_CoolingTime;
extern int CheIdx_Gamma;
extern int CheIdx_MolecularWeight;
extern int CheIdx_Temperature;



//-------------------------------------------------------------------------------------------------------
// Function    :  Dengo_Prepare
// Description :  Fill up the input host array h_Che_Array[] for the Dengo solver
//
// Note        :  1. Prepare Che_NField variables
//                   --> Che_NField and the corresponding array indices in h_Che_Array[] (e.g., CheIdx_Dens)
//                       are declared and set by Init_MemAllocate_Dengo()
//                2. This function always prepares the latest FluSg data
//
// Parameter   :  lv          : Target refinement level
//                h_Che_Array : Host array to store the prepared data
//                NPG         : Number of patch groups prepared at a time
//                PID0_List   : List recording the patch indicies with LocalID==0 to be udpated
//-------------------------------------------------------------------------------------------------------
void Dengo_Prepare( const int lv, real h_Che_Array[], const int NPG, const int *PID0_List )
{

// check
#  ifdef GAMER_DEBUG
   if ( CheIdx_Dens == Idx_Undefined )
      Aux_Error( ERROR_INFO, "CheIdx_Dens is undefined !!\n" );
   if ( CheIdx_sEint == Idx_Undefined )
      Aux_Error( ERROR_INFO, "CheIdx_sEint is undefined !!\n" );
   if ( CheIdx_Ek == Idx_Undefined )
      Aux_Error( ERROR_INFO, "CheIdx_Ek is undefined !!\n" );
   /*
   if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE6 ) {
      if (  Idx_e == Idx_Undefined  ||  CheIdx_e == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_e is undefined for \"DENGO_PRI_CHE_NSPE6\" !!\n" );
      if (  Idx_HI == Idx_Undefined  ||  CheIdx_HI == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HI is undefined for \"DENGO_PRI_CHE_NSPE6\" !!\n" );
      if (  Idx_HII == Idx_Undefined  ||  CheIdx_HII == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HII is undefined for \"DENGO_PRI_CHE_NSPE6\" !!\n" );
      if (  Idx_HeI == Idx_Undefined  ||  CheIdx_HeI == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HeI is undefined for \"DENGO_PRI_CHE_NSPE6\" !!\n" );
      if (  Idx_HeII == Idx_Undefined  ||  CheIdx_HeII == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HeII is undefined for \"DENGO_PRI_CHE_NSPE6\" !!\n" );
      if (  Idx_HeIII == Idx_Undefined  ||  CheIdx_HeIII == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HeIII is undefined for \"DENGO_PRI_CHE_NSPE6\" !!\n" );
   }

   if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE9 ) {
      if (  Idx_HM == Idx_Undefined  ||  CheIdx_HM == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HM is undefined for \"DENGO_PRI_CHE_NSPE9\" !!\n" );
      if (  Idx_H2I == Idx_Undefined  ||  CheIdx_H2I == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_H2I is undefined for \"DENGO_PRI_CHE_NSPE9\" !!\n" );
      if (  Idx_H2II == Idx_Undefined  ||  CheIdx_H2II == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_H2II is undefined for \"DENGO_PRI_CHE_NSPE9\" !!\n" );
   }

   if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE12 ) {
      if (  Idx_DI == Idx_Undefined  ||  CheIdx_DI == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_DI is undefined for \"DENGO_PRI_CHE_NSPE12\" !!\n" );
      if (  Idx_DII == Idx_Undefined  ||  CheIdx_DII == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_DII is undefined for \"DENGO_PRI_CHE_NSPE12\" !!\n" );
      if (  Idx_HDI == Idx_Undefined  ||  CheIdx_HDI == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_HDI is undefined for \"DENGO_PRI_CHE_NSPE12\" !!\n" );
   }

   if ( DENGO_METAL ) {
      if (  Idx_Metal == Idx_Undefined  ||  CheIdx_Metal == Idx_Undefined  )
         Aux_Error( ERROR_INFO, "[Che]Idx_Metal is undefined for \"DENGO_METAL\" !!\n" );
   }
   */
#  endif // #ifdef GAMER_DEBUG


   const int  Size1pg         = CUBE(PS2);
   const int  Size1v          = NPG*Size1pg;
#  ifdef DUAL_ENERGY
   
   // HERE FIXED GAMMA FIELD!!!!!!!!!!!
   const real  Gamma_m1       = GAMMA - (real)1.0;
   const real _Gamma_m1       = (real)1.0 / Gamma_m1;
   const bool CheckMinPres_No = false;
#  endif

   real *Ptr_Dens0  = h_Che_Array + CheIdx_Dens *Size1v;
   real *Ptr_sEint0 = h_Che_Array + CheIdx_sEint*Size1v;
   real *Ptr_Ek0    = h_Che_Array + CheIdx_Ek   *Size1v;
   real *Ptr_e0     = h_Che_Array + CheIdx_e    *Size1v;
   real *Ptr_HI0    = h_Che_Array + CheIdx_HI   *Size1v;
   real *Ptr_HII0   = h_Che_Array + CheIdx_HII  *Size1v;
   real *Ptr_HeI0   = h_Che_Array + CheIdx_HeI  *Size1v;
   real *Ptr_HeII0  = h_Che_Array + CheIdx_HeII *Size1v;
   real *Ptr_HeIII0 = h_Che_Array + CheIdx_HeIII*Size1v;
   real *Ptr_HM0    = h_Che_Array + CheIdx_HM   *Size1v;
   real *Ptr_H2I0   = h_Che_Array + CheIdx_H2I  *Size1v;
   real *Ptr_H2II0  = h_Che_Array + CheIdx_H2II *Size1v;
   // additional hydro info from chemistry solver
   real *Ptr_CoolingTime0     = h_Che_Array + CheIdx_CoolingTime     * Size1v;
   real *Ptr_Gamma0           = h_Che_Array + CheIdx_Gamma           * Size1v;
   real *Ptr_MolecularWeight0 = h_Che_Array + CheIdx_MolecularWeight * Size1v; 
   real *Ptr_Temperature0     = h_Che_Array + CheIdx_Temperature     * Size1v; 
#  pragma omp parallel
   {

// thread-private variables
   int  idx_pg, PID, PID0, offset;  // idx_pg: array indices within a patch group
   real Dens, Px, Py, Pz, Etot, _Dens, Ek, sEint;
   real (*fluid)[PS1][PS1][PS1]=NULL;;

   real *Ptr_Dens=NULL, *Ptr_sEint=NULL, *Ptr_Ek=NULL, *Ptr_e=NULL, *Ptr_HI=NULL, *Ptr_HII=NULL;
   real *Ptr_HeI=NULL, *Ptr_HeII=NULL, *Ptr_HeIII=NULL, *Ptr_HM=NULL, *Ptr_H2I=NULL, *Ptr_H2II=NULL;
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

      Ptr_Dens  = Ptr_Dens0  + offset;
      Ptr_sEint = Ptr_sEint0 + offset;
      Ptr_Ek    = Ptr_Ek0    + offset;
      Ptr_e     = Ptr_e0     + offset;
      Ptr_HI    = Ptr_HI0    + offset;
      Ptr_HII   = Ptr_HII0   + offset;
      Ptr_HeI   = Ptr_HeI0   + offset;
      Ptr_HeII  = Ptr_HeII0  + offset;
      Ptr_HeIII = Ptr_HeIII0 + offset;
      Ptr_HM    = Ptr_HM0    + offset;
      Ptr_H2I   = Ptr_H2I0   + offset;
      Ptr_H2II  = Ptr_H2II0  + offset;

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
            Dens  = *( fluid[DENS][0][0] + idx_p );
            Px    = *( fluid[MOMX][0][0] + idx_p );
            Py    = *( fluid[MOMY][0][0] + idx_p );
            Pz    = *( fluid[MOMZ][0][0] + idx_p );
            Etot  = *( fluid[ENGY][0][0] + idx_p );
            _Dens = (real)1.0 / Dens;
            Ek    = (real)0.5*( SQR(Px) + SQR(Py) + SQR(Pz) )*_Dens;

//          use the dual-energy variable to calculate the internal energy if applicable
#           ifdef DUAL_ENERGY

#           if   ( DUAL_ENERGY == DE_ENPY )
            sEint = CPU_DensEntropy2Pres( Dens, *(fluid[ENPY][0][0]+idx_p), Gamma_m1, CheckMinPres_No, NULL_REAL )
                    *_Dens*_Gamma_m1;
#           elif ( DUAL_ENERGY == DE_EINT )
#           error : DE_EINT is NOT supported yet !!
#           endif

#           else
            sEint = ( Etot - Ek )*_Dens;
#           endif // #ifdef DUAL_ENERGY ... else

//          mandatory fields
            Ptr_Dens [idx_pg] = Dens;
            Ptr_sEint[idx_pg] = sEint;
            Ptr_Ek   [idx_pg] = Ek;

//          6-species network
            //if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE6 ) {
            Ptr_e    [idx_pg] = *( fluid[Idx_e    ][0][0] + idx_p );
            Ptr_HI   [idx_pg] = *( fluid[Idx_HI   ][0][0] + idx_p );
            Ptr_HII  [idx_pg] = *( fluid[Idx_HII  ][0][0] + idx_p );
            Ptr_HeI  [idx_pg] = *( fluid[Idx_HeI  ][0][0] + idx_p );
            Ptr_HeII [idx_pg] = *( fluid[Idx_HeII ][0][0] + idx_p );
            Ptr_HeIII[idx_pg] = *( fluid[Idx_HeIII][0][0] + idx_p );
            //}

//          9-species network
            //if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE9 ) {
            Ptr_HM   [idx_pg] = *( fluid[Idx_HM   ][0][0] + idx_p );
            Ptr_H2I  [idx_pg] = *( fluid[Idx_H2I  ][0][0] + idx_p );
            Ptr_H2II [idx_pg] = *( fluid[Idx_H2II ][0][0] + idx_p );
            //}
            
            Ptr_CoolingTime    [idx_pg] = *(fluid[Idx_CoolingTime    ][0][0] + idx_p);
            Ptr_Gamma          [idx_pg] = *(fluid[Idx_Gamma          ][0][0] + idx_p);
            Ptr_MolecularWeight[idx_pg] = *(fluid[Idx_MolecularWeight][0][0] + idx_p);
            Ptr_Temperature    [idx_pg] = *(fluid[Idx_Temperature    ][0][0] + idx_p);

//          12-species network
            /*
            if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE12 ) {
            Ptr_DI   [idx_pg] = *( fluid[Idx_DI   ][0][0] + idx_p );
            Ptr_DII  [idx_pg] = *( fluid[Idx_DII  ][0][0] + idx_p );
            Ptr_HDI  [idx_pg] = *( fluid[Idx_HDI  ][0][0] + idx_p );
            }
            */

//          metallicity for metal cooling
            /*
            if ( DENGO_METAL )
            Ptr_Metal[idx_pg] = *( fluid[Idx_Metal][0][0] + idx_p );
            */
            idx_pg ++;
         } // for (int idx_p=0; idx_p<CUBE(PS1); idx_p++)

      } // for (int LocalID=0; LocalID<8; LocalID++)
   } // for (int TID=0; TID<NPG; TID++)

   } // end of OpenMP parallel region


// set cell size and link pointers for different fields
//   Che_FieldData->grid_dx         = amr->dh[lv];

   Che_FieldData->density         = Ptr_Dens0;
   Che_FieldData->ge_density      = Ptr_sEint0;

   //if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE6 ) {
   Che_FieldData->de_density      = Ptr_e0;
   Che_FieldData->H_1_density     = Ptr_HI0;
   Che_FieldData->H_2_density     = Ptr_HII0;
   Che_FieldData->He_1_density    = Ptr_HeI0;
   Che_FieldData->He_2_density    = Ptr_HeII0;
   Che_FieldData->He_3_density    = Ptr_HeIII0;
   //}

   //if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE9 ) {
   Che_FieldData->H_m0_density    = Ptr_HM0;
   Che_FieldData->H2_1_density    = Ptr_H2I0;
   Che_FieldData->H2_2_density    = Ptr_H2II0;
   //}
   
   // additional info from the chemistry solver
   Che_FieldData->CoolingTime        = Ptr_CoolingTime0;
   Che_FieldData->MolecularWeight    = Ptr_MolecularWeight0;
   Che_FieldData->Gamma              = Ptr_Gamma0;
   Che_FieldData->temperature        = Ptr_Temperature0;



/* if ( DENGO_PRIMORDIAL >= DENGO_PRI_CHE_NSPE12 ) {
   Che_FieldData->DI_density      = Ptr_DI0;
   Che_FieldData->DII_density     = Ptr_DII0;
   Che_FieldData->HDI_density     = Ptr_HDI0;
   }

   if ( DENGO_METAL )
   Che_FieldData->metal_density   = Ptr_Metal0;
*/
} // FUNCTION : Dengo_Prepare



#endif // #ifdef SUPPORT_DENGO
