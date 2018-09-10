#include <thread>
#include <fstream>
#include <vector>
#include <algorithm>
#include "UiGenerator.hpp"
#include "SdlRenderer.hpp"

int main(void){

    using namespace Bead;
    UiGenerator generator;
    View * viewRoot = generator.parseAndCreate("/tmp/app00.xml");
    viewRoot->composeRecursively({0,0});

    {
        SdlRenderer sdlRenderer(viewRoot->getSize());

        {
            bool done = false;

            for (;;){
                sdlRenderer.startRendering();
                sdlRenderer.clear(Color(0x6e828b,0));
                viewRoot->renderRecursively(&sdlRenderer);
                sdlRenderer.stopRendering();
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
    }

    return 0;
}
