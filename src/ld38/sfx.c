#include <base/error.h>
#include <base/game.h>
#include <GFraMe/gframe.h>
#include <GFraMe/core/gfmAudio_bkend.h>

int song;
int text;
int ok;
int jump;
int step;

err initSFX() {
    gfmRV rv;
    
    rv = gfm_loadAudio(&song, game.pCtx, "mml/song.mml", sizeof("mml/song.mml") - 1);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfm_loadAudio(&text, game.pCtx, "sfx/text.wav", sizeof("sfx/text.wav") - 1);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfm_loadAudio(&ok, game.pCtx, "sfx/ok.wav", sizeof("sfx/ok.wav") - 1);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfm_loadAudio(&jump, game.pCtx, "sfx/jump.wav", sizeof("sfx/jump.wav") - 1);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);
    rv = gfm_loadAudio(&step, game.pCtx, "sfx/step.wav", sizeof("sfx/step.wav") - 1);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

err playSong() {
    gfmRV rv;

    rv = gfm_playAudio(0, game.pCtx, song, 0.8);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

err playText() {
    gfmRV rv;

    rv = gfm_playAudio(0, game.pCtx, text, 0.4);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

err playOk() {
    gfmRV rv;

    rv = gfm_playAudio(0, game.pCtx, ok, 0.4);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

err playJump() {
    gfmRV rv;

    rv = gfm_playAudio(0, game.pCtx, jump, 0.4);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

err playStep() {
    gfmRV rv;

    rv = gfm_playAudio(0, game.pCtx, step, 0.4);
    ASSERT(rv == GFMRV_OK, ERR_GFMERR);

    return ERR_OK;
}

