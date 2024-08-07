// put this file in the <chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC/> directory

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef BAREMETAL
#include <sys/mman.h>
#endif
#include "include/gemmini_testutils.h"

int main()
{
#ifndef BAREMETAL
  if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
  {
    perror("mlockall failed");
    exit(1);
  }
#endif

  printf("Flush Gemmini TLB of stale virtual addresses\n");
  gemmini_flush(0);

  gemmini_fence();

  uint64_t start, end; // used to measure cycles

  printf("Initialize our input and output matrices in main memory\n");
  elem_t A[DIM][DIM], B[DIM][DIM];
  elem_t C[DIM][DIM];

  // initalize A matrix
  for (size_t i = 0; i < DIM; i++)
    for (size_t j = 0; j < DIM; j++)
      A[i][j] = i + j;

  printf("A matrix:\n");
  printMatrix(A);

  // initalize B matrix
  for (size_t i = 0; i < DIM; i++)
    for (size_t j = 0; j < DIM; j++)
      B[i][j] = i * j;

  printf("B matrix:\n");
  printMatrix(B);

  printf("Calculate the scratchpad addresses of all our matrices\n");
  printf("  Note: The scratchpad is \"row-addressed\", where each address contains one matrix row\n");
  size_t A_sp_addr = 0;       // stores A
  size_t B_sp_addr = DIM;     // stores B
  size_t C_sp_addr = 2 * DIM; // stores C

  // configure the load and store dimensions
  start = read_cycles();
  gemmini_config_ld(DIM * sizeof(elem_t));
  gemmini_config_st(DIM * sizeof(elem_t));
  end = read_cycles();
  printf("Config ld/st cycles: %llu\n", end - start);

  gemmini_fence();

  printf("Move A matrix from main memory into Gemmini's scratchpad\n");
  start = read_cycles();
  gemmini_mvin(A, A_sp_addr);
  end = read_cycles();
  printf("Move A cycles: %llu\n", end - start);

  gemmini_fence();

  printf("Move B matrix from main memory into Gemmini's scratchpad\n");
  start = read_cycles();
  gemmini_mvin(B, B_sp_addr);
  end = read_cycles();
  printf("Move B cycles: %llu\n", end - start);

  gemmini_fence();

  start = read_cycles();
  gemmini_config_ex(OUTPUT_STATIONARY, 0, 0);
  end = read_cycles();
  printf("Config ex cycles: %llu\n", end - start);

  gemmini_fence();

  printf("Preload zeros into the systolic array (C matrix) \n");
  start = read_cycles();
  gemmini_preload_zeros(C_sp_addr); // preload zeros into C
  end = read_cycles();
  printf("Preload zeros cycles: %llu\n", end - start);

  gemmini_fence();

  printf("Multiply A matrix with B matrix with a bias of 0\n");
  start = read_cycles();
  gemmini_compute_preloaded(A_sp_addr, B_sp_addr);
  end = read_cycles();
  printf("Compute matmul cycles: %llu\n", end - start);

  gemmini_fence();

  printf("Move C matrix from Gemmini's scratchpad into main memory\n");
  start = read_cycles();
  gemmini_mvout(C, C_sp_addr);
  end = read_cycles();
  printf("Move C cycles: %llu\n", end - start);

  gemmini_fence();

  printf("C matrix:\n");
  printMatrix(C);

  printf("Total cycles: ");
  printf("%llu\n", read_cycles());

  exit(0);
}