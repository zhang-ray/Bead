#include <thread>
#include <fstream>
#include <vector>
#include <algorithm>
#include "UiGenerator.hpp"
#include "SdlRenderer.hpp"

int main(void){

    using namespace Bead;
    UiGenerator generator;
    View * viewRoot = generator.parseAndCreate("/tmp/calculator.xml");
    if (!viewRoot){
        return -1;
    }
    viewRoot->composeRecursively({0,0});

    {
        auto lcd = (TextView *)(viewRoot->findViewById("lcd"));
        // add action
        for (int i = 0; i < 10; i++){
            auto button = (TextView *)(viewRoot->findViewById(std::to_string(i)));
            button->setAction([=](){
                lcd->appendText(std::to_string(i));
            });
        }
    }

    SdlRenderer sdlRenderer(viewRoot->getSize());

    bool done = false;

    for (;;){
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            }
            else if (event.type == SDL_MOUSEBUTTONUP){
                viewRoot->handleTouchRecursively(
                {(double)(event.button.x), (double)(event.button.y)}
                            , TouchDownUp::UP
                            );
            }if (event.type == SDL_MOUSEBUTTONDOWN){
                viewRoot->handleTouchRecursively(
                {(double)(event.button.x), (double)(event.button.y)}
                            , TouchDownUp::DOWN
                            );
            }
        }
        if (done){
            break;
        }

        sdlRenderer.startRendering();
        viewRoot->renderRecursively(&sdlRenderer);
        sdlRenderer.stopRendering();
    }

    return 0;
}
