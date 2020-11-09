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

#ifndef H2SETTINGS_H
#define H2SETTINGS_H

#include <string>
#include <list>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "gamedefs.h"
#include "maps_fileinfo.h"
#include "game.h"
#include "players.h"
#include "game_io.h"
#include "bitmodes.h"

#ifdef ANDROID
#include <android/log.h>
#endif

#define FORMAT_VERSION_2707 0x0A93
#define FORMAT_VERSION_2689 0x0A81
#define FORMAT_VERSION_2667 0x0A6B
#define FORMAT_VERSION_2663 0x0A67
#define FORMAT_VERSION_2662 0x0A66
#define FORMAT_VERSION_2655 0x0A5F
#define FORMAT_VERSION_2646 0x0A56
#define FORMAT_VERSION_2632 0x0A48
#define FORMAT_VERSION_2629 0x0A45
#define FORMAT_VERSION_2626 0x0A42
#define FORMAT_VERSION_2602 0x0A2A
#define FORMAT_VERSION_2566 0x0A06
#define FORMAT_VERSION_2562 0x0A02
#define FORMAT_VERSION_2522 0x09DA
#define FORMAT_VERSION_2520 0x09D8
#define CURRENT_FORMAT_VERSION FORMAT_VERSION_2707
#define LAST_FORMAT_VERSION FORMAT_VERSION_2520

#define ListMapsDirectory std::list<std::string>

enum
{
    DBG_WARN	= 0x0001,
    DBG_INFO	= 0x0002,
    DBG_TRACE	= 0x0003,

    DBG_ENGINE	= 0x000C,
    DBG_GAME	= 0x0030,
    DBG_BATTLE	= 0x00C0,
    DBG_AI	= 0x0300,
    DBG_NETWORK	= 0x0C00,
    DBG_OTHER	= 0x3000,
    DBG_DEVEL	= 0xC000,

    DBG_ENGINE_WARN = 0x0004,
    DBG_GAME_WARN   = 0x0010,
    DBG_BATTLE_WARN = 0x0040,
    DBG_AI_WARN     = 0x0100,
    DBG_NETWORK_WARN= 0x0400,
    DBG_OTHER_WARN  = 0x1000,

    DBG_ENGINE_INFO = 0x0008,
    DBG_GAME_INFO   = 0x0020,
    DBG_BATTLE_INFO = 0x0080,
    DBG_AI_INFO     = 0x0200,
    DBG_NETWORK_INFO= 0x0800,
    DBG_OTHER_INFO  = 0x2000,

    DBG_ENGINE_TRACE = DBG_ENGINE,
    DBG_GAME_TRACE   = DBG_GAME,
    DBG_BATTLE_TRACE = DBG_BATTLE,
    DBG_AI_TRACE     = DBG_AI,
    DBG_NETWORK_TRACE= DBG_NETWORK,
    DBG_OTHER_TRACE  = DBG_OTHER,

    DBG_ALL = DBG_ENGINE | DBG_GAME | DBG_BATTLE | DBG_AI | DBG_NETWORK | DBG_OTHER,

    DBG_ALL_WARN = DBG_ENGINE_WARN | DBG_GAME_WARN | DBG_BATTLE_WARN | DBG_AI_WARN | DBG_NETWORK_WARN | DBG_OTHER_WARN,
    DBG_ALL_INFO = DBG_ENGINE_INFO | DBG_GAME_INFO | DBG_BATTLE_INFO | DBG_AI_INFO | DBG_NETWORK_INFO | DBG_OTHER_INFO,
    DBG_ALL_TRACE= DBG_ENGINE_TRACE | DBG_GAME_TRACE | DBG_BATTLE_TRACE | DBG_AI_TRACE | DBG_NETWORK_TRACE | DBG_OTHER_TRACE
};

const char* StringDebug(int);

enum
{
    SCROLL_SLOW   = 4,
    SCROLL_NORMAL = 8,
    SCROLL_FAST1  = 16,
    SCROLL_FAST2  = 32
};

#if defined(__SYMBIAN32__)
 #define VERBOSE(x)
 #undef DEBUG
 #define DEBUG(x, y, z)
