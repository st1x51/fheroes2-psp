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

#include <cmath>
#include <algorithm>
#include <functional>
#include "artifact.h"
#include "world.h"
#include "castle.h"
#include "settings.h"
#include "agg.h"
#include "speed.h"
#include "luck.h"
#include "race.h"
#include "morale.h"
#include "monster.h"
#include "payment.h"
#include "profit.h"
#include "cursor.h"
#include "kingdom.h"
#include "visit.h"
#include "battle2.h"
#include "heroes.h"
#include "localclient.h"
#include "game_focus.h"
#include "game_interface.h"
#include "game_static.h"
#include "ai.h"

// heroes_action.cpp
void PlayPickupSound(void);

const char* Heroes::GetName(heroes_t id)
{
    const char* names[] = {
	// knight
	_("Lord Kilburn"), _("Sir Gallanth"), _("Ector"), _("Gwenneth"), _("Tyro"), _("Ambrose"), _("Ruby"), _("Maximus"), _("Dimitry"),
	// barbarian
	_("Thundax"), _("Fineous"), _("Jojosh"), _("Crag Hack"), _("Jezebel"), _("Jaclyn"), _("Ergon"), _("Tsabu"), _("Atlas"),
	// sorceress
	_("Astra"), _("Natasha"), _("Troyan"), _("Vatawna"), _("Rebecca"), _("Gem"), _("Ariel"), _("Carlawn"), _("Luna"),
	// warlock
	_("Arie"), _("Alamar"), _("Vesper"), _("Crodo"), _("Barok"), _("Kastore"), _("Agar"), _("Falagar"), _("Wrathmont"),
	// wizard
	_("Myra"), _("Flint"), _("Dawn"), _("Halon"), _("Myrini"), _("Wilfrey"), _("Sarakin"), _("Kalindra"), _("Mandigal"),
	// necromant
	_("Zom"), _("Darlana"), _("Zam"), _("Ranloo"), _("Charity"), _("Rialdo"), _("Roxana"), _("Sandro"), _("Celia"),
	// campains
	_("Roland"), _("Lord Corlagon"), _("Sister Eliza"), _("Archibald"), _("Lord Halton"), _("Brother Bax"),
	// loyalty version
	_("Solmyr"), _("Dainwin"), _("Mog"), _("Uncle Ivan"), _("Joseph"), _("Gallavant"), _("Elderian"), _("Ceallach"), _("Drakonia"), _("Martine"), _("Jarkonas"),
	// debug
	"SandySandy", "Unknown" };

    return names[id];
}

s8 ObjectVisitedModifiersResult(u8 type, const u8* objs, u8 size, const Heroes & hero, std::string* strs)
{
    s8 result = 0;

    for(u8 ii = 0; ii < size; ++ii)
    {
    	if(hero.isVisited(objs[ii]))
    	{
    	    result += GameStatic::ObjectVisitedModifiers(objs[ii]);

    	    if(strs)
    	    {
		strs->append(MP2::StringObject(objs[ii]));
		StringAppendModifiers(*strs, GameStatic::ObjectVisitedModifiers(objs[ii]));
		strs->append("\n");
	    }
    	}
    }

    return result;
}

Heroes::heroes_t Heroes::ConvertID(u8 index)
{
    return index > UNKNOWN ? UNKNOWN : static_cast<heroes_t>(index);
}

Heroes::Heroes() : move_point_scale(-1), army(this), portrait(UNKNOWN), race(UNKNOWN), path(*this),
    direction(Direction::RIGHT), sprite_index(18), patrol_square(0)
{
}

Heroes::Heroes(heroes_t ht, u8 rc) : HeroBase(Skill::Primary::HEROES, rc), killer_color(Color::NONE), experience(0),
    move_point_scale(-1), secondary_skills(rc), army(this), hid(ht), portrait(ht), race(rc),
    save_maps_object(MP2::OBJ_ZERO), path(*this), direction(Direction::RIGHT), sprite_index(18), patrol_square(0)
{
    name = _(Heroes::GetName(ht));

    // hero is freeman
    color = Color::NONE;

    // set default army
    army.Reset(true);

    // extra hero
    switch(hid)
    {
        case ROLAND:
            attack    = 0;
            defense   = 1;
            power     = 4;
            knowledge = 5;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::ARCHERY, Skill::Level::BASIC));
    	    break;

        case CORLAGON:
            attack    = 5;
            defense   = 3;
            power     = 1;
            knowledge = 1;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::BALLISTICS, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::BASIC));
    	    break;
        
        case ELIZA:
            attack    = 0;
            defense   = 1;
            power     = 2;
            knowledge = 6;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NAVIGATION, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::ARCHERY, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LUCK, Skill::Level::BASIC));
    	    break;

    	case ARCHIBALD:
            attack    = 1;
            defense   = 1;
            power     = 4;
            knowledge = 4;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
    	    break;

    	case HALTON:
            attack    = 3;
            defense   = 3;
            power     = 3;
            knowledge = 2;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::BALLISTICS, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::DIPLOMACY, Skill::Level::BASIC));
    	    break;
    	
    	case BAX:
            attack    = 1;
            defense   = 1;
            power     = 4;
            knowledge = 3;

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::EXPERT));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NAVIGATION, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::BASIC));
    	    break;

	case SOLMYR:
	case DRAKONIA:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    break;

	case DAINWIN:
	case ELDERIAN:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::BASIC));
	    break;

	case MOG:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::WISDOM, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::NECROMANCY, Skill::Level::ADVANCED));
	    break;
	
	case UNCLEIVAN:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    break;
	
	case JOSEPH:
	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LEADERSHIP, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::BASIC));
	    break;
	
	case GALLAVANT:
	    break;
	
	case CEALLACH:
	    break;
	
	case MARTINE:
	    break;

	case JARKONAS:
	    break;

    	case SANDYSANDY:
	    army.Clear();
	    army.JoinTroop(Monster::BLACK_DRAGON, 2);
	    army.JoinTroop(Monster::RED_DRAGON, 3);

	    secondary_skills.clear();
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::PATHFINDING, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::LOGISTICS, Skill::Level::ADVANCED));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::SCOUTING, Skill::Level::BASIC));
	    secondary_skills.push_back(Skill::Secondary(Skill::Secondary::MYSTICISM, Skill::Level::BASIC));

	    PickupArtifact(Artifact::STEALTH_SHIELD);
	    PickupArtifact(Artifact::DRAGON_SWORD);
	    PickupArtifact(Artifact::NOMAD_BOOTS_MOBILITY);
	    PickupArtifact(Artifact::TRAVELER_BOOTS_MOBILITY);
	    PickupArtifact(Artifact::TRUE_COMPASS_MOBILITY);

	    experience = 777;
	    magic_point = 120;

	    // all spell in magic book
	    for(u8 spell = Spell::FIREBALL; spell < Spell::STONE; ++spell) AppendSpellToBook(Spell(spell), true);
	    break;

	default: break;
    }

    if(! magic_point)
	SetSpellPoints(GetMaxSpellPoints());
    move_point = GetMaxMovePoints();
}

