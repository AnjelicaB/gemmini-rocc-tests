// See LICENSE for license details.

#ifndef SRC_MAIN_C_GEMMINI_H
#define SRC_MAIN_C_GEMMINI_H

#undef abs

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>

#include "include/gemmini_params.h"

#define GEMMINI_ASSERTIONS

// Accelerator interface
#include "rocc-software/src/xcustom.h"

#define k_CONFIG 0
#define k_MVIN2 1
#define k_MVIN 2
#define k_MVOUT 3
#define k_COMPUTE_PRELOADED 4
#define k_COMPUTE_ACCUMULATE 5
#define k_PRELOAD 6
#define k_FLUSH 7

#define k_LOOP_WS 8
#define k_LOOP_WS_CONFIG_BOUNDS 9
#define k_LOOP_WS_CONFIG_ADDRS_AB 10
#define k_LOOP_WS_CONFIG_ADDRS_DC 11
#define k_LOOP_WS_CONFIG_STRIDES_AB 12
#define k_LOOP_WS_CONFIG_STRIDES_DC 13

#define k_MVIN3 14

#define k_LOOP_CONV_WS 15
#define k_LOOP_CONV_WS_CONFIG_1 16
#define k_LOOP_CONV_WS_CONFIG_2 17
#define k_LOOP_CONV_WS_CONFIG_3 18
#define k_LOOP_CONV_WS_CONFIG_4 19
#define k_LOOP_CONV_WS_CONFIG_5 20
#define k_LOOP_CONV_WS_CONFIG_6 21

#define k_LOOP_LD_CONFIG_BOUNDS 22
#define k_LOOP_LD_CONFIG_ADDRS 23
#define k_LOOP_CONV_LD_CONFIG_BOUNDS 24
#define k_LOOP_CONV_LD_CONFIG_ADDRS 25

#define CONFIG_EX 0
#define CONFIG_LD 1
#define CONFIG_ST 2
#define CONFIG_IM2COL 3

#define GARBAGE_ADDR ((uint32_t)(-1))
#define OUTPUT_STATIONARY 0
#define WEIGHT_STATIONARY 1

#define NO_ACTIVATION 0
#define RELU 1
#define RELU6 2

#ifdef ELEM_T_IS_FLOAT
elem_t elem_t_bits_to_elem_t(elem_t_bits x) {
    union {
        elem_t_bits b;
        elem_t f;
    } un;

    un.b = x;
    return un.f;
}

elem_t_bits elem_t_to_elem_t_bits(elem_t x) {
    union {
        elem_t_bits b;
        elem_t f;
    } un;

    un.f = x;
    return un.b;
}

acc_t acc_t_bits_to_acc_t(acc_t_bits x) {
    union {
        acc_t_bits b;
        acc_t f;
    } un;

    un.b = x;
    return un.f;
}

acc_t_bits acc_t_to_acc_t_bits(acc_t x) {
    union {
        acc_t_bits b;
        acc_t f;
    } un;

    un.f = x;
    return un.b;
}

bool elem_t_isnan(elem_t x) {
    elem_t_bits bits = elem_t_to_elem_t_bits(x);
    uint64_t exp = (bits >> (ELEM_T_SIG_BITS-1)) & (((uint64_t)1 << ELEM_T_EXP_BITS) - 1);
    uint64_t sig = bits & (((uint64_t)1 << ELEM_T_SIG_BITS) - 1);
    bool is_nan_or_inf = exp == (((uint64_t)1 << ELEM_T_EXP_BITS) - 1);
    bool is_not_inf = sig != 0;
    return is_nan_or_inf && is_not_inf;
}

bool acc_t_isnan(acc_t x) {
    acc_t_bits bits = acc_t_to_acc_t_bits(x);
    uint64_t exp = (bits >> (ACC_T_SIG_BITS-1)) & (((uint64_t)1 << ACC_T_EXP_BITS) - 1);
    uint64_t sig = bits & (((uint64_t)1 << ACC_T_SIG_BITS) - 1);
    bool is_nan_or_inf = exp == (((uint64_t)1 << ACC_T_EXP_BITS) - 1);
    bool is_not_inf = sig != 0;
    return is_nan_or_inf && is_not_inf;
}
#endif

#ifdef HAS_MVIN_SCALE
scale_t scale_t_bits_to_scale_t(scale_t_bits x) {
    union {
        scale_t_bits b;
        scale_t f;
    } un;

    un.b = x;
    return un.f;
}

scale_t_bits scale_t_to_scale_t_bits(scale_t x) {
    union {
        scale_t_bits b;
        scale_t f;
    } un;

    un.f = x;
    return un.b;
}
#endif

#ifdef HAS_MVIN_ACC_SCALE
scale_acc_t scale_acc_t_bits_to_scale_acc_t(scale_acc_t_bits x) {
    union {
        scale_acc_t_bits b;
        scale_acc_t f;
    } un;

    un.b = x;
    return un.f;
}

scale_acc_t_bits scale_acc_t_to_scale_acc_t_bits(scale_acc_t x) {
    union {
        scale_acc_t_bits b;
        scale_acc_t f;
    } un;

    un.f = x;
    return un.b;
}
#endif

acc_scale_t acc_scale_t_bits_to_acc_scale_t(acc_scale_t_bits x) {
    union {
        acc_scale_t_bits b;
        acc_scale_t f;
    } un;

    un.b = x;
    return un.f;
}

acc_scale_t_bits acc_scale_t_to_acc_scale_t_bits(acc_scale_t x) {
    union {
        acc_scale_t_bits b;
        acc_scale_t f;
    } un;

    un.f = x;
    return un.b;
}

#define ROCC_INSTRUCTION_RS1_RS2(x, rs1, rs2, funct) \
  ROCC_INSTRUCTION_0_R_R(x, rs1, rs2, funct)

// mvin and mvout
#define gemmini_extended_mvin(dram_addr, spad_addr, cols, rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, dram_addr, ((uint64_t)(rows) << (ADDR_LEN + 16)) | ((uint64_t)(cols) << ADDR_LEN) | (spad_addr), k_MVIN)

#define gemmini_extended_mvin2(dram_addr, spad_addr, cols, rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, dram_addr, ((uint64_t)(rows) << (ADDR_LEN + 16)) | ((uint64_t)(cols) << ADDR_LEN) | (spad_addr), k_MVIN2)

#define gemmini_extended_mvin3(dram_addr, spad_addr, cols, rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, dram_addr, ((uint64_t)(rows) << (ADDR_LEN + 16)) | ((uint64_t)(cols) << ADDR_LEN) | (spad_addr), k_MVIN3)

#define gemmini_block_mvin(dram_addr, spad_addr, len) \
  gemmini_extended_mvin(dram_addr, spad_addr, (len) * DIM, DIM)

#define gemmini_mvin(dram_addr, spad_addr) \
  gemmini_extended_mvin(dram_addr, spad_addr, DIM, DIM)

#define gemmini_extended_mvout(dram_addr, spad_addr, cols, rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, dram_addr, ((uint64_t)(rows) << (ADDR_LEN + 16)) | ((uint64_t)(cols) << ADDR_LEN) | (uint64_t)(spad_addr), k_MVOUT)

#define gemmini_mvout(dram_addr, spad_addr) \
  gemmini_extended_mvout(dram_addr, spad_addr, DIM, DIM)

// compute
#define gemmini_extended_compute_preloaded(A, BD, A_cols, A_rows, BD_cols, BD_rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(A_rows) << (ADDR_LEN + 16)) | ((uint64_t)(A_cols) << ADDR_LEN) | (uint64_t)(A), ((uint64_t)(BD_rows) << (ADDR_LEN + 16)) | ((uint64_t)(BD_cols) << ADDR_LEN) | (uint64_t)(BD), k_COMPUTE_PRELOADED)

#define gemmini_extended_compute_accumulated(A, BD, A_cols, A_rows, BD_cols, BD_rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(A_rows) << (ADDR_LEN + 16)) | ((uint64_t)(A_cols) << ADDR_LEN) | (uint64_t)(A), ((uint64_t)(BD_rows) << (ADDR_LEN + 16)) | ((uint64_t)(BD_cols) << ADDR_LEN) | (uint64_t)(BD), k_COMPUTE_ACCUMULATE)

#define gemmini_compute_preloaded(A, BD) \
  gemmini_extended_compute_preloaded(A, BD, DIM, DIM, DIM, DIM)

#define gemmini_compute_accumulated(A, BD) \
  gemmini_extended_compute_accumulated(A, BD, DIM, DIM, DIM, DIM)

// preload
#define gemmini_extended_preload(BD, C, BD_cols, BD_rows, C_cols, C_rows) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(BD_rows) << (ADDR_LEN + 16)) | ((uint64_t)(BD_cols) << ADDR_LEN) | (uint64_t)(BD), ((uint64_t)(C_rows) << (ADDR_LEN + 16)) | ((uint64_t)(C_cols) << ADDR_LEN) | (uint64_t)(C), k_PRELOAD)

#define gemmini_preload(BD, C) \
  gemmini_extended_preload(BD, C, DIM, DIM, DIM, DIM)

#define gemmini_preload_zeros(C) \
  gemmini_preload(GARBAGE_ADDR, C)

// config
#define gemmini_extended2_config_ex(dataflow, act, sys_shift, acc_scale, relu6_shift, A_stride, A_transpose, B_transpose, ocol, row_turn, kdim, stride, channel, row_left, kdim2, weight_double_bank, weight_triple_bank) \
  { \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)acc_scale_t_to_acc_scale_t_bits((acc_scale_t)acc_scale) << 32) | ((uint64_t)(A_stride) << 16) | (B_transpose << 9) | (A_transpose << 8) | ((act) << 3) | ((dataflow) << 2) | CONFIG_EX, ((uint64_t)(relu6_shift) << 32) | (sys_shift), k_CONFIG); \
    \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(weight_triple_bank) << 59) | ((uint64_t)(weight_double_bank) << 58) | ((uint64_t)(row_left) << 54) | ((uint64_t)(row_turn) << 42) | CONFIG_IM2COL, ((uint64_t)ocol << 56) | ((uint64_t)kdim2 << 48) | ((uint64_t)kdim << 44) | ((uint64_t)channel << 23) | ((uint64_t)stride << 20), k_CONFIG) \
  }

#define gemmini_extended_config_ex(dataflow, act, sys_shift, acc_scale, relu6_shift, A_stride, A_transpose, B_transpose) \
  gemmini_extended2_config_ex(dataflow, act, sys_shift, acc_scale, relu6_shift, A_stride, A_transpose, B_transpose, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define gemmini_config_ex(dataflow, act, sys_shift, acc_scale, relu6_shift) \
    gemmini_extended_config_ex(dataflow, act, sys_shift, acc_scale, relu6_shift, 1, 0, 0)

#define gemmini_extended4_config_ld(stride, scale, shrunk, block_mvin_stride, id) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(scale_t_to_scale_t_bits(scale)) << 32) | ((uint64_t)(block_mvin_stride) << 16) | ((id) << 3) | ((shrunk) << 2) | CONFIG_LD, stride, k_CONFIG)

#define gemmini_extended3_config_ld(stride, scale, shrunk, id) \
  gemmini_extended4_config_ld(stride, scale, shrunk, DIM, id)

#define gemmini_extended2_config_ld(stride, scale, shrunk) \
  gemmini_extended3_config_ld(stride, scale, shrunk, 0)

#define gemmini_extended_config_ld(stride, scale) \
  gemmini_extended2_config_ld(stride, scale, false)

#define gemmini_config_ld(stride) \
  gemmini_extended_config_ld(stride, MVIN_SCALE_IDENTITY)

#define gemmini_extended_config_st(stride, pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, upad, lpad) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(ocols) << 56) | ((uint64_t)(orows) << 48) | ((uint64_t)(pocols) << 40) | ((uint64_t)(porows) << 32) | ((uint64_t)(pool_out_dim) << 24) | ((uint64_t)(lpad) << 10) | ((uint64_t)(upad) << 8) | ((uint64_t)(pool_size) << 6) | ((uint64_t)(pool_stride) << 4) | CONFIG_ST, stride, k_CONFIG)

#define gemmini_config_st(stride) \
    gemmini_extended_config_st(stride, 0, 0, 0, 0, 0, 0, 0, 0, 0)

// flush
#define gemmini_flush(skip) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, skip, 0, k_FLUSH)

// fence
#define gemmini_fence() asm volatile("fence")

// weight-stationary matmul loop
#define gemmini_loop_ws(I, J, K, pad_I, pad_J, pad_K, A, B, D, C, A_stride, B_stride, D_stride, C_stride, A_transpose, B_transpose, full_C, low_D, ex_accumulate) \
  { \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(pad_K) << 32) | ((uint64_t)(pad_J) << 16) | (uint64_t)(pad_I), ((uint64_t)(K) << 32) | ((uint64_t)(J) << 16) | (uint64_t)(I), k_LOOP_WS_CONFIG_BOUNDS) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, A, B, k_LOOP_WS_CONFIG_ADDRS_AB) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, D, C, k_LOOP_WS_CONFIG_ADDRS_DC) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, A_stride, B_stride, k_LOOP_WS_CONFIG_STRIDES_AB) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, D_stride, C_stride, k_LOOP_WS_CONFIG_STRIDES_DC) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((low_D) << 2) | ((full_C) << 1) | (ex_accumulate), ((B_transpose) << 1) | (A_transpose), k_LOOP_WS) \
  }

#define gemmini_loop_ld(AB, profile, max_row, max_col, pad_row, pad_col, dram_addr, row_stride, latency, alert_cycle, unlock_cycle, pause_turn) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(AB) << 63) | ((uint64_t)(profile) << 62) | ((uint64_t)(pad_col) << 16) | (uint64_t)(pad_row), ((uint64_t)(pause_turn) << 58) | ((uint64_t)(unlock_cycle) << 54) | ((uint64_t)(alert_cycle) << 48) | ((uint64_t)(latency) << 32) | ((uint64_t)(max_col) << 16) | (uint64_t)(max_row), k_LOOP_LD_CONFIG_BOUNDS) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, dram_addr, row_stride, k_LOOP_LD_CONFIG_ADDRS) 

// weight-stationary matmul loop
#define gemmini_loop_conv_ws(batch_size, in_dim, in_channels, out_channels, out_dim, pool_out_dim, stride, padding, kernel_dim, pool_size, pool_stride, pool_padding, batches, porows, pocols, pochs, krows, kcols, kchs, lpad, rpad, upad, dpad, plpad, prpad, pupad, pdpad, orows, ocols, weights, pool_out, output, bias, input, no_bias, out_both, no_pool, depthwise, in_stride, weight_stride, out_stride) \
  { \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(out_channels) << 48) | ((uint64_t)(in_channels) << 32) | ((uint64_t)(in_dim) << 16) | (uint64_t)(batch_size), \
      ((uint64_t)(padding) << 48) | ((uint64_t)(stride) << 32) | ((uint64_t)(pool_out_dim) << 16) | (uint64_t)(out_dim), k_LOOP_CONV_WS_CONFIG_1) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(kernel_dim) << 48) | ((uint64_t)(pool_size) << 32) | ((uint64_t)(pool_stride) << 16) | (uint64_t)(pool_padding), \
      ((uint64_t)(batches) << 48) | ((uint64_t)(porows) << 32) | ((uint64_t)(pocols) << 16) | (uint64_t)(pochs), k_LOOP_CONV_WS_CONFIG_2) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(krows) << 48) | ((uint64_t)(kcols) << 32) | ((uint64_t)(kchs) << 16) | (uint64_t)(lpad), \
      ((uint64_t)(rpad) << 48) | ((uint64_t)(upad) << 32) | ((uint64_t)(dpad) << 16) | (uint64_t)(plpad), k_LOOP_CONV_WS_CONFIG_3) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(orows) << 48) | ((uint64_t)(prpad) << 32) | ((uint64_t)(pupad) << 16) | (uint64_t)(pdpad), \
      ((uint64_t)(in_stride) << 48) | ((uint64_t)(weight_stride) << 32) | ((uint64_t)(out_stride) << 16) | ((uint64_t)(ocols)), k_LOOP_CONV_WS_CONFIG_4) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, weights, \
      output, k_LOOP_CONV_WS_CONFIG_5) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, bias, \
      input, k_LOOP_CONV_WS_CONFIG_6) \
    ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, pool_output, \
      ((uint64_t)(depthwise) << 63) | ((uint64_t)(out_both) << 62) | ((uint64_t)(no_bias) << 61) | ((uint64_t) no_pool), k_LOOP_CONV_WS) \
  }

#define gemmini_loop_ld_conv(dram_addr, dram_stride, in_channels, out_channels, kernel_dim, pochs, krows, kcols, kchs, latency, alert_cycle, unlock_cycle, pause_turn, depthwise)\
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, ((uint64_t)(krows) << 48) | ((uint64_t)(kcols) << 32) | ((uint64_t)(kchs) << 16) | ((uint64_t)(pochs)), ((uint64_t)(pause_turn) << 58) | ((uint64_t)(unlock_cycle) << 54) | ((uint64_t)(alert_cycle) << 48) | ((uint64_t)(latency) << 32) | ((uint64_t)(kernel_dim)), k_LOOP_CONV_LD_CONFIG_BOUNDS) \
  ROCC_INSTRUCTION_RS1_RS2(XCUSTOM_ACC, dram_addr, ((uint64_t)(depthwise) << 63) | ((uint64_t)(dram_stride) << 32) | ((uint64_t)(out_channels) << 16) | ((uint64_t)(in_channels)), k_LOOP_CONV_LD_CONFIG_ADDRS)
 


// Tiling functions
static void sp_tiled_matmul_os(const elem_t * A, const elem_t * B, const void * D, void * C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        size_t I, size_t J, size_t K, size_t pad_I, size_t pad_J, size_t pad_K,
        size_t A_row_stride, size_t B_row_stride, size_t D_row_stride, size_t C_row_stride,
        bool a_transpose, bool b_transpose,
        bool full_C, bool low_D,
        bool no_bias, bool repeating_bias,
		  bool skip_A, bool skip_B, bool detect_conflict) {

  const uint32_t A_sp_addr_start = 0;
  const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - K * J * DIM;
  const uint32_t D_sp_addr_start = 1 << (ADDR_LEN-1);
  const uint32_t C_sp_addr_start = (3 << (ADDR_LEN-2)) | (full_C << (ADDR_LEN-3));

  const int A_blocks = K <= MAX_BLOCK_LEN ? K : MAX_BLOCK_LEN;
  const int B_blocks = J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN;
  const int D_blocks = J <= MAX_BLOCK_LEN_ACC ? J : MAX_BLOCK_LEN_ACC;

  // Move-in D
  if (D != NULL && !no_bias) {
    const size_t D_stride = repeating_bias ? 0 : D_row_stride * sizeof(acc_t);
    gemmini_extended_config_ld(D_stride, D_scale_factor);

    for (size_t i = 0; i < I; i++) {
      for (size_t j = 0; j < J; j += D_blocks) {
        const size_t bias_row = repeating_bias ? 0 : i;
        const acc_t * const D_dram_addr = (acc_t *)D + (bias_row * D_row_stride + j)*DIM;

        const uint32_t D_sp_addr_acc = D_sp_addr_start + (i*J + j)*DIM;

        const size_t blocks = j + D_blocks <= J ? D_blocks : J-j;

        const size_t cols = blocks * DIM - (j + blocks >= J ? pad_J : 0);
        const size_t rows = DIM - (i == I-1 ? pad_I : 0);

        gemmini_extended_mvin(D_dram_addr, D_sp_addr_acc, cols, rows);
      }
    }
  }

  // Move-in B
  gemmini_extended_config_ld(B_row_stride * sizeof(elem_t), B_scale_factor);
  for (size_t j = 0; j < J; j += B_blocks) {
    for (size_t k = 0; k < K; k++) {
      const elem_t * const B_dram_addr = B + (k*B_row_stride + j)*DIM;
      const uint32_t B_sp_addr = B_sp_addr_start + (k*J + j)*DIM;
      const size_t blocks = j + B_blocks <= J ? B_blocks : J-j;
      const size_t cols = blocks * DIM - (j + blocks >= J ? pad_J : 0);
      const size_t rows = DIM - (k == K-1 ? pad_K : 0);
      gemmini_extended_mvin(B_dram_addr, B_sp_addr, cols, rows);
    }
  }

  // Move-in A
  gemmini_extended_config_ld(A_row_stride * sizeof(elem_t), A_scale_factor);
  for (size_t i = 0; i < I; i++) {
    for (size_t k = 0; k < K; k += A_blocks) {
      const elem_t * const A_dram_addr = A + (i*A_row_stride + k)*DIM;
      const uint32_t A_sp_addr = A_sp_addr_start + (i*K + k)*DIM;
      const size_t blocks = k + A_blocks <= K ? A_blocks : K-k;
      const size_t cols = blocks * DIM - (k + blocks >= K ? pad_K : 0);
      const size_t rows = DIM - (i == I-1 ? pad_I : 0);
      gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
    }
  }

  for (size_t i = 0; i < I; i++) {
    for (size_t j = 0; j < J; j++) {
      const uint32_t C_sp_addr = C_sp_addr_start + (i*J + j)*DIM;

      for (size_t k = 0; k < K; k++) {

        const uint32_t A_sp_addr = A_sp_addr_start + (i*K + k)*DIM;
        const uint32_t B_sp_addr = B_sp_addr_start + (k*J + j)*DIM;

        uint32_t out_sp_addr = k == K-1 ? C_sp_addr : GARBAGE_ADDR;

        // If we're not using a bias, then we want to overwrite what's in the
        // accumulator, rather than writing over it
        int no_bias_new_matrix = no_bias && D != NULL && k == K-1;
        if (no_bias_new_matrix) {
          out_sp_addr &= ~(1 << (ADDR_LEN-2));
        }

        const size_t A_cols = DIM - (k == K - 1 ? pad_K : 0);
        const size_t A_rows = DIM - (i == I - 1 ? pad_I : 0);
        const size_t B_cols = DIM - (j == J - 1 ? pad_J : 0);
        const size_t B_rows = DIM - (k == K - 1 ? pad_K : 0);
        const size_t C_cols = DIM - (j == J - 1 ? pad_J : 0);
        const size_t C_rows = DIM - (i == I - 1 ? pad_I : 0);

        gemmini_extended_preload(GARBAGE_ADDR, out_sp_addr, DIM, DIM, C_cols, C_rows);

        if (k == 0) { // First iteration
          gemmini_extended_compute_preloaded(A_sp_addr, B_sp_addr, A_cols, A_rows, B_cols, B_rows);
        } else { // All other iterations
          gemmini_extended_compute_accumulated(A_sp_addr, B_sp_addr, A_cols, A_rows, B_cols, B_rows);
        }
      }
    }
  }

  // Move-out C
  if (C != NULL) {
    const size_t sizeof_C = full_C ? sizeof(acc_t) : sizeof(elem_t);

    for (size_t i = 0; i < I; i++) {
      for (size_t j = 0; j < J; j++) {
        void * const C_dram_addr = (int8_t*)C + (i*C_row_stride + j)*DIM*sizeof_C;
        const uint32_t C_sp_addr = C_sp_addr_start + (i*J + j)*DIM;

        const size_t C_cols = DIM - (j == J - 1 ? pad_J : 0);
        const size_t C_rows = DIM - (i == I - 1 ? pad_I : 0);

        gemmini_extended_mvout(C_dram_addr, C_sp_addr, C_cols, C_rows);
      }
    }
  }
}

static void sp_tiled_matmul_ws(const elem_t * A, const elem_t * B,
        const void * D, void * C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        size_t I, size_t J, size_t K, size_t pad_I, size_t pad_J, size_t pad_K,
        size_t A_row_stride, size_t B_row_stride, size_t D_row_stride, size_t C_row_stride,
        bool a_transpose, bool b_transpose,
        bool full_C, bool low_D,
        bool no_bias, bool repeating_bias,
		  bool skip_A, bool skip_B, bool detect_conflict) {

  /*
  const uint32_t A_sp_addr_start = 0;
  const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - K * J * DIM;
  const uint32_t D_sp_addr_start = 1 << (ADDR_LEN-1);
  const uint32_t C_sp_addr_start = 3 << (ADDR_LEN-2) | (full_C << (ADDR_LEN-3));

  const int A_blocks = a_transpose ? (I <= MAX_BLOCK_LEN ? I : MAX_BLOCK_LEN) :
    (K <= MAX_BLOCK_LEN ? K : MAX_BLOCK_LEN);
  const int B_blocks = b_transpose ? (K <= MAX_BLOCK_LEN ? K : MAX_BLOCK_LEN) :
    (J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN);
  const int D_blocks = low_D ? (J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN) :
    (J <= MAX_BLOCK_LEN_ACC ? J : MAX_BLOCK_LEN_ACC);
  const int C_blocks = full_C ? 1 : (J <= MAX_BLOCK_LEN ? J : MAX_BLOCK_LEN);

  const size_t sizeof_D = low_D ? sizeof(elem_t) : sizeof(acc_t);
  const size_t sizeof_C = full_C ? sizeof(acc_t) : sizeof(elem_t);

  // Move-in D
  if (D != NULL && !no_bias) {
    for (size_t i = 0; i < I; i++) {
      const size_t rows = DIM - (i == I-1 ? pad_I : 0);
      for (size_t j = 0; j < J; j += D_blocks) {
        const size_t bias_row = repeating_bias ? 0 : i;
        const void * const D_dram_addr = (int8_t *)D + (bias_row * D_row_stride + j)*DIM*sizeof_D;
        const uint32_t D_sp_addr_acc = D_sp_addr_start + (i*J + j)*DIM;
        size_t blocks = j + D_blocks <= J ? D_blocks : J-j;
        const size_t cols = blocks * DIM - (j + blocks >= J ? pad_J : 0);
        gemmini_extended_mvin3(D_dram_addr, D_sp_addr_acc, cols, rows);
      }
    }
  }

  for (size_t j = 0; j < J; j++) {
    for (size_t k = 0; k < K; k++) {
      for (size_t i = 0; i < I; i++) {
        const uint32_t A_sp_addr = a_transpose ? (A_sp_addr_start + (k*I + i)*DIM) :
          (A_sp_addr_start + (i*K + k)*DIM);
        const uint32_t B_sp_addr = b_transpose ? (B_sp_addr_start + (j*K + k)*DIM) :
          (B_sp_addr_start + (k*J + j)*DIM);
        const uint32_t C_sp_addr = C_sp_addr_start + (i*J + j)*DIM;

        // Mvin A
        if (a_transpose) {
          if (j == 0 && i % A_blocks == 0) {
            const elem_t * const A_dram_addr = A + (k*A_row_stride + i)*DIM;
            const size_t blocks = i + A_blocks <= I ? A_blocks : I-i;
            const size_t cols = blocks * DIM - (i + blocks >= I ? pad_I : 0);
            const size_t rows = DIM - (k == K-1 ? pad_K : 0);
            gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
          }
        } else {
          if (j == 0 && k % A_blocks == 0) {
            const elem_t * const A_dram_addr = A + (i*A_row_stride + k)*DIM;
            const size_t blocks = k + A_blocks <= K ? A_blocks : K-k;
            const size_t cols = blocks * DIM - (k + blocks >= K ? pad_K : 0);
            const size_t rows = DIM - (i == I-1 ? pad_I : 0);
            gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
          }
        }

        // Mvin B
        if (b_transpose) {
          if (i == 0 && k % B_blocks == 0) {
            const elem_t * const B_dram_addr = B + (j*B_row_stride + k)*DIM;
            const size_t blocks = k + B_blocks <= K ? B_blocks : K-k;
            const size_t cols = blocks * DIM - (k + blocks >= K ? pad_K : 0);
            const size_t rows = DIM - (j == J-1 ? pad_J : 0);
            gemmini_extended_mvin2(B_dram_addr, B_sp_addr, cols, rows);
          }
        } else {
          if (i == 0 && j % B_blocks == 0) {
            const elem_t * const B_dram_addr = B + (k*B_row_stride + j)*DIM;
            const size_t blocks = j + B_blocks <= J ? B_blocks : J-j;
            const size_t cols = blocks * DIM - (j + blocks >= J ? pad_J : 0);
            const size_t rows = DIM - (k == K-1 ? pad_K : 0);
            gemmini_extended_mvin2(B_dram_addr, B_sp_addr, cols, rows);
          }
        }

        // Compute
        {
          uint32_t pre_sp_addr = i == 0 ? B_sp_addr : GARBAGE_ADDR;
          uint32_t out_sp_addr = C_sp_addr;

          // If we're not using a bias, then we want to overwrite what's in the
          // accumulator, rather than writing over it
          int no_bias_new_matrix = no_bias && D != NULL && k == 0;
          if (no_bias_new_matrix) {
            out_sp_addr &= ~(1 << (ADDR_LEN-2));
          }

          const size_t A_cols = DIM - (k == K - 1 ? pad_K : 0);
          const size_t A_rows = DIM - (i == I - 1 ? pad_I : 0);
          const size_t B_cols = DIM - (j == J - 1 ? pad_J : 0);
          const size_t B_rows = DIM - (k == K - 1 ? pad_K : 0);
          const size_t C_cols = DIM - (j == J - 1 ? pad_J : 0);
          const size_t C_rows = DIM - (i == I - 1 ? pad_I : 0);

          gemmini_extended_preload(pre_sp_addr, out_sp_addr, B_cols, B_rows, C_cols, C_rows);

          if (i == 0) { // First iteration
            gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
          } else { // All other iterations
            gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, A_cols, A_rows, DIM, DIM);
          }
        }

        // Move-out C
        if (C != NULL && k == K-1 && (j == J-1 || j % C_blocks == C_blocks-1)) {
          const size_t rounded_j = (j / C_blocks) * C_blocks;

          const uint32_t rounded_C_sp_addr = C_sp_addr_start + (i*J + rounded_j)*DIM;
          void * const C_dram_addr = (int8_t*)C + (i*C_row_stride + rounded_j)*DIM*sizeof_C;

          const size_t blocks = rounded_j + C_blocks <= J ? C_blocks : J-rounded_j;
          const size_t cols = blocks * DIM - (rounded_j + blocks >= J ? pad_J : 0);
          const size_t rows = DIM - (i == I - 1 ? pad_I : 0);

          gemmini_extended_mvout(C_dram_addr, rounded_C_sp_addr, cols, rows);
        }
      }
    }
  }
  */
	// latency = 0 to turn of looploader
	int latency = detect_conflict ? 4000 : 0;//priority*1500 + 1000 : 0;
	int alert = 40;
	int unlock_cycle = 5;//4 + priority*2;
	int pause_turn = 2;
	if(skip_B) { 
		gemmini_loop_ld(false, false, K, J, pad_K, pad_J, B, B_row_stride, latency, alert, unlock_cycle, pause_turn);
	}
	else if(skip_A) {
		gemmini_loop_ld(true, false, I, K, pad_I, pad_K, A, A_row_stride, latency, alert, unlock_cycle, pause_turn);
	}

  // Combined loop
  gemmini_loop_ws(I, J, K, pad_I, pad_J, pad_K, A, B, no_bias ? NULL : D, C,
    A_row_stride, B_row_stride, repeating_bias ? 0 : D_row_stride, C_row_stride,
    a_transpose, b_transpose,
    full_C, low_D, !no_bias || D == NULL);
}