#elif defined(ANDROID)
namespace std
{
 #define endl "\n"
}
 #define VERBOSE(x) { std::ostringstream osss; osss << x; __android_log_print(ANDROID_LOG_INFO, "FHeroes", "%s", osss.str().c_str()); }
#else
 #define VERBOSE(x) std::cout << x << std::endl
#endif

#ifdef WITH_DEBUG
 #define DEBUG(x, y, z) if(IS_DEBUG(x, y)) VERBOSE(String::GetTime() << ": [" << StringDebug(x) << "]\t" << __FUNCTION__ << ":  " << z)
#else
 #define DEBUG(x, y, z)
#endif

#define IS_DEVEL() IS_DEBUG(DBG_DEVEL, DBG_INFO)

bool IS_DEBUG(int name, int level);

class Settings
{
public:
    enum
    {
	GAME_AUTOSAVE_BEGIN_DAY		= 0x10000010,
	GAME_REMEMBER_LAST_FOCUS	= 0x10000020,
	GAME_SAVE_REWRITE_CONFIRM	= 0x10000040,
	GAME_CASTLE_FLASH_BUILDING	= 0x10000080,
	GAME_SHOW_SYSTEM_INFO		= 0x10000100,
	GAME_AUTOSAVE_ON		= 0x10000200,
	GAME_USE_FADE			= 0x10000400,
	GAME_SHOW_SDL_LOGO		= 0x10000800,
	GAME_EVIL_INTERFACE		= 0x10001000,
	GAME_HIDE_INTERFACE		= 0x10002000,
	GAME_ALSO_CONFIRM_AUTOSAVE	= 0x10004000,
	GAME_REMEMBER_LAST_FILENAME	= 0x10008000,
	GAME_DYNAMIC_INTERFACE		= 0x10010000,
        GAME_BATTLE_SHOW_GRID		= 0x10020000,
        GAME_BATTLE_SHOW_MOUSE_SHADOW	= 0x10040000,
        GAME_BATTLE_SHOW_MOVE_SHADOW	= 0x10080000,
	GAME_BATTLE_SHOW_DAMAGE		= 0x10100000,
	//UNUSED			= 0x10200000,
	POCKETPC_HIDE_CURSOR		= 0x10400000,
	POCKETPC_LOW_MEMORY		= 0x10800000,
	POCKETPC_TAP_MODE		= 0x11000000,
	POCKETPC_DRAG_DROP_SCROLL	= 0x12000000,
	POCKETPC_LOW_RESOLUTION		= 0x14000000,

	/* influence on game balance: save to savefile */
	WORLD_SHOW_VISITED_CONTENT	= 0x20000001,
	WORLD_ABANDONED_MINE_RANDOM	= 0x20000002,
	WORLD_SAVE_MONSTER_BATTLE	= 0x20000004,
	WORLD_ALLOW_SET_GUARDIAN	= 0x20000008,
	WORLD_NOREQ_FOR_ARTIFACTS	= 0x20000010,
	WORLD_ARTIFACT_CRYSTAL_BALL	= 0x20000020,
	WORLD_SCOUTING_EXTENDED		= 0x20000040,
	WORLD_ONLY_FIRST_MONSTER_ATTACK	= 0x20000080,
	WORLD_EYE_EAGLE_AS_SCHOLAR	= 0x20000100,
	WORLD_BUY_BOOK_FROM_SHRINES	= 0x20000200,
	WORLD_BAN_WEEKOF		= 0x20000400,
	WORLD_BAN_PLAGUES		= 0x20000800,
	UNIONS_ALLOW_HERO_MEETINGS	= 0x20001000,
	UNIONS_ALLOW_CASTLE_VISITING	= 0x20002000,
	UNIONS_ALLOW_VIEW_MAPS		= 0x20004000,
	HEROES_AUTO_MOVE_BATTLE_DST	= 0x20008000,
	WORLD_BAN_MONTHOF_MONSTERS	= 0x20010000,
	HEROES_TRANSCRIBING_SCROLLS	= 0x20020000,
	WORLD_NEW_VERSION_WEEKOF	= 0x20040000,
	CASTLE_ALLOW_GUARDIANS		= 0x20080000,
	CASTLE_ALLOW_BUY_FROM_WELL	= 0x20100000,
	HEROES_LEARN_SPELLS_WITH_DAY	= 0x20200000,
	HEROES_ALLOW_BANNED_SECSKILLS	= 0x20400000,
	HEROES_COST_DEPENDED_FROM_LEVEL = 0x20800000,
	HEROES_REMEMBER_POINTS_RETREAT  = 0x21000000,
	HEROES_SURRENDERING_GIVE_EXP	= 0x22000000,
	HEROES_RECALCULATE_MOVEMENT	= 0x24000000,
	HEROES_PATROL_ALLOW_PICKUP	= 0x28000000,

