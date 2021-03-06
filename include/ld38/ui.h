/**
 * Manage rendering the UI
 */
#ifndef __UI_H__
#define __UI_H__

#include <base/error.h>
#include <ld38/interactable.h>

/** Initialize the UI */
err ui_init();

/** Free all memory used by the UI */
void ui_clean();

/** Reset the UI */
void ui_reset();

/** Check whether text is currently active. Return 1 on true */
uint32_t ui_isTextActive();

/** Enqueue texts */
err ui_queueText(char **ppText, uint32_t num);

/** Display a new text into the window */
err ui_setTextWindow(char *pText);

/** Update the current verb. Position must be in screen-space! */
err ui_updateVerb(interactable *pEvent, int x, int y);

/** Draw the ui */
err ui_draw();

#endif /* __UI_H__ */

