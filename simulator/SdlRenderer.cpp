#include "SdlRenderer.hpp"


namespace Bead {



SdlRenderer::SdlRenderer(const Vector2d &size){
    if (0==SDL_Init(SDL_INIT_VIDEO)){
        if (SDL_CreateWindowAndRenderer(size[0],size[1], 0, &window, &renderer) == 0) {

            return;
        }
    }

    throw; // failed
}

SdlRenderer::~SdlRenderer(){
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}


void SdlRenderer::fillRectangle(const Rectangle &rect, const Color &color){
    SDL_SetRenderDrawColor(renderer,
                           (color.rgb_>>16)&0xff,
                           (color.rgb_>>8)&0xff,
                           color.rgb_&0xff,
                           color.alpha_*0xff);
    SDL_Rect sdlRect{
        (int)(rect.left + offset_[0]),
                (int)(rect.top + offset_[1]),
                (int)(rect.right - rect.left),
                (int)(rect.bottom-rect.top)
    };
    SDL_RenderFillRect(renderer, &sdlRect);
}

void SdlRenderer::clear(const Color &color){
    SDL_SetRenderDrawColor(renderer,
                           (color.rgb_>>16)&0xff,
                           (color.rgb_>>8)&0xff,
                           color.rgb_&0xff,
                           color.alpha_*0xff);
    SDL_RenderClear(renderer);
}

void SdlRenderer::stopRendering(){
    SDL_RenderPresent(renderer);
}


}