	CASTLE_MAGEGUILD_POINTS_TURN	= 0x30000001,
	WORLD_ARTSPRING_SEPARATELY_VISIT= 0x30000002,
	CASTLE_ALLOW_RECRUITS_SPECIAL	= 0x30000004,
	WORLD_STARTHERO_LOSSCOND4HUMANS = 0x30000008,
	WORLD_1HERO_HIRED_EVERY_WEEK	= 0x30000010,
	WORLD_DWELLING_ACCUMULATE_UNITS = 0x30000020,
	WORLD_GUARDIAN_TWO_DEFENSE	= 0x30000040,
	HEROES_ARENA_ANY_SKILLS		= 0x30000080,
	WORLD_USE_UNIQUE_ARTIFACTS1	= 0x30000100,
	WORLD_USE_UNIQUE_ARTIFACTS2	= 0x30000200,
	//WORLD_USE_UNIQUE_ARTIFACTS3	= 0x30000400,
	//WORLD_USE_UNIQUE_ARTIFACTS4	= 0x30000800,
	WORLD_ENHANCED_ARTIFACT_INFO	= 0x30001000,
	WORLD_WIND_WATER_MILLS_CAPTURED = 0x30004000,

	BATTLE_ARCHMAGE_RESIST_BAD_SPELL= 0x40001000,
	BATTLE_MAGIC_TROOP_RESIST	= 0x40002000,
	BATTLE_TROOP_DIRECTION		= 0x40008000,
	BATTLE_SOFT_WAITING		= 0x40010000,
	BATTLE_REVERSE_WAIT_ORDER	= 0x40020000,
        BATTLE_MERGE_ARMIES		= 0x40100000,
        BATTLE_SKIP_INCREASE_DEFENSE	= 0x40200000,
	BATTLE_OBJECTS_ARCHERS_PENALTY	= 0x42000000,

	SETTINGS_LAST
    };

    static Settings & Get(void);

    bool Read(const std::string & filename);
    bool Save(const std::string & filename) const;

    std::string String(void) const;
    bool SetCurrentFileInfo(const std::string &);
    void SetCurrentFileInfo(const Maps::FileInfo &);
    Maps::FileInfo & CurrentFileInfo(void);
    const Maps::FileInfo & CurrentFileInfo(void) const;

    u16 Debug(void) const;
    u8 HeroesMoveSpeed(void) const;
    u8 AIMoveSpeed(void) const;
    u8 BattleSpeed(void) const;
    u8 ScrollSpeed(void) const;
    u32 MemoryLimit(void) const;

    const std::string & DataDirectory(void) const;
    const std::string & LocalPrefix(void) const;
    const std::string & PlayMusCommand(void) const;
    const std::string & SelectVideoDriver(void) const;

    const ListMapsDirectory & GetListMapsDirectory(void) const;

    u8 GameDifficulty(void) const;

    const std::string & MapsCharset(void) const;
    const std::string & ForceLang(void) const;
    const std::string & FontsNormal(void) const;
    const std::string & FontsSmall(void) const;
    u8 FontsNormalSize(void) const;
    u8 FontsSmallSize(void) const;
    bool FontSmallRenderBlended(void) const;
    bool FontNormalRenderBlended(void) const;

    const Point & PosRadar(void) const;
    const Point & PosButtons(void) const;
    const Point & PosIcons(void) const;
    const Point & PosStatus(void) const;

    void SetPosRadar(const Point &);
    void SetPosButtons(const Point &);
    void SetPosIcons(const Point &);
    void SetPosStatus(const Point &);

    u32  DisplayFlags(void) const;

