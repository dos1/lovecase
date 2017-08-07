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

	bool touch;

	bool notebook_enabled;
};

struct CommonResources* CreateGameData(struct Game *game);
void DestroyGameData(struct Game *game, struct CommonResources *resources);
bool GlobalEventHandler(struct Game *game, ALLEGRO_EVENT *ev);
