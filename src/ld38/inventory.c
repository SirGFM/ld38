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
#include <ld38/ui.h>
#include <stdint.h>
#include <string.h>

/** Store the requirements within the program */
struct stLinkReq {
    fact *factList;
    person *peopleList;
    artifact *artifactList;
};
/** Sorry, I'm not sorry... macros are a fun source of headaches =D */
#define X(en, name, facts, people, artifacts) \
  static fact __ ## en ## __facts[] = facts; \
  static person __ ## en ## __people[] = people; \
  static artifact __ ## en ## __artifacts[] = artifacts;
    X_LINKS
#undef X
static fact __LINK_ZERO__facts[] = { FACT_ZERO };
static person __LINK_ZERO__people[] = { PERSON_ZERO };
static artifact __LINK_ZERO__artifacts[] = { ARTIFACT_ZERO };
static struct stLinkReq _linkRequirements[NUM_LINKS] = {
    [LINK_ZERO] = { __LINK_ZERO__facts, __LINK_ZERO__people, __LINK_ZERO__artifacts },
#define X(en, name, ...) \
  [en] = { .factList = __ ## en ## __facts, \
      .peopleList = __ ## en ## __people, \
      .artifactList = __ ## en ## __artifacts},
    X_LINKS
#undef X
};

struct stInventoryState {
    /* How many links were made */
    uint32_t linksMade;
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

/** Check if a list has all the listed entries */
#define HASFUNC(fname, list) \
    static uint32_t fname(uint32_t entry) { \
        struct stLinkReq *req; \
        uint32_t i; \
        req = _linkRequirements + entry; \
        for (i = 0; req->list[i] != 0; i++) { \
            if (!inventory.list[req->list[i]]) { \
                return 0; \
            } \
        } \
        return 1; \
    }
HASFUNC(_hasFacts, factList)
HASFUNC(_hasPeople, peopleList)
HASFUNC(_hasArtifacts, artifactList)
#undef HASFUNC

/** Check if any new link was made */
static void inventory_checkLinks() {
    uint32_t i;

    for (i = 0; i < NUM_LINKS; i++) {
        /** Check if the link was already made */
        if (inventory.linkList[i]) {
            continue;
        }

        if (_hasFacts(i) && _hasPeople(i) && _hasArtifacts(i)) {
            inventory.linkList[i] = 1;
            inventory.linksMade++;
            /* TODO Queue message */
        }
    }
}

/** Add a fact to the inventory */
void inventory_addFact(fact f) {
    /** Check if entry has already been set */
    if (inventory.factList[f]) {
        return;
    }

    inventory.factList[f] = 1;
    inventory_checkLinks();
}

/** Add a person to the inventory */
void inventory_addPerson(person p) {
    /** Check if entry has already been set */
    if (inventory.peopleList[p]) {
        return;
    }

    inventory.peopleList[p] = 1;
    inventory_checkLinks();
}

/** Add an artifact to the inventory */
void inventory_addArtifact(artifact a) {
    /** Check if entry has already been set */
    if (inventory.artifactList[a]) {
        return;
    }

    inventory.artifactList[a] = 1;
    inventory_checkLinks();
}

/** Check how many links were made */
uint32_t invetory_getLinkPercentage() {
    return  (100 * inventory.linksMade) / (NUM_LINKS - 1);
}

#define X(en, name, ...) \
    if (memcmp(pId, name, sizeof(name)) == 0) { \
        return en; \
    }

/** Look-up a fact from its identifier */
fact inventory_getFact(char *pId) {
    X_FACTS

    return FACT_ZERO;
}

/** Look-up a person from its identifier */
person inventory_getPerson(char *pId) {
    X_PEOPLE

    return PERSON_ZERO;
}

/** Look-up an artifact from its identifier */
artifact inventory_getArtifact(char *pId) {
    X_ARTIFACTS

    return ARTIFACT_ZERO;
}

#undef X

/** Check if a given link was made */
uint32_t inventory_checkLink(link index) {
    ASSERT(index < NUM_LINKS, 0);
    return inventory.linkList[index];
}

/** Reset the inventory state */
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

