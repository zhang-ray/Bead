#pragma once
#include <vector>
#include <string>

namespace Bead{

class Xml final {
public:
    class Attribute{
    public:
        std::string key_;
        std::string value_;
    };

    class Element {
    public:
        std::string name_;
        std::vector<Element> subElements_;
        std::vector<Attribute> attributes_;
        Element *parent_ = nullptr;
    };

public:
    bool load(const char *filename);
    bool save(const char *filename);
    std::vector<Xml::Element> subElements_;
};

}