static void tiled_matmul_outer(size_t dim_I, size_t dim_J, size_t dim_K,
        const elem_t* A, const elem_t* B,
        const void * D, void * C,
        size_t stride_A, size_t stride_B, size_t stride_D, size_t stride_C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        size_t tile_I, size_t tile_J, size_t tile_K,
        int act, acc_scale_t scale, size_t relu6_shift, bool repeating_bias,
        bool a_transpose, bool b_transpose,
        bool full_C, bool low_D,
        int dataflow, bool skip_A, bool skip_B) {

  const size_t dim_I_padded = (dim_I / DIM + (dim_I % DIM != 0)) * DIM;
  const size_t dim_J_padded = (dim_J / DIM + (dim_J % DIM != 0)) * DIM;
  const size_t dim_K_padded = (dim_K / DIM + (dim_K % DIM != 0)) * DIM;

  const size_t I0 = dim_I_padded / (tile_I*DIM) + (dim_I_padded % (tile_I*DIM) != 0);
  const size_t J0 = dim_J_padded / (tile_J*DIM) + (dim_J_padded % (tile_J*DIM) != 0);
  const size_t K0 = dim_K_padded / (tile_K*DIM) + (dim_K_padded % (tile_K*DIM) != 0);

  // These lines here are supposed to help us deal with when the dimensions of
  // the systolic array aren't divisible by the tiling factors
  const size_t last_I = dim_I_padded % (tile_I*DIM) == 0 ? tile_I : (dim_I_padded/DIM) % tile_I;
  const size_t last_J = dim_J_padded % (tile_J*DIM) == 0 ? tile_J : (dim_J_padded/DIM) % tile_J;
  const size_t last_K = dim_K_padded % (tile_K*DIM) == 0 ? tile_K : (dim_K_padded/DIM) % tile_K;

  // These lines are supposed to figure out how much padding the hardware is
  // supposed to add for the final tile
  const size_t padding_I = dim_I_padded - dim_I;
  const size_t padding_J = dim_J_padded - dim_J;
  const size_t padding_K = dim_K_padded - dim_K;

  const bool no_bias = D == NULL;

  if (no_bias) {
    D = (void*) 1; // Dummy address which isn't NULL
  }

  const size_t sizeof_D = low_D ? sizeof(elem_t) : sizeof(acc_t) ;
  const size_t sizeof_C = full_C ? sizeof(acc_t) : sizeof(elem_t);

  gemmini_extended_config_ex(dataflow, act, 0, scale, relu6_shift, 1, a_transpose, b_transpose);
  gemmini_config_st(stride_C * sizeof_C);
  gemmini_extended3_config_ld(stride_A * sizeof(elem_t), A_scale_factor, false, 0);
  gemmini_extended3_config_ld(stride_B * sizeof(elem_t), B_scale_factor, false, 1);
  gemmini_extended3_config_ld(repeating_bias ? 0 : (stride_D * sizeof_D), D_scale_factor, low_D, 2);

  void (*inner)(const elem_t *, const elem_t *, const void *, void *,
        scale_t, scale_t, scale_acc_t,
        size_t, size_t, size_t, size_t, size_t, size_t,
        size_t, size_t, size_t, size_t,
        bool, bool,
        bool, bool,
        bool, bool,
		  bool, bool, bool);

  if (dataflow == OUTPUT_STATIONARY) {
    inner = &sp_tiled_matmul_os;
  } else /* if (dataflow == WEIGHT_STATIONARY) */ {
    inner = &sp_tiled_matmul_ws;
  }

  //ToDo: if skip_B, J outermost loop / if skip_A, I outermost loop
  for (size_t j0 = 0; j0 < J0; j0++)
    for (size_t i1 = 0; i1 < I0; i1++){
		size_t i0 = ((int)(j0) % 2 == 0) ? i1 : I0 - i1 - 1;
		bool detect_conflict = (i1 != 0) && (skip_A || skip_B);
      for (size_t k0 = 0; k0 < K0; k0++) {
        const void * pre;
        if (k0 != 0) {
          pre = NULL;
        } else {
          size_t bias_row = repeating_bias ? 0 : i0*tile_I*DIM;
          // pre = &(((acc_t*)D)[bias_row * stride_D + j0 * tile_J * DIM]);
          pre = (int8_t*)D + (bias_row * stride_D + j0 * tile_J * DIM)*sizeof_D;
        }
        void * out = k0 == K0-1 ? (int8_t*)C + (i0*tile_I*DIM*stride_C + j0*tile_J*DIM)*sizeof_C : NULL;

        const size_t I = i0 < I0-1 ? tile_I : last_I;
        const size_t J = j0 < J0-1 ? tile_J : last_J;
        const size_t K = k0 < K0-1 ? tile_K : last_K;

        const size_t pad_I = i0 == I0-1 ? padding_I : 0;
        const size_t pad_J = j0 == J0-1 ? padding_J : 0;
        const size_t pad_K = k0 == K0-1 ? padding_K : 0;

        const elem_t * a = a_transpose ? (A + k0*tile_K*DIM*stride_A + i0*tile_I*DIM)
          : (A + i0*tile_I*DIM*stride_A + k0*tile_K*DIM);

        const elem_t * b = b_transpose ? (B + j0*tile_J*DIM*stride_B + k0*tile_K*DIM)
          : (B + k0*tile_K*DIM*stride_B + j0*tile_J*DIM);

        (*inner)(a, b, pre, out,
            A_scale_factor, B_scale_factor, D_scale_factor,
            I, J, K,
            pad_I, pad_J, pad_K,
            stride_A, stride_B, stride_D, stride_C,
            a_transpose, b_transpose,
            full_C, low_D,
            no_bias, repeating_bias,
				detect_conflict, skip_A, skip_B);
      }
	 }
  gemmini_fence();
}

static elem_t scale_and_sat(acc_t x, int act, acc_scale_t scale, size_t relu6_shift) {
  // Scale value down and round it
  x = ACC_SCALE(x, scale);
  // Clip result
  x = x > elem_t_max ? elem_t_max : (x < elem_t_min ? elem_t_min : x);
  // Apply activation function
  if (act == RELU) {
    x = x < 0 ? 0 : x;
  }
  // TODO add another define to check if relu6_shift is actually used or not
  else if (act == RELU6) {
    int max = 6 << relu6_shift;
    x = x < 0 ? 0 : (x > max ? max : x);
  }
  return x;
}

#ifdef HAS_MVIN_SCALE
#define GEMMINI_SCALE(x, scale) MVIN_SCALE((x), (scale))
#else
#define GEMMINI_SCALE(x, scale) (x)
#endif

#ifdef HAS_MVIN_ACC_SCALE
#define GEMMINI_ACC_SCALE(x, scale) MVIN_SCALE_ACC((x), (scale))
#else
#define GEMMINI_ACC_SCALE(x, scale) (x)
#endif

static void matmul_cpu(bool transA, bool transB, size_t DIM_I, size_t DIM_J, size_t DIM_K,
        const elem_t* A, const elem_t* B, const acc_t * D,
        elem_t* C,
        size_t stride_A, size_t stride_B, size_t stride_D, size_t stride_C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        int act, acc_scale_t scale, size_t relu6_shift, bool repeating_bias) {

  const int no_bias = D == NULL;
  if (DIM_I % 4 == 0 && DIM_J % 4 == 0) {
    for (size_t i = 0; i < DIM_I; i += 4) {
      for (size_t j = 0; j < DIM_J; j += 4) {

        acc_t result[4][4]; // = {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

        for (size_t ii = 0; ii < 4; ii++)
          for (size_t jj = 0; jj < 4; jj++) {
            const size_t bias_row = repeating_bias ? 0 : i + ii;
            result[ii][jj] = no_bias ? 0 :
              GEMMINI_ACC_SCALE(*(D + bias_row*stride_D + j + jj), D_scale_factor);
          }

        for (size_t k = 0; k < DIM_K; k++) {
          result[0][0] +=
                GEMMINI_SCALE(*(A + i*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j), B_scale_factor);
          result[0][1] +=
                GEMMINI_SCALE(*(A + i*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+1), B_scale_factor);
          result[0][2] +=
                GEMMINI_SCALE(*(A + i*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+2), B_scale_factor);
          result[0][3] +=
                GEMMINI_SCALE(*(A + i*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+3), B_scale_factor);
          result[1][0] +=
                GEMMINI_SCALE(*(A + (i+1)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j), B_scale_factor);
          result[1][1] +=
                GEMMINI_SCALE(*(A + (i+1)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+1), B_scale_factor);
          result[1][2] +=
                GEMMINI_SCALE(*(A + (i+1)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+2), B_scale_factor);
          result[1][3] +=
                GEMMINI_SCALE(*(A + (i+1)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+3), B_scale_factor);
          result[2][0] +=
                GEMMINI_SCALE(*(A + (i+2)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j), B_scale_factor);
          result[2][1] +=
                GEMMINI_SCALE(*(A + (i+2)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+1), B_scale_factor);
          result[2][2] +=
                GEMMINI_SCALE(*(A + (i+2)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+2), B_scale_factor);
          result[2][3] +=
                GEMMINI_SCALE(*(A + (i+2)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+3), B_scale_factor);
          result[3][0] +=
                GEMMINI_SCALE(*(A + (i+3)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j), B_scale_factor);
          result[3][1] +=
                GEMMINI_SCALE(*(A + (i+3)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+1), B_scale_factor);
          result[3][2] +=
                GEMMINI_SCALE(*(A + (i+3)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+2), B_scale_factor);
          result[3][3] +=
                GEMMINI_SCALE(*(A + (i+3)*stride_A + k), A_scale_factor) *
                GEMMINI_SCALE(*(B + k*stride_B + j+3), B_scale_factor);
        }

        *(C + i*stride_C + j) =
             scale_and_sat(result[0][0], act, scale, relu6_shift);
        *(C + i*stride_C + j+1) =
             scale_and_sat(result[0][1], act, scale, relu6_shift);
        *(C + i*stride_C + j+2) =
             scale_and_sat(result[0][2], act, scale, relu6_shift);
        *(C + i*stride_C + j+3) =
             scale_and_sat(result[0][3], act, scale, relu6_shift);
        *(C + (i+1)*stride_C + j) =
             scale_and_sat(result[1][0], act, scale, relu6_shift);
        *(C + (i+1)*stride_C + j+1) =
             scale_and_sat(result[1][1], act, scale, relu6_shift);
        *(C + (i+1)*stride_C + j+2) =
             scale_and_sat(result[1][2], act, scale, relu6_shift);
        *(C + (i+1)*stride_C + j+3) =
             scale_and_sat(result[1][3], act, scale, relu6_shift);
        *(C + (i+2)*stride_C + j) =
             scale_and_sat(result[2][0], act, scale, relu6_shift);
        *(C + (i+2)*stride_C + j+1) =
             scale_and_sat(result[2][1], act, scale, relu6_shift);
        *(C + (i+2)*stride_C + j+2) =
             scale_and_sat(result[2][2], act, scale, relu6_shift);
        *(C + (i+2)*stride_C + j+3) =
             scale_and_sat(result[2][3], act, scale, relu6_shift);
        *(C + (i+3)*stride_C + j) =
             scale_and_sat(result[3][0], act, scale, relu6_shift);
        *(C + (i+3)*stride_C + j+1) =
             scale_and_sat(result[3][1], act, scale, relu6_shift);
        *(C + (i+3)*stride_C + j+2) =
             scale_and_sat(result[3][2], act, scale, relu6_shift);
        *(C + (i+3)*stride_C + j+3) =
             scale_and_sat(result[3][3], act, scale, relu6_shift);
      }
    }
  } else {
    for (size_t i = 0; i < DIM_I; i++) {
      for (size_t j = 0; j < DIM_J; j++) {
        const elem_t* a = !transA ? (A + (i * stride_A)) : A + i;
        const elem_t* b = !transB ? (B + j) : (B + (j * stride_B));
        elem_t* c = C + (i * stride_C) + j;

        const size_t bias_row = repeating_bias ? 0 : i;
        acc_t sum = no_bias ? 0 : GEMMINI_ACC_SCALE(*(D + bias_row * stride_D + j), D_scale_factor);

        for (size_t k = 0; k < DIM_K; k++) {
          sum += (GEMMINI_SCALE(*a, A_scale_factor) * GEMMINI_SCALE(*b, B_scale_factor));
          b += !transB ? stride_B : 1;
          a += !transA ? 1 : stride_A;
        }

        *c = scale_and_sat(sum, act, scale, relu6_shift);
      }
    }
  }
}

#undef GEMMINI_SCALE

// General matmul which can be run with different dataflows, or on the CPU
enum tiled_matmul_type_t {OS, WS, CPU}; // TODO rename this so it's name also applies to convs

// This function runs a tiled matrix multiplication, with hardcoded tiling
// factors
static void tiled_matmul(size_t dim_I, size_t dim_J, size_t dim_K,
        const elem_t* A, const elem_t* B,
        const void * D, void* C,
        size_t stride_A, size_t stride_B, size_t stride_D, size_t stride_C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        int act, acc_scale_t scale, size_t relu6_shift, bool repeating_bias,
        size_t tile_I, size_t tile_J, size_t tile_K,
        bool transpose_A, bool transpose_B,
        bool full_C, bool low_D,
        enum tiled_matmul_type_t tiled_matmul_type,
		  bool skip_A, bool skip_B) {

#ifdef GEMMINI_ASSERTIONS
  // Make sure that the tiling factors make sense
  if (tile_I <= 0) {
    printf("tile_I is non-positive\n");
    exit(1);
  } else if (tile_J <= 0) {
    printf("tile_J is non-positive\n");
    exit(1);
  } else if (tile_K <= 0) {
    printf("tile_K is non-positive\n");
    exit(1);
  }

  const size_t dim_I_padded = (dim_I / DIM + (dim_I % DIM != 0)) * DIM;
  const size_t dim_J_padded = (dim_J / DIM + (dim_J % DIM != 0)) * DIM;
  const size_t dim_K_padded = (dim_K / DIM + (dim_K % DIM != 0)) * DIM;

  if (tile_I * DIM > dim_I_padded) {
    printf("tile_I is too large (tile_I * DIM > dim_I_padded)\n");
    exit(1);
  } else if (tile_J * DIM > dim_J_padded) {
    printf("tile_J is too large (tile_J * DIM > dim_J_padded)\n");
    exit(1);
  } else if (tile_K * DIM > dim_K_padded) {
    printf("tile_K is too large (tile_K * DIM > dim_K_padded)\n");
    exit(1);
  }

  const bool double_buffered = tiled_matmul_type == WS;

  const size_t total_spad_size = double_buffered ? BANK_NUM * BANK_ROWS / 2 :
      BANK_NUM * BANK_ROWS;
  const size_t total_acc_size = double_buffered ? ACC_ROWS / 2 : ACC_ROWS;

  const size_t total_spad_rows =
      (tile_I * tile_K * DIM) +   // Rows to store A
      (tile_K * tile_J * DIM);    // Rows to store B

  if (total_spad_rows > total_spad_size) {
    printf("Not enough space in scratchpad to store A and B matrices\n");
    exit(1);
  }

  const size_t total_acc_rows =
      tile_I * tile_J * DIM;      // Rows to store C

  if (total_acc_rows > total_acc_size) {
    printf("Not enough space in accumulator to store C\n");
    exit(1);
  }

  if (tile_I > 65535 || tile_J > 65535 || tile_K > 65535) {
    printf("I, J, and K tiling factors must be less than 65535, to fit within the bounds of the LOOP_WS function");
    exit(1);
  }

  char matmul_type_str[][4] = {"OS", "WS", "CPU"};

  // Check if transpose options are correct
  if (((tiled_matmul_type == OS) && (transpose_A || transpose_B)) ||
    (tiled_matmul_type == WS && transpose_A && transpose_B)) {
    printf("Not implemented: %s matmul, a_transpose=%d, b_transpose=%d\n", matmul_type_str[tiled_matmul_type], transpose_A, transpose_B);
    exit(1);
  }

  // Check if full_C options are correct
  if ((tiled_matmul_type == CPU && (full_C || low_D)) ||
      (tiled_matmul_type == OS && low_D)) {
    printf("Not implemented: %s matmul, full_C=%d, low_D=%d\n", matmul_type_str[tiled_matmul_type], full_C, low_D);
  }
#endif

  // Run a tiled matrix multiplication on either Gemmini or the CPU
  if (tiled_matmul_type == OS || tiled_matmul_type == WS) {
    tiled_matmul_outer(dim_I, dim_J, dim_K,
        A, B, D, C,
        stride_A, stride_B, stride_D, stride_C,
        A_scale_factor, B_scale_factor, D_scale_factor,
        tile_I, tile_J, tile_K,
        act, scale, relu6_shift, repeating_bias,
        transpose_A, transpose_B,
        full_C, low_D,
        (int)tiled_matmul_type,
		  skip_A, skip_B);
  } else /*if (tiled_matmul_type == CPU)*/ {
    matmul_cpu(transpose_A, transpose_B, dim_I, dim_J, dim_K,
            A, B, (const acc_t*) D, (elem_t*)C,
            stride_A, stride_B, stride_D, stride_C,
            A_scale_factor, B_scale_factor, D_scale_factor,
            act, scale, relu6_shift, repeating_bias);
  }
}

static size_t tiled_matmul_total_spad_rows(size_t I, size_t J, size_t K) {
  return (I * K + K * J) * DIM;
}

static size_t tiled_matmul_total_acc_rows(size_t I, size_t J) {
  return (I * J) * DIM;
}

// This function runs a tiled matrix multiplication, with automatically
// calculated tiling factors
static void tiled_matmul_auto(size_t dim_I, size_t dim_J, size_t dim_K,
        const elem_t* A, const elem_t* B,
        const void * D, void * C,
        size_t stride_A, size_t stride_B, size_t stride_D, size_t stride_C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        int act, acc_scale_t scale, size_t relu6_shift, bool repeating_bias,
        bool transpose_A, bool transpose_B,
        bool full_C, bool low_D,
        enum tiled_matmul_type_t tiled_matmul_type) {

#define partition_rows (BANK_NUM * BANK_ROWS / 2)
#define mats_in_partition (partition_rows / DIM)
#define mats_in_acc (ACC_ROWS / DIM)
#define max_tile_i_j ((size_t)sqrt(mats_in_acc))
#define max_tile_k (mats_in_partition / max_tile_i_j)

    // "db_" means "double-buffered"
#define db_partition_rows ((BANK_NUM * BANK_ROWS / 2) / 2)
#define db_mats_in_partition (db_partition_rows / DIM)
#define db_mats_in_acc ((ACC_ROWS / 2) / DIM)
#define db_max_tile_i_j ((size_t)sqrt(db_mats_in_acc))
#define db_max_tile_k (db_mats_in_partition / db_max_tile_i_j)

    const size_t dim_I_padded = (dim_I / DIM + (dim_I % DIM != 0)) * DIM;
    const size_t dim_J_padded = (dim_J / DIM + (dim_J % DIM != 0)) * DIM;
    const size_t dim_K_padded = (dim_K / DIM + (dim_K % DIM != 0)) * DIM;

    const bool double_buffered = tiled_matmul_type == WS;

    const size_t max_spad_rows = double_buffered ? BANK_NUM * BANK_ROWS / 2 :
      BANK_NUM * BANK_ROWS;
    const size_t max_acc_rows = double_buffered ? ACC_ROWS / 2 : ACC_ROWS;

    size_t tile_I, tile_J, tile_K;

    if (double_buffered) {
       tile_I = dim_I_padded/DIM < db_max_tile_i_j ? dim_I_padded/DIM : db_max_tile_i_j;
       tile_J = dim_J_padded/DIM < db_max_tile_i_j ? dim_J_padded/DIM : db_max_tile_i_j;
       tile_K = dim_K_padded/DIM < db_max_tile_k ? dim_K_padded/DIM : db_max_tile_k;
    } else {
       tile_I = dim_I_padded/DIM < max_tile_i_j ? dim_I_padded/DIM : max_tile_i_j;
       tile_J = dim_J_padded/DIM < max_tile_i_j ? dim_J_padded/DIM : max_tile_i_j;
       tile_K = dim_K_padded/DIM < max_tile_k ? dim_K_padded/DIM : max_tile_k;
    }

    // Fill scratchpad as much as possible
    while (true) {
      bool increased = false;

      if (tiled_matmul_total_spad_rows(tile_I, tile_J+1, tile_K) <= max_spad_rows &&
          tiled_matmul_total_acc_rows(tile_I, tile_J+1) <= max_acc_rows &&
          (tile_J+1) * DIM <= dim_J_padded) {
        tile_J++;
        increased = true;
      }

      if (tiled_matmul_total_spad_rows(tile_I+1, tile_J, tile_K) <= max_spad_rows &&
          tiled_matmul_total_acc_rows(tile_I+1, tile_J) <= max_acc_rows &&
          (tile_I+1) * DIM <= dim_I_padded) {
        tile_I++;
        increased = true;
      }

      if (tiled_matmul_total_spad_rows(tile_I, tile_J, tile_K+1) <= max_spad_rows &&
          (tile_K+1) * DIM <= dim_K_padded) {
        tile_K++;
        increased = true;
      }

      if (!increased)
        break;
    }

    /*
    const int spad_rows = tiled_matmul_total_spad_rows(tile_I, tile_J, tile_K);
    const int acc_rows = tiled_matmul_total_acc_rows(tile_I, tile_J);

    printf("tile_I: %d\n", tile_I);
    printf("tile_J: %d\n", tile_J);
    printf("tile_K: %d\n\n", tile_J);

    printf("spad_rows: %d\n", spad_rows);
    printf("acc_rows: %d\n\n", acc_rows);

    printf("spad_row utilization: %d%%\n", (spad_rows * 100) / max_spad_rows);
    printf("acc_row utilization: %d%%\n\n", (acc_rows * 100) / max_acc_rows);
    */

    tiled_matmul(dim_I, dim_J, dim_K,
        A, B, D, C,
        stride_A, stride_B, stride_D, stride_C,
        A_scale_factor, B_scale_factor, D_scale_factor,
        act, scale, relu6_shift, repeating_bias,
        tile_I, tile_J, tile_K,
        transpose_A, transpose_B,
        full_C, low_D,
        tiled_matmul_type, false, false);

#undef partition_rows
#undef mats_in_partition
#undef mats_in_acc
#undef max_tile_i_j
#undef max_tile_k
}

// this is with loop_ld to detect multicore conflicts
static void tiled_matmul_auto_loopld(size_t dim_I, size_t dim_J, size_t dim_K,
        const elem_t* A, const elem_t* B,
        const void * D, void * C,
        size_t stride_A, size_t stride_B, size_t stride_D, size_t stride_C,
        scale_t A_scale_factor, scale_t B_scale_factor, scale_acc_t D_scale_factor,
        int act, acc_scale_t scale, size_t relu6_shift, bool repeating_bias,
        bool transpose_A, bool transpose_B,
        bool full_C, bool low_D,
        enum tiled_matmul_type_t tiled_matmul_type,
		  bool skip_A, bool skip_B) {

#define partition_rows (BANK_NUM * BANK_ROWS / 2)
#define mats_in_partition (partition_rows / DIM)
#define mats_in_acc (ACC_ROWS / DIM)
#define max_tile_i_j ((size_t)sqrt(mats_in_acc))
#define max_tile_k (mats_in_partition / max_tile_i_j)

    // "db_" means "double-buffered"
#define db_partition_rows ((BANK_NUM * BANK_ROWS / 2) / 2)
#define db_mats_in_partition (db_partition_rows / DIM)
#define db_mats_in_acc ((ACC_ROWS / 2) / DIM)
#define db_max_tile_i_j ((size_t)sqrt(db_mats_in_acc))
#define db_max_tile_k (db_mats_in_partition / db_max_tile_i_j)

    const size_t dim_I_padded = (dim_I / DIM + (dim_I % DIM != 0)) * DIM;
    const size_t dim_J_padded = (dim_J / DIM + (dim_J % DIM != 0)) * DIM;
    const size_t dim_K_padded = (dim_K / DIM + (dim_K % DIM != 0)) * DIM;

    const bool double_buffered = tiled_matmul_type == WS;

    const size_t max_spad_rows = double_buffered ? BANK_NUM * BANK_ROWS / 2 :
      BANK_NUM * BANK_ROWS;
    const size_t max_acc_rows = double_buffered ? ACC_ROWS / 2 : ACC_ROWS;

    size_t tile_I, tile_J, tile_K;

    if (double_buffered) {
       tile_I = dim_I_padded/DIM < db_max_tile_i_j ? dim_I_padded/DIM : db_max_tile_i_j;
       tile_J = dim_J_padded/DIM < db_max_tile_i_j ? dim_J_padded/DIM : db_max_tile_i_j;
       tile_K = dim_K_padded/DIM < db_max_tile_k ? dim_K_padded/DIM : db_max_tile_k;
    } else {
       tile_I = dim_I_padded/DIM < max_tile_i_j ? dim_I_padded/DIM : max_tile_i_j;
       tile_J = dim_J_padded/DIM < max_tile_i_j ? dim_J_padded/DIM : max_tile_i_j;
       tile_K = dim_K_padded/DIM < max_tile_k ? dim_K_padded/DIM : max_tile_k;
    }

    // Fill scratchpad as much as possible
    while (true) {
      bool increased = false;

      if (tiled_matmul_total_spad_rows(tile_I, tile_J+1, tile_K) <= max_spad_rows &&
          tiled_matmul_total_acc_rows(tile_I, tile_J+1) <= max_acc_rows &&
          (tile_J+1) * DIM <= dim_J_padded) {
        tile_J++;
        increased = true;
      }

      if (tiled_matmul_total_spad_rows(tile_I+1, tile_J, tile_K) <= max_spad_rows &&
          tiled_matmul_total_acc_rows(tile_I+1, tile_J) <= max_acc_rows &&
          (tile_I+1) * DIM <= dim_I_padded) {
        tile_I++;
        increased = true;
      }

      if (tiled_matmul_total_spad_rows(tile_I, tile_J, tile_K+1) <= max_spad_rows &&
          (tile_K+1) * DIM <= dim_K_padded) {
        tile_K++;
        increased = true;
      }

      if (!increased)
        break;
    }

    /*
    const int spad_rows = tiled_matmul_total_spad_rows(tile_I, tile_J, tile_K);
    const int acc_rows = tiled_matmul_total_acc_rows(tile_I, tile_J);

    printf("tile_I: %d\n", tile_I);
    printf("tile_J: %d\n", tile_J);
    printf("tile_K: %d\n\n", tile_J);

    printf("spad_rows: %d\n", spad_rows);
    printf("acc_rows: %d\n\n", acc_rows);

    printf("spad_row utilization: %d%%\n", (spad_rows * 100) / max_spad_rows);
    printf("acc_row utilization: %d%%\n\n", (acc_rows * 100) / max_acc_rows);
    */

    tiled_matmul(dim_I, dim_J, dim_K,
        A, B, D, C,
        stride_A, stride_B, stride_D, stride_C,
        A_scale_factor, B_scale_factor, D_scale_factor,
        act, scale, relu6_shift, repeating_bias,
        tile_I, tile_J, tile_K,
        transpose_A, transpose_B,
        full_C, low_D,
        tiled_matmul_type, skip_A, skip_B);

#undef partition_rows
#undef mats_in_partition
#undef mats_in_acc
#undef max_tile_i_j
#undef max_tile_k
}

