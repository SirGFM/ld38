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
    /* Collision group 'floor_collision' */ 
    CASE(T_FLOOR, T_PLAYER)
    CASE(T_WALL, T_PLAYER)
        if (node1.pChild != node2.pChild) {
            /* Filter out self collision */
            erv = _defaultFloorCollision(&node1, &node2);
        }
    break;
    CASE(T_PLAYER, T_FLOOR)
    CASE(T_PLAYER, T_WALL)
        if (node1.pChild != node2.pChild) {
            /* Filter out self collision */
            erv = _defaultFloorCollision(&node2, &node1);
        }
    break;
#  if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
    SELFCASE(T_FLOOR)
    IGNORE(T_FLOOR, T_WALL)
    IGNORE(T_FLOOR, T_INTERACTABLE)
    IGNORE(T_FLOOR, T_DOOR)
    IGNORE(T_WALL, T_FLOOR)
    SELFCASE(T_WALL)
    IGNORE(T_WALL, T_INTERACTABLE)
    IGNORE(T_WALL, T_DOOR)
        erv = ERR_OK;
    break;
#  endif /* defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__)) */
    /* Collision group 'interactable' */ 
    CASE(T_INTERACTABLE, T_PLAYER)
    CASE(T_DOOR, T_PLAYER)
    CASE(T_FACT, T_PLAYER)
    CASE(T_ARTIFACT, T_PLAYER)
    CASE(T_PERSON, T_PLAYER)
        if (node1.pChild != node2.pChild) {
            /* Filter out self collision */
            erv = _onInteract(&node1, &node2);
        }
    break;
    CASE(T_PLAYER, T_INTERACTABLE)
    CASE(T_PLAYER, T_DOOR)
    CASE(T_PLAYER, T_FACT)
    CASE(T_PLAYER, T_ARTIFACT)
    CASE(T_PLAYER, T_PERSON)
        if (node1.pChild != node2.pChild) {
            /* Filter out self collision */
            erv = _onInteract(&node2, &node1);
        }
    break;
#  if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
    IGNORE(T_INTERACTABLE, T_FLOOR)
    IGNORE(T_INTERACTABLE, T_WALL)
    SELFCASE(T_INTERACTABLE)
    IGNORE(T_INTERACTABLE, T_DOOR)
    IGNORE(T_INTERACTABLE, T_FACT)
    IGNORE(T_INTERACTABLE, T_ARTIFACT)
    IGNORE(T_INTERACTABLE, T_PERSON)
    IGNORE(T_DOOR, T_FLOOR)
    IGNORE(T_DOOR, T_WALL)
    IGNORE(T_DOOR, T_INTERACTABLE)
    SELFCASE(T_DOOR)
    IGNORE(T_DOOR, T_FACT)
    IGNORE(T_DOOR, T_ARTIFACT)
    IGNORE(T_DOOR, T_PERSON)
    IGNORE(T_FACT, T_FLOOR)
    IGNORE(T_FACT, T_WALL)
    IGNORE(T_FACT, T_INTERACTABLE)
    IGNORE(T_FACT, T_DOOR)
    SELFCASE(T_FACT)
    IGNORE(T_FACT, T_ARTIFACT)
    IGNORE(T_FACT, T_PERSON)
    IGNORE(T_ARTIFACT, T_FLOOR)
    IGNORE(T_ARTIFACT, T_WALL)
    IGNORE(T_ARTIFACT, T_INTERACTABLE)
    IGNORE(T_ARTIFACT, T_DOOR)
    IGNORE(T_ARTIFACT, T_FACT)
    SELFCASE(T_ARTIFACT)
    IGNORE(T_ARTIFACT, T_PERSON)
    IGNORE(T_PERSON, T_FLOOR)
    IGNORE(T_PERSON, T_WALL)
    IGNORE(T_PERSON, T_INTERACTABLE)
    IGNORE(T_PERSON, T_DOOR)
    IGNORE(T_PERSON, T_FACT)
    IGNORE(T_PERSON, T_ARTIFACT)
    SELFCASE(T_PERSON)
        erv = ERR_OK;
    break;
#  endif /* defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__)) */
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
