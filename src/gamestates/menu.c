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
#include <allegro5/allegro_primitives.h>
#include <libsuperderpy.h>
#include <math.h>
#include <stdio.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	ALLEGRO_FONT* font;
	int option, blink;
	ALLEGRO_BITMAP* bg;

	ALLEGRO_SAMPLE* sample;
	ALLEGRO_SAMPLE_INSTANCE* music;

	ALLEGRO_SAMPLE* sample_click;
	ALLEGRO_SAMPLE_INSTANCE* click;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {}

void Gamestate_Tick(struct Game* game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.
	data->blink++;
	if (data->blink >= 60) {
		data->blink = 0;
	}
}

void OpenBrowser(char* url) {
	char* cmd;
#ifdef ALLEGRO_WINDOWS
	cmd = "start \"\" \"%s\"";
#elif defined(ALLEGRO_MACOSX)
	cmd = "open \"%s\"";
#elif defined(ALLEGRO_ANDROID)
	cmd = "am start --user 0 -a android.intent.action.VIEW -d \"%s\"";
#else
	cmd = "xdg-open \"%s\"";
#endif
	char command[255];
	snprintf(command, 255, cmd, url);
	system(command);
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.

	al_draw_bitmap(data->bg, 0, 0, 0);
	al_draw_filled_rectangle(0, 158, 320, 180, al_map_rgba(0, 0, 0, 64));

	const char* texts[] = {"Start game", "Options", "dosowisko.net", "Quit",
		"Fullscreen: on", "Music: on", "Sounds: on", "Voice: on", "Back",
		"Fullscreen: off", "Music: off", "Sounds: off", "Voice: off", "Back"};

	if (data->blink < 45) {
		if (game->data->touch) {
			DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), 320 / 2, 165, ALLEGRO_ALIGN_CENTER, texts[data->option]);
			DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), 10, 165, ALLEGRO_ALIGN_LEFT, "<");
			DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), 310, 165, ALLEGRO_ALIGN_RIGHT, ">");
		} else {
			char text[255];
			snprintf(text, 255, "< %s >", texts[data->option]);
			DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), 320 / 2, 165, ALLEGRO_ALIGN_CENTER, text);
		}
	}
}

void MenuLeft(struct Game* game, struct GamestateResources* data) {
	al_stop_sample_instance(data->click);
	al_play_sample_instance(data->click);
	data->blink = 0;
	data->option--;
	if (data->option == 8) {
		data->option = 13;
	}
	if (data->option == 3) {
		data->option = 8;
	}
	if (data->option == -1) {
		data->option = 3;
	}

#ifdef ALLEGRO_ANDROID
	if (data->option == 9) {
		data->option = 13;
	}
	if (data->option == 4) {
		data->option = 8;
	}
#endif
}

void MenuRight(struct Game* game, struct GamestateResources* data) {
	al_stop_sample_instance(data->click);
	al_play_sample_instance(data->click);
	data->blink = 0;
	data->option++;

	if (data->option == 4) {
		data->option = 0;
	}
	if (data->option == 9) {
		data->option = 4;
#ifdef ALLEGRO_ANDROID
		data->option++;
#endif
	}
	if (data->option == 14) {
		data->option = 9;
#ifdef ALLEGRO_ANDROID
		data->option++;
#endif
	}
}

void MenuEscape(struct Game* game, struct GamestateResources* data) {
	if (data->option >= 4) {
		al_stop_sample_instance(data->click);
		al_play_sample_instance(data->click);
		data->blink = 0;
		data->option = 0;
	} else {
		UnloadAllGamestates(game);
	}
}

