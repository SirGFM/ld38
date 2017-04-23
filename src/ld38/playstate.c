/**
 * The game's main state. Contains most of the logic.
 */
#include <base/error.h>
#include <base/game.h>
#include <base/input.h>
#include <GFraMe/gfmCamera.h>
#include <GFraMe/gfmParser.h>
#include <ld38/chunk.h>
#include <ld38/event_handler.h>
#include <ld38/interactable.h>
#include <ld38/inventory.h>
#include <ld38/level_list.h>
#include <ld38/player.h>
#include <ld38/playstate.h>
#include <ld38/ui.h>
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
    /** Store which links the player has made */
    link links[NUM_LINKS];
};
static struct stPlaystate playstate = {0};

static char *pWinText[] = {
    "WHO WOULD HAVE THOUGHT THAT THEY LIVE SO CLOSE TO ME!?",
    "NOW I REALLY WANT TO GO TO THE EXIHIBIT AND MEET THEM...",
    "THIS IS SUCH A SMALL WORLD, AFTER ALL.",
    "WHO KNOWS WHAT ELSE I'LL FIND OUT AT EXIHIBIT?",
    "...",
    "(HELLO, IT'S THE DEV! UNFORTUNATELLY, THIS IS AS FAR AS THE GAME GOES...",
    "I DECIDED TO CUT IT SHORT BUT TRY TO PROPERLY FINISH IT...",
    "I HOPE YOU AT LEAST ENJOYED THE IDEA. :D",
    "THANKS FOR PLAYING!!)"
};
static uint32_t winTextCount = 9;

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
    chunk_configureCamera(playstate.pCurChunk);

    player_reset();
    ui_reset();

    return ERR_OK;
}

/** Update a single frame */
err playstate_update() {
    interactable *pEvent;
    err erv;
    gfmRV rv;
    int cx, cy, px, py;
    link i;

    ASSERT(game.currentState == ST_PLAYSTATE, ERR_INVALID_STATE);
    ASSERT(playstate.pCurChunk != 0, ERR_INVALID_STATE);

    /** Text takes control over the game */
    if (ui_isTextActive()) {
        return ERR_OK;
    }

    /** Check if any new link was made */
    for (i = 0; i < NUM_LINKS; i++) {
        if (!playstate.links[i] && inventory_checkLink(i)) {
            playstate.links[i] = 1;
            switch (i) {
                case 0: {
                    /* IGNORE */
                } break;
                case LINK_A: {
                    ui_queueText(pWinText, winTextCount);
                } break;
                default: {
                    ASSERT(0, ERR_INVALID_INDEX);
                }
            }
        }
    }

    eventHandler_unqueue();
    erv = chunk_update(playstate.pCurChunk);
    ASSERT(erv == ERR_OK, erv);
    erv = player_update();
    ASSERT(erv == ERR_OK, erv);

    pEvent = eventHandler_getQueued();
    if (pEvent != 0 && DID_JUST_PRESS(action)) {
        switch (pEvent->verb) {
            case ACT_INSPECT: {
                if (pEvent->t == T_FACT) {
                    inventory_addFact(pEvent->data.inventoryEntry.value);
                }
                else if (pEvent->t == T_ARTIFACT) {
                    inventory_addArtifact(pEvent->data.inventoryEntry.value);
                }
                erv = ui_queueText(pEvent->data.inventoryEntry.ppFlavor
                        , pEvent->data.inventoryEntry.numFlavors);
                ASSERT(erv == ERR_OK, erv);
            } break;
            case ACT_TALK: {
                inventory_addPerson(pEvent->data.inventoryEntry.value);
                erv = ui_queueText(pEvent->data.inventoryEntry.ppFlavor
                        , pEvent->data.inventoryEntry.numFlavors);
                ASSERT(erv == ERR_OK, erv);
            } break;
            case ACT_ENTER: {
                chunk *pNext;

                if (pEvent->data.door.target == DOOR_TARGET_POP) {
                    /* Pop a previously queued chunk */
                    pNext = chunk_popParent(playstate.pCurChunk);
                }
                else {
                    ASSERT(pEvent->data.door.target < MAX_CHUNK
                            , ERR_INVALID_INDEX);
                    pNext = chunk_pushParent(
                            playstate.pWorld[pEvent->data.door.target]
                            , playstate.pCurChunk);
                }
                playstate.pCurChunk = pNext;
            } break;
            default: {
                ASSERT(0, ERR_INVALID_EVENT);
            }
        }
    }
    else {
        erv = player_tryJump();
        ASSERT(erv == ERR_OK, erv);
    }

    player_getTopLeftPosition(&px, &py);
    rv = gfmCamera_getPosition(&cx, &cy, game.pCamera);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    erv = ui_updateVerb(pEvent, px - cx, py - cy);
    ASSERT(erv == ERR_OK, erv);

    return ERR_OK;
}

/** Draw a single frame */
err playstate_draw() {
    err erv;

    ASSERT(game.currentState == ST_PLAYSTATE, ERR_INVALID_STATE);

    erv = chunk_draw(playstate.pCurChunk);
    ASSERT(erv == ERR_OK, erv);
    erv = player_draw();
    ASSERT(erv == ERR_OK, erv);
    erv = ui_draw();
    ASSERT(erv == ERR_OK, erv);

    return ERR_OK;
}

