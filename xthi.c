/*
 * xthi - alternative implementation for Calcua
 *  
 * Based on: https://git.ecdf.ed.ac.uk/dmckain/xthi
 */
#ifdef __linux__
#  define _GNU_SOURCE
#endif

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>

#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#ifndef NO_MPI
#  include <mpi.h>
#endif

#ifdef __linux__
#  include <sched.h>
#  include <numa.h>
#endif

#define HOSTNAME_MAX_LENGTH 64 // Max hostname length before truncation
#define RECORD_SIZE 128 // Max per-thread/process record size
#define RECORD_WORDS 8 // Number of words in each record

// #define DEBUGINFO 

// Brief usage instructions
const char *usage = 
    "Enhanced version of Cray's wee xthi \"where am I running?\" parallel code.\n"
    "\n"
    "Usage:\n"
    "     xthi [label] [cpu_chew_seconds]\n"
    "*or* xthi.nompi [label] [cpu_chew_seconds]\n"
    "\n"
    "\n"
    ;

void do_xthi(long chew_cpu_secs, char * label);
void output_records(const char *records, int count, const char **heads);
void update_widths(size_t *widths, const char *record);
void format_record(const char *record, const size_t *sizes, const char **heads);
void chew_cpu(long chew_cpu_secs);
int parse_args(int argc, char *argv[], char** label, long *chew_cpu_secs, int* verbose);

#ifdef __linux__
  char *cpuset_to_cstr(cpu_set_t *mask, char *str);
#endif

#ifdef __linux__
  static const int is_linux = 1;
#else
  static const int is_linux = 0;
#endif

#ifndef NO_MPI
  static const int is_mpi = 1;
#else
  static const int is_mpi = 0;
#endif


int main(int argc, char *argv[])
{
    int exit_code = EXIT_SUCCESS;

 // variables to hold the command line argumenrs
    long chew_cpu_secs = 0L;
    char * label;
    int verbose = 0;
  
  #ifndef NO_MPI
    MPI_Init(&argc, &argv);
  #endif

    if (parse_args(argc, argv, &label, &chew_cpu_secs, &verbose)) 
    {// Command line args are good => do xthi work
        do_xthi(chew_cpu_secs, label);
    }
    else
    { // Bad args => return failure
        exit_code = EXIT_FAILURE;
    }

  #ifndef NO_MPI
    MPI_Finalize();
  #endif
  
    return exit_code;
}

