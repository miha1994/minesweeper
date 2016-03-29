#include "Game.h"
#include "read_write.h"
#include "some_structures.h"

int main()
{
	srand (41);
	dig_colors[1] = CLR(31,36,255,255);
	dig_colors[2] = CLR(0,142,0,255);
	dig_colors[3] = CLR(224,0,0,255);
	dig_colors[4] = CLR(0,0,166,255);
	dig_colors[5] = CLR(137,0,0,255);
	dig_colors[6] = CLR(0,138,148,255);
	dig_colors[7] = CLR(0,0,0,255);
	dig_colors[8] = CLR(122,122,122,255);

	Game *game = new Game ();
	if (game->init (true)) {
		while (game->running) {
			CNTRL ("run");
			game->update ();
			game->render ();
		}
		game->del ();
	}
    return 0;
}