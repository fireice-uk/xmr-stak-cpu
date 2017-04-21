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

#pragma once

#ifdef __GNUC__
#   include <x86intrin.h>
#else
#   include <intrin.h>
#endif // __GNUC__

namespace xmr_stak
{
    inline
    unsigned long long
    __attribute__((target ("bmi2")))
    mulx_u64 (unsigned long long x, unsigned long long y, unsigned long long *hi)
    {
        return _mulx_u64(x,y,hi);
    }
}