void Heroes::LoadFromMP2(s32 map_index, const void *ptr, const Color::color_t cl, const u8 rc)
{
    // reset modes
    modes = 0;

    SetIndex(map_index);

    color = cl;
    killer_color = Color::NONE;

    const u8  *ptr8  = static_cast<const u8 *>(ptr);
    u16 byte16 = 0;
    u32 byte32 = 0;

    // unknown
    ++ptr8;

    // custom troops
    //bool custom_troop = false;
    if(*ptr8)
    {
        ++ptr8;
	//custom_troop = true;

        // monster1
        army.At(0).SetMonster(Monster(*ptr8 + 1));
        ++ptr8;

        // monster2
        army.At(1).SetMonster(Monster(*ptr8 + 1));
        ++ptr8;

        // monster3
        army.At(2).SetMonster(Monster(*ptr8 + 1));
        ++ptr8;

        // monster4
        army.At(3).SetMonster(Monster(*ptr8 + 1));
        ++ptr8;

        // monster5
        army.At(4).SetMonster(Monster(*ptr8 + 1));
        ++ptr8;

        // count1
        byte16 = ReadLE16(ptr8);
        army.At(0).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count2
        byte16 = ReadLE16(ptr8);
        army.At(1).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count3
        byte16 = ReadLE16(ptr8);
        army.At(2).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count4
        byte16 = ReadLE16(ptr8);
        army.At(3).SetCount(byte16);
        ++ptr8;
        ++ptr8;

        // count5
        byte16 = ReadLE16(ptr8);
        army.At(4).SetCount(byte16);
        ++ptr8;
        ++ptr8;
    }
    else
    {
        ptr8 += 16;
    }

    // custom portrate
    bool custom_portrait = false;
    if(*ptr8)
    {
	SetModes(NOTDEFAULTS);

	custom_portrait = true;
	++ptr8;
	// index sprite portrait
	portrait = ConvertID(*ptr8);
	if(UNKNOWN == portrait)
	{
	    DEBUG(DBG_GAME, DBG_WARN, "custom portrait incorrect: " << static_cast<int>(portrait));
	    portrait = hid;
	}
	++ptr8;
    }
    else
	ptr8 += 2;

    // artifact 1
    PickupArtifact(Artifact(*ptr8));
    ++ptr8;

    // artifact 2
    PickupArtifact(Artifact(*ptr8));
    ++ptr8;

    // artifact 3
    PickupArtifact(Artifact(*ptr8));
    ++ptr8;

    // unknown byte
    ++ptr8;

    // experience
    byte32 = ReadLE32(ptr8);
    experience = byte32;
    ptr8 += 4;

    bool custom_secskill = false;
    // custom skill
    if(*ptr8)
    {
	SetModes(NOTDEFAULTS);

	custom_secskill = true;
	++ptr8;

	secondary_skills.ReadFromMP2(ptr8);
	ptr8 += 16;
    }
    else
    {
	++ptr8;

        ptr8 += 16;
    }
    
    // unknown
    ++ptr8;

    // custom name
    if(*ptr8)
    {
	SetModes(NOTDEFAULTS);
	name = Game::GetEncodeString(reinterpret_cast<const char *>(ptr8 + 1));
    }
    ptr8 += 14;

    // patrol
    if(*ptr8)
    {
	SetModes(PATROL);
	patrol_center = GetCenter();
    }
    ++ptr8;

    // count square
    patrol_square = *ptr8;
    ++ptr8;

    // end

    // save general object
    save_maps_object = MP2::OBJ_ZERO;

    // fix zero army
    if(!army.isValid()) army.Reset(true);

    // level up
    u8 level = GetLevel();
    while(1 < level--)
    {
	SetModes(NOTDEFAULTS);
	LevelUp(custom_secskill, true);
    }

    // fixed race for custom portrait (after level up)
    if(custom_portrait && race != rc)
	race = rc;

    if(race & (Race::SORC | Race::WRLK | Race::WZRD | Race::NECR) &&
	! HaveSpellBook())
    {
	Spell spell = Skill::Primary::GetInitialSpell(race);
        if(spell.isValid())
        {
            SpellBookActivate();
            AppendSpellToBook(spell, true);
        }
    }

    // other param
    SetSpellPoints(GetMaxSpellPoints());
    move_point = GetMaxMovePoints();

    DEBUG(DBG_GAME , DBG_INFO, name << ", color: " << Color::String(color) << ", race: " << Race::String(race));
}

Heroes::heroes_t Heroes::GetID(void) const
{
    return hid;
}

u8 Heroes::GetMobilityIndexSprite(void) const
{
    // valid range (0 - 25)
    const u8 index = !CanMove() ? 0 : move_point / 100;
    return 25 >= index ? index : 25;
}

u8 Heroes::GetManaIndexSprite(void) const
{
    // valid range (0 - 25)
    u8 r = GetSpellPoints() / 5;
    return 25 >= r ? r : 25;
}

u8 Heroes::GetAttack(void) const
{
    return GetAttack(NULL);
}

u8 Heroes::GetAttack(std::string* strs) const
{
    s16 result = attack + GetAttackModificator(strs);

    return result < 0 ? 0 : (result > 255 ? 255 : result);
}

u8 Heroes::GetDefense(void) const
{
    return GetDefense(NULL);
}

u8 Heroes::GetDefense(std::string* strs) const
{
    s16 result = defense + GetDefenseModificator(strs);

    return result < 0 ? 0 : (result > 255 ? 255 : result);
}

u8 Heroes::GetPower(void) const
{
    return GetPower(NULL);
}

u8 Heroes::GetPower(std::string* strs) const
{
    s16 result = power + GetPowerModificator(strs);

    return result < 0 ? 0 : (result > 255 ? 255 : result);
}

u8 Heroes::GetKnowledge(void) const
{
    return GetKnowledge(NULL);
}

u8 Heroes::GetKnowledge(std::string* strs) const
{
    s16 result = knowledge + GetKnowledgeModificator(strs);

    return result < 0 ? 0 : (result > 255 ? 255 : result);
}

void Heroes::IncreasePrimarySkill(const Skill::Primary::skill_t skill)
{
    switch(skill)
    {
	case Skill::Primary::ATTACK:	++attack; break;
	case Skill::Primary::DEFENSE:	++defense; break;
	case Skill::Primary::POWER:	++power; break;
	case Skill::Primary::KNOWLEDGE:	++knowledge; break;
	default: break;
    }
}

u32 Heroes::GetExperience(void) const
{
    return experience;
}

void Heroes::IncreaseMovePoints(const u16 point)
{
    move_point += point;
}

u16 Heroes::GetMovePoints(void) const
{
    return move_point;
}

u16 Heroes::GetMaxSpellPoints(void) const
{
    return 10 * GetKnowledge();
}

