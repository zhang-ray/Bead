#pragma once

#include <SDL2/SDL.h>
#include "Renderer.hpp"

namespace Bead {



class SdlRenderer final: public Renderer {
public:
    SdlRenderer(const Vector2d &size);
    ~SdlRenderer();

    // Renderer interface
public:
    void drawLine(const Vector2d &p0, const Vector2d &p1, const Color &color, const double lineWidth){}
    void drawCircle(const Vector2d &p0, const Color &color, const double radius, const double strokeSize){}
    void drawRectangle(const Rectangle &rect, const Color &color, const double strokeSize){}
    bool drawText(const char *text, const Rectangle &postion, const Color &color, bool setCenter, const size_t fontSize){}
    void fillCircle(const Vector2d &p0, const Color &color, const double radius){}
    void fillRectangle(const Rectangle &rect, const Color &color);
    bool startRendering(){}
    void clear(const Color &color);
    void move(const Vector2d &absOffset){
        offset_ = absOffset;
    }
    void stopRendering();


private:
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    Vector2d offset_;


};












}
