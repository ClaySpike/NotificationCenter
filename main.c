#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

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
  TTF_Init();

  TTF_Font *kosugi = TTF_OpenFont("KosugiMaru-Regular.ttf", 32);

  SDL_Color color = {255, 255, 255};

  SDL_Surface *weekdays_s[] = {
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"日", color),
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"月", color),
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"火", color),
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"水", color),
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"木", color),
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"金", color),
      TTF_RenderUNICODE_Blended(kosugi, (const unsigned short *)L"土", color)};
  SDL_Rect rect = {0, 0, weekdays_s[0]->w, weekdays_s[0]->h};
  SDL_Texture *weekdays_t[7];
  for (size_t i = 0; i < 7; i++) {
    weekdays_t[i] = SDL_CreateTextureFromSurface(renderer, weekdays_s[i]);
    free(weekdays_s[i]);
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

    time_t cur_time = time(NULL);
    struct tm *cur_loc_time = localtime(&cur_time);

    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, weekdays_t[cur_loc_time->tm_wday], NULL, &rect);

    SDL_RenderPresent(renderer);

    rect.x = 0;
  }

  for (size_t i = 0; i < 7; i++) {
    SDL_DestroyTexture(weekdays_t[i]);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(kosugi);

  SDL_Quit();
  return 0;
}
