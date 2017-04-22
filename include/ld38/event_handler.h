/**
 * Store the event that may be dispatched this frame and issue, if requested.
 */
#ifndef __EVENT_HANDLER_H__
#define __EVENT_HANDLER_H__

#include <ld38/interactable.h>

/** Unqueue any previous event. Must be called on the start of every frame. */
void eventHandler_unqueue();

/** Try to queue an event. It may be ignored if a higher priority event has
 * already been queued. */
void eventHandler_queue(interactable *pEvent);

/** Get the currently queued event, if any */
interactable* eventHandler_getQueued();

#endif /* __EVENT_HANDLER_H__ */

