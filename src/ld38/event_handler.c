/**
 * Store the event that may be dispatched this frame and issue, if requested.
 */
#include <ld38/event_handler.h>
#include <ld38/interactable.h>

static interactable *_pEvent;

/** Unqueue any previous event. Must be called on the start of every frame. */
void eventHandler_unqueue() {
    _pEvent = 0;
}

/** Try to queue an event. It may be ignored if a higher priority event has
 * already been queued. */
void eventHandler_queue(interactable *pEvent) {
    if (_pEvent == 0 || _pEvent->verb < pEvent->verb) {
        _pEvent = pEvent;
        /** TODO Re-draw the current verb on the UI */
    }
}

/** Get the currently queued event, if any */
interactable* eventHandler_getQueued() {
    return _pEvent;
}

