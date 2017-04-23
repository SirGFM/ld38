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
    X(MY_HOUSE, "my_house") \
    X(WOODCARVER_HOUSE, "woodcarver_house") \
    X(UNFINISHED_WOOD, "unfinished_wood")

#define X_PEOPLE \
    X(PAINTER, "painter")

#define X_ARTIFACTS \
    X(DOG_STATUE, "dog") \
    X(BIRD_STATUE, "bird")

#define LINK_ARR(...) __VA_ARGS__
#define X_LINKS \
    X(LINK_A, "link_a", LINK_ARR({WOODCARVER_HOUSE, UNFINISHED_WOOD, 0}), LINK_ARR({PAINTER, 0}), LINK_ARR({DOG_STATUE, BIRD_STATUE, 0}))

/** == ENUMERATION OF AVAILABLE STUFF ======================================= */

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

/** Look-up a fact from its identifier */
fact inventory_getFact(char *pId);

/** Look-up a person from its identifier */
person inventory_getPerson(char *pId);

/** Look-up an artifact from its identifier */
artifact inventory_getArtifact(char *pId);

/** Check if a given link was made */
uint32_t inventory_checkLink(link index);

/** Reset the inventory state */
err inventorystate_reset();

/** Update/control the inventory state */
err inventorystate_update();

/** Draw the inventory state */
err inventorystate_draw();

#endif /* __INVENTORY_H__ */

