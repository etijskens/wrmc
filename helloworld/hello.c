#define _GNU_SOURCE
#ifdef __linux__
#  define _GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <omp.h>
#include <sched.h>
#include <numa.h>

char *cpuset_to_cstr(cpu_set_t *mask, char *str)
{
    char *ptr = str;
    int i, j, entry_made = 0;
    for (i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, mask)) {
            int run = 0;
            entry_made = 1;
            for (j = i + 1; j < CPU_SETSIZE; j++) {
                if (CPU_ISSET(j, mask)) run++;
                else break;
            }
            if (!run)
                sprintf(ptr, "%d,", i);
            else if (run == 1) {
                sprintf(ptr, "%d,%d,", i, i + 1);
                i++;
            } else {
                sprintf(ptr, "%d-%d,", i, i + run);
                i += run;
            }
            while (*ptr != 0) ptr++;
        }
    }
    ptr -= entry_made;
    //ptr = 0;
    return str;
}

int 
main(int argc, char *argv[]) 
{
    int mpi_rank, n_mpi_ranks, namelen;
    char host_name[MPI_MAX_PROCESSOR_NAME];
    int omp_thrd = 0, n_omp_thrds = 1;

    char label = '0';
    if( argc > 1 ) {
        label = argv[1][0];
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_mpi_ranks);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Get_processor_name(host_name, &namelen);
    int pid = getpid();

    int cpu_mpi = sched_getcpu();
    
    printf // ("Hello from OpenMP thread %d/%d from MPI mpi_rank %d/%d on %s\n",
        ( "srun=%c Host=%s  Pid=%d  MPI_rank=%.3d/%.3d                      CPU=%d\n"
        , label
        , host_name, pid
        , mpi_rank, n_mpi_ranks
        , cpu_mpi
        );


  #pragma omp parallel default(shared) private(omp_thrd, n_omp_thrds)
    {
        n_omp_thrds = omp_get_num_threads();
        omp_thrd = omp_get_thread_num();

        int cpu = sched_getcpu();
        int numa_node = numa_node_of_cpu(cpu);
        char cpu_affinity_buf[7 * CPU_SETSIZE];

        cpu_set_t coremask;
        memset(cpu_affinity_buf, 0, sizeof(cpu_affinity_buf));
        sched_getaffinity(0, sizeof(coremask), &coremask);
        cpuset_to_cstr(&coremask, cpu_affinity_buf);
        printf // ("Hello from OpenMP thread %d/%d from MPI mpi_rank %d/%d on %s\n",
          ( "srun=%c Host=%s  Pid=%d  MPI_rank=%.3d/%.3d  OMP_thread=%.3d/%.3d  CPU=%.3d  NUMA_node=%d  CPU_affinity=%s\n"
          , label
          , host_name, pid
          , mpi_rank, n_mpi_ranks
          , omp_thrd, n_omp_thrds
          , cpu
          , numa_node
          , cpu_affinity_buf
          );
        sleep(10);
    }

    MPI_Finalize();
}