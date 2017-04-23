/**
 * Handles the inventory. It's only rendered when the game is on the inventory
 * state.
 */
#ifndef __INVENTORY_H__
#define __INVENTORY_H__

#include <base/error.h>

/** == LIST OF AVAILABLE STUFF AS X-MACROS ================================== */

#define X_INFORMATIONS \
    X(INFO_A, "info_a")

#define X_PEOPLE \
    X(PERSON_A, "person_a")

#define X_ARTIFACTS \
    X(ARTIFACT_A, "artifact_a")

/** == ENUMERATION OF AVAILABLE STUFF ======================================= */

#define X(en, ...) en,

enum enInformation {
    X_INFORMATIONS
};
typedef enum enInformation information;

enum enPerson {
    X_PEOPLE
};
typedef enum enPerson person;

enum enArtificat {
    X_ARTIFACTS
};
typedef enum enArtifact artifact;

#undef X

/** == FUNCTIONS DECLARATION ================================================ */

/** Alloc all memory used by the inventory */
err inventory_init();

/** Free all memory used by the inventory */
void inventory_clean();

/** Reset the inventory to its original state */
void inventory_reset();

/** Add an information to the inventory */
void inventory_addInformation(information info);

/** Add a person to the inventory */
void inventory_addPerson(person p);

/** Add an artifact to the inventory */
void inventory_addArtifact(artifact a);

/** Update/control the inventory state */
err inventoryState_update();

/** Draw the inventory state */
err inventoryState_draw();

#endif /* __INVENTORY_H__ */