    bool QVGA(void) const;
    bool Editor(void) const;
    bool Sound(void) const;
    bool Music(void) const;
    bool ShowControlPanel(void) const;
    bool ShowRadar(void) const;
    bool ShowIcons(void) const;
    bool ShowButtons(void) const;
    bool ShowStatus(void) const;
    bool Unicode(void) const;
    bool PocketPC(void) const;
    bool UseAltResource(void) const;
    bool PriceLoyaltyVersion(void) const;
    bool LoadedGameVersion(void) const;
    bool MusicExt(void) const;
    bool MusicMIDI(void) const;
    bool MusicCD(void) const;
    void BinarySave(void) const;
    void BinaryLoad(void);

    bool ExtModes(u32) const;
    void ExtSetModes(u32);
    void ExtResetModes(u32);
    const char* ExtName(u32) const;
    bool ExtAllowBuyFromWell(void) const;
    bool ExtShowVisitedContent(void) const;
    bool ExtScouteExtended(void) const;
    bool ExtRememberLastFocus(void) const;
    bool ExtRememberLastFilename(void) const;
    bool ExtAbandonedMineRandom(void) const;
    bool ExtSaveMonsterBattle(void) const;
    bool ExtAllowSetGuardian(void) const;
    bool ExtNoRequirementsForArtifacts(void) const;
    bool ExtArtifactCrystalBall(void) const;
    bool ExtLearnSpellsWithDay(void) const;
    bool ExtOnlyFirstMonsterAttack(void) const;
    bool ExtEyeEagleAsScholar(void) const;
    bool ExtBuySpellBookFromShrine(void) const;
    bool ExtHeroRecruitCostDependedFromLevel(void) const;
    bool ExtRememberPointsForHeroRetreating(void) const;
    bool ExtHeroSurrenderingGiveExp(void) const;
    bool ExtHeroRecalculateMovement(void) const;
    bool ExtHeroPatrolAllowPickup(void) const;
    bool ExtHeroAllowTranscribingScroll(void) const;
    bool ExtHeroAutoMove2BattleTarget(void) const;
    bool ExtHeroAllowBannedSecSkillsUpgrade(void) const;
    bool ExtHeroArenaCanChoiseAnySkills(void) const;
    bool ExtUnionsAllowCastleVisiting(void) const;
    bool ExtUnionsAllowHeroesMeetings(void) const;
    bool ExtUnionsAllowViewMaps(void) const;
    bool ExtAllowCastleGuardians(void) const;
    bool ExtWorldBanMonthOfMonsters(void) const;
    bool ExtWorldBanWeekOf(void) const;
    bool ExtWorldNewVersionWeekOf(void) const;
    bool ExtWorldBanPlagues(void) const;
    bool ExtWorldArtesianSpringSeparatelyVisit(void) const;
    bool ExtWorldStartHeroLossCond4Humans(void) const;
    bool ExtWorldOneHeroHiredEveryWeek(void) const;
    bool ExtWorldDwellingsAccumulateUnits(void) const;
    bool ExtWorldUseUniqueArtifacts1(void) const;
    bool ExtWorldUseUniqueArtifacts2(void) const;
    bool ExtWorldWindWaterMillsCaptured(void) const;
    bool ExtWorldGuardianObjectsTwoDefense(void) const;
    bool ExtWorldEnhancedArtifactInfo(void) const;
    bool ExtCastleAllowFlash(void) const;
    bool ExtCastleGuildRestorePointsTurn(void) const;
    bool ExtCastleAllowRecruitSpecialHeroes(void) const;
    bool ExtBattleShowDamage(void) const;
    bool ExtBattleTroopDirection(void) const;
    bool ExtBattleSoftWait(void) const;
    bool ExtBattleMagicTroopCanResist(void) const;
    bool ExtBattleArchmageCanResistBadMagic(void) const;
    bool ExtBattleObjectsArchersPenalty(void) const;
    bool ExtBattleMergeArmies(void) const;
    bool ExtBattleSkipIncreaseDefense(void) const;
    bool ExtBattleReverseWaitOrder(void) const;
    bool ExtRewriteConfirm(void) const;
    bool ExtAutosaveConfirm(void) const;
    bool ExtHideCursor(void) const;
    bool ExtBattleShowGrid(void) const;
    bool ExtBattleShowMouseShadow(void) const;
    bool ExtBattleShowMoveShadow(void) const;
    bool ExtShowSystemInfo(void) const;
    bool ExtAutoSaveBeginOfDay(void) const;
    bool ExtAutoSaveOn(void) const;
    bool ExtUseFade(void) const;
    bool ExtShowSDL(void) const;
    bool ExtLowMemory(void) const;
    bool ExtTapMode(void) const;
    bool ExtDragDropScroll(void) const;
    bool ExtLowResolution(void) const;
    bool EvilInterface(void) const;
    bool DynamicInterface(void) const;
    bool HideInterface(void) const;

