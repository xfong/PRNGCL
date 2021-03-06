/******************************************************************************
 * @file     prngcl_constant.cpp
 * @author   Vadim Demchik <vadimdi@yahoo.com>
 * @version  1.1.2
 *
 * @brief    [PRNGCL library]
 *           contains implementation, description and initialization procedures of
 *           constant "pseudo-random number generator",
 *           which produces series of constant values for debugging purposes
 *
 *
 * @section  LICENSE
 *
 * Copyright (c) 2013-2015 Vadim Demchik
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *    Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 *****************************************************************************/

#include "hgpu_prng.h"

#define HGPU_PRNG_CL_FILE         PRNGCL_ROOT_PATH "prngcl_constant.cl"

#define HGPU_PRNG_CONSTANT_m      (4294967296.0)  /**< devider for production a floating-point PRNs on integer value. */
#define HGPU_PRNG_CONSTANT_min    0               /**< minimal possible integer value, that could be produced by PRNG. */
#define HGPU_PRNG_CONSTANT_max    4294967295      /**< maximal possible integer value, that could be produced by PRNG (=2^32-1). */
#define HGPU_PRNG_CONSTANT_min_FP (0.0)           /**< minimal possible floating value, that could be produced by PRNG. */
#define HGPU_PRNG_CONSTANT_max_FP (4294967295.0/4294967296.0) /**< maximal possible floating value, that could be produced by PRNG. */
#define HGPU_PRNG_CONSTANT_k      (0.0)           /**< value for conversion of single to double precision (0 means do not convert to double). */


    typedef struct {
        unsigned int x;
    } HGPU_PRNG_CONSTANT_state_t;

            static void   HGPU_PRNG_CONSTANT_initialize(void* PRNG_state, unsigned int PRNG_randseries);
    static unsigned int   HGPU_PRNG_CONSTANT_produce_one_uint_CPU(void* PRNG_state);
          static double   HGPU_PRNG_CONSTANT_produce_one_double_CPU(void* PRNG_state);
            static void   HGPU_PRNG_CONSTANT_init_GPU(HGPU_GPU_context* context,void* PRNG_state,HGPU_PRNG_parameters* PRNG_parameters);
            static char*  HGPU_PRNG_CONSTANT_options_GPU(HGPU_GPU_context* context,void* PRNG_state,HGPU_PRNG_parameters* PRNG_parameters);
            static void   HGPU_PRNG_CONSTANT_parameters_set(void* PRNG_state,HGPU_parameter** parameters);

static void
HGPU_PRNG_CONSTANT_initialize(void* PRNG_state, unsigned int PRNG_randseries){
    HGPU_PRNG_CONSTANT_state_t* state = (HGPU_PRNG_CONSTANT_state_t*) PRNG_state;
    HGPU_PRNG_srand(PRNG_randseries);

    state->x = HGPU_PRNG_rand32bit();
}

static unsigned int
HGPU_PRNG_CONSTANT_produce_one_uint_CPU(void* PRNG_state){
    HGPU_PRNG_CONSTANT_state_t* state = (HGPU_PRNG_CONSTANT_state_t*) PRNG_state;
    return (state->x);
}

static double
HGPU_PRNG_CONSTANT_produce_one_double_CPU(void* PRNG_state){
    double y = (double) HGPU_PRNG_CONSTANT_produce_one_uint_CPU(PRNG_state);
    return (y / HGPU_PRNG_CONSTANT_m);
}