/* Main xthi work - the fun stuff lives here! */
void do_xthi(long chew_cpu_secs, char* label) 
{
    int mpi_rank = -1;
    int pid = getpid();
    int num_threads = -1;
    char *thread_data = NULL;

  #ifndef NO_MPI
    int mpi_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  #endif

 // Get short part of hostname
 // NB: gethostname() doesn't necessarily null terminate on truncation, so add explicit terminator
    char hostname_buf[HOSTNAME_MAX_LENGTH + 1];
    hostname_buf[HOSTNAME_MAX_LENGTH] = '\0';
    gethostname(hostname_buf, HOSTNAME_MAX_LENGTH);
    char *dot_ptr = strchr(hostname_buf, '.');
    if (dot_ptr != NULL)
       //dot_ptr = '\0';
    

 // Launch OpenMP threads and gather data
 // Each thread will store RECORD_SIZE characters, stored as a flat single array
    thread_data = malloc(sizeof(char) * omp_get_max_threads() * RECORD_SIZE);
    #pragma omp parallel default(none) shared(label, hostname_buf, pid, mpi_rank, thread_data, num_threads)
    {// Let each thread do a short CPU chew
        chew_cpu(0);

        #pragma omp master
        num_threads = omp_get_num_threads();

     // Gather thread-specific info
        int thread_num = omp_get_thread_num();

     // Gather additional Linux-specific info (if available)
      #ifdef __linux__
        int cpu = sched_getcpu();
        int numa_node = numa_node_of_cpu(cpu);
        char cpu_affinity_buf[7 * CPU_SETSIZE];

        cpu_set_t coremask;
        memset(cpu_affinity_buf, 0, sizeof(cpu_affinity_buf));
        sched_getaffinity(0, sizeof(coremask), &coremask);
        cpuset_to_cstr(&coremask, cpu_affinity_buf);
      #else
        int cpu = -1;
        int numa_node = -1;
        char *cpu_affinity_buf = "-";
      #endif

     // Record as a space-separated substring for easy MPI comms
        snprintf(thread_data + (thread_num * RECORD_SIZE), RECORD_SIZE,
                 "%s %s %d %d %d %d %d %.50s",
                 label, hostname_buf, pid, mpi_rank, thread_num, cpu, numa_node, cpu_affinity_buf);
    }

 // Work out which headings to include
    const char *heads[RECORD_WORDS] = 
    { "Label"
    , "Host"
    , "Pid"
    , is_mpi ? "MPI_rank" : NULL
    , num_threads > 1 ? "OMP_thread" : NULL
    , is_linux ? "CPU" : NULL
    , is_linux ? "NUMA_node" : NULL
    , is_linux ? "CPU_affinity" : NULL
    ,
    };

 // Aggregate and output result
  #ifdef NO_MPI
    output_records(thread_data, num_threads, heads);
  #else
    // MPI tasks aggregate data back to manager process (mpi_rank==0)
    if (mpi_rank != 0)
    {// This is a worker - send data to manager
        MPI_Ssend(thread_data, RECORD_SIZE * num_threads, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }
    else
    {// This is the manager - gather data from each worker
        int all_size = num_threads * mpi_size;
        char *all_data = malloc(sizeof(char) * RECORD_SIZE * all_size);

     // Copy manager thread's data into all data
        memcpy(all_data, thread_data, num_threads * RECORD_SIZE);

     // Then receive from each MPI task and each thread
        for (int j=1; j<mpi_size; ++j) {
            MPI_Recv(all_data + RECORD_SIZE * num_threads * j,
                     RECORD_SIZE * num_threads, MPI_CHAR,
                     j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        output_records(all_data, all_size, heads);

     // Tidy up manager-specific stuff
        free(all_data);
    }
  #endif

 // Maybe chew CPU for a bit
    if (chew_cpu_secs > 0) {
        #pragma omp parallel default(none) shared(chew_cpu_secs)
        {
            chew_cpu(chew_cpu_secs);
        }
    }

    // Tidy up
    free(thread_data);
}


/* Chews CPU for roughly (i.e. at least) the given number of seconds */
void chew_cpu(const long chew_cpu_secs) {
    time_t start, end;
    time(&start);
    volatile int count = 0;
    do {
        for (int i=0; i<100000; ++i)
            ;
        ++count;
        time(&end);
    } while (difftime(end, start) < chew_cpu_secs);
}


/* Outputs all of the accumulated data in a reasonably nice formatted fashion
 *
 * Params:
 * records: flattened string-delimited data, as gathered in main()
 * count: number of records within the data array
 * heads: headings to output, a NULL suppresses a particular record
 */
void output_records(const char *records, int count, const char **heads)
{
 // Calculate widths for formatting
    size_t widths[RECORD_WORDS] = { 0 };
    for (int k=0; k < count; ++k) {
        update_widths(widths, records + RECORD_SIZE * k);
    }

 // Output formatted messages
    for (int k=0; k < count; ++k) {
        format_record(records + RECORD_SIZE * k, widths, heads);
    }
    fflush(stdout);
}


/* Helper to check and update the current field widths for the given record */
void update_widths(size_t *widths, const char *record)
{
    int cur_word = 0; // Current word index
    size_t cur_len = 0; // Length of current word
    do {
        if (*record == '\0' || *record == ' ') {
            // End of word / record
            assert(cur_word < RECORD_WORDS);
            if (cur_len > widths[cur_word]) {
                widths[cur_word] = cur_len;
            }
            ++cur_word;
            cur_len = 0;
        }
        else {
            // Inside a word
            ++cur_len;
        }
    } while (*record++);
}


/* Formats the given record */
void format_record(const char *record, const size_t *sizes, const char **heads)
{
    int cur_word = 0; // Current word index
    const char *wordstart = record; // Start of current word
    do {
        if (*record == '\0' || *record == ' ') {
            // End of word/record
            assert((size_t) cur_word < sizeof(sizes));
            assert((size_t) cur_word < sizeof(heads));
            if (heads[cur_word]) {
                printf("%s=", heads[cur_word]);
                for (size_t j = 0; j < sizes[cur_word] - (record - wordstart); ++j) {
                    putchar(' ');
                }
                while (wordstart < record) {
                    putchar(*wordstart++);
                }
                if (*record == ' ') {
                    printf("  ");
                }
            }
            wordstart = record + 1;
            ++cur_word;
        }
    } while (*record++);
    putchar('\n');
}


typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

str2int_errno str2int(int *out, char *s, int base) 
{// Convert string s to int out. 
 // from https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
 //
 // @param[out] out The converted int. Cannot be NULL.
 //
 // @param[in] s Input string to be converted.
 //
 //     The format is the same as strtol,
 //     except that the following are inconvertible:
 //
 //     - empty string
 //     - leading whitespace
 //     - any trailing characters that are not part of the number
 //
 //     Cannot be NULL.
 //
 // @param[in] base Base to interpret string in. Same range as strtol (2 to 36).
 //
 // @return Indicates if the operation succeeded, or why it failed.
  #ifdef DEBUGINFO
    printf("DEBUGINFO[str2int] s=<%s>\n", s);
  #endif
    char *end;
    if (s[0] == '\0' || isspace(s[0]))
        return STR2INT_INCONVERTIBLE;
    errno = 0;
    long l = strtol(s, &end, base);
    /* Both checks are needed because INT_MAX == LONG_MAX is possible. */
    if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX)) {
      #ifdef DEBUGINFO
        printf("DEBUGINFO[str2int] s=<%s> -> STR2INT_OVERFLOW\n", s);
      #endif
        return STR2INT_OVERFLOW;
    }
    if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN)) {
      #ifdef DEBUGINFO
        printf("DEBUGINFO[str2int] s=<%s> -> STR2INT_UNDERFLOW\n", s);
      #endif
        return STR2INT_UNDERFLOW;
    }
    if (*end != '\0') {
      #ifdef DEBUGINFO
        printf("DEBUGINFO[str2int] s=<%s> -> STR2INT_INCONVERTIBLE\n", s);
      #endif
        return STR2INT_INCONVERTIBLE;
    }
    
    *out = l;
  #ifdef DEBUGINFO
    printf("DEBUGINFO[str2int] s=<%s> -> %li\n", s, l);
  #endif
    return STR2INT_SUCCESS;
}

