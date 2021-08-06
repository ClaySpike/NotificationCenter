#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

int dayofweek(int y, int m, int d);
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
    cur_loc_time->tm_year +=
        1900;  // Might have some problems in the year 2147481747 so deal with
               // that if you are from that year I guess
    cur_loc_time->tm_mon++;

    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &SCREEN);

    // Print the current time to the screen
    SDL_Rect c = {
        49,
        0,
        0,
        0,
    };
    SDL_QueryTexture(ascii[':' - 33], NULL, NULL, &c.w, &c.h);
    c.x -= c.w / 2;

    SDL_RenderCopy(renderer, ascii[':' - 33], NULL, &c);
    SDL_Point minute_left = {c.x + c.w, c.y};

    int pm = cur_loc_time->tm_hour > 11;
    if (pm) {
      cur_loc_time->tm_hour -= 12;
    } else if (cur_loc_time->tm_hour == 0) {
      cur_loc_time->tm_hour = 12;
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

    c.x = 49;
    c.y += c.h;

    // Printing whether am or pm
    if (pm) {
      SDL_QueryTexture(ascii['P' - 33], NULL, NULL, &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['P' - 33], NULL, &c);
      c.x += c.w;
      SDL_QueryTexture(ascii['M' - 33], NULL, NULL, &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['M' - 33], NULL, &c);
    } else {
      SDL_QueryTexture(ascii['A' - 33], NULL, NULL, &c.w, &c.h);
      c.x -= c.w;
      SDL_RenderCopy(renderer, ascii['A' - 33], NULL, &c);
      c.x += c.w;
      SDL_QueryTexture(ascii['M' - 33], NULL, NULL, &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['M' - 33], NULL, &c);
    }

    c.y += c.h;

    // Printing the Date
    SDL_QueryTexture(ascii['/' - 33], NULL, NULL, &c.w, &c.h);
    c.x -= c.w / 2;
    SDL_RenderCopy(renderer, ascii['/' - 33], NULL, &c);
    SDL_Point day_left = {c.x + c.w, c.y};

    if (cur_loc_time->tm_mon > 9) {
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

    c.x = day_left.x;
    c.y = day_left.y;
    if (cur_loc_time->tm_mday > 9) {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mday % 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mday % 10 - 33],
                     NULL, &c);
      c.x += c.w;
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mday / 10 - 33], NULL, NULL,
                       &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mday / 10 - 33],
                     NULL, &c);
    } else {
      SDL_QueryTexture(ascii['0' + cur_loc_time->tm_mday - 33], NULL, NULL,
                       &c.w, &c.h);
      SDL_RenderCopy(renderer, ascii['0' + cur_loc_time->tm_mday - 33], NULL,
                     &c);
    }

    SDL_QueryTexture(ascii['/' - 33], NULL, NULL, &c.w, &c.h);
    c.x -= c.w;
    SDL_RenderCopy(renderer, ascii['/' - 33], NULL, &c);

    // Draw the Calendar on the screen
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect cal = {98, 0, 462, 480};
    SDL_RenderDrawRect(renderer, &cal);

    int num_days_in_month;
    if (cur_loc_time->tm_mon == 4 || cur_loc_time->tm_mon == 6 ||
        cur_loc_time->tm_mon == 9 || cur_loc_time->tm_mon == 11) {
      num_days_in_month = 30;
    } else if (cur_loc_time->tm_mon == 2) {
      if (cur_loc_time->tm_year % 4 == 0 &&
          (cur_loc_time->tm_year % 100 == 0 && cur_loc_time->tm_year % 400)) {
        num_days_in_month = 29;
      }
      num_days_in_month = 28;
    } else {
      num_days_in_month = 31;
    }
    int first_wday = dayofweek(cur_loc_time->tm_year, cur_loc_time->tm_mon, 1);
    int num_week_rows = ceil(((double)num_days_in_month + first_wday) / 7);
    int cal_day_height = (480 - 66) / num_week_rows;
    cal.w = 66;  // 462 / 7
    cal.h = ((480 - 66) % num_week_rows) + 66;
    for (size_t i = 0; i < 7; i++) {
      SDL_RenderDrawRect(renderer, &cal);
      SDL_QueryTexture(weekdays_t[i], NULL, NULL, &c.w, &c.h);
      c.x = cal.x + (cal.w - c.w) / 2;
      c.y = cal.y + (cal.h - c.h) / 2;
      SDL_RenderCopy(renderer, weekdays_t[i], NULL, &c);

      cal.x += cal.w;
    }

    // Draw the days on the calendar
    cal.y += cal.h;
    cal.h = cal_day_height;
    cal.x = 98;
    int day = -6;
    for (size_t i = 0; i < num_week_rows; i++) {
      for (size_t j = 0; j < 7; j++) {
        if (i == 0 && j == first_wday) {
          day = 1;
        }
        if (day >= 1 && day <= num_days_in_month) {
          c.x = cal.x;
          c.y = cal.y;
          if (day > 9) {
            SDL_QueryTexture(ascii['0' + day / 10 - 33], NULL, NULL, &c.w,
                             &c.h);
            SDL_RenderCopy(renderer, ascii['0' + day / 10 - 33], NULL, &c);
            c.x += c.w;
            SDL_QueryTexture(ascii['0' + day % 10 - 33], NULL, NULL, &c.w,
                             &c.h);
            SDL_RenderCopy(renderer, ascii['0' + day % 10 - 33], NULL, &c);
          } else {
            SDL_QueryTexture(ascii['0' + day - 33], NULL, NULL, &c.w, &c.h);
            SDL_RenderCopy(renderer, ascii['0' + day - 33], NULL, &c);
          }
          day++;
        }
        SDL_RenderDrawRect(renderer, &cal);
        cal.x += cal.w;
      }
      cal.x = 98;
      cal.y += cal.h;
    }

    SDL_RenderPresent(renderer);
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

// From
// https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Implementation-dependent_methods
int dayofweek(int y, int m, int d) /* 1 <= m <= 12,  y > 1752 (in the U.K.) */
{
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  y -= m < 3;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}
