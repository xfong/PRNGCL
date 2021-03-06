/******************************************************************************
 * @file     PRNGCL.cpp
 * @author   Vadim Demchik <vadimdi@yahoo.com>
 * @version  1.1.2
 *
 * @brief    [PRNGCL library]
 *           Library of pseudo-random number generators for Monte Carlo simulations on GPUs
 *
 *
 * @section  CREDITS
 *
 *   Vadim Demchik,
 *   "Pseudorandom Numbers Generation for Monte Carlo Simulations on GPUs: OpenCL Approach",
 *   ch.12 in book "Numerical Computations with GPUs", pp 245-271,
 *   doi: 10.1007/978-3-319-06548-9_12, Springer International Publishing, 2014
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

#include "PRNGCL.h"
#include "examples/prngcl_example_pi.h"

int main(int argc, char ** argv)
{
    HGPU_GPU_test(argc,argv);

//    HGPU_GPU_example_pi(argc,argv);

    return 0;
}
