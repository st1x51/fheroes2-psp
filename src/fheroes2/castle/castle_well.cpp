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

#include <string>
#include "agg.h"
#include "button.h"
#include "cursor.h"
#include "speed.h"
#include "settings.h"
#include "resource.h"
#include "castle.h"
#include "kingdom.h"
#include "world.h"
#include "text.h"

struct dwelling_t : public std::pair<u32, u16>
{
    dwelling_t(u32 type, u16 count) : std::pair<u32, u16>(type, count){};
};

struct dwellings_t : public std::vector<dwelling_t>
{
    dwellings_t() { reserve(6); };
};

u16 HowManyRecruitMonster(const Castle & castle, u32 dw, const Funds & add, Funds & res)
{
    const Monster ms(castle.GetRace(), castle.GetActualDwelling(dw));
    const Kingdom & kingdom = world.GetKingdom(castle.GetColor());

    if(! castle.GetArmy().CanJoinTroop(ms)) return 0;
                                                                                                                         
    u16 count = castle.GetDwellingLivedCount(dw);
    payment_t payment;

    while(count)
    {
        payment = ms.GetCost() * count;
        res = payment;
        payment += add;
        if(kingdom.AllowPayment(payment)) break;
        --count;
    }

    return count;
}

void Castle::OpenWell(void)
{
    const Settings & conf = Settings::Get();
    Display & display = Display::Get();
    // cursor
    Cursor & cursor = Cursor::Get();

    cursor.Hide();

    Dialog::FrameBorder frameborder;
    frameborder.SetPosition((display.w() - 640 - BORDERWIDTH * 2) / 2, (display.h() - 480 - BORDERWIDTH * 2) / 2, 640, 480);
    frameborder.Redraw();
    
    const Point cur_pt(frameborder.GetArea().x, frameborder.GetArea().y);

    Point dst_pt(cur_pt);

    // button exit
    dst_pt.x = cur_pt.x + 578;
    dst_pt.y = cur_pt.y + 461;
    Button buttonExit(dst_pt, ICN::WELLXTRA, 0, 1);

    dst_pt.x = cur_pt.x;
    dst_pt.y = cur_pt.y + 461;
    Button buttonMax(dst_pt, ICN::BUYMAX, 0, 1);
    
    const Rect rectMonster1(cur_pt.x + 20, cur_pt.y + 18, 288, 124);
    const Rect rectMonster2(cur_pt.x + 20, cur_pt.y + 168, 288, 124);
    const Rect rectMonster3(cur_pt.x + 20, cur_pt.y + 318, 288, 124);
    const Rect rectMonster4(cur_pt.x + 334, cur_pt.y + 18, 288, 124);
    const Rect rectMonster5(cur_pt.x + 334, cur_pt.y + 168, 288, 124);
    const Rect rectMonster6(cur_pt.x + 334, cur_pt.y + 318, 288, 124);
    
    buttonExit.Draw();

    WellRedrawInfoArea(cur_pt);

    if(! conf.ExtAllowBuyFromWell())
	buttonMax.SetDisable(true);
    else
    {
	buttonMax.Draw();
    }

    std::vector<u32> alldwellings;
    alldwellings.reserve(6);
    alldwellings.push_back(DWELLING_MONSTER6);
    alldwellings.push_back(DWELLING_MONSTER5);
    alldwellings.push_back(DWELLING_MONSTER4);
    alldwellings.push_back(DWELLING_MONSTER3);
    alldwellings.push_back(DWELLING_MONSTER2);
    alldwellings.push_back(DWELLING_MONSTER1);

    cursor.Show();
    display.Flip();

    bool redraw = false;
    LocalEvent & le = LocalEvent::Get();

    // loop
    while(le.HandleEvents())
    {
        le.MousePressLeft(buttonExit) ? buttonExit.PressDraw() : buttonExit.ReleaseDraw();

	buttonMax.isEnable() &&
        le.MousePressLeft(buttonMax) ? buttonMax.PressDraw() : buttonMax.ReleaseDraw();

        if(le.MouseClickLeft(buttonExit) || HotKeyCloseWindow) break;

        // extended version (click - buy dialog monster)
        if(conf.ExtAllowBuyFromWell())
        {
	    if(buttonMax.isEnable() && le.MouseClickLeft(buttonMax))
	    {
		dwellings_t results;
		Funds cur, total;
		u16 can_recruit;
		std::string str;

		for(std::vector<u32>::const_iterator
		    it = alldwellings.begin(); it != alldwellings.end(); ++it)
		if(0 != (can_recruit = HowManyRecruitMonster(*this, *it, total, cur)))
		{
		    results.push_back(dwelling_t(*it, can_recruit));
		    total += cur;
		    const Monster ms(race, GetActualDwelling(*it));
		    str.append(ms.GetPluralName(can_recruit));
		    str.append(" - ");
		    String::AddInt(str, can_recruit);
		    str.append("\n");
		}

		if(str.empty()) str = "None";

		if(Dialog::YES ==
		    Dialog::ResourceInfo(_("Buy Monsters:"), str, total, Dialog::YES|Dialog::NO))
		{
		    for(dwellings_t::const_iterator
			it = results.begin(); it != results.end(); ++it)
		    {
			const dwelling_t & dw = *it;
			RecruitMonster(dw.first, dw.second);
		    }
		    redraw = true;
		}
	    }

    	    if(building & DWELLING_MONSTER1 && dwelling[0] && le.MouseClickLeft(rectMonster1) &&
    		RecruitMonster(DWELLING_MONSTER1, Dialog::RecruitMonster(
            	    Monster(race, DWELLING_MONSTER1), dwelling[0]))) redraw = true;
    	    else
    	    if(building & DWELLING_MONSTER2 && dwelling[1] && le.MouseClickLeft(rectMonster2) &&
    		RecruitMonster(DWELLING_MONSTER2, Dialog::RecruitMonster(
            	    Monster(race, GetActualDwelling(DWELLING_MONSTER2)), dwelling[1]))) redraw = true;
    	    else
    	    if(building & DWELLING_MONSTER3 && dwelling[2] && le.MouseClickLeft(rectMonster3) &&
    		RecruitMonster(DWELLING_MONSTER3, Dialog::RecruitMonster(
            	    Monster(race, GetActualDwelling(DWELLING_MONSTER3)), dwelling[2]))) redraw = true;
    	    else
    	    if(building & DWELLING_MONSTER4 && dwelling[3] && le.MouseClickLeft(rectMonster4) &&
    		RecruitMonster(DWELLING_MONSTER4, Dialog::RecruitMonster(
            	    Monster(race, GetActualDwelling(DWELLING_MONSTER4)), dwelling[3]))) redraw = true;
    	    else
    	    if(building & DWELLING_MONSTER5 && dwelling[4] && le.MouseClickLeft(rectMonster5) &&
    		RecruitMonster(DWELLING_MONSTER5, Dialog::RecruitMonster(
            	    Monster(race, GetActualDwelling(DWELLING_MONSTER5)), dwelling[4]))) redraw = true;
    	    else
    	    if(building & DWELLING_MONSTER6 && dwelling[5] && le.MouseClickLeft(rectMonster6) &&
                RecruitMonster(DWELLING_MONSTER6, Dialog::RecruitMonster(
                    Monster(race, GetActualDwelling(DWELLING_MONSTER6)), dwelling[5]))) redraw = true;

	    if(redraw)
	    {
        	cursor.Hide();
		WellRedrawInfoArea(cur_pt);
		buttonMax.Draw();
		cursor.Show();
        	display.Flip();
		redraw = false;
	    }
	}
    }
}

