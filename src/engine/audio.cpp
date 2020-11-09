/***************************************************************************
 *   Copyright (C) 2008 by Andrey Afletdinov <fheroes2@gmail.com>          *
 *                                                                         *
 *   Part of the Free Heroes2 Engine:                                      *
 *   http://sourceforge.net/projects/fheroes2                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "system.h"
#include "audio.h"

namespace Audio
{
    static Spec hardware;
}

Audio::Spec::Spec()
{
    freq = 0;
    format = 0;
    channels = 0;
    silence = 0;
    samples = 0;
    size = 0;
    callback = NULL;
    userdata = NULL;
}

Audio::CVT::CVT()
{
    needed = 0;
    src_format = 0;
    dst_format = 0;
    rate_incr = 0;
    buf = NULL;
    len = 0;
    len_cvt = 0;
    len_mult = 0;
    len_ratio = 0;
    filters[0] = NULL;
    filters[1] = NULL;
    filters[2] = NULL;
    filters[3] = NULL;
    filters[4] = NULL;
    filters[5] = NULL;
    filters[6] = NULL;
    filters[7] = NULL;
    filters[8] = NULL;
    filters[9] = NULL;
    filter_index = 0;
}

bool Audio::CVT::Build(const Audio::Spec & src, const Audio::Spec & dst)
{
    if(1 == SDL_BuildAudioCVT(this, src.format, src.channels, src.freq, dst.format, dst.channels, dst.freq)) return true;

    ERROR(SDL_GetError());
    return false;
}

bool Audio::CVT::Convert(void)
{
    if(0 == SDL_ConvertAudio(this)) return true;
    
    ERROR(SDL_GetError());
    return false;
}

Audio::Spec & Audio::GetHardwareSpec(void)
{
    return hardware;
}
void memcpy_vfpu( void* dst, const void* src, size_t size )
{
    //less than 16bytes or there is no 32bit alignment -> not worth optimizing
	if( ((u32)src&0x3) != ((u32)dst&0x3) && (size<16) )
    {
        memcpy( dst, src, size );
        return;
    }

    u8* src8 = (u8*)src;
    u8* dst8 = (u8*)dst;

	// Align dst to 4 bytes or just resume if already done
	while( ((u32)dst8&0x3)!=0 )
	{
		*dst8++ = *src8++;
		size--;
	}

	u32 *dst32=(u32*)dst8;
	u32 *src32=(u32*)src8;

	// Align dst to 16 bytes or just resume if already done
	while( ((u32)dst32&0xF)!=0 )
	{
		*dst32++ = *src32++;
		size -= 4;
	}

	dst8=(u8*)dst32;
	src8=(u8*)src32;

	if( ((u32)src8&0xF)==0 )	//Both src and dst are 16byte aligned
	{
		while (size>63)
		{
			asm(".set	push\n"					// save assembler option
				".set	noreorder\n"			// suppress reordering
				"lv.q c000, 0(%1)\n"
				"lv.q c010, 16(%1)\n"
				"lv.q c020, 32(%1)\n"
				"lv.q c030, 48(%1)\n"
				"sv.q c000, 0(%0)\n"
				"sv.q c010, 16(%0)\n"
				"sv.q c020, 32(%0)\n"
				"sv.q c030, 48(%0)\n"
				"addiu  %2, %2, -64\n"			//size -= 64;
				"addiu	%1, %1, 64\n"			//dst8 += 64;
				"addiu	%0, %0, 64\n"			//src8 += 64;
				".set	pop\n"					// restore assembler option
				:"+r"(dst8),"+r"(src8),"+r"(size)
				:
				:"memory"
				);
		}

		while (size>15)
		{
			asm(".set	push\n"					// save assembler option
				".set	noreorder\n"			// suppress reordering
				"lv.q c000, 0(%1)\n"
				"sv.q c000, 0(%0)\n"
				"addiu  %2, %2, -16\n"			//size -= 16;
				"addiu	%1, %1, 16\n"			//dst8 += 16;
				"addiu	%0, %0, 16\n"			//src8 += 16;
				".set	pop\n"					// restore assembler option
				:"+r"(dst8),"+r"(src8),"+r"(size)
				:
				:"memory"
				);
		}
	}
	else 	//At least src is 4byte and dst is 16byte aligned
    {
		while (size>63)
		{
			asm(".set	push\n"					// save assembler option
				".set	noreorder\n"			// suppress reordering
				"ulv.q c000, 0(%1)\n"
				"ulv.q c010, 16(%1)\n"
				"ulv.q c020, 32(%1)\n"
				"ulv.q c030, 48(%1)\n"
				"sv.q c000, 0(%0)\n"
				"sv.q c010, 16(%0)\n"
				"sv.q c020, 32(%0)\n"
				"sv.q c030, 48(%0)\n"
				"addiu  %2, %2, -64\n"			//size -= 64;
				"addiu	%1, %1, 64\n"			//dst8 += 64;
				"addiu	%0, %0, 64\n"			//src8 += 64;
				".set	pop\n"					// restore assembler option
				:"+r"(dst8),"+r"(src8),"+r"(size)
				:
				:"memory"
				);
		}

		while (size>15)
		{
			asm(".set	push\n"					// save assembler option
				".set	noreorder\n"			// suppress reordering
				"ulv.q c000, 0(%1)\n"
				"sv.q c000, 0(%0)\n"
				"addiu  %2, %2, -16\n"			//size -= 16;
				"addiu	%1, %1, 16\n"			//dst8 += 16;
				"addiu	%0, %0, 16\n"			//src8 += 16;
				".set	pop\n"					// restore assembler option
				:"+r"(dst8),"+r"(src8),"+r"(size)
				:
				:"memory"
				);
		}
    }

	// Most copies are completed with the VFPU, so fast out
	if (size == 0)
		return;

	dst32=(u32*)dst8;
	src32=(u32*)src8;

	//Copy remaning 32bit...
	while( size>3 )
	{
		*dst32++ = *src32++;
		size -= 4;
	}

	dst8=(u8*)dst32;
	src8=(u8*)src32;

	//Copy remaning bytes if any...
	while( size>0 )
    {
        *dst8++ = *src8++;
        size--;
    }
}
#define memcpy 	memcpy_vfpu
