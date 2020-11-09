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
#include "agg.h"
#include "cursor.h"
#include "text.h"
#include "world.h"
#include "payment.h"
#include "kingdom.h"
#include "tools.h"
#include "heroes.h"
#include "artifact.h"
#include "editor_dialogs.h"
#include "settings.h"
#include "localclient.h"
#include "selectartifactbar.h"

#define MAXARTIFACTLINE		7

enum
{
    FLAGS_READONLY	= 0x01,
    FLAGS_USEART32	= 0x02,
    FLAGS_CHANGEMODE	= 0x04
};

SelectArtifactsBar::SelectArtifactsBar(Heroes &h) : hero(h), interval(0), selected(-1), flags(0), background(NULL)
{
}

const Rect & SelectArtifactsBar::GetArea(void) const
{
    return pos;
}

bool SelectArtifactsBar::isValid(void) const
{
    return background;
}

bool SelectArtifactsBar::isSelected(void) const
{
    return 0 <= selected;
}

s8 SelectArtifactsBar::Selected(void) const
{
    return selected;
}

s8 SelectArtifactsBar::GetIndexFromCoord(const Point & coord)
{
    if(!background) return -1;

    if(coord.y < pos.y + background->h())
	for(u8 ii = 0; ii < MAXARTIFACTLINE; ++ii)
	{
	    const Rect rt(pos.x + ii * (background->w() + interval), pos.y, background->w(), background->h());
	    if(rt & coord) return ii;
	}
    else
    if(coord.y > pos.y + background->h() + interval)
	for(u8 ii = 0; ii < MAXARTIFACTLINE; ++ii)
	{
	    const Rect rt(pos.x + ii * (background->w() + interval), pos.y + background->h() + interval, background->w(), background->h());
	    if(rt & coord) return ii + MAXARTIFACTLINE;
	}

    return -1;
}

void SelectArtifactsBar::SetPos(const Point & pt)
{
    SetPos(pt.x, pt.y);
}

void SelectArtifactsBar::SetPos(s16 cx, s16 cy)
    
{
    pos.x = cx;
    pos.y = cy;
    spritecursor.Save(cx, cy);
}

void SelectArtifactsBar::SetBackgroundSprite(const Surface & sf)
{
    background = &sf;
    pos.w = MAXARTIFACTLINE * sf.w() + (MAXARTIFACTLINE - 1) * interval;
    pos.h = 2 * background->h() + interval;

    offset.x = (background->w() - spritecursor.w()) / 2;
    offset.y = (background->h() - spritecursor.h()) / 2;
}

void SelectArtifactsBar::SetCursorSprite(const Surface & sf)
{
    spritecursor.SetSprite(sf);

    if(background)
    {
	offset.x = (background->w() - sf.w()) / 2;
	offset.y = (background->h() - sf.h()) / 2;
    }
}

void SelectArtifactsBar::SetInterval(const u8 it)
{
    interval = it;
    pos.w = (MAXARTIFACTLINE - 1) * interval;
    if(background)
    {
	pos.w += MAXARTIFACTLINE * background->w();
	pos.h += interval;
    }
}

void SelectArtifactsBar::SetChangeMode(void)
{
    flags |= FLAGS_CHANGEMODE;
}

void SelectArtifactsBar::SetReadOnly(void)
{
    flags |= FLAGS_READONLY;
}

void SelectArtifactsBar::SetUseArts32Sprite(void)
{
    flags |= FLAGS_USEART32;
}

bool SelectArtifactsBar::ChangeMode(void) const
{
    return flags & FLAGS_CHANGEMODE;
}

bool SelectArtifactsBar::ReadOnly(void) const
{
    return flags & FLAGS_READONLY;
}

