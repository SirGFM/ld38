/**
 * Handles the inventory. It's only rendered when the game is on the inventory
 * state.
 */
#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include <base/error.h>
#include <stdint.h>

/** == LIST OF AVAILABLE STUFF AS X-MACROS ================================== */

#define X_FACTS \
    X(LOCKED_HOUSE, "locked_house")

#define X_PEOPLE \
    X(WEIRDO, "weirdo")

#define X_ARTIFACTS \
    X(GOSH, "gosh")

#define LINK_ARR(...) __VA_ARGS__
#define X_LINKS \
    X(LINK_A, "link_a", LINK_ARR({LOCKED_HOUSE, 0}), LINK_ARR({WEIRDO, 0}), LINK_ARR({GOSH, 0}))

/** == ENUMERATION OF AVAILABLE STUFF ======================================= */

enum enEntryType {
    IET_FACT
  , IET_PERSON
  , IET_ARTIFACT
};
typedef enum enEntryType entryType;

#define X(en, ...) en,

enum enFact {
    FACT_ZERO = 0,
    X_FACTS
    NUM_FACTS
};
typedef enum enFact fact;

enum enPerson {
    PERSON_ZERO = 0,
    X_PEOPLE
    NUM_PEOPLE
};
typedef enum enPerson person;

enum enArtifact {
    ARTIFACT_ZERO = 0,
    X_ARTIFACTS
    NUM_ARTIFACTS
};
typedef enum enArtifact artifact;

enum enLink {
    LINK_ZERO = 0,
    X_LINKS
    NUM_LINKS
};
typedef enum enLink link;

#undef X

/** == FUNCTIONS DECLARATION ================================================ */

/** Alloc all memory used by the inventory */
err inventory_init();

/** Free all memory used by the inventory */
void inventory_clean();

/** Add a fact to the inventory */
void inventory_addFact(fact f);

/** Add a person to the inventory */
void inventory_addPerson(person p);

/** Add an artifact to the inventory */
void inventory_addArtifact(artifact a);

/** Check how many links were made */
uint32_t invetory_getLinkPercentage();

/** Reset the inventory state */
err inventorystate_reset();

/** Update/control the inventory state */
err inventorystate_update();

/** Draw the inventory state */
err inventorystate_draw();

#endif /* __INVENTORY_H__ */