u16 Heroes::GetMaxMovePoints(void) const
{
    u16 point = 0;
    u8 acount = 0;

    // start point
    if(isShipMaster())
    {
	point = 1500;

	// skill navigation
	point += point * GetSecondaryValues(Skill::Secondary::NAVIGATION) / 100;

	// artifact bonus
        acount = HasArtifact(Artifact::SAILORS_ASTROLABE_MOBILITY);
        if(acount) point += acount * 1000;

        // visited object
        if(isVisited(MP2::OBJ_LIGHTHOUSE)) point += 500;
    }
    else
    {
    	switch(army.GetSlowestTroop().GetSpeed())
	{
	    default: break;
	    case Speed::CRAWLING:
	    case Speed::VERYSLOW:	point = 1000; break;
	    case Speed::SLOW:		point = 1100; break;
	    case Speed::AVERAGE:	point = 1200; break;
	    case Speed::FAST:		point = 1300; break;
	    case Speed::VERYFAST:	point = 1400; break;
	    case Speed::ULTRAFAST:
	    case Speed::BLAZING:
	    case Speed::INSTANT:	point = 1500; break;
	}

	// skill logistics
	point += point * GetSecondaryValues(Skill::Secondary::LOGISTICS) / 100;

	// artifact bonus
	acount = HasArtifact(Artifact::NOMAD_BOOTS_MOBILITY);
	if(acount) point += acount * 600;

	acount = HasArtifact(Artifact::TRAVELER_BOOTS_MOBILITY);
	if(acount) point += acount * 300;

        // visited object
        if(isVisited(MP2::OBJ_STABLES)) point += 500;
    }

    acount = HasArtifact(Artifact::TRUE_COMPASS_MOBILITY);
    if(acount) point += acount * 500;

    return point;
}

s8 Heroes::GetMorale(void) const
{
    return GetMoraleWithModificators(NULL);
}

s8 Heroes::GetMoraleWithModificators(std::string *strs) const
{
    s8 result = Morale::NORMAL;

    // bonus artifact
    result += GetMoraleModificator(strs);

    if(army.AllTroopsIsRace(Race::NECR)) return Morale::NORMAL;

    // bonus leadership
    result += Skill::GetLeadershipModifiers(GetLevelSkill(Skill::Secondary::LEADERSHIP), strs);

    // object visited
    const u8 objs [] = { MP2::OBJ_BUOY, MP2::OBJ_OASIS, MP2::OBJ_WATERINGHOLE, MP2::OBJ_TEMPLE, MP2::OBJ_GRAVEYARD, MP2::OBJ_DERELICTSHIP, MP2::OBJ_SHIPWRECK };
    result += ObjectVisitedModifiersResult(MDF_MORALE, objs, ARRAY_COUNT(objs), *this, strs);

    // result
    if(result < Morale::AWFUL)	return Morale::TREASON;
    else
    if(result < Morale::POOR)	return Morale::AWFUL;
    else
    if(result < Morale::NORMAL)	return Morale::POOR;
    else
    if(result < Morale::GOOD)	return Morale::NORMAL;
    else
    if(result < Morale::GREAT)	return Morale::GOOD;
    else
    if(result < Morale::BLOOD)	return Morale::GREAT;

    return Morale::BLOOD;
}

s8 Heroes::GetLuck(void) const
{
    return GetLuckWithModificators(NULL);
}

s8 Heroes::GetLuckWithModificators(std::string *strs) const
{
    s8 result = Luck::NORMAL;

    // bonus artifact
    result += GetLuckModificator(strs);

    // bonus luck
    result += Skill::GetLuckModifiers(GetLevelSkill(Skill::Secondary::LUCK), strs);

    // object visited
    const u8 objs [] = { MP2::OBJ_MERMAID, MP2::OBJ_FAERIERING, MP2::OBJ_FOUNTAIN, MP2::OBJ_IDOL, MP2::OBJ_PYRAMID };
    result += ObjectVisitedModifiersResult(MDF_LUCK, objs, ARRAY_COUNT(objs), *this, strs);

    if(result < Luck::AWFUL)	return Luck::CURSED;
    else
    if(result < Luck::BAD)	return Luck::AWFUL;
    else
    if(result < Luck::NORMAL)	return Luck::BAD;
    else
    if(result < Luck::GOOD)	return Luck::NORMAL;
    else
    if(result < Luck::GREAT)	return Luck::GOOD;
    else
    if(result < Luck::IRISH)	return Luck::GREAT;

    return Luck::IRISH;
}

/* recrut hero */
bool Heroes::Recruit(u8 cl, const Point & pt)
{
    if(color != Color::NONE)
    {
	DEBUG(DBG_GAME, DBG_WARN, "not freeman");
	return false;
    }

    Kingdom & kingdom = world.GetKingdom(cl);

    if(kingdom.AllowRecruitHero(false, 0))
    {
	Maps::Tiles & tiles = world.GetTiles(pt);
	color = Color::Get(cl);
	killer_color = Color::NONE;
	SetCenter(pt);
	if(!Modes(SAVEPOINTS)) move_point = GetMaxMovePoints();
	MovePointsScaleFixed();

	if(!army.isValid()) army.Reset(false);

	tiles.SetHeroes(this);
	kingdom.AddHeroes(this);

	return true;
    }

    return false;
}

bool Heroes::Recruit(const Castle & castle)
{
    if(NULL == castle.GetHeroes().Guest() &&
	Recruit(castle.GetColor(), castle.GetCenter()))
    {
	if(castle.GetLevelMageGuild())
	{
	    // magic point
	    if(!Modes(SAVEPOINTS)) SetSpellPoints(GetMaxSpellPoints());
	    // learn spell
	    castle.MageGuildEducateHero(*this);
	}
	return true;
    }

    return false;
}

void Heroes::ActionNewDay(void)
{
    Funds resource;

    // find artifacts                                                                                 
    const u8 artifacts[] = { Artifact::GOLDEN_GOOSE, Artifact::ENDLESS_SACK_GOLD, Artifact::ENDLESS_BAG_GOLD,
                Artifact::ENDLESS_PURSE_GOLD, Artifact::ENDLESS_POUCH_SULFUR, Artifact::ENDLESS_VIAL_MERCURY,
                Artifact::ENDLESS_POUCH_GEMS, Artifact::ENDLESS_CORD_WOOD, Artifact::ENDLESS_CART_ORE,
                Artifact::ENDLESS_POUCH_CRYSTAL, Artifact::UNKNOWN };

    for(u8 index = 0; artifacts[index] != Artifact::UNKNOWN; ++index)
        resource += ProfitConditions::FromArtifact(artifacts[index]) * bag_artifacts.Count(Artifact(artifacts[index]));

    // TAX_LIEN
    resource -= ProfitConditions::FromArtifact(Artifact::TAX_LIEN) * bag_artifacts.Count(Artifact(Artifact::TAX_LIEN));

    // estates skill bonus
    resource.gold += GetSecondaryValues(Skill::Secondary::ESTATES);

    // added
    if(resource.GetValidItems()) world.GetKingdom(GetColor()).AddFundsResource(resource);

    // recovery move points
    move_point = GetMaxMovePoints();
    MovePointsScaleFixed();

    // stables visited?
    if(isVisited(MP2::OBJ_STABLES)) move_point += 400;

    // recovery spell points
    //if(HaveSpellBook())
    {
	u16 curr = GetSpellPoints();
	u16 maxp = GetMaxSpellPoints();
	const Castle* castle = inCastle();

	// possible visit arteian spring 2 * max
	if(curr < maxp)
	{
	    // in castle?
	    if(castle && castle->GetLevelMageGuild())
	    {
		//restore from mage guild
		if(Settings::Get().ExtCastleGuildRestorePointsTurn())
		    curr += maxp * GameStatic::GetMageGuildRestoreSpellPointsPercentDay(castle->GetLevelMageGuild()) / 100;
		else
		    curr = maxp;
	    }

	    // everyday
	    curr += GameStatic::GetHeroesRestoreSpellPointsPerDay();

	    // power ring action
	    u8 acount = HasArtifact(Artifact::POWER_RING);
	    if(acount)
		    curr += acount * Artifact(Artifact::POWER_RING).ExtraValue();

	    // secondary skill
	    curr += GetSecondaryValues(Skill::Secondary::MYSTICISM);

	    SetSpellPoints(curr > maxp ? maxp : curr);
	}
    }

    // remove day visit object
    visit_object.remove_if(Visit::isDayLife);
    

    // new day, new capacities
    ResetModes(SAVEPOINTS);
}