// this is with loop_ld to detect multicore conflicts
static void tiled_matmul_profile(size_t repeat){
#define partition_rows (BANK_NUM * BANK_ROWS / 2)
#define mats_in_partition (partition_rows / DIM)
#define mats_in_acc (ACC_ROWS / DIM)
#define max_tile_i_j ((size_t)sqrt(mats_in_acc))
#define max_tile_k (mats_in_partition / max_tile_i_j)

    // "db_" means "double-buffered"
#define db_partition_rows ((BANK_NUM * BANK_ROWS / 2) / 2)
#define db_mats_in_partition (db_partition_rows / DIM)
#define db_mats_in_acc ((ACC_ROWS / 2) / DIM)
#define db_max_tile_i_j ((size_t)sqrt(db_mats_in_acc))
#define db_max_tile_k (db_mats_in_partition / db_max_tile_i_j)
/*
    const size_t dim_I_padded = (dim_I / DIM + (dim_I % DIM != 0)) * DIM;
    const size_t dim_J_padded = (dim_J / DIM + (dim_J % DIM != 0)) * DIM;
    const size_t dim_K_padded = (dim_K / DIM + (dim_K % DIM != 0)) * DIM;

    const size_t max_spad_rows = double_buffered ? BANK_NUM * BANK_ROWS / 2 :
      BANK_NUM * BANK_ROWS;
    const size_t max_acc_rows = double_buffered ? ACC_ROWS / 2 : ACC_ROWS;
*/
    size_t tile_I, tile_J, tile_K;
	 tile_I = 4;// db_max_tile_i_j;
	 tile_K = 8;//db_max_tile_k;
	 const size_t dim_I = tile_I * DIM;
	 const size_t dim_K = tile_K * DIM;
	 const size_t stride_A = dim_K;
#define col_A db_max_tile_k*DIM
#define row_A db_max_tile_i_j*DIM
	 static elem_t small_A[64][128] row_align(MAX_BLOCK_LEN);
	 elem_t* A = (elem_t*) small_A;
	 // use small size A and B so that we don't need to tile
    //tile_I = dim_I_padded/DIM < max_tile_i_j ? dim_I_padded/DIM : max_tile_i_j;
    //tile_J = dim_J_padded/DIM < max_tile_i_j ? dim_J_padded/DIM : max_tile_i_j;
    //tile_K = dim_K_padded/DIM < max_tile_k ? dim_K_padded/DIM : max_tile_k;
    /*
    const int spad_rows = tiled_matmul_total_spad_rows(tile_I, tile_J, tile_K);
    const int acc_rows = tiled_matmul_total_acc_rows(tile_I, tile_J);

    printf("tile_I: %d\n", tile_I);
    printf("tile_J: %d\n", tile_J);
    printf("tile_K: %d\n\n", tile_J);

    printf("spad_rows: %d\n", spad_rows);
    printf("acc_rows: %d\n\n", acc_rows);

    printf("spad_row utilization: %d%%\n", (spad_rows * 100) / max_spad_rows);
    printf("acc_row utilization: %d%%\n\n", (acc_rows * 100) / max_acc_rows);
    */

  gemmini_extended3_config_ld(stride_A * sizeof(elem_t), MVIN_SCALE_IDENTITY, false, 0);
  for(int i = 0; i < repeat; i++){
  	   gemmini_loop_ld(true, true, tile_I, tile_K, 0, 0, A, stride_A, 0, 0, 0, (i > 3) ?  3 : 0);
		gemmini_fence();
  }

#undef partition_rows
#undef mats_in_partition
#undef mats_in_acc
#undef max_tile_i_j
#undef max_tile_k
#undef col_A
#undef row_A
}



static void sp_tiled_conv_A_stride(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,
		  int in_stride, int out_stride, int weight_stride,

        int stride, int padding, int kernel_dim,

        int pool_size, int pool_stride, int pool_padding,

        int batches,
        int porows, int pocols, int pochs,
        int krows, int kcols, int kchs,

        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        const elem_t * input,
        const elem_t * weights,
        elem_t * output,
        const acc_t * bias,
		  elem_t * pool_output,

        bool no_bias, bool no_pool, bool out_both,
		  bool skip_weight, bool detect_conflict) {

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    // Note: "irows" and "icols" includes padding
    const int irows = orows * stride + krows - 1; // - 2 * padding;
    const int icols = ocols * stride + kcols - 1; // - 2 * padding;
    const int irows_unpadded = irows - upad - dpad;
    const int icols_unpadded = icols - lpad - rpad;
    const int ichs = kchs;

    // Calculate spad address offsets
    const int out_channels_per_bank = ochs / DIM + (ochs % DIM != 0);
    const int B_rows = out_channels_per_bank * kcols * krows * kchs;

    static uint32_t D_sp_addr_row = 0;
    static uint32_t C_sp_addr_row = 0;

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = BANK_NUM * BANK_ROWS - B_rows;
    const uint32_t D_sp_addr_start = (1 << (ADDR_LEN - 1)) + D_sp_addr_row;
    const uint32_t C_sp_addr_start = (3 << (ADDR_LEN - 2)) + C_sp_addr_row;

    if (bias != 0) {
      D_sp_addr_row = (D_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
    }

    if (output != 0) {
      C_sp_addr_row = (C_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
    }

	int latency = 0;//detect_conflict ? 2500 : 0;//priority*1500 + 1000 : 0;
	int alert_cycle = 60;
	int unlock_cycle = detect_conflict ? 6 : 0;//4 + priority*2;
	int pause_turn = 2;
	bool depthwise = (kchs == 1) && (in_channels != 1);
	if(skip_weight){
		gemmini_loop_ld_conv(weights, weight_stride, in_channels, out_channels, kernel_dim, pochs, krows, kcols, kchs, latency, alert_cycle, unlock_cycle, pause_turn, depthwise);
	}
	gemmini_loop_conv_ws(batch_size, in_dim, in_channels, out_channels, out_dim, pool_out_dim, stride, padding, kernel_dim, pool_size, pool_stride, pool_padding, batches, porows, pocols, pochs, krows, kcols, kchs, lpad, rpad, upad, dpad, plpad, prpad, pupad, pdpad, orows, ocols, weights, pool_output, output, bias, input, no_bias, out_both, no_pool, depthwise, in_stride, weight_stride, out_stride);

    /*
    // mvin bias
    if (!no_bias && bias != NULL) {
        // TODO we probably don't need quite this many nested loops for this part

        const int max_ochs_per_mvin = ochs < MAX_BLOCK_LEN_ACC * DIM ? ochs :
            MAX_BLOCK_LEN_ACC * DIM;

        gemmini_extended4_config_ld(0, MVIN_SCALE_IDENTITY, false, batches * orows * ocols, 2);

        for (int b = 0; b < batches; b++)
            for (int orow = 0; orow < orows; orow++)
                for (int ocol = 0; ocol < ocols; ocol += DIM) {
                    const int I = ocols - ocol > DIM ? DIM : ocols - ocol;

                    for (int och = 0; och < ochs; och += max_ochs_per_mvin) {
                        const int J = ochs - och > max_ochs_per_mvin ? max_ochs_per_mvin : ochs - och;

                        const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                        gemmini_extended_mvin3(bias + och,
                                D_sp_addr,
                                J, I);
                    }
                }
    }

    // mvin input
    {
        const int max_ichs_per_mvin = ichs < MAX_BLOCK_LEN * DIM ? ichs :
            MAX_BLOCK_LEN * DIM;

        gemmini_extended4_config_ld(in_channels * sizeof(elem_t), MVIN_SCALE_IDENTITY, false, batches * irows * icols, 0);

        for (int b = 0; b < batches; b++)
            for (int irow = -upad; irow < irows_unpadded + dpad; irow++) {
                const int irow_padded = irow + upad;

                for (int icol = -lpad; icol < icols_unpadded + rpad;) {
                    // TODO There might be some unnecessary mvins here at the edge of the image

                    int I = icols_unpadded - icol > DIM ? DIM : icols_unpadded - icol;

                    if (icol < 0) {
                        I = -icol > DIM ? DIM : -icol;
                    } else if (icol >= icols_unpadded) {
                        I = icols_unpadded + rpad - icol > DIM ? DIM : icols_unpadded + rpad - icol;
                    }

                    const int icol_padded = icol + lpad;

                    for (int ich = 0; ich < ichs; ich += max_ichs_per_mvin) {
                        const int K = ichs - ich > max_ichs_per_mvin ?
                            max_ichs_per_mvin : ichs - ich;

                        const uint32_t A_sp_addr = A_sp_addr_start + (ich / DIM) * batches * irows * icols + b * irows * icols + irow_padded * icols + icol_padded;

                        const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded;

                        elem_t * in = is_zeros ? NULL :
                            input + (b*in_dim*in_dim + irow*in_dim + icol) * in_channels + ich;

                        gemmini_extended_mvin(in,
                                A_sp_addr,
                                K, I);
                    }

                    icol += I;
                }
            }
    }

    // mvin weights
    {
        const int max_ochs_per_mvin = ochs < MAX_BLOCK_LEN * DIM ? ochs :
            MAX_BLOCK_LEN * DIM;

        gemmini_extended4_config_ld(out_channels * sizeof(elem_t), MVIN_SCALE_IDENTITY, false, krows * kcols * kchs, 1);

        for (int och = 0; och < ochs; och += max_ochs_per_mvin) {
            const int J = ochs - och > max_ochs_per_mvin ?
                max_ochs_per_mvin : ochs - och;

            for (int krow = 0; krow < krows; krow++)
                for (int kcol = 0; kcol < kcols; kcol++)
                    for (int kch = 0; kch < kchs; kch += DIM) {
                        const int K = kchs - kch > DIM ? DIM : kchs - kch;

                        const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * krows * kcols * kchs + krow * kcols * kchs + kcol * kchs + kch;

                        gemmini_extended_mvin2(weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + och,
                            B_sp_addr,
                            J, K);
                    }
        }
    }

    for (int och = 0; och < ochs; och += DIM) {
      for (int krow = 0; krow < krows; krow++) {
        for (int kcol = 0; kcol < kcols; kcol++) {
          for (int kch = 0; kch < kchs; kch += DIM) {
            for (int b = 0; b < batches; b++) {
              for (int orow = 0; orow < orows; orow++) {
                for (int ocol = 0; ocol < ocols; ocol += DIM) {
                  const int irow = orow * stride + krow;
                  const int icol = ocol * stride + kcol;
                  const int C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;
                  // Over here, construct a new matrix
                  //
                  // Let us assume that we only ever operate on
                  // one pixel in one row.
                  // Thus, krows == kcols == 1
                  //
                  // Then, for every set of I, J, and K values
                  //     - I = ocols
                  //     - J = ochs
                  //     - K = kchs
                  const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
                  const int J = ochs - och > DIM ? DIM : ochs - och;
                  const int K = (kchs - kch > DIM ? DIM : kchs - kch);
                  const bool new_weights = b == 0 && orow == 0 && ocol == 0;
                  const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM) * batches * irows * icols + b * irows * icols + irow * icols + icol;
                  const uint32_t B_sp_addr = new_weights ?
                    (B_sp_addr_start + (och / DIM) * krows * kcols * kchs + krow * kcols * kchs + kcol * kchs + kch)
                    : GARBAGE_ADDR;
                  // perform matmul
                  const uint32_t out_sp_addr =
                    (bias != NULL && no_bias) && krow == 0 && kcol == 0 && kch == 0 ?
                    C_sp_addr & ~((uint32_t)(1 << (ADDR_LEN - 2))) :
                    C_sp_addr;
                  gemmini_extended_preload(B_sp_addr, out_sp_addr,
                          J, K, J, I);
                  if (new_weights) {
                    gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                  } else {
                    gemmini_extended_compute_accumulated(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                  }
					 }
				  }
				}
			 }
		  }
		}
	 }
	 // mvout output
    if (output != NULL) {
        if (no_pool) {
            for (int b = 0; b < batches; b++)
                for (int orow = 0; orow < orows; orow++)
                    for (int ocol = 0; ocol < ocols; ocol += DIM) {
                        const int I = ocols - ocol > DIM ? DIM : ocols - ocol;

                        for (int och = 0; och < ochs; och += DIM) {
                            const int J = ochs - och > DIM ? DIM : ochs - och;

                            const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                            gemmini_extended_mvout(output + (b*out_dim*out_dim + orow*out_dim + ocol) * out_channels + och,
                                    C_sp_addr,
                                    J, I);
                        }
                    }
        } else {
            gemmini_extended_config_st(out_channels * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);

            gemmini_fence(); // TODO remove this when the ROB can accurately handle these
            for (int b = 0; b < batches; b++) {
                for (int poch = 0; poch < pochs; poch += DIM) {
                    const int channels = poch + DIM >= pochs ? pochs - poch : DIM;

                    elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + poch;

                    const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;

                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            channels, 0);
                }
            }
            gemmini_fence();
        }
    }
    */
}

//resnet downsampling layer (no padding, kernel size 1, stride 2)
//due to poor instruction issue bandwidth
static void sp_tiled_conv_ds(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,

        int stride, //int padding, int kernel_dim,
        int pool_size, int pool_stride, int pool_padding  __attribute__((unused)),

        int batches,
        int porows, int pocols, int pochs,
        int kchs,

        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        const elem_t * input,
	    uint32_t B_sp_addr_outer,
        const elem_t * weights,
        elem_t * output,
        const acc_t * bias,

	    int act, acc_scale_t scale, int relu6_shift,
        bool no_bias, bool no_pool,
	    int weight_bank) {

    // const bool no_padding = lpad == 0 && rpad == 0 && upad == 0 && dpad == 0;
    // printf("SP_TILED_CONV no_padding: %d", no_padding);

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    const int irows = (orows - 1) * stride + 1;
    const int icols = (ocols - 1) * stride + 1;//kcols; 
    const int ichs = kchs;

	const int im2col_height = ocols*orows;
    // const int im2col_width = kchs;
	const int row_left = im2col_height%DIM;
	const int row_turn = row_left == 0 ? im2col_height/DIM - 1 : im2col_height/DIM;
	const int double_bank = weight_bank > 1 ? 1 : 0;
	const int triple_bank = weight_bank > 2 ? 1 : 0;
	  
    int odims = im2col_height;
 
	  gemmini_extended2_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, 1, false, false, ocols, row_turn, 1, stride, kchs, row_left, 1, double_bank, triple_bank); //if want 2 banks for weight, last is 1

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = B_sp_addr_outer == 0 ? (BANK_NUM - weight_bank) * BANK_ROWS : B_sp_addr_outer;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN - 1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN - 2);

    // printf("mvin bias\n");
    // mvin bias

    if (!no_bias && bias != NULL) {
        // TODO we probably don't need quite this many nested loops for this part
        gemmini_config_ld(0);
        for (int b = 0; b < batches; b++)
          for (int och = 0; och < ochs; och += DIM) {
               //const int J = ochs - och > DIM ? DIM : ochs - och;
               const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
	        for(int odim = 0; odim < odims; odim += DIM){
 		    const int I = odims - odim > DIM ? DIM : odims - odim;
                        gemmini_extended_mvin(bias + och,
                                D_sp_addr+odim,
                                DIM, I);
                    }
                }
    }

   // mvin weights if it hasn't moved-in in outer loop
//    printf("weight move in \n");
   if(B_sp_addr_outer == 0){
    gemmini_config_ld(out_channels*sizeof(elem_t));
    for (int och = 0; och < ochs; och += DIM) {
        const int J = ochs - och > DIM ? DIM : ochs - och;
        const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kchs; 
        for (int kch = 0; kch < kchs; kch += DIM) {
           const int K = kchs - kch > DIM ? DIM : kchs - kch;
           gemmini_extended_mvin(weights + kch * out_channels + och,
                        B_sp_addr+kch,
                        J, K);
	}
    }
   }

//	gemmini_fence();
 int idims = irows*icols; 
int bidims = batches*idims;
    // mvin input
//     printf("mvin inputs\n");
    gemmini_config_ld(in_channels * sizeof(elem_t));

   for (int b = 0; b < batches; b++) {
        for (int irow = 0; irow < irows; irow++) {
                const elem_t * in = input + (b*in_dim*in_dim + irow*in_dim) * in_channels;// + ich;
       		const uint32_t A_sp_addr = A_sp_addr_start + b * idims + irow * icols;
                   for (int ich = 0; ich < ichs; ich += DIM) {
                      // const int K = ichs - ich > DIM ? DIM : ichs - ich;
                       gemmini_extended_mvin(in+ich,
                            A_sp_addr + (ich/DIM)*bidims,
                            DIM, icols);
		}
       }
    }

  // Compute
  // previously attempted to merge with mvout
//   printf("compute  \n");
	//gemmini_fence();
	 if(odims > DIM){ //output dimension (row*col) bigger than DIM
	   for (int b = 0; b < batches; b++){
	        for (int och = 0; och < ochs; och += DIM) {
 		   const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kchs;// + kch;	 
     		   const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;

 		   for (int kch = 0; kch < kchs; kch += DIM) {
//                	gemmini_extended_mvin(weights + kch * out_channels + och,
//                        	B_sp_addr+kch,
//                        	DIM, DIM);
	
			const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM)*batches*idims + b*idims;           
            		for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
				const int I = odims - odim > DIM ? DIM : odims - odim;
                        	gemmini_extended_preload(B_sp_addr+kch, C_sp_addr+odim,
                                	 DIM, DIM, DIM, I);
                        	gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, DIM, I, DIM, I);
			}
            	  }
//	if(output!=NULL) gemmini_extended_mvout(output + (b * out_dim * out_dim)*out_channels + och, C_sp_addr, DIM, 0);
	
              }
     	  }
  	}else{//ds layer
	   for (int b = 0; b < batches; b++){
        	for (int och = 0; och < ochs; och += DIM) {
  	    		const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;
            		int kch_bound = 0;
  	    		for (int kch = 0; kch + 7*DIM < kchs; kch += 8*DIM) {
				const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kchs + kch;
/*
        			for(int kk = 0; kk < 8*DIM; kk += DIM){
                    			gemmini_extended_mvin(weights + (kk+kch) * out_channels + och,
                        			B_sp_addr+kk,
                        		DIM, DIM);
				}
*/
				const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM)*bidims + b*idims; 
                		gemmini_extended_preload(B_sp_addr, C_sp_addr,
                                        DIM, DIM, DIM, odims);
               		 	gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, DIM, odims, DIM, odims);

                		gemmini_extended_preload(B_sp_addr+DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr+bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);

                		gemmini_extended_preload(B_sp_addr+2*DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr+2*bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);

                		gemmini_extended_preload(B_sp_addr+3*DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr+3*bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);

                		gemmini_extended_preload(B_sp_addr + 4*DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr + 4*bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);

                		gemmini_extended_preload(B_sp_addr + 5*DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr+5*bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);

				gemmini_extended_preload(B_sp_addr + 6*DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr+6*bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);

				gemmini_extended_preload(B_sp_addr + 7 * DIM, C_sp_addr,
                                        DIM, DIM, DIM, odims);
				gemmini_extended_compute_preloaded(A_sp_addr+7*bidims, GARBAGE_ADDR, DIM, odims, DIM, odims);
				kch_bound = kch + 8*DIM;
                
                	}
	    //if kch is not divisible by DIM
 	    		for (; kch_bound < kchs; kch_bound += DIM) {
//	        		const int K = kchs - kch > DIM ? DIM : kchs - kch;
				const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kchs + kch_bound;
//                		gemmini_extended_mvin(weights + kch_bound * out_channels + och,
//                       		B_sp_addr,
//                        		DIM, DIM);
				const uint32_t A_sp_addr = A_sp_addr_start + (kch_bound / DIM)*bidims + b*idims; 
	
		                gemmini_extended_preload(B_sp_addr, C_sp_addr,
                		           DIM, DIM, DIM, odims);
                		gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, DIM, odims, DIM, odims);
               
                	}
//	const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;
//	if(output!=NULL) gemmini_extended_mvout(output + (b * out_dim * out_dim)*out_channels + och, C_sp_addr, DIM, 0);
	
            	}
       	   }
  	}

 // mvout output
//   printf("mvout \n");
   if (output != NULL) {
		gemmini_extended_config_st(out_channels * sizeof(elem_t), 0, 1, out_dim, 0, 0, orows, ocols, 0, 0);
		for(int b = 0; b < batches; b++)
			for(int och = 0; och < ochs; och += DIM){
				const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;
				gemmini_extended_mvout(output + (b * out_dim * out_dim)*out_channels + och, C_sp_addr, DIM, 0);
			}	
	}

}

static void sp_tiled_conv_dw(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,

        int stride, int padding, int kernel_dim,

        int pool_size, int pool_stride, int pool_padding,

        int batches,
        int porows, int pocols,// int pochs,
        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        elem_t * input,
        // uint32_t B_sp_addr_start,
        elem_t * weights,
        elem_t * output,
        acc_t * bias,

	    int act, acc_scale_t scale, int relu6_shift,
        bool no_bias, bool no_pool, bool mvin_weight
	) {

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    // Calculate image dimensions
    const int irows = (orows - 1) * stride + kernel_dim;
    const int icols = (ocols - 1) * stride + kernel_dim;//kcols; 
    const int irows_unpadded = irows - upad - dpad;
    const int icols_unpadded = icols - lpad - rpad;
    int kchs = 1;
    int kdims = kernel_dim * kernel_dim; 

    int double_bank = 0;//weight_bank > 1 ? 1 : 0;
    int triple_bank = 0;//weight_bank > 2 ? 1 : 0;
	const int odims = ocols*orows;
	const int row_left = odims%DIM;
	const int row_turn = row_left == 0 ? odims/DIM - 1 : odims/DIM;
	gemmini_extended2_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, 1, false, false, ocols, row_turn, kernel_dim, stride, kchs, row_left, kdims, double_bank, triple_bank); //if want 2 banks for weight, last is 1

    int idims = irows*icols;
    int bidims = batches*idims;

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = (BANK_NUM-1) * BANK_ROWS;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN - 1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN - 2);

   if (!no_bias && bias != NULL) { 
       gemmini_config_ld(0);
       for (int b = 0; b < batches; b++){
	    const int J = 1;
	    const uint32_t D_sp_addr = D_sp_addr_start + b * odims;// + odim;
            for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
		const int I = odims - odim > DIM ? DIM : odims - odim;
                gemmini_extended_mvin(bias,// + och,
                      	D_sp_addr+odim,
                       	J, I);
	    }
	  }
    }	

  if (mvin_weight) {
    // mvin weights
    // printf("weight move in \n");
       gemmini_config_ld(out_channels * sizeof(elem_t));
       for (int krow = 0; krow < kernel_dim; krow++){
            const uint32_t B_sp_addr = B_sp_addr_start+ krow*kernel_dim;
 
            for (int kcol = 0; kcol < kernel_dim; kcol++){
                    gemmini_extended_mvin(weights + (krow*kernel_dim + kcol) * out_channels,
                        B_sp_addr+kcol,
                        1, 1);
	    }
      }
    }
  
    // mvin input
    // printf("mvin inputs\n");
    gemmini_config_ld(in_channels * sizeof(elem_t));
    static elem_t zeros[MAX_BYTES / sizeof(elem_t)] = {0};
//    gemmini_fence(); // TODO fix ROB to get rid of this requirement
    for (int b = 0; b < batches; b++) {
        for (int irow = -upad; irow < irows_unpadded + dpad; irow++) {
            const int irow_padded = irow + upad;

            for (int icol = -lpad; icol < icols_unpadded + rpad;) {
                // TODO There might be some unnecessary mvins here at the edge of the image

                int I = icols_unpadded - icol > DIM ? DIM : icols_unpadded - icol;
                elem_t * in = input + (b*in_dim*in_dim + irow*in_dim + icol) * in_channels;// + ich;
 
                if (icol < 0) {
                    I = -icol > DIM ? DIM : -icol;
                } else if (icol >= icols_unpadded) {
                    I = icols_unpadded + rpad - icol > DIM ? DIM : icols_unpadded + rpad - icol;
                }
                const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded; 
                const int icol_padded = icol + lpad;
		const uint32_t A_sp_addr = A_sp_addr_start + b * idims + irow_padded * icols + icol_padded;
		if(is_zeros){
	           	   gemmini_config_ld(0);
			//for (int ich = 0; ich < ichs; ich += DIM) {
                    	   //const int K = ichs - ich > DIM ? DIM : ichs - ich;
                           in = &zeros[0];
                           gemmini_extended_mvin(in,//+ich,
                            A_sp_addr,// + (ich/DIM)*bidims,
                            1, I);
                    	//}
		   gemmini_config_ld(in_channels * sizeof(elem_t));


		}else{
                   //for (int ich = 0; ich < ichs; ich += DIM) {
                       //const int K = ichs - ich > DIM ? DIM : ichs - ich;
                       gemmini_extended_mvin(in,//+ich,
                            A_sp_addr,// + (ich/DIM)*bidims,
                            1, I);

                   // }
		}
                icol += I;
            }
        }
    }
//    gemmini_fence();
	
//   gemmini_config_ld(0);
   for (int b = 0; b < batches; b++){
	    const int J = 1;
	   //const uint32_t D_sp_addr = D_sp_addr_start + b * odims;// + odim;
      	    const uint32_t C_sp_addr_outer = C_sp_addr_start + b * odims;// + odim;
	    
		const uint32_t A_sp_addr = A_sp_addr_start + b*idims;           
		const int kkdims = kdims;
		const uint32_t B_sp_addr = B_sp_addr_start;
		const int K = 1;

            	for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
			const int I = odims - odim > DIM ? DIM : odims - odim;
                        //	gemmini_extended_mvin(bias,// + och,
                        //        	D_sp_addr+odim,
                        //        	J, I);
			const uint32_t C_sp_addr = C_sp_addr_outer + odim;

			for(int kkdim = 0; kkdim < kkdims; kkdim += K){	
                                gemmini_extended_preload(B_sp_addr + kkdim, C_sp_addr,
                                        J, K, J, I);
                                gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
                 
                	}
//		gemmini_extended_mvout(output + (b * out_dim * out_dim)*out_channels, C_sp_addr_outer, J, 0);
//	   }
         }
     }

   // mvout output
//    printf("mvout \n");
//    if (output != NULL) {
//        if (no_pool) {
            for (int b = 0; b < batches; b++)
                for (int orow = 0; orow < orows; orow++)
                    for (int ocol = 0; ocol < ocols; ocol += DIM) {
                        const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
                        const uint32_t C_sp_addr = C_sp_addr_start + b * orows * ocols + orow * ocols + ocol;

                        gemmini_extended_mvout(output + (b*out_dim*out_dim + orow*out_dim + ocol) * out_channels,
                                    C_sp_addr,
                                    1, I);
                        }    
}

//for first layer
static void sp_tiled_conv_first(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,

        int stride, int padding, //int kernel_dim,

        int pool_size, int pool_stride, int pool_padding,

        int batches,
        int porows, int pocols, int pochs,
	int krows, int kchs,

//        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        elem_t * input,
	//uint32_t B_sp_addr_start,
        elem_t * weights,
        elem_t * output,
        acc_t * bias,

	    int act, acc_scale_t scale, int relu6_shift,
        bool no_bias, bool no_pool, bool mvin_weight,
	int weight_bank) {

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    const int irows = (orows - 1) * stride + krows;
    const int icols = (ocols - 1) * stride + 1;//krows; 
    int kdims = krows*krows; 
    const int ichs = kchs*krows; //pack rows (kchs: normal channel number)

    int double_bank = weight_bank > 1 ? 1 : 0;
    int triple_bank = weight_bank > 2 ? 1 : 0;
    const int odims = ocols*orows;
    const int row_left = odims%DIM;
    const int row_turn = row_left == 0 ? odims/DIM - 1 : odims/DIM;
    gemmini_extended2_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, 1, false, false, ocols, row_turn, 1, stride, ichs, row_left, krows, double_bank, triple_bank); //if want 2 banks for weight, last is 1

    int idims = irows*icols;
    int bidims = batches*idims;

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = (BANK_NUM - weight_bank) * BANK_ROWS;// - B_rows;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN - 1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN - 2);

    // printf("mvin bias\n");
    // mvin bias

    if (!no_bias && bias != NULL) {
        // TODO we probably don't need quite this many nested loops for this part
        gemmini_config_ld(0);
        for (int b = 0; b < batches; b++)
           for (int och = 0; och < ochs; och += DIM) {
               const int J = ochs - och > DIM ? DIM : ochs - och;
               const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
	        for(int odim = 0; odim < odims; odim += DIM){
                   // const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
		    const int I = odims - odim > DIM ? DIM : odims - odim;
                        gemmini_extended_mvin(bias + och,
                                D_sp_addr+odim,
                                J, I);
                    }
                }
    	   }

