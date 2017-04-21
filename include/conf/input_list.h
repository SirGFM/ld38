/**
 * @file include/conf/input_list.h
 *
 * List of available buttons and their default keys.
 *
 * To make the most of this header, two macros must be defined:
 *  X_KEY(name, key) - retrieve the name (i.e., variable) of a virtual button
 *                     and its virtual button
 *  X_GPAD(name, button, port) - retrieve the name (i.e., variable) of a virtual
 *                               button, its gamepad button and its port
 *
 * All virtual keys are created based on the 'X_KEY' macro.
 */
#ifndef __INPUT_LIST_H__
#define __INPUT_LIST_H__

/** Create a list of system buttons and their default mapping. */
#define X_SYSTEM_BUTTON_LIST \
  X_KEY(fullscreen \
        , gfmKey_f12)

/** Create a list of buttons and their default mapping. */
#define X_RELEASE_BUTTON_LIST \
  X_KEY(left \
        , gfmKey_left) \
  X_KEY(right \
        , gfmKey_right) \
  X_KEY(up \
        , gfmKey_up) \
  X_KEY(down \
        , gfmKey_down) \
  X_KEY(pause \
        , gfmKey_esc)


/** Add default alternate mappings for buttons */
#define X_ALTERNATE_BUTTON_MAPPING \
  X_GPAD(left \
        , gfmController_laxis_left \
        , 0/*port*/) \
  X_GPAD(right \
        , gfmController_laxis_right \
        , 0/*port*/) \
  X_GPAD(up \
        , gfmController_laxis_up \
        , 0/*port*/) \
  X_GPAD(down \
        , gfmController_laxis_down \
        , 0/*port*/) \
  X_GPAD(pause \
        , gfmController_start \
        , 0/*port*/)

/** Create a list of debug buttons */
#if defined(DEBUG)
#  define X_DEBUG_BUTTON_LIST \
     X_KEY(qt \
        , gfmKey_f11) \
     X_KEY(gif \
        , gfmKey_f10) \
     X_KEY(dbgResetFps \
        , gfmKey_f7) \
     X_KEY(dbgStep \
        , gfmKey_f6) \
     X_KEY(dbgPause \
        , gfmKey_f5)
#else
#  define X_DEBUG_BUTTON_LIST
#endif

/** Concatenate both lists */
#define X_BUTTON_LIST \
  X_RELEASE_BUTTON_LIST \
  X_SYSTEM_BUTTON_LIST \
  X_DEBUG_BUTTON_LIST

#endif /* __INPUT_LIST_H__ */

