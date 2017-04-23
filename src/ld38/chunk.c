/**
 * A single (possibly nested) instance of the world. Contains all
 * objects/entities that the player may interact with, as well as a pointer to
 * any parenting chunk (so they may all be rendered).
 */
#include <base/collision.h>
#include <base/error.h>
#include <base/game.h>
#include <base/gfx.h>
#include <conf/type.h>
#include <GFraMe/gfmError.h>
#include <GFraMe/gfmHitbox.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmParser.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmTilemap.h>
#include <ld38/chunk.h>
#include <ld38/interactable.h>
#include <ld38/level_list.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HITBOX          128
#define MAX_INTERACTIBLE    64
#define TM_DEFAULT_WIDTH    40
#define TM_DEFAULT_HEIGHT   30
#define TM_DEFAULT_TILE     -1

static char *pDictNames[] = {
#define X(name, type)   name,
    X_TYPE_DICT
#undef X
};
static int pDictTypes[] = {
#define X(name, type)   type,
    X_TYPE_DICT
#undef X
};
static int dictLen = sizeof(pDictTypes) / sizeof(int);

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
    /* Chunk's position */
    int x;
    int y;
    /* Chunk's dimensions */
    int height;
    int width;
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

/** Retrieve the next interactable */
static interactable* _getInteractable(chunk *pCtx) {
    ASSERT(pCtx->interactableCount < MAX_INTERACTIBLE, 0);
    pCtx->interactableCount++;
    return pCtx->data + (pCtx->interactableCount - 1);
}

static err _doParseInteractable(chunk *pCtx, gfmParser *pParser
        , interactable *pData, type t) {
    gfmRV rv;
    int h, w, x, y;

    rv = gfmParser_getPos(&x, &y, pParser);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmParser_getDimensions(&w, &h, pParser);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    x += pCtx->x;
    y += pCtx->y;
    rv = gfmHitbox_initItem(pCtx->pAreas, pData, x, y, w, h, t
            , pCtx->areaCount);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    pCtx->areaCount++;

    return ERR_OK;
}

/** Alloc and initialize a new chunk. The parser may be uninitialized, since
 * it's recycled, but it shall not be NULL */
err chunk_init(chunk **ppCtx, gfmParser *pParser, const char *pTilemap
        , const char *pObjects) {
    chunk *pCtx;
    err erv;
    gfmRV rv;
    int i, len;

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

    pCtx->x = 0;
    pCtx->y = 0;
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
            rv = gfmParser_getPos(&pCtx->x, &pCtx->y, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);

            pCtx->x *= -1;
            pCtx->y *= -1;
        }
        else if (strcmp(pType, "wall") == 0) {
            int h, w, x, y;

            rv = gfmParser_getPos(&x, &y, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            rv = gfmParser_getDimensions(&w, &h, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);

            x += pCtx->x;
            y += pCtx->y;
            rv = gfmHitbox_initItem(pCtx->pAreas, 0/*pCtx*/, x, y, w, h, T_WALL
                    , pCtx->areaCount);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            pCtx->areaCount++;
        }
        else if (strcmp(pType, "door") == 0) {
            interactable *pData;
            char *pKey, *pVal;
            int num;

            pData = _getInteractable(pCtx);
            ASSERT(pData, ERR_PARSINGERR);

            rv = gfmParser_getNumProperties(&num, pParser);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            ASSERT(num == 1, ERR_PARSINGERR);
            rv = gfmParser_getProperty(&pKey, &pVal, pParser, 0/*index*/);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            ASSERT(strcmp(pKey, "target") == 0, ERR_PARSINGERR);

            if (strcmp(pVal, "pop") == 0) {
                pData->data.door.target = DOOR_TARGET_POP;
            }
            else {
                erv = levels_getIndex(&pData->data.door.target, pVal);
                ASSERT(erv == ERR_OK, erv);
            }
            pData->t = T_DOOR;
            pData->verb = ACT_ENTER;

            erv =_doParseInteractable(pCtx, pParser, pData, T_DOOR);
            ASSERT(erv == ERR_OK, erv);
        }
    }

    len = (int)strlen(pTilemap);
    rv = gfmTilemap_getNew(&pCtx->pMap);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmTilemap_init(pCtx->pMap, gfx.pSset8x8, TM_DEFAULT_WIDTH
            , TM_DEFAULT_HEIGHT, TM_DEFAULT_TILE);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmTilemap_loadf(pCtx->pMap, game.pCtx, (char*)pTilemap, len
            , pDictNames, pDictTypes, dictLen);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmTilemap_getDimension(&pCtx->width, &pCtx->height, pCtx->pMap);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmTilemap_setPosition(pCtx->pMap, pCtx->x, pCtx->y);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    /* Fix area's position */
    /* TODO Fix this bug on the framework! */
    rv = gfmTilemap_getAreasLength(&len, pCtx->pMap);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    for (i = 0; i < len; i++) {
        gfmObject *pObj;
        int x, y;

        rv = gfmTilemap_getArea(&pObj, pCtx->pMap, i);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        rv = gfmObject_getPosition(&x, &y, pObj);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        x += pCtx->x;
        y += pCtx->y;
        rv = gfmObject_setPosition(pObj, x, y);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    }

    return ERR_OK;
}

/** Reset the chunk to its initial state */
err chunk_reset(chunk *pCtx) {
    ASSERT(pCtx, ERR_ARGUMENTBAD);

    pCtx->pParent = 0;

    return ERR_OK;
}

/** Push a chunk. Returns the chunk itself. */
chunk* chunk_pushParent(chunk *pSelf, chunk *pParent) {
    pSelf->pParent = pParent;
    return pSelf;
}

/** Pop a chunk */
chunk* chunk_popParent(chunk *pCtx) {
    chunk *pTmp;

    pTmp = pCtx->pParent;
    pCtx->pParent = 0;

    return pTmp;
}

/** Update the chunk and load this frame's collision information */
err chunk_update(chunk *pCtx) {
    gfmRV rv;

    ASSERT(pCtx, ERR_ARGUMENTBAD);

    /* Re-initialize the quadtree to this chunk's dimensions */
    rv = gfmQuadtree_initRoot(collision.pQt, pCtx->x - 8, pCtx->y - 8
            , pCtx->width + 16, pCtx->height + 16, 8/*depth*/, 16/*nodes*/);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    /* Add geometry to the quadtree */
    rv = gfmQuadtree_populateTilemap(collision.pQt, pCtx->pMap);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmHitbox_populateQuadtree(pCtx->pAreas, collision.pQt, pCtx->areaCount);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

/** Draw this chunk and its parents */
err chunk_draw(chunk *pCtx) {
    err erv;
    gfmRV rv;

    ASSERT(pCtx, ERR_ARGUMENTBAD);

    if (pCtx->pParent) {
        erv = chunk_draw(pCtx->pParent);
        ASSERT(erv == ERR_OK, erv);
    }

    rv = gfmTilemap_draw(pCtx->pMap, game.pCtx);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