    bool NetworkDedicatedServer(void) const;
    bool NetworkLocalClient(void) const;

    const Size & VideoMode(void) const;
    void SetAutoVideoMode(void);

    u8   SoundVolume(void) const;
    u8   MusicVolume(void) const;

    void SetDebug(const u16 d);
    void SetEditor(void);
    void SetUnicode(bool);
    void SetPriceLoyaltyVersion(void);
    void SetGameDifficulty(u8);
    void SetEvilInterface(bool);
    void SetBattleGrid(bool);
    void SetBattleMovementShaded(bool);
    void SetBattleMouseShaded(bool);
    void SetShowPanel(bool);
    void SetShowRadar(bool);
    void SetShowIcons(bool);
    void SetShowButtons(bool);
    void SetShowStatus(bool);
    void SetNetworkLocalClient(bool);
    void SetNetworkDedicatedServer(bool);
    void SetMemoryLimit(u32);
    void SetAIMoveSpeed(u8);
    void SetScrollSpeed(u8);
    void SetHeroesMoveSpeed(u8);
    void SetBattleSpeed(u8);

    void SetSoundVolume(const u8 v);
    void SetMusicVolume(const u8 v);
    void ResetSound(void);
    void ResetMusic(void);
    
    bool GameType(u8) const;
    u8   GameType(void) const;
    void SetGameType(u8);

    Players & GetPlayers(void);
    const Players & GetPlayers(void) const;

    const u8 & CurrentColor(void) const;
    void SetCurrentColor(u8);
    u8   PreferablyCountPlayers(void) const;
    void SetPreferablyCountPlayers(u8 c);

    void SetLocalPrefix(const char*);

    u16	GetPort(void) const;

    // from maps info
    bool AllowChangeRace(u8) const;
    const std::string & MapsFile(void) const;
    const std::string & MapsName(void) const;
    const std::string & MapsDescription(void) const;
    u8  MapsDifficulty(void) const;
    u16 MapsWidth(void) const;
    bool GameStartWithHeroes(void) const;
    u16 ConditionWins(void) const;
    u16 ConditionLoss(void) const;
    bool WinsCompAlsoWins(void) const;
    bool WinsAllowNormalVictory(void) const;
    u8 WinsFindArtifactID(void) const;
    bool WinsFindUltimateArtifact(void) const;
    u16 WinsSidePart(void) const;
    u32 WinsAccumulateGold(void) const;
    u32 WinsMapsIndexObject(void) const;
    u32 LossMapsIndexObject(void) const;
    u16 LossCountDays(void) const;

    static std::string GetVersion(void);

protected:
    void Parse(const std::string & left, const std::string & right);
    void PostLoad(void);

private:
    friend class Game::IO;

    Settings();
    ~Settings();

    BitModes opt_global;

    BitModes opt_game;
    BitModes opt_battle;
    BitModes opt_world;
    BitModes opt_addons;

    u16 debug;

    Size video_mode;

    u8 game_difficulty;

    std::string path_data_directory;
    std::string local_prefix;

    ListMapsDirectory list_maps_directory;

    std::string font_normal;
    std::string font_small;
    std::string force_lang;
    std::string maps_charset;
    u8 size_normal;
    u8 size_small;

    Maps::FileInfo current_maps_file;

    u8 sound_volume;
    u8 music_volume;
    u8 heroes_speed;
    u8 ai_speed;
    u8 scroll_speed;
    u8 battle_speed;

    u8 game_type;
    u8 preferably_count_players;

    std::string playmus_command;
    std::string video_driver;

    u16 port;
    
    u32 memory_limit;

    Point pos_radr;
    Point pos_bttn;
    Point pos_icon;
    Point pos_stat;

    Players players;
};

#endif
