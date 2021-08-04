#include <SDL2/SDL.h>
#include <time.h>

const SDL_Rect SCREEN = {0, 0, 800, 480};

int main(int argc, char const *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  SDL_Window *window;
  SDL_Renderer *renderer;

  if (SDL_CreateWindowAndRenderer(
          SCREEN.w, SCREEN.h,
          SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC, &window,
          &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                 "Couldn't create window and renderer: %s", SDL_GetError());
    return 2;
  }

  int run = 1;

  while (run) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        run = 0;
        break;
      }
    }
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
  return 0;
}
