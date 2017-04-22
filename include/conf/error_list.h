/**
 * @file include/conf/error_list.h
 *
 * Enumerate all possible errors.
 */
#ifndef __CONF_ERROR_LIST_H__
#define __CONF_ERROR_LIST_H__

/** List of errors. Should be accessed through the 'err' enumeration */
#define ERROR_LIST \
    X(ERR_OK) \
    X(ERR_FORCEEXIT) \
    X(ERR_ARGUMENTBAD) \
    X(ERR_MISSINGPARAM) \
    X(ERR_GFMERR) \
    X(ERR_PARSINGERR) \
    X(ERR_UNHANDLED_COLLISION) \
    X(ERR_INVALID_STATE) \
    X(ERR_ALLOC_FAILED) \
    X(ERR_INVALID_INDEX) \
    X(ERR_ITEM_NOT_FOUND) \
    X(ERR_MAX)

#endif /* __CONF_ERROR_LIST_H__ */

