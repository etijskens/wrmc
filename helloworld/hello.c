#ifdef __linux__
#  define _GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <omp.h>
#include <sched.h>

int 
main(int argc, char *argv[]) 
{
    int mpi_rank, n_mpi_ranks, namelen;
    char host_name[MPI_MAX_PROCESSOR_NAME];
    int omp_thrd = 0, n_omp_thrds = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_mpi_ranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Get_processor_name(host_name, &namelen);
    int pid = getpid();

    //omp_set_num_threads(4);

  #pragma omp parallel default(shared) private(omp_thrd, n_omp_thrds)
    {
        int cpu = sched_getcpu();
        n_omp_thrds = omp_get_num_threads();
        omp_thrd = omp_get_thread_num();
        printf // ("Hello from OpenMP thread %d/%d from MPI mpi_rank %d/%d on %s\n",
          ( "Host=%s  Pid=%d  OMP_thread=%.3d/%.3d  MPI_rank=%.3d/%.3d  CPU=%d\n"
          , host_name, pid
          , mpi_rank, n_mpi_ranks
          , omp_thrd, n_omp_thrds
          , cpu
          );
    }

    MPI_Finalize();
}