#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>

void generate_random(double *input, size_t size) {
  for (size_t i = 0; i < size; i++) {
    input[i] = rand() / (double)(RAND_MAX);
  }
}

double serial_sum(double *x, size_t size) {
  double start_time, run_time;
  double sum_val = 0.0;

  start_time = omp_get_wtime();

  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  run_time = omp_get_wtime() - start_time;
  printf("%f\n", size, run_time);

  return sum_val;
}

double omp_sum(double *x, size_t size, int num_threads) {
  double start_time, run_time;
  double sum_val = 0.0;

  omp_set_num_threads(num_threads);

  start_time = omp_get_wtime();

  #pragma omp parallel for
  for (size_t i = 0; i < size; i++) {
    sum_val += x[i];
  }

  run_time = omp_get_wtime() - start_time;
  printf("%f\n", size, run_time);

  return sum_val;
}

double omp_critical_sum(double *x, size_t size, int num_threads) {
  double start_time, run_time;
  double sum_val = 0.0;

  omp_set_num_threads(num_threads);

  start_time = omp_get_wtime();

  #pragma omp parallel for
  for (size_t i = 0; i < size; i++) {
    #pragma omp critical
    sum_val += x[i];
  }

  run_time = omp_get_wtime() - start_time;
  printf("%f\n", size, run_time);

  return sum_val;
}

double omp_local_sum(double *x, size_t size, int num_threads) {
  double start_time, run_time;
  double sum_val = 0.0;

  omp_set_num_threads(num_threads);

  double local_sum[num_threads];

  start_time = omp_get_wtime();

  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    local_sum[id] = 0.0;
    for (int i = id; i < size; i += num_threads) {
      local_sum[id] += x[i];
    }
  }

  for (int i = 0; i < num_threads; i++) {
    sum_val += local_sum[i];
  }

  run_time = omp_get_wtime() - start_time;
  printf("%f\n", size, run_time);

  return sum_val;
}

double opt_local_sum(double *x, size_t size, int num_threads) {
  double start_time, run_time;
  double sum_val = 0.0;

  omp_set_num_threads(num_threads);

  double local_sum[num_threads][64]; // padded by 64 doubles

  start_time = omp_get_wtime();

  #pragma omp parallel
  {
    int id = omp_get_thread_num();
    local_sum[id][0] = 0.0;
    for (int i = id; i < size; i += num_threads) {
      local_sum[id][0] += x[i];
    }
  }

  for (int i = 0; i < num_threads; i++) {
    sum_val += local_sum[i][0];
  }

  run_time = omp_get_wtime() - start_time;
  printf("%f\n", size, run_time);

  return sum_val;
}

int main(int argc, char* argv[]) {
  int mode = 0;
  int num_threads = 1;
  int reps = 1;
  if (argc > 1){
    mode = atoi(argv[1]);
    num_threads = atoi(argv[2]);
    reps = atoi(argv[3]);
  }

  srand(time(NULL));

  unsigned long size = 10000000; //10e7
  double* arr = (double*)malloc(size * sizeof(double));

  for (int i = 0; i < reps; i++) {
    generate_random(arr, size);
    switch (mode)
    {
    case 0:
      serial_sum(arr, size);
      break;
    
    case 1:
      omp_sum(arr, size, num_threads);
      break;

    case 2:
      omp_critical_sum(arr, size, num_threads);
      break;

    case 3:
      omp_local_sum(arr, size, num_threads);
      break;

    case 4:
      opt_local_sum(arr, size, num_threads);
      break;
    
    default:
      break;
    }
  }

  free(arr);
  return 0;
}