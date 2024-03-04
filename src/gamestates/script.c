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
#include <stdio.h>

struct GamestateResources {
	// This struct is for every resource allocated and used by your gamestate.
	// It gets created on load and then gets passed around to all other function calls.
	char* cur_actor;
	char* cur_emoti;

	char* skip_to;

	bool stopped;

	bool dialog_enabled;
	int dialog_highlight;
	bool speech_jack;

	char* tut_text;

	int selected;

	struct Character* scene;
	ALLEGRO_BITMAP* actor;
	ALLEGRO_BITMAP* button;

	ALLEGRO_FILE* script_file;

	char* script;
	int speech_counter;
	struct Dialog {
		char* text;
		char* script[255];
		int script_length;
		struct Timeline* timeline;
	} dialogs[255];
	int dialog_count;

	int delay;

	char* speech;
	char* status;

	int fade;

	ALLEGRO_FONT* font;

	struct Timeline* timeline;

	ALLEGRO_BITMAP* icon;
	ALLEGRO_BITMAP *notebook, *notebook_hand;

	bool notebook_on;

	int mousex, mousey;
	int mouse_visible;

	ALLEGRO_BITMAP* cursor;

	int prev_selected;

	ALLEGRO_SAMPLE* sample_ev;
	ALLEGRO_SAMPLE_INSTANCE* evidence;
};

int Gamestate_ProgressCount = 1; // number of loading steps as reported by Gamestate_Load

void Gamestate_Logic(struct Game* game, struct GamestateResources* data, double delta) {
	// Called 60 times per second. Here you should do all your game logic.
	if (!data->notebook_on) {
		TM_Process(data->timeline, delta);
	}
	/*for (int i=0; i<data->dialog_count; i++) {
		TM_Process(data->dialogs[i].timeline);
	}*/
	if (data->scene) {
		AnimateCharacter(game, data->scene, delta, 1);
	}
	if (data->mouse_visible) {
		data->mouse_visible--;
	}
}

void Gamestate_Draw(struct Game* game, struct GamestateResources* data) {
	// Called as soon as possible, but no sooner than next Gamestate_Logic call.
	// Draw everything to the screen here.
	//	al_draw_text(data->font, al_map_rgb(255,255,255), game->viewport.width / 2, game->viewport.height / 2,
	//           ALLEGRO_ALIGN_CENTRE, "Nothing to see here, move along!");

	if (data->scene) {
		DrawCharacter(game, data->scene);
	}
	if (data->actor) {
		al_draw_bitmap(data->actor, 0, 0, 0);
	}

	if (data->notebook_on) {
		al_draw_filled_rectangle(0, 0, 320, 180, al_map_rgba(0, 0, 0, 192));
		al_draw_bitmap(data->notebook, 0, 0, 0);
		al_draw_filled_rectangle(0, 0, 320, 20 + (game->data->evidence_len) * 10, al_map_rgba(0, 0, 0, 128));
		for (int i = 0; i < game->data->evidence_len; i++) {
			DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), 5, (game->data->evidence_len - i) * 10,
				ALLEGRO_ALIGN_LEFT, game->data->evidence[game->data->evidence_len - i - 1]);
		}
		al_draw_bitmap(data->notebook_hand, 0, 0, 0);
		al_draw_filled_rectangle(0, 160, 320, 180, al_map_rgba(0, 0, 0, 128));
		DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), game->viewport.width / 2, 167,
			ALLEGRO_ALIGN_CENTRE, game->data->touch ? "Touch to go back" : "SPACE to go back");
		return;
	}

	if (data->tut_text) {
		al_draw_bitmap(data->notebook, 0, 0, 0);
		al_draw_filled_rectangle(0, 0, 320, 180, al_map_rgba(0, 0, 0, 192));
		DrawWrappedTextWithShadow(data->font, al_map_rgb(255, 255, 255), 5, game->viewport.height / 2,
			310, ALLEGRO_ALIGN_CENTRE, data->tut_text);
	}

	if (data->status) {
		al_draw_bitmap(data->notebook, 0, 0, 0);
		al_draw_line(0, 160, 320, 160, al_map_rgb(255, 255, 255), 3);
		al_draw_filled_rectangle(0, 160, 320, 180, al_map_rgba(0, 0, 0, 192));

		DrawTextWithShadow(data->font, al_map_rgb(255, 255, 255), game->viewport.width / 2, 167,
			ALLEGRO_ALIGN_CENTRE, data->status);
	}
	if (data->speech) {
		al_draw_line(0, 140, 320, 140, data->speech_jack ? al_map_rgb(255, 255, 255) : al_map_rgb(255, 222, 255), 3);
		al_draw_filled_rectangle(0, 140, 320, 180, al_map_rgba(0, 0, 0, 128));
		DrawWrappedText(data->font, data->speech_jack ? al_map_rgb(255, 255, 255) : al_map_rgb(255, 222, 255), 5, 145, 310,
			ALLEGRO_ALIGN_LEFT, data->speech);
		if (data->speech_jack) {
			al_draw_bitmap(data->icon, 2, 140 - 32, 0);
		}
	}
	if (data->dialog_enabled) {
		int height = game->data->touch ? 16 : 10;
		al_draw_line(0, 170 - ((data->dialog_count + 1) * height), 320, 170 - ((data->dialog_count + 1) * height), al_map_rgb(255, 255, 255), 3);
		al_draw_filled_rectangle(0, 170 - ((data->dialog_count + 1) * height), 320, 180, al_map_rgba(0, 0, 0, 192));
		for (int i = 0; i <= data->dialog_count; i++) {
			al_draw_text(data->font, (i == data->dialog_highlight) ? al_map_rgb(128, 255, 255) : al_map_rgb(255, 255, 255), 5, 175 - (data->dialog_count + 1 - i) * height + (height - 10) / 2,
				ALLEGRO_ALIGN_LEFT, data->dialogs[i].text + 6);
		}
	}

	if ((game->data->notebook_enabled) && (game->data->touch)) {
		al_draw_bitmap(data->button, 320 - 31, 2, 0);
	}

	al_draw_filled_rectangle(0, 0, 320, 180, al_map_rgba(0, 0, 0, 255 - data->fade));

	if (data->mouse_visible) {
		al_draw_bitmap(data->cursor, data->mousex - 6, data->mousey - 1, 0);
	}
}