void SelectArtifactsBar::Redraw(Surface & dst)
{
    if(!background) return;

    spritecursor.Hide();
    Point pt(pos);
    BagArtifacts & arts = hero.GetBagArtifacts();

    for(u8 ii = 0; ii < MAXARTIFACTLINE; ++ii)
    {
	const Artifact & art = arts[ii];

	background->Blit(pt, dst);
	if(art.isValid())
	{
	    if(flags & FLAGS_USEART32)
    		AGG::GetICN(ICN::ARTFX, art.IndexSprite32()).Blit(pt.x + 1, pt.y + 1, dst);
	    else
    		AGG::GetICN(ICN::ARTIFACT, art.IndexSprite64()).Blit(pt, dst);
	}
	pt.x += background->w() + interval;
    }

    pt.x = pos.x;
    pt.y += background->h() + interval;

    for(u8 ii = 0; ii < MAXARTIFACTLINE; ++ii)
    {
	const Artifact & art = arts[ii + MAXARTIFACTLINE];

	background->Blit(pt, dst);
	if(art.isValid())
	{
	    if(flags & FLAGS_USEART32)
    		AGG::GetICN(ICN::ARTFX, art.IndexSprite32()).Blit(pt.x + 1, pt.y + 1, dst);
	    else
    		AGG::GetICN(ICN::ARTIFACT, art.IndexSprite64()).Blit(pt, dst);
	}

	pt.x += background->w() + interval;
    }

    if(0 <= selected)
    {
	MAXARTIFACTLINE > selected ? 
	spritecursor.Show(pos.x + selected * (background->w() + interval) + offset.x, flags & FLAGS_USEART32 ? pos.y : pos.y + offset.y) :
	spritecursor.Show(pos.x + (selected - MAXARTIFACTLINE) * (background->w() + interval) + offset.x, flags & FLAGS_USEART32 ? pos.y + background->h() + interval : pos.y + background->h() + interval + offset.y);
    }
}

void SelectArtifactsBar::Reset(void)
{
    selected = -1;
    if(Cursor::Get().isVisible()) Cursor::Get().Hide();
    spritecursor.Hide();
}

void SelectArtifactsBar::Select(u8 index)
{
    if(background && index < MAXARTIFACTLINE * 2)
    {
	selected = index;
	spritecursor.Hide();
	MAXARTIFACTLINE > index ? 
	spritecursor.Show(pos.x + selected * (background->w() + interval) + offset.x, flags & FLAGS_USEART32 ? pos.y : pos.y + offset.y) :
	spritecursor.Show(pos.x + (selected - MAXARTIFACTLINE) * (background->w() + interval) + offset.x, flags & FLAGS_USEART32 ? pos.y + background->h() + interval : pos.y + background->h() + interval + offset.y);
    }
}

bool SelectArtifactsBar::QueueEventProcessing(SelectArtifactsBar & bar)
{
    LocalEvent & le = LocalEvent::Get();

    if(! bar.isValid()) return false;

    const s8 index1 = bar.GetIndexFromCoord(le.GetMouseCursor());
    if(0 > index1 || HEROESMAXARTIFACT <= index1) return false;
    BagArtifacts & arts = bar.hero.GetBagArtifacts();

    Artifact & art1 = arts[index1];
    bool change = false;
    Cursor::Get().Hide();

    // left click
    if(le.MouseClickLeft(bar.GetArea()))
    {
	if(bar.isSelected())
	{
	    const s8 index2 = bar.Selected();
	    Artifact & art2 = arts[index2];

	    // dialog
	    if(index1 == index2)
	    {
		if(art1() == Artifact::MAGIC_BOOK)
		{
		    if(bar.ChangeMode())
			bar.hero.EditSpellBook();
		    else
			bar.hero.OpenSpellBook(SpellBook::ALL, false);
		}
		else
		if(art1() == Artifact::SPELL_SCROLL &&
		    Settings::Get().ExtHeroAllowTranscribingScroll() &&
		    bar.hero.CanTranscribeScroll(art1))
		{
		    Spell spell = art1.GetSpell();

		    if(! spell.isValid())
		    {
			DEBUG(DBG_GAME, DBG_WARN, "invalid spell");
		    }
		    else
		    if(bar.hero.CanLearnSpell(spell))
		    {
			payment_t cost = spell.GetCost();
			u16 answer = 0;
			std::string msg = _("Do you want to use your knowledge of magical secrets to transcribe the %{spell} Scroll into your spell book?\nThe Scroll will be consumed.\n Spell point: %{sp}");

			String::Replace(msg, "%{spell}", spell.GetName());
			String::Replace(msg, "%{sp}", spell.SpellPoint());
			if(spell.MovePoint())
			{
			    msg.append("\n");
			    msg.append("Move point: %{mp}");
			    String::Replace(msg, "%{mp}", spell.MovePoint());
			}

			if(cost.GetValidItems())
			    answer = Dialog::ResourceInfo("", msg, cost, Dialog::YES|Dialog::NO);
			else
			    answer = Dialog::Message("", msg, Font::BIG, Dialog::YES|Dialog::NO);

			if(answer == Dialog::YES)
			    bar.hero.TranscribeScroll(art1);

			change = true;
		    }
		}
		else
		    Dialog::ArtifactInfo(art1.GetName(), art1.GetDescription(), art1);
	    }
	    // exchange
	    else
	    {
	    	std::swap(art1, art2);
		change = true;
#ifdef WITH_NET
	    	FH2LocalClient::SendHeroesSwapArtifacts(bar.hero, index1, bar.hero, index2);
#endif
	    }

	    bar.Reset();
	    bar.Redraw();
	}
	else
	// select
	if(!bar.ReadOnly() && art1.isValid())
	{
	    bar.Select(index1);
	    change = true;
	}
	else
	if(bar.ChangeMode() && !art1.isValid())
	{
	    art1 = Dialog::SelectArtifact();
	    if(art1.isValid())
		change = true;
	}
    }
    else
    // press right
    if(le.MousePressRight(bar.GetArea()))
    {
        bar.Reset();
	// show quick info
	if(art1.isValid())
	{
	    if(bar.ChangeMode())
    	    {
                art1.Reset();                                                                                                
                change = true;                                                                                                 
    	    }
	    else
	    Dialog::ArtifactInfo(art1.GetName(), art1.GetDescription(), art1, 0);
	}
    }

    Cursor::Get().Show();
    Display::Get().Flip();

    return change;
}

