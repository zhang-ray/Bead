#pragma once

#include "View.hpp"
namespace Bead {

class SubviewInfo {
public:
    virtual ~SubviewInfo(){ }
};



class Layout {
public:
    virtual ~Layout() { }

    friend class View;
    friend class UiGenerator;

protected:
    virtual bool arrange(const Vector2d &parentInnerSize, Vector2d &contentSize) = 0;
    virtual bool add(const std::string &description, View *newView) = 0;


    virtual View *findViewById(const std::string &queryId) {
        for (auto c : subviews_) {
            auto ret = c.first->findViewById(queryId);
            if (ret) return ret;
        }
        return nullptr;
    }

    void settle(View *view, SubviewInfo* info){
        view->parentLayout_ = this;
        subviews_.push_back(std::make_pair(view, info));
    }


    void resize(View *view, const Vector2d &newSize) {
        view->size_ = newSize;
    }

    void setWidth(View *view, const double newWidth) {
        view->size_[0] = newWidth;
    }

    void setHeight(View *view, const double newHeight) {
        view->size_[1] = newHeight;
    }

    Vector2d getRelativePosition(View *view) {
        return view->relativeOffset_;
    }

    void setRelativePosition(View *view, const Vector2d &newOffset){
        view->relativeOffset_ = newOffset;
    }

    Vector2d getSize(View *view){
        return view->size_;
    }

    bool visible(View *view){
        return !(view->visible_);
    }

    void setVisible(View *view, bool visible){
        view->visible_ = !visible;
    }

    std::vector<std::pair<View*, SubviewInfo*>> subviews_;

    View *parent_ = nullptr;
};



class DockLayout final : public Layout {
public:
    enum DockPosition {
        EdgeTop,
        EdgeBottom,
        EdgeRight,
        EdgeLeft,
        Center,
        Rest,
        Nb
    };

    class SubviewInfoDock : public SubviewInfo {
    public:
        SubviewInfoDock(DockPosition t): subviewType_(t){ }
        DockPosition subviewType_;
    };



    virtual bool arrange(const Vector2d &parentInnerSize, Vector2d &contentSize) override {
        contentSize = { 0 , 0 };

        double top = 0;
        double left = 0;
        double right = parentInnerSize[0];
        double bottom = parentInnerSize[1];

        bool finished = false;
        for (auto p : subviews_) {
            auto view = p.first;
            auto relativeOffset = getRelativePosition(view);

            auto subviewType = dynamic_cast<SubviewInfoDock*>(p.second);

            if (finished) { return false; }

            switch (subviewType->subviewType_){
            case DockPosition::EdgeBottom:
                setWidth(view, right - left);
                bottom -= getSize(view)[1];
                setRelativePosition(view, { left, bottom });
                break;
            case DockPosition::EdgeRight:
                setHeight(view, bottom - top);
                right -= getSize(view)[0];
                setRelativePosition(view, { right, top });
                break;
            case DockPosition::EdgeTop:
                setWidth(view, right - left);
                setRelativePosition(view, { left, top });
                top += getSize(view)[1];
                break;
            case DockPosition::EdgeLeft:
                setHeight(view, bottom - top);
                setRelativePosition(view, { left, top });
                left += getSize(view)[0];
                break;
            case DockPosition::Rest:
                setWidth(view, right - left);
                setHeight(view, bottom - top);
                setRelativePosition(view, { left, top });
                finished = true;
                break;
            case DockPosition::Center:
                auto refSize = getSize(view);
                setRelativePosition(view, { parentInnerSize[0] / 2 - refSize[0] / 2, parentInnerSize[1] / 2 - refSize[1] / 2 });
                finished = true;
                break;
            }

            for (int i = 0; i < 2; i++) {
                auto refSize = getSize(view);
                if ((relativeOffset + refSize)[i] > contentSize[i]) contentSize[i] = (relativeOffset + refSize)[i];
            }
        }

        return true;
    }

    virtual bool add(const std::string &description, View *view) {
        auto index = description.find(',');
        auto dockTypeString = description.substr(0, index);
        auto valueString = description.substr(index + 1);
        if (dockTypeString == "top") {
            setHeight(view, stof(valueString));
            settle(view, new SubviewInfoDock(EdgeTop));
        }
        else if (dockTypeString == "left") {
            setWidth(view, stof(valueString));
            settle(view, new SubviewInfoDock(EdgeLeft));
        }
        else if (dockTypeString == "bottom") {
            setHeight(view, stof(valueString));
            settle(view, new SubviewInfoDock(EdgeBottom));
        }
        else if (dockTypeString == "right") {
            setWidth(view, stof(valueString));
            settle(view, new SubviewInfoDock(EdgeRight));
        }
        else if (dockTypeString == "fill") {
            settle(view, new SubviewInfoDock(Rest));
        }
        else if (dockTypeString == "center") {
            auto size = (Vector2d*)(Vector2d::parse(valueString));
            if (!size) {
                throw;
            }
            resize(view, *size);
            settle(view, new SubviewInfoDock(Center));
        }

        return true;
    }
};


}