//	printf("mvin_weight \n");
    if(mvin_weight){
	gemmini_config_ld(out_channels*sizeof(elem_t));
	  for (int och = 0; och < ochs; och += DIM) {
       		const int J = ochs - och > DIM ? DIM : ochs - och;
		for (int ich = 0; ich < ichs; ich += DIM) { //duplication for first layer
       		    const int K = ichs - ich > DIM ? DIM : ichs - ich;
        		    for (int krow = 0; krow < krows; krow++){
	               		   const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * krows * ichs+ ich*krows + krow*K;//krow * kcols * kchs + kcol * kchs + kch;
                      		   gemmini_extended_mvin(weights + (krow*(krows*in_channels) + ich) * out_channels + och,
                      				B_sp_addr,
                        			J, K);
	    			}
       			    }
    		  }	
    }
    // mvin input
//     printf("mvin inputs\n");
    gemmini_config_ld(in_channels * sizeof(elem_t));
    for (int b = 0; b < batches; b++) {
        for (int irow = 0; irow < irows; irow++) {
            for (int icol = 0; icol < icols;) {
                int I = icols - icol > DIM ? DIM : icols- icol;
                elem_t * in = input + (b*in_dim*in_dim + irow*in_dim + icol) * in_channels;// + ich;
 
		const uint32_t A_sp_addr = A_sp_addr_start + b * idims + irow * icols + icol;
                for (int ich = 0; ich < ichs; ich += DIM) {
                     const int K = ichs - ich > DIM ? DIM : ichs - ich;
                     gemmini_extended_mvin(in+ich,
                            A_sp_addr + (ich/DIM)*bidims,
                            K, I);

                }
                icol += I;
            }
        }
    }

//   printf("matmul computation \n");
   for (int b = 0; b < batches; b++){
        for (int och = 0; och < ochs; och += DIM) {
            const int J = ochs - och > DIM ? DIM : ochs - och;
	    const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
	    for (int kch = 0; kch < ichs; kch += DIM) { //treat as 3x7=21 channels
	        const int K = ichs - kch > DIM ? DIM : ichs - kch;
		const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM)*batches*idims + b*idims;           
		const int kkdims = K*krows;//kdims;
		const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kchs * kdims + kch*krows;

            	for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
			const int I = odims - odim > DIM ? DIM : odims - odim;
			for(int kkdim = 0; kkdim < kkdims; kkdim += K){	
                                gemmini_extended_preload(B_sp_addr + kkdim, C_sp_addr+odim,
                                        J, K, J, I);
                                gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
			}
                }
             }
/*
 //attempt to merge mvout with matmul
     	       if(output!=NULL){
                     elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + och;
                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            J, 0);
 		}
*/ 

        }
   }

    // mvout output
   if (output != NULL) {
               gemmini_extended_config_st(out_channels * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);
           for (int b = 0; b < batches; b++) {
               for (int poch = 0; poch < pochs; poch += DIM) {
                    const int channels = poch + DIM >= pochs ? pochs - poch : DIM;
                     elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + poch;
                    const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;
                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            channels, 0);
                }
            }
   }
}

//has mvin weight
static void sp_tiled_conv_ws_original(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,

        int stride, int padding, int kernel_dim,

        int pool_size, int pool_stride, int pool_padding,

        int batches,
        int porows, int pocols, int pochs,
	int krows, int kchs,

        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        const elem_t * input,
        const elem_t * weights,
        elem_t * output,
        const acc_t * bias,

	    int act, int scale, int relu6_shift,
        bool no_bias, bool no_pool,
	int weight_bank) {

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    const int irows = (orows - 1) * stride + krows;
    const int icols = (ocols - 1) * stride + krows;//kcols; 
    const int irows_unpadded = irows - upad - dpad;
    const int icols_unpadded = icols - lpad - rpad;
    const int ichs = kchs;
    int kdims = krows*krows; 
    int idims = irows*icols;
    int bidims = batches*irows*icols;

    int odims = ocols*orows;
    const int im2col_width = kdims*kchs;
    const int row_left = odims%DIM;
    const int row_turn = row_left == 0 ? odims/DIM - 1 : odims/DIM;
    const int turn = im2col_width%DIM == 0 ? im2col_width/DIM : im2col_width/DIM + 1;
    const int double_bank = weight_bank > 1 ? 1 : 0;
    const int triple_bank = weight_bank > 2 ? 1 : 0;
    gemmini_extended2_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, 1, false, false, ocols, row_turn, krows, stride, kchs, row_left, kdims, double_bank, triple_bank); //if want 2 banks for weight, last is 1

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = (BANK_NUM - weight_bank) * BANK_ROWS;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN - 1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN - 2);

    // printf("mvin bias\n");
    // mvin bias
    if (!no_bias && bias != NULL) {
        // TODO we probably don't need quite this many nested loops for this part
        gemmini_config_ld(0);
        for (int b = 0; b < batches; b++)
          for (int och = 0; och < ochs; och += DIM) {
               const int J = ochs - och > DIM ? DIM : ochs - och;
               const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
	        for(int odim = 0; odim < odims; odim += DIM){
       		    const int I = odims - odim > DIM ? DIM : odims - odim;
                    gemmini_extended_mvin(bias + och,
                                D_sp_addr+odim,
                                J, I);
                }
          }
    }

    // mvin weights
//    printf("weight move in \n");
    gemmini_config_ld(out_channels * sizeof(elem_t));
    for (int och = 0; och < ochs; och += DIM) {
        const int J = ochs - och > DIM ? DIM : ochs - och;

      for (int kch = 0; kch < kchs; kch += DIM) {
        const int K = kchs - kch > DIM ? DIM : kchs - kch;
        for (int krow = 0; krow < krows; krow++){
            const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs + kch*kdims + krow*krows*K;
 
            for (int kcol = 0; kcol < krows; kcol++){
                    gemmini_extended_mvin(weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + och,
                        B_sp_addr+kcol*K,
                        J, K);
	    }
        }
      }
    }
    // mvin input
    // printf("mvin inputs\n");
    gemmini_config_ld(in_channels * sizeof(elem_t));
    static elem_t zeros[MAX_BYTES / sizeof(elem_t)] = {0}; 
//    gemmini_fence(); // TODO fix ROB to get rid of this requirement
    for (int b = 0; b < batches; b++) {
        for (int irow = -upad; irow < irows_unpadded + dpad; irow++) {
            const int irow_padded = irow + upad;

            for (int icol = -lpad; icol < icols_unpadded + rpad;) {
                int I = icols_unpadded - icol > DIM ? DIM : icols_unpadded - icol;
                const elem_t * in = input + (b*in_dim*in_dim + irow*in_dim + icol) * in_channels;// + ich;
 
                if (icol < 0) {
                    I = -icol > DIM ? DIM : -icol;
                } else if (icol >= icols_unpadded) {
                    I = icols_unpadded + rpad - icol > DIM ? DIM : icols_unpadded + rpad - icol;
                }
                const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded; 
                const int icol_padded = icol + lpad;
		const uint32_t A_sp_addr = A_sp_addr_start + b * idims + irow_padded * icols + icol_padded;
		if(is_zeros){
                  	   gemmini_config_ld(0); 
			for (int ich = 0; ich < ichs; ich += DIM) {
                    	   const int K = ichs - ich > DIM ? DIM : ichs - ich;
                           in = &zeros[0];
                           gemmini_extended_mvin(in+ich,
                            A_sp_addr + (ich/DIM)*bidims,
                            K, I);
                         }
		        gemmini_config_ld(in_channels * sizeof(elem_t));
		}else{
                   for (int ich = 0; ich < ichs; ich += DIM) {
                       const int K = ichs - ich > DIM ? DIM : ichs - ich;
                       gemmini_extended_mvin(in+ich,
                            A_sp_addr + (ich/DIM)*bidims,
                            K, I);
                    }
		}
                icol += I;
            }
        }
    }
//    gemmini_fence(); // TODO fix ROB to get rid of this requirement
  // Compute
   for (int b = 0; b < batches; b++){
        for (int och = 0; och < ochs; och += DIM) {
            const int J = ochs - och > DIM ? DIM : ochs - och;
 	    for (int kch = 0; kch < kchs; kch += DIM) {
	        const int K = kchs - kch > DIM ? DIM : kchs - kch;
		const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM)*batches*idims + b*idims;           
		const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs + kch*kdims;	
            	for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
			const int I = odims - odim > DIM ? DIM : odims - odim;
     	       	    	const int C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims + odim;	           
			for(int kkdim = 0; kkdim < K*kdims; kkdim += K){
			        gemmini_extended_preload(B_sp_addr+kkdim, C_sp_addr,
                                        J, K, J, I);
                                gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
			}
                    }
                }
            }
       }

    // mvout output
   if (output != NULL) {
        if (no_pool) {
            for (int b = 0; b < batches; b++)
                for (int orow = 0; orow < orows; orow++)
                    for (int ocol = 0; ocol < ocols; ocol += DIM) {
                        const int I = ocols - ocol > DIM ? DIM : ocols - ocol;

                        for (int och = 0; och < ochs; och += DIM) {
                            const int J = ochs - och > DIM ? DIM : ochs - och;
		            const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                            gemmini_extended_mvout(output + (b*out_dim*out_dim + orow*out_dim + ocol) * out_channels + och,
                                    C_sp_addr,
                                    J, I);
                        }
                    
                    }

	   } else {
//		   printf("pool \n");
              gemmini_extended_config_st(out_channels * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);
            gemmini_fence(); // TODO remove this when the ROB can accurately handle these
            for (int b = 0; b < batches; b++) {
                for (int poch = 0; poch < pochs; poch += DIM) {
                    const int channels = poch + DIM >= pochs ? pochs - poch : DIM;
                    elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + poch;
                    const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;
                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            channels, 0);
                }
            }
            gemmini_fence(); // TODO remove this when the ROB can accurately handle these
       }
   }

}

//first layer padding region
static void sp_tiled_conv_ws_original_first(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,

        int stride, int padding, int kernel_dim,

        int pool_size, int pool_stride, int pool_padding,

        int batches,
        int porows, int pocols, int pochs,
	int krows, int kchs,

        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        elem_t * input,
        elem_t * weights,
        elem_t * output,
        acc_t * bias,

        int act, acc_scale_t scale, int relu6_shift,
        bool no_bias, bool no_pool, bool mvin_weight,
	int weight_bank) {

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    const int irows = (orows - 1) * stride + krows;
    const int icols = (ocols - 1) * stride + krows;//kcols; 
    const int irows_unpadded = irows - upad - dpad;
    const int icols_unpadded = icols - lpad - rpad;
    const int ichs = kchs;
    int kdims = krows*krows; 
int idims = irows*icols;
int bidims = batches*irows*icols;

    int odims = ocols*orows;
    const int row_left = odims%DIM;
    const int row_turn = row_left == 0 ? odims/DIM - 1 : odims/DIM;
    const int double_bank = weight_bank > 1 ? 1 : 0;
    const int triple_bank = weight_bank > 2 ? 1 : 0;
    gemmini_extended2_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, 1, false, false, ocols, row_turn, krows, stride, kchs, row_left, kdims, double_bank, triple_bank); //if want 2 banks for weight, last is 1

    const uint32_t A_sp_addr_start = 0;
    const uint32_t B_sp_addr_start = (BANK_NUM - weight_bank) * BANK_ROWS;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN - 1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN - 2);

    // printf("mvin bias\n");
    // mvin bias
    if (!no_bias && bias != NULL) {
        // TODO we probably don't need quite this many nested loops for this part
        gemmini_config_ld(0);
        for (int b = 0; b < batches; b++)
          for (int och = 0; och < ochs; och += DIM) {
               const int J = ochs - och > DIM ? DIM : ochs - och;
               const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
	        for(int odim = 0; odim < odims; odim += DIM){
		    const int I = odims - odim > DIM ? DIM : odims - odim;
                    gemmini_extended_mvin(bias + och,
                                D_sp_addr+odim,
                                J, I);
                }
           }
    }

    // mvin input
    // printf("mvin inputs\n");
    gemmini_config_ld(in_channels * sizeof(elem_t));
    static elem_t zeros[MAX_BYTES / sizeof(elem_t)] = {0}; 
   for (int b = 0; b < batches; b++) {
        for (int irow = -upad; irow < irows_unpadded + dpad; irow++) {
            const int irow_padded = irow + upad;

            for (int icol = -lpad; icol < icols_unpadded + rpad;) {
                int I = icols_unpadded - icol > DIM ? DIM : icols_unpadded - icol;
                elem_t * in = input + (b*in_dim*in_dim + irow*in_dim + icol) * in_channels;// + ich;
 
                if (icol < 0) {
                    I = -icol > DIM ? DIM : -icol;
                } else if (icol >= icols_unpadded) {
                    I = icols_unpadded + rpad - icol > DIM ? DIM : icols_unpadded + rpad - icol;
                }
                const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded; 
                const int icol_padded = icol + lpad;
		const uint32_t A_sp_addr = A_sp_addr_start + b * idims + irow_padded * icols + icol_padded;
		if(is_zeros){
                        gemmini_config_ld(0); 
                        in = &zeros[0];
                        gemmini_extended_mvin(in,
                            A_sp_addr,
                            ichs, I);
	 	        gemmini_config_ld(in_channels * sizeof(elem_t));
		}else{
                      gemmini_extended_mvin(in,
                            A_sp_addr,
                            ichs, I);

       		}
                icol += I;
            }
        }
    }
//    gemmini_fence(); // TODO fix ROB to get rid of this requirement

  if(mvin_weight){
    gemmini_config_ld(out_channels * sizeof(elem_t));
    for (int och = 0; och < ochs; och += DIM) {
        const int J = ochs - och > DIM ? DIM : ochs - och;
        const int K = kchs;//kchs - kch > DIM ? DIM : kchs - kch;
        for (int krow = 0; krow < krows; krow++){
            const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs + krow*krows*K;
 
            for (int kcol = 0; kcol < krows; kcol++){
                    gemmini_extended_mvin(weights + (krow*kernel_dim*in_channels + kcol*in_channels) * out_channels + och,
                        B_sp_addr+kcol*K,
                        J, K);
	    }
        }
    
    }
  }

  // Compute
//if(krows != 1){
    for (int b = 0; b < batches; b++){
        for (int och = 0; och < ochs; och += DIM) {
            const int J = ochs - och > DIM ? DIM : ochs - och;
      	    const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;	           
	    const int K = kchs;// - kch > DIM ? DIM : kchs - kch;
	    const uint32_t A_sp_addr = A_sp_addr_start + b*idims;           
	    const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs;// + kch*kdims;	
            for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
		const int I = odims - odim > DIM ? DIM : odims - odim;
     		for(int kkdim = 0; kkdim < K*kdims; kkdim += K){
		        gemmini_extended_preload(B_sp_addr+kkdim, C_sp_addr+odim,
                                J, K, J, I);
                        gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
		}
            }
/* 
 //attempt to merge matmul and mvout          
                    elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + och;
                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            J, 0);
*/ 
	}
    }

   if (output != NULL) {
           gemmini_extended_config_st(out_channels * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);
           for (int b = 0; b < batches; b++) {
                for (int poch = 0; poch < pochs; poch += DIM) {
                    const int channels = poch + DIM >= pochs ? pochs - poch : DIM;

                    elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + poch;

                    const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;

                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            channels, 0);
                }
            }
   }

}

static int tiled_conv_total_spad_rows(bool acc, bool weight,
        int stride,
        int batches,
        int porows, int pocols, int ochs,
        int krows, int kcols, int kchs,
        int pool_size, int pool_stride) {

    const int orows = porows * pool_stride + pool_size - 1;
    const int ocols = pocols * pool_stride + pool_size - 1;

    const int irows = orows * stride + krows - 1; // - 2 * padding;
    const int icols = ocols * stride + kcols - 1; // - 2 * padding;
    const int ichs = kchs;

    const int in_channels_per_bank = ichs / DIM + (ichs % DIM != 0);
    const int out_channels_per_bank = ochs / DIM + (ochs % DIM != 0);

    const int A_rows = in_channels_per_bank * batches * irows * icols;
    const int B_rows = out_channels_per_bank * kcols * krows * kchs;
    const int C_rows = out_channels_per_bank * batches * orows * ocols;

    if (acc)
        return C_rows;
    else if(weight)
        return B_rows;
    else
        return A_rows;
}

static int tiled_conv_total_spad_rows_A_stride(bool acc,
        int stride,
        int dilation,
        int batches,
        int porows, int pocols, int ochs,
        int krows, int kcols, int kchs,
        int pool_size, int pool_stride) {

    const int orows = porows * pool_stride + pool_size - 1;
    const int ocols = pocols * pool_stride + pool_size - 1;

    const int irows = orows * stride + dilation * (krows - 1); // - 2 * padding;
    const int icols = ocols * stride + dilation * (kcols - 1); // - 2 * padding;
    const int ichs = kchs;

    const int in_channels_per_bank = ichs / DIM + (ichs % DIM != 0);
    const int out_channels_per_bank = ochs / DIM + (ochs % DIM != 0);

    const int A_rows = in_channels_per_bank * batches * irows * icols;
    const int B_rows = out_channels_per_bank * kcols * krows * kchs;
    const int C_rows = out_channels_per_bank * batches * orows * ocols;

    return acc ? C_rows : A_rows + B_rows;
}

static void conv_cpu_without_pool(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift) {

  bool no_bias = bias == NULL;

  for (int b = 0; b < batch_size; b++) {
    for (int orow = 0; orow < out_dim; orow++) {
      for (int ocol = 0; ocol < out_dim; ocol++) {
        for (int och = 0; och < out_channels; och++) {

          acc_t opixel = no_bias ? 0 : bias[och];

          for (int krow = 0; krow < kernel_dim; krow++) {
            const int irow = orow * stride + krow * dilation - padding;

            for (int kcol = 0; kcol < kernel_dim; kcol++) {
              const int icol = ocol * stride + kcol * dilation - padding;

              for (int kch = 0; kch < in_channels; kch++) {
                elem_t ipixel = irow < 0 || irow >= in_dim || icol < 0 || icol >= in_dim ?
                    0 :
                    *(input + (b * in_dim * in_dim + irow * in_dim + icol) * in_channels + kch);

                elem_t weight = *(weights + (krow * kernel_dim * in_channels + kcol * in_channels + kch) * out_channels + och);

                //acc_t past_opixel = opixel;
                opixel += weight * ipixel;
              }
            }
          }

          *(output+(b*out_dim*out_dim+orow*out_dim+ocol)*out_channels + och) =
            scale_and_sat(opixel, act, scale, relu6_shift);
        }
      }
    }
  }
}

static void conv_cpu(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding) {

  const bool no_pool = pool_stride == 0;
  if (no_pool) {
    conv_cpu_without_pool(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, dilation, padding, kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift);
    return;
  }

  const bool no_bias = bias == NULL;
  int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

  for (int b = 0; b < batch_size; b++) {
    for (int porow = 0; porow < pool_out_dim; porow++) {
      for (int pocol = 0; pocol < pool_out_dim; pocol++) {
        for (int poch = 0; poch < out_channels; poch++) {

          elem_t running_max = 0;
          bool running_max_initialized = false;

          for (int pwrow = 0; pwrow < pool_size; pwrow++) {
            const int orow = porow * pool_stride + pwrow - pool_padding;

            for (int pwcol = 0; pwcol < pool_size; pwcol++) {
              const int ocol = pocol * pool_stride + pwcol - pool_padding;

              if (orow < 0 || orow >= out_dim || ocol < 0 || ocol >= out_dim) {
                if (!running_max_initialized || running_max < 0) {
                  running_max = 0;
                  running_max_initialized = true;
                }
              } else {

                acc_t opixel = no_bias ? 0 : bias[poch];

                for (int krow = 0; krow < kernel_dim; krow++) {
                  const int irow = orow * stride + krow * dilation - padding;

                  for (int kcol = 0; kcol < kernel_dim; kcol++) {
                    const int icol = ocol * stride + kcol * dilation - padding;

                    for (int kch = 0; kch < in_channels; kch++) {
                      elem_t ipixel = irow < 0 || irow >= in_dim || icol < 0 || icol >= in_dim ?
                          0 :
                          *(input + (b * in_dim * in_dim + irow * in_dim + icol) * in_channels + kch);

                      elem_t weight = *(weights + (krow * kernel_dim * in_channels + kcol * in_channels + kch) * out_channels + poch);

                      opixel += weight * ipixel;
                    }
                  }
                }

                opixel = scale_and_sat(opixel, act, scale, relu6_shift);
                if (!running_max_initialized || opixel > running_max) {
                  running_max = opixel;
                  running_max_initialized = true;
                }
              }

              if (pwrow == pool_size - 1 && pwcol == pool_size - 1) {
                *(output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol)*out_channels + poch) = running_max;
              }
            }
          }
        }
      }
    }
  }
}

static void tiled_conv_A_stride(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,

        int batches,
        int porows, int pocols, int pochs,
        int krows, int kcols, int kchs,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type, bool skip_weight) {

    if (tiled_conv_type == CPU) {
      if (pool_size == 1 && pool_stride == 1 && pool_padding == 0) {
        pool_stride = 0;
      }

      conv_cpu(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, dilation, padding, kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift,
        pool_size, pool_stride, pool_padding);
      return;
    } else if (tiled_conv_type == OS) {
      printf("Gemmini convs do not currently support OS\n");
      exit(1);
    }

    // TODO move everything below this into a tiled_conv_outer function to match the tiled_matmul function

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

#ifdef GEMMINI_ASSERTIONS
    {
        // const int orows = porows * pool_stride + pool_size - 1;
        // const int ocols = pocols * pool_stride + pool_size - 1;

        // Check that data will fit in scratchpad
        const int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);

        if (spad_rows > BANK_NUM * BANK_ROWS / 2) {
            printf("not enough scratchpad space to store inputs and weights, %d\n", spad_rows);
            exit(1);
        }
        if (acc_rows > ACC_ROWS / 2) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
        if (kernel_dim <= padding) {
            printf("kernel_dim must be larger than padding\n");
            exit(1);
        }
        if (dilation != 1) {
            printf("dilation is only supported on CPU\n");
            exit(1);
        }
    }
#endif

    gemmini_config_st(out_channels * sizeof(elem_t));
    gemmini_extended_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, stride, false, false);

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 //default dram_stride
	 const int out_stride = out_channels;
	 const int in_stride = in_channels;
	 const int weight_stride = out_channels;
    for (int b = 0; b < batch_size; b += batches) {
        for (int porow = 0; porow < pool_out_dim; porow += porows) {
            const int orow = porow * pool_stride - pool_padding;

            for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
                const int ocol = pocol * pool_stride - pool_padding;

                for (int poch = 0; poch < out_channels; poch += pochs) {
                    for (int krow = 0; krow < kernel_dim; krow += krows) {
                        const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride + krow - padding;

                        for (int kcol = 0; kcol < kernel_dim; kcol += kcols) {
                            const int ocol_floored = ocol < 0 ? 0 : ocol;
                            const int icol = ocol_floored * stride + kcol - padding;

                            for (int kch = 0; kch < in_channels; kch += kchs) {
                                elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_channels + poch;

                                if (krow + krows < kernel_dim ||
                                        kcol + kcols < kernel_dim ||
                                        kch + kchs < in_channels) {
                                    out = NULL;
                                }

                                acc_t * bias_ = bias + poch;
                                if (krow > 0 ||
                                        kcol > 0 ||
                                        kch > 0) {
                                    bias_ = NULL;
                                }

                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
                                const int krows_ = kernel_dim - krow > krows ? krows : kernel_dim - krow;
                                const int kcols_ = kernel_dim - kcol > kcols ? kcols : kernel_dim - kcol;
                                const int kchs_ = in_channels - kch > kchs ? kchs : in_channels - kch;

                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

                                const int icols_ = (ocols_ - plpad - prpad) * stride + kcols_ - 1;
                                const int irows_ = (orows_ - pupad - pdpad) * stride + krows_ - 1;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

                                sp_tiled_conv_A_stride(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,
												in_stride, out_stride, weight_stride,

                                    stride, padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
                                    krows_, kcols_, kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels + kch,
                                    weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + poch,
                                    out,
                                    bias_,
												NULL,

                                    no_bias, no_pool, false,
												skip_weight, false);
                            }
                        }
                    }
                }
            }
        }
    }
}
//with cid
static void tiled_conv_A_stride_cid(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,
		  int in_stride, int out_stride, int weight_stride,

        int batches,
        int porows, int pocols, int pochs,
        int krows, int kcols, int kchs,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,
		  elem_t * pool_output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding, bool pool_ceil_dim,

        enum tiled_matmul_type_t tiled_conv_type, 
		  size_t och_divide, size_t orow_divide, size_t cid, bool skip_weight, bool both_out) {

    if (tiled_conv_type == CPU) {
      if (pool_size == 1 && pool_stride == 1 && pool_padding == 0) {
        pool_stride = 0;
      }

      conv_cpu(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, dilation, padding, kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift,
        pool_size, pool_stride, pool_padding);
      return;
    } else if (tiled_conv_type == OS) {
      printf("Gemmini convs do not currently support OS\n");
      exit(1);
    }

    // TODO move everything below this into a tiled_conv_outer function to match the tiled_matmul function

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }
    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

#ifdef GEMMINI_ASSERTIONS
    {
        // const int orows = porows * pool_stride + pool_size - 1;
        // const int ocols = pocols * pool_stride + pool_size - 1;

        // Check that data will fit in scratchpad
        const int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);

        if (spad_rows > BANK_NUM * BANK_ROWS / 2) {
            printf("not enough scratchpad space to store inputs and weights, %d\n", spad_rows);
            exit(1);
        }
        if (acc_rows > ACC_ROWS / 2) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
        if (kernel_dim <= padding) {
            printf("kernel_dim must be larger than padding\n");
            exit(1);
        }
        if (dilation != 1) {
            printf("dilation is only supported on CPU\n");
            exit(1);
        }
    }
#endif


//	 int och_stride = (och_padding) ? out_channels * och_divide + MAX_BLOCK_LEN * DIM : out_channels * och_divide;
//	 int ich_stride = (ich_padding) ? in_channels + MAX_BLOCK_LEN * DIM : in_channels;
//	 int out_stride = (och_concat) ? och_stride * 2 : och_stride; //for now, ignore padding(ToDo)
//	 int weight_stride = out_channels * och_divide;
//	 int in_stride = in_channels;
//	 int out_stride = weight_stride * och_concat;
	 gemmini_config_st(out_stride * sizeof(elem_t));
    gemmini_extended_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, stride, false, false);

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 if (pool_ceil_dim)
      pool_out_dim += (out_dim + 2*pool_padding - pool_size) % pool_stride != 0;


	 int pool_out_row = pool_out_dim / orow_divide;
	 const int porow_start = pool_out_row * cid;
	 const int porow_end = pool_out_row * (cid + 1);
#ifdef GEMMINI_ASSERTIONS
	 if(pool_out_dim % orow_divide != 0){
		 printf("output dimension is not divisible in inner loop \n");
		 exit(1);
	 }
#endif   
	 for (int poch = 0; poch < out_channels; poch += pochs) {
       for (int b = 0; b < batch_size; b += batches) {
           for (int porow = porow_start; porow < porow_end; porow += porows) {
				   //printf("porow_start: %d, porow_end: %d, porow: %d \n", porow_start, porow_end, porow);
				   const int orow = porow * pool_stride - pool_padding;
               for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
                   const int ocol = pocol * pool_stride - pool_padding;
						 bool detect_conflict = true;// (porow == 0 && pocol == 0 && b == 0) ? false : true;
				//printf("detect_conflict: %d \n", detect_conflict);
						 for (int krow = 0; krow < kernel_dim; krow += krows) {
                        const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride + krow - padding;
                        for (int kcol = 0; kcol < kernel_dim; kcol += kcols) {
                            const int ocol_floored = ocol < 0 ? 0 : ocol;
                            const int icol = ocol_floored * stride + kcol - padding;

                            for (int kch = 0; kch < in_channels; kch += kchs) {
                                //elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_stride + poch;
										  elem_t * pool_out = !both_out ? NULL : pool_output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_stride + poch;
                                elem_t * out = both_out ? output + (b*out_dim*out_dim + orow_floored*out_dim + ocol_floored) * out_stride + poch : output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_stride + poch;
					  
                                if (krow + krows < kernel_dim ||
                                        kcol + kcols < kernel_dim ||
                                        kch + kchs < in_channels) {
                                    out = NULL;
												pool_out = NULL;
                                }

                                acc_t * bias_ = bias + poch;
                                if (krow > 0 ||
                                        kcol > 0 ||
                                        kch > 0) {
                                    bias_ = NULL;
                                }
                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = porow_end - porow > porows ? porows : porow_end - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
                                const int krows_ = kernel_dim - krow > krows ? krows : kernel_dim - krow;
                                const int kcols_ = kernel_dim - kcol > kcols ? kcols : kernel_dim - kcol;
                                const int kchs_ = in_channels - kch > kchs ? kchs : in_channels - kch;

                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

                                const int icols_ = (ocols_ - plpad - prpad) * stride + kcols_ - 1;
                                const int irows_ = (orows_ - pupad - pdpad) * stride + krows_ - 1;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

                                sp_tiled_conv_A_stride(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,
												in_stride, out_stride, weight_stride,

                                    stride, padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
                                    krows_, kcols_, kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_stride + kch,
                                    weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * weight_stride + poch,
                                    out,
                                    bias_,
												pool_out,

                                    no_bias, no_pool, both_out,
												skip_weight, detect_conflict);
                            }
                        }
                    }
                }
            }
        }
    }
}

