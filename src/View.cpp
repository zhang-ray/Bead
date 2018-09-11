#include "View.hpp"
#include "Layout.hpp"
#include "Renderer.hpp"

namespace Bead {

void View::composeRecursively(const Vector2d &oldOffset) {
    offset_ = oldOffset + relativeOffset_;

    if (layout_) {
        if (!propertyDB_.getProperty("inner_margin")) {
            propertyDB_.setConstProperty("inner_margin", "{0,0,0,0}");
        }

        if (auto pInnerMargin = (Rectangle*)(propertyDB_.getProperty("inner_margin").get())) {
            auto innerMargin = *pInnerMargin;
            Vector2d contentSize;
            layout_->arrange({
                                 size_[0] - innerMargin.right - innerMargin.left,
                                 size_[1] - innerMargin.bottom - innerMargin.top
                             }, contentSize);

            for (auto &c : layout_->subviews_) {
                c.first->composeRecursively(offset_ + Vector2d{
                                                innerMargin.left,
                                                innerMargin.top });
            }
        }
    }
}

void View::renderRecursively(Renderer *pRenderer) {
    if (visible_) return;

    pRenderer->move(offset_);
    onRenderBackground(pRenderer);
    onRenderBeforeSubview(pRenderer);

    if (layout_) {
        for (auto c : layout_->subviews_) {
            c.first->renderRecursively(pRenderer);
        }
    }

    pRenderer->move(offset_);
    onRenderAfterSubview(pRenderer);
}


void View::handleTouchRecursively(const Vector2d &touchPos, const TouchDownUp &upDown){
    if (visible_) return;

    if (!visible_ ) {
        if ((touchPos[0] >= offset_[0]) &&
                (touchPos[1] >= offset_[1]) &&
                (touchPos[0] < offset_[0] + size_[0]) &&
                (touchPos[1] < offset_[1] + size_[1])) {
            if (upDown==TouchDownUp::DOWN){
                onTouchDown(touchPos);
            }
            else if (upDown==TouchDownUp::UP){
                onTouchUp(touchPos);
            }
        }
    }

    if (subviewHandleTouchEvent_){
        if (layout_) {
            for (auto c : layout_->subviews_) {
                c.first->handleTouchRecursively(touchPos, upDown);
            }
        }
    }
}


void View::resetAsRoot(){
    std::function<void(View *realRoot, Layout *layout)> ___
            = [&___](View *realRoot, Layout *layout) {
        if (layout) {
            for (auto c : layout->subviews_) {
                c.first->root_ = realRoot;
                ___(realRoot, c.first->layout_);
            }
        }
    };
    ___(this, layout_);


    size_ = getSize();
}

void View::onRenderBackground(Renderer *pRenderer) {
    if (auto pcolor = (Color*)propertyDB_.getProperty("background_color").get()) {
        pRenderer->fillRectangle(size_, *pcolor);
    }
}

Vector2d View::getSize(){
    auto p2d = (Vector2d *) propertyDB_.getProperty("size").get();
    return *p2d;
}




size_t View::s_maxId = 10000;


View::View(){
    propertyDB_.registerProperty({"id", Text::parse });
    propertyDB_.registerProperty({"background_color", Color::parse });
    propertyDB_.registerProperty({"inner_margin", Rectangle::parse });
    propertyDB_.registerProperty({"border_color", Color::parse });
    propertyDB_.registerProperty({"border_width", Scalar::parse });
    propertyDB_.registerProperty({"size", Vector2d::parse});
}

View * View::findViewById(const std::string &queryId){
    if (auto pId = (Text*)propertyDB_.getProperty("id").get()) {
        if (pId->text == queryId) {
            return this;
        }
    }
    if (layout_) {
        return layout_->findViewById(queryId);
    }
    return nullptr;
}

View * View::findViewById(size_t queryId){
    if (queryId == globalID_) return this;
    if (layout_) {
        // recursively
        for (auto c : layout_->subviews_) {
            auto ret = c.first->findViewById(queryId);
            if (ret) return ret;
        }
    }
    return nullptr;
}

bool View::setStaticProperty(const std::string &key, const std::string &value){
    return propertyDB_.setConstProperty(key, value);
}

bool View::setProperty(const std::string &usage, const std::string &key, const std::string &value){
    return propertyDB_.setProperty(usage, key, value);
}

void View::setLayout(Layout *newLayout){
    if (layout_) {
        delete layout_;
    }
    layout_ = newLayout;
    layout_->parent_ = this;
}



TextView::TextView(){
    propertyDB_.registerUsages({"Disable", "Enable", "TouchDown"});

    propertyDB_.registerProperty({ "font_color", Color::parse });
    propertyDB_.registerProperty({ "font_size", Scalar::parse });
    propertyDB_.registerProperty({ "border_color", Color::parse });
    propertyDB_.registerProperty({ "border_width", Scalar::parse });
    propertyDB_.registerProperty({ "inner_margin", Rectangle::parse });
    propertyDB_.registerProperty({ "text", Text::parse });
}

void TextView::setAction(std::function<void()> action){
    if (action) {
        clickedAction_ = action;
    }
}

void TextView::setText(const std::string &newText){
    propertyDB_.setConstProperty("text", newText);
}

void TextView::appendText(const std::string &newText){
    if (auto pText = (Text*)propertyDB_.getProperty("text").get()) {
        propertyDB_.setConstProperty("text", pText->text+ newText);
    }
    else {
        setText(newText);
    }
}

void TextView::setEnable(bool enable) {
    disabled_ = !enable;
    std::string usage;
    if (enable) {
        usage = "Enable";
    }
    else {
        usage = "Disable";
    }
    propertyDB_.approachToUsage(usage);
}



void TextView::onTouchUp(const Vector2d &touchPos){
    if (disabled_) return;
    if (clickedAction_) clickedAction_();
}


void TextView::onRenderAfterSubview(Renderer *pRenderer) {
    if (auto pText = (Text*)propertyDB_.getProperty("text").get()) {
        if (auto pFontColor = (Color*)propertyDB_.getProperty("font_color").get()) {
            if (auto pFontSize = (Scalar*)propertyDB_.getProperty("font_size").get()) {
                pRenderer->drawText(pText->get()
                                  , Rectangle(0, (double)size_[1], 0, (double)size_[0])
                        , *pFontColor
                        , false
                        , (size_t)pFontSize->get());
            }
        }
    }
}


}
