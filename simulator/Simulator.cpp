#include <thread>
#include <fstream>
#include <vector>
#include <algorithm>
#include <SDL2/SDL.h>
#include "UiGenerator.hpp"

int main(void){

    Bead::UiGenerator generator;
    Bead::View * viewRoot = generator.parseAndCreate("/tmp/appUI.xml");
    auto size = viewRoot->getSize();
    viewRoot->composeRecursively({0,0});

    {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        auto const width = size[0];
        auto const height = size[1];

        if (SDL_Init(SDL_INIT_VIDEO) == 0){
            if (SDL_CreateWindowAndRenderer(width,height, 0, &window, &renderer) == 0) {


                bool done = false;

                for (;;){
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                    SDL_RenderClear(renderer);
                    SDL_SetRenderDrawColor(renderer, 0x22, 0x77, 0x00, 0x33);

                    SDL_RenderPresent(renderer);
                    SDL_Event event;
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            done = true;
                        }
                    }
                    if (done){
                        break;
                    }
                }
            }

            if (renderer) SDL_DestroyRenderer(renderer);
            if (window) SDL_DestroyWindow(window);
            SDL_Quit();
        }
    }

    return 0;
}
