 
#include "headers.h"
 
/* debugging header */
#include <cegdb.h>

/* malloc debug stuff */
char malloc_logpath_memory[256];
 
/*--------------------------------------------------------------------------
 * Test driver for structured matrix interface (structured storage)
 *--------------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------
 * Example 3:
 *
 *    Read matrix and vector from disk.
 *----------------------------------------------------------------------*/
int
main( int   argc,
      char *argv[] )
{
   MPI_Comm           *comm;
   int                 matrix_num_ghost[6] = { 1, 1, 0, 0, 0, 0};
   int                 vector_num_ghost[6] = { 1, 1, 1, 1, 1, 1};
                     
   zzz_StructMatrix   *matrix;
   zzz_StructVector   *vector;
   zzz_StructVector   *tmp_vector;

   int                 num_procs, myid;

   /*-----------------------------------------------------------
    * Initialize some stuff
    *-----------------------------------------------------------*/
 
   /* Initialize MPI */
   MPI_Init(&argc, &argv);

   MPI_Comm_size(MPI_COMM_WORLD, &num_procs );
   MPI_Comm_rank(MPI_COMM_WORLD, &myid );
   MPI_Comm_dup(MPI_COMM_WORLD, comm);

   cegdb(&argc, &argv, myid);

   /* malloc debug stuff */
   malloc_logpath = malloc_logpath_memory;
   sprintf(malloc_logpath, "malloc.log.%04d", myid);

   /*-----------------------------------------------------------
    * Read in the matrix
    *-----------------------------------------------------------*/

   matrix = zzz_ReadStructMatrix(comm, "zin_matrix", matrix_num_ghost);
 
   zzz_PrintStructMatrix(comm, "zout_matrix", matrix, 0);

   /*-----------------------------------------------------------
    * Read in the vector
    *-----------------------------------------------------------*/

   vector = zzz_ReadStructVector(comm, "zin_vector", vector_num_ghost);
 
   zzz_PrintStructVector(comm, "zout_vector", vector, 0);

   /*-----------------------------------------------------------
    * Do a matvec
    *-----------------------------------------------------------*/

   tmp_vector =
      zzz_NewStructVector(comm, zzz_StructVectorGrid(vector));
   zzz_InitializeStructVector(tmp_vector);
   zzz_AssembleStructVector(tmp_vector);

   zzz_StructMatvec(1.0, matrix, vector, 0.0, tmp_vector);

   zzz_PrintStructVector(comm, "zout_matvec", tmp_vector, 0);

   /*-----------------------------------------------------------
    * Copy the vector into tmp_vector
    *-----------------------------------------------------------*/

   zzz_StructCopy(vector, tmp_vector);

   zzz_PrintStructVector(comm, "zout_copy", tmp_vector, 0);

   /*-----------------------------------------------------------
    * Scale tmp_vector
    *-----------------------------------------------------------*/

   zzz_StructScale(2.0, tmp_vector);

   zzz_PrintStructVector(comm, "zout_scale", tmp_vector, 0);

   /*-----------------------------------------------------------
    * Do an Axpy (2*vector - vector) = vector
    *-----------------------------------------------------------*/

   zzz_StructAxpy(-1.0, vector, tmp_vector);

   zzz_PrintStructVector(comm, "zout_axpy", tmp_vector, 0);

   /*-----------------------------------------------------------
    * Finalize things
    *-----------------------------------------------------------*/

   zzz_FreeStructGrid(zzz_StructMatrixGrid(matrix));
   zzz_FreeStructMatrix(matrix);
   zzz_FreeStructGrid(zzz_StructVectorGrid(vector));
   zzz_FreeStructVector(vector);
   zzz_FreeStructVector(tmp_vector);

   /* malloc debug stuff */
   malloc_verify(0);
   malloc_shutdown();

   /* Finalize MPI */
   MPI_Finalize();

   return 0;
}