void Gamestate_ProcessEvent(struct Game* game, struct GamestateResources* data, ALLEGRO_EVENT* ev) {
	// Called for each event in Allegro event queue.
	// Here you can handle user input, expiring timers etc.

	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		if (data->notebook_on) {
			data->notebook_on = false;
		} else {
			UnloadCurrentGamestate(game); // mark this gamestate to be stopped and unloaded
			LoadGamestate(game, "menu");
			StartGamestate(game, "menu");
		}
		// When there are no active gamestates, the engine will quit.
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_SPACE)) {
		data->notebook_on = false;
		// When there are no active gamestates, the engine will quit.
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_FULLSTOP)) {
		data->speech_counter = 1;
		data->delay = 1;
		TM_SkipDelay(data->timeline);
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_N)) {
		data->notebook_on = game->data->notebook_enabled;
		data->tut_text = NULL;
	}
	if ((data->tut_text) && (ev->type == ALLEGRO_EVENT_TOUCH_END)) {
		data->notebook_on = game->data->notebook_enabled;
		data->tut_text = NULL;
	}
	if ((data->notebook_on) && (ev->type == ALLEGRO_EVENT_TOUCH_END)) {
		data->notebook_on = false;
		return;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_1)) {
		data->dialog_highlight = 0;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_2)) {
		data->dialog_highlight = 1;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_3)) {
		data->dialog_highlight = 2;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_DOWN) && (ev->keyboard.keycode == ALLEGRO_KEY_4)) {
		data->dialog_highlight = 3;
	}

	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_1)) {
		data->selected = 0;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_2)) {
		data->selected = 1;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_3)) {
		data->selected = 2;
	}
	if ((ev->type == ALLEGRO_EVENT_KEY_UP) && (ev->keyboard.keycode == ALLEGRO_KEY_4)) {
		data->selected = 3;
	}

	if ((ev->type == ALLEGRO_EVENT_MOUSE_AXES) || (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) || (ev->type == ALLEGRO_EVENT_TOUCH_MOVE)) {
		if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
			data->mousex = (ev->mouse.x / (float)al_get_display_width(game->display)) * game->viewport.width;
			data->mousey = (ev->mouse.y / (float)al_get_display_height(game->display)) * game->viewport.height;
			data->mouse_visible = 180;
		} else {
			data->mousex = (ev->touch.x / (float)al_get_display_width(game->display)) * game->viewport.width;
			data->mousey = (ev->touch.y / (float)al_get_display_height(game->display)) * game->viewport.height;
		}

		int height = game->data->touch ? 16 : 10;
		if (data->dialog_enabled) {
			if (data->mousey >= (175 - (data->dialog_count + 1) * height)) {
				int y = data->mousey - (175 - (data->dialog_count + 1) * height);
				data->dialog_highlight = y / height;
			} else {
				data->dialog_highlight = -1;
			}
		}
	}

	if (ev->type == ALLEGRO_EVENT_TOUCH_BEGIN) {
		data->mouse_visible = 0;
	}
	if ((ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) || (ev->type == ALLEGRO_EVENT_TOUCH_END)) {
		if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			data->mousex = (ev->mouse.x / (float)al_get_display_width(game->display)) * game->viewport.width;
			data->mousey = (ev->mouse.y / (float)al_get_display_height(game->display)) * game->viewport.height;
			data->mouse_visible = 180;
		} else {
			data->mousex = (ev->touch.x / (float)al_get_display_width(game->display)) * game->viewport.width;
			data->mousey = (ev->touch.y / (float)al_get_display_height(game->display)) * game->viewport.height;
		}
		if ((data->mousex > 320 - 32) && (data->mousey < 30)) {
			data->notebook_on = game->data->notebook_enabled;
			data->tut_text = NULL;
		} else {
			if (data->dialog_enabled) {
				data->selected = data->dialog_highlight;
			} else {
				data->speech_counter = 1;
				data->delay = 1;
				TM_SkipDelay(data->timeline);
			}
		}
	}
}

