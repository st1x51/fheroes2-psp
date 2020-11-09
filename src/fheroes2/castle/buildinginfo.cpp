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

#include <bitset>
#include "agg.h"
#include "monster.h"
#include "settings.h"
#include "cursor.h"
#include "button.h"
#include "world.h"
#include "race.h"
#include "kingdom.h"
#include "payment.h"
#include "profit.h"
#include "statusbar.h"
#include "buildinginfo.h"

#ifdef WITH_XML
#include "xmlccwrap.h"
#endif

struct buildstats_t
{
    const u32   id2;
    const u8    race;
    cost_t      cost;
};

buildstats_t _builds[] = {
    // id                                             gold wood mercury ore sulfur crystal gems
    { BUILD_THIEVESGUILD, Race::ALL, { 750, 5, 0, 0, 0, 0, 0 } },
    { BUILD_TAVERN,       Race::ALL, { 500, 5, 0, 0, 0, 0, 0 } },
    { BUILD_SHIPYARD,     Race::ALL, {2000,20, 0, 0, 0, 0, 0 } },
    { BUILD_WELL,         Race::ALL, { 500, 0, 0, 0, 0, 0, 0 } },
    { BUILD_STATUE,       Race::ALL, {1250, 0, 0, 5, 0, 0, 0 } },
    { BUILD_LEFTTURRET,   Race::ALL, {1500, 0, 0, 5, 0, 0, 0 } },
    { BUILD_RIGHTTURRET,  Race::ALL, {1500, 0, 0, 5, 0, 0, 0 } },
    { BUILD_MARKETPLACE,  Race::ALL, { 500, 5, 0, 0, 0, 0, 0 } },
    { BUILD_MOAT,         Race::ALL, { 750, 0, 0, 0, 0, 0, 0 } },
    { BUILD_CASTLE,       Race::ALL, {5000,20, 0,20, 0, 0, 0 } },
    { BUILD_CAPTAIN,      Race::ALL, { 500, 0, 0, 0, 0, 0, 0 } },
    { BUILD_MAGEGUILD1,   Race::ALL, {2000, 5, 0, 5, 0, 0, 0 } },
    { BUILD_MAGEGUILD2,   Race::ALL, {1000, 5, 4, 5, 4, 4, 4 } },
    { BUILD_MAGEGUILD3,   Race::ALL, {1000, 5, 6, 5, 6, 6, 6 } },
    { BUILD_MAGEGUILD4,   Race::ALL, {1000, 5, 8, 5, 8, 8, 8 } },
    { BUILD_MAGEGUILD5,   Race::ALL, {1000, 5,10, 5,10,10,10 } },

    { BUILD_WEL2, Race::KNGT, {1000, 0, 0, 0, 0, 0, 0 } },
    { BUILD_WEL2, Race::BARB, {1000, 0, 0, 0, 0, 0, 0 } },
    { BUILD_WEL2, Race::SORC, {1000, 0, 0, 0, 0, 0, 0 } },
    { BUILD_WEL2, Race::WRLK, {1000, 0, 0, 0, 0, 0, 0 } },
    { BUILD_WEL2, Race::WZRD, {1000, 0, 0, 0, 0, 0, 0 } },
    { BUILD_WEL2, Race::NECR, {1000, 0, 0, 0, 0, 0, 0 } },

    { BUILD_SPEC, Race::KNGT, {1500, 5, 0,15, 0, 0, 0 } },
    { BUILD_SPEC, Race::BARB, {2000,10, 0,10, 0, 0, 0 } },
    { BUILD_SPEC, Race::SORC, {1500, 0, 0, 0, 0,10, 0 } },
    { BUILD_SPEC, Race::WRLK, {3000, 5, 0,10, 0, 0, 0 } },
    { BUILD_SPEC, Race::WZRD, {1500, 5, 5, 5, 5, 5, 5 } },
    { BUILD_SPEC, Race::NECR, {1000, 0,10, 0,10, 0, 0 } },

    { BUILD_SHRINE, Race::NECR, {4000,10, 0, 0, 0,10, 0 } },

    { DWELLING_MONSTER1, Race::KNGT, { 200, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER2, Race::KNGT, {1000, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE2, Race::KNGT, {1500, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER3, Race::KNGT, {1000, 0, 0, 5, 0, 0, 0 } },
    { DWELLING_UPGRADE3, Race::KNGT, {1500, 0, 0, 5, 0, 0, 0 } },
    { DWELLING_MONSTER4, Race::KNGT, {2000,10, 0,10, 0, 0, 0 } },
    { DWELLING_UPGRADE4, Race::KNGT, {2000, 5, 0, 5, 0, 0, 0 } },
    { DWELLING_MONSTER5, Race::KNGT, {3000,20, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE5, Race::KNGT, {3000,10, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER6, Race::KNGT, {5000,20, 0, 0, 0,20, 0 } },
    { DWELLING_UPGRADE6, Race::KNGT, {5000,10, 0, 0, 0,10, 0 } },

    { DWELLING_MONSTER1, Race::BARB, { 300, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER2, Race::BARB, { 800, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE2, Race::BARB, {1200, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER3, Race::BARB, {1000, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER4, Race::BARB, {2000,10, 0,10, 0, 0, 0 } },
    { DWELLING_UPGRADE4, Race::BARB, {3000, 5, 0, 5, 0, 0, 0 } },
    { DWELLING_MONSTER5, Race::BARB, {4000, 0, 0,20, 0, 0, 0 } },
    { DWELLING_UPGRADE5, Race::BARB, {2000, 0, 0,10, 0, 0, 0 } },
    { DWELLING_MONSTER6, Race::BARB, {6000, 0, 0,20, 0,20, 0 } },

    { DWELLING_MONSTER1, Race::SORC, { 500, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER2, Race::SORC, {1000, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE2, Race::SORC, {1500, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER3, Race::SORC, {1500, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE3, Race::SORC, {1500, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER4, Race::SORC, {1500, 0, 0,10, 0, 0, 0 } },
    { DWELLING_UPGRADE4, Race::SORC, {1500, 0, 5, 0, 0, 0, 0 } },
    { DWELLING_MONSTER5, Race::SORC, {3000,10, 0, 0, 0, 0,10 } },
    { DWELLING_MONSTER6, Race::SORC, {10000, 0,20,30, 0, 0, 0 } },

    { DWELLING_MONSTER1, Race::WRLK, { 500, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER2, Race::WRLK, {1000, 0, 0,10, 0, 0, 0 } },
    { DWELLING_MONSTER3, Race::WRLK, {2000, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER4, Race::WRLK, {3000, 0, 0, 0, 0, 0,10 } },
    { DWELLING_UPGRADE4, Race::WRLK, {2000, 0, 0, 0, 0, 0, 5 } },
    { DWELLING_MONSTER5, Race::WRLK, {4000, 0, 0, 0,10, 0, 0 } },
    { DWELLING_MONSTER6, Race::WRLK, {15000,0, 0,30,20, 0, 0 } },
    { DWELLING_UPGRADE6, Race::WRLK, {5000, 0, 0, 5,10, 0, 0 } },
    { DWELLING_UPGRADE7, Race::WRLK, {5000, 0, 0, 5,10, 0, 0 } },

    { DWELLING_MONSTER1, Race::WZRD, { 400, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER2, Race::WZRD, { 800, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER3, Race::WZRD, {1500, 5, 0, 5, 0, 0, 0 } },
    { DWELLING_UPGRADE3, Race::WZRD, {1500, 0, 5, 0, 0, 0, 0 } },
    { DWELLING_MONSTER4, Race::WZRD, {3000, 5, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER5, Race::WZRD, {3500, 5, 5, 5, 5, 5, 5 } },
    { DWELLING_UPGRADE5, Race::WZRD, {4000, 5, 0, 5, 0, 0, 0 } },
    { DWELLING_MONSTER6, Race::WZRD, {12500,5, 0, 5, 0, 0,20 } },
    { DWELLING_UPGRADE6, Race::WZRD, {12500,5, 0, 5, 0, 0,20 } },

    { DWELLING_MONSTER1, Race::NECR, { 400, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER2, Race::NECR, {1000, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE2, Race::NECR, {1000, 0, 0, 0, 0, 0, 0 } },
    { DWELLING_MONSTER3, Race::NECR, {1500, 0, 0,10, 0, 0, 0 } },
    { DWELLING_UPGRADE3, Race::NECR, {1500, 0, 0, 5, 0, 0, 0 } },
    { DWELLING_MONSTER4, Race::NECR, {3000,10, 0, 0, 0, 0, 0 } },
    { DWELLING_UPGRADE4, Race::NECR, {4000, 5, 0, 0, 0,10,10 } },
    { DWELLING_MONSTER5, Race::NECR, {4000,10, 0, 0,10, 0, 0 } },
    { DWELLING_UPGRADE5, Race::NECR, {3000, 0, 0, 5, 0, 5, 0 } },
    { DWELLING_MONSTER6, Race::NECR, {10000,10,5,10, 5, 5, 5 } },

    // end
    { BUILD_NOTHING, Race::NONE, { 0, 0, 0, 0, 0, 0, 0 } },
};

void BuildingInfo::UpdateCosts(const std::string & spec)
{
#ifdef WITH_XML
    // parse buildings.xml
    TiXmlDocument doc;
    const TiXmlElement* xml_buildings = NULL;
    size_t index = 0;

    if(doc.LoadFile(spec.c_str()) &&
        NULL != (xml_buildings = doc.FirstChildElement("buildings")))
    {
        for(const TiXmlElement* xml_building = xml_buildings->FirstChildElement("building");
    		xml_building && BUILD_NOTHING != _builds[index].id2; xml_building = xml_building->NextSiblingElement("building"), ++index)
        {
    	    cost_t & cost = _builds[index].cost;
	    int value;

            xml_building->Attribute("gold", &value); cost.gold = value;
            xml_building->Attribute("wood", &value); cost.wood = value;
            xml_building->Attribute("mercury", &value); cost.mercury = value;
            xml_building->Attribute("ore", &value); cost.ore = value;
            xml_building->Attribute("sulfur", &value); cost.sulfur = value;
            xml_building->Attribute("crystal", &value); cost.crystal = value;
            xml_building->Attribute("gems", &value); cost.gems = value;
	}
    }
    else
    VERBOSE(spec << ": " << doc.ErrorDesc());
#endif
}

payment_t BuildingInfo::GetCost(u32 build, u8 race)
{
    payment_t payment;
    const buildstats_t* ptr = &_builds[0];

    while(BUILD_NOTHING != ptr->id2 && !(ptr->id2 == build && (!race || (race & ptr->race)))) ++ptr;

    if(BUILD_NOTHING != ptr->id2)
    {
	payment.gold = ptr->cost.gold;
	payment.wood = ptr->cost.wood;
	payment.mercury = ptr->cost.mercury;
	payment.ore = ptr->cost.ore;
	payment.sulfur = ptr->cost.sulfur;
	payment.crystal = ptr->cost.crystal;
	payment.gems = ptr->cost.gems;
    }

    return payment;
}

u8 GetIndexBuildingSprite(u32 build)
{
    switch(build)
    {
	case DWELLING_MONSTER1:	return 19;
	case DWELLING_MONSTER2: return 20;
	case DWELLING_MONSTER3: return 21;
	case DWELLING_MONSTER4: return 22;
	case DWELLING_MONSTER5: return 23; 
	case DWELLING_MONSTER6: return 24;
	case DWELLING_UPGRADE2: return 25;
	case DWELLING_UPGRADE3: return 26;
	case DWELLING_UPGRADE4: return 27;
	case DWELLING_UPGRADE5: return 28;
	case DWELLING_UPGRADE6: return 29;
	case DWELLING_UPGRADE7: return 30;
	case BUILD_MAGEGUILD1:
	case BUILD_MAGEGUILD2:
	case BUILD_MAGEGUILD3:
	case BUILD_MAGEGUILD4:
	case BUILD_MAGEGUILD5:	return 0;
	case BUILD_THIEVESGUILD:return 1;
	case BUILD_SHRINE:
	case BUILD_TAVERN:	return 2;
	case BUILD_SHIPYARD:	return 3;
	case BUILD_WELL:	return 4;
	case BUILD_CASTLE:	return 6;
	case BUILD_STATUE:	return 7;
	case BUILD_LEFTTURRET:	return 8;
	case BUILD_RIGHTTURRET:	return 9;
	case BUILD_MARKETPLACE:	return 10;
	case BUILD_WEL2:	return 11;
	case BUILD_MOAT:	return 12;
	case BUILD_SPEC:	return 13;
	case BUILD_CAPTAIN:	return 15;
	default: break;
    }

    return 0;
}

BuildingInfo::BuildingInfo(const Castle & c, building_t b) : castle(c), building(b), area(0, 0, 135, 57), disable(false)
{
    if(IsDwelling()) building = castle.GetActualDwelling(b);

    building = castle.isBuild(b) ? castle.GetUpgradeBuilding(b) : b;

    if(IsDwelling())
    {
	description = _("The %{building} produces %{monster}.");
        String::Replace(description, "%{building}", Castle::GetStringBuilding(building, castle.GetRace()));
        String::Replace(description, "%{monster}", String::Lower(Monster(castle.GetRace(), building).GetMultiName()));
    }
    else
	description = Castle::GetDescriptionBuilding(building, castle.GetRace());

    switch(building)
    {
	case BUILD_WELL:
    	    String::Replace(description, "%{count}", Castle::GetGrownWell());
	    break;

	case BUILD_WEL2:
    	    String::Replace(description, "%{count}", Castle::GetGrownWel2());
	    break;

	case BUILD_CASTLE:
	case BUILD_STATUE:
	case BUILD_SPEC:
	{
	    const payment_t profit = ProfitConditions::FromBuilding(building, castle.GetRace());
	    String::Replace(description, "%{count}", profit.gold);
	    break;
	}

	default: break;
    }

    // necr and tavern check
    if(BUILD_SHRINE == building && (Race::NECR != castle.GetRace() || !Settings::Get().PriceLoyaltyVersion()))
	disable = true;

    if(b == BUILD_CAPTAIN)
    {
	const Sprite & sprite = AGG::GetICN(ICN::Get4Captain(castle.GetRace()),
						(building & BUILD_CAPTAIN ? 1 : 0));
	area.w = sprite.w();
	area.h = sprite.h();
    }
}

u32 BuildingInfo::operator() (void) const
{
    return building;
}

void BuildingInfo::SetPos(s16 x, s16 y)
{
    area.x = x;
    area.y = y;
}

const Rect & BuildingInfo::GetArea(void) const
{
    return area;
}

bool BuildingInfo::IsDisable(void) const
{
    return disable;
}

bool BuildingInfo::IsDwelling(void) const
{
    switch(building)
    {
	case DWELLING_MONSTER1:
	case DWELLING_MONSTER2:
	case DWELLING_MONSTER3:
	case DWELLING_MONSTER4:
	case DWELLING_MONSTER5:
	case DWELLING_MONSTER6:
	case DWELLING_UPGRADE2:
	case DWELLING_UPGRADE3:
	case DWELLING_UPGRADE4:
	case DWELLING_UPGRADE5:
	case DWELLING_UPGRADE6:
	case DWELLING_UPGRADE7:
	    return true;
	default: break;
    }
    return false;
}

void BuildingInfo::RedrawCaptain(void)
{
    AGG::GetICN(ICN::Get4Captain(castle.GetRace()),
				(building & BUILD_CAPTAIN ? 1 : 0)).Blit(area.x, area.y);

    const Sprite & sprite_allow = AGG::GetICN(ICN::TOWNWIND, 11);
    const Sprite & sprite_deny  = AGG::GetICN(ICN::TOWNWIND, 12);
    const Sprite & sprite_money = AGG::GetICN(ICN::TOWNWIND, 13);
    Point dst_pt;

    buildcond_t build_condition = castle.CheckBuyBuilding(building);
    bool allow_buy = build_condition == ALLOW_BUILD;

    // indicator
    dst_pt.x = area.x + 65;
    dst_pt.y = area.y + 60;
    if(castle.isBuild(building)) sprite_allow.Blit(dst_pt);
    else
    if(! allow_buy)
    {
	if(LACK_RESOURCES == build_condition)
	    sprite_money.Blit(dst_pt);
	else
	    sprite_deny.Blit(dst_pt);
    }
}

void BuildingInfo::Redraw(void)
{
    if(BUILD_CAPTAIN == building)
    {
	RedrawCaptain();
	return;
    }

    u8 index = GetIndexBuildingSprite(building);
    Display & display = Display::Get();

    AGG::GetICN(ICN::BLDGXTRA, 0).Blit(area.x, area.y);

    if(disable)
    {
	display.FillRect(0, 0, 0, Rect(area.x + 1, area.y + 1, area.w, area.h));
	return;
    }

    AGG::GetICN(ICN::Get4Building(castle.GetRace()),
		index).Blit(area.x + 1, area.y + 1);

    const Sprite & sprite_allow = AGG::GetICN(ICN::TOWNWIND, 11);
    const Sprite & sprite_deny  = AGG::GetICN(ICN::TOWNWIND, 12);
    const Sprite & sprite_money = AGG::GetICN(ICN::TOWNWIND, 13);
    Point dst_pt;

    buildcond_t build_condition = castle.CheckBuyBuilding(building);
    bool allow_buy = build_condition == ALLOW_BUILD;

    // indicator
    dst_pt.x = area.x + 115;
    dst_pt.y = area.y + 40;
    if(castle.isBuild(building)) sprite_allow.Blit(dst_pt);
    else
    if(! allow_buy)
    {
	if(LACK_RESOURCES == build_condition)
	    sprite_money.Blit(dst_pt);
	else
	    sprite_deny.Blit(dst_pt);
    }

    // status bar
    if(!castle.isBuild(building))
    {
	dst_pt.x = area.x;
	dst_pt.y = area.y + 58;
	AGG::GetICN(ICN::CASLXTRA, allow_buy ? 1 : 2).Blit(dst_pt);
    }

    // name
    Text text(Castle::GetStringBuilding(building, castle.GetRace()), Font::SMALL);
    dst_pt.x = area.x + 68 - text.w() / 2;
    dst_pt.y = area.y + 59;
    text.Blit(dst_pt);
}


const char* BuildingInfo::GetName(void) const
{
    return Castle::GetStringBuilding(building, castle.GetRace());
}

const std::string & BuildingInfo::GetDescription(void) const
{
    return description;
}

bool BuildingInfo::QueueEventProcessing(void)
{
    if(disable) return false;

    LocalEvent & le = LocalEvent::Get();
    if(le.MouseClickLeft(area))
    {
	if(!castle.isBuild(BUILD_CASTLE))
	    Dialog::Message("", _("For this action it is necessary first to build a castle."), Font::BIG, Dialog::OK);
	else
	if(castle.isBuild(building))
	    Dialog::Message(GetName(), GetDescription(), Font::BIG, Dialog::OK);
	else
	    return DialogBuyBuilding(true);
    }
    else
    if(le.MousePressRight(area))
    {
	if(castle.isBuild(building))
	    Dialog::Message(GetName(), GetDescription(), Font::BIG);
	else
	    DialogBuyBuilding(false);
    }
    return false;
}

bool BuildingInfo::DialogBuyBuilding(bool buttons) const
{
    Display & display = Display::Get();

    const ICN::icn_t system = (Settings::Get().EvilInterface() ? ICN::SYSTEME : ICN::SYSTEM);

    Cursor & cursor = Cursor::Get();
    cursor.Hide();

    TextBox box1(description, Font::BIG, BOXAREA_WIDTH);

    // prepare requires build string
    std::string str;
    std::bitset<32> requires(castle.GetBuildingRequires(building));
    if(requires.any())
    {
	u8 count = 0;
	for(u8 pos = 0; pos < requires.size(); ++pos)
	{
	    if(requires.test(pos))
	    {
		u32 value = 1;
		value <<= pos;
		    
		++count;

		if(! castle.isBuild(value))
		{
		    str += Castle::GetStringBuilding(static_cast<building_t>(value), castle.GetRace());
		    if(count < requires.count()) str += ", ";
		}
	    }
	}
    }

    bool requires_true = str.size();
    Text requires_text(_("Requires:"), Font::BIG);
    TextBox box2(str, Font::BIG, BOXAREA_WIDTH);

    Resource::BoxSprite rbs(PaymentConditions::BuyBuilding(castle.GetRace(), building), BOXAREA_WIDTH);

    const Sprite & window_icons = AGG::GetICN(ICN::BLDGXTRA, 0);
    const u8 space = Settings::Get().QVGA() ? 5 : 10;
    Dialog::Box box(space + window_icons.h() + space + box1.h() + space + (requires_true ? requires_text.h() + box2.h() + space : 0) + rbs.GetArea().h, buttons);
    const Rect & box_rt = box.GetArea();
    LocalEvent & le = LocalEvent::Get();

    Point dst_pt;

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + box_rt.h - AGG::GetICN(system, 1).h();
    Button button1(dst_pt, system, 1, 2);

    dst_pt.x = box_rt.x + box_rt.w - AGG::GetICN(system, 3).w();
    dst_pt.y = box_rt.y + box_rt.h - AGG::GetICN(system, 3).h();
    Button button2(dst_pt, system, 3, 4);

    dst_pt.x = box_rt.x + (box_rt.w - window_icons.w()) / 2;
    dst_pt.y = box_rt.y + space;
    window_icons.Blit(dst_pt);

    const Sprite & building_icons = AGG::GetICN(ICN::Get4Building(castle.GetRace()),
					    GetIndexBuildingSprite(building));
    dst_pt.x = box_rt.x + (box_rt.w - building_icons.w()) / 2;
    dst_pt.y += 1;
    building_icons.Blit(dst_pt);

    Text text(GetName(), Font::SMALL);
    dst_pt.x = box_rt.x + (box_rt.w - text.w()) / 2;
    dst_pt.y += 57;
    text.Blit(dst_pt);

    dst_pt.x = box_rt.x;
    dst_pt.y = box_rt.y + space + window_icons.h() + space;
    box1.Blit(dst_pt);

    dst_pt.y += box1.h() + space;
    if(requires_true)
    {
	dst_pt.x = box_rt.x + (box_rt.w - requires_text.w()) / 2;
	requires_text.Blit(dst_pt);

	dst_pt.x = box_rt.x;
	dst_pt.y += requires_text.h();
	box2.Blit(dst_pt);

	dst_pt.y += box2.h() + space;
    }

    rbs.SetPos(dst_pt.x, dst_pt.y);
    rbs.Redraw();

    if(buttons)
    {
	if(ALLOW_BUILD != castle.CheckBuyBuilding(building))
	    button1.SetDisable(true);

	button1.Draw();
	button2.Draw();
    }

    cursor.Show();
    display.Flip();

    // message loop
    while(le.HandleEvents())
    {
        if(!buttons && !le.MousePressRight()) break;

        le.MousePressLeft(button1) ? button1.PressDraw() : button1.ReleaseDraw();
        le.MousePressLeft(button2) ? button2.PressDraw() : button2.ReleaseDraw();

        if(button1.isEnable() &&
	    (Game::HotKeyPress(Game::EVENT_DEFAULT_READY) ||
    	    le.MouseClickLeft(button1))) return true;

        if(Game::HotKeyPress(Game::EVENT_DEFAULT_EXIT) ||
    	    le.MouseClickLeft(button2)) break;
    }

    return false;
}

void BuildingInfo::SetStatusMessage(StatusBar & bar) const
{
    std::string str;
    const char* name = GetName();

    if(castle.isBuild(building))
    {
        str = _("%{name} is already built");
        String::Replace(str, "%{name}", name);
    }
    else
    {
        if(!castle.AllowBuild())
        {
            str = _("Cannot build. Already built here this turn.");
        }
        else
        if(castle.AllowBuild() && ! world.GetKingdom(castle.GetColor()).AllowPayment(GetCost(castle.GetRace(), building)))
        {
            str = _("Cannot afford %{name}");
            String::Replace(str, "%{name}", name);
        }
        else
        if(BUILD_SHIPYARD == building && !castle.HaveNearlySea())
        {
            str = _("Cannot build %{name} because castle is too far from water.");
            String::Replace(str, "%{name}", name);
        }
        else
        if(!castle.AllowBuyBuilding(building))
        {
            str = _("Cannot build %{name}");
            String::Replace(str, "%{name}", name);
        }
        else
        {
            str = _("Build %{name}");
            String::Replace(str, "%{name}", name);
        }
    }

    bar.ShowMessage(str);
}
