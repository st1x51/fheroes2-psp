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

#ifndef H2ARMY_H
#define H2ARMY_H

#include <string>
#include <vector>
#include "bitmodes.h"
#include "heroes_base.h"
#include "army_troop.h"

class Castle;
class Heroes;
namespace Maps { class Tiles; }
namespace Battle2 { class Armies; }

namespace Army
{
    enum format_t
    {
	FORMAT_GROUPED = 0,
	FORMAT_SPREAD  = 1
    };

    std::string SizeString(u32);
    std::string TroopSizeString(const Troop &);

    // 0: fight, 1: free join, 2: join with gold, 3: flee
    u8 GetJoinSolution(const Heroes &, const Maps::Tiles &, u32 &, s32 &);

    typedef std::vector<Troop> Troops;

    class army_t
    {
	public:
	    army_t(HeroBase* s = NULL);
	    army_t(const Maps::Tiles &);

	    void	UpgradeMonsters(const Monster &);
	    void	Clear(void);
	    void	Reset(bool = false);	// reset: soft or hard

	    void	BattleExportKilled(army_t &) const;
	    void	BattleInit(void);
	    void	BattleQuit(void);
	    u32		BattleKilled(void) const;

	    Troop &	FirstValid(void);
	    Troop &	At(u8);
	    Troop &	GetWeakestTroop(void);

	    const Troop &	At(u8) const;
	    const Troop &	GetSlowestTroop(void) const;

	    u8		GetRace(void) const;
	    u8		GetColor(void) const;
	    u8          GetControl(void) const;

	    void        SetColor(u8);

	    u8		Size(void) const;
	    u8		GetCount(void) const;
	    u8		GetUniqCount(void) const;
	    u32		MonsterCounts(const Monster &) const;
	    s8		GetMorale(void) const;
	    s8		GetLuck(void) const;
	    s8		GetMoraleModificator(std::string *strs) const;
	    s8		GetLuckModificator(std::string *strs) const;
	    u32		CalculateExperience(void) const;
	    u32		ActionToSirens(void);
	    u32		GetSurrenderCost(void) const;

	    u16		GetAttack(bool = true) const;
	    u16		GetDefense(bool = true) const;
	    u32		GetHitPoints(void) const;
	    u32		GetDamageMin(void) const;
	    u32		GetDamageMax(void) const;
	    u32		GetStrength(void) const;

	    bool	isValid(void) const;
	    bool	HasMonster(const Monster &) const;
	    bool	JoinTroop(const Troop & troop);
	    bool	JoinTroop(const Monster & mon, const u32 count);
	    bool 	JoinArmy(Army::army_t &);
	    bool	CanJoinTroop(const Monster & mon) const;
	    bool	CanJoinArmy(const Army::army_t &) const;
	    bool	StrongerEnemyArmy(const army_t &) const;
	    bool	AllTroopsIsRace(u8) const;

	    void	JoinStrongestFromArmy(army_t &);
            void	KeepOnlyWeakestTroops(army_t &);
	    void	UpgradeTroops(const Castle &);
	    void	ArrangeForBattle(void);

	    std::string String(void) const;

	    const HeroBase* GetCommander(void) const;
	    HeroBase*       GetCommander(void);
	    void            SetCommander(HeroBase*);

	    void        SetCombatFormat(format_t);
	    u8          GetCombatFormat(void) const;

	    Troops	Optimize(void) const;

	    s8		GetTroopIndex(const Troop &) const;

	protected:
	    friend class Battle2::Armies;

	    army_t(const army_t &);
	    army_t & operator= (const army_t &);

	    void	Import(army_t &);
	    void	Import(const Troops &);

	    Troops		troops;
	    HeroBase*		commander;
	    u8			combat_format;
	    u8			color;
    };

    void DrawMons32Line(const army_t &, s16, s16, u16, u8 = 0, u8 = 0);
    void DrawMons32LineWithScoute(const army_t &, s16, s16, u16, u8, u8, u8);
}

#endif