bool SelectArtifactsBar::QueueEventProcessing(SelectArtifactsBar & bar1, SelectArtifactsBar & bar2)
{
    LocalEvent & le = LocalEvent::Get();
    bool change = false;

    if(!bar1.isValid() || !bar2.isValid()) return false;

    if((bar1.isSelected() || (!bar1.isSelected() && !bar2.isSelected())) && le.MouseCursor(bar1.GetArea()))
	    return QueueEventProcessing(bar1);
    else
    if((bar2.isSelected() || (!bar1.isSelected() && !bar2.isSelected())) && le.MouseCursor(bar2.GetArea()))
	    return QueueEventProcessing(bar2);
    else
    if(bar1.isSelected() && le.MouseCursor(bar2.GetArea()))
    {
	const s8 index1 = bar2.GetIndexFromCoord(le.GetMouseCursor());
	if(0 > index1 || HEROESMAXARTIFACT <= index1) return false;
	const s8 index2 = bar1.Selected();

	BagArtifacts & arts1 = bar1.hero.GetBagArtifacts();
	BagArtifacts & arts2 = bar2.hero.GetBagArtifacts();

	Artifact & art1 = arts1[index2];
	Artifact & art2 = arts2[index1];

	Cursor::Get().Hide();

	// left click
	if(le.MouseClickLeft(bar2.GetArea()))
	{
	    if(art1() != Artifact::MAGIC_BOOK && art2() != Artifact::MAGIC_BOOK)
	    {
		std::swap(art1, art2);
		change = true;

#ifdef WITH_NET
	    	FH2LocalClient::SendHeroesSwapArtifacts(bar1.hero, index2, bar2.hero, index1);
#endif

		bar1.Reset();
		bar2.Reset();

		bar1.Redraw();
		bar2.Redraw();
	    }
	}
	else
	// press right
	if(le.MousePressRight(bar2.GetArea()))
	{
	    bar1.Reset();
	    Dialog::ArtifactInfo(art2.GetName(), art2.GetDescription(), art2, 0);
	}

	Cursor::Get().Show();
	Display::Get().Flip();
    }
    else
    if(bar2.isSelected() && le.MouseCursor(bar1.GetArea()))
    {
	const s8 index1 = bar1.GetIndexFromCoord(le.GetMouseCursor());
	if(0 > index1 || HEROESMAXARTIFACT <= index1) return false;
	const s8 index2 = bar2.Selected();

	BagArtifacts & arts1 = bar1.hero.GetBagArtifacts();
	BagArtifacts & arts2 = bar2.hero.GetBagArtifacts();

	Artifact & art1 = arts1[index1];
	Artifact & art2 = arts2[index2];

	Cursor::Get().Hide();

	// left click
	if(le.MouseClickLeft(bar1.GetArea()))
	{
	    if(art1() != Artifact::MAGIC_BOOK && art2() != Artifact::MAGIC_BOOK)
	    {
		std::swap(art1, art2);
		change = true;

#ifdef WITH_NET
	    	FH2LocalClient::SendHeroesSwapArtifacts(bar1.hero, index1, bar2.hero, index2);
#endif

		bar1.Reset();
		bar2.Reset();

		bar1.Redraw();
		bar2.Redraw();
	    }
	}
	else
	// press right
	if(le.MousePressRight(bar1.GetArea()))
	{
	    bar2.Reset();
	    Dialog::ArtifactInfo(art1.GetName(), art1.GetDescription(), art1, 0);
	}

	Cursor::Get().Show();
	Display::Get().Flip();
    }

    return change;
}
