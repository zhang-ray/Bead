#include "UiGenerator.hpp"


namespace Bead {

template<typename T> View * createView_() { return (View*) new T; }
template<typename T> Layout * createLayout_() { return (Layout*) new T; }

#define REGISTER_VIEW(name) viewClassMap_[#name] = &createView_<name>;
#define REGISTER_LAYOUT(name) layoutClassMap_[#name] = &createLayout_<name>;

View *UiGenerator::parseAndCreate(const char *uiFilePath){
    Xml xmlDocument;
    if (!xmlDocument.load(uiFilePath)) return nullptr;

    REGISTER_VIEW(View);
    REGISTER_VIEW(TextView);

    REGISTER_LAYOUT(DockLayout);


    if (viewRoot_ != nullptr) return viewRoot_;

    if (!("Application" == xmlDocument.subElements_[0].name_)) {
        return nullptr;//
    }

    for (auto &ele : xmlDocument.subElements_[0].subElements_) {
        if (ele.name_ ==  "View") {
            if (viewRoot_==nullptr){
                viewRoot_ = (View *)parseView(ele, nullptr);
            }
        }
        else if (auto orphanView = parseView(ele, nullptr)) {
            // handle orphan View
            orphanViewList_.push_back(orphanView);
        }
    }

    viewRoot_->resetAsRoot();

    return viewRoot_;
}


View * UiGenerator::createView(const char* viewType){
    return viewClassMap_[viewType]();
}

Layout * UiGenerator::createLayout(const char* layoutType, const char *layoutInfo){
    auto layout = layoutClassMap_[layoutType]();
    return layout;
}

View * UiGenerator::parseView(const Xml::Xml::Element &element, Layout *parentlayout){
    auto view = createView(element.name_.c_str());
    Layout *layout = nullptr;
    std::string subviewInfoText;

    for (const auto &attr : element.attributes_) {
        if ("layout" == attr.key_) {
            auto index = attr.value_.find(',');
            std::string layoutType = attr.value_;
            std::string layoutInfo;
            if (index != std::string::npos) {
                // have something
                layoutType = attr.value_.substr(0, index);
                layoutInfo = attr.value_.substr(index + 1);
            }
            layout = createLayout(layoutType.c_str(), layoutInfo.c_str());
        }
        else if ("subview_info" == attr.key_) {
            subviewInfoText = attr.value_;
            if (parentlayout) {
                parentlayout->add(subviewInfoText, view);
            }
        }
        else if (view->setStaticProperty(attr.key_, attr.value_)) {

        }
        else {
            throw;
        }
    }

    for (const auto &sub : element.subElements_) {
        auto subview = parseView(sub, layout);
    }

    if (layout) view->setLayout(layout);

    return view;
}

}
