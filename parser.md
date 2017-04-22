# Parser Notes

Notes about objects recognized by the parser and stuff.

## Level organization

Every level has a tilemap and an object. The exported file should have the same
base name but the prefix "_tm" and "_obj", respectively.

This causes each level's object layer to be listed before its tilemap layer
(since they are sorted in alphabetical order).

Tilemaps must have two custom properties: "width" and "height", which must point
to the horizontal and vertical coordinates of the last tile within the tilemap.

All layers must have its origin set to (0,0). To make layers easier to modify on
tiled, a offset is set so they are displayed on the correct position. In turn,
this created the necessity of an "in-game offset", which is represented by an
object. **NOTE That this object must be the first one on the object layer!**

## Objects types

### offset

Object used to mark a layer's offset from the origin. Since this object is
placed on the world's origin, the resulting coordinate must be inverted before
being added to anything.

Only its position is used.

