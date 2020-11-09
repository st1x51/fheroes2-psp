# makefile
# project: Free Heroes2
#
# Options:
# DEBUG: build in debug mode
# RELEASE: build with addons extensions
#
# WITHOUT_ZLIB: build without zlib (disable comressed save files)
# WITHOUT_MIXER: build without SDL_mixer library
# WITHOUT_AUDIOCD: disable audio cd support
# WITHOUT_NETWORK: build without SDL_net library
# WITHOUT_UNICODE: build without unicode (disable translation and ttf font)
# WITHOUT_IMAGE: build without SDL_image library (disable cache image, icn2png)
# WITHOUT_EDITOR
# WITHOUT_XML: skip build tinyxml, used for load alt. resources
# WITH_TOOLS: build tools
# WITH_AI: empty (default), simple (Non-commercial License)

PSPSDK =$(shell psp-config --pspsdk-path)
TARGET = fheroes2

EXTRA_TARGETS    = EBOOT.PBP
PSP_EBOOT_TITLE  = FHeroes II Ported By Crow_Bar
PSP_EBOOT_ICON   = ICON0.PNG
PSP_EBOOT_PIC1   = PIC0.PNG

PSP_FW_VERSION   = 500
BUILD_PRX        = 1
INCDIR           = xmlccwrap engine fheroes2\agg fheroes2\ai fheroes2\algorithm fheroes2\army fheroes2\battle fheroes2\castle fheroes2\dialog \
fheroes2\editor fheroes2\game fheroes2\gui fheroes2\heroes fheroes2\kingdom fheroes2\image fheroes2\maps fheroes2\monster fheroes2\network fheroes2\objects \
fheroes2\pocketpc fheroes2\resource fheroes2\spell fheroes2\system fheroes2\test

PSP_LARGE_MEMORY = 1
#fheroes2\ai\simple\ai_battle.o \
#fheroes2\ai\simple\ai_castle.o \
#fheroes2\ai\simple\ai_heroes.o \
#fheroes2\ai\simple\ai_kingdom.o \
#fheroes2\ai\simple\ai_simple.o \

