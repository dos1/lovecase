/*! \file walk.c
 *  \brief Walking gamestate.
 */
/*
 * Copyright (c) Sebastian Krzyszkowiak <dos@dosowisko.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "../common.h"
#include <libsuperderpy.h>
#include <math.h>
#include <allegro5/allegro_primitives.h>

struct MenuResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		ALLEGRO_FONT *font;
		int option, blink;
		ALLEGRO_BITMAP *bg;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct MenuResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink++;
	if (data->blink >= 60) {
		data->blink = 0;
	}
}

void Gamestate_Draw(struct Game *game, struct MenuResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	al_set_target_backbuffer(game->display);
	al_draw_bitmap(data->bg, 0, 0, 0);
	al_draw_filled_rectangle(0, 158, 320, 180, al_map_rgba(0,0,0,64));

	const char* texts[] = { "< Start game >", "< Options >", "< dosowisko.net >", "< Quit >",
	                        "< Fullscreen: on >", "< Music: on >", "< Sounds: on >", "< Voice: on >", "< Back >",
	                        "< Fullscreen: off >", "< Music: off >", "< Sounds: off >", "< Voice: off >", "< Back >"
	                      };

	if (data->blink < 45) {
		DrawTextWithShadow(data->font, al_map_rgb(255,255,255), 320/2, 165, ALLEGRO_ALIGN_CENTER, texts[data->option]);
	}
}

void Gamestate_ProcessEvent(struct Game *game, struct MenuResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		if (data->option >= 4) {
			//al_stop_sample_instance(game->data->button);
			//al_play_sample_instance(game->data->button);
			data->blink = 0;
			data->option = 0;
		} else {
			UnloadAllGamestates(game);
		}
	}
	if (ev->type==ALLEGRO_EVENT_KEY_DOWN) {
		if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
			//al_stop_sample_instance(game->data->button);
			//al_play_sample_instance(game->data->button);
			data->blink = 0;
			data->option--;
			if (data->option==8) {
				data->option = 13;
			}
			if (data->option==3) {
				data->option = 8;
			}
			if (data->option==-1) {
				data->option = 3;
			}
		}
		if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			//al_stop_sample_instance(game->data->button);
			//al_play_sample_instance(game->data->button);
			data->blink = 0;
			data->option++;
			if (data->option==4) {
				data->option = 0;
			}
			if (data->option==9) {
				data->option=4;
			}
			if (data->option==14) {
				data->option=9;
			}
		}

		if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER) {
			//al_stop_sample_instance(game->data->button);
			//al_play_sample_instance(game->data->button);
			data->blink = 0;
			switch (data->option) {
				case 0:
					UnloadAllGamestates(game);
					LoadGamestate(game, "dispatcher");
					StartGamestate(game, "dispatcher");
					//StartGame(game, !game->data->logo);
					break;
				case 1:
					data->option = 4;
					break;
				case 2:
#ifdef ALLEGRO_WINDOWS
					system("start \"\" \"https://dosowisko.net\"");
#elif defined(ALLEGRO_MACOSX)
					system("open \"https://dosowisko.net\"");
#else
					system("xdg-open \"https://dosowisko.net\"");
#endif
					UnloadAllGamestates(game);
					break;
				case 3:
					UnloadAllGamestates(game);
					break;
				case 4:
				case 9:
					// fullscreen
					game->config.fullscreen = !game->config.fullscreen;
					if (game->config.fullscreen) {
						SetConfigOption(game, "SuperDerpy", "fullscreen", "1");
						al_hide_mouse_cursor(game->display);
					} else {
						SetConfigOption(game, "SuperDerpy", "fullscreen", "0");
						al_show_mouse_cursor(game->display);
					}
					al_set_display_flag(game->display, ALLEGRO_FULLSCREEN_WINDOW, game->config.fullscreen);
					SetupViewport(game, game->viewport_config);
					PrintConsole(game, "Fullscreen toggled");
					break;
				case 5:
				case 10:
					// music
					game->config.music = game->config.music ? 0 : 10;
					SetConfigOption(game, "SuperDerpy", "music", game->config.music ? "10" : "0");
					al_set_mixer_gain(game->audio.music, game->config.music/10.0);
					break;
				case 6:
				case 11:
					// sounds
					game->config.fx = game->config.fx ? 0 : 10;
					SetConfigOption(game, "SuperDerpy", "fx", game->config.fx ? "10" : "0");
					al_set_mixer_gain(game->audio.fx, game->config.fx/10.0);
					break;
				case 7:
				case 12:
					// voices
					game->config.voice = game->config.voice ? 0 : 10;
					SetConfigOption(game, "SuperDerpy", "voice", game->config.voice ? "10" : "0");
					al_set_mixer_gain(game->audio.voice, game->config.voice/10.0);
					break;
				case 8:
				case 13:
					data->option = 0;
					break;
			}
		}
	}

	switch (data->option) {
		case 4:
			if (!game->config.fullscreen) {
				data->option += 5;
			}
			break;
		case 5:
			if (!game->config.music) {
				data->option += 5;
			}
			break;
		case 6:
			if (!game->config.fx) {
				data->option += 5;
			}
			break;
		case 7:
			if (!game->config.voice) {
				data->option += 5;
			}
			break;
		case 9:
			if (game->config.fullscreen) {
				data->option -= 5;
			}
			break;
		case 10:
			if (game->config.music) {
				data->option -= 5;
			}
			break;
		case 11:
			if (game->config.fx) {
				data->option -= 5;
			}
			break;
		case 12:
			if (game->config.voice) {
				data->option -= 5;
			}
			break;
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct MenuResources *data = malloc(sizeof(struct MenuResources));
	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bg = al_load_bitmap(GetDataFilePath(game, "menu.png"));

	return data;
}

void Gamestate_Unload(struct Game *game, struct MenuResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	free(data);
}

void Gamestate_Start(struct Game *game, struct MenuResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->option = 0;
	data->blink = 0;
}

void Gamestate_Stop(struct Game *game, struct MenuResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

// Ignore those for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game *game, struct MenuResources* data) {}
void Gamestate_Pause(struct Game *game, struct MenuResources* data) {}
void Gamestate_Resume(struct Game *game, struct MenuResources* data) {}
