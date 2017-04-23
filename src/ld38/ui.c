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
#include <ld38/ui.h>
#include <string.h>

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
    /** Current text, if any */
    gfmTilemap *pTextWindow;
    gfmText *pText;
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

/** Check whether text is currently active. Return 1 on true */
uint32_t ui_isTextActive() {
    gfmRV rv;

    if (!(ui.flags & UI_WINDOW_VISIBLE)) {
        return 0;
    }

    rv = gfmText_update(ui.pText, game.pCtx);
    //ASSERT(rv == GFMRV_OK, 0);
    if (DID_JUST_PRESS(action)) {
        if (gfmText_didFinish(ui.pText) == GFMRV_TRUE) {
            ui.flags &= ~UI_WINDOW_VISIBLE;
            return 0;
        }
        else {
            rv = gfmText_forceFinish(ui.pText);
            ASSERT(rv == GFMRV_OK, 0);
        }
    }

    return 1;
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
        default: {
        }
    }

    rv = gfmText_setPosition(ui.pVerb, x + offX, y + offY);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmText_update(ui.pVerb, game.pCtx);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

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
        rv = gfmTilemap_draw(ui.pTextWindow, game.pCtx);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
        rv = gfmText_draw(ui.pText, game.pCtx);
        ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    }

    return ERR_OK;
}

