/**
 * The game's main state. Contains most of the logic.
 */
#include <base/error.h>
#include <base/game.h>
#include <ld38/playstate.h>

struct stPlaystate {
    int dummy;
};
static struct stPlaystate playstate = {0};

/** Free all memory used by the playstate */
void playstate_clean() {
}

/** Initialize and alloc the playstate */
err playstate_init() {
    return ERR_OK;
}

/** Reset the playstate so it may start to run again */
err playstate_reset() {
    ASSERT(game.nextState == ST_PLAYSTATE, ERR_INVALID_STATE);

    return ERR_OK;
}

/** Update a single frame */
err playstate_update() {
    ASSERT(game.currentState == ST_PLAYSTATE, ERR_INVALID_STATE);

    return ERR_OK;
}

/** Draw a single frame */
err playstate_draw() {
    ASSERT(game.currentState == ST_PLAYSTATE, ERR_INVALID_STATE);

    return ERR_OK;
}

