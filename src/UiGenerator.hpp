#pragma once
#include <map>
#include <vector>
#include <string>
#include "Xml.hpp"
#include "Layout.hpp"
#include "View.hpp"


namespace Bead{

class UiGenerator final {
private:
    View *viewRoot_ = nullptr;
    std::vector<View*> orphanViewList_;
    std::map<std::string, View*(*)()> viewClassMap_;
    std::map<std::string, Layout*(*)()> layoutClassMap_;
public:
    UiGenerator(){}
    View *parseAndCreate(const char *uiFilePath);

private:
    void parseStyle(const Xml::Element &sytleRootElement);
    View *createView(const char* viewType);
    View *parseView(const Xml::Element &element, Layout *parentlayout);
    Layout *createLayout(const char* layoutType, const char *layoutInfo);
};



}
