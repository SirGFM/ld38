/**
 * A single (possibly nested) instance of the world. Contains all
 * objects/entities that the player may interact with, as well as a pointer to
 * any parenting chunk (so they may all be rendered).
 */
#ifndef __CHUNK_TYPE__
#define __CHUNK_TYPE__

/** "Export" the chunk type */
typedef struct stChunk chunk;

#endif /* __CHUNK_TYPE__ */

#ifndef __CHUNK_H__
#define __CHUNK_H__

#include <base/error.h>
#include <GFraMe/gfmParser.h>

/** Free all memory used by a chunk */
void chunk_clean(chunk **ppCtx);

/** Alloc and initialize a new chunk. The parser may be uninitialized, since
 * it's recycled, but it shall not be NULL */
err chunk_init(chunk **ppCtx, gfmParser *pParser, const char *pTilemap
        , const char *pObjects);

/** Reset the chunk to its initial state */
err chunk_reset(chunk *pCtx);

/** Push a chunk. Returns the chunk itself. */
chunk* chunk_pushParent(chunk *pSelf, chunk *pParent);

/** Pop a chunk */
chunk* chunk_popParent(chunk *pCtx);

/** Configure the camera to this chunk */
void chunk_configureCamera(chunk *pCtx);

/** Update the chunk and load this frame's collision information */
err chunk_update(chunk *pCtx);

/** Draw this chunk and its parents */
err chunk_draw(chunk *pCtx);

#endif /* __CHUNK_H__*/

