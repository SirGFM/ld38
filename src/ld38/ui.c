/**
 * Manage rendering the UI
 */
#include <base/error.h>
#include <base/game.h>
#include <base/gfx.h>
#include <base/input.h>
#include <GFraMe/gfmText.h>
#include <GFraMe/gfmTilemap.h>
#include <ld38/interactable.h>
#include <ld38/sfx.h>
#include <ld38/ui.h>
#include <string.h>

#define MAX_TEXT_QUEUE  100
#define TEXT_DELAY      100
#define VERB_MAXWIDTH   12
#define WINDOW_WIDTH    40
#define WINDOW_HEIGHT   30
#define WINDOW_TILE     -1
#define WINDOW_FILE     "lvl/ui/window.gfm"
#define WINDOW_X        12
#define WINDOW_Y        12
#define WINDOW_MAXWIDTH 37
#define WINDOW_MAXLINES 2

static char *pDictNames[] = {"dummy"};
static int pDictTypes[] = {0};
static int dictLen = sizeof(pDictTypes) / sizeof(int);

enum enUiFlags {
    UI_VERB_VISIBLE   = 0x0001
  , UI_WINDOW_VISIBLE = 0x0002
};

struct stUi {
    /** Current text, if any (and queue of next texts) */
    gfmTilemap *pTextWindow;
    gfmText *pText;
    char *pTextQueue[MAX_TEXT_QUEUE];
    uint32_t lenQueue;
    /** The current verb */
    gfmText *pVerb;
    action verb;
    /** Controls rendering everything */
    enum enUiFlags flags;
};
static struct stUi ui = {0};

