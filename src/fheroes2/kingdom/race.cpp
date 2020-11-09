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

#include "engine.h"
#include "race.h"

const char* Race::String(u8 race)
{
    const char* str_race[] = { _("Knight"), _("Barbarian"), _("Sorceress"),
	_("Warlock"), _("Wizard"), _("Necromancer"), _("Multi"), "Random", "Neutral" };

    switch(race)
    {
        case Race::KNGT: return str_race[0];
        case Race::BARB: return str_race[1];
        case Race::SORC: return str_race[2];
        case Race::WRLK: return str_race[3];
        case Race::WZRD: return str_race[4];
        case Race::NECR: return str_race[5];
        case Race::MULT: return str_race[6];
        case Race::RAND: return str_race[7];
	case Race::NONE: return str_race[8];
        default: break;
    }

    return str_race[8];
}

u8 Race::Rand(void)
{
    switch(Rand::Get(1, 6))
    {
        case 1: return Race::KNGT;
        case 2: return Race::BARB;
        case 3: return Race::SORC;
        case 4: return Race::WRLK;
        case 5: return Race::WZRD;
	default: break;
    }

    return Race::NECR;
}

u8 Race::Get(u8 index)
{
    switch(index)
    {
	case KNGT:	return KNGT;
	case BARB:	return BARB;
	case SORC:	return SORC;
	case WRLK:	return WRLK;
	case WZRD:	return WZRD;
	case NECR:	return NECR;
	case MULT:	return MULT;
	case RAND:	return RAND;
	default: break;
    }

    return Race::NONE;
}
