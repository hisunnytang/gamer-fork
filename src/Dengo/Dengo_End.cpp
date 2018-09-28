#include "GAMER.h"

#ifdef SUPPORT_DENGO




//-------------------------------------------------------------------------------------------------------
// Function    :  Dengo_End
// Description :  Free the resources used by Dengo
//
// Note        :  1. Invoked by End_GAMER()
//
// Parameter   :  None
//
// Return      :  None
//-------------------------------------------------------------------------------------------------------
void Dengo_End()
{

// nothing to do if Dengo is disabled
   if ( !DENGO_ACTIVATE )   return;


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "%s ...\n", __FUNCTION__ );


//   delete dengo_data;
//   dengo_data = NULL;


   if ( MPI_Rank == 0 )    Aux_Message( stdout, "%s ... done\n", __FUNCTION__ );

} // FUNCTION : Dengo_End



#endif // #ifdef SUPPORT_DENGO
