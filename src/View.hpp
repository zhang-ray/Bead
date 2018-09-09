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

protected:
    Layout *layout_ = nullptr;
    Layout *parentLayout_ = nullptr;
    Vector2d offset_, relativeOffset_, size_;
    View *root_ = nullptr;
    PropertyDatabase propertyDB_;
    bool visible_ = false;
    bool subviewHandleMouseEvent_ = true;
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
    void renderRecursively(Renderer *pCanvas);
    void handleTouchRecursively(const Vector2d &touchPos, const TouchDownUp &upDown);

    virtual void onRenderBeforeSubview(Renderer *pCanvas){}
    virtual void onRenderAfterSubview(Renderer *pCanvas){}
    virtual void onRenderBackground(Renderer *pCanvas);
    virtual void onTouchUp(const Vector2d &touchPos) {}
    virtual void onTouchDown(const Vector2d &touchPos) {}
    /************************************************************************/
};


class TextView final : public View {
public:
    TextView();
    virtual ~TextView() { }
    virtual void setAction(std::function<void()> action);
    void setEnable(bool enable);

protected:
    bool disabled_ = false;
    Color grphicColor_;
    std::function<void()> clickedAction_;

public:
    virtual void onRenderAfterSubview(Renderer *pCanvas) override;
    virtual void onTouchUp(const Vector2d &touchPos) override;
    virtual void onTouchDown(const Vector2d &touchPos) override{
        if (disabled_) return;
        propertyDB_.approachToUsage("TouchDown");
    }
};



}
