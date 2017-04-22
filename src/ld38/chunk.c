/**
 * A single (possibly nested) instance of the world. Contains all
 * objects/entities that the player may interact with, as well as a pointer to
 * any parenting chunk (so they may all be rendered).
 */
#include <base/error.h>
#include <base/game.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmHitbox.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmTilemap.h>
#include <ld38/chunk.h>
#include <ld38/interactable.h>
#include <ld38/level_list.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HITBOX          128
#define MAX_INTERACTIBLE    64

struct stChunk {
    /** The tilemap */
    gfmTilemap *pMap;
    /** Parent chunk, rendered before this */
    chunk *pParent;
    /** Areas with collision */
    gfmHitbox *pAreas;
    /** Number of areas used */
    uint32_t areaCount;
    /** Number of interactables used */
    uint32_t interactableCount;
    /** "Statically" alloc'ed data for any interactable within this chunk */
    interactable data[MAX_INTERACTIBLE];
};

/** Free all memory used by a chunk */
void chunk_clean(chunk **ppCtx) {
    chunk *pCtx;

    if (!ppCtx || !*ppCtx) {
        return;
    }
    pCtx = *ppCtx;

    gfmTilemap_free(&pCtx->pMap);
    gfmHitbox_free(&pCtx->pAreas);

    free(*ppCtx);
    *ppCtx = 0;
}

/** Alloc and initialize a new chunk. The parser may be uninitialized, since
 * it's recycled, but it shall not be NULL */
err chunk_init(chunk **ppCtx, gfmParser *pParser, const char *pTilemap
        , const char *pObjects) {
    chunk *pCtx;
    err erv;
    gfmRV rv;
    int len, offX, offY;

    ASSERT(ppCtx, ERR_ARGUMENTBAD);
    ASSERT(pParser, ERR_ARGUMENTBAD);
    ASSERT(pTilemap, ERR_ARGUMENTBAD);
    ASSERT(pObjects, ERR_ARGUMENTBAD);

    pCtx = calloc(1, sizeof(chunk));
    ASSERT(pCtx, ERR_ALLOC_FAILED);
    *ppCtx = pCtx;

    rv = gfmHitbox_getNewList(&pCtx->pAreas, MAX_HITBOX);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    /* First, parse the objects (mainly to retrieve the chunk's offset) */
    len = (int)strlen(pObjects);
    rv = gfmParser_init(pParser, game.pCtx, (char*)pObjects, len);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    offX = 0;
    offY = 0;
    while (1) {
        char *pType;

        rv = gfmParser_parseNext(pParser);
        ASSERT(rv == GFMRV_OK || rv == GFMRV_PARSER_FINISHED, ERR_GFMERR);
        if (rv == GFMRV_PARSER_FINISHED) {
            break;
        }

        /* These strcmp are safe because the parser guarantees it's a string of
         * resonable length */
        rv = gfmParser_getIngameType(&pType, pParser);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        if (strcmp(pType, "offset") == 0) {
            rv = gfmParser_getPos(&offX, &offY, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);

            offX *= -1;
            offY *= -1;
        }
        else if (strcmp(pType, "wall") == 0) {
            int h, w, x, y;

            rv = gfmParser_getPos(&x, &y, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            rv = gfmParser_getDimensions(&w, &h, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);

            x += offX;
            y += offY;
            rv = gfmHitbox_initItem(pCtx->pAreas, 0/*pCtx*/, x, y, w, h, T_WALL
                    , pCtx->areaCount);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            pCtx->areaCount++;
        }
        else if (strcmp(pType, "door") == 0) {
            interactable *pData;
            char *pKey, *pVal;
            int h, num, w, x, y;

            ASSERT(pCtx->interactableCount < MAX_INTERACTIBLE, ERR_PARSINGERR);

            rv = gfmParser_getPos(&x, &y, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            rv = gfmParser_getDimensions(&w, &h, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            pData = pCtx->data + pCtx->interactableCount;
            pCtx->interactableCount++;

            rv = gfmParser_getNumProperties(&num, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            ASSERT(num == 1, ERR_PARSINGERR);
            rv = gfmParser_getProperty(&pKey, &pVal, pParser, 0/*index*/);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            ASSERT(strcmp(pKey, "target") == 0, ERR_PARSINGERR);

            if (strcmp(pVal, "pop") == 0) {
                pData->data.door.target = 0;
            }
            else {
                erv = levels_getIndex(&pData->data.door.target, pVal);
                ASSERT(erv == ERR_OK, erv);
            }
            pData->t = T_DOOR;
            pData->verb = ACT_ENTER;

            x += offX;
            y += offY;
            rv = gfmHitbox_initItem(pCtx->pAreas, pData, x, y, w, h, T_DOOR
                    , pCtx->areaCount);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            pCtx->areaCount++;
        }
    }

    len = (int)strlen(pTilemap);
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

