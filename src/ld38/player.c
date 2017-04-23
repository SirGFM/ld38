/**
 * Controls the player (and propagates its commands to the playstate)
 */
#include <base/collision.h>
#include <base/error.h>
#include <base/game.h>
#include <base/gfx.h>
#include <base/input.h>
#include <conf/type.h>
#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>
#include <ld38/entity_macros.h>
#include <ld38/player.h>
#include <stdint.h>

/** TODO ADJUST */
#define PL_SPEED                TILES_TO_PX(7)
#define PL_JUMP_SPEED           JUMP_SPEED(25, 3)
#define PL_JUMP_ACCELERATION    JUMP_ACCELERATION(25, 3)
#define PL_FALL_ACCELERATION    JUMP_ACCELERATION(18, 3)

enum enPlayerAnim {
    PL_STAND = 0
  , PL_WALK
  , PL_JUMP
  , PL_ANIM_COUNT
};
typedef enum enPlayerAnim playerAnim;

static int _playerAnimData[] = {
             /* len | fps | loop | frames */
/* PL_STAND */   1  ,  0  ,  0   , 48
/* PL_WALK  */,  4  ,  10 ,  1   , 49,50,51,52
/* PL_JUMP  */,  1  ,  0  ,  0   , 51
};

struct stPlayer {
    gfmSprite *pSelf;
    uint32_t justJumped;
};

static struct stPlayer player = {0};

/** Create the player (load its animations and stuff) */
err player_init() {
    gfmRV rv;

    rv = gfmSprite_getNew(&player.pSelf);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmSprite_init(player.pSelf, 0/*x*/, 0/*y*/, 6/*width*/, 12/*height*/
            , gfx.pSset16x16, -5/*offX*/, 0/*offY*/, &player, T_PLAYER);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmSprite_setVerticalAcceleration(player.pSelf, PL_FALL_ACCELERATION);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    rv = gfmSprite_addAnimationsStatic(player.pSelf, _playerAnimData);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmSprite_playAnimation(player.pSelf, PL_STAND);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

/** Free all memory used by the player */
void player_clean() {
    gfmSprite_free(&player.pSelf);
}

/** Return the player to its original position */
err player_reset() {
    /* TODO Implement this */
    return ERR_OK;
}

/** Update the player */
err player_update() {
    double vx, vy;
    err erv;
    gfmRV rv;
    gfmCollision dir;

    player.justJumped = 0;

    if (IS_PRESSED(right)) {
        vx = PL_SPEED;
        rv = gfmSprite_setDirection(player.pSelf, 0);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    }
    else if (IS_PRESSED(left)) {
        vx = -PL_SPEED;
        rv = gfmSprite_setDirection(player.pSelf, 1);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    }
    else {
        vx = 0;
    }
    rv = gfmSprite_setHorizontalVelocity(player.pSelf, vx);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    rv = gfmSprite_getCollision(&dir, player.pSelf);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    if (!(dir & gfmCollision_down)) {
        rv = gfmSprite_getVerticalVelocity(&vy, player.pSelf);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        if (vy >= 0) {
            rv = gfmSprite_setVerticalAcceleration(player.pSelf
                    , PL_FALL_ACCELERATION);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        }
    }

    rv = gfmSprite_update(player.pSelf, game.pCtx);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    rv = gfmQuadtree_collideSprite(collision.pQt, player.pSelf);
    if (rv == GFMRV_QUADTREE_OVERLAPED) {
        erv = doCollide(collision.pQt);
        ASSERT(erv == ERR_OK, erv);
        rv = GFMRV_QUADTREE_DONE;
    }
    ASSERT(rv == GFMRV_QUADTREE_DONE, ERR_GFMERR);

    do {
        int x, y;

        gfmSprite_getCenter(&x, &y, player.pSelf);
        gfmCamera_centerAtPoint(game.pCamera, x, y);
    } while (0);

    return ERR_OK;
}

/** Draw the player */
err player_draw() {
    double vx;
    gfmRV rv;
    gfmCollision dir;

    rv = gfmSprite_getCollision(&dir, player.pSelf);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    if (!player.justJumped && (dir & gfmCollision_down)) {
        rv = gfmSprite_getHorizontalVelocity(&vx, player.pSelf);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);

        if (vx != 0) {
            rv = gfmSprite_playAnimation(player.pSelf, PL_WALK);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        }
        else {
            rv = gfmSprite_playAnimation(player.pSelf, PL_STAND);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
            rv = gfmSprite_resetAnimation(player.pSelf);
            ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        }
    }

    rv = gfmSprite_draw(player.pSelf, game.pCtx);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

/** Try to jump */
err player_tryJump() {
    gfmRV rv;
    gfmCollision dir;

    rv = gfmSprite_getCollision(&dir, player.pSelf);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    if ((dir & gfmCollision_down) && DID_JUST_PRESS(action)) {
        rv = gfmSprite_setVerticalVelocity(player.pSelf, PL_JUMP_SPEED);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        rv = gfmSprite_setVerticalAcceleration(player.pSelf
                , PL_JUMP_ACCELERATION);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);

        rv = gfmSprite_playAnimation(player.pSelf, PL_JUMP);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        rv = gfmSprite_resetAnimation(player.pSelf);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);

        player.justJumped = 1;
    }

    return ERR_OK;
}

/** Retrieve the player's position */
void player_getTopLeftPosition(int *pX, int *pY) {
    gfmSprite_getPosition(pX, pY, player.pSelf);
}

