/**
 * @file misc/auto/collisioncases.c
 *
 * File generated from 'misc/collision.json' to simplify handling collisions
 *
 * DO NOT EDIT MANUALLY
 */

/* Avoid error if it's a self collision */erv = ERR_OK;
/* Merge both types into a single one, so it's easier to compare */
switch (MERGE_TYPES(node1.type, node2.type)) {
    /* On Linux, a SIGINT is raised any time a unhandled collision
     * happens. When debugging, GDB will stop here and allow the user to
     * check which types weren't handled */
    default: {
#  if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
        /* Unfiltered collision, do something about it */
        raise(SIGINT);
        erv = ERR_UNHANDLED_COLLISION;
#  endif /* defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__)) */
    }
} /* switch (MERGE_TYPES(node1.type, node2.type)) */
ASSERT(erv == ERR_OK, erv);
