# Alternative implementation of xthi example code

## Background

Modified after https://git.ecdf.ed.ac.uk/dmckain/xthi

## Usage

This code gets built into 2 slightly different binaries.
Choose which one you want to use to suit your needs as follows:

* **xthi**: can be run as MPI and/or OpenMP.
* **xthi.nompi**: has no MPI support. This can be useful for running
  OpenMP-only on Slurm.

The code should be launched in the same way as any MPI and/or OpenMP code.

Command line arguments 

* a positive number [optional, defaults to 10]: number of seconds to do some random work
* a string [optional]: Label, useful to distinguish multiple calls in the job script
* "-v" [optional]: verbose mode, prints the command line argumnents as parsed.

Here are some examples:

```sh
# MPI: launching 4 tasks, each chewing CPU for 30 seconds
mpirun -np 4 xthi 30

# OpenMP: launching 2 threads, each chewing CPU for 60 seconds
OMP_NUM_THREADS=2 xthi.nompi 60

# Hybrid MPI & OpenMP: 4 MPI tasks, each launching 2 OpenMP threads
OMP_NUM_THREADS=2 mpirun -np 4 xthi
```

Here's a concrete example of running this as hybrid MPI & OpenMP code on one of the
School's "walk-in" compute nodes. (So not using Slurm here.) The output is included
and explained below. Here we're launching 4 MPI tasks, each running 2 OpenMP threads,
thus 8 parallel tasks in total:

```sh
$ OMP_NUM_THREADS=2 mpirun -np 4 xthi
Host=phcompute002  MPI Rank=0  OMP Thread=0  CPU= 2  NUMA Node=0  CPU Affinity=  0-7
Host=phcompute002  MPI Rank=0  OMP Thread=1  CPU= 4  NUMA Node=0  CPU Affinity=  0-7
Host=phcompute002  MPI Rank=1  OMP Thread=0  CPU= 8  NUMA Node=1  CPU Affinity= 8-15
Host=phcompute002  MPI Rank=1  OMP Thread=1  CPU=15  NUMA Node=1  CPU Affinity= 8-15
Host=phcompute002  MPI Rank=2  OMP Thread=0  CPU=16  NUMA Node=2  CPU Affinity=16-23
Host=phcompute002  MPI Rank=2  OMP Thread=1  CPU=22  NUMA Node=2  CPU Affinity=16-23
Host=phcompute002  MPI Rank=3  OMP Thread=0  CPU=24  NUMA Node=3  CPU Affinity=24-31
Host=phcompute002  MPI Rank=3  OMP Thread=1  CPU=30  NUMA Node=3  CPU Affinity=24-31
```

The output provides information about each of the 8 parallel tasks, telling you
the following:

* **Host**: The host that the code is running on. This can be useful when running
  the codes on our Slurm cluster, or when running MPI code across multiple hosts.
* **MPI Rank**: The current MPI rank (if the code is run as MPI).
* **OMP Thread**: The OpenMP thread number (if the code launches OpenMP threads).
* **CPU**: The CPU that this task/thread is currently running on.
* **NUMA Node**: The memory node associated with the current CPU.
* **CPU Affinity**: Tells you which CPU(s) the current task/thread may run on.
  This is set by Slurm (and is controllable by you) when running jobs via Slurm.

The output is sorted in lexicographic order: thus by Host first, then MPI Rank,
then OpenMP thread.

## Further usage info for the School of Physics & Astronomy Ubuntu Linux Platform

This code is deployed on all Ubuntu hosts.

However note that the MPI version of the code has been compiled with OpenMPI,
so will only work with OpenMPI. If you want to use it with Intel MPI or MPICH
then you'll need to compile your own copy. See below for details on compiling.

## Compiling with Make

I've provided a simple `Makefile` for building this on the SoPA Ubuntu Linux platform
using OpenMPI. (This is also used to generate the corresponding Debian package.)

This `Makefile` also works on the SoPA Scientific Linux 7 platform, though you'll
need to load an MPI implementation first, for example: `module load openmpi`.

This code has been written for Linux: some of the CPU affinity & NUMA functionality
uses specific Linux & glibc functions. I've tried to delimit these with C pre-processor
conditionals, so the code should build OK on macOS, albeit with reduced reporting capability.

## Compiling with CMake

I've also provided a `CMakeLists.txt` for using this with CMake.

Here are some examples of how you can use this on the SoPA Ubuntu Linux platform:

```sh
mkdir -p build
cd build

# To use OpenMPI
cmake ..
make

# To use MPICH
cmake -DMPI_C_COMPILER=mpicc.mpich ..
make

# To use Intel MPI
module load intel/mpi/2021.2.0
cmake
make
```

## Original Sources

The original code was obtained from:
https://raw.githubusercontent.com/Wildish-LBL/SLURM-demo/master/xthi.c

It's mentioned in Cray's documentation at:
https://pubs.cray.com/bundle/CLE_User_Application_Placement_Guide_CLE52UP04_S-2496/page/Run_an_OpenMP_Application.html

The codes all incorporate a helper function to nicely format
CPU affinity, taken from:
https://github.com/karelzak/util-linux

## License

None of the original xthi codes came with an obviously visible license
so I'm not sure of their original license.

The util-linux code is GPL 2.0.
