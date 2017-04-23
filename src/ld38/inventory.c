/**
 * Handles the inventory. It's only rendered when the game is on the inventory
 * state.
 */
#include <base/error.h>
#include <base/gfx.h>
#include <base/game.h>
#include <base/input.h>
#include <GFraMe/gframe.h>
#include <GFraMe/gfmTilemap.h>
#include <GFraMe/gfmText.h>
#include <ld38/inventory.h>

struct stInventoryState {
    /** List of things found by the player */
    fact factList[NUM_FACTS];
    person peopleList[NUM_PEOPLE];
    artifact artifactList[NUM_ARTIFACTS];
    /** List of links made (the game's score) */
    link linkList[NUM_LINKS];
    /** TODO Stuff used to control the ui */
};
static struct stInventoryState inventory = {0};

/** Alloc all memory used by the inventory */
err inventory_init() {
    return ERR_OK;
}

/** Free all memory used by the inventory */
void inventory_clean() {
}

/** Check if any new link was made */
static void inventory_checkLinks() {
}

/** Add a fact to the inventory */
void inventory_addFact(fact f) {
    inventory_checkLinks();
}

/** Add a person to the inventory */
void inventory_addPerson(person p);

/** Add an artifact to the inventory */
void inventory_addArtifact(artifact a);

/** Reset the inventory state to its initial state */
err inventorystate_reset() {
    return ERR_OK;
}

/** Update/control the inventory state */
err inventorystate_update() {
    return ERR_OK;
}

/** Draw the inventory state */
err inventorystate_draw() {
    return ERR_OK;
}

