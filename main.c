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
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'日', color),
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'月', color),
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'火', color),
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'水', color),
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'木', color),
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'金', color),
      TTF_RenderGlyph_Blended(kosugi, (Uint16)L'土', color)};
  SDL_Rect rect = {0, 0, weekdays_s[0]->w, weekdays_s[0]->h};
  SDL_Texture *weekdays_t[7];
  for (size_t i = 0; i < 7; i++) {
    weekdays_t[i] = SDL_CreateTextureFromSurface(renderer, weekdays_s[i]);
    free(weekdays_s[i]);
  }

  // Create Cache for ASCII characters
  SDL_Texture *ascii[127 - 33];
  for (size_t i = 33; i < 127; i++) {
    SDL_Surface *temp = TTF_RenderGlyph_Blended(kosugi, i, color);
    ascii[i - 33] = SDL_CreateTextureFromSurface(renderer, temp);
    free(temp);
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

    // Print the day of the week to the screen
    SDL_RenderCopy(renderer, weekdays_t[cur_loc_time->tm_wday], NULL, &rect);

    // Print the current time to the screen
    SDL_Rect c = {
        SCREEN.w / 2,
        0,
        0,
        0,
    };
    SDL_QueryTexture(ascii[':' - 33], NULL, NULL, &c.w, &c.h);
    c.x -= c.w / 2;

    SDL_RenderCopy(renderer, ascii[':' - 33], NULL, &c);
    SDL_Point hour_right = {c.x, c.y};
    SDL_Point minute_left = {c.x + c.w, c.y};

    int pm = cur_loc_time->tm_hour > 11;
    if (pm) {
      cur_loc_time->tm_hour -= 12;
    }

    // Print hour to the screen
    if (cur_loc_time->tm_hour > 9) {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_hour % 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_hour % 10 - 33],
                     NULL, &c);

      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_hour / 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_hour / 10 - 33],
                     NULL, &c);
    } else {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_hour - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_hour - 33], NULL,
                     &c);
    }

    // Print minute to the screen
    c.x = minute_left.x;
    SDL_QueryTexture(ascii['0' + cur_loc_time->tm_min / 10 - 33], NULL, NULL,
                     &c.w, &c.h);
    SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_min / 10 - 33], NULL,
                   &c);
    c.x += c.w;

    SDL_QueryTexture(ascii['0' + cur_loc_time->tm_min % 10 - 33], NULL, NULL,
                     &c.w, &c.h);
    SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_min % 10 - 33], NULL,
                   &c);
    c.x += c.w;

    // Printing whether am or pm
    if (pm) {
      SDL_QueryTexture(ascii['P' - 33], NULL, NULL, &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['P' - 33], NULL, &c);
      c.x += c.w;
      SDL_QueryTexture(ascii['M' - 33], NULL, NULL, &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['M' - 33], NULL, &c);
      c.x += c.w;
    } else {
      SDL_QueryTexture(ascii['A' - 33], NULL, NULL, &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['A' - 33], NULL, &c);
      c.x += c.w;
      SDL_QueryTexture(ascii['M' - 33], NULL, NULL, &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['M' - 33], NULL, &c);
      c.x += c.w;
    }

    // Printing the Date
    c.x = SCREEN.w;
    if (cur_loc_time->tm_mday > 9) {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mday % 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mday % 10 - 33],
                     NULL, &c);
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mday / 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mday / 10 - 33],
                     NULL, &c);
    } else {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mday - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mday - 33], NULL,
                     &c);
    }

    SDL_QueryTexture(ascii['/' - 33], NULL, NULL, &c.w, &c.h);
    c.x -= c.w;
    SDL_RenderCopy(renderer, ascii['/' - 33], NULL, &c);

    if (cur_loc_time->tm_mday > 9) {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mon % 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mon % 10 - 33],
                     NULL, &c);
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mon / 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mon / 10 - 33],
                     NULL, &c);
    } else {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mon - 33], NULL, NULL, &c.w,
                       &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mon - 33], NULL,
                     &c);
    }

    SDL_RenderPresent(renderer);

    rect.x = 0;
  }

  for (size_t i = 0; i < 7; i++) {
    SDL_DestroyTexture(weekdays_t[i]);
  }
  for (size_t i = 0; i < 127 - 33; i++) {
    SDL_DestroyTexture(ascii[i]);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_CloseFont(kosugi);

  SDL_Quit();
  return 0;
}
