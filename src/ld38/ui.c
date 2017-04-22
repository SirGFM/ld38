/**
 * Manage rendering the UI
 */
#include <base/error.h>
#include <base/game.h>
#include <base/gfx.h>
#include <GFraMe/gfmText.h>
#include <ld38/interactable.h>
#include <ld38/ui.h>

#define TEXT_DELAY      100
#define VERB_MAXWIDTH   12

enum enUiFlags {
    UI_VERB_VISIBLE = 0x0001
};

struct stUi {
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

    rv = gfmText_getNew(&ui.pVerb);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfmText_init(ui.pVerb, 0/*x*/, 0/*y*/, VERB_MAXWIDTH, 1/*maxLines*/
            , TEXT_DELAY, 0/*reserved*/, gfx.pSset8x8, 0/*first tile*/);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

/** Free all memory used by the UI */
void ui_clean() {
    gfmText_free(&ui.pVerb);
}

/** Reset the UI */
void ui_reset() {
    ui.flags = 0;
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

    return ERR_OK;
}

