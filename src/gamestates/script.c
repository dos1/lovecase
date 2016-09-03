/*! \file empty.c
 *  \brief Empty gamestate.
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

struct GamestateResources {
		// This struct is for every resource allocated and used by your gamestate.
		// It gets created on load and then gets passed around to all other function calls.
		char *cur_actor;
		char *cur_emoti;
		char *cur_scene;

		char *skip_to;

		struct Character *scene;
		ALLEGRO_BITMAP *actor;

		ALLEGRO_FILE *script_file;

		char *script;
		int speech_counter;
		struct Dialog {
			char *text;
			char *script[255];
			int script_length;
		} dialogs[255];
		int dialog_count;

		char *speech;
		char *status;

		ALLEGRO_FONT *font;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game *game, struct GamestateResources* data) {
	// Called 60 times per second. Here you should do all your game logic.

}

void Gamestate_Draw(struct Game *game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	//	al_draw_text(data->font, al_map_rgb(255,255,255), game->viewport.width / 2, game->viewport.height / 2,
	    //           ALLEGRO_ALIGN_CENTRE, "Nothing to see here, move along!");

	if (data->status) {
		al_draw_text(data->font, al_map_rgb(255,255,255), game->viewport.width / 2, 5,
		    ALLEGRO_ALIGN_CENTRE, data->status);
	}
	if (data->speech) {
		al_draw_text(data->font, al_map_rgb(255,255,255), 5, 165,
		    ALLEGRO_ALIGN_LEFT, data->speech);
	}
	if (data->dialog_count) {
		for (int i=0; i<data->dialog_count; i++) {
			al_draw_text(data->font, al_map_rgb(255,255,255), 5, 175 - (data->dialog_count-i)*10,
			    ALLEGRO_ALIGN_LEFT, data->dialogs[i].text);
		}
	}
}

void Gamestate_ProcessEvent(struct Game *game, struct GamestateResources* data, ALLEGRO_EVENT *ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.
	if ((ev->type==ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
		// When there are no active gamestates, the engine will quit.
	}
}

void* Gamestate_Load(struct Game *game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources *data = calloc(1, sizeof(struct GamestateResources)); // everything is NULL at this point

	data->font = al_create_builtin_font();

	data->script_file = al_fopen(game->data->script, "r");

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game *game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_fclose(data->script_file);
	free(data);
}

void Gamestate_Start(struct Game *game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	/*data->speech = "Spicz";
	data->status = "Ewidens";
	data->dialog_count = 4;
	data->dialogs[0] = (struct Dialog){.text = "lala1"};
	data->dialogs[1] = (struct Dialog){.text = "lala2"};
	data->dialogs[2] = (struct Dialog){.text = "lala3"};
	data->dialogs[3] = (struct Dialog){.text = "lala4"};*/
}

void Gamestate_Stop(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
}

void Gamestate_Pause(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets paused (so only Draw is being called, no Logic not ProcessEvent)
	// Pause your timers here.
}

void Gamestate_Resume(struct Game *game, struct GamestateResources* data) {
	// Called when gamestate gets resumed. Resume your timers here.
}

// Ignore this for now.
// TODO: Check, comment, refine and/or remove:
void Gamestate_Reload(struct Game *game, struct GamestateResources* data) {}