void Heroes::ActionNewWeek(void)
{
    // remove week visit object
    visit_object.remove_if(Visit::isWeekLife);
    
    // fix artesian spring effect
    if(GetSpellPoints() > GetMaxSpellPoints()) SetSpellPoints(GetMaxSpellPoints());
}

void Heroes::ActionNewMonth(void)
{
    // remove month visit object
    visit_object.remove_if(Visit::isMonthLife);
}

void Heroes::ActionAfterBattle(void)
{
    // remove month visit object
    visit_object.remove_if(Visit::isBattleLife);
    //
    SetModes(ACTION);
}

void Heroes::RescanPath(void)
{
    if(path.isValid())
    {
	const Maps::Tiles & tile = world.GetTiles(path.GetDestinationIndex());

	if(!isShipMaster() && tile.isWater() &&
	    !MP2::isNeedStayFront(tile.GetObject()))
	    path.PopBack();
    }

    if(path.isValid())
    {
	if(CONTROL_AI & GetControl())
	{
	    if(path.hasObstacle()) path.Reset();
	}
	else
	{
	    path.Rescan();
	}
    }
}

/* if hero in castle */
const Castle* Heroes::inCastle(void) const
{
    const Castle* castle = Color::NONE != color ? world.GetCastle(GetIndex()) : NULL;
    return castle && castle->GetHeroes() == this ? castle : NULL;
}

Castle* Heroes::inCastle(void)
{
    Castle* castle = Color::NONE != color ? world.GetCastle(GetIndex()) : NULL;
    return castle && castle->GetHeroes() == this ? castle : NULL;
}

/* is visited cell */
bool Heroes::isVisited(const Maps::Tiles & tile, const Visit::type_t type) const
{
    const s32 & index = tile.GetIndex();
    const MP2::object_t object = tile.GetObject(false);

    if(Visit::GLOBAL == type) return world.GetKingdom(color).isVisited(index, object);

    return visit_object.end() != std::find(visit_object.begin(), visit_object.end(), IndexObject(index, object));
}

/* return true if object visited */
bool Heroes::isVisited(const u8 object, const Visit::type_t type) const
{
    if(Visit::GLOBAL == type) return world.GetKingdom(color).isVisited(object);

    return visit_object.end() != std::find_if(visit_object.begin(), visit_object.end(),
				std::bind2nd(std::mem_fun_ref(&IndexObject::isObject), object));
}

/* set visited cell */
void Heroes::SetVisited(const s32 index, const Visit::type_t type)
{
    const Maps::Tiles & tile = world.GetTiles(index);
    const MP2::object_t object = tile.GetObject(false);

    if(Visit::GLOBAL == type)
	world.GetKingdom(color).SetVisited(index, object);
    else
    if(! isVisited(tile) && MP2::OBJ_ZERO != object)
	visit_object.push_front(IndexObject(index, object));
}

void Heroes::SetVisitedWideTile(const s32 index, const u8 object, const Visit::type_t type)
{
    const Maps::Tiles & tile = world.GetTiles(index);
    const Maps::TilesAddon* addon = tile.FindObjectConst(object);
    u8 wide = 0;

    switch(object)
    {
	case MP2::OBJ_SKELETON:
	case MP2::OBJ_OASIS:
	case MP2::OBJ_STANDINGSTONES:
	case MP2::OBJ_ARTESIANSPRING:	wide = 2; break;
	case MP2::OBJ_WATERINGHOLE:	wide = 4; break;
	default: break;
    }

    if(addon && wide)
    {
	for(s32 ii = tile.GetIndex() - (wide - 1); ii <= tile.GetIndex() + (wide - 1); ++ii)
	    if(Maps::isValidAbsIndex(ii) &&
		world.GetTiles(ii).FindAddonLevel1(addon->uniq)) SetVisited(ii, type);
    }
}

u8 Heroes::GetCountArtifacts(void) const
{
    return bag_artifacts.CountArtifacts();
}

bool Heroes::HasUltimateArtifact(void) const
{
    return bag_artifacts.ContainUltimateArtifact();
}

bool Heroes::IsFullBagArtifacts(void) const
{
    return bag_artifacts.isFull();
}

bool Heroes::PickupArtifact(const Artifact & art)
{
    if(!art.isValid()) return false;

    //const Settings & conf = Settings::Get();

    if(! bag_artifacts.PushArtifact(art))
    {
	if(CONTROL_HUMAN == GetControl())
	{
	    art() == Artifact::MAGIC_BOOK ?
	    Dialog::Message("", _("You must purchase a spell book to use the mage guild, but you currently have no room for a spell book. Try giving one of your artifacts to another hero."), Font::BIG, Dialog::OK) :
	    Dialog::Message(art.GetName(), _("You have no room to carry another artifact!"), Font::BIG, Dialog::OK);
	}
	return false;
    }

    // check: anduran garb
    if(bag_artifacts.MakeBattleGarb())
    {
	if(CONTROL_HUMAN == GetControl())
	    Dialog::ArtifactInfo("", _("The three Anduran artifacts magically combine into one."), Artifact::BATTLE_GARB);
    }

    return true;
}

/* return level hero */
u8 Heroes::GetLevel(void) const
{
    return GetLevelFromExperience(experience);
}

void Heroes::IncreaseExperience(const u32 exp)
{
    const u8 level_old = GetLevelFromExperience(experience);
    const u8 level_new = GetLevelFromExperience(experience + exp);

    for(u8 ii = 0; ii < level_new - level_old; ++ii) LevelUp(false);

    experience += exp;
}

/* calc level from exp */
u8 Heroes::GetLevelFromExperience(u32 exp)
{
    for(u8 lvl = 1; lvl < 255; ++ lvl) if(exp < GetExperienceFromLevel(lvl)) return lvl;

    return 0;
}

/* calc exp from level */
u32 Heroes::GetExperienceFromLevel(u8 lvl)
{
    switch(lvl)
    {
	case 0:		return 0;
	case 1:		return 1000;
	case 2:		return 2000;
	case 3:		return 3200;
	case 4:		return 4500;
	case 5:		return 6000;
	case 6:		return 7700;
	case 7:		return 9000;
	case 8: 	return 11000;
	case 9:		return 13200;
	case 10:	return 15500;
	case 11:	return 18500;
	case 12:	return 22100;
	case 13:	return 26400;
	case 14:	return 31600;
	case 15:	return 37800;
	case 16:	return 45300;
	case 17:	return 54200;
	case 18:	return 65000;
	case 19:	return 78000;
	case 20:	return 93600;
	case 21:	return 112300;
	case 22:	return 134700;
	case 23:	return 161600;
	case 24:	return 193900;
	case 25:	return 232700;
	case 26:	return 279300;
	case 27:	return 335200;
	case 28:	return 402300;
	case 29:	return 482800;
	case 30:	return 579400;
	case 31:	return 695300;
	case 32:	return 834400;
	case 33:	return 1001300;
	case 34:	return 1201600;
	case 35:	return 1442000;
	case 36:	return 1730500;
	case 37:	return 2076700;
	case 38:	return 2492100;
	case 39:	return 2990600;

	default:        break;
    }

    const u32 l1 = GetExperienceFromLevel(lvl - 1);
    return (l1 + static_cast<u32>(round((l1 - GetExperienceFromLevel(lvl - 2)) * 1.2 / 100) * 100));
}

