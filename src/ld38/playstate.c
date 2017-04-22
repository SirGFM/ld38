/**
 * The game's main state. Contains most of the logic.
 */
#include <base/error.h>
#include <base/game.h>
#include <ld38/chunk.h>
#include <ld38/playstate.h>
#include <stdint.h>

#define MAX_CHUNK   32

char *level_list[] = {
    #include <auto/level_list.h>
};

struct stPlaystate {
    /** All chunks loaded into the game. The main chunk is expected to be the
     * first one. */
    chunk *pWorld[MAX_CHUNK];
    /** The active chunk */
    chunk *pCurChunk;
    /** How many chunks were loaded */
    uint32_t chunkCount;
};
static struct stPlaystate playstate = {0};

/** Free all memory used by the playstate */
void playstate_clean() {
    uint32_t i;

    /* Try to clean every chunk (should be OK even if not initialized) */
    for (i = 0; i < MAX_CHUNK; i++) {
        chunk_clean(playstate.pWorld + i);
    }
}

/** Initialize and alloc the playstate */
err playstate_init() {
    return ERR_OK;
}

/** Reset the playstate so it may start to run again */
err playstate_reset() {
    uint32_t i;
    err erv;

    ASSERT(game.nextState == ST_PLAYSTATE, ERR_INVALID_STATE);

    for (i = 0; i < playstate.chunkCount; i++) {
        erv = chunk_reset(playstate.pWorld[i]);
        ASSERT(erv == ERR_OK, erv);
    }
    playstate.pCurChunk = playstate.pWorld[0];

    return ERR_OK;
}

/** Update a single frame */
err playstate_update() {
    err erv;

    ASSERT(game.currentState == ST_PLAYSTATE, ERR_INVALID_STATE);

    erv = chunk_update(playstate.pCurChunk);
    ASSERT(erv == ERR_OK, erv);

    return ERR_OK;
}

/** Draw a single frame */
err playstate_draw() {
    err erv;

    ASSERT(game.currentState == ST_PLAYSTATE, ERR_INVALID_STATE);

    erv = chunk_draw(playstate.pCurChunk);
    ASSERT(erv == ERR_OK, erv);

    return ERR_OK;
}

