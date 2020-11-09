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

#include "agg.h"
#include "settings.h"
#include "cursor.h"
#include "dialog.h"

#define  ANGLEWIDTH 44

void DrawBorder1(s16, s16, u16, u16, Surface &);
void DrawBorder2(s16, s16, u16, u16, const Surface &, Surface &);

Dialog::FrameBorder::FrameBorder() : border(BORDERWIDTH)
{
}

void Dialog::FrameBorder::SetBorder(u8 b)
{
    border = b;

    area.x = Rect::x + b;
    area.y = Rect::y + b;
    area.w = Rect::w > 2 * b ? Rect::w - 2 * b : 0;
    area.h = Rect::h > 2 * b ? Rect::h - 2 * b : 0;
}

bool Dialog::FrameBorder::isValid(void) const
{
    return Background::isValid();
}

void Dialog::FrameBorder::SetSize(u16 encw, u16 ench)
{
    Display & display = Display::Get();

    if(display.w() < encw || display.h() < ench || encw < (border * 4) || ench < (border * 4))
    DEBUG(DBG_GAME, DBG_WARN, "size: " << "out of range");

    Rect::w = encw + 2 * border;
    Rect::h = ench + 2 * border;
    area.w = encw;
    area.h = ench;
}

void Dialog::FrameBorder::SetPosition(s16 posx, s16 posy, u16 encw, u16 ench)
{
    if(Background::isValid()) Background::Restore();

    if(encw && ench)
    {
    	Background::Save(posx, posy, encw + 2 * border, ench + 2 * border);
	area.w = encw;
	area.h = ench;
    }
    else
    	Background::Save(posx, posy);

    area.x = posx + border;
    area.y = posy + border;
    top = Rect(posx, posy, area.w, border);
}

const Rect & Dialog::FrameBorder::GetTop(void) const
{
    return top;
}

const Rect & Dialog::FrameBorder::GetRect(void) const
{
    return Background::GetRect();
}

const Rect & Dialog::FrameBorder::GetArea(void) const
{
    return area;
}

void Dialog::FrameBorder::Redraw(void)
{
    DrawBorder1(GetRect().x, GetRect().y, GetRect().w, GetRect().h, Display::Get());
}

void Dialog::FrameBorder::Redraw(const Surface & sf)
{
    DrawBorder2(GetRect().x, GetRect().y, GetRect().w, GetRect().h, sf, Display::Get());
}

void Dialog::FrameBorder::Redraw(const Rect & rt, const Surface & sf)
{
    DrawBorder2(rt.x, rt.y, rt.w, rt.h, sf, Display::Get());
}

Dialog::FrameBorder::~FrameBorder()
{
    if(Cursor::Get().isVisible()){ Cursor::Get().Hide(); };
    Background::Restore();
}