void MenuSelect(struct Game* game, struct GamestateResources* data) {
	al_stop_sample_instance(data->click);
	al_play_sample_instance(data->click);
	data->blink = 0;
	switch (data->option) {
		case 0:
			UnloadAllGamestates(game);
			DestroyGameData(game);
			game->data = CreateGameData(game);
			LoadGamestate(game, "dispatcher");
			StartGamestate(game, "dispatcher");
			break;
		case 1:
			data->option = 4;
			break;
		case 2:
			OpenBrowser("https://dosowisko.net/");
			UnloadAllGamestates(game);
			break;
		case 3:
			UnloadAllGamestates(game);
			break;
		case 4:
		case 9:
			// fullscreen
			ToggleFullscreen(game);
			break;
		case 5:
		case 10:
			// music
			game->config.music = game->config.music ? 0 : 10;
			SetConfigOption(game, "SuperDerpy", "music", game->config.music ? "10" : "0");
			al_set_mixer_gain(game->audio.music, game->config.music / 10.0);
			break;
		case 6:
		case 11:
			// sounds
			game->config.fx = game->config.fx ? 0 : 10;
			SetConfigOption(game, "SuperDerpy", "fx", game->config.fx ? "10" : "0");
			al_set_mixer_gain(game->audio.fx, game->config.fx / 10.0);
			break;
		case 7:
		case 12:
			// voices
			game->config.voice = game->config.voice ? 0 : 10;
			SetConfigOption(game, "SuperDerpy", "voice", game->config.voice ? "10" : "0");
			al_set_mixer_gain(game->audio.voice, game->config.voice / 10.0);
			break;
		case 8:
		case 13:
			data->option = 0;
			break;
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		MenuEscape(game, data);
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_BACK)) {
		MenuEscape(game, data);
	}

	if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
		if (ev->keyboard.keycode == ALLEGRO_KEY_LEFT) {
			MenuLeft(game, data);
		}
		if (ev->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
			MenuRight(game, data);
		}

		if (ev->keyboard.keycode == ALLEGRO_KEY_ENTER) {
			MenuSelect(game, data);
		}
	}

	if (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		if (ev->touch.primary) {
			int x = ev->touch.x;
			int y = ev->touch.y;
			WindowCoordsToViewport(game, &x, &y);

			if ((y >= 158) && (y <= 180)) {
				if ((x >= 0) && (x < 40)) {
					MenuLeft(game, data);
				} else if ((x > 280) && (x <= 320)) {
					MenuRight(game, data);
				} else {
					MenuSelect(game, data);
				}
			}
		}
	}

	switch (data->option) {
		case 4:
			if (!game->config.fullscreen) {
				data->option += 5;
			}
#ifdef ALLEGRO_ANDROID
			data->option++;
#endif
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
#ifdef ALLEGRO_ANDROID
			data->option++;
#endif
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

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources* data = malloc(sizeof(struct GamestateResources));
	al_set_new_bitmap_flags(al_get_new_bitmap_flags() & ~ALLEGRO_MAG_LINEAR);
	data->font = al_create_builtin_font();
	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar

	data->bg = al_load_bitmap(GetDataFilePath(game, "menu.png"));

	data->sample = al_load_sample(GetDataFilePath(game, "menu.flac"));
	data->music = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->music, game->audio.music);
	al_set_sample_instance_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

	data->sample_click = al_load_sample(GetDataFilePath(game, "click.flac"));
	data->click = al_create_sample_instance(data->sample_click);
	al_attach_sample_instance_to_mixer(data->click, game->audio.fx);

	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_destroy_sample_instance(data->music);
	al_destroy_sample(data->sample);
	al_destroy_sample_instance(data->click);
	al_destroy_sample(data->sample_click);
	al_destroy_bitmap(data->bg);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->option = 0;
	data->blink = 0;
	if (game->data->music) {
		al_stop_sample_instance(game->data->music);
	}
	al_play_sample_instance(data->music);
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
	al_stop_sample_instance(data->music);
	if (game->data->music) {
		al_play_sample_instance(game->data->music);
	}
}

void Gamestate_Pause(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
void Gamestate_Reload(struct Game* game, struct GamestateResources* data) {}
