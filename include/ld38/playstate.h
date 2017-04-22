/**
 * The game's main state. Contains most of the logic.
 */
#ifndef __PLAYSTATE_H__
#define __PLAYSTATE_H__

#include <base/error.h>

/** Free all memory used by the playstate */
void playstate_clean();

/** Initialize and alloc the playstate */
err playstate_init();

/** Reset the playstate so it may start to run again */
err playstate_reset();

/** Update a single frame */
err playstate_update();

/** Draw a single frame */
err playstate_draw();

#endif /* __PLAYSTATE_H__ */