OBJS = \
engine\audio.o \
engine\audio_cdrom.o \
engine\audio_mixer.o \
engine\audio_music.o \
engine\dir.o \
engine\display.o \
engine\engine.o \
engine\error.o \
engine\font.o \
engine\IMG_savepng.o \
engine\localevent.o \
engine\rand.o \
engine\rect.o \
engine\sdlnet.o \
engine\serialize.o \
engine\sprites.o \
engine\surface.o \
engine\system.o \
engine\thread.o \
engine\tinyconfig.o \
engine\tools.o \
engine\translations.o \
engine\xmi2mid.o \
engine\zzlib.o \
fheroes2\agg\agg.o \
fheroes2\agg\icn.o \
fheroes2\agg\m82.o \
fheroes2\agg\mus.o \
fheroes2\agg\sprite.o \
fheroes2\agg\til.o \
fheroes2\agg\xmi.o \
fheroes2\ai\empty\ai_empty.o \
fheroes2\ai\ai_action.o \
fheroes2\army\army.o \
fheroes2\army\army_bar.o \
fheroes2\army\army_troop.o \
fheroes2\battle\battle_action.o \
fheroes2\battle\battle_arena.o \
fheroes2\battle\battle_army.o \
fheroes2\battle\battle_board.o \
fheroes2\battle\battle_bridge.o \
fheroes2\battle\battle_catapult.o \
fheroes2\battle\battle_cell.o \
fheroes2\battle\battle_command.o \
fheroes2\battle\battle_dialogs.o \
fheroes2\battle\battle_grave.o \
fheroes2\battle\battle_interface.o \
fheroes2\battle\battle_main.o \
fheroes2\battle\battle_only.o \
fheroes2\battle\battle_tower.o \
fheroes2\battle\battle_troop.o \
fheroes2\castle\buildinginfo.o \
fheroes2\castle\captain.o \
fheroes2\castle\castle.o \
fheroes2\castle\castle_building.o \
fheroes2\castle\castle_dialog.o \
fheroes2\castle\castle_mageguild.o \
fheroes2\castle\castle_tavern.o \
fheroes2\castle\castle_town.o \
fheroes2\castle\castle_well.o \
fheroes2\castle\mageguild.o \
fheroes2\dialog\dialog_adventure.o \
fheroes2\dialog\dialog_arena.o \
fheroes2\dialog\dialog_armyinfo.o \
fheroes2\dialog\dialog_box.o \
fheroes2\dialog\dialog_buyboat.o \
fheroes2\dialog\dialog_chest.o \
fheroes2\dialog\dialog_file.o \
fheroes2\dialog\dialog_frameborder.o \
fheroes2\dialog\dialog_gameinfo.o \
fheroes2\dialog\dialog_giftresources.o \
fheroes2\dialog\dialog_guardian.o \
fheroes2\dialog\dialog_levelup.o \
fheroes2\dialog\dialog_marketplace.o \
fheroes2\dialog\dialog_message.o \
fheroes2\dialog\dialog_quickinfo.o \
fheroes2\dialog\dialog_recrut.o \
fheroes2\dialog\dialog_resourceinfo.o \
fheroes2\dialog\dialog_selectcount.o \
fheroes2\dialog\dialog_selectfile.o \
fheroes2\dialog\dialog_selectitems.o \
fheroes2\dialog\dialog_selectscenario.o \
fheroes2\dialog\dialog_settings.o \
fheroes2\dialog\dialog_skillinfo.o \
fheroes2\dialog\dialog_spellinfo.o \
fheroes2\dialog\dialog_spriteinfo.o \
fheroes2\dialog\dialog_system.o \
fheroes2\dialog\dialog_thievesguild.o \
fheroes2\game\difficulty.o \
fheroes2\game\fheroes2.o \
fheroes2\game\game.o \
fheroes2\game\game_delays.o \
fheroes2\game\game_highscores.o \
fheroes2\game\game_hotkeys.o \
fheroes2\game\game_interface.o \
fheroes2\game\game_io.o \
fheroes2\game\game_loadgame.o \
fheroes2\game\game_mainmenu.o \
fheroes2\game\game_newgame.o \
fheroes2\game\game_over.o \
fheroes2\game\game_scenarioinfo.o \
fheroes2\game\game_startgame.o \
fheroes2\game\game_static.o \
fheroes2\gui\button.o \
fheroes2\gui\cursor.o \
fheroes2\gui\interface_border.o \
fheroes2\gui\interface_buttons.o \
fheroes2\gui\interface_cpanel.o \
fheroes2\gui\interface_events.o \
fheroes2\gui\interface_focus.o \
fheroes2\gui\interface_gamearea.o \
fheroes2\gui\interface_icons.o \
fheroes2\gui\interface_radar.o \
fheroes2\gui\interface_status.o \
fheroes2\gui\splitter.o \
fheroes2\gui\statusbar.o \
fheroes2\gui\text.o \
fheroes2\heroes\direction.o \
fheroes2\heroes\heroes.o \
fheroes2\heroes\heroes_action.o \
fheroes2\heroes\heroes_base.o \
fheroes2\heroes\heroes_dialog.o \
fheroes2\heroes\heroes_indicator.o \
fheroes2\heroes\heroes_meeting.o \
fheroes2\heroes\heroes_move.o \
fheroes2\heroes\heroes_recruits.o \
fheroes2\heroes\heroes_spell.o \
fheroes2\heroes\route.o \
fheroes2\heroes\route_pathfind.o \
fheroes2\heroes\skill.o \
fheroes2\kingdom\color.o \
fheroes2\kingdom\kingdom.o \
fheroes2\kingdom\kingdom_overview.o \
fheroes2\kingdom\luck.o \
fheroes2\kingdom\morale.o \
fheroes2\kingdom\payment.o \
fheroes2\kingdom\profit.o \
fheroes2\kingdom\puzzle.o \
fheroes2\kingdom\race.o \
fheroes2\kingdom\speed.o \
fheroes2\kingdom\week.o \
fheroes2\kingdom\world.o \
fheroes2\kingdom\world_loadmap.o \
fheroes2\maps\ground.o \
fheroes2\maps\maps.o \
fheroes2\maps\maps_actions.o \
fheroes2\maps\maps_fileinfo.o \
fheroes2\maps\maps_objects.o \
fheroes2\maps\maps_tiles.o \
fheroes2\maps\maps_tiles_quantity.o \
fheroes2\maps\mp2.o \
fheroes2\maps\position.o \
fheroes2\maps\visit.o \
fheroes2\monster\monster.o \
fheroes2\objects\mounts.o \
fheroes2\objects\objcrck.o \
fheroes2\objects\objdirt.o \
fheroes2\objects\objdsrt.o \
fheroes2\objects\objgras.o \
fheroes2\objects\objlava.o \
fheroes2\objects\objmult.o \
fheroes2\objects\objsnow.o \
fheroes2\objects\objswmp.o \
fheroes2\objects\objtown.o \
fheroes2\objects\objwatr.o \
fheroes2\objects\objxloc.o \
fheroes2\objects\trees.o \
fheroes2\pocketpc\pocketpc_armyinfo.o \
fheroes2\pocketpc\pocketpc_castle.o \
fheroes2\pocketpc\pocketpc_dialog.o \
fheroes2\pocketpc\pocketpc_heroes.o \
fheroes2\pocketpc\pocketpc_kingdom.o \
fheroes2\pocketpc\pocketpc_mainmenu.o \
fheroes2\pocketpc\pocketpc_meeting.o \
fheroes2\pocketpc\pocketpc_selectscenario.o \
fheroes2\pocketpc\pocketpc_thievesguild.o \
fheroes2\resource\artifact.o \
fheroes2\resource\artifact_ultimate.o \
fheroes2\resource\maps_text.o \
fheroes2\resource\resource.o \
fheroes2\spell\spell.o \
fheroes2\spell\spell_book.o \
fheroes2\spell\spell_storage.o \
fheroes2\system\gamedefs.o \
fheroes2\system\players.o \
fheroes2\system\settings.o \
fheroes2\test\test.o \
fheroes2\test\test_monstersprite.o \
xmlccwrap\gzstream.o \
xmlccwrap\tinystr.o \
xmlccwrap\tinyxml.o \
xmlccwrap\tinyxmlerror.o \
xmlccwrap\tinyxmlparser.o


PSPBIN = $(PSPSDK)/../bin
LIBS = -lz -lSDL_mixer -lvorbisfile -lvorbis -logg -lm -lSDL -lpspirkeyb -lpsppower -lpspaudio -lpspgu -lGL -lpspvfpu -lpsphprm -lpsprtc -lpsputility -lstdc++

CFLAGS += -IC:/pspsdk/psp/include/SDL -G0 -O2 -Wall -fsigned-char -DWITH_ZLIB -DTIXML_USE_STL -DPSP

CXXFLAGS += $(CFLAGS) -fexceptions -fno-rtti
ASFLAGS += $(CFLAGS) -c
LIBS += -lvorbisfile -lvorbis -logg -lm -lpsppower -lpsprtc

include $(PSPSDK)/lib/build.mak
