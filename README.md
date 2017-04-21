# GFM's Ludum Dare #38 Entry

My entry for Ludum Dare #38.

The template was based on my
[Meeting Jam 2017 entry](https://github.com/SirGFM/meeting17/tree/84a48b7bd595eb307d3e0f462b49c72182c14d67)
and my current main project, [JJAT+](https://github.com/SirGFM/jjat_2/tree/v0.5.1).

## Compile instructions

This game has the following dependencies:

* [SDL 2](https://www.libsdl.org/)
* [GFraMe - v2.4.1](https://github.com/SirGFM/GFraMe/tree/v2.4.1)
* [c_synth - v1.0.2.c](https://github.com/SirGFM/c_synth/tree/v1.0.2.c)

After building/installing the dependencies, run:

```
make RELEASE=yes
```

Compiling on Windows is still terrible documented... I'll fix that, eventually...
Cross-compiling for Windows, on the other hand, is somewhat easy when using
[MXE](https://github.com/mxe/mxe).

