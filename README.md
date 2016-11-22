
## SDL2 OpenGL Basic Template

Simple and useful (supposedly) cross-platform template for OpenGL. Uses SDL_GL_GetProcAddress() to create the OpenGL functions so that you do not need to link or include the OS specific opengl library. 



```C++
// Use this!
#include <opengl.h>
// instead of
#include <gl.h>
// or
#include <SDL_opengl.h>
```

..and you don't need to link it in the Makefile..

```Makefile
LDLIBS = -lSDL2 # No gl here
```

..then make sure you use `Init_OpenGL()` after creating the OpenGL context to initialise the functions.

## Credits

The [opengl.h](opengl.h) and [opengl.cpp](opengl.cpp) files are by nlguillemot who provided the [source code](https://github.com/nlguillemot/dof/blob/ad41e71b17446e9c6522570b1bed5571ed9dabdd/viewer/opengl.h) and talks about it in [this article](https://nlguillemot.wordpress.com/2016/11/18/opengl-renderer-design/). The opengl files are also derived from from Khronos' glcorearb.h. 