static TM_ACTION(Speak) {
	char* text = TM_GetArg(action->arguments, 0);
	bool jack = (bool)TM_GetArg(action->arguments, 1);
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		data->speech = text + 6;
		data->speech_counter = 60 * 4;
		data->speech_jack = jack;
	}
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		data->speech_counter--;
		return !data->speech_counter;
	}
	if (action->state == TM_ACTIONSTATE_DESTROY) {
		data->speech = NULL;
	}
	return true;
}

static TM_ACTION(Delay) {
	char* arg = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		data->delay = strtoul(arg, NULL, 10) * 60;
	}
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		data->delay--;
		return !data->delay;
	}
	return true;
}

static TM_ACTION(Tutorial) {
	char* text = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		data->tut_text = text;
		game->data->notebook_enabled = true;
	}
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		return !data->tut_text;
	}
	return true;
}

static TM_ACTION(NotebookOverview) {
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		data->notebook_on = true;
		char path[255];

		snprintf(path, 255, "SCORE: %d", game->data->score);

		game->data->evidence[game->data->evidence_len] = strdup(path);
		game->data->evidence_len++;
	}
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		return !data->notebook_on;
	}
	return true;
}

static TM_ACTION(ShowEvidence) {
	char* text = TM_GetArg(action->arguments, 0) + 10;
	char* s = TM_GetArg(action->arguments, 0) + 6;

	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }

		char score[4];
		score[0] = s[0];
		score[1] = s[1];
		score[2] = s[2];
		score[3] = 0;

		game->data->score += strtoul(score, NULL, 10);

		char path[255];

		snprintf(path, 255, "%s", text);
		data->status = strdup(path);

		game->data->evidence[game->data->evidence_len] = data->status;
		game->data->evidence_len++;

		al_play_sample_instance(data->evidence);

		data->speech_counter = 60 * 3;
	}
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		data->speech_counter--;
		return !data->speech_counter;
	}
	if (action->state == TM_ACTIONSTATE_DESTROY) {
		data->status = NULL;
	}
	return true;
}

static TM_ACTION(FadeIn) {
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		data->fade += 3;
		return data->fade >= 255;
	}
	if (action->state == TM_ACTIONSTATE_DESTROY) {
		data->fade = 255;
	}
	return false;
}

