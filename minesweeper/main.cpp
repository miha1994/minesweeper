#include "Game.h"

#include"read_write.h"

int main()
{
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