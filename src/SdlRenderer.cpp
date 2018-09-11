#include "SdlRenderer.hpp"


namespace Bead {



SdlRenderer::SdlRenderer(const Vector2d &size){
    if (0==SDL_Init(SDL_INIT_VIDEO)){
        if (SDL_CreateWindowAndRenderer(size[0],size[1], 0, &window, &renderer) == 0) {


            TTF_Init();

            return;
        }
    }

    throw; // failed
}

SdlRenderer::~SdlRenderer(){


    TTF_Quit();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool SdlRenderer::drawText(const char *text, const Rectangle &postion, const Color &color, bool setCenter, const size_t fontSize){

    TTF_Font * font = TTF_OpenFont("/usr/share/fonts/truetype/ubuntu-font-family/Ubuntu-R.ttf", fontSize);
    const char * error = TTF_GetError();
    SDL_Color sdlColor = {
        (uint8_t)((color.rgb_>>16)&0xff),
        (uint8_t)((color.rgb_>>8)&0xff),
        (uint8_t)(color.rgb_&0xff )};
    SDL_Surface * surface = TTF_RenderText_Solid(font, text, sdlColor);
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer,
     surface);

    int texW = postion.right - postion.left;
    int texH = postion.bottom - postion.top;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = { (int) (postion.left + offset_[0]), (int)(postion.top+ offset_[1]), texW, texH };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    TTF_CloseFont(font);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

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