static TM_ACTION(FadeOut) {
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		data->fade -= 3;
		return data->fade <= 0;
	}
	if (action->state == TM_ACTIONSTATE_DESTROY) {
		data->fade = 0;
	}
	return false;
}

static TM_ACTION(GoTo) {
	char* name = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		PrintConsole(game, "jumping to label %s", name);
		data->skip_to = name;
	}
	return true;
}

static TM_ACTION(GoToCheck) {
	char* name = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		if (data->prev_selected == data->selected) {
			PrintConsole(game, "jumping to label %s", name);
			data->skip_to = name;
		}
	}
	return true;
}

static TM_ACTION(Label) {
	char* name = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_START) {
		if (!data->skip_to) return true;
		if (strcmp(data->skip_to, name) == 0) {
			PrintConsole(game, "jumped to label %s", name);
			data->skip_to = NULL;
		}
	}
	return true;
}

static TM_ACTION(RunScript) {
	char* name = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		game->data->script = name;
		UnloadCurrentGamestate(game);
		LoadGamestate(game, "dispatcher");
		StartGamestate(game, "dispatcher");
	}
	return true;
}

static TM_ACTION(SetActor) {
	char* name = TM_GetArg(action->arguments, 0);

	if (action->state == TM_ACTIONSTATE_INIT) {
		char path[255];

		snprintf(path, 255, "actors/%s-%s.png", name, "default");
		action->arguments = TM_AddToArgs(action->arguments, 1, al_load_bitmap(GetDataFilePath(game, path)));
	}
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		ALLEGRO_BITMAP* bitmap = TM_GetArg(action->arguments, 1);

		PrintConsole(game, "Setting up actor %s", name);
		if (data->actor) {
			al_destroy_bitmap(data->actor);
		}
		if (data->cur_emoti) {
			free(data->cur_emoti);
		}
		if (data->cur_actor) {
			free(data->cur_actor);
		}
		data->cur_emoti = strdup("default");
		data->cur_actor = strdup(name);
		data->actor = bitmap;
	}
	return true;
}

static TM_ACTION(SetEmoti) {
	char* name = TM_GetArg(action->arguments, 0);

	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }

		char path[255];

		snprintf(path, 255, "actors/%s-%s.png", data->cur_actor, name);
		ALLEGRO_BITMAP* bitmap = al_load_bitmap(GetDataFilePath(game, path));

		PrintConsole(game, "Setting up emoti %s for actor %s", name, data->cur_actor);
		if (data->actor) {
			al_destroy_bitmap(data->actor);
		}
		if (data->cur_emoti) {
			free(data->cur_emoti);
		}
		data->cur_emoti = strdup(name);
		data->actor = bitmap;
	}
	return true;
}

static TM_ACTION(SetScene) {
	char* name = TM_GetArg(action->arguments, 0);
	if (action->state == TM_ACTIONSTATE_INIT) {
		struct Character* character = CreateCharacter(game, name);
		RegisterSpritesheet(game, character, "scene");
		LoadSpritesheets(game, character, NULL);
		SelectSpritesheet(game, character, "scene");
		SetCharacterPosition(game, character, 0, 0, 0);
		action->arguments = TM_AddToArgs(action->arguments, 1, character);
	}
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		PrintConsole(game, "Setting up scene %s", name);
		if (data->scene) {
			DestroyCharacter(game, data->scene);
		}
		data->scene = TM_GetArg(action->arguments, 1);
	}
	return true;
}

static TM_ACTION(ExitGame) {
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		UnloadCurrentGamestate(game);
		LoadGamestate(game, "menu");
		StartGamestate(game, "menu");
	}
	return true;
}

static TM_ACTION(AddDialogOption) {
	char* arg = TM_GetArg(action->arguments, 0);

	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		data->dialog_count++;
		data->dialogs[data->dialog_count].timeline = TM_Init(game, data, "dialog-option");
		data->dialogs[data->dialog_count].text = arg;
		TM_AddAction(data->dialogs[data->dialog_count].timeline, Speak, TM_AddToArgs(NULL, 2, arg, 1));
	}
	return true;
}
void InterpretCommand(struct Game* game, struct GamestateResources* data, struct Timeline* timeline, char* buf);