void DrawBorder1(s16 posx, s16 posy, u16 posw, u16 posh, Surface & sf)
{
    const Sprite & surdbkg = (Settings::Get().EvilInterface() ? AGG::GetICN(ICN::SURDRBKE, 0) : AGG::GetICN(ICN::SURDRBKG, 0));
    Rect  src_rt;
    Point dst_pt;

    // top left angle
    src_rt = Rect(SHADOWWIDTH, 0, ANGLEWIDTH, BORDERWIDTH);
    dst_pt = Point(posx, posy);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // top bar
    src_rt = Rect(SHADOWWIDTH + ANGLEWIDTH + 20, 0, BORDERWIDTH * 2, BORDERWIDTH);
    dst_pt = Point(posx + ANGLEWIDTH, posy);
    while(dst_pt.x < posx + posw - BORDERWIDTH * 2)
    {
	surdbkg.Blit(src_rt, dst_pt, sf);
	dst_pt.x += src_rt.w;
    }

    // top right angle
    src_rt = Rect(surdbkg.w() - ANGLEWIDTH, 0, ANGLEWIDTH, BORDERWIDTH);
    dst_pt = Point(posx + posw - src_rt.w, posy);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // bottom left angle
    src_rt = Rect(SHADOWWIDTH, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH, ANGLEWIDTH, BORDERWIDTH);
    dst_pt = Point(posx, posy + posh - src_rt.h);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // bottom bar
    src_rt = Rect(SHADOWWIDTH + ANGLEWIDTH, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH, BORDERWIDTH * 2, BORDERWIDTH);
    dst_pt = Point(posx + ANGLEWIDTH, posy + posh - src_rt.h);
    while(dst_pt.x < posx + posw - BORDERWIDTH * 2)
    {
        surdbkg.Blit(src_rt, dst_pt, sf);
        dst_pt.x += src_rt.w;
    }

    // bottom right angle
    src_rt = Rect(surdbkg.w() - ANGLEWIDTH, surdbkg.h() - SHADOWWIDTH - BORDERWIDTH, ANGLEWIDTH, BORDERWIDTH);
    dst_pt = Point(posx + posw - src_rt.w, posy + posh - src_rt.h);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // left top angle
    src_rt = Rect(SHADOWWIDTH, 0, BORDERWIDTH, ANGLEWIDTH);
    dst_pt = Point(posx, posy);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // left bar
    src_rt = Rect(SHADOWWIDTH, ANGLEWIDTH, BORDERWIDTH, BORDERWIDTH * 2);
    dst_pt = Point(posx, posy + ANGLEWIDTH);
    while(dst_pt.y < posy + posh - BORDERWIDTH * 3)
    {
	surdbkg.Blit(src_rt, dst_pt, sf);
	dst_pt.y += src_rt.h;
    }

    // left bottom angle
    src_rt = Rect(SHADOWWIDTH, surdbkg.h() - SHADOWWIDTH - ANGLEWIDTH, BORDERWIDTH, ANGLEWIDTH);
    dst_pt = Point(posx, posy + posh - src_rt.h);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // right top angle
    src_rt = Rect(surdbkg.w() - BORDERWIDTH, 0, BORDERWIDTH, ANGLEWIDTH);
    dst_pt = Point(posx + posw - src_rt.w, posy);
    surdbkg.Blit(src_rt, dst_pt, sf);

    // right bar
    src_rt = Rect(surdbkg.w() - BORDERWIDTH, ANGLEWIDTH, BORDERWIDTH, ANGLEWIDTH);
    dst_pt = Point(posx + posw - src_rt.w, posy + ANGLEWIDTH);
    while(dst_pt.y < posy + posh - BORDERWIDTH * 3)
    {
	surdbkg.Blit(src_rt, dst_pt, sf);
	dst_pt.y += src_rt.h;
    }

    // right bottom angle
    src_rt = Rect(surdbkg.w() - BORDERWIDTH, surdbkg.h() - SHADOWWIDTH - ANGLEWIDTH, BORDERWIDTH, ANGLEWIDTH);
    dst_pt = Point(posx + posw - src_rt.w, posy + posh - src_rt.h);
    surdbkg.Blit(src_rt, dst_pt, sf);
}

void DrawBorder2(s16 posx, s16 posy, u16 posw, u16 posh, const Surface & bk, Surface & sf)
{
    Rect  src_rt;
    Point dst_pt;

    u16 cw = bk.w() / 3;
    u16 ch = bk.h() / 3;

    u16 bw = bk.w() - (cw * 2);
    u16 bh = bk.h() - (ch * 2);

    if(posw < cw)
    {
	cw = posw / 2 - 4;
	bw = 2;
    }

    if(posh < ch)
    {
	ch = posh / 2 - 4;
	bh = 4;
    }

    const u16 ox = (posw - (posw / bw) * bw) / 2;
    const u16 oy = (posh - (posh / bh) * bh) / 2;

    // body
    if(bw < posw && bh < posh)
    {
	for(u16 yy = 0; yy < (posh / bh); ++yy)
	{
	    for(u16 xx = 0; xx < (posw / bw); ++xx)
	    {
		bk.Blit(Rect(cw, ch, bw, bh), posx + ox + xx * bw, posy + oy + yy * bh, sf);
	    }
	}
    }

    // top, bottom bar
    for(u16 xx = 0; xx < (posw / bw); ++xx)
    {
	bk.Blit(Rect(cw, 0, bw, ch), posx + ox + xx * bw, posy, sf);
	bk.Blit(Rect(cw, bk.h() - ch, bw, ch), posx + ox + xx * bw, posy + posh - ch, sf);
    }

    // left, right bar
    for(u16 yy = 0; yy < (posh / bh); ++yy)
    {
	bk.Blit(Rect(0, ch, bw, ch), posx, posy + oy + yy * bh, sf);
	bk.Blit(Rect(bk.w() - cw, ch, bw, ch), posx + posw - cw, posy + oy + yy * bh, sf);
    }

    // top left angle
    src_rt = Rect(0, 0, cw, ch);
    dst_pt = Point(posx, posy);
    bk.Blit(src_rt, dst_pt, sf);

    // top right angle
    src_rt = Rect(bk.w() - cw, 0, cw,ch);
    dst_pt = Point(posx + posw - src_rt.w, posy);
    bk.Blit(src_rt, dst_pt, sf);

    // bottom left angle
    src_rt = Rect(0, bk.h() - ch, cw, ch);
    dst_pt = Point(posx, posy + posh - src_rt.h);
    bk.Blit(src_rt, dst_pt, sf);

    // bottom right angle
    src_rt = Rect(bk.w() - cw, bk.h() - ch, cw, ch);
    dst_pt = Point(posx + posw - src_rt.w, posy + posh - src_rt.h);
    bk.Blit(src_rt, dst_pt, sf);
}
