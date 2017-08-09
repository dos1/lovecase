/*! \file common.c
 *  \brief Common stuff that can be used by all gamestates.
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

#include "common.h"
#include <stdio.h>
#include <libsuperderpy.h>

bool GlobalEventHandler(struct Game *game, ALLEGRO_EVENT *ev) {
	if (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		game->data->touch = true;
	}
#ifndef ALLEGRO_ANDROID
	if (ev->type == ALLEGRO_EVENT_KEY_DOWN) {
		game->data->touch = false;
	}
#endif
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_F)) {
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
	}

	return false;
}

struct CommonResources* CreateGameData(struct Game *game) {
	struct CommonResources *data = calloc(1, sizeof(struct CommonResources));
	data->script = "000-intro";
	data->evidence_len = 0;
	data->evidence[data->evidence_len] = NULL;

	data->sample = al_load_sample(GetDataFilePath(game, "music.flac"));
	data->music = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->music, game->audio.music);
	al_set_sample_instance_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);

	data->notebook_enabled = game->config.debug;

	data->touch = false;
#ifdef ALLEGRO_ANDROID
	data->touch = true;
#endif

	return data;
}

void DestroyGameData(struct Game *game, struct CommonResources *data) {
	for (int i=0; i<data->evidence_len; i++) {
		free(data->evidence[i]);
	}
	al_stop_sample_instance(game->data->music);
	free(data);
}