int min (int a, int b) {return (a > b) ? b: a;}

int parse_args(int argc, char *argv[], char** label, long *chew_cpu_secs, int* verbose)
{// Parses command line arguments.
 //
 // Just chew_cpu_secs for now, but might become more exciting later!
 //
 // Returns 1 if all good, 0 otherwise.

    int const max_args = 4;
    if (argc > max_args) {
        fputs(usage, stderr);
        return 0;
    }
    
    long seconds = 10;
    int seconds_ok = 0;
    char *str = NULL; 
    int str_ok = 0;
    for (int i=1; i<min(argc, max_args); ++i) {
      #ifdef DEBUGINFO
        printf("DEBUGINFO[parse_args] iarg=%d\n", i);
      #endif
        int tmp;
        str2int_errno e = str2int(&tmp, argv[i], 10);
        if (e == STR2INT_SUCCESS && !seconds_ok) 
        {// accept argv[i] is a number -> chew_cpu_secs
            seconds = tmp;
            seconds_ok = 1;
          #ifdef DEBUGINFO
            printf("DEBUGINFO[parse_args] parsing seconds (%d)\n", tmp);
          #endif
        } else
        {// accept argv[i] is a string -> label or "-v"
          #ifdef DEBUGINFO
            printf("DEBUGINFO[parse_args] parsing string: argv[i=%d]=%s\n", i, argv[i]);
          #endif
            if (strcmp(argv[i], "-v") == 0) {
                *verbose = 1;
              #ifdef DEBUGINFO
                printf("DEBUGINFO[parse_args] parsing \"-v\" (verbose=1)\n");
              #endif
            } else if (!str_ok) {
                str = argv[i];
                str_ok = 1;
              #ifdef DEBUGINFO
                printf("DEBUGINFO[parse_args] parsing label (%s)\n", str);
              #endif
            }
        } 
    }
    
    *label = str;
    *chew_cpu_secs = seconds;
    if (*verbose) {
        printf("Running with options:\n");
        if (seconds_ok) {
            printf("  Duration set to: %li\n", *chew_cpu_secs);
        } else {
            printf("  Default duration: %li\n", *chew_cpu_secs);
        }
        if (str_ok) {
            printf("  Label set to: %s\n", *label);
        } else {
            printf("  Default label: %s\n", *label);
        }        
    }
    return 1;
}


/* Formats a CPU affinity mask in a nice way
 *
 * As with the original xthi code, this was taken from from:
 * util-linux-2.13-pre7/schedutils/taskset.c
 *
 * However this function is no longer included in
 * https://github.com/karelzak/util-linux/
 */
#ifdef __linux__
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
#endif