/* buy book */
bool Heroes::BuySpellBook(const Castle* castle, u8 shrine)
{
    if(HaveSpellBook() || Color::NONE == color) return false;

    const payment_t payment = PaymentConditions::BuySpellBook(shrine);
    Kingdom & kingdom = world.GetKingdom(color);

    std::string header = _("To cast spells, you must first buy a spell book for %{gold} gold.");
    String::Replace(header, "%{gold}", payment.gold);

    if( ! kingdom.AllowPayment(payment))
    {
	if(CONTROL_HUMAN == GetControl())
	{
	    header.append(". ");
	    header.append(_("Unfortunately, you seem to be a little short of cash at the moment."));
	    Dialog::Message("", header, Font::BIG, Dialog::OK);
	}
	return false;
    }

    if(CONTROL_HUMAN == GetControl())
    {
	const Sprite & border = AGG::GetICN(ICN::RESOURCE, 7);
	Surface sprite(border.w(), border.h());

	border.Blit(sprite);
	AGG::GetICN(ICN::ARTIFACT, Artifact(Artifact::MAGIC_BOOK).IndexSprite64()).Blit(5, 5, sprite);

	header.append(". ");
	header.append(_("Do you wish to buy one?"));

	if(Dialog::NO == Dialog::SpriteInfo("", header, sprite, Dialog::YES | Dialog::NO)) return false;
    }

    if(SpellBookActivate())
    {
	kingdom.OddFundsResource(payment);

	// add all spell to book
	if(castle) castle->MageGuildEducateHero(*this);
#ifdef WITH_NET
	FH2LocalClient::SendHeroesBuyMagicBook(*this);
#endif
	return true;
    }

    return false;
}

/* return true is move enable */
bool Heroes::isEnableMove(void) const
{
    return Modes(ENABLEMOVE) && path.isValid() && path.GetFrontPenalty() <= move_point;
}

bool Heroes::CanMove(void) const
{
    return move_point >= Maps::Ground::GetPenalty(GetIndex(), Direction::CENTER, GetLevelSkill(Skill::Secondary::PATHFINDING));
}

/* set enable move */
void Heroes::SetMove(bool f)
{
    if(f)
    {
	SetModes(ENABLEMOVE);
    }
    else
    {
	ResetModes(ENABLEMOVE);

	// reset sprite position
	switch(direction)
        {
            case Direction::TOP:            sprite_index = 0; break;
            case Direction::BOTTOM:         sprite_index = 36; break;
            case Direction::TOP_RIGHT:
            case Direction::TOP_LEFT:       sprite_index = 9; break;
            case Direction::BOTTOM_RIGHT:
            case Direction::BOTTOM_LEFT:    sprite_index = 27; break;
            case Direction::RIGHT:
            case Direction::LEFT:           sprite_index = 18; break;
            default: break;
	}
    }
}

bool Heroes::isShipMaster(void) const
{
    return Modes(SHIPMASTER);
}

void Heroes::SetShipMaster(bool f)
{
    f ? SetModes(SHIPMASTER) : ResetModes(SHIPMASTER);
}

Skill::SecSkills & Heroes::GetSecondarySkills(void)
{
    return secondary_skills;
}

bool Heroes::HasSecondarySkill(u8 skill) const
{
    return Skill::Level::NONE != secondary_skills.GetLevel(skill);
}

u16 Heroes::GetSecondaryValues(u8 skill) const
{
    return secondary_skills.GetValues(skill);
}

bool Heroes::HasMaxSecondarySkill(void) const
{
    return HEROESMAXSKILL <= secondary_skills.size();
}

u8 Heroes::GetLevelSkill(u8 skill) const
{
    return secondary_skills.GetLevel(skill);
}

void Heroes::LearnSkill(const Skill::Secondary & skill)
{
    if(skill.isValid())
	secondary_skills.AddSkill(skill);
}

void Heroes::Scoute(void) const
{
    Maps::ClearFog(GetIndex(), GetScoute(), GetColor());
}

u8 Heroes::GetScoute(void) const
{
    u8 acount = HasArtifact(Artifact::TELESCOPE);

    return (acount ? acount * Game::GetViewDistance(Game::VIEW_TELESCOPE) : 0) +
	Game::GetViewDistance(Game::VIEW_HEROES) + GetSecondaryValues(Skill::Secondary::SCOUTING);
}

u16 Heroes::GetVisionsDistance(void) const
{
    u16 dist = Spell(Spell::VISIONS).ExtraValue();
    u8 acount = HasArtifact(Artifact::CRYSTAL_BALL);

    if(acount)
        dist = acount * (Settings::Get().UseAltResource() ? dist * 2 + 2 : 8);

    return dist;
}

/* return route range in days */
u8 Heroes::GetRangeRouteDays(const s32 dst) const
{
    const u32 max = GetMaxMovePoints();
    const u16 limit = max * 5 / 100; // limit ~5 day

    // approximate distance, this restriction calculation
    if((4 * max / 100) < Maps::GetApproximateDistance(GetIndex(), dst))
    {
	DEBUG(DBG_GAME, DBG_INFO, "distance limit");
	return 0;
    }

    Route::Path test(*this);
    // approximate limit, this restriction path finding algorithm
    if(test.Calculate(dst, limit))
    {
	u32 total = test.TotalPenalty();
	if(move_point >= total) return 1;

	total -= move_point;
	if(max >= total) return 2;

	total -= move_point;
	if(max >= total) return 3;

	return 4;
    }
    else
    DEBUG(DBG_GAME, DBG_INFO, "iteration limit: " << limit);

    return 0;
}

/* up level */
void Heroes::LevelUp(bool skipsecondary, bool autoselect)
{
    u8 primary = LevelUpPrimarySkill();
    if(! skipsecondary)
	LevelUpSecondarySkill(primary, (autoselect || (CONTROL_AI & GetControl())));
    if(CONTROL_AI & GetControl()) AI::HeroesLevelUp(*this);
}

u8 Heroes::LevelUpPrimarySkill(void)
{
    u8 skill = Skill::Primary::LevelUp(race, GetLevel());

    DEBUG(DBG_GAME, DBG_INFO, "for " << GetName() << ", up " << Skill::Primary::String(skill));
    return skill;
}