static void tiled_conv_A_stride_loopld(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,
		  int in_stride, int out_stride, int weight_stride,

        int batches,
        int porows, int pocols, int pochs,
        int krows, int kcols, int kchs,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,
		  elem_t * pool_output,
		  
        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding, bool pool_ceil_dim,

        enum tiled_matmul_type_t tiled_conv_type, 
		  size_t och_divide, bool skip_weight, bool both_out) {

    if (tiled_conv_type == CPU) {
      if (pool_size == 1 && pool_stride == 1 && pool_padding == 0) {
        pool_stride = 0;
      }

      conv_cpu(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, dilation, padding, kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift,
        pool_size, pool_stride, pool_padding);
      return;
    } else if (tiled_conv_type == OS) {
      printf("Gemmini convs do not currently support OS\n");
      exit(1);
    }

    // TODO move everything below this into a tiled_conv_outer function to match the tiled_matmul function

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

#ifdef GEMMINI_ASSERTIONS
    {
        // const int orows = porows * pool_stride + pool_size - 1;
        // const int ocols = pocols * pool_stride + pool_size - 1;

        // Check that data will fit in scratchpad
        const int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);

        if (spad_rows > BANK_NUM * BANK_ROWS / 2) {
            printf("not enough scratchpad space to store inputs and weights, %d\n", spad_rows);
            exit(1);
        }
        if (acc_rows > ACC_ROWS / 2) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
        if (kernel_dim <= padding) {
            printf("kernel_dim must be larger than padding\n");
            exit(1);
        }
        if (dilation != 1) {
            printf("dilation is only supported on CPU\n");
            exit(1);
        }
    }
#endif

//	 int och_stride = (och_padding) ? out_channels * och_divide + MAX_BLOCK_LEN * DIM : out_channels * och_divide;
//	 int ich_stride = (ich_padding) ? in_channels + MAX_BLOCK_LEN * DIM : in_channels;
//	 int out_stride = (och_concat) ? och_stride * 2 : och_stride; //for now, let's ignore padding (ToDo)
	 gemmini_config_st(out_stride * sizeof(elem_t));
    gemmini_extended_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, stride, false, false);

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 if (pool_ceil_dim)
      pool_out_dim += (out_dim + 2*pool_padding - pool_size) % pool_stride != 0;


	 for (int poch = 0; poch < out_channels; poch += pochs) {
       for (int b = 0; b < batch_size; b += batches) {
           for (int porow = 0; porow < pool_out_dim; porow += porows) {
               const int orow = porow * pool_stride - pool_padding;
               for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
                   const int ocol = pocol * pool_stride - pool_padding;
						 bool detect_conflict = (porow == 0 && pocol == 0 && b == 0) ? false : true;
				//printf("detect_conflict: %d \n", detect_conflict);
						 for (int krow = 0; krow < kernel_dim; krow += krows) {
                        const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride + krow - padding;
                        for (int kcol = 0; kcol < kernel_dim; kcol += kcols) {
                            const int ocol_floored = ocol < 0 ? 0 : ocol;
                            const int icol = ocol_floored * stride + kcol - padding;

                            for (int kch = 0; kch < in_channels; kch += kchs) {
                                elem_t * out = both_out ? output + (b*out_dim*out_dim + orow_floored*out_dim + ocol_floored) * out_stride + poch : output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_stride + poch;
										  elem_t * pool_out = both_out ? pool_output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_stride + poch : NULL;
                                if (krow + krows < kernel_dim ||
                                        kcol + kcols < kernel_dim ||
                                        kch + kchs < in_channels) {
                                    out = NULL;
												pool_out = NULL;
                                }

                                acc_t * bias_ = bias + poch;
                                if (krow > 0 ||
                                        kcol > 0 ||
                                        kch > 0) {
                                    bias_ = NULL;
                                }

//printf("batch: %d, poch: %d, porow: %d, pocol: %d, krow: %d, kcol: %d, kch: %d \n", b, poch, porow, pocol, krow, kcol, kch);
                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
                                const int krows_ = kernel_dim - krow > krows ? krows : kernel_dim - krow;
                                const int kcols_ = kernel_dim - kcol > kcols ? kcols : kernel_dim - kcol;
                                const int kchs_ = in_channels - kch > kchs ? kchs : in_channels - kch;

                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

                                const int icols_ = (ocols_ - plpad - prpad) * stride + kcols_ - 1;
                                const int irows_ = (orows_ - pupad - pdpad) * stride + krows_ - 1;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

                                sp_tiled_conv_A_stride(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,
												in_stride, out_stride, weight_stride,

                                    stride, padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
                                    krows_, kcols_, kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_stride + kch,
                                    weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * weight_stride + poch,
                                    out,
                                    bias_,
												pool_out,

                                    no_bias, no_pool, both_out,
												skip_weight, detect_conflict);
                            }
                        }
                    }
                }
            }
        }
    }
}

static void tiled_conv_A_stride_dw(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,
	     int in_stride, int out_stride, int weight_stride,

        int batches,
        int porows, int pocols, int pochs,
        int krows, int kcols, int kchs,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type, 
		  size_t och_divide, size_t orow_divide, size_t cid, bool skip_weight) {

	// TODO move everything below this into a tiled_conv_outer function to match the tiled_matmul function

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

#ifdef GEMMINI_ASSERTIONS
    {
        // const int orows = porows * pool_stride + pool_size - 1;
        // const int ocols = pocols * pool_stride + pool_size - 1;

        // Check that data will fit in scratchpad
        const int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, batches, porows, pocols, pochs, krows, kcols, kchs, pool_size, pool_stride);

        if (spad_rows > BANK_NUM * BANK_ROWS / 2) {
            printf("not enough scratchpad space to store inputs and weights, %d\n", spad_rows);
            exit(1);
        }
        if (acc_rows > ACC_ROWS / 2) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
        if (kernel_dim <= padding) {
            printf("kernel_dim must be larger than padding\n");
            exit(1);
        }
        if (dilation != 1) {
            printf("dilation is only supported on CPU\n");
            exit(1);
        }
    }
#endif

//	 int och_stride = (och_padding) ? out_channels * och_divide + MAX_BLOCK_LEN * DIM : out_channels * och_divide;
//	 int ich_stride = (ich_padding) ? in_channels * och_divide + MAX_BLOCK_LEN * DIM : in_channels * och_divide;
	 int och_stride = out_stride;//out_channels * och_divide;
	 int ich_stride = in_stride;//in_channels * och_divide;
	 gemmini_config_st(och_stride * sizeof(elem_t));
    gemmini_extended_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, stride, false, false);

	 int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 int pool_out_row = pool_out_dim / orow_divide;
	 const int porow_start = (orow_divide == 1) ? 0 : pool_out_row * cid;
	 const int porow_end = (orow_divide == 1) ? pool_out_dim : pool_out_row * (cid + 1);
#ifdef GEMMINI_ASSERTIONS
	 if(orow_divide > 1 && pool_out_dim % orow_divide != 0){
		 printf("out dim cannot be divided by orow_divide \n");
		 exit(1);
	 }
