/**
 * Controls the player (and propagates its commands to the playstate)
 */
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <base/error.h>

/** Create the player (load its animations and stuff) */
err player_init();

/** Free all memory used by the player */
void player_clean();

/** Return the player to its original position */
err player_reset();

/** Update the player */
err player_update();

/** Draw the player */
err player_draw();

#endif /* __PLAYER_H__ */

