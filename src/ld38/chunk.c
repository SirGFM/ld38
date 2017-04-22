/**
 * A single (possibly nested) instance of the world. Contains all
 * objects/entities that the player may interact with, as well as a pointer to
 * any parenting chunk (so they may all be rendered).
 */
#include <base/error.h>
#include <base/game.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmTilemap.h>
#include <ld38/chunk.h>

#include <stdlib.h>

struct stChunk {
    /** The tilemap */
    gfmTilemap *pMap;
    /** Parent chunk, rendered before this */
    chunk *pParent;
    /** Name of the chunk, so it may be searched */
    char *pName;
};

/** Free all memory used by a chunk */
void chunk_clean(chunk **ppCtx) {
    chunk *pCtx;

    if (!ppCtx || !*ppCtx) {
        return;
    }
    pCtx = *ppCtx;

    if (pCtx->pMap) {
        gfmTilemap_free(&pCtx->pMap);
    }
    if (pCtx->pName) {
        free(pCtx->pName);
    }

    free(*ppCtx);
    *ppCtx = 0;
}

/** Alloc and initialize a new chunk. The parser may be uninitialized, since
 * it's recycled, but it shall not be NULL */
err chunk_init(chunk **ppCtx, gfmParser *pParser, const char *pTilemap
        , const char *pObjects) {
    chunk *pCtx;
    gfmRV rv;

    ASSERT(ppCtx, ERR_ARGUMENTBAD);
    ASSERT(pParser, ERR_ARGUMENTBAD);
    ASSERT(pTilemap, ERR_ARGUMENTBAD);
    ASSERT(pObjects, ERR_ARGUMENTBAD);

    pCtx = calloc(1, sizeof(chunk));
    ASSERT(pCtx, ERR_ALLOC_FAILED);
    *ppCtx = pCtx;

    rv = gfmTilemap_getNew(&pCtx->pMap);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

/** Reset the chunk to its initial state */
err chunk_reset(chunk *pCtx) {
    ASSERT(pCtx, ERR_ARGUMENTBAD);

    pCtx->pParent = 0;

    return ERR_OK;
}

/** Update the chunk and load this frame's collision information */
err chunk_update(chunk *pCtx) {
    ASSERT(pCtx, ERR_ARGUMENTBAD);

    /* TODO Re-initialize the quadtree to this chunk's dimensions */
    /* TODO Add geometry to the quadtree */

    return ERR_OK;
}

/** Draw this chunk and its parents */
err chunk_draw(chunk *pCtx) {
    err erv;

    ASSERT(pCtx, ERR_ARGUMENTBAD);

    if (pCtx->pParent) {
        erv = chunk_draw(pCtx->pParent);
        ASSERT(erv == ERR_OK, erv);
    }

    return ERR_OK;
}

