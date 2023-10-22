#include "sim.h"
#include <SDL.h>
#include <iostream>
#include <unistd.h>
#include <unordered_map>

static SDL_Window *window = NULL;
static int shouldContinue = 1;

static void processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            shouldContinue = 0;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                shouldContinue = 0;
                break;
            default:
                break;
            }
        default:
            break;
        }
    }
}

extern "C" void simBegin() {
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Main window",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              1280,
                              720,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    simFlush();
}

extern "C" void simEnd() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

extern "C" int simShouldContinue() { return shouldContinue; }

SDL_Surface *windowSurface = NULL;
int w = 0;
int h = 0;
int offsetX = 0;
int offsetY = 0;
int cellSize = 0;

extern "C" void simSetPixel(int x, int y, int rgb) {
    int r = (rgb >> 16) & 255;
    int g = (rgb >> 8) & 255;
    int b = (rgb >> 0) & 255;
    Uint32 color = SDL_MapRGB(windowSurface->format, r, g, b);
    SDL_Rect rect = {};
    rect.x = cellSize * x + offsetX;
    rect.y = cellSize * y + offsetY;
    rect.w = cellSize;
    rect.h = cellSize;
    SDL_FillRect(windowSurface, &rect, color);
}

extern "C" void simFlush() {
    SDL_UpdateWindowSurface(window);
    SDL_Delay(50);
    processEvents();

    windowSurface = SDL_GetWindowSurface(window);
    SDL_GetWindowSize(window, &w, &h);

    int cand1 = w / SIM_X_SIZE;
    int cand2 = h / SIM_Y_SIZE;
    cellSize = cand1 < cand2 ? cand1 : cand2;
    offsetX = (w - SIM_X_SIZE * cellSize) / 2;
    offsetY = (h - SIM_Y_SIZE * cellSize) / 2;

    SDL_Rect rect = {};
    rect.x = 0;
    rect.y = 0;
    rect.w = w;
    rect.h = h;
    Uint32 gray = SDL_MapRGB(windowSurface->format, 192, 192, 192);
    SDL_FillRect(windowSurface, &rect, gray);
}

extern "C" void logUse(char *user, char *usee) {
    // Gather statistics and output them
    // at the end of the program, when the code will be unloaded.
    // It's not the main code, so we can use C++ RAII here.
    struct Statistics {
        std::unordered_map<const char *,
                           std::unordered_map<const char *, long long>>
            usageNum;

        ~Statistics() {
            for (auto &[user, usees] : usageNum) {
                for (auto &[usee, num] : usees) {
                    std::cout << user << " <- " << usee << ": " << num << "\n";
                }
            }
        }

        void account(const char *user, const char *usee) {
            usageNum.try_emplace(user);
            usageNum[user].try_emplace(usee, 0);
            usageNum[user][usee]++;
        }
    };

    static Statistics stats;
    stats.account(user, usee);
    // std::cout << user << " <- " << usee << '\n';
}
