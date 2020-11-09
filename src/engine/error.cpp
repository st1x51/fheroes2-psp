/***************************************************************************
 *   Copyright (C) 2009 by Andrey Afletdinov <fheroes2@gmail.com>          *
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

#include <stdexcept>
#include <iostream>
#include "error.h"
#ifdef ANDROID
#include <android/log.h>
#endif

/* exception */
void Error::Except(const char* func, const char* message)
{
#ifndef ANDROID
    std::cerr << "exception: " << func << ": " << message << std::endl;
    throw Exception();
#else
 __android_log_print(ANDROID_LOG_FATAL, "fheroes2", "======");
 __android_log_print(ANDROID_LOG_FATAL, "fheroes2", "Error: %s: %s", func, message);
 __android_log_print(ANDROID_LOG_FATAL, "fheroes2", "======");
#endif
}