void Heroes::LevelUpSecondarySkill(u8 primary, bool autoselect)
{
    Skill::Secondary sec1;
    Skill::Secondary sec2;

    secondary_skills.FindSkillsForLevelUp(race, sec1, sec2);
    DEBUG(DBG_GAME, DBG_INFO, GetName() << " select " << Skill::Secondary::String(sec1.Skill()) <<
						    " or " << Skill::Secondary::String(sec2.Skill()));
    Skill::Secondary* selected = NULL;

    if(autoselect)
    {
	if(Skill::Secondary::UNKNOWN == sec1.Skill() || Skill::Secondary::UNKNOWN == sec2.Skill())
	{
	    if(Skill::Secondary::UNKNOWN != sec1.Skill())
		selected = &sec1;
	    else
	    if(Skill::Secondary::UNKNOWN != sec2.Skill())
		selected = &sec2;
	}
	else
	if(Skill::Secondary::UNKNOWN != sec1.Skill() && Skill::Secondary::UNKNOWN != sec2.Skill())
    	    selected = (Rand::Get(0, 1) ? &sec1 : &sec2);
    }
    else
    {
	AGG::PlaySound(M82::NWHEROLV);
	u8 result = Dialog::LevelUpSelectSkill(name, Skill::Primary::String(primary), sec1, sec2);

	if(Skill::Secondary::UNKNOWN != result)
	    selected = result == sec2.Skill() ? & sec2 : &sec1;
    }

    // level up sec. skill
    if(selected)
    {
	DEBUG(DBG_GAME, DBG_INFO, GetName() << ", selected: " << Skill::Secondary::String(selected->Skill()));

	std::vector<Skill::Secondary>::iterator it;

	it = std::find_if(secondary_skills.begin(), secondary_skills.end(), 
			std::bind2nd(std::mem_fun_ref(&Skill::Secondary::isSkill), selected->Skill()));

	if(it != secondary_skills.end())
	    (*it).NextLevel();
	else
	    secondary_skills.push_back(Skill::Secondary(selected->Skill(), Skill::Level::BASIC));

	// post action
	switch(selected->Skill())
	{
	    case Skill::Secondary::SCOUTING:
		Scoute();
		break;

	    default: break;
        }
    }
}

/* apply penalty */
bool Heroes::ApplyPenaltyMovement(void)
{
    const u16 penalty = path.isValid() ?
	    path.GetFrontPenalty() :
	    Maps::Ground::GetPenalty(GetIndex(), Direction::CENTER, GetLevelSkill(Skill::Secondary::PATHFINDING));

    if(move_point >= penalty) move_point -= penalty;
    else return false;

    return true;
}

bool Heroes::MayStillMove(void) const
{
    if(Modes(SLEEPER|GUARDIAN) || isFreeman()) return false;
    return path.isValid() ? (move_point >= path.GetFrontPenalty()) : CanMove();
}

bool Heroes::isValid(void) const
{
    return hid != UNKNOWN;
}

bool Heroes::isFreeman(void) const
{
    return isValid() && Color::NONE == color && !Modes(JAIL);
}

void Heroes::SetFreeman(const u8 reason)
{
    if(isFreeman()) return;

    bool savepoints = false;
    Kingdom & kingdom = world.GetKingdom(color);

    if((Battle2::RESULT_RETREAT | Battle2::RESULT_SURRENDER) & reason)
    {
	if(Settings::Get().ExtRememberPointsForHeroRetreating()) savepoints = true;
	kingdom.SetLastLostHero(*this);
    }

    if(!army.isValid() || (Battle2::RESULT_RETREAT & reason)) army.Reset(false);
    else
    if((Battle2::RESULT_LOSS & reason) && !(Battle2::RESULT_SURRENDER & reason)) army.Reset(true);

    if(color != Color::NONE) kingdom.RemoveHeroes(this);

    color = Color::NONE;
    world.GetTiles(GetIndex()).SetHeroes(NULL);
    modes = 0;
    SetIndex(-1);
    move_point_scale = -1;
    path.Reset();
    SetMove(false);
    SetModes(ACTION);
    if(savepoints) SetModes(SAVEPOINTS);
}

const Surface & Heroes::GetPortrait30x22(heroes_t hid)
{
    if(Heroes::SANDYSANDY > hid) return AGG::GetICN(ICN::MINIPORT, hid);
    else
    if(Heroes::SANDYSANDY == hid) return AGG::GetICN(ICN::MINIPORT, BAX);

    return AGG::GetICN(ICN::MINIPORT, 0);
}

const Surface & Heroes::GetPortrait50x46(heroes_t hid)
{
    if(Heroes::SANDYSANDY > hid) return AGG::GetICN(ICN::PORTMEDI, hid + 1);
    else
    if(Heroes::SANDYSANDY == hid) return AGG::GetICN(ICN::PORTMEDI, BAX + 1);

    return AGG::GetICN(ICN::PORTMEDI, 0);
}

const Surface & Heroes::GetPortrait101x93(heroes_t hid)
{
    ICN::icn_t icn = ICN::PORTxxxx(hid);

    return AGG::GetICN(ICN::UNKNOWN != icn ? icn : ICN::PORT0000, 0);
}

const Surface & Heroes::GetPortrait30x22(void) const
{
    return GetPortrait30x22(portrait);
}

const Surface & Heroes::GetPortrait50x46(void) const
{
    return GetPortrait50x46(portrait);
}

const Surface & Heroes::GetPortrait101x93(void) const
{
    return GetPortrait101x93(portrait);
}

void Heroes::SetKillerColor(Color::color_t c)
{
    killer_color = c;
}

Color::color_t Heroes::GetKillerColor(void) const
{
    return killer_color;
}

u8 Heroes::GetControl(void) const
{
    return world.GetKingdom(color).GetControl();
}

u8 Heroes::GetMapsObject(void) const
{
    return save_maps_object;
}

void Heroes::SetMapsObject(u8 obj)
{
    save_maps_object = obj != MP2::OBJ_HEROES ? obj : MP2::OBJ_ZERO;
}

bool Heroes::AllowBattle(bool attacker) const
{
    if(!attacker)
    switch(world.GetTiles(GetIndex()).GetObject(false))
    {
	case MP2::OBJ_TEMPLE: return false;
	default: break;
    }

    return true;
}

void Heroes::ActionPreBattle(void)
{
}

void RedrawGameAreaAndHeroAttackMonster(Heroes & hero, s32 dst)
{
    // redraw gamearea for monster action sprite
    if(CONTROL_HUMAN == hero.GetControl())
    {
	Interface::Basic & I = Interface::Basic::Get();
        Cursor::Get().Hide();
	I.gameArea.SetCenter(hero.GetCenter());
	GameFocus::SetRedraw();
	I.Redraw();
        Cursor::Get().Show();
	// force flip, for monster attack show sprite
	Display::Get().Flip();
    }
    hero.Action(dst);
}

void Heroes::ActionNewPosition(void)
{
    const Settings & conf = Settings::Get();
    // check around monster
    MapsIndexes targets = Maps::GetTilesUnderProtection(GetIndex());

    if(targets.size())
    {
	bool skip_battle = false;
	SetMove(false);
	GetPath().Hide();

	// first target
        MapsIndexes::iterator it = std::find(targets.begin(), targets.end(),
                                                        GetPath().GetDestinedIndex());
        if(it != targets.end())
	{
	    RedrawGameAreaAndHeroAttackMonster(*this, *it);
	    targets.erase(it);
	    if(conf.ExtOnlyFirstMonsterAttack()) skip_battle = true;
	}

	// other around targets
	for(MapsIndexes::const_iterator
	    it = targets.begin(); it != targets.end() && !isFreeman() && !skip_battle; ++it)
	{
	    RedrawGameAreaAndHeroAttackMonster(*this, *it);
	    if(conf.ExtOnlyFirstMonsterAttack()) skip_battle = true;
	}
    }

    if(! isFreeman() &&
	world.GetTiles(GetIndex()).GetObject(false) == MP2::OBJ_EVENT)
    {
	Action(GetIndex());
	SetMove(false);
    }

    ResetModes(VISIONS);
}

