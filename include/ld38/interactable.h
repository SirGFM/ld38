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
  , ACT_INSPECT
  , ACT_TALK
  , ACT_ENTER
  , ACT_MAX
};
typedef enum enAction action;

/** Data used by doors */
struct stDoor {
    /** Index of the door's target */
    uint32_t target;
};

/** Common interactable used by inventory dwellers */
struct stInventoryEntry {
    /** Value of the interactable (a fact, person or artifact) */
    uint32_t value;
    /** How many flavor texts the entry has */
    uint32_t numFlavors;
    /** List of flavor texts */
    char **ppFlavor;
};

/** Merge different interactables' data types into a single structure */
union unInteractableData {
    struct stDoor door;
    struct stInventoryEntry inventoryEntry;
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

