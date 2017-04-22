/*
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
  *
  */


#include "cryptonight.h"
#include "cryptonight_aesni.h"
#include "cryptonight_hash.h"
#include <memory.h>
#include <stdio.h>



template<size_t ITERATIONS, size_t MEM, bool SOFT_AES, bool PREFETCH>
void
#ifdef __GNUC__
__attribute__((target ("bmi2")))
#endif
cryptonight_hash_bmi2(const void* input, size_t len, void* output, cryptonight_ctx* ctx0)
{
    cryptonight_hash<ITERATIONS, MEM, SOFT_AES, PREFETCH>(
       input, len, output, ctx0);
}

// This lovely creation will do 2 cn hashes at a time. We have plenty of space on silicon
// to fit temporary vars for two contexts. Function will read len*2 from input and write 64 bytes to output
// We are still limited by L3 cache, so doubling will only work with CPUs where we have more than 2MB to core (Xeons)
template<size_t ITERATIONS, size_t MEM, bool SOFT_AES, bool PREFETCH>
#ifdef __GNUC__
__attribute__((target ("bmi2")))
#endif
void cryptonight_double_hash_bmi2(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx0, cryptonight_ctx* __restrict ctx1)
{
    cryptonight_double_hash<ITERATIONS, MEM, SOFT_AES, PREFETCH>(
        input, len, output, ctx0, ctx1);
}


template void cryptonight_double_hash_bmi2<0x80000, MEMORY, false, false>(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx0, cryptonight_ctx* __restrict ctx1);
template void cryptonight_double_hash_bmi2<0x80000, MEMORY, false, true>(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx0, cryptonight_ctx* __restrict ctx1);
template void cryptonight_double_hash_bmi2<0x80000, MEMORY, true, false>(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx0, cryptonight_ctx* __restrict ctx1);
template void cryptonight_double_hash_bmi2<0x80000, MEMORY, true, true>(const void* input, size_t len, void* output, cryptonight_ctx* __restrict ctx0, cryptonight_ctx* __restrict ctx1);
template void cryptonight_hash_bmi2<0x80000, MEMORY, false, false>(const void* input, size_t len, void* output, cryptonight_ctx* ctx0);
template void cryptonight_hash_bmi2<0x80000, MEMORY, false, true>(const void* input, size_t len, void* output, cryptonight_ctx* ctx0);
template void cryptonight_hash_bmi2<0x80000, MEMORY, true, false>(const void* input, size_t len, void* output, cryptonight_ctx* ctx0);
template void cryptonight_hash_bmi2<0x80000, MEMORY, true, true>(const void* input, size_t len, void* output, cryptonight_ctx* ctx0);
