/**
 * Manages the list of levels
 */
#ifndef __LEVEL_LIST_H__
#define __LEVEL_LIST_H__

#include <base/error.h>
#include <stdint.h>

/** Retrieve the index of the main level */
uint32_t levels_getMainIndex();

/** Retrieve the number of availables levels */
uint32_t levels_getNum();

/** Retrieve the path to a level's files */
err levels_getFiles(const char **ppTilemap, const char **ppObjects
        , uint32_t index);

/** Retrieve a level's index from its name */
err levels_getIndex(uint32_t *pIndex, char *pName);

#endif /* __LEVEL_LIST_H__ */

