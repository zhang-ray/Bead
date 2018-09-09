#pragma once
#include "Property.hpp"


namespace Bead{

/// TODO:
///  drawBitmap
class Renderer {
public:
    virtual ~Renderer() { }

    virtual void drawLine(const Vector2d &p0, const Vector2d &p1, const Color &color, const double lineWidth = 1) = 0;
    virtual void drawCircle(const Vector2d &p0, const Color &color, const double radius = 3, const double strokeSize = 1) = 0;
    virtual void drawRectangle(const Rectangle &rect, const Color &color, const double strokeSize = 1 /*, StrokeStyle */) = 0;
    virtual bool drawText(const char *text, const Rectangle &postion, const Color &color, bool setCenter = false, const size_t fontSize = 20) = 0;

    virtual void fillCircle(const Vector2d &p0, const Color &color, const double radius = 2) = 0;
    virtual void fillRectangle(const Rectangle &rect, const Color &color) = 0;

    virtual bool startRendering() = 0;
    virtual void clear(const Color& color) = 0;
    virtual void move(const Vector2d &absOffset) = 0;
    virtual void stopRendering() = 0;
};

}