void Heroes::SetCenterPatrol(const Point & pt)
{
    patrol_center = pt;
}

const Point & Heroes::GetCenterPatrol(void) const
{
    return patrol_center;
}

u8 Heroes::GetSquarePatrol(void) const
{
    return patrol_square;
}

u8 Heroes::CanScouteTile(s32 dst) const
{
    u8 scouting = GetSecondaryValues(Skill::Secondary::SCOUTING);
    bool army_info = false;

    switch(world.GetTiles(dst).GetObject())
    {
	case MP2::OBJ_MONSTER:
	case MP2::OBJ_CASTLE:
	case MP2::OBJ_HEROES:
	    army_info = true; break;

	default: break;
    }

    if(army_info)
    {
	// depends from distance
	if(Maps::GetApproximateDistance(GetIndex(), dst) <= GetVisionsDistance())
	{
	    // check crystal ball
	    return HasArtifact(Artifact::CRYSTAL_BALL) ?
			    Skill::Level::EXPERT : scouting;
	}
	else
	{
	    // check spell identify hero
	    if(world.GetKingdom(GetColor()).Modes(Kingdom::IDENTIFYHERO) &&
		MP2::OBJ_HEROES == world.GetTiles(dst).GetObject())
		return Skill::Level::EXPERT;
	}
    }
    else
    {
	if(Settings::Get().ExtScouteExtended())
	{
	    //const Maps::Tiles & tile = world.GetTiles(dst);

	    u16 dist = GetSecondaryValues(Skill::Secondary::SCOUTING) ? GetScoute() : 0;
	    if(Modes(VISIONS) && dist < GetVisionsDistance()) dist = GetVisionsDistance();

	    if(dist > Maps::GetApproximateDistance(GetIndex(), dst))
		return scouting;
	}
    }

    return 0;
}

void Heroes::MovePointsScaleFixed(void)
{
    move_point_scale = move_point * 1000 / GetMaxMovePoints();
}

void Heroes::RecalculateMovePoints(void)
{
    if(0 <= move_point_scale) move_point = GetMaxMovePoints() * move_point_scale / 1000;
}

void Heroes::Move2Dest(const s32 & dst_index, bool skip_action /* false */)
{
    Maps::Tiles & tiles_from = world.GetTiles(GetIndex());
    Maps::Tiles & tiles_to = world.GetTiles(dst_index);

    tiles_from.SetHeroes(NULL);
    SetIndex(dst_index);
    Scoute();
    ApplyPenaltyMovement();
    tiles_to.SetHeroes(this);

    if(!skip_action)
	ActionNewPosition();
}

std::string Heroes::String(void) const
{
    std::ostringstream os;

    os <<
	"name            : " << name << std::endl <<
	"race            : " << Race::String(race) << std::endl <<
	"color           : " << Color::String(color) << std::endl <<
	"experience      : " << experience << std::endl <<
	"level           : " << static_cast<int>(GetLevel()) << std::endl <<
	"magic point     : " << GetSpellPoints() << std::endl <<
	"position x      : " << GetCenter().x << std::endl <<
	"position y      : " << GetCenter().y << std::endl <<
	"move point      : " << move_point << std::endl <<
	"max magic point : " << GetMaxSpellPoints() << std::endl <<
	"max move point  : " << GetMaxMovePoints() << std::endl <<
	"direction       : " << Direction::String(direction) << std::endl <<
	"index sprite    : " << static_cast<u16>(sprite_index) << std::endl <<
	"in castle       : " << (inCastle() ? "true" : "false") << std::endl <<
	"save object     : " << MP2::StringObject(world.GetTiles(GetIndex()).GetObject(false)) << std::endl <<
	"flags           : " << (Modes(SHIPMASTER) ? "SHIPMASTER," : "") <<
                                         (Modes(SCOUTER) ? "SCOUTER," : "") <<
                                         (Modes(HUNTER) ? "HUNTER," : "") <<
                                         (Modes(PATROL) ? "PATROL," : "") <<
                                         (Modes(AIWAITING) ? "WAITING," : "") <<
                                         (Modes(STUPID) ? "STUPID," : "") << std::endl;
    if(Modes(PATROL))
    {
	os << "patrol square   : " << static_cast<u16>(patrol_square) << std::endl;
    }

    if(! visit_object.empty())
    {
	os << "visit objects   : ";
	for(std::list<IndexObject>::const_iterator
	    it = visit_object.begin(); it != visit_object.end(); ++it)
		os << MP2::StringObject((*it).second) << "(" << (*it).first << "), ";
	os << std::endl;
    }

    if(GetControl() & CONTROL_AI)
    {
	os <<
	    "skills          : " << secondary_skills.String() << std::endl <<
	    "artifacts       : " << bag_artifacts.String() << std::endl <<
	    "spell book      : " << (HaveSpellBook() ? spell_book.String() : "disabled") << std::endl <<
	    "army dump       : " << army.String() << std::endl;

	os << AI::HeroesString(*this);
    }

    return os.str();
}

struct InCastleAndGuardian : public std::binary_function <const Castle*, Heroes*, bool>
{
    bool operator() (const Castle* castle, Heroes* hero) const
    {
        const Point & cpt = castle->GetCenter();
        const Point & hpt = hero->GetCenter();
        return cpt.x == hpt.x && cpt.y == hpt.y + 1 && hero->Modes(Heroes::GUARDIAN);
    }
};

struct InCastleNotGuardian : public std::binary_function <const Castle*, Heroes*, bool>
{
    bool operator() (const Castle* castle, Heroes* hero) const
    {
        return castle->GetCenter() == hero->GetCenter() && !hero->Modes(Heroes::GUARDIAN);
    }
};

struct InJailMode : public std::binary_function <s32, Heroes*, bool>
{
    bool operator() (s32 index, Heroes* hero) const
    {
	return hero->Modes(Heroes::JAIL) && index == hero->GetIndex();
    }
};

AllHeroes::AllHeroes()
{
    reserve(HEROESMAXCOUNT + 2);
}

AllHeroes::~AllHeroes()
{
    AllHeroes::clear();
}