#endif
	 
	 for (int b = 0; b < batch_size; b += batches) {
       for (int porow = porow_start; porow < porow_end; porow += porows) {
           const int orow = porow * pool_stride - pool_padding;
           for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
               const int ocol = pocol * pool_stride - pool_padding;
			   	bool detect_conflict = (porow == 0 && pocol == 0 && b == 0) ? false : true;
				//printf("detect_conflict: %d \n", detect_conflict);
					for (int poch = 0; poch < out_channels; poch += pochs) {
						 for (int krow = 0; krow < kernel_dim; krow += krows) {
                        const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride + krow - padding;
                        for (int kcol = 0; kcol < kernel_dim; kcol += kcols) {
                            const int ocol_floored = ocol < 0 ? 0 : ocol;
                            const int icol = ocol_floored * stride + kcol - padding;
                               elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * och_stride + poch;

                                if (krow + krows < kernel_dim ||
                                        kcol + kcols < kernel_dim) {
                                    out = NULL;
                                }
                                acc_t * bias_ = bias + poch;
                                if (krow > 0 ||
                                        kcol > 0) {
                                    bias_ = NULL;
                                }

                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = porow_end - porow > porows ? porows : porow_end - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
                                const int krows_ = kernel_dim - krow > krows ? krows : kernel_dim - krow;
                                const int kcols_ = kernel_dim - kcol > kcols ? kcols : kernel_dim - kcol;
                        
										  const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

                                const int icols_ = (ocols_ - plpad - prpad) * stride + kcols_ - 1;
                                const int irows_ = (orows_ - pupad - pdpad) * stride + krows_ - 1;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

                                sp_tiled_conv_A_stride(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,
												ich_stride, och_stride, och_stride,

                                    stride, padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
                                    krows_, kcols_, 1, //hardcode kchs as 1

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * ich_stride + poch, //instead of och
                                    weights + poch,// + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * och_stride + poch,
                                    out,
                                    bias_,
												NULL,

                                    no_bias, no_pool, false,
												skip_weight, detect_conflict); 
                        }
                    }
                }
            }
        }
    }
}
static void tiled_conv_A_stride_auto(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int dilation, int padding, int kernel_dim,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type) {

    const bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    // Tile convolution params

    // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
    int args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};
    const int max_args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};

    const int orows_idx = 1;
    const int ocols_idx = 2;
    const int out_channels_idx = 3;
    const int in_channels_idx = 6;

    // We divide by 2 for the sake of double-buffering
    const int max_spad_rows = (BANK_NUM*BANK_ROWS / 2);
    const int max_acc_rows = (ACC_ROWS / 2);

    int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

    while (spad_rows > max_spad_rows || acc_rows > max_acc_rows) {
        int max_val = -1;
        int max_idx = -1;

        for (size_t i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            // We avoid reducing ocols when possible to keep the spatial array fully utilized
            if (!(i == ocols_idx && args[i] <= DIM && args[orows_idx] > 1)
                    && args[i] > max_val) {
                max_val = args[i];
                max_idx = i;
            }
        }

        if (max_idx == out_channels_idx || max_idx == in_channels_idx) {
            // For input and output channels, there's no point in subtracting by just one
            if (args[max_idx] % DIM != 0) {
                args[max_idx] = (args[max_idx] / DIM) * DIM;
            } else {
                args[max_idx] -= DIM;
            }
            args[max_idx] = args[max_idx] == 0 ? 1 : args[max_idx];
        } else if(max_idx == ocols_idx){
			  if(args[max_idx] % DIM != 0)
				  args[max_idx] = (args[max_idx] / DIM) * DIM;
			  else args[max_idx] -= DIM;
		  }
		  else {
            args[max_idx]--;
        }

        spad_rows = tiled_conv_total_spad_rows_A_stride(false,
            stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
        acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    }

    // Check if we can increase ocols
    bool not_increased = false;
    while (!not_increased) {
        not_increased = true;

        int args_candidate[] = {args[0], args[1], args[2], args[3], args[4], args[5], args[6]};
        args_candidate[ocols_idx]+=DIM;

        if (args_candidate[ocols_idx] > max_args[ocols_idx])
            continue;

        spad_rows = tiled_conv_total_spad_rows_A_stride(false,
            stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);
        acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);

        if (spad_rows <= max_spad_rows && acc_rows <= max_acc_rows) {
            args[ocols_idx] = args_candidate[ocols_idx];
            not_increased = false;
        }
    }

    // Check if there are any parameters that we can currently still increase
    bool nothing_increased = false;
    while (!nothing_increased) {
        nothing_increased = true;

        for (size_t i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            int args_candidate[] = {args[0], args[1], args[2], args[3], args[4], args[5], args[6]};
            if(i == ocols_idx) args_candidate[i] += DIM;
				else args_candidate[i]++;

            if (args_candidate[i] > max_args[i])
                continue;

            spad_rows = tiled_conv_total_spad_rows_A_stride(false,
                stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);
            acc_rows = tiled_conv_total_spad_rows_A_stride(true,
                stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);

            if (spad_rows <= max_spad_rows && acc_rows <= max_acc_rows) {
                args[i] = args_candidate[i];
                nothing_increased = false;
            }
        }
    }

    const int batches = args[0];
    const int orows = args[1];
    const int ocols = args[2];
    const int ochs = args[3];
    const int krows = args[4];
    const int kcols = args[5];
    const int kchs = args[6];

    /*
    spad_rows = tiled_conv_total_spad_rows_A_stride(false,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    acc_rows = tiled_conv_total_spad_rows_A_stride(true,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

	 
    printf("batches = %d\n", batches);
    printf("orows   = %d\n", orows);
    printf("ocols   = %d\n", ocols);
    printf("ochs    = %d\n", ochs);
    printf("krows   = %d\n", krows);
    printf("kcols   = %d\n", kcols);
    printf("kchs    = %d\n\n", kchs);

    printf("total spad_rows reserved: %d\n", spad_rows);
    printf("total acc_rows reserved: %d\n\n", acc_rows);

    printf("scratchpad row utilization: %d%%\n", (spad_rows*100) / max_spad_rows);
    printf("accumulator row utilization: %d%%\n\n", (acc_rows*100) / max_acc_rows);

    printf("inner matmul size: i=%d, j=%d, k=%d\n\n", ocols, ochs, kchs);
  */

    tiled_conv_A_stride(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, dilation, padding, kernel_dim,

        batches,
        orows, ocols, ochs,
        krows, kcols, kchs,

        input,
        weights,
        bias,
        output,

        act, scale, relu6_shift,
        pool_size, no_pool ? 0 : pool_stride, pool_padding,

        tiled_conv_type, false);
}
// division by row dimension
static void tiled_conv_A_stride_auto_cid(
	  int batch_size, int in_dim, int in_channels,
	  int out_channels, int out_dim,
	  int stride, int dilation, int padding, int kernel_dim,
	  int out_stride, //for concatenation

	  elem_t * input,
	  elem_t * weights,
	  acc_t * bias,
	  elem_t * output,

	  int act, acc_scale_t scale, size_t relu6_shift,
	  int pool_size, int pool_stride, int pool_padding, bool pool_ceil_dim,

	  enum tiled_matmul_type_t tiled_conv_type, 
	  size_t orow_divide, size_t batch_divide, size_t cid, bool skip_weight) {

	 const bool no_pool = pool_stride == 0;
	 if (no_pool) {
		  pool_size = 1;
		  pool_stride = 1;
		  pool_padding = 0;
	 }
#ifdef GEMMINI_ASSERTIONS
	 if(batch_size == 1 && batch_divide > 1){
		 printf("batch_divide set wrong \n");
		 exit(1);
	 }
	 if(orow_divide > 1 && batch_divide > 1){
		 printf("Allowed to divide in single dimension only \n");
		 exit(1);
	 }
#endif
//	 int out_stride = out_channels;
	 int in_stride = in_channels;
	 int weight_stride = out_channels;
	
	 int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 if (pool_ceil_dim)
      pool_out_dim += (out_dim + 2*pool_padding - pool_size) % pool_stride != 0;
	 
	 // divide in batch dimension
	 batch_size = batch_size / batch_divide;
	 int batch_in_offset = (batch_divide > 1) ? batch_size*in_dim*in_dim*in_stride*cid : 0;
	 int batch_out_offset = (batch_divide > 1) ? batch_size*pool_out_dim*pool_out_dim*out_stride*cid : 0; // not dividing in out_channel dimension
/*
	 if(batch_divide > 1){
	    batch_in_offset = (ich_padding && (in_channels % 128 == 0)) ? batch_size * in_dim * in_dim * (in_channels + 64) : batch_size*in_dim*in_dim*in_channels;	
	    batch_out_offset = (och_padding && (out_channels % 128 == 0)) ? pool_out_dim * pool_out_dim * (out_channels + 64) * batch_size : batch_size * pool_out_dim * pool_out_dim * out_channels;	
	 } //only need batch level offset when we divide batch dimension
*/
	 // divide in row dimension (single batch)
	 bool row_divisible = (orow_divide > 1) && (pool_out_dim % orow_divide == 0);
	 int pool_out_row = (row_divisible) ? (pool_out_dim / orow_divide) : pool_out_dim;
	 const size_t och_divide = (row_divisible) ? 1 : orow_divide; //if row isn't divisible, divide channel instead
  	 out_channels = out_channels / och_divide;
  	 const int out_offset = (och_divide > 1) ? out_channels * cid : 0;
	 //printf("batch_in_offset: %d, batch_out_offset: %d, out_offset: %d, pool_out_row: %d \n", batch_in_offset, batch_out_offset, out_offset, pool_out_row);
  	 // Tile convolution params

	 // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
	 int args[] = {batch_size, pool_out_row, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};
	 const int max_args[] = {batch_size, pool_out_row, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};
/*
    printf("batches = %d\n", args[0]);
    printf("orows   = %d\n", args[1]);
    printf("ocols   = %d\n", args[2]);
    printf("ochs    = %d\n", args[3]);
    printf("krows   = %d\n", args[4]);
    printf("kcols   = %d\n", args[5]);
    printf("kchs    = %d\n\n", args[6]);
*/

	 const int orows_idx = 1;
	 const int ocols_idx = 2;
	 const int out_channels_idx = 3;
	 const int in_channels_idx = 6;

	 // We divide by 2 for the sake of double-buffering
	 const int max_spad_rows = (BANK_NUM*BANK_ROWS / 2);
	 const int max_acc_rows = (ACC_ROWS / 2);

	 int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
		  stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
	 int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
		  stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

	 bool down_sample = (in_dim > out_dim);
	 while (spad_rows > max_spad_rows || acc_rows > max_acc_rows) {
		  int max_val = -1;
		  int max_idx = -1;

		  for (size_t j = 0; j < sizeof(args)/sizeof(args[0]); j++) {
				// We avoid reducing ocols when possible to keep the spatial array fully utilized
			  size_t i = 0;
	//		  if(!down_sample){
			  if(j == 0) i = 0;
			  else if (j == 4) i = orows_idx;
			  else if(j == 1) i = ocols_idx;
				else if (j == 2) i = 4;
				else if(j == 3) i = 5;
				else if(j == 5) i = down_sample ? in_channels_idx : out_channels_idx;
				else if(j == 6) i = down_sample ? out_channels_idx : in_channels_idx;
		
				  if(i == 0 && args[0] > 1){ // batch first
						max_val = args[0];
						max_idx = 0;
						break;
					}else if(((pool_stride > 1 && args[in_channels_idx] >= DIM) || args[in_channels_idx] == MAX_BLOCK_LEN * DIM) && args[out_channels_idx] <= MAX_BLOCK_LEN * DIM){
					//}else if(args[in_channels_idx] >= DIM && args[out_channels_idx] <= MAX_BLOCK_LEN * DIM){	
						if(i == orows_idx && args[orows_idx] > 1 && (args[ocols_idx] <= DIM || (args[in_channels_idx] <= DIM * MAX_BLOCK_LEN && args[out_channels_idx] == MAX_BLOCK_LEN*DIM))){// && (args[orows_idx] >= args[ocols_idx] || args[ocols_idx] <= DIM)){ //decrease orows as much as possible 
							max_val = args[orows_idx];
							max_idx = orows_idx;
							break;
						}else if(i == ocols_idx && (args[i]) > DIM){
							max_val = args[ocols_idx];
							max_idx = ocols_idx;
							break;
						}else if((i==4 || i == 5) && args[i] > 1){
							max_val = args[i];
							max_idx = i;
						  break;
						}else if(args[i] > DIM && pool_stride > 1 && (i == in_channels_idx || i == out_channels_idx)){
							max_val = args[i];
							max_idx = i;
						}
					}else if (!(i == ocols_idx && args[i] <= DIM && args[orows_idx] > 1)
							  && args[i] > max_val) { // and then move on to channels
						 if(!((i==out_channels_idx || i==in_channels_idx) && args[i] <= DIM)){
						    max_val = args[i];
						    max_idx = i;
						 }
					}
				}

		  if (max_idx == out_channels_idx || max_idx == in_channels_idx) {
				if(max_val > MAX_BLOCK_LEN * DIM || pool_stride > 1){
			   // For input and output channels, there's no point in subtracting by just one
					if (args[max_idx] > MAX_BLOCK_LEN*DIM && args[max_idx] % (MAX_BLOCK_LEN * DIM) != 0) {
						 args[max_idx] = (args[max_idx] / (MAX_BLOCK_LEN * DIM)) * (MAX_BLOCK_LEN * DIM);
					} else {
						 //args[max_idx] -= (MAX_BLOCK_LEN * DIM);
						if(args[max_idx] % (2*DIM) == 0) args[max_idx] = args[max_idx] / 2;
						else args[max_idx] = ((args[max_idx]-1) / DIM) * DIM;
					}
					args[max_idx] = args[max_idx] == 0 ? 1 : args[max_idx];
				}
				else if (args[4] > 1 || args[5] > 1){
//					if(down_sample) args[max_idx] -= DIM;
//					else {
					if(args[4] > 1) args[4] = 1;//args[4]--;
					else if(args[5] > 1) args[5]--;
//					}
				}
				else if(args[in_channels_idx] < DIM){//for first layer
					args[max_idx] = args[max_idx] / 2;
				}
				else if (args[orows_idx] > 4){
					args[orows_idx] = args[orows_idx] / 2;
				}
				else if(args[ocols_idx] > DIM){
					args[ocols_idx] = DIM;
				}

		  } else {
			  if(max_idx == ocols_idx){
				  if(args[max_idx] % DIM != 0) args[max_idx] = (args[max_idx]/DIM)*DIM;
				  else args[max_idx] -= (DIM/pool_stride);
			  }else{
				  if(max_idx == 4 || max_idx == 5) args[max_idx] = 1;
				  else args[max_idx]--;
			  }
		  }

			  //printf("max_val: %d, max_idx: %d \n", max_val, max_idx);
	
		  spad_rows = tiled_conv_total_spad_rows_A_stride(false,
				stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
		  acc_rows = tiled_conv_total_spad_rows_A_stride(true,
				stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
	 }
/*
    printf("batches = %d\n", args[0]);
    printf("orows   = %d\n", args[1]);
    printf("ocols   = %d\n", args[2]);
    printf("ochs    = %d\n", args[3]);
    printf("krows   = %d\n", args[4]);
    printf("kcols   = %d\n", args[5]);
    printf("kchs    = %d\n\n", args[6]);
*/

	 // Check if we can increase ocols
	 bool not_increased = false;

	 // Check if there are any parameters that we can currently still increase
	 bool nothing_increased = false;
	 bool kdim_increase = true;
	 while (!nothing_increased) {
		  nothing_increased = true;
		  //kdim_increase = true;

		  for (size_t j = 0; j < sizeof(args)/sizeof(args[0]); j++) {
			   //size_t i =j;//  down_sample ? j : 6-j;
				size_t i = j;
			   if(!down_sample){
					if(j == 0) i = 5;//in_channels_idx;
					else if (j == 1) i = in_channels_idx;
					else if(j == 2) i = 4;//in_channels_idx;
					else if (j == 3) i = out_channels_idx;
					else if(j == 4) i = ocols_idx;
					else if(j == 5) i = orows_idx;
					else if(j == 6) i = 0;
				}
				else{
					if(j == 0) i = out_channels_idx;
					else if (j == 1) i = orows_idx;//5;
					else if (j == 2) i = ocols_idx;//4;
					else if (j == 3) i = in_channels_idx;
					else if(j == 4) i = 5;//orows_idx;//ocols_idx;
					else if(j == 5) i = 4;//ocols_idx;//orows_idx;
					else if(j == 6) i = 0;
				}
				int args_candidate[] = {args[0], args[1], args[2], args[3], args[4], args[5], args[6]};
				if(i == out_channels_idx || i == in_channels_idx) args_candidate[i] *= 2;//+= MAX_BLOCK_LEN * DIM;//!down_sample ? MAX_BLOCK_LEN * DIM : DIM;
				//else if(i == in_channels_idx) args_candidate[i] += MAX_BLOCK_LEN * DIM;
				else if(i == ocols_idx && (args[i] % DIM == 0)) args_candidate[i] += DIM;
				else args_candidate[i]+= kdim_increase && (i == 4 || i == 5) ? 2 : 1;
				if (args_candidate[i] > max_args[i])
					 continue;

				spad_rows = tiled_conv_total_spad_rows_A_stride(false,
					 stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);
				acc_rows = tiled_conv_total_spad_rows_A_stride(true,
					 stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);

				if (spad_rows <= max_spad_rows && acc_rows <= max_acc_rows) {
					 args[i] = args_candidate[i];
					 nothing_increased = false;
					 kdim_increase = false;
				}
		  }
	 }

	 const int batches = args[0];
	 const int orows = args[1];
	 const int ocols = args[2];
	 const int ochs = args[3];
	 const int krows = args[4];
	 const int kcols = args[5];
	 const int kchs = args[6];

/*
    spad_rows = tiled_conv_total_spad_rows_A_stride(false,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    acc_rows = tiled_conv_total_spad_rows_A_stride(true,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

    printf("batches = %d\n", batches);
    printf("orows   = %d\n", orows);
    printf("ocols   = %d\n", ocols);
    printf("ochs    = %d\n", ochs);
    printf("krows   = %d\n", krows);
    printf("kcols   = %d\n", kcols);
    printf("kchs    = %d\n\n", kchs);

    printf("total spad_rows reserved: %d\n", spad_rows);
    printf("total acc_rows reserved: %d\n\n", acc_rows);

    printf("scratchpad row utilization: %d%%\n", (spad_rows*100) / max_spad_rows);
    printf("accumulator row utilization: %d%%\n\n", (acc_rows*100) / max_acc_rows);

    printf("inner matmul size: i=%d, j=%d, k=%d\n\n", ocols, ochs, kchs);
  */
  if(row_divisible){
	 tiled_conv_A_stride_cid(
		  batch_size, in_dim, in_channels,
		  out_channels, out_dim,
		  stride, dilation, padding, kernel_dim,
		  in_stride, out_stride, weight_stride,

		  batches,
		  orows, ocols, ochs,
		  krows, kcols, kchs,

		  input + batch_in_offset,
		  weights,
		  bias,
		  output + batch_out_offset,
		  NULL,

		  act, scale, relu6_shift,
		  pool_size, no_pool ? 0 : pool_stride, pool_padding, pool_ceil_dim,

		  tiled_conv_type, och_divide, orow_divide, cid, skip_weight, false);
  }else{
	  bool no_bias = (bias == NULL);
	 tiled_conv_A_stride_loopld(
		  batch_size, in_dim, in_channels,
		  out_channels, out_dim,
		  stride, dilation, padding, kernel_dim,
		  in_stride, out_stride, weight_stride,

		  batches,
		  orows, ocols, ochs,
		  krows, kcols, kchs,

		  input + batch_in_offset,
		  weights + out_offset,
		  no_bias ? NULL : bias + out_offset,
		  output + out_offset + batch_out_offset,
		  NULL,

		  act, scale, relu6_shift,
		  pool_size, no_pool ? 0 : pool_stride, pool_padding, pool_ceil_dim,

		  tiled_conv_type, och_divide, skip_weight, false);
  }
}

// with double output
static void tiled_conv_A_stride_auto_double_out(
	  int batch_size, int in_dim, int in_channels,
	  int out_channels, int out_dim,
	  int stride, int dilation, int padding, int kernel_dim,
	  int out_stride, //for concatenation

	  elem_t * input,
	  elem_t * weights,
	  acc_t * bias,
	  elem_t * output,
	  elem_t * pool_output,

	  int act, acc_scale_t scale, size_t relu6_shift,
	  int pool_size, int pool_stride, int pool_padding, bool pool_ceil_dim, 

	  enum tiled_matmul_type_t tiled_conv_type, 
	  size_t orow_divide, size_t batch_divide, size_t cid, bool skip_weight) {
	 const bool both_out = true;
	 const bool no_pool = pool_stride == 0;
	 if (no_pool) {
		  pool_size = 1;
		  pool_stride = 1;
		  pool_padding = 0;
	 }

	//const bool no_pool = false;
#ifdef GEMMINI_ASSERTIONS
	 if(batch_size == 1 && batch_divide > 1){
		 printf("batch_divide set wrong \n");
		 exit(1);
	 }
	 if(orow_divide > 1 && batch_divide > 1){
		 printf("Allowed to divide in single dimension only \n");
		 exit(1);
	 }
	 if(no_pool && both_out){
		 printf("have to pool when having both output \n");
		 exit(1);
	 }
#endif
//	 int out_stride = out_channels;
	 int in_stride = in_channels;
	 int weight_stride = out_channels;
	
	 int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 if (pool_ceil_dim)
      pool_out_dim += (out_dim + 2*pool_padding - pool_size) % pool_stride != 0;

	 // divide in batch dimension
	 batch_size = batch_size / batch_divide;
	 int batch_in_offset = (batch_divide > 1) ? batch_size*in_dim*in_dim*in_stride*cid : 0;
	 int batch_out_offset = (batch_divide > 1) ? batch_size*pool_out_dim*pool_out_dim*out_stride*cid : 0; // not dividing in out_channel dimension
/*
	 if(batch_divide > 1){
	    batch_in_offset = (ich_padding && (in_channels % 128 == 0)) ? batch_size * in_dim * in_dim * (in_channels + 64) : batch_size*in_dim*in_dim*in_channels;	
	    batch_out_offset = (och_padding && (out_channels % 128 == 0)) ? pool_out_dim * pool_out_dim * (out_channels + 64) * batch_size : batch_size * pool_out_dim * pool_out_dim * out_channels;	
	 } //only need batch level offset when we divide batch dimension
*/
	 // divide in row dimension (single batch)
	 bool row_divisible = (orow_divide > 1) && (pool_out_dim % orow_divide == 0);
	 int pool_out_row = (row_divisible) ? (pool_out_dim / orow_divide) : pool_out_dim;
	 const size_t och_divide = (row_divisible) ? 1 : orow_divide; //if row isn't divisible, divide channel instead
  	 out_channels = out_channels / och_divide;
  	 const int out_offset = (och_divide > 1) ? out_channels * cid : 0;
	 //printf("batch_in_offset: %d, batch_out_offset: %d, out_offset: %d, pool_out_row: %d \n", batch_in_offset, batch_out_offset, out_offset, pool_out_row);
  	 // Tile convolution params

	 // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
	 int args[] = {batch_size, pool_out_row, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};
	 const int max_args[] = {batch_size, pool_out_row, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};
/*
    printf("batches = %d\n", args[0]);
    printf("orows   = %d\n", args[1]);
    printf("ocols   = %d\n", args[2]);
    printf("ochs    = %d\n", args[3]);
    printf("krows   = %d\n", args[4]);
    printf("kcols   = %d\n", args[5]);
    printf("kchs    = %d\n\n", args[6]);
*/

	 const int orows_idx = 1;
	 const int ocols_idx = 2;
	 const int out_channels_idx = 3;
	 const int in_channels_idx = 6;

	 // We divide by 2 for the sake of double-buffering
	 const int max_spad_rows = (BANK_NUM*BANK_ROWS / 2);
	 const int max_acc_rows = (ACC_ROWS / 2);

	 int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
		  stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
	 int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
		  stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

	 bool down_sample = (in_dim > out_dim);
	 while (spad_rows > max_spad_rows || acc_rows > max_acc_rows) {
		  int max_val = -1;
		  int max_idx = -1;

		  for (size_t j = 0; j < sizeof(args)/sizeof(args[0]); j++) {
				// We avoid reducing ocols when possible to keep the spatial array fully utilized
			  size_t i = 0;
	//		  if(!down_sample){
			  if(j == 0) i = 0;
			  else if (j == 4) i = orows_idx;
			  else if(j == 1) i = ocols_idx;
				else if (j == 2) i = 4;
				else if(j == 3) i = 5;
				else if(j == 5) i = down_sample ? in_channels_idx : out_channels_idx;
				else if(j == 6) i = down_sample ? out_channels_idx : in_channels_idx;
		
				  if(i == 0 && args[0] > 1){ // batch first
						max_val = args[0];
						max_idx = 0;
						break;
					}else if(((pool_stride > 1 && args[in_channels_idx] >= DIM) || args[in_channels_idx] == MAX_BLOCK_LEN * DIM) && args[out_channels_idx] <= MAX_BLOCK_LEN * DIM){
					//}else if(args[in_channels_idx] >= DIM && args[out_channels_idx] <= MAX_BLOCK_LEN * DIM){	
						if(i == orows_idx && args[orows_idx] > 1 && (args[ocols_idx] <= DIM || (args[in_channels_idx] <= DIM * MAX_BLOCK_LEN && args[out_channels_idx] == MAX_BLOCK_LEN*DIM))){// && (args[orows_idx] >= args[ocols_idx] || args[ocols_idx] <= DIM)){ //decrease orows as much as possible 
							max_val = args[orows_idx];
							max_idx = orows_idx;
							break;
						}else if(i == ocols_idx && (args[i]) > DIM){
							max_val = args[ocols_idx];
							max_idx = ocols_idx;
							break;
						}else if((i==4 || i == 5) && args[i] > 1){
							max_val = args[i];
							max_idx = i;
						  break;
						}else if(args[i] > DIM && pool_stride > 1 && (i == in_channels_idx || i == out_channels_idx)){
							max_val = args[i];
							max_idx = i;
						}
					}else if (!(i == ocols_idx && args[i] <= DIM && args[orows_idx] > 1)
							  && args[i] > max_val) { // and then move on to channels
						 if(!((i==out_channels_idx || i==in_channels_idx) && args[i] <= DIM)){
						    max_val = args[i];
						    max_idx = i;
						 }
					}
				}

		  if (max_idx == out_channels_idx || max_idx == in_channels_idx) {
				if(max_val > MAX_BLOCK_LEN * DIM || pool_size > 1){
			   // For input and output channels, there's no point in subtracting by just one
					if (args[max_idx] > MAX_BLOCK_LEN*DIM && args[max_idx] % (MAX_BLOCK_LEN * DIM) != 0) {
						 args[max_idx] = (args[max_idx] / (MAX_BLOCK_LEN * DIM)) * (MAX_BLOCK_LEN * DIM);
					} else {
						 //args[max_idx] -= (MAX_BLOCK_LEN * DIM);
						if(args[max_idx] % (2*DIM) == 0) args[max_idx] = args[max_idx] / 2;
						else args[max_idx] = ((args[max_idx]-1) / DIM) * DIM;
					}
					args[max_idx] = args[max_idx] == 0 ? 1 : args[max_idx];
				}
				else if (args[4] > 1 || args[5] > 1){
//					if(down_sample) args[max_idx] -= DIM;
//					else {
					if(args[4] > 1) args[4] = 1;//args[4]--;
					else if(args[5] > 1) args[5]--;
//					}
				}
				else if(args[in_channels_idx] < DIM){//for first layer
					args[max_idx] = args[max_idx] / 2;
				}
				else if (args[orows_idx] > 4){
					args[orows_idx] = args[orows_idx] / 2;
				}
				else if(args[ocols_idx] > DIM){
					args[ocols_idx] = DIM;
				}
				else if(max_val == MAX_BLOCK_LEN * DIM){
					args[max_idx] /= 2;
				}

		  } else {
			  if(max_idx == ocols_idx){
				  if(args[max_idx] % DIM != 0) args[max_idx] = (args[max_idx]/DIM)*DIM;
				  else args[max_idx] -= (DIM/pool_stride);
			  }else{
				  if(max_idx == 4 || max_idx == 5) args[max_idx] = 1;
				  else args[max_idx]--;
			  }
		  }

//			  printf("max_val: %d, max_idx: %d \n", max_val, max_idx);
	
		  spad_rows = tiled_conv_total_spad_rows_A_stride(false,
				stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
		  acc_rows = tiled_conv_total_spad_rows_A_stride(true,
				stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
	 }
/*
    printf("batches = %d\n", args[0]);
    printf("orows   = %d\n", args[1]);
    printf("ocols   = %d\n", args[2]);
    printf("ochs    = %d\n", args[3]);
    printf("krows   = %d\n", args[4]);
    printf("kcols   = %d\n", args[5]);
    printf("kchs    = %d\n\n", args[6]);
*/

	 // Check if we can increase ocols
	 bool not_increased = false;

	 // Check if there are any parameters that we can currently still increase
	 bool nothing_increased = false;
	 bool kdim_increase = true;
	 while (!nothing_increased) {
		  nothing_increased = true;
		  //kdim_increase = true;

		  for (size_t j = 0; j < sizeof(args)/sizeof(args[0]); j++) {
			   //size_t i =j;//  down_sample ? j : 6-j;
				size_t i = j;
			   if(!down_sample){
					if(j == 0) i = 5;//in_channels_idx;
					else if (j == 1) i = in_channels_idx;
					else if(j == 2) i = 4;//in_channels_idx;
					else if (j == 3) i = out_channels_idx;
					else if(j == 4) i = ocols_idx;
					else if(j == 5) i = orows_idx;
					else if(j == 6) i = 0;
				}
				else{
					if(j == 0) i = out_channels_idx;
					else if (j == 1) i = orows_idx;//5;
					else if (j == 2) i = ocols_idx;//4;
					else if (j == 3) i = in_channels_idx;
					else if(j == 4) i = 5;//orows_idx;//ocols_idx;
					else if(j == 5) i = 4;//ocols_idx;//orows_idx;
					else if(j == 6) i = 0;
				}
				int args_candidate[] = {args[0], args[1], args[2], args[3], args[4], args[5], args[6]};
				if(i == out_channels_idx || i == in_channels_idx) args_candidate[i] *= 2;//+= MAX_BLOCK_LEN * DIM;//!down_sample ? MAX_BLOCK_LEN * DIM : DIM;
				//else if(i == in_channels_idx) args_candidate[i] += MAX_BLOCK_LEN * DIM;
				else if(i == ocols_idx && (args[i] % DIM == 0)) args_candidate[i] += DIM;
				else args_candidate[i]+= kdim_increase && (i == 4 || i == 5) ? 2 : 1;
				if (args_candidate[i] > max_args[i])
					 continue;

				spad_rows = tiled_conv_total_spad_rows_A_stride(false,
					 stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);
				acc_rows = tiled_conv_total_spad_rows_A_stride(true,
					 stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);

				if (spad_rows <= max_spad_rows && acc_rows <= max_acc_rows) {
					 args[i] = args_candidate[i];
					 nothing_increased = false;
					 kdim_increase = false;
				}
		  }
	 }

	 const int batches = args[0];
	 const int orows = args[1];
	 const int ocols = args[2];
	 const int ochs = args[3];
	 const int krows = args[4];
	 const int kcols = args[5];
	 const int kchs = args[6];

/*
    spad_rows = tiled_conv_total_spad_rows_A_stride(false,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    acc_rows = tiled_conv_total_spad_rows_A_stride(true,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

    printf("batches = %d\n", batches);
    printf("orows   = %d\n", orows);
    printf("ocols   = %d\n", ocols);
    printf("ochs    = %d\n", ochs);
    printf("krows   = %d\n", krows);
    printf("kcols   = %d\n", kcols);
    printf("kchs    = %d\n\n", kchs);

    printf("total spad_rows reserved: %d\n", spad_rows);
    printf("total acc_rows reserved: %d\n\n", acc_rows);

    printf("scratchpad row utilization: %d%%\n", (spad_rows*100) / max_spad_rows);
    printf("accumulator row utilization: %d%%\n\n", (acc_rows*100) / max_acc_rows);

    printf("inner matmul size: i=%d, j=%d, k=%d\n\n", ocols, ochs, kchs);
  */
  if(row_divisible){
	 tiled_conv_A_stride_cid(
		  batch_size, in_dim, in_channels,
		  out_channels, out_dim,
		  stride, dilation, padding, kernel_dim,
		  in_stride, out_stride, weight_stride,

		  batches,
		  orows, ocols, ochs,
		  krows, kcols, kchs,

		  input + batch_in_offset,
		  weights,
		  bias,
		  output + batch_out_offset,
		  both_out ? pool_output + batch_out_offset : NULL,

		  act, scale, relu6_shift,
		  pool_size, no_pool ? 0 : pool_stride, pool_padding, pool_ceil_dim,

		  tiled_conv_type, och_divide, orow_divide, cid, skip_weight, both_out);
  }else{
	  bool no_bias = (bias == NULL);
	 tiled_conv_A_stride_loopld(
		  batch_size, in_dim, in_channels,
		  out_channels, out_dim,
		  stride, dilation, padding, kernel_dim,
		  in_stride, out_stride, weight_stride,

		  batches,
		  orows, ocols, ochs,
		  krows, kcols, kchs,

		  input + batch_in_offset,
		  weights + out_offset,
		  no_bias ? NULL : bias + out_offset,
		  output + out_offset + batch_out_offset,
		  both_out ? pool_output + out_offset + batch_out_offset : NULL,

		  act, scale, relu6_shift,
		  pool_size, no_pool ? 0 : pool_stride, pool_padding, pool_ceil_dim,

		  tiled_conv_type, och_divide, skip_weight, both_out);
  }
}

static void tiled_conv_A_stride_dw_auto_cid(
		  int batch_size, int in_dim, int in_channels,
		  int out_channels, int out_dim,
		  int stride, int dilation, int padding, int kernel_dim,

		  elem_t * input,
		  elem_t * weights,
		  acc_t * bias,
		  elem_t * output,

		  int act, acc_scale_t scale, size_t relu6_shift,
		  int pool_size, int pool_stride, int pool_padding,

		  enum tiled_matmul_type_t tiled_conv_type, 
		  size_t orow_divide, size_t batch_divide, int cid, bool skip_weight) {

	 const bool no_pool = pool_stride == 0;
	 if (no_pool) {
		  pool_size = 1;
		  pool_stride = 1;
		  pool_padding = 0;
	 }
#ifdef GEMMINI_ASSERTIONS
	 if(batch_size == 1 && batch_divide > 1){
		 printf("batch_divide set wrong \n");
		 exit(1);
	 }
	 if(orow_divide > 1 && batch_divide > 1){
		 printf("Allowed to divide in single dimension only \n");
		 exit(1);
	 }
#endif
	 int out_stride = out_channels;
	 int in_stride = in_channels;
	 int weight_stride = out_stride;
	 int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

	 // divide in batch dimension
	 batch_size = batch_size / batch_divide;
	 int batch_in_offset = (batch_divide > 1) ? batch_size*in_dim*in_dim*in_channels*cid : 0;
	 int batch_out_offset = (batch_divide > 1) ? batch_size*pool_out_dim*pool_out_dim*out_channels*cid : 0; // not dividing in out_channel dimension

	 // divide in row dimension (single batch)
	 bool row_divisible = false;//(orow_divide > 1) && (pool_out_dim % orow_divide == 0);
	 int pool_out_row = (row_divisible) ? (pool_out_dim / orow_divide) : pool_out_dim;
	 const size_t och_divide = orow_divide;//(row_divisible) ? 1 : orow_divide; //if row isn't divisible, divide channel instead
	 orow_divide = 1;
	 //if(!row_divisible)	orow_divide = 1;
	 out_channels = out_channels / och_divide;
	 in_channels = in_channels / och_divide; //divide input channel as well
  	 const int out_offset = ((row_divisible && (orow_divide > 1)) || (batch_divide > 1)) ? 0 : out_channels * cid;
	 const int in_offset = (out_offset == 0) ? 0 : in_channels * cid;

	 // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
	 int args[] = {batch_size, pool_out_dim, pool_out_dim, 1, kernel_dim, kernel_dim, 1};
	 const int max_args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, kernel_dim, kernel_dim, in_channels};

	 const int orows_idx = 1;
	 const int ocols_idx = 2;
	 const int out_channels_idx = 3;
	 const int in_channels_idx = 6;

	 // We divide by 2 for the sake of double-buffering
	 const int max_spad_rows = (BANK_NUM*BANK_ROWS / 2);
	 const int max_acc_rows = (ACC_ROWS / 2);

	 int spad_rows = tiled_conv_total_spad_rows_A_stride(false,
		  stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
	 int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
		  stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

	 while (spad_rows > max_spad_rows || acc_rows > max_acc_rows) {
		  int max_val = -1;
		  int max_idx = -1;

		  for (size_t j = 0; j < sizeof(args)/sizeof(args[0]) - 2; j++) {
				// We avoid reducing ocols when possible to keep the spatial array fully utilized
			  size_t i = 0;
			  if(j == 0) i = 0;
			  else if (j == 4) i = orows_idx;
			  else if(j == 1) i = ocols_idx;
				else if (j == 2) i = 4;
				else if(j == 3) i = 5;

			  if(i == 0 && args[0] > 1){ // batch first
					max_val = args[0];
					max_idx = 0;
					break;
				}else if (!(i == ocols_idx && args[i] <= DIM)
						  && args[i] > max_val) { // and then move on to channels
					 max_val = args[i];
					 max_idx = i;
				}
		  }

			  if(max_idx == ocols_idx){
				  if(args[max_idx] % DIM != 0) args[max_idx] = (args[max_idx]/DIM)*DIM;
				  else args[max_idx] -= DIM;
			  }else{
				  if(max_idx == 4 || max_idx == 5) args[max_idx] = 1;
				  else args[max_idx]--;
			  }

		  spad_rows = tiled_conv_total_spad_rows_A_stride(false,
				stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
		  acc_rows = tiled_conv_total_spad_rows_A_stride(true,
				stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
	 }
/*
    printf("batches = %d\n", args[0]);
    printf("orows   = %d\n", args[1]);
    printf("ocols   = %d\n", args[2]);
    printf("ochs    = %d\n", args[3]);
    printf("krows   = %d\n", args[4]);
    printf("kcols   = %d\n", args[5]);
    printf("kchs    = %d\n\n", args[6]);
*/

	 // Check if we can increase ocols
	 bool not_increased = false;

	 // Check if there are any parameters that we can currently still increase
	 bool nothing_increased = false;
	 bool kdim_increase = true;
	 while (!nothing_increased) {
		  nothing_increased = true;
		  //kdim_increase = true;

		  for (size_t j = 0; j < sizeof(args)/sizeof(args[0]) - 2; j++) {
			   //size_t i =j;//  down_sample ? j : 6-j;
				size_t i = j;
				if(j == 0) i = 5;//in_channels_idx;
				else if (j == 1) i = 4;
				else if(j == 2) i = ocols_idx;
				else if (j == 3) i = orows_idx;
				else if(j == 4) i = 0;
				int args_candidate[] = {args[0], args[1], args[2], args[3], args[4], args[5], args[6]};
				if(i == ocols_idx && (args[i] % DIM == 0)) args_candidate[i] += DIM;
				else args_candidate[i]+= kdim_increase && (i == 4 || i == 5) ? 2 : 1;
				if (args_candidate[i] > max_args[i])
					 continue;

				spad_rows = tiled_conv_total_spad_rows_A_stride(false,
					 stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);
				acc_rows = tiled_conv_total_spad_rows_A_stride(true,
					 stride, dilation, args_candidate[0], args_candidate[1], args_candidate[2], args_candidate[3], args_candidate[4], args_candidate[5], args_candidate[6], pool_size, pool_stride);

				if (spad_rows <= max_spad_rows && acc_rows <= max_acc_rows) {
					 args[i] = args_candidate[i];
					 nothing_increased = false;
					 kdim_increase = false;
				}
		  }
	 }

	 const int batches = args[0];
	 const int orows = args[1];
	 const int ocols = args[2];
	 const int ochs = args[3];
	 const int krows = args[4];
	 const int kcols = args[5];
	 const int kchs = args[6];

/*
    spad_rows = tiled_conv_total_spad_rows_A_stride(false,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    acc_rows = tiled_conv_total_spad_rows_A_stride(true,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

    printf("batches = %d\n", batches);
    printf("orows   = %d\n", orows);
    printf("ocols   = %d\n", ocols);
    printf("ochs    = %d\n", ochs);
    printf("krows   = %d\n", krows);
    printf("kcols   = %d\n", kcols);
    printf("kchs    = %d\n\n", kchs);

    printf("total spad_rows reserved: %d\n", spad_rows);
    printf("total acc_rows reserved: %d\n\n", acc_rows);

    printf("scratchpad row utilization: %d%%\n", (spad_rows*100) / max_spad_rows);
    printf("accumulator row utilization: %d%%\n\n", (acc_rows*100) / max_acc_rows);

    printf("inner matmul size: i=%d, j=%d, k=%d\n\n", ocols, ochs, kchs);
  */
	 bool no_bias = (bias == NULL);
	 tiled_conv_A_stride_dw(
		  batch_size, in_dim, in_channels,
		  out_channels, out_dim,
		  stride, dilation, padding, kernel_dim,
		  in_stride, out_stride, weight_stride,

		  batches,
		  orows, ocols, ochs,
		  krows, kcols, kchs,

		  input + batch_in_offset + in_offset,
		  weights + out_offset,
		  no_bias ? NULL : bias + out_offset,
		  output + batch_out_offset + out_offset,

		  act, scale, relu6_shift,
		  pool_size, no_pool ? 0 : pool_stride, pool_padding,

		  tiled_conv_type, och_divide, orow_divide, cid, skip_weight);
}



// This function is for a convolution with kernel_dim=1, stride==2, padding=0, and no pooling
static void tiled_conv_downsample(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,

        enum tiled_matmul_type_t tiled_conv_type) {

    const int stride = 2;

    for (int b = 0; b < batch_size; b++) {
        for (int irow = 0; irow < in_dim; irow += stride) {
            const int orow = irow / stride;

            const int I = in_dim / stride; // number of columns in row
            const int J = out_channels;
            const int K = in_channels;

            const elem_t * A = input + (b*in_dim + irow)*in_dim*in_channels;
            const elem_t * B = weights;
            const acc_t * D = bias;
            elem_t * C = output + (b*out_dim + orow)*out_dim*out_channels;

            const int A_stride = in_channels * 2;
            const int B_stride = out_channels;
            const int D_stride = out_channels;
            const int C_stride = out_channels;

            tiled_matmul_auto(I, J, K, A, B, (void*)D, (void*)C,
                    A_stride, B_stride, D_stride, C_stride,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    MVIN_SCALE_IDENTITY, act, scale, relu6_shift,
                    true, false, false, false, false, tiled_conv_type);
        }
    }
}

static void tiled_conv_downsample_cid(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,

        enum tiled_matmul_type_t tiled_conv_type,
        size_t orow_divide, size_t batch_divide, size_t cid, bool skip_weight) {

	 const int stride = 2;
	 const int A_stride = in_channels * 2;
	 const int C_stride = out_channels;
/*
	 const int A_stride = ich_padding ? (in_channels + 64) * 2 : in_channels * 2;
	 const int B_stride = och_padding ? (out_channels + 64) : out_channels;
	 const int D_stride = och_padding ? (out_channels + 64) : out_channels;
	 const int C_stride = och_padding ? (out_channels + 64) : out_channels;
*/
#ifdef GEMMINI_ASSERTIONS
	 if(batch_divide > 1 && orow_divide > 1){
		 printf("currently divide in either batch or orow \n");
		 exit(1);
	 }
	 if(batch_divide > 1 && batch_size % batch_divide != 0){
		 printf("batch not divisible \n");
		 exit(1);
	 }
#endif
	 bool row_divisible = (orow_divide > 1) &&  (out_dim % orow_divide == 0);
	 in_dim = (row_divisible) ? in_dim / orow_divide : in_dim; // divide if it is divisible
	 batch_size = batch_size / batch_divide;
	 out_dim = (row_divisible) ? out_dim / orow_divide : out_dim;
	 out_channels = (row_divisible) ? out_channels : out_channels / orow_divide; // divide if row can't be divided
	 int ocol_offset = (row_divisible || (orow_divide == 1)) ? 0 : out_channels * cid; // need offset if channel is divided
	 int A_orow_offset = (row_divisible) ? A_stride * cid * in_dim : 0; // need offset if row is divided
	 int C_orow_offset = (row_divisible) ? C_stride * cid * out_dim : 0; // need offset if row is divided
	 int batch_in_offset = (batch_divide > 1) ? batch_size*in_dim*in_dim*(A_stride / 2)*cid : 0;
	 int batch_out_offset = (batch_divide > 1) ? batch_size*out_dim*out_dim*C_stride*cid : 0;

    for (int b = 0; b < batch_size; b++) {
        for (int irow = 0; irow < in_dim; irow += stride) {
            const int orow = irow / stride;

            const int I = in_dim / stride; // number of columns in row
            const int J = out_channels;
            const int K = in_channels;

            const elem_t * A = input + (b*in_dim + irow)*in_dim*in_channels + A_orow_offset + batch_in_offset;
            const elem_t * B = weights + ocol_offset;
            const acc_t * D = bias + ocol_offset;
            elem_t * C = output + (b*out_dim + orow)*out_dim*out_channels + ocol_offset + C_orow_offset + batch_out_offset;

            tiled_matmul_auto_loopld(I, J, K, A, B, (void*)D, (void*)C,
                    A_stride, C_stride, C_stride, C_stride,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    MVIN_SCALE_IDENTITY, act, scale, relu6_shift,
                    true, false, false, false, false, tiled_conv_type,
                    false, skip_weight);
        }
    }
}


static void tiled_conv_downsample_loopld(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,

        enum tiled_matmul_type_t tiled_conv_type,
        size_t och_divide, bool skip_weight) {

    const int stride = 2;

    for (int b = 0; b < batch_size; b++) {
        for (int irow = 0; irow < in_dim; irow += stride) {
            const int orow = irow / stride;

            const int I = in_dim / stride; // number of columns in row
            const int J = out_channels;
            const int K = in_channels;

            const elem_t * A = input + (b*in_dim + irow)*in_dim*in_channels;
            const elem_t * B = weights;
            const acc_t * D = bias;
            elem_t * C = output + (b*out_dim + orow)*out_dim*out_channels;

				const int A_stride = in_channels*2;
				const int B_stride = out_channels * och_divide;
				/*
            const int A_stride = ich_padding ? (in_channels + 64) * 2 : in_channels * 2;
            const int B_stride = och_padding ? (out_channels * och_divide + 64) : out_channels * och_divide;
            const int D_stride = och_padding ? (out_channels * och_divide + 64) : out_channels * och_divide;
            const int C_stride = och_padding ? (out_channels * och_divide + 64) : out_channels * och_divide;
				*/
            tiled_matmul_auto_loopld(I, J, K, A, B, (void*)D, (void*)C,
                    A_stride, B_stride, B_stride, B_stride,
                    MVIN_SCALE_IDENTITY, MVIN_SCALE_IDENTITY,
                    MVIN_SCALE_IDENTITY, act, scale, relu6_shift,
                    true, false, false, false, false, tiled_conv_type,
                    false, skip_weight);
        }
    }
}


static void tiled_conv_dw(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        int batches,
        int porows, int pocols,// int pochs,
//        int krows, int kcols, int kchs,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type) {

    const int krows = kernel_dim;
    const int kcols = kernel_dim;
    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

#ifdef GEMMINI_ASSERTIONS
    {
        // Check that data will fit in scratchpad
        const int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
            stride, batches, porows, pocols, 1, kcols, kcols, 1, pool_size, pool_stride);
        const int spad_rows_input = tiled_conv_total_spad_rows(false, false,
            stride, batches, porows, pocols, 1, kcols, kcols, 1, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows(true, false,
            stride, batches, porows, pocols, 1, kcols, kcols, 1, pool_size, pool_stride);

	const int weight_bank = 1;

        if (spad_rows_weight > BANK_ROWS * weight_bank) {
            printf("not enough scratchpad space to store weights\n");
            exit(1);
        }
        if (spad_rows_input > BANK_ROWS*(BANK_NUM - weight_bank)) {
            printf("not enough scratchpad space to store inputs\n");
            exit(1);
        }
        if (acc_rows > ACC_ROWS) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
    }
#endif

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
    if (no_pool) {
        gemmini_config_st(out_channels * sizeof(elem_t));
    }
       for (int b = 0; b < batch_size; b += batches) {
     	    for (int porow = 0; porow < pool_out_dim; porow += porows) {
            	const int orow = porow * pool_stride - pool_padding;
            	for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
		    for(int poch = 0; poch < out_channels; poch += 1){
			int kch = poch;
		        bool mvin_weight = true;
                        const int ocol = pocol * pool_stride - pool_padding;
                        const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride - padding;
                        const int ocol_floored = ocol < 0 ? 0 : ocol;
                        const int icol = ocol_floored * stride - padding;

                            //for (int kch = 0; kch < in_channels; kch += 1) {
                                elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_channels + poch;

                                acc_t * bias_ = bias + poch;
                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

				const int icols_ = (ocols_ - 1 - plpad - prpad) * stride + kcols;//+ kcols_;
                                const int irows_ = (orows_ - 1 - pupad - pdpad) * stride + kcols;//krows_;


                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

                                // printf("upad: %d\n", upad);
                                // printf("dpad: %d\n", dpad);
                                // printf("lpad: %d\n", lpad);
                                // printf("rpad: %d\n", rpad);
                                // printf("pupad: %d\n", pupad);
                                // printf("pdpad: %d\n", pdpad);
                                // printf("plpad: %d\n", plpad);
                                // printf("prpad: %d\n", prpad);

                                sp_tiled_conv_dw(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_,// pochs_,

				    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels + kch,
                                    //weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + poch,
			     	    weights + poch,
                                    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool, mvin_weight);
                            }
                        }
                    }

    }
}

static void tiled_conv_first(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding,

        int batches,
        int porows, int pocols, int pochs,
	int kcols, int kchs,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

	enum tiled_matmul_type_t tiled_conv_type,
	int weight_bank) {


    if (tiled_conv_type == CPU) {
      if (pool_size == 1 && pool_stride == 1 && pool_padding == 0) {
        pool_stride = 0;
      }

      conv_cpu(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, 1, padding, kcols,//kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift,
        pool_size, pool_stride, pool_padding);
      return;
    } else if (tiled_conv_type == OS) {
      printf("Gemmini convs do not currently support OS\n");
      exit(1);
    }

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

    bool no_1d = pool_stride == 0 && pool_size == 0;
    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

//    printf("no_1d: %d, no_pool: %d, pool_size: %d, pool_stride: %d \n", no_1d, no_pool, pool_size, pool_stride);

#ifdef GEMMINI_ASSERTIONS
    {
        // Check that data will fit in scratchpad
        const int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);
        const int spad_rows_input = tiled_conv_total_spad_rows(false, false,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows(true, false,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);

        // printf("spad_rows: %d\n", spad_rows);
        // printf("acc_rows: %d\n", acc_rows);
        // exit(1);

        if (spad_rows_weight > weight_bank*BANK_ROWS) {
            printf("not enough scratchpad space to store weights\n");
            exit(1);
        }
        if (spad_rows_input > BANK_ROWS*(BANK_NUM - weight_bank)) {
            printf("not enough scratchpad space to store inputs\n");
            exit(1);
        }
        if (acc_rows > ACC_ROWS) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }

    }
#endif

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    if (no_pool && no_1d) {
        gemmini_config_st(out_channels * sizeof(elem_t));
    }
    int pmax = 0;
    for(int porow = 0; porow < pool_out_dim; porow += porows)
	pmax = porow;
    bool mvin_weight = false;

    for (int b = 0; b < batch_size; b += batches) {
        for (int porow = 0; porow < pool_out_dim; porow += porows) {
            const int orow = porow * pool_stride - pool_padding;

            for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
		if(pocol == 0 || porow == 0 || pocol == pmax || porow == pmax){ //when there is padding
                const int ocol = pocol * pool_stride - pool_padding;
                for (int poch = 0; poch < out_channels; poch += pochs) {
			if(pocol == 0 && porow == 0 && b == 0 && poch == 0) mvin_weight = true;
			else mvin_weight = false;

	                const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride - padding;//+ krow - padding;
                        const int ocol_floored = ocol < 0 ? 0 : ocol;
                        const int icol = ocol_floored * stride - padding; //+ kcol - padding;
//			    printf("icol: %d, ocols: %d, ocol: %d, ocol_floored: %d, kcol: %d \n", icol, ocols, ocol, ocol_floored, kcol);

                            //for (int kch = 0; kch < in_channels; kch += kchs) {
                        elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_channels + poch;
                                
                        acc_t * bias_ = bias + poch;
                               
                        const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                        const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                        const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                        const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
			const int kchs_ = in_channels;

                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

        			const int icols_ = (ocols_ - 1 - plpad - prpad) * stride + kcols;//+ kcols_;
                                const int irows_ = (orows_ - 1 - pupad - pdpad) * stride + kcols;//krows_;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

                               sp_tiled_conv_ws_original_first(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, padding, kcols,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
			            kcols, kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels,// + kch,	
				    weights + poch,
		 		    //weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + poch,
                                    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool, mvin_weight,
                                    weight_bank);
	
                           }
                        }
                    }
	       }
    }

    for (int b = 0; b < batch_size; b += batches) {
        for (int porow = porows; porow < pmax; porow += porows) {
            const int orow = porow * pool_stride - pool_padding;

            for (int pocol = pocols; pocol < pmax; pocol += pocols) {
                const int ocol = pocol * pool_stride - pool_padding;

                for (int poch = 0; poch < out_channels; poch += pochs) {
			if(poch == 0 && pocol == pocols && porow == porows && b == 0) mvin_weight = true;
			else mvin_weight = false;
	                const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride - padding;//+ krow - padding;
                        const int ocol_floored = ocol < 0 ? 0 : ocol;
                        const int icol = ocol_floored * stride - padding; //+ kcol - padding;
                               elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_channels + poch;
                                
                                acc_t * bias_ = bias + poch;
                               

                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
				const int kchs_ = in_channels;

                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

       				const int icols_ = (ocols_ - 1 - plpad - prpad) * stride + kcols;//+ kcols_;
                                const int irows_ = (orows_ - 1 - pupad - pdpad) * stride + kcols;//krows_;

                               sp_tiled_conv_first(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, padding,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
			            kcols, kchs_,

                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow)*in_dim + (icol)) * in_channels,// + kch,
				    weights + poch,
		 		    //weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + poch,
                                    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool, mvin_weight,
                                    weight_bank);
	                           }
                        }
                    }
	     //  }
    }
}


static void sp_tiled_conv_ws(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim, int pool_out_dim,

        int stride, int padding, //int kernel_dim,

        int pool_size, int pool_stride, int pool_padding,

        int batches,
        int porows, int pocols, int pochs,
	int krows, int kchs,

        int lpad, int rpad, int upad, int dpad,
        int plpad, int prpad, int pupad, int pdpad,

        const elem_t * input,
	uint32_t B_sp_addr_start,
        //elem_t * weights,
        elem_t * output,
        const acc_t * bias,

	int act, acc_scale_t scale, int relu6_shift,
        bool no_bias, bool no_pool,
	int weight_bank) {

    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    // Calculate image dimensions
    const int irows = (orows - 1) * stride + krows;
    const int icols = (ocols - 1) * stride + krows;//kcols; 
    const int irows_unpadded = irows - upad - dpad;
    const int icols_unpadded = icols - lpad - rpad;
    const int ichs = kchs;
    int kdims = krows*krows; 

    int double_bank = weight_bank > 1 ? 1 : 0;
    int triple_bank = weight_bank > 2 ? 1 : 0;
	const int odims = ocols*orows;
//	const int im2col_width = kdims*kchs;
	const int row_left = odims%DIM;
	const int row_turn = row_left == 0 ? odims/DIM - 1 : odims/DIM;
//	const int turn = im2col_width%DIM == 0 ? im2col_width/DIM : im2col_width/DIM + 1;
	gemmini_extended2_config_ex(WEIGHT_STATIONARY, act, 0, scale, relu6_shift, 1, false, false, ocols, row_turn, krows, stride, kchs, row_left, kdims, double_bank, triple_bank); //if want 2 banks for weight, last is 1

    int idims = irows*icols;
    int bidims = batches*idims;
   if(no_pool){
	gemmini_config_st(out_channels*sizeof(elem_t));
   }
   else{
	   gemmini_extended_config_st(out_channels * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);
   }
    const uint32_t A_sp_addr_start = 0;
    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN - 1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN - 2);

     //printf("mvin bias\n");
    // mvin bias

    if (!no_bias && bias != NULL) {
        // TODO we probably don't need quite this many nested loops for this part
        gemmini_config_ld(0);
        for (int b = 0; b < batches; b++)
          for (int och = 0; och < ochs; och += DIM) {
               const int J = ochs - och > DIM ? DIM : ochs - och;
               const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
	        for(int odim = 0; odim < odims; odim += DIM){
                   // const int I = ocols - ocol > DIM ? DIM : ocols - ocol;
		    const int I = odims - odim > DIM ? DIM : odims - odim;
                        gemmini_extended_mvin(bias + och,
                                D_sp_addr+odim,
                                J, I);
                    }
                }
    }

    // mvin input
    // printf("mvin inputs\n");
    gemmini_config_ld(in_channels * sizeof(elem_t));
    static elem_t zeros[MAX_BYTES / sizeof(elem_t)] = {0};
//    gemmini_fence(); // TODO fix ROB to get rid of this requirement
    for (int b = 0; b < batches; b++) {
        for (int irow = -upad; irow < irows_unpadded + dpad; irow++) {
            const int irow_padded = irow + upad;

            for (int icol = -lpad; icol < icols_unpadded + rpad;) {
                int I = icols_unpadded - icol > DIM ? DIM : icols_unpadded - icol;
                const elem_t * in = input + (b*in_dim*in_dim + irow*in_dim + icol) * in_channels;// + ich;
 
                if (icol < 0) {
                    I = -icol > DIM ? DIM : -icol;
                } else if (icol >= icols_unpadded) {
                    I = icols_unpadded + rpad - icol > DIM ? DIM : icols_unpadded + rpad - icol;
                }
                const bool is_zeros = irow < 0 || irow >= irows_unpadded || icol < 0 || icol >= icols_unpadded; 
                const int icol_padded = icol + lpad;
		const uint32_t A_sp_addr = A_sp_addr_start + b * idims + irow_padded * icols + icol_padded;
		if(is_zeros){
	           	   gemmini_config_ld(0);
			for (int ich = 0; ich < ichs; ich += DIM) {
                    	   const int K = ichs - ich > DIM ? DIM : ichs - ich;
                           in = &zeros[0];
                           gemmini_extended_mvin(in+ich,
                            A_sp_addr + (ich/DIM)*bidims,
                            K, I);
                    }
		   gemmini_config_ld(in_channels * sizeof(elem_t));


		}else{
                   for (int ich = 0; ich < ichs; ich += DIM) {
                       const int K = ichs - ich > DIM ? DIM : ichs - ich;
                       gemmini_extended_mvin(in+ich,
                            A_sp_addr + (ich/DIM)*bidims,
                            K, I);

                    }
		}
                icol += I;
            }
        }
    }

//printf("matmul \n");
   for (int b = 0; b < batches; b++){
        for (int och = 0; och < ochs; och += DIM) {
            const int J = ochs - och > DIM ? DIM : ochs - och;
     	    const uint32_t C_sp_addr_outer = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;

	    for (int kch = 0; kch < kchs; kch += DIM) {
	        const int K = kchs - kch > DIM ? DIM : kchs - kch;
		const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM)*batches*idims + b*idims;           
		const int kkdims = K*kdims;
		const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs + kch*kdims;
	
            	for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
			const int I = odims - odim > DIM ? DIM : odims - odim;
			const uint32_t C_sp_addr = C_sp_addr_outer + odim;

			for(int kkdim = 0; kkdim < kkdims; kkdim += K){	
                                gemmini_extended_preload(B_sp_addr + kkdim, C_sp_addr,
                                        J, K, J, I);
                                gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
			}
                   }
                }
   	}
     }

   //gemmini_fence();
