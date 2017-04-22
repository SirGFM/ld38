/**
 * The game's main state. Contains most of the logic.
 */
#include <base/error.h>
#include <base/game.h>
#include <GFraMe/gfmParser.h>
#include <ld38/chunk.h>
#include <ld38/level_list.h>
#include <ld38/playstate.h>
#include <stdint.h>

#define MAX_CHUNK   32

struct stPlaystate {
    /** Game's parser */
    gfmParser *pParser;
    /** The active chunk */
    chunk *pCurChunk;
    /** All chunks loaded into the game. The main chunk is expected to be the
     * first one. */
    chunk *pWorld[MAX_CHUNK];
    /** How many chunks were loaded */
    uint32_t chunkCount;
};
static struct stPlaystate playstate = {0};

/** Free all memory used by the playstate */
void playstate_clean() {
    uint32_t i;

    gfmParser_free(&playstate.pParser);
    /* Try to clean every chunk (should be OK even if not initialized) */
    for (i = 0; i < MAX_CHUNK; i++) {
        chunk_clean(playstate.pWorld + i);
    }
}

/** Initialize and alloc the playstate */
err playstate_init() {
    err erv;
    gfmRV rv;

    rv = gfmParser_getNew(&playstate.pParser);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    for (playstate.chunkCount = 0; playstate.chunkCount < levels_getNum();
            playstate.chunkCount++) {
        const char *pTilemap, *pObjects;

        ASSERT(playstate.chunkCount < MAX_CHUNK, ERR_INVALID_INDEX);

        erv = levels_getFiles(&pTilemap, &pObjects, playstate.chunkCount);
        ASSERT(erv == ERR_OK, erv);

        rv = gfmParser_reset(playstate.pParser);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        erv = chunk_init(playstate.pWorld + playstate.chunkCount
                , playstate.pParser, pTilemap, pObjects);
        ASSERT(erv == ERR_OK, erv);
    }

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
    playstate.pCurChunk = playstate.pWorld[levels_getMainIndex()];

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