static TM_ACTION(AddDialogSubcommand) {
	char* arg = TM_GetArg(action->arguments, 0);

	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		InterpretCommand(game, data, data->dialogs[data->dialog_count].timeline, arg);
	}
	return true;
}

static TM_ACTION(ExecuteDialogTree) {
	if (action->state == TM_ACTIONSTATE_START) {
		if (data->skip_to) { return true; }
		data->selected = -1;
		data->dialog_highlight = -1;
		data->dialog_enabled = true;
	}
	if (action->state == TM_ACTIONSTATE_RUNNING) {
		if (data->skip_to) { return true; }
		if ((data->selected == -1) || (data->selected > data->dialog_count)) {
			return false;
		} else {
			data->dialog_enabled = false;
		}

		TM_Process(data->dialogs[data->selected].timeline, action->delta);
		return TM_IsEmpty(data->dialogs[data->selected].timeline);
	}
	if (action->state == TM_ACTIONSTATE_DESTROY) {
		for (int i = 0; i <= data->dialog_count; i++) {
			TM_Destroy(data->dialogs[i].timeline);
		}
		data->dialog_count = -1;
		data->prev_selected = data->selected;
	}
	return false;
}

void InterpretCommand(struct Game* game, struct GamestateResources* data, struct Timeline* timeline, char* buf) {
	char* cmd = strdup(buf);
	if ((!cmd[0]) || (cmd[0] == '-') || (cmd[0] == '\n')) {
		free(cmd);
		return;
	}
	cmd[5] = '\0';
	char* arg = NULL;
	if (strlen(buf) > 6) {
		arg = cmd + 6;
		if (arg[strlen(arg) - 1] == '\n') {
			arg[strlen(arg) - 1] = 0;
		}
	}
	if (strcmp(cmd, "ACTOR") == 0) {
		TM_AddAction(timeline, SetActor, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "SCENE") == 0) {
		TM_AddAction(timeline, SetScene, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "DELAY") == 0) {
		TM_AddAction(timeline, Delay, TM_AddToArgs(NULL, 1, arg));

		//TM_AddDelay(data->timeline, strtoul(arg, NULL, 10)*60);
	} else if (strcmp(cmd, "SAYAC") == 0) {
		PrintConsole(game, "say actor");
		TM_AddAction(timeline, Speak, TM_AddToArgs(NULL, 2, arg, 0));
	} else if (strcmp(cmd, "SAYJK") == 0) {
		PrintConsole(game, "say jack");
		TM_AddAction(timeline, Speak, TM_AddToArgs(NULL, 2, arg, 1));
	} else if (strcmp(cmd, "FADEI") == 0) {
		TM_AddAction(timeline, FadeIn, NULL);
	} else if (strcmp(cmd, "FADEO") == 0) {
		TM_AddAction(timeline, FadeOut, NULL);
	} else if (strcmp(cmd, "EVIDE") == 0) {
		PrintConsole(game, "evidence");
		TM_AddAction(timeline, ShowEvidence, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "DIALO") == 0) {
		PrintConsole(game, "dialog tree");
		TM_AddAction(timeline, AddDialogOption, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "  CMD") == 0) {
		PrintConsole(game, "  -- subcommand");
		TM_AddAction(timeline, AddDialogSubcommand, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "RUND!") == 0) {
		PrintConsole(game, "RUN dialog tree!");
		TM_AddAction(timeline, ExecuteDialogTree, NULL);

	} else if (strcmp(cmd, "TUTOR") == 0) {
		PrintConsole(game, "tutorial block");
		TM_AddAction(timeline, Tutorial, TM_AddToArgs(NULL, 1, arg));

	} else if (strcmp(cmd, "GOTO!") == 0) {
		PrintConsole(game, "GO TO");
		TM_AddAction(timeline, GoTo, TM_AddToArgs(NULL, 1, arg));

	} else if (strcmp(cmd, "CHECK") == 0) {
		PrintConsole(game, "GO TO IF SAME");
		TM_AddAction(timeline, GoToCheck, TM_AddToArgs(NULL, 1, arg));

	} else if (strcmp(cmd, "EMOTI") == 0) {
		TM_AddAction(timeline, SetEmoti, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "LABEL") == 0) {
		PrintConsole(game, "label achieved");
		TM_AddAction(timeline, Label, TM_AddToArgs(NULL, 1, arg));

	} else if (strcmp(cmd, "SCRIP") == 0) {
		PrintConsole(game, "switch to script");
		TM_AddAction(timeline, RunScript, TM_AddToArgs(NULL, 1, arg));
	} else if (strcmp(cmd, "NOTES") == 0) {
		PrintConsole(game, "sum up the game");
		TM_AddAction(timeline, NotebookOverview, NULL);
	} else if (strcmp(cmd, "CLOSE") == 0) {
		PrintConsole(game, "close the game");
		TM_AddAction(timeline, ExitGame, NULL);

	} else {
		FatalError(game, false, "UNRECOGNIZED COMMAND %s", buf);
	}
	// FIXME: cmd/buf leaking
}

