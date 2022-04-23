// Sample Sort with Threads
// by Oleksandr Litus
// cs3650 / Nat Tuck / Fall 2019

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <float.h>
#include <math.h>
#include <string.h>

#include "float_vec.h"
#include "barrier.h"
#include "utils.h"

// Struct for one job
typedef struct sort_job {
    int thr_id;
    char* fout_name;
    fvec* data_vec;
    fvec* pivots_vec;
    long* thr_sizes;
    barrier* barr;
} sort_job;


// Helper fuction for qsort
// Compares two floats for sortig from smallest to gretest
static
int
compare(const void * a, const void * b)
{
    float fa = *(float*) a;
    float fb = *(float*) b;
    return (fa > fb) - (fa < fb);
}


// Sort the given vector from smallest to greatest usig qsort
void
qsort_fvec(fvec* xs)
{
    qsort(xs->data, xs->size, sizeof(float), compare);
}


// Devide data into equally sized samples, return pivots
fvec*
sample(fvec* data_vec, int thr_num)
{
    // Randomly select 3 * (thr_num-1) items from the data
    long rand_size = 3 * (thr_num - 1);
    fvec* rand_vec = make_fvec(rand_size);
    for (long ii = 0; ii < rand_size; ++ii) {
        fvec_push(rand_vec, data_vec->data[rand() % data_vec->size]);
    }
   
    // Sort random items using qsort
    qsort_fvec(rand_vec);

    // Allocate an empty array of samples
    // Fisrt and last item are bounds
    fvec* pivots_vec = make_fvec(2);
     
    // Add min value of float at the start
    fvec_push(pivots_vec, 0);
    
    // Take the median of each group of three in the sorted array
    for (long ii = 1; ii < rand_size; ii += 3) {
        fvec_push(pivots_vec, rand_vec->data[ii]);
    }
    
    // Add INFINITY at the end
    fvec_push(pivots_vec, INFINITY);
    
    // free random items array
    free_fvec(rand_vec);

    return pivots_vec;
}


// Chooses and sorts its portion of data_vec
void*
sort_worker(void* job_point) 
{
    // Get the job information
    sort_job* job = (sort_job*) job_point;
    int id = job->thr_id;

    // Create tmp vector of floats to sort locally
    fvec* local_vec = make_fvec(2);

    float start_pivot = job->pivots_vec->data[id];
    float end_pivot = job->pivots_vec->data[id + 1];

    // Select the floats to be sorted by this worker
    for (long ii = 0; ii < job->data_vec->size; ++ii) {
        int more_start = job->data_vec->data[ii] >= start_pivot;
        int less_end = job->data_vec->data[ii] < end_pivot;
        
        if (more_start && less_end) {
            fvec_push(local_vec, job->data_vec->data[ii]);
        }
    }

    // Print stats about this worker
    printf("%d: start %.04f, count %ld\n", id, 
            job->pivots_vec->data[id], local_vec->size);

    // Write the number of items (nn) selected to sizes at slot p.
    job->thr_sizes[id] = local_vec->size;

    // Sort the local array with qsort
    qsort_fvec(local_vec);

    // Wait for barier
    barrier_wait(job->barr);

    // Open a separate file descriptor to the output file in each thread.
    int fout_d = open(job->fout_name, O_WRONLY, 0644);

    // Find the offset for the file
    long off = 0;
    for (long ii = 0; ii < id; ++ii) {
        off += job->thr_sizes[ii];
    }
    off *= sizeof(float);
    off += sizeof(long);

    // Use lseek to move to the correct location to write in the file.
    int rv = lseek(fout_d, off, SEEK_SET);
    check_rv(rv);

    // Write sorted loacal array into the file
    write(fout_d, local_vec->data, local_vec->size * sizeof(float));

    // Close the file descrpt
    rv = close(fout_d);
    check_rv(rv);

    // Free the local array
    free_fvec(local_vec);

    return NULL;
}