//printf("mvout\n");
  if(output!=NULL){
     if (no_pool) {
            for (int b = 0; b < batches; b++)
                for (int orow = 0; orow < orows; orow++)
                    for (int ocol = 0; ocol < ocols; ocol += DIM) {
                        const int I = ocols - ocol > DIM ? DIM : ocols - ocol;

                        for (int och = 0; och < ochs; och += DIM) {
                            const int J = ochs - och > DIM ? DIM : ochs - och;
                            const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                            gemmini_extended_mvout(output + (b*out_dim*out_dim + orow*out_dim + ocol) * out_channels + och,
                                    C_sp_addr,
                                    J, I);
                        }
                    
                    }

      } else {
            gemmini_fence(); // TODO remove this when the ROB can accurately handle these
            for (int b = 0; b < batches; b++) {
                for (int poch = 0; poch < pochs; poch += DIM) {
                    const int channels = poch + DIM >= pochs ? pochs - poch : DIM;
                    elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + poch;
                    const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;
                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            channels, 0);
                }
            }
            gemmini_fence(); // TODO remove this when the ROB can accurately handle these
      }
   }
//attempts to merge mvin-matmul-mvout loops
//ROB problem: some have wrong results
/*
   gemmini_config_ld(0);
   for (int b = 0; b < batches; b++){
        for (int och = 0; och < ochs; och += DIM) {
            const int J = ochs - och > DIM ? DIM : ochs - och;
            const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;
      	    const uint32_t C_sp_addr_outer = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;// + odim;

	    for (int kch = 0; kch < kchs; kch += DIM) {
	        const int K = kchs - kch > DIM ? DIM : kchs - kch;
		const uint32_t A_sp_addr = A_sp_addr_start + (kch / DIM)*batches*idims + b*idims;           
		const int kkdims = K*kdims;
		const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs + kch*kdims;
	
            	for(int odim = 0; odim < odims; odim += DIM){ //both dimension at the same time
			const int I = odims - odim > DIM ? DIM : odims - odim;
			if(kch == 0)
                        	gemmini_extended_mvin(bias + och,
                                	D_sp_addr+odim,
                                	J, I);
			const uint32_t C_sp_addr = C_sp_addr_outer + odim;

			for(int kkdim = 0; kkdim < kkdims; kkdim += K){	
                                gemmini_extended_preload(B_sp_addr + kkdim, C_sp_addr,
                                        J, K, J, I);
                                gemmini_extended_compute_preloaded(A_sp_addr, GARBAGE_ADDR, K, I, J, I);
			}
                   }
                }

 	    if(output!=NULL){
		//const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;
		if(no_pool){
  		   gemmini_extended_mvout(output + (b * out_dim * out_dim)*out_channels + och, C_sp_addr_outer, J, 0);
		}
		else{
		   gemmini_extended_mvout(output + (b * pool_out_dim * pool_out_dim) * out_channels + och, C_sp_addr_outer, J, 0);
		}
	   }

   	}
     }
           if (no_pool) {
            for (int b = 0; b < batches; b++)
                for (int orow = 0; orow < orows; orow++)
                    for (int ocol = 0; ocol < ocols; ocol += DIM) {
                        const int I = ocols - ocol > DIM ? DIM : ocols - ocol;

                        for (int och = 0; och < ochs; och += DIM) {
                            const int J = ochs - och > DIM ? DIM : ochs - och;
//			int J = 1; int och = 0;
                            const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

                            gemmini_extended_mvout(output + (b*out_dim*out_dim + orow*out_dim + ocol) * out_channels + och,
                                    C_sp_addr,
                                    J, I);
                        }

                    }
	   } else {
//		   printf("pool \n");
              gemmini_extended_config_st(out_channels * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);
//             gemmini_fence(); // TODO remove this when the ROB can accurately handle these
            for (int b = 0; b < batches; b++) {
                for (int poch = 0; poch < pochs; poch += DIM) {
                    const int channels = poch + DIM >= pochs ? pochs - poch : DIM;
                    elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*out_channels + poch;
                    const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;
                    gemmini_extended_mvout(pout,
                            C_sp_addr,
                            channels, 0);
                }
            }
//            gemmini_fence();
        }
*/
/*
    // mvout output
   if (output != NULL) {
		gemmini_extended_config_st(out_channels * sizeof(elem_t), 0, 1, out_dim, 0, 0, orows, ocols, 0, 0);
		for(int b = 0; b < batches; b++)
			for(int och = 0; och < ochs; och += DIM){
				const int J = ochs - och > DIM ? DIM : ochs - och;
				const uint32_t C_sp_addr = C_sp_addr_start + (och / DIM) * batches * odims + b * odims;
				gemmini_extended_mvout(output + (b * out_dim * out_dim)*out_channels + och, C_sp_addr, J, 0);
			}	
		//}

*/
//            gemmini_fence();
//    	uint64_t end_mvout = read_cyclesh();
//	printf("mvout cycles: %d \n", end_mvout - start_mvout);

}

//outer loop without weight mvin (due to large channel size)
static void tiled_conv_original(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        int batches,
        int porows, int pocols, int pochs,
	int kcols, int kchs,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

	enum tiled_matmul_type_t tiled_conv_type,
	int weight_bank) {


    if (tiled_conv_type == CPU) {
      if (pool_size == 1 && pool_stride == 1 && pool_padding == 0) {
        pool_stride = 0;
      }

      conv_cpu(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, 1, padding, kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift,
        pool_size, pool_stride, pool_padding);
      return;
    } else if (tiled_conv_type == OS) {
      printf("Gemmini convs do not currently support OS\n");
      exit(1);
    }

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

//    bool no_1d = pool_stride == 0 && pool_size == 0;
    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }
#ifdef GEMMINI_ASSERTIONS
    {
        // Check that data will fit in scratchpad
        const int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);
        const int spad_rows_input = tiled_conv_total_spad_rows(false, false,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows(true, false,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);

        if (spad_rows_weight > weight_bank*BANK_ROWS) {
            printf("not enough scratchpad space to store weights\n");
            exit(1);
        }
        if (spad_rows_input > BANK_ROWS*(BANK_NUM - weight_bank)) {
            printf("not enough scratchpad space to store inputs\n");
            exit(1);
        }
        if (acc_rows > ACC_ROWS) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
        if (kernel_dim <= padding) {
            printf("kernel_dim must be larger than padding\n");
            exit(1);
        }
    }
#endif

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    if (no_pool) {
        gemmini_config_st(out_channels * sizeof(elem_t));
    }

    for (int b = 0; b < batch_size; b += batches) {
        for (int porow = 0; porow < pool_out_dim; porow += porows) {
            const int orow = porow * pool_stride - pool_padding;

            for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
                const int ocol = pocol * pool_stride - pool_padding;

                for (int poch = 0; poch < out_channels; poch += pochs) {
	                const int orow_floored = orow < 0 ? 0 : orow;
                        const int irow = orow_floored * stride - padding;//+ krow - padding;
                        const int ocol_floored = ocol < 0 ? 0 : ocol;
                        const int icol = ocol_floored * stride - padding; //+ kcol - padding;
//			    printf("icol: %d, ocols: %d, ocol: %d, ocol_floored: %d, kcol: %d \n", icol, ocols, ocol, ocol_floored, kcol);

                            for (int kch = 0; kch < in_channels; kch += kchs) {
                                elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_channels + poch;
                                if (kch + kchs < in_channels) {
                                    out = NULL;
                                }
                                const acc_t * bias_ = bias + poch;
                                if (kch > 0) {
                                    bias_ = NULL;
                                }

                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
                                const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
			        const int kchs_ = in_channels - kch > kchs ? kchs : in_channels - kch;

                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

				const int icols_ = (ocols_ - 1 - plpad - prpad) * stride + kcols;//+ kcols_;
                                const int irows_ = (orows_ - 1 - pupad - pdpad) * stride + kcols;//krows_;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;

				if(kernel_dim != 1)
                                  sp_tiled_conv_ws_original(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
			            kcols, kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels + kch,
				    weights + kch * out_channels + poch,
		 		    //weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + poch,
                                    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool,
                                    weight_bank);
			  
				else //downsampling layer
                                  sp_tiled_conv_ds(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
			            kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels + kch,
				    0,	
				    weights + kch * out_channels + poch,
		 		    //weights + (krow*kernel_dim*in_channels + kcol*in_channels + kch) * out_channels + poch,
                                    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool, 
                                    weight_bank);
                           }
                        }
                    }
	       }
    }
}


static void tiled_conv(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        int batches,
        int porows, int pocols, int pochs,
        int krows, int kcols, int kchs,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        int weight_bank, enum tiled_matmul_type_t tiled_conv_type) {

    if (tiled_conv_type == CPU) {
      if (pool_size == 1 && pool_stride == 1 && pool_padding == 0) {
        pool_stride = 0;
      }

      conv_cpu(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, 1, padding, kernel_dim,
        input, weights, bias, output,
        act, scale, relu6_shift,
        pool_size, pool_stride, pool_padding);
      return;
    } else if (tiled_conv_type == OS) {
      printf("Gemmini convs do not currently support OS\n");
      exit(1);
    }

    // TODO move everything below this into a tiled_conv_outer function to match the tiled_matmul function

    bool no_bias = false;
    if (bias == NULL) {
        bias = (acc_t*)1;
        no_bias = true;
    }

    bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }



#ifdef GEMMINI_ASSERTIONS
    {
        // Check that data will fit in scratchpad
        const int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);
        const int spad_rows_input = tiled_conv_total_spad_rows(false, false,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);
        const int acc_rows = tiled_conv_total_spad_rows(true, false,
            stride, batches, porows, pocols, pochs, kcols, kcols, kchs, pool_size, pool_stride);

	if (spad_rows_weight > BANK_ROWS * weight_bank) {
            printf("not enough scratchpad space to store weights\n");
            exit(1);
        }
        if (spad_rows_input > BANK_ROWS*(BANK_NUM - weight_bank)) {
            printf("not enough scratchpad space to store inputs\n");
            exit(1);
        }
        if (acc_rows > ACC_ROWS) {
            printf("not enough accumulator space to store outputs\n");
            exit(1);
        }
    }
#endif
    int kdims = kcols*kcols;
    const uint32_t B_sp_addr_start = (BANK_NUM - weight_bank) * BANK_ROWS;
 
    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

      for (int poch = 0; poch < out_channels; poch += pochs) {
           const int pochs_ = out_channels - poch > pochs ? pochs : out_channels - poch;
                const acc_t * bias_ = bias + poch;

		const int kchs_ = in_channels;
		gemmini_config_ld(out_channels*sizeof(elem_t));
		//mvin weight on the outer loop
		//printf("mvin weight\n");
		  for (int och = 0; och < pochs_; och += DIM) {
        		const int J = pochs_ - och > DIM ? DIM : pochs_ - och;
      			for (int ich = 0; ich < kchs_; ich += DIM) {
        		    const int K = kchs_ - ich > DIM ? DIM : kchs_ - ich;
        		    for (int krow = 0; krow < kcols; krow++){
				const elem_t * weight = weights + poch + (krow*kcols*in_channels + ich) * out_channels + och;
				const uint32_t B_sp_addr = B_sp_addr_start + (och / DIM) * kdims * kchs_ + ich*kdims + krow*kcols*K;// + kcol*K;
            			for (int kcol = 0; kcol < kcols; kcol++){
					gemmini_extended_mvin(weight + kcol*in_channels*out_channels,
						B_sp_addr+kcol*K,
                        			J, K);
	    			}
       			    }
			}
    		  }	
		  for (int b = 0; b < batch_size; b += batches) {
		        for (int porow = 0; porow < pool_out_dim; porow += porows) {
		            const int orow = porow * pool_stride - pool_padding;
            		    for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
				elem_t * out = output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_channels + poch;
		                const int ocol = pocol * pool_stride - pool_padding;
                                const int batches_ = batch_size - b > batches ? batches : batch_size - b;
                                const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
                                const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
			        const int orow_floored = orow < 0 ? 0 : orow;
            			const int irow = orow_floored * stride - padding;//+ krow - padding;
            			const int ocol_floored = ocol < 0 ? 0 : ocol;
            			const int icol = ocol_floored * stride - padding; //+ kcol - padding;
 
                                const int ocols_ = pocols_ * pool_stride + pool_size - 1;
                                const int orows_ = porows_ * pool_stride + pool_size - 1;

                                const int plpad = ocol < 0 ? -ocol : 0;
                                const int prpad = ocol + ocols_ > out_dim ? ocol + ocols_ - out_dim : 0;
                                const int pupad = orow < 0 ? -orow : 0;
                                const int pdpad = orow + orows_ > out_dim ? orow + orows_ - out_dim : 0;

     				const int icols_ = (ocols_ - 1 - plpad - prpad) * stride + kcols;//+ kcols_;
                                const int irows_ = (orows_ - 1 - pupad - pdpad) * stride + kcols;//krows_;

                                const int lpad = icol < 0 ? -icol : 0;
                                const int rpad = icol + icols_ > in_dim ? icol + icols_ - in_dim : 0;
                                const int upad = irow < 0 ? -irow : 0;
                                const int dpad = irow + irows_ > in_dim ? irow + irows_ - in_dim : 0;
/*				printf("ocols_: %d \n", ocols_);
				printf("orows_: %d \n", orows_);
      				printf("icols_: %d \n", icols_);
				printf("irows_: %d \n", irows_);
				printf("kchs_: %d \n", kchs_);
				printf("kch: %d \n", kch);

                                 printf("upad: %d\n", upad);
                                 printf("dpad: %d\n", dpad);
                                 printf("lpad: %d\n", lpad);
                                 printf("rpad: %d\n", rpad);
                                 printf("pupad: %d\n", pupad);
                                 printf("pdpad: %d\n", pdpad);
                                 printf("plpad: %d\n", plpad);
                                 printf("prpad: %d\n", prpad);
*/
			if(kcols != 1)
                               sp_tiled_conv_ws(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, padding,// kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
                                    kcols, kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels,// + kch,
				    B_sp_addr_start,
				   // weights + kch * out_channels + poch,
		 		    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool,
                                    weight_bank);
				else
                               sp_tiled_conv_ds(
                                    batch_size, in_dim, in_channels,
                                    out_channels, out_dim, pool_out_dim,

                                    stride, //padding, kernel_dim,

                                    pool_size, pool_stride, pool_padding,

                                    batches_,
                                    porows_, pocols_, pochs_,
                                    kchs_,

                                    lpad, rpad, upad, dpad,
                                    plpad, prpad, pupad, pdpad,

                                    input + (b*in_dim*in_dim + (irow+upad)*in_dim + (icol+lpad)) * in_channels, //+ kch,
                                    B_sp_addr_start,
                                    NULL, //weights + kch * out_channels + poch,
                                    out,
                                    bias_,

                                    act, scale, relu6_shift,
                                    no_bias, no_pool,
                                    weight_bank);

                            }
                        }
                    }
//	       }
    }
//	printf("mvin total cycles %d \n", mvin_cycles);
}

