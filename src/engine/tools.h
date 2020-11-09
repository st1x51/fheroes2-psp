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
#ifndef H2TOOLS_H
#define H2TOOLS_H

#include <string>
#include "localevent.h"
#include "types.h"

namespace String
{
    std::string Trim(std::string);
    std::string Lower(std::string);
    std::string Upper(std::string);
    void AddInt(std::string &str, int value);
    int  ToInt(const std::string &str);
    std::string Double(double, u8);

    void Replace(std::string &, const char*, const char *);
    void Replace(std::string &, const char*, const std::string &);
    void Replace(std::string &, const char*, int);

    void AppendKey(std::string &, KeySym, u16);

    // from SDL_ttf
    std::vector<u16> UTF8_to_UNICODE(const std::string &);
    std::string UNICODE_to_UTF8(const std::vector<u16> &);

    std::string GetTime(void);
}

int Sign(int);
KeySym KeySymFromChar(char);
bool PressIntKey(u32 min, u32 max, u32 & result);

std::string GetDirname(const std::string &);
std::string GetBasename(const std::string &);

u32 GetMemoryUsage(void);

bool SaveMemToFile(const std::vector<u8> &, const std::string &);
bool LoadFileToMem(std::vector<u8> &, const std::string &);
bool FilePresent(const std::string &);

std::string EncodeString(const std::string & str, const char* charset);

void ToolsSrcRectFixed(Rect &, s16 &, s16 &, const u16, const u16, const Rect &);

int	sdl_putenv(const char *name, const char *value, int overwrite);
char*	sdl_getenv(const char* env);

#endif
