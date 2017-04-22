/**
 * @file src/collision.c
 *
 * Declare only the collision function.
 *
 * In a way to try and keep this saner, collision "sets" (e.g., if types 'a',
 * 'b' and and 'c' collide against 'a', 'd' or 'f') are described on a separated
 * json file, which automatically (i.e., from the Makefile) generates the
 * described switch-case code.
 */
#include <base/collision.h>
#include <base/error.h>
#include <base/game.h>
#include <conf/type.h>

#include <GFraMe/gfmError.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>

#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
#  include <stdlib.h>
#  include <signal.h>
#endif

/** Hold all pointers (and the type) for a colliding object */
struct stCollisionNode {
    gfmObject *pObject;
    gfmSprite *pSprite;
    void *pChild;
    int type;
};
typedef struct stCollisionNode collisionNode;

/** Merge two types into a single one */
#define MERGE_TYPES(type1, type2) \
    (TYPE(type1) | (TYPE(type2) << T_BITS))

/** Check if two entities of the specified type collided */
#define CASE(type1, type2) \
    case (MERGE_TYPES(type1, type2)):

/** Collide against self type */
#define SELFCASE(type) \
    case (MERGE_TYPES(type, type)):

/** Ignore collision between entities of different types */
#define IGNORE(type1, type2) \
    case (MERGE_TYPES(type1, type2)):

/** Collide the player against an interactable */
static inline err _onInteract(collisionNode *pInteractable
        , collisionNode *pPlayer) {
    /** TODO */
    return ERR_OK;
}

/** Collide a solid (floor or wall) against an entity */
static inline err _defaultFloorCollision(collisionNode *pSolid
        , collisionNode *pEntity) {
    gfmRV rv;

    rv = gfmObject_collide(pSolid->pObject, pEntity->pObject);
    if (rv == GFMRV_TRUE) {
        gfmCollision dir;

        gfmObject_getCurrentCollision(&dir, pEntity->pObject);
        if (dir & gfmCollision_down) {
            int ew, ex, ey, sw, sx;

            /* Check if colliding against the corner and push it */
            gfmObject_getPosition(&ex, &ey, pEntity->pObject);
            gfmObject_getWidth(&ew, pEntity->pObject);
            gfmObject_getHorizontalPosition(&sx, pSolid->pObject);
            gfmObject_getWidth(&sw, pSolid->pObject);

            if (ex + ew == sx) {
                /* Push slightly to the right */
                gfmObject_setPosition(pEntity->pObject, ex + 1, ey - 1);
            }
            else if (ex + ew == sx - 1) {
                /* Push slightly to the right */
                gfmObject_setPosition(pEntity->pObject, ex + 2, ey - 1);
            }
            else if (ex == sx + sw) {
                /* Push slightly to the left */
                gfmObject_setPosition(pEntity->pObject, ex - 1, ey - 1);
            }
            else if (ex == sx + sw + 1) {
                /* Push slightly to the left */
                gfmObject_setPosition(pEntity->pObject, ex - 2, ey - 1);
            }
            else {
                gfmObject_setVerticalVelocity(pEntity->pObject, 0);
            }
        }
        else if (dir & gfmCollision_up) {
            gfmObject_setVerticalVelocity(pEntity->pObject, 0);
        }
    }

    return ERR_OK;
}

/**
 * Retrieve the type and all the children for a given object.
 *
 * @param  [ in]pNode The object (with a valid gfmObject)
 */
static void _getSubtype(collisionNode *pNode) {
    gfmObject_getChild(&pNode->pChild, &pNode->type, pNode->pObject);

    if (pNode->type == gfmType_sprite && pNode->pChild) {
        pNode->pSprite = (gfmSprite*)pNode->pChild;
        gfmSprite_getChild(&pNode->pChild, &pNode->type, pNode->pSprite);
    }
}

/**
 * Continue handling collision.
 * 
 * Different from the other functions on this module, this one is declared on
 * src/collision.c (instead of src/base/collision.c). This decision was made
 * because this function shall be modified for each game.
 *
 * @param  [ in]pQt The current quadtree
 */
err doCollide(gfmQuadtreeRoot *pQt) {
    /** GFraMe return value */
    gfmRV rv;

    /* Continue colliding until the quadtree finishes (or collision is
     * skipped) */
    rv = GFMRV_QUADTREE_OVERLAPED;
    collision.flags  &= ~CF_SKIP;
    while (rv != GFMRV_QUADTREE_DONE && !(collision.flags & CF_SKIP)) {
        collisionNode node1, node2;
        err erv;

        /* Retrieve the two overlaping objects and their types */
        rv = gfmQuadtree_getOverlaping(&node1.pObject, &node2.pObject
                , pQt);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        _getSubtype(&node1);
        _getSubtype(&node2);

        /* Import the filtered collision tuples */
        #include <auto/collisioncases.c>

        /** Update the quadtree (so any other collision is detected) */
        rv = gfmQuadtree_continue(pQt);
        ASSERT(rv == GFMRV_QUADTREE_OVERLAPED || rv == GFMRV_QUADTREE_DONE,
                ERR_GFMERR);
    }

    return ERR_OK;
}