void Castle::WellRedrawInfoArea(const Point & cur_pt)
{
    AGG::GetICN(ICN::WELLBKG, 0).Blit(cur_pt);

    Text text;
    Point dst_pt, pt;

    if(Settings::Get().ExtAllowBuyFromWell())
    {
	const Sprite & button = AGG::GetICN(ICN::BUYMAX, 0);
	Rect src_rt(0, 461, button.w(), 19);
	AGG::GetICN(ICN::WELLBKG, 0).Blit(src_rt, cur_pt.x + button.w(), cur_pt.y + 461);
    }

    text.Set(_("Town Population Information and Statistics"), Font::BIG);
    dst_pt.x = cur_pt.x + 280 - text.w() / 2;
    dst_pt.y = cur_pt.y + 462;
    text.Blit(dst_pt);

    u32 dw = DWELLING_MONSTER1;

    while(dw <= DWELLING_MONSTER6)
    {
	bool present = false;
	u32 dw_orig = DWELLING_MONSTER1;
	u8 icnindex = 0;
	u16 available = 0;

	switch(dw)
	{
	    case DWELLING_MONSTER1:
		pt.x = cur_pt.x;
		pt.y = cur_pt.y;
		present = DWELLING_MONSTER1 & building;
		icnindex = 19;
		available = dwelling[0];
		break;
	    case DWELLING_MONSTER2:
		pt.x = cur_pt.x;
		pt.y = cur_pt.y + 150;
		present = DWELLING_MONSTER2 & building;
		dw_orig = GetActualDwelling(DWELLING_MONSTER2);
		icnindex = DWELLING_UPGRADE2 & building ? 25 : 20;
		available = dwelling[1];
		break;
	    case DWELLING_MONSTER3:
		pt.x = cur_pt.x;
		pt.y = cur_pt.y + 300;
		present = DWELLING_MONSTER3 & building;
		dw_orig = GetActualDwelling(DWELLING_MONSTER3);
		icnindex = DWELLING_UPGRADE3 & building ? 26 : 21;
		available = dwelling[2];
		break;
	    case DWELLING_MONSTER4:
		pt.x = cur_pt.x + 314;
		pt.y = cur_pt.y + 1;
		present = DWELLING_MONSTER4 & building;
		dw_orig = GetActualDwelling(DWELLING_MONSTER4);
		icnindex = DWELLING_UPGRADE4 & building ? 27 : 22;
		available = dwelling[3];
		break;
	    case DWELLING_MONSTER5:
		pt.x = cur_pt.x + 314;
		pt.y = cur_pt.y + 151;
		present = DWELLING_MONSTER5 & building;
		dw_orig = GetActualDwelling(DWELLING_MONSTER5);
		icnindex = DWELLING_UPGRADE5 & building ? 28 : 23;
		available = dwelling[4];
		break;
	    case DWELLING_MONSTER6:
		pt.x = cur_pt.x + 314;
		pt.y = cur_pt.y + 301;
		present = DWELLING_MONSTER6 & building;
		dw_orig = GetActualDwelling(DWELLING_MONSTER6);
		icnindex = DWELLING_UPGRADE7 & building ? 30 : (DWELLING_UPGRADE6 & building ? 29 : 24);
		available = dwelling[5];
		break;
	    default:
		break;
	}

	const Monster monster(race, dw_orig);

	// sprite
	dst_pt.x = pt.x + 21;
	dst_pt.y = pt.y + 35;
	AGG::GetICN(ICN::Get4Building(race), icnindex).Blit(dst_pt);
	// text
	text.Set(GetStringBuilding(dw_orig, race), Font::SMALL);
	dst_pt.x = pt.x + 86 - text.w() / 2;
	dst_pt.y = pt.y + 103;
	text.Blit(dst_pt);
	// monster
	const Sprite & smonster = AGG::GetICN(monster.ICNMonh(), 0);
	dst_pt.x = pt.x + 193 - smonster.w() / 2;
	dst_pt.y = pt.y + 124 - smonster.h();
	smonster.Blit(dst_pt);
	// name
	text.Set(monster.GetMultiName());
	dst_pt.x = pt.x + 122 - text.w() / 2;
	dst_pt.y = pt.y + 16;
	text.Blit(dst_pt);
	// attack
	std::string str;
	str = _("Attack");
	str += ": ";
	String::AddInt(str, monster.GetAttack());
	text.Set(str);
	dst_pt.x = pt.x + 268 - text.w() / 2;
	dst_pt.y = pt.y + 22;
	text.Blit(dst_pt);
	// defense
	str = _("Defense");
	str += ": ";
	String::AddInt(str, monster.GetDefense());
	text.Set(str);
	dst_pt.x = pt.x + 268 - text.w() / 2;
	dst_pt.y = pt.y + 34;
	text.Blit(dst_pt);
	// damage
	str = _("Damg");
	str += ": ";
	String::AddInt(str, monster.GetDamageMin());
	str += "-";
	String::AddInt(str, monster.GetDamageMax());
	text.Set(str);
	dst_pt.x = pt.x + 268 - text.w() / 2;
	dst_pt.y = pt.y + 46;
	text.Blit(dst_pt);
	// hp
	str = _("HP");
	str += ": ";
	String::AddInt(str, monster.GetHitPoints());
	text.Set(str);
	dst_pt.x = pt.x + 268 - text.w() / 2;
        dst_pt.y = pt.y + 58;
	text.Blit(dst_pt);
        // speed
        str = _("Speed");
        str += ": ";
	text.Set(str);
	dst_pt.x = pt.x + 268 - text.w() / 2;
        dst_pt.y = pt.y + 78;
	text.Blit(dst_pt);
	text.Set(Speed::String(monster.GetSpeed()));
	dst_pt.x = pt.x + 268 - text.w() / 2;
        dst_pt.y = pt.y + 90;
	text.Blit(dst_pt);
	
	if(present)
	{
	    u8 grown = monster.GetGrown();
	    grown += building & BUILD_WELL ? GetGrownWell() : 0;
	    if(DWELLING_MONSTER1 & dw) grown += building & BUILD_WEL2 ? GetGrownWel2() : 0;

    	    text.Set(_("Growth"));
	    dst_pt.x = pt.x + 268 - text.w() / 2;
    	    dst_pt.y = pt.y + 110;
    	    text.Blit(dst_pt);
	    str = "+ ";
	    String::AddInt(str, grown);
    	    str += " / ";
    	    str += _("week");
	    text.Set(str);
	    dst_pt.x = pt.x + 268 - text.w() / 2;
    	    dst_pt.y = pt.y + 122;
    	    text.Blit(dst_pt);

    	    str = _("Available");
    	    str += ": ";
    	    text.Set(str);
	    dst_pt.x = pt.x + 44;
    	    dst_pt.y = pt.y + 122;
    	    text.Blit(dst_pt);
	    str.clear();
	    String::AddInt(str, available);
	    text.Set(str, Font::BIG);
    	    dst_pt.x = pt.x + 129 - text.w() / 2;
    	    dst_pt.y = pt.y + 119;
	    text.Blit(dst_pt);
	}
	
	dw <<= 1;
    }
}