void* Gamestate_Load(struct Game* game, void (*progress)(struct Game*)) {
	// Called once, when the gamestate library is being loaded.
	// Good place for allocating memory, loading bitmaps etc.
	struct GamestateResources* data = calloc(1, sizeof(struct GamestateResources)); // everything is NULL at this point
	al_set_new_bitmap_flags(al_get_new_bitmap_flags() & ~ALLEGRO_MAG_LINEAR);

	data->font = al_create_builtin_font();

	char path[255];
	snprintf(path, 255, "scripts/%s.sd", game->data->script);
	data->script_file = al_fopen(GetDataFilePath(game, path), "r");
	data->icon = al_load_bitmap(GetDataFilePath(game, "icons/lovecase.png"));
	data->notebook = al_load_bitmap(GetDataFilePath(game, "notebook.png"));
	data->notebook_hand = al_load_bitmap(GetDataFilePath(game, "notebook-hand.png"));
	data->cursor = al_load_bitmap(GetDataFilePath(game, "cursor.png"));

	data->sample_ev = al_load_sample(GetDataFilePath(game, "evidence.flac"));
	data->evidence = al_create_sample_instance(data->sample_ev);
	al_attach_sample_instance_to_mixer(data->evidence, game->audio.fx);

	data->timeline = TM_Init(game, data, "script");

	data->button = al_load_bitmap(GetDataFilePath(game, "button.png"));

	do {
		char buf[255];
		al_fgets(data->script_file, buf, 255);
		InterpretCommand(game, data, data->timeline, buf);
	} while (!al_feof(data->script_file));

	progress(game); // report that we progressed with the loading, so the engine can draw a progress bar
	return data;
}

void Gamestate_Unload(struct Game* game, struct GamestateResources* data) {
	// Called when the gamestate library is being unloaded.
	// Good place for freeing all allocated memory and resources.
	al_destroy_font(data->font);
	al_fclose(data->script_file);
	TM_Destroy(data->timeline);
	if (data->scene) {
		DestroyCharacter(game, data->scene);
	}
	if (data->actor) {
		al_destroy_bitmap(data->actor);
	}
	if (data->cur_emoti) {
		free(data->cur_emoti);
	}
	if (data->cur_actor) {
		free(data->cur_actor);
	}
	al_destroy_bitmap(data->button);
	al_destroy_bitmap(data->icon);
	al_destroy_bitmap(data->notebook);
	al_destroy_bitmap(data->notebook_hand);
	al_destroy_bitmap(data->cursor);
	al_destroy_sample_instance(data->evidence);
	al_destroy_sample(data->sample_ev);
	free(data);
}

void Gamestate_Start(struct Game* game, struct GamestateResources* data) {
	// Called when this gamestate gets control. Good place for initializing state,
	// playing music etc.
	data->dialog_count = -1;
	/*data->speech = "Spicz";
	data->status = "Ewidens";
	data->dialog_count = 4;
	data->dialogs[0] = (struct Dialog){.text = "lala1"};
	data->dialogs[1] = (struct Dialog){.text = "lala2"};
	data->dialogs[2] = (struct Dialog){.text = "lala3"};
	data->dialogs[3] = (struct Dialog){.text = "lala4"};*/
}

void Gamestate_Stop(struct Game* game, struct GamestateResources* data) {
	// Called when gamestate gets stopped. Stop timers, music etc. here.
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