void AllHeroes::Init(void)
{
    if(size())
	AllHeroes::clear();

    const bool loyalty = Settings::Get().PriceLoyaltyVersion();

    // knight: LORDKILBURN, SIRGALLANTH, ECTOR, GVENNETH, TYRO, AMBROSE, RUBY, MAXIMUS, DIMITRY
    for(u8 hid = Heroes::LORDKILBURN; hid <= Heroes::DIMITRY; ++hid)
        push_back(new Heroes(static_cast<Heroes::heroes_t>(hid), Race::KNGT));

    // barbarian: THUNDAX, FINEOUS, JOJOSH, CRAGHACK, JEZEBEL, JACLYN, ERGON, TSABU, ATLAS
    for(u8 hid = Heroes::THUNDAX; hid <= Heroes::ATLAS; ++hid)
        push_back(new Heroes(static_cast<Heroes::heroes_t>(hid), Race::BARB));

    // sorceress: ASTRA, NATASHA, TROYAN, VATAWNA, REBECCA, GEM, ARIEL, CARLAWN, LUNA
    for(u8 hid = Heroes::ASTRA; hid <= Heroes::LUNA; ++hid)
        push_back(new Heroes(static_cast<Heroes::heroes_t>(hid), Race::SORC));

    // warlock: ARIE, ALAMAR, VESPER, CRODO, BAROK, KASTORE, AGAR, FALAGAR, WRATHMONT
    for(u8 hid = Heroes::ARIE; hid <= Heroes::WRATHMONT; ++hid)
        push_back(new Heroes(static_cast<Heroes::heroes_t>(hid), Race::WRLK));

    // wizard: MYRA, FLINT, DAWN, HALON, MYRINI, WILFREY, SARAKIN, KALINDRA, MANDIGAL
    for(u8 hid = Heroes::MYRA; hid <= Heroes::MANDIGAL; ++hid)
        push_back(new Heroes(static_cast<Heroes::heroes_t>(hid), Race::WZRD));

    // necromancer: ZOM, DARLANA, ZAM, RANLOO, CHARITY, RIALDO, ROXANA, SANDRO, CELIA                            
    for(u8 hid = Heroes::ZOM; hid <= Heroes::CELIA; ++hid)
        push_back(new Heroes(static_cast<Heroes::heroes_t>(hid), Race::NECR));

    // from campain
    push_back(new Heroes(Heroes::ROLAND, Race::WZRD));
    push_back(new Heroes(Heroes::CORLAGON, Race::KNGT));
    push_back(new Heroes(Heroes::ELIZA, Race::SORC));
    push_back(new Heroes(Heroes::ARCHIBALD, Race::WRLK));
    push_back(new Heroes(Heroes::HALTON, Race::KNGT));
    push_back(new Heroes(Heroes::BAX, Race::NECR));

    // loyalty version
    push_back(new Heroes(loyalty ? Heroes::SOLMYR : Heroes::UNKNOWN, Race::WZRD));
    push_back(new Heroes(loyalty ? Heroes::DAINWIN : Heroes::UNKNOWN, Race::WRLK));
    push_back(new Heroes(loyalty ? Heroes::MOG : Heroes::UNKNOWN, Race::NECR));
    push_back(new Heroes(loyalty ? Heroes::UNCLEIVAN : Heroes::UNKNOWN, Race::BARB));
    push_back(new Heroes(loyalty ? Heroes::JOSEPH : Heroes::UNKNOWN, Race::KNGT));
    push_back(new Heroes(loyalty ? Heroes::GALLAVANT : Heroes::UNKNOWN, Race::KNGT));
    push_back(new Heroes(loyalty ? Heroes::ELDERIAN : Heroes::UNKNOWN, Race::WRLK));
    push_back(new Heroes(loyalty ? Heroes::CEALLACH : Heroes::UNKNOWN, Race::KNGT));
    push_back(new Heroes(loyalty ? Heroes::DRAKONIA : Heroes::UNKNOWN, Race::WZRD));
    push_back(new Heroes(loyalty ? Heroes::MARTINE : Heroes::UNKNOWN, Race::SORC));
    push_back(new Heroes(loyalty ? Heroes::JARKONAS : Heroes::UNKNOWN, Race::BARB));

    // devel
    push_back(new Heroes(IS_DEVEL() ? Heroes::SANDYSANDY : Heroes::UNKNOWN, Race::WRLK));
    push_back(new Heroes(Heroes::UNKNOWN, Race::KNGT));
}

void AllHeroes::clear(void)
{
    for(iterator
	it = begin(); it != end(); ++it) delete *it;
    std::vector<Heroes *>::clear();
}

Heroes* VecHeroes::Get(Heroes::heroes_t hid) const
{
    const std::vector<Heroes *> & vec = * this;
    return hid != Heroes::UNKNOWN ? vec[hid] : NULL;
}

Heroes* VecHeroes::Get(s32 index) const
{
    const_iterator it = std::find_if(begin(), end(),
    	    std::bind2nd(std::mem_fun(&Heroes::isPosition), index));
    return end() != it ? *it : NULL;
}

Heroes* AllHeroes::GetGuest(const Castle & castle) const
{
    const_iterator it = std::find_if(begin(), end(),
	    std::bind1st(InCastleNotGuardian(), &castle));
    return end() != it ? *it : NULL;
}

Heroes* AllHeroes::GetGuard(const Castle & castle) const
{
    const_iterator it = Settings::Get().ExtAllowCastleGuardians() ?                                                          
        std::find_if(begin(), end(), std::bind1st(InCastleAndGuardian(), &castle)) : end();
    return end() != it ? *it : NULL;
}

Heroes* AllHeroes::GetFreeman(u8 race) const
{
    const Settings & conf = Settings::Get();

    u8 min = Heroes::UNKNOWN;
    u8 max = Heroes::UNKNOWN;

    switch(race)
    {
	case Race::KNGT:
	    min = Heroes::LORDKILBURN;
	    max = Heroes::DIMITRY;
	    break;
	
	case Race::BARB:
	    min = Heroes::THUNDAX;
	    max = Heroes::ATLAS;
	    break;
	
	case Race::SORC:
	    min = Heroes::ASTRA;
	    max = Heroes::LUNA;
	    break;
	    
	case Race::WRLK:
	    min = Heroes::ARIE;
	    max = Heroes::WRATHMONT;
	    break;
	
	case Race::WZRD:
	    min = Heroes::MYRA;
	    max = Heroes::MANDIGAL;
	    break;
	
	case Race::NECR:
	    min = Heroes::ZOM;
	    max = Heroes::CELIA;
	    break;
	
	default:
	    min = Heroes::LORDKILBURN;
	    max = conf.ExtCastleAllowRecruitSpecialHeroes() ? (conf.PriceLoyaltyVersion() ? Heroes::JARKONAS : Heroes::BAX) : Heroes::CELIA;
	    break;
    }

    std::vector<u8> freeman_heroes;
    freeman_heroes.reserve(HEROESMAXCOUNT);

    // find freeman in race (skip: manual changes)
    for(u8 ii = min; ii <= max; ++ii)
	if(at(ii)->isFreeman() && !at(ii)->Modes(Heroes::NOTDEFAULTS)) freeman_heroes.push_back(ii);

    // not found, find any race
    if(Race::NONE != race && freeman_heroes.empty())
    {
	min = Heroes::LORDKILBURN;
	max = conf.ExtCastleAllowRecruitSpecialHeroes() ? (conf.PriceLoyaltyVersion() ? Heroes::JARKONAS : Heroes::BAX) : Heroes::CELIA;

	for(u8 ii = min; ii <= max; ++ii)
	    if(at(ii)->isFreeman()) freeman_heroes.push_back(ii);
    }

    // not found, all heroes busy
    if(freeman_heroes.empty())
    {
	DEBUG(DBG_GAME, DBG_WARN, "freeman not found, all heroes busy.");
	return NULL;
    }

    return at(*Rand::Get(freeman_heroes));
}

void AllHeroes::Scoute(u8 colors) const
{
    for(const_iterator it = begin(); it != end(); ++it)
	if(colors & (*it)->GetColor()) (*it)->Scoute();                        
}

Heroes* AllHeroes::FromJail(s32 index) const
{
    const_iterator it = std::find_if(begin(), end(),
	    std::bind1st(InJailMode(), index));
    return end() != it ? *it : NULL;
}

bool AllHeroes::HaveTwoFreemans(void) const
{
    return 2 <= std::count_if(begin(), end(),
			    std::mem_fun(&Heroes::isFreeman));
}
