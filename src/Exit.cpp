#ifdef PLATFORM_WEB

#include <emscripten/emscripten.h>

#elif PLATFORM_DESKTOP

#include <windows.h>

#endif

void exitGame(int code) {
#ifdef PLATFORM_WEB
  emscripten_cancel_main_loop();
  emscripten_force_exit(code);
#else
  exit(code);
#endif
}
