/**
 * @file include/conf/type.h
 *
 * Define all in-game types.
 */
#ifndef __CONF_TYPE_H__
#define __CONF_TYPE_H__

#include <GFraMe/gfmTypes.h>

/** Mask that return the proper 16 bit type */
#define T_MASK 0x0000ffff
/** Number of bits per type */
#define T_BITS 16
/**
 * How many bits there are for any given "base type". Different types that share
 * the same base one will be rendered within the quadtree with the same color.
 */
#define T_BASE_NBITS 5

/** Retrieve an object's type (mask out all non-type bits) */
#define TYPE(type) \
    (type & T_MASK)

/* Examples from my current main project */
enum enType {
      T_FLOOR        = gfmType_reserved_5  /* ( 8) purple */
    , T_INTERACTABLE = gfmType_reserved_3  /* ( 6) light blue */

    , T_WALL         = (1 << T_BASE_NBITS) | T_FLOOR
    , T_DOOR         = (1 << T_BASE_NBITS) | T_INTERACTABLE
};
typedef enum enType type;

/** X-Macro used by chunk to create the list of parse-able types */
#define X_TYPE_DICT \
    X("floor", T_FLOOR)

#endif /* __CONF_TYPE_H__ */