static void
HGPU_PRNG_CONSTANT_init_GPU(HGPU_GPU_context* context,void*,HGPU_PRNG_parameters* PRNG_parameters){
    size_t randoms_size             = HGPU_GPU_context_buffer_size_align(context,PRNG_parameters->instances * PRNG_parameters->samples);
    cl_float4*  PRNG_randoms        = NULL;
    cl_double4* PRNG_randoms_double = NULL;
    if (PRNG_parameters->precision==HGPU_precision_double)
        PRNG_randoms_double = (cl_double4*) calloc(randoms_size,sizeof(cl_double4));
    else
        PRNG_randoms        = (cl_float4*)  calloc(randoms_size,sizeof(cl_float4));
    if ((!PRNG_randoms_double) && (!PRNG_randoms))
        HGPU_error_message(HGPU_ERROR_NO_MEMORY,"could not allocate memory for randoms");

    unsigned int randoms_id = 0;

    if (PRNG_parameters->precision==HGPU_precision_double)
        randoms_id = HGPU_GPU_context_buffer_init(context,PRNG_randoms_double,HGPU_GPU_buffer_type_io,randoms_size,sizeof(cl_double4));
    else
        randoms_id = HGPU_GPU_context_buffer_init(context,PRNG_randoms,HGPU_GPU_buffer_type_io,randoms_size,sizeof(cl_float4));
    HGPU_GPU_context_buffer_set_name(context,randoms_id,(char*) "(CONSTANT) PRNG_randoms");

    PRNG_parameters->id_buffer_input_seeds = HGPU_GPU_MAX_BUFFERS;
    PRNG_parameters->id_buffer_seeds       = HGPU_GPU_MAX_BUFFERS;
    PRNG_parameters->id_buffer_randoms     = randoms_id;
}

static char*
HGPU_PRNG_CONSTANT_options_GPU(HGPU_GPU_context* context,void* PRNG_state,HGPU_PRNG_parameters* PRNG_parameters){
    char* result = NULL;
    if ((!context) || (!PRNG_state)) return result;
    result = (char*) calloc(HGPU_GPU_MAX_OPTIONS_LENGTH,sizeof(char));
    HGPU_PRNG_CONSTANT_state_t* state = (HGPU_PRNG_CONSTANT_state_t*) PRNG_state;
    int j = 0;
    if (PRNG_parameters->precision==HGPU_precision_double)
        j += sprintf_s(result+j,HGPU_GPU_MAX_OPTIONS_LENGTH-j,"-D CONSTANT_FP=(%1.23e)",(state->x / HGPU_PRNG_CONSTANT_m));
    else
        j += sprintf_s(result+j,HGPU_GPU_MAX_OPTIONS_LENGTH-j,"-D CONSTANT_FP=(%1.16f)",(state->x / HGPU_PRNG_CONSTANT_m));
    return result;
}

static void
HGPU_PRNG_CONSTANT_parameters_set(void* PRNG_state,HGPU_parameter** parameters){
    if ((!parameters) || (!PRNG_state)) return;
    HGPU_PRNG_CONSTANT_state_t* state = (HGPU_PRNG_CONSTANT_state_t*) PRNG_state;
    HGPU_parameter* parameter = HGPU_parameters_get_by_name(parameters,(char*) HGPU_PARAMETER_PRNG_SEED1);
    if (parameter)
        (*state).x = parameter->value_integer;
}

static const HGPU_PRNG_description
HGPU_PRNG_CONSTANT_description = {
        "CONSTANT",                                   // name
        32,                                           // bitness
        HGPU_PRNG_output_type_double,                 // PRNG GPU output type
        HGPU_PRNG_CONSTANT_min,                       // PRNG_min_uint_value
        HGPU_PRNG_CONSTANT_max,                       // PRNG_max_uint_value
        HGPU_PRNG_CONSTANT_min_FP,                    // PRNG_min_double_value
        HGPU_PRNG_CONSTANT_max_FP,                    // PRNG_max_double_value
        HGPU_PRNG_CONSTANT_k,                         // PRNG_k_value
        sizeof(HGPU_PRNG_CONSTANT_state_t),           // size of PRNG state
        &HGPU_PRNG_CONSTANT_initialize,               // PRNG initialization
        &HGPU_PRNG_CONSTANT_parameters_set,           // PRNG additional parameters initialization
        &HGPU_PRNG_CONSTANT_produce_one_uint_CPU,     // PRNG production one unsigned integer
        &HGPU_PRNG_CONSTANT_produce_one_double_CPU,   // PRNG production one double
        //
        &HGPU_PRNG_CONSTANT_init_GPU,                 // PRNG init for GPU procedure
        &HGPU_PRNG_CONSTANT_options_GPU,              // PRNG additional compilation options
        HGPU_PRNG_CL_FILE,                            // PRNG source codes
        NULL,                                         // PRNG init kernel
        "constant_series"                             // PRNG production kernel
};

const HGPU_PRNG_description*  HGPU_PRNG_CONSTANT  = &HGPU_PRNG_CONSTANT_description;

#undef HGPU_PRNG_CL_FILE