static void tiled_conv_auto_first(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

	enum tiled_matmul_type_t tiled_conv_type) {
   int weight_bank = 1;

   const bool no_pool = pool_stride == 0 || (pool_stride == 1 && pool_size == 1 && pool_padding == 0);
//    const bool no_1d = pool_stride == 0 && pool_size == 0;
    const bool no_1d =false;// no_pool; //Todo: change to 1d

    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }
    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
    int args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, in_channels};

    int acc_rows = tiled_conv_total_spad_rows(true, false,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
//   printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);

    int och_floor = (args[3]/DIM) + 1;
    while(acc_rows > ACC_ROWS){ //batch output channel, output dimension affects
		if(args[1] != 1){
			args[1]--;
			args[2]--;
		}else{
			if(args[3] >= args[0]){
				och_floor = och_floor - 1;
				args[3] = och_floor * DIM;
			}
			else args[0]--;
		}
	acc_rows = tiled_conv_total_spad_rows(true, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    }
//	printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);

    int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    while(spad_rows_weight > BANK_ROWS * weight_bank){ //tile weight first (allocate bank3 to weight)
	//input channel, output channel
	och_floor = och_floor -	1;
	args[3] = och_floor * DIM;
	spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        	stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    }

    int batches = args[0];
    int orows = args[1];
    int ocols = args[2];
    int ochs = args[3];
    int kchs = args[4];

/*
     printf("batches = %d\n", batches);
     printf("orows = %d\n", orows);
     printf("ocols = %d\n", ocols);
     printf("ochs = %d\n", ochs);
//     printf("kcols = %d\n", kernel_dim);
     printf("kchs = %d\n", kchs);
*/

   tiled_conv_first(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, padding,// kernel_dim,

        batches,
        orows, ocols, ochs,
	kernel_dim, kchs,

        input,
        weights,
        bias,
        output,

        act, scale, relu6_shift,
        no_1d ? 0 : pool_size, no_pool ? 0 : pool_stride, pool_padding,

	tiled_conv_type,
	weight_bank);


	gemmini_fence();
}

//for mobilenet depthwise conv
static void tiled_conv_auto_dw(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type) {

    const bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    const int weight_bank = 1;//((int)(kernel_dim*kernel_dim*in_channels)/BANK_ROWS)+1;
    // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
    int args[] = {batch_size, pool_out_dim, pool_out_dim, 1, 1}; //out_channel, in_channel to 1

    int acc_rows = tiled_conv_total_spad_rows(true, false,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
//   printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);

    int och_floor = (args[3]/DIM) + 1;
    while(acc_rows > ACC_ROWS){ //batch output channel, output dimension affects
 //tile output dimension
		if(args[1] != 1){
			args[1]--;
			args[2]--;
		}else{
			args[0]--;	
		}

	acc_rows = tiled_conv_total_spad_rows(true, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);	
    }

    int batches = args[0];
    int orows = args[1];
    int ocols = args[2];
    int ochs = args[3];
    int krows = kernel_dim;//args[4];
    int kcols = kernel_dim;//args[5];
    int kchs = args[4];

/*
     printf("batches = %d\n", batches);
     printf("orows = %d\n", orows);
     printf("ocols = %d\n", ocols);
     printf("ochs = %d\n", ochs);
     printf("kcols = %d\n", kernel_dim);
     printf("kchs = %d\n", kchs);
*/
    tiled_conv_dw(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, padding, kernel_dim,

        batches,
        orows, ocols,// ochs,
//        krows, kcols, kchs,

        input,
        weights,
        bias,
        output,

        act, scale, relu6_shift,
        pool_size, no_pool ? 0 : pool_stride, pool_padding,
        
        tiled_conv_type);
}

//for resnet deeper layers
//when we need to tile input channel dimension
static void tiled_conv_auto_original(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,
	
	enum tiled_matmul_type_t tiled_conv_type) {

   const int weight_bank = in_channels > 500? 3 : 2;
   const bool no_pool = pool_stride == 0 || (pool_stride == 1 && pool_size == 1 && pool_padding == 0);
    const bool no_1d = no_pool; //Todo: change to 1d
 
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }
    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    int args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, in_channels};

    int acc_rows = tiled_conv_total_spad_rows(true, false,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);

    int och_floor = (args[3]/DIM) + 1;
    int kch_floor = (args[4]/DIM) + 1;

    int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    while(spad_rows_weight > weight_bank * BANK_ROWS){ //tile weight first (allocate bank3 to weight)
		if(kch_floor > och_floor){
			kch_floor--;
			args[4] = kch_floor * DIM;
		}else{
			och_floor--;
			args[3] = och_floor * DIM;
		}
		spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        	stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
	}


       while(acc_rows > ACC_ROWS){ //batch output channel, output dimension affects
	   if(args[1] <= 7){
		if(args[0] > 1){
			args[0]--;
		}
		else{
			och_floor--;
			args[3] = och_floor*DIM;
		}
	   }
	   else{

		int max_val = -1;
		int max_idx = -1;
		if(args[0]*2 < args[1]){
			args[1]--;
			args[2]--;
		}
		else args[0]--;
	   }
	acc_rows = tiled_conv_total_spad_rows(true, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);	
    }
//	printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);
    int spad_rows_input = tiled_conv_total_spad_rows(false, false,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
 //   printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);


    while(spad_rows_input > BANK_ROWS*(BANK_NUM-weight_bank)){// tile input last
	//batch, input dimension, input channel	
		int max_val = -1;
		int max_idx = -1;
		for(int i = 0; i < 5; i++){
			if(args[i] > max_val && i != 3){
				if(i!=4){
					max_val = args[i];
					max_idx = i;
				}else if(kch_floor > 1){
					max_val = args[4];
					max_idx = 4;
				}
			}
		}
		if(max_idx == 4){
			kch_floor = kch_floor -1;
			args[4] = kch_floor * DIM;
		}
		else  args[max_idx]--;

		spad_rows_input = tiled_conv_total_spad_rows(false, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);	
    }


    int batches = args[0];
    int orows = args[1];
    int ocols = args[2];
    int ochs = args[3];
    int kchs = args[4];


    /*
    printf("batches = %d\n", batches);
    printf("orows = %d\n", orows);
    printf("ocols = %d\n", ocols);
    printf("ochs = %d\n", ochs);
    printf("kcols = %d\n", kernel_dim);
    printf("kchs = %d\n", kchs);
    */

    tiled_conv_original(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, padding, kernel_dim,

        batches,
        orows, ocols, ochs,
	kernel_dim, kchs,

        input,
        weights,
        bias,
        output,

        act, scale, relu6_shift,
        no_1d ? 0 : pool_size, no_pool ? 0 : pool_stride, pool_padding,

	tiled_conv_type, weight_bank);
	gemmini_fence();
}


//tiling function for deeper layers (when C is large)
static void tiled_conv_auto_largeC(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        elem_t * input,
        elem_t * weights,
        acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type) {

    const bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }
    const int weight_bank = 2; //hard-coded number of weight banks to use

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;

    // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
    int args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, in_channels};

    int acc_rows = tiled_conv_total_spad_rows(true, false,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
//   printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);


    int och_floor = (args[3]/DIM) + 1;
    int kch_floor = (args[4]/DIM) + 1;

 int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    while(spad_rows_weight > weight_bank * BANK_ROWS){ //tile weight first (allocate bank3 to weight)
	//input channel, output channel
	och_floor--;
	args[3] = och_floor * DIM;
	spad_rows_weight = tiled_conv_total_spad_rows(false, true,
       	stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    }


       while(acc_rows > ACC_ROWS){ //batch output channel, output dimension affects
		int max_val = -1;
		int max_idx = -1;
		for(int i = 0; i < 3; i++){
			if(args[i] > max_val){
				max_val = args[i];
				max_idx = i;
			}
		}

		args[max_idx]--;
	acc_rows = tiled_conv_total_spad_rows(true, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);	
    }
//	printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);
    int spad_rows_input = tiled_conv_total_spad_rows(false, false,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);

    while(spad_rows_input > BANK_ROWS*(BANK_NUM-weight_bank)){// tile input last
	//batch, input dimension	
		int max_val = -1;
		int max_idx = -1;
		for(int i = 0; i < 3; i++){
			if(args[i] > max_val){
				max_val = args[i];
				max_idx = i;
			}
		}
		args[max_idx]--;

	spad_rows_input = tiled_conv_total_spad_rows(false, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);	
    }

    int batches = args[0];
    int orows = args[1];
    int ocols = args[2];
    int ochs = args[3];
    int kchs = args[4];
    int krows = kernel_dim;
    int kcols = kernel_dim;
/*
     printf("batches = %d\n", batches);
     printf("orows = %d\n", orows);
     printf("ocols = %d\n", ocols);
     printf("ochs = %d\n", ochs);
     printf("kcols = %d\n", kernel_dim);
     printf("kchs = %d\n", kchs);
*/


    tiled_conv(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, padding, kernel_dim,

        batches,
        orows, ocols, ochs,
        krows, kcols, kchs,

        input,
        weights,
        bias,
        output,

        act, scale, relu6_shift,
        pool_size, no_pool ? 0 : pool_stride, pool_padding,
        weight_bank, tiled_conv_type);
}

static void tiled_conv_auto(
        int batch_size, int in_dim, int in_channels,
        int out_channels, int out_dim,
        int stride, int padding, int kernel_dim,

        const elem_t * input,
        const elem_t * weights,
        const acc_t * bias,
        elem_t * output,

        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

        enum tiled_matmul_type_t tiled_conv_type) {

    const bool no_pool = pool_stride == 0;
    if (no_pool) {
        pool_size = 1;
        pool_stride = 1;
        pool_padding = 0;
    }

    int pool_out_dim = (out_dim + 2*pool_padding - pool_size) / pool_stride + 1;
    const int weight_bank = 1;
    int args[] = {batch_size, pool_out_dim, pool_out_dim, out_channels, in_channels};

    int och_floor = (args[3]/DIM) + 1;
 
    int spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    while(spad_rows_weight > BANK_ROWS * weight_bank){ //tile weight first (allocate bank3 to weight)
	//input channel, output channel
	och_floor = och_floor -	1;
	args[3] = och_floor * DIM;
	spad_rows_weight = tiled_conv_total_spad_rows(false, true,
        	stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);
    }
    
    int acc_rows = tiled_conv_total_spad_rows(true, false,
		stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);

    while(acc_rows > ACC_ROWS){ //batch output channel, output dimension affects
 //tile output dimension
	args[1]--;
	args[2]--;

	acc_rows = tiled_conv_total_spad_rows(true, false, stride, args[0], args[1], args[2], args[3], kernel_dim, kernel_dim, args[4], pool_size, pool_stride);	
    }
//	printf("batch: %d, out_dim: %d, out_channel: %d, in_channel: %d \n", args[0], args[1], args[3], args[4]);

    int batches = args[0];
    int orows = args[1];
    int ocols = args[2];
    int ochs = args[3];
    int krows = kernel_dim;//args[4];
    int kcols = kernel_dim;//args[5];
    int kchs = args[4];

/*
     printf("batches = %d\n", batches);
     printf("orows = %d\n", orows);
     printf("ocols = %d\n", ocols);
     printf("ochs = %d\n", ochs);
     printf("kcols = %d\n", kernel_dim);
     printf("kchs = %d\n", kchs);
*/
    tiled_conv(
        batch_size, in_dim, in_channels,
        out_channels, out_dim,
        stride, padding, kernel_dim,

        batches,
        orows, ocols, ochs,
        krows, kcols, kchs,

        input,
        weights,
        bias,
        output,

        act, scale, relu6_shift,
        pool_size, no_pool ? 0 : pool_stride, pool_padding,
        
        weight_bank, tiled_conv_type);
}

static void resadd_cpu(const size_t I, const size_t J,
        const scale_t A_scale,
        const scale_t B_scale,
        const acc_scale_t C_scale,
        const elem_t * A,
        const elem_t * B,
        elem_t * C,
        bool relu) {

	const int minimum = relu ? 0 : elem_t_min;

    for (size_t i = 0; i < I; i++) {
        for (size_t j = 0; j < J; j++) {
            const elem_t * a = A + i * J + j;
            const elem_t * b = B + i * J + j;
            elem_t * c = C + i * J + j;

            acc_t result = MVIN_SCALE(*a, A_scale) + MVIN_SCALE(*b, B_scale);
            result = ACC_SCALE(result, C_scale);
            result = result > elem_t_max ? elem_t_max :
                (result < minimum ? minimum : result);

            *c = result;
        }
    }
}

static void sp_tiled_resadd(const size_t I, const size_t J,
        const scale_t A_scale,
        const scale_t B_scale,
        const elem_t * A, const elem_t * B, elem_t * C,
        size_t A_row_stride, size_t B_row_stride, size_t C_row_stride,
        bool relu) {

    // Use the new mvin2 command to overlap mvin A, mvin B, and mvout C

    size_t blocks = (J/DIM + (J % DIM != 0));
    if (blocks > MAX_BLOCK_LEN) blocks = MAX_BLOCK_LEN;

    const uint32_t D_sp_addr_start = 1 << (ADDR_LEN-1);
    const uint32_t C_sp_addr_start = 3 << (ADDR_LEN-2);

    const size_t rounded_up_J = (J / DIM + (J % DIM != 0)) * DIM;

    // Mvin A
    // printf("Mving A\n");
    for (size_t i = 0; i < I; i += DIM) {
        for (size_t j = 0; j < J; j += blocks * DIM) {
            const size_t cols = j + blocks*DIM <= J ? blocks*DIM : J-j;
            const size_t rows = i + DIM <= I ? DIM : I-i;

            const elem_t * const A_dram_addr = A + i * A_row_stride + j;
            const uint32_t A_sp_addr = D_sp_addr_start + i * (rounded_up_J/DIM) + j;

            gemmini_extended_mvin(A_dram_addr, A_sp_addr, cols, rows);
        }
    }

    // Mvin B
    // printf("Mving B\n");
    for (size_t i = 0; i < I; i += DIM) {
        for (size_t j = 0; j < J; j += blocks * DIM) {
            const size_t cols = j + blocks*DIM <= J ? blocks*DIM : J-j;
            const size_t rows = i + DIM <= I ? DIM : I-i;

            const elem_t * const B_dram_addr = B + i * B_row_stride + j;
            const uint32_t B_sp_addr = C_sp_addr_start + i * (rounded_up_J/DIM) + j;
            gemmini_extended_mvin2(B_dram_addr, B_sp_addr, cols, rows);
        }
    }

    // Mvout C from accumulator
    // printf("Mvout C from accumulator\n");
    for (size_t i = 0; i < I; i += DIM) {
        for (size_t j = 0; j < J; j += blocks * DIM) {
            const size_t cols = j + blocks*DIM <= J ? blocks*DIM : J-j;
            const size_t rows = i + DIM <= I ? DIM : I-i;

            elem_t * const C_dram_addr = C + i * C_row_stride + j;
            const uint32_t C_sp_addr = D_sp_addr_start + i * (rounded_up_J/DIM) + j;
            gemmini_extended_mvout(C_dram_addr, C_sp_addr, cols, rows);
        }
    }
}

// Compute MVIN_SCALE(A, A_scale) + MVIN_SCALE(B, B_scale) = C
static void tiled_resadd(const size_t I, const size_t J,
        const size_t tile_I, const size_t tile_J,
        const scale_t A_scale,
        const scale_t B_scale,
        const acc_scale_t C_scale,
        const elem_t * A,
        const elem_t * B,
        elem_t * C,
        bool relu,
        enum tiled_matmul_type_t matadd_type,
        size_t och_divide) {

    size_t J_stride = och_divide * J;//(padding_B) ? och_divide * J + 64 : och_divide * J;
    gemmini_config_st(J_stride * sizeof(elem_t));
    gemmini_config_ex(WS, relu ? RELU : NO_ACTIVATION, 0, C_scale, 0);

    gemmini_extended4_config_ld(J_stride * sizeof(elem_t), A_scale, true, DIM, 0);
    gemmini_extended4_config_ld(J_stride * sizeof(elem_t), B_scale, true, DIM, 1);

    for (size_t i = 0; i < I; i += tile_I) {
        for (size_t j = 0; j < J; j += tile_J) {
            const size_t I_tile = i + tile_I <= I ? tile_I : I - i;
            const size_t J_tile = j + tile_J <= J ? tile_J : J - j;

            const elem_t * a = A + i * J_stride + j;
            const elem_t * b = B + i * J_stride + j;
            elem_t * c = C + i * J + j;

            sp_tiled_resadd(I_tile, J_tile,
                    A_scale, B_scale, a, b, c,
                    J_stride, J_stride, J_stride,
                    relu);
        }
    }

    gemmini_fence();
}

// Compute (A >> A_shift) + B = C
static void tiled_resadd_auto(const size_t I, const size_t J,
        const scale_t A_scale,
        const scale_t B_scale,
        const acc_scale_t C_scale,
        const elem_t * A,
        const elem_t * B,
        elem_t * C,
        bool relu,
        enum tiled_matmul_type_t matadd_type) {

    if (matadd_type == CPU) {
        resadd_cpu(I, J,
            A_scale, B_scale, C_scale, A, B, C,
            relu);
        return;
    }

    size_t tile_I = I, tile_J = J;

    // size_t total_spad_rows = 2 * (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));
    size_t total_acc_rows = (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));

    // TODO this is a very inefficient way of doing this...
    while (total_acc_rows > ACC_ROWS) {
        if (tile_I >= tile_J)
            tile_I--;
        else
            tile_J--;

        total_acc_rows = (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));
    }

    // printf("tile_I: %llu\n", tile_I);
    // printf("tile_J: %llu\n", tile_J);

    if (matadd_type == WS) {
      tiled_resadd(I, J, tile_I, tile_J,
            A_scale, B_scale, C_scale, A, B, C,
            relu, matadd_type, 1);
    } else if(matadd_type == CPU){
	  resadd_cpu(I, J, A_scale, B_scale, C_scale,
		A, B, C, relu);
    }
    else {
      printf("Unsupported type\n");
      exit(1);
    }
}
// Compute (A >> A_shift) + B = C
static void tiled_resadd_auto_loopld(const size_t I, const size_t J,
        const scale_t A_scale,
        const scale_t B_scale,
        const acc_scale_t C_scale,
        const elem_t * A,
        const elem_t * B,
        elem_t * C,
        bool relu,
        enum tiled_matmul_type_t matadd_type,
        bool skip_A, bool skip_B, bool padding_A, bool padding_B, size_t och_divide) {

    if (matadd_type == CPU) {
        resadd_cpu(I, J,
            A_scale, B_scale, C_scale, A, B, C,
            relu);
        return;
    }

    size_t tile_I = I, tile_J = J;

    // size_t total_spad_rows = 2 * (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));
    size_t total_acc_rows = (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));

    // TODO this is a very inefficient way of doing this...
    while (total_acc_rows > ACC_ROWS) {
        if (tile_I >= tile_J)
            tile_I--;
        else
            tile_J--;

        total_acc_rows = (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));
    }

    // printf("tile_I: %llu\n", tile_I);
    // printf("tile_J: %llu\n", tile_J);

    if (matadd_type == WS) {
      tiled_resadd(I, J, tile_I, tile_J,
            A_scale, B_scale, C_scale, A, B, C,
            relu, matadd_type, och_divide);
    } else if(matadd_type == CPU){
          resadd_cpu(I, J, A_scale, B_scale, C_scale,
                A, B, C, relu);
    }
    else {
      printf("Unsupported type\n");
      exit(1);
    }
}
// Compute (A >> A_shift) + B = C
static void tiled_resadd_auto_cid(size_t I, size_t J,
        const scale_t A_scale,
        const scale_t B_scale,
        const acc_scale_t C_scale,
        const elem_t * A,
        const elem_t * B,
        elem_t * C,
        bool relu,
        enum tiled_matmul_type_t matadd_type,
        bool skip_A, bool skip_B, size_t orow_divide, size_t batch_divide, size_t cid) {

    if (matadd_type == CPU) {
        resadd_cpu(I, J,
            A_scale, B_scale, C_scale, A, B, C,
            relu);
        return;
    }
	 size_t J_stride = J;
    bool row_divisible = orow_divide > 1 && (I % orow_divide == 0);
    size_t och_divide = (row_divisible) ? 1 : orow_divide; // if row is divisible, no need to divide channel
    I = (row_divisible) ? I / orow_divide : I;
    I = I/batch_divide;
    size_t tile_I = I; // divide when it is divisible
    J = J / och_divide;
    size_t tile_J = J;

//    size_t J_stride = (padding_B) ? och_divide * J + 64 : och_divide * J;
 
    int out_offset = (och_divide > 1) ? tile_J * cid : 0; // no offset if divided in row dimension
    int orow_offset = (row_divisible) ? J_stride * cid * I : 0;
    int batch_offset = (batch_divide > 1) ? cid * J_stride * I : 0;
    // size_t total_spad_rows = 2 * (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));
    size_t total_acc_rows = (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));

    // TODO this is a very inefficient way of doing this...
    while (total_acc_rows > ACC_ROWS) {
        if (tile_I >= tile_J)
            tile_I--;
        else
            tile_J--;

        total_acc_rows = (tile_I / DIM + (tile_I % DIM != 0))*DIM * (tile_J / DIM + (tile_J % DIM != 0));
    }

    // printf("tile_I: %llu\n", tile_I);
    // printf("tile_J: %llu\n", tile_J);

    if (matadd_type == WS) {
      tiled_resadd(I, J, tile_I, tile_J,
            A_scale, B_scale, C_scale, A + batch_offset + out_offset + orow_offset, B + batch_offset + orow_offset + out_offset, C + batch_offset + orow_offset + out_offset,
            relu, matadd_type, och_divide);
    } else if(matadd_type == CPU){
          resadd_cpu(I, J, A_scale, B_scale, C_scale,
                A, B, C, relu);
    }
    else {
      printf("Unsupported type\n");
      exit(1);
    }
}

static void sp_tiled_pool(
        int batch_size, int in_dim, int channels,
		  int pool_out_dim, 
        int pool_size, int pool_stride, int pool_padding,
		  int stride,

        int batches,
        int porows, int pocols, int pochs,
        int plpad, int prpad, int pupad, int pdpad,

        const elem_t * input,
        elem_t * output)
{
    const int orows = porows * pool_stride + pool_size - 1 - pupad - pdpad;
    const int ocols = pocols * pool_stride + pool_size - 1 - plpad - prpad;
    const int ochs = pochs;

    int D_sp_addr_row = (D_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;
    int C_sp_addr_row = (C_sp_addr_row + ACC_ROWS / 2) % ACC_ROWS;

    const uint32_t D_sp_addr_start = (1 << (ADDR_LEN - 1)) + D_sp_addr_row;
    const uint32_t C_sp_addr_start = (3 << (ADDR_LEN - 2)) + C_sp_addr_row;
    gemmini_extended_config_st(stride * sizeof(elem_t), pool_stride, pool_size, pool_out_dim, porows, pocols, orows, ocols, pupad, plpad);
    gemmini_extended4_config_ld(stride * sizeof(elem_t), MVIN_SCALE_IDENTITY, true, batches * orows * ocols, 2);

  //  gemmini_extended4_config_ld(J_stride * sizeof(elem_t), B_scale, true, DIM, 1);


        const int max_ochs_per_mvin = ochs < MAX_BLOCK_LEN_ACC * DIM ? ochs :
            MAX_BLOCK_LEN_ACC * DIM;

	  for (int b = 0; b < batches; b++)
			for (int orow = 0; orow < orows; orow++)
				 for (int ocol = 0; ocol < ocols; ocol += DIM) {
					  const int I = ocols - ocol > DIM ? DIM : ocols - ocol;

					  for (int och = 0; och < ochs; och += max_ochs_per_mvin) {
							const int J = ochs - och > max_ochs_per_mvin ? max_ochs_per_mvin : ochs - och;

							const uint32_t D_sp_addr = D_sp_addr_start + (och / DIM) * batches * orows * ocols + b * orows * ocols + orow * ocols + ocol;

							gemmini_extended_mvin3(input + (b*in_dim*in_dim + orow*in_dim + ocol) * stride + och,
									  D_sp_addr,
									  J, I);
					  }
				 }

		for (int b = 0; b < batches; b++) {
			 for (int poch = 0; poch < pochs; poch += DIM) {
				  const int out_channels = poch + DIM >= pochs ? pochs - poch : DIM;

				  elem_t * pout = output + (b * pool_out_dim * pool_out_dim)*stride + poch;

				  const uint32_t C_sp_addr = C_sp_addr_start + (poch / DIM) * batches * orows * ocols + b * orows * ocols;

				  gemmini_extended_mvout(pout,
							 C_sp_addr,
							 out_channels, 0);
			 }
		}

}

static void tiled_pool(
        int batch_size, int in_dim, int channels,
		  int pool_out_dim,
		  int batches,
        int porows, int pocols, int pochs,
       
		  const elem_t * input,
        elem_t * pool_output,
		  
        int act, acc_scale_t scale, size_t relu6_shift,
        int pool_size, int pool_stride, int pool_padding,

		  size_t och_divide) {

	 int out_stride = channels * och_divide;
	 gemmini_config_st(out_stride * sizeof(elem_t));
    gemmini_extended_config_ex(WEIGHT_STATIONARY, RELU, 0, MVIN_SCALE_IDENTITY, 0, 1, false, false);
//	 int stride = channels*och_divide;
//    gemmini_extended4_config_ld(stride * sizeof(elem_t), MVIN_SCALE_IDENTITY, true, DIM, 0);
 
    for (int poch = 0; poch < channels; poch += pochs) {
       for (int b = 0; b < batch_size; b += batches) {
           for (int porow = 0; porow < pool_out_dim; porow += porows) {
               const int orow = porow * pool_stride - pool_padding;
	       const int orow_floored = orow < 0 ? 0 : orow;          
	       for (int pocol = 0; pocol < pool_out_dim; pocol += pocols) {
                   const int ocol = pocol * pool_stride - pool_padding;
		   const int ocol_floored = ocol < 0 ? 0 : ocol;
           
                   elem_t * out = pool_output + (b*pool_out_dim*pool_out_dim + porow*pool_out_dim + pocol) * out_stride + poch;
		   const elem_t * in = input + (b*in_dim*in_dim + orow_floored*in_dim + ocol_floored) * out_stride + poch;

		 //printf("batch: %d, poch: %d, porow: %d, pocol: %d, krow: %d, kcol: %d, kch: %d \n", b, poch, porow, pocol, krow, kcol, kch);
		  const int batches_ = batch_size - b > batches ? batches : batch_size - b;
		  const int porows_ = pool_out_dim - porow > porows ? porows : pool_out_dim - porow;
		  const int pocols_ = pool_out_dim - pocol > pocols ? pocols : pool_out_dim - pocol;
		  const int pochs_ = channels - poch > pochs ? pochs : channels - poch;
		  const int ocols_ = pocols_ * pool_stride + pool_size - 1;
		  const int orows_ = porows_ * pool_stride + pool_size - 1;

		  const int plpad = ocol < 0 ? -ocol : 0;
		  const int prpad = ocol + ocols_ > in_dim ? ocol + ocols_ - in_dim : 0;
		  const int pupad = orow < 0 ? -orow : 0;
		  const int pdpad = orow + orows_ > in_dim ? orow + orows_ - in_dim : 0;

		 sp_tiled_pool(
			batch_size, in_dim, channels,
			pool_out_dim,
			pool_size, pool_stride, pool_padding,
			out_stride,

			batches_,
			porows_, pocols_, pochs_,
			plpad, prpad, pupad, pdpad,

			in,
			out);
		} 
            }
        }
    }
}
// pooling using Gemmini DMA
static void tiled_pool_auto_cid(size_t batch_size, size_t channels, size_t in_dim,
		size_t pool_out_dim, size_t stride,
		size_t pool_size, size_t pool_stride, size_t pool_padding,
		const elem_t * A,
      elem_t * C,
      size_t och_divide, size_t batch_divide, size_t cid) {

	 bool relu = true;
	 //int stride = channels;
    batch_size = batch_size/batch_divide;
    channels = channels / och_divide;
    //int pool_out_dim = (in_dim + 2*pool_padding - pool_size) / pool_stride + 1;
	 int batch_in_offset = (batch_divide > 1) ? batch_size*in_dim*in_dim*stride*cid : 0;
	 int batch_out_offset = (batch_divide > 1) ? batch_size*pool_out_dim*pool_out_dim*stride*cid : 0; // not dividing in out_channel dimension
 	 const int out_offset = (och_divide > 1) ? channels * cid : 0;
	 
	 // int args[] = {batch_size, porows, pocols, pochs, krows, kcols, kchs};
    int args[] = {batch_size, pool_out_dim, pool_out_dim, channels, 1, 1, DIM};
    const int max_args[] = {batch_size, pool_out_dim, pool_out_dim, channels, 1, 1, DIM};

    const int orows_idx = 1;
    const int ocols_idx = 2;
    const int channels_idx = 3;
   // We divide by 2 for the sake of double-buffering
    const int max_spad_rows = (BANK_NUM*BANK_ROWS / 2);
    const int max_acc_rows = (ACC_ROWS / 2);
	 const int dilation = 1;
    int acc_rows = tiled_conv_total_spad_rows_A_stride(true,
        stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);

    while (acc_rows > max_acc_rows) {
        int max_val = -1;
        int max_idx = -1;

        for (size_t i = 0; i < sizeof(args)/sizeof(args[0]); i++) {
            // We avoid reducing ocols when possible to keep the spatial array fully utilized
			  if(i == channels_idx && args[i] > MAX_BLOCK_LEN * DIM){
				  args[i] = (args[i] - 1) / (MAX_BLOCK_LEN*DIM) * DIM;
				  break;
			  }
			  else if(i == 0 && args[i] > 1){
				  args[i] = 1;
				  break;
			  } // for batch
           else if ((i!=channels_idx) &&  args[i] > max_val) {
                max_val = args[i];
                max_idx = i;
           }
        }
		  args[max_idx]--;
        acc_rows = tiled_conv_total_spad_rows_A_stride(true,
            stride, dilation, args[0], args[1], args[2], args[3], args[4], args[5], args[6], pool_size, pool_stride);
    }

    // printf("tile_I: %llu\n", tile_I);
    // printf("tile_J: %llu\n", tile_J);

      tiled_pool(batch_size, in_dim, channels, pool_out_dim,
				args[0], args[1], args[2], args[3],
            A + batch_in_offset + out_offset, C + batch_out_offset + out_offset,	
				RELU, MVIN_SCALE_IDENTITY, 0,
				pool_size, pool_stride, pool_padding,
				och_divide);
   
}

static void global_average_cpu(const elem_t * input, elem_t * output,
    int batches, int channels, int dim) {
  const int count = dim * dim;

  for (int batch = 0; batch < batches; batch++) {
    for (int channel = 0; channel < channels; channel++) {
      acc_t sum = 0;
      for (int row = 0; row < dim; row++) {
        for (int col = 0; col < dim; col++) {
          size_t pixel = batch * dim * dim + row * dim + col;

          sum += input[pixel * channels + channel];
        }
      }

      output[batch * channels + channel] = (sum + count/2) / count;
    }
  }
}

static void sp_tiled_global_average(const elem_t * input, elem_t * output,
    int batches, int channels, int dim, int channel_tile_size) {
  const uint32_t C_acc_addr_start = ((uint32_t)1 << 31);

  size_t blocks = channel_tile_size/DIM + (channel_tile_size % DIM != 0);
  if (blocks > MAX_BLOCK_LEN) blocks = MAX_BLOCK_LEN;

  for (int channel = 0; channel < channel_tile_size; channel += blocks*DIM) {
    for (int row = 0; row < dim; row++) {
      for (int col = 0; col < dim; col++) {
        const elem_t * in = input +
          (row * dim + col) * channels +
          channel;

        const uint32_t acc_addr_start = C_acc_addr_start |
          ((row != 0 || col != 0) << 30);

        const uint32_t acc_addr = acc_addr_start + channel / DIM;

        const size_t cols = channel + blocks*DIM <= channel_tile_size ?
          blocks*DIM : channel_tile_size - channel;

        const size_t rows = 1;

        gemmini_extended_mvin(in, acc_addr, cols, rows);
      }
    }
  }

  for (int channel = 0; channel < channel_tile_size; channel += DIM) {
    elem_t * out = output + channel;

    const uint32_t acc_addr = C_acc_addr_start + channel / DIM;

    const size_t cols = channel + DIM <= channel_tile_size ?
      DIM : channel_tile_size - channel;

    const size_t rows = 1; // TODO we should move out more than just one row here

    gemmini_extended_mvout(out, acc_addr, cols, rows);
  }
}

static void tiled_global_average(const elem_t * input, elem_t * output,
    int batches, int channels, int dim,
    int channel_tile_size) {

  gemmini_extended4_config_ld(DIM*sizeof(elem_t), MVIN_SCALE_IDENTITY, true, 1, 0);
  gemmini_config_ex(0, NO_ACTIVATION, 0, 1.0 / (dim*dim), 0);
  gemmini_config_st(0);

  for (int batch = 0; batch < batches; batch++) {
    for (int channel = 0; channel < channels; channel += channel_tile_size) {
      const int tile_size = channel + channel_tile_size <= channels ?
        channel_tile_size : channels - channel;

      sp_tiled_global_average(input + batch * dim * dim * channels + channel,
          output + batch * channels + channel,
          batches, channels, dim, tile_size);
    }
  }
}

static void tiled_global_average_auto(const elem_t * input, elem_t * output,
    int batches, int channels, int dim,
    enum tiled_matmul_type_t type) {
  if (type == CPU) {
    return global_average_cpu(input, output, batches, channels, dim);
  }

  int channel_tile_size = channels;

  int acc_rows = channel_tile_size / DIM + (channel_tile_size % DIM != 0);
  while (acc_rows > ACC_ROWS) {
    channel_tile_size--;
    acc_rows = channel_tile_size / DIM + (channel_tile_size % DIM != 0);
  }

  tiled_global_average(input, output, batches, channels, dim,
      channel_tile_size);
}


#undef abs

#endif // SRC_MAIN_C_GEMMINI_H
