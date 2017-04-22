/**
 * Manages the list of levels
 */
#include <base/error.h>
#include <ld38/level_list.h>
#include <stdint.h>
#include <string.h>

/* NOTE: Each entry occupies 2 indexes: the tilemap and its objects */
static const char *level_list[] = {
    #include <auto/level_list.h>
};

/** Retrieve the number of availables levels */
uint32_t levels_getNum() {
    return sizeof(level_list) / sizeof(const char *) / 2;
}

/** Retrieve the path to a level's files */
err levels_getFiles(const char **ppTilemap, const char **ppObjects
        , uint32_t index) {
    ASSERT(index < levels_getNum(), ERR_INVALID_INDEX);
    ASSERT(ppTilemap, ERR_ARGUMENTBAD);
    ASSERT(ppObjects, ERR_ARGUMENTBAD);

    *ppTilemap = level_list[index * 2];
    *ppObjects = level_list[index * 2 + 1];

    return ERR_OK;
}

/** Retrieve a level's index from its name */
err levels_getIndex(uint32_t *pIndex, char *pName) {
    uint32_t i;

    for (i = 0; i < levels_getNum(); i++) {
        /* Compare the entire strings, '\0' included */
        if (memcmp(level_list[i * 2], pName, strlen(level_list[i * 2]) + 1)
                == 0) {
            *pIndex = i;
            return ERR_OK;
        }
    }

    return ERR_ITEM_NOT_FOUND;
}

