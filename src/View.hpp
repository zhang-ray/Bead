#pragma once

#include "Event.hpp"
#include "Renderer.hpp"

#include <functional>
#include "Property.hpp"
#include <string>
#include <thread>
#include "Xml.hpp"
#include <unordered_map>


namespace Bead {

class Renderer;
class Layout;

class View {
public:
    friend class Layout;
    friend class UiGenerator;
    friend class Simulator;

protected:
    Layout *layout_ = nullptr;
    Layout *parentLayout_ = nullptr;
    Vector2d offset_, relativeOffset_, size_;
    View *root_ = nullptr;
    PropertyDatabase propertyDB_;
    bool visible_ = false;
    bool subviewHandleTouchEvent_ = true;
    static size_t s_maxId;
    size_t globalID_ = ++s_maxId;
public:
    virtual ~View(){}
    View();
    View *findViewById(const std::string &queryId);
    View *findViewById(size_t queryId);
    Vector2d getSize();

    bool setStaticProperty(const std::string &key, const std::string &value);
    bool setProperty(const std::string &usage, const std::string &key, const std::string &value);
    void setLayout(Layout *newLayout);
    void resetAsRoot();

    void composeRecursively(const Vector2d &oldOffset);
    void renderRecursively(Renderer *pRenderer);
    void handleTouchRecursively(const Vector2d &touchPos, const TouchDownUp &upDown);

    virtual void onRenderBeforeSubview(Renderer *pRenderer){}
    virtual void onRenderAfterSubview(Renderer *pRenderer){}
    virtual void onRenderBackground(Renderer *pRenderer);
    virtual void onTouchUp(const Vector2d &touchPos) {}
    virtual void onTouchDown(const Vector2d &touchPos) {}

};


class TextView final : public View {
public:
    TextView();
    virtual ~TextView() { }
    virtual void setAction(std::function<void()> action);
    void setText(const std::string &newText);
    void appendText(const std::string &newText);
    void setEnable(bool enable);

protected:
    bool disabled_ = false;
    Color grphicColor_;
    std::function<void()> clickedAction_ = nullptr;

public:
    virtual void onRenderAfterSubview(Renderer *pRenderer) override;
    virtual void onTouchUp(const Vector2d &touchPos) override;
    virtual void onTouchDown(const Vector2d &touchPos) override{
        if (disabled_) return;
        propertyDB_.approachToUsage("TouchDown");
    }
};



}
