#define LIBSUPERDERPY_DATA_TYPE struct CommonResources
#include <libsuperderpy.h>

struct CommonResources {
		// Fill in with common data accessible from all gamestates.
	char *script;
	char *evidence[255];
	int evidence_len;
	int score;

	ALLEGRO_SAMPLE *sample;
	ALLEGRO_SAMPLE_INSTANCE *music;

	ALLEGRO_SAMPLE *sample2;
	ALLEGRO_SAMPLE_INSTANCE *music2;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *resources);
int DrawWrappedText(ALLEGRO_FONT *af,char atext[1024],ALLEGRO_COLOR fc, int x1, int y1, int width, int flags,bool draw);
