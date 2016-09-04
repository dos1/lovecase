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

struct CommonResources* CreateGameData(struct Game *game) {
	struct CommonResources *data = calloc(1, sizeof(struct CommonResources));
	data->script = "000-intro";
	data->evidence_len = 0;
	data->evidence[data->evidence_len] = NULL;

	data->sample = al_load_sample(GetDataFilePath(game, "menu.flac"));
	data->music = al_create_sample_instance(data->sample);
	al_attach_sample_instance_to_mixer(data->music, game->audio.music);
	al_set_sample_instance_playmode(data->music, ALLEGRO_PLAYMODE_LOOP);


	data->sample2 = al_load_sample(GetDataFilePath(game, "music.flac"));
	data->music2 = al_create_sample_instance(data->sample2);
	al_attach_sample_instance_to_mixer(data->music2, game->audio.music);
	al_set_sample_instance_playmode(data->music2, ALLEGRO_PLAYMODE_LOOP);

	data->notebook_enabled = game->config.debug;

	return data;
}

void DestroyGameData(struct Game *game, struct CommonResources *resources) {
	al_stop_sample_instance(game->data->music2);
	al_stop_sample_instance(game->data->music);
	free(resources);
}

int DrawWrappedText(ALLEGRO_FONT *af,char atext[1024],ALLEGRO_COLOR fc, int x1, int y1, int width, int flags,bool draw)
{
	   char stext[1024]; // Copy of the passed text.
		 char * pch; // A pointer to each word.
		 char word[255]; // A string containing the word (for convienence)
		 char breakchar[12]; // Contains the break line character "\n "
		 char Lines[40][1024]; // A lovely array of strings to hold all the lines (40 max atm)
		 char TempLine[1024]; // Holds the string data of the current line only.
		 int CurrentLine = 0; // Counts which line we are currently using.
		 int q; // Used for loops

		 // Setup our strings
		 strcpy(stext,atext);
		 strcpy(breakchar,"\n ");
		 strcpy(TempLine,"");
		 for(q = 0;q < 40;q+=1)
		 {
			    sprintf(Lines[q],"");
		 }
		 //-------------------- Code Begins

		 pch = strtok (stext," ");                               // Get the first word.
		 do
		 {
			    strcpy(word,"");                                  // Truncate the string, to ensure there's no crazy stuff in there from memory.
					sprintf(word,"%s ",pch);
					sprintf(TempLine,"%s%s",TempLine,word);             // Append the word to the end of TempLine
					// This code checks for the new line character.
					if(strcmp(word,breakchar) == 0)
					{
						    CurrentLine+=1;                                 // Move down a Line
								strcpy(TempLine,"");                            // Clear the tempstring
					}
					else
					{
						    if(al_get_text_width(af,TempLine) >= (width))   // Check if text is larger than the area.
								{
									    strcpy(TempLine,word);                      // clear the templine and add the word to it.
											CurrentLine+=1;                             // Move to the next line.
								}
								if(CurrentLine < 40)
								{
									    strcat(Lines[CurrentLine],word);                // Append the word to whatever line we are currently on.
								}
					}
					pch = strtok (NULL, " ");                           // Get the next word.
		 }while (pch != NULL);
		 // ---------------------------------- Time to draw.
		 if(draw == true)                                       //Check whether we are actually drawing the text.
		 {
			    for(q = 0;q <=CurrentLine;q+=1)                     // Move through each line and draw according to the passed flags.
					{
						    if(flags == ALLEGRO_ALIGN_LEFT)
									   al_draw_text(af,fc, x1, y1 + (q * al_get_font_line_height(af)), ALLEGRO_ALIGN_LEFT,Lines[q]);
								if(flags == ALLEGRO_ALIGN_CENTRE)
									   al_draw_text(af,fc, x1 + (width/2), y1 + (q * al_get_font_line_height(af)), ALLEGRO_ALIGN_CENTRE,Lines[q]);
								if(flags == ALLEGRO_ALIGN_RIGHT)
									   al_draw_text(af,fc, x1 + width, y1 + (q * al_get_font_line_height(af)), ALLEGRO_ALIGN_RIGHT,Lines[q]);
					}

		 }
		 return((CurrentLine+1) * al_get_font_line_height(af));  // Return the actual height of the text in pixels.
}
