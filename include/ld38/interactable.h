/**
 * Data for stuff that may be interacted with (maybe?)
 */
#ifndef __INTERACTABLE_H__
#define __INTERACTABLE_H__

#include <conf/type.h>
#include <stdint.h>

#define DOOR_TARGET_POP 0x10000

/** List of actions. The priority grows with the number/index. */
enum enAction {
    ACT_NONE = 0
  , ACT_ENTER
  , ACT_MAX
};
typedef enum enAction action;

/** Data used by doors */
struct stDoor {
    /** Index of the door's target */
    uint32_t target;
};

/** Merge different interactables' data types into a single structure */
union unInteractableData {
    struct stDoor door;
};

/** Data structure for any interactable */
struct stInteractable {
    /** Type of this interactable */
    type t;
    /** Action used by the player to interact with this. */
    action verb;
    /** Data for the given type */
    union unInteractableData data;
};
typedef struct stInteractable interactable;

#endif /* __INTERACTABLE_H__ */