/** Initialize the UI */
err ui_init() {
    gfmRV rv;

    /* Initialize the command text */
    rv = gfmText_getNew(&ui.pVerb);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmText_init(ui.pVerb, 0/*x*/, 0/*y*/, VERB_MAXWIDTH, 1/*maxLines*/
            , TEXT_DELAY, 0/*reserved*/, gfx.pSset8x8, 0/*first tile*/);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    /* Initialize the text window */
    rv = gfmTilemap_getNew(&ui.pTextWindow);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmTilemap_init(ui.pTextWindow, gfx.pSset8x8, WINDOW_WIDTH, WINDOW_HEIGHT
            , WINDOW_TILE);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmTilemap_loadf(ui.pTextWindow, game.pCtx, WINDOW_FILE
            , sizeof(WINDOW_FILE) - 1, pDictNames, pDictTypes, dictLen);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    rv = gfmText_getNew(&ui.pText);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmText_init(ui.pText, WINDOW_X, WINDOW_Y, WINDOW_MAXWIDTH
            , WINDOW_MAXLINES, TEXT_DELAY, 0/*reserved*/, gfx.pSset8x8
            , 0/*first tile*/);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

/** Free all memory used by the UI */
void ui_clean() {
    gfmText_free(&ui.pVerb);
    gfmTilemap_free(&ui.pTextWindow);
}

/** Reset the UI */
void ui_reset() {
    ui.flags = 0;
}

/** Remove a single entry from the queue. Return whether there was another entry
 * or not. */
static uint32_t _ui_popQueue() {
    if (ui.lenQueue == 0) {
        return 0;
    }

    ui.lenQueue--;
    ui_setTextWindow(ui.pTextQueue[ui.lenQueue]);

    return 1;
}

static int did_skip = 0;
static void _playSound(gfmText *pText, int skip) {
    char c;

    if (gfmText_getJustRendered(&c, pText) == GFMRV_OK) {
        if (skip && !did_skip) {
            did_skip = ~did_skip;
            return;
        }

        did_skip = 0;
        playText();
    }
}

/** Check whether text is currently active. Return 1 on true */
uint32_t ui_isTextActive() {
    gfmRV rv;

    if (!(ui.flags & UI_WINDOW_VISIBLE)) {
        return 0;
    }

    rv = gfmText_update(ui.pText, game.pCtx);
    _playSound(ui.pText, 1);
    //ASSERT(rv == GFMRV_OK, 0);
    if (DID_JUST_PRESS(action)) {
        if (gfmText_didFinish(ui.pText) == GFMRV_TRUE) {
            /* Try to unqueue a text and finish */
            if (!_ui_popQueue()) {
                ui.flags &= ~UI_WINDOW_VISIBLE;
            }
        }
        else {
            rv = gfmText_forceFinish(ui.pText);
            ASSERT(rv == GFMRV_OK, 0);
        }

        playOk();
    }

    return 1;
}

/** Enqueue texts */
err ui_queueText(char **ppText, uint32_t num) {
    uint32_t i;

    ASSERT(num < MAX_TEXT_QUEUE, ERR_ARGUMENTBAD);

    /** Enqueue them in reverse order so it's easier to recover the last */
    memset(ui.pTextQueue, 0x0, sizeof(char*) * MAX_TEXT_QUEUE);
    for (i = 0; i < num; i++) {
        ui.pTextQueue[num - i - 1] = ppText[i];
    }

    ui.lenQueue = num;

    if (!(ui.flags & UI_WINDOW_VISIBLE)) {
        _ui_popQueue();
    }

    return ERR_OK;
}

/** Display a new text into the window */
err ui_setTextWindow(char *pText) {
    gfmRV rv;

    /* This will most likely come from a parsed level (exported from tiled)...
     * I.e., terrible idea! (anyone could change the text file and make fun
     * stuff) */
    rv = gfmText_setText(ui.pText, pText, strlen(pText), 1/*doCopy*/);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    ui.flags |= UI_WINDOW_VISIBLE;

    return ERR_OK;
}

/** Update the current verb. Position must be in screen-space! */
err ui_updateVerb(interactable *pEvent, int x, int y) {
    gfmRV rv;
    int offX, offY;

    if (!pEvent) {
        ui.verb = ACT_NONE;
        ui.flags &= ~UI_VERB_VISIBLE;

        return ERR_OK;
    }
        
    if (pEvent->verb != ui.verb) {
        switch (pEvent->verb) {
            case ACT_ENTER: {
                rv = gfmText_setTextStatic(ui.pVerb, "ENTER", 1/*doCopy*/);
            } break;
            case ACT_INSPECT: {
                rv = gfmText_setTextStatic(ui.pVerb, "INSPECT", 1/*doCopy*/);
            } break;
            case ACT_TALK: {
                rv = gfmText_setTextStatic(ui.pVerb, "TALK", 1/*doCopy*/);
            } break;
            default: {
                ASSERT(0, ERR_INVALID_EVENT);
            }
        }
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);

        ui.verb = pEvent->verb;
        ui.flags |= UI_VERB_VISIBLE;
    }

    offY = -12;
    switch (ui.verb) {
        case ACT_ENTER: {
            offX = -16;
        } break;
        case ACT_INSPECT: {
            offX = -24;
        } break;
        case ACT_TALK: {
            offX = -12;
        } break;
        default: {
        }
    }

    rv = gfmText_setPosition(ui.pVerb, x + offX, y + offY);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmText_update(ui.pVerb, game.pCtx);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    _playSound(ui.pVerb, 0);

    return ERR_OK;
}

/** Draw the ui */
err ui_draw() {
    gfmRV rv;

    if (ui.flags & UI_VERB_VISIBLE) {
        rv = gfmText_draw(ui.pVerb, game.pCtx);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    }
    if (ui.flags & UI_WINDOW_VISIBLE) {
        int x, y;

        gfmCamera_getPosition(&x, &y, game.pCamera);
        gfmTilemap_setPosition(ui.pTextWindow, x, y);
        rv = gfmTilemap_draw(ui.pTextWindow, game.pCtx);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        rv = gfmText_draw(ui.pText, game.pCtx);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    }

    return ERR_OK;
}