// Create given number of threads, each sorting its part of the data_vec
void
run_sort_workers(fvec* data_vec, sort_job* sort_jobs, int thr_num)
{
    pthread_t workers[thr_num];

    // Spawn thr_num of threads running sort_worker
    for (long tt = 0; tt < thr_num; ++tt) {
        int rv = pthread_create(&(workers[tt]), 0, sort_worker, 
                &sort_jobs[tt]);
        check_rv(rv);
    }
   
    // Wait for all threads
    for (long tt = 0; tt < thr_num; ++tt) {
        void* thr_ret;
        int rv = pthread_join(workers[tt], &thr_ret);
        check_rv(rv);
    }
}


// Sort the given array using given number of threads
void
sample_sort(fvec* data_vec, int thr_num, long* thr_sizes, 
        barrier* barr, char* fout_name)
{
    // Devide data into equaly sized samples for each thread
    fvec* pivots_vec = sample(data_vec, thr_num);
   
    // Create sort_job for the workers
    sort_job* sort_jobs = calloc(thr_num, sizeof(sort_job));
    for (int tt = 0; tt < thr_num; ++tt) {
        sort_jobs[tt].thr_id = tt;
        sort_jobs[tt].fout_name = fout_name;
        sort_jobs[tt].data_vec = data_vec;
        sort_jobs[tt].pivots_vec = pivots_vec;
        sort_jobs[tt].thr_sizes = thr_sizes;
        sort_jobs[tt].barr = barr;
    }

    // Create thr_num threads to sort each sample
    run_sort_workers(data_vec, sort_jobs, thr_num);
    
    // Free created pivots
    free_fvec(pivots_vec);

    // Free sort_job
    free(sort_jobs);
}


// Accept the number of processes tt, and the file containing the array,
// then call sample_sort on the extracted array
int
main(int argc, char* argv[])
{
    /* GET ARGUMENTS */
    // Print usage in case user input is of wrong size
    if (argc != 4) {
        printf("Usage:\n");
        printf("\t%s thr_num input.dat output.dat\n", argv[0]);
        return 1;
    }

    const int thr_num = atoi(argv[1]);
    assert(thr_num > 0);
    const char* fin_name = argv[2];
    char* fout_name = argv[3];


    /* GET THE INPUT */
    // Get the status of the input file
    int rv;
    struct stat fin_status;
    rv = stat(fin_name, &fin_status);
    check_rv(rv);

    // Get and check the size of the input file
    const long fin_size = fin_status.st_size;
    if (fin_size < 8) {
        printf("File too small.\n");
        return 1;
    }

    // Open the given input file
    int fin_d = open(fin_name, O_RDWR);
    check_rv(fin_d);

    // Get the number of floats
    long data_count;
    rv = read(fin_d, &data_count, sizeof(long));
    check_rv(rv);

    // Create fvec for the input
    fvec* data_vec = make_fvec(data_count);
    data_vec->size = data_count;
    rv = read(fin_d, data_vec->data, data_vec->size * sizeof(float));
    check_rv(rv);

    // Close input file
    rv = close(fin_d);
    assert(rv != -1);


    /* SETUP */
    // Generate seed for randomness
    seed_rng();

    // Create output file with the size of input file
    int fout_d = open(fout_name, O_WRONLY | O_CREAT, 0644);
    check_rv(fout_d);
    ftruncate(fout_d, fin_size);
    
    // Write the number of floats as the a long into output
    rv = write(fout_d, &data_count, sizeof(long));
    check_rv(rv);

    // Create array of sizes for each thread
    long sizes_bytes = thr_num * sizeof(long);
    long* thr_sizes = calloc(1, sizes_bytes);

    assert(thr_sizes != MAP_FAILED);
    
    // Create a barier with size thr_num
    barrier* barr = make_barrier(thr_num);

    
    /* SORT */
    sample_sort(data_vec, thr_num, thr_sizes, barr, fout_name);


    /* FREE & CLOSE */
    free_barrier(barr);

    free_fvec(data_vec);

    free(thr_sizes);

    rv = close(fout_d);
    check_rv(rv);

    return 0;
}

