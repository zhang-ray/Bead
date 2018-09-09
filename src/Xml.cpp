#include "Xml.hpp"
#include <fstream>
#include <memory>
#include <cstring> // memcpy

namespace Bead{

namespace {

auto tabSize = 4;

enum class LeftBracketType : uint8_t {UnknownType = 0,Comment,CDATA,XmlHeader,ElementFooter,ElementHeader};
const std::vector<std::string> leftBracketIdentifiers = {"<!--","<![CDATA[","<?","</","<",};
enum class RightBracketType : uint8_t {UnknownType = 0,Comment,CDATA,XmlHeader,Element_Simple,Element_Normal,};
const std::vector<std::string> rightBracketIdentifiers = {"-->","]]","?>","/>",">",};
const std::vector<std::pair<const char, std::string>> transformTexts = {{ '<', "&lt;" },{ '>', "&gt;" },{ '&', "&amp;" },{ '\'', "&apos;" },{ '\"', "&quot;" },};

class ReferenceStream {public: const char *startPosition_; const char *endPosition_;};



class Parser {
public:
    Xml::Element *currentParent_ = nullptr;
    ReferenceStream range_;
    LeftBracketType leftBracketType_;
    RightBracketType rightBracketType_;
    const char *lastLeftBracket_ = nullptr;

public:
    bool pase() {
        auto getLeftBracketType = [](const char *text) -> LeftBracketType {
            for (decltype(leftBracketIdentifiers.size()) i = 0; i < leftBracketIdentifiers.size(); i++) {
                auto len = leftBracketIdentifiers[i].length();
                if (leftBracketIdentifiers[i].compare(0, len, text, len) == 0)
                    return (LeftBracketType)(i + 1);
            }
            return LeftBracketType::UnknownType;
        };

        auto isWhiteSpace = [](char p) -> bool {
            auto isUTF8Continuation = [](const char p) -> bool{ return (p & 0x80) != 0; };
            return !isUTF8Continuation(p) && isspace(static_cast<unsigned char>(p));
        };

        auto skipWhiteSpace = [&isWhiteSpace](const char* &p) { for (; isWhiteSpace(*p); p++); };

        auto getElementNameAndAttribute = [&skipWhiteSpace, &isWhiteSpace](Xml::Element &element, ReferenceStream range) -> bool {

            auto parseText = [] (std::string &outputString, const char *lastPosition, const char *currentPosition) -> bool{
                auto len = currentPosition - lastPosition + 1;
                outputString.resize(len);
                memcpy((void*)outputString.c_str(), (void*)lastPosition, len);
                for (size_t index = 0; index < outputString.length(); index++) {
                    if (outputString[index] == '&') {
                        for (auto &p : transformTexts) {
                            auto golden = p.second;
                            auto len = golden.length();
                            auto _2 = &(outputString[index]);
                            if (index + len < outputString.length()) {
                                if (golden.compare(0, len, _2, len) == 0) {
                                    outputString.erase(outputString.begin() + index,outputString.begin() + index + len - 1);
                                    outputString[index] = p.first;
                                    break;
                                }
                            }
                        }
                    }
                }
                return true;
            };


            skipWhiteSpace(range.startPosition_);
            auto curr = range.startPosition_;
            for (; curr < range.endPosition_; curr++) { if (isWhiteSpace(*curr)) break; }

            {
                auto len = curr - range.startPosition_;
                if (!isWhiteSpace(*curr)) len++;
                element.name_.resize(len);
                memcpy((void*)element.name_.c_str(), (void*)range.startPosition_, len);
            }


            if (curr == range.endPosition_) {
                // No Attribute
                return true;
            }

            // Get Attributes Here
            for (;; ) {
                skipWhiteSpace(curr);
                auto lastPos = curr;
                for (; curr < range.endPosition_; curr++) {
                    if ('=' == *curr) {
                        break;
                    }
                }
                Xml::Attribute attribute;
                parseText(attribute.key_, lastPos, curr - 1);
                if ('=' == *curr) curr++;

                skipWhiteSpace(curr);
                for (; curr < range.endPosition_; curr++) {
                    if ('\"' == *curr) break;
                }
                auto firstQuotation = curr++;
                for (; curr < range.endPosition_; curr++) {
                    if ('\"' == *curr) break;
                }
                parseText(attribute.value_, firstQuotation + 1, curr - 1);
                if ('\"' == *curr) curr++;
                element.attributes_.push_back(std::move(attribute));

                if (curr >= range.endPosition_) break;
            }

            return true;
        };


        auto getRightBracketType = [](const char *texts) -> RightBracketType {
            for (decltype(rightBracketIdentifiers.size()) i = 0; i < rightBracketIdentifiers.size(); i++) {
                auto len = rightBracketIdentifiers[i].length();
                auto golden = rightBracketIdentifiers[i];
                auto __2 = texts - len + 1;
                auto compareResult = golden.compare(0, len, __2, len);
                if (compareResult == 0) {
                    return (RightBracketType)(i + 1);
                }
            }
            return RightBracketType::UnknownType;
        };


        auto &range = range_;
        size_t a = 0;
        size_t b = 0;
        for (auto curr = range.startPosition_; curr < range.endPosition_; curr++) {
            if (*curr == '<') {
                a++;
                leftBracketType_ = getLeftBracketType(curr);
                lastLeftBracket_ = curr;
            }
            else if (*curr == '>') {
                b++;
                if (leftBracketType_ == LeftBracketType::ElementHeader) {
                    Xml::Element newElement;
                    auto endOfHeader = curr;
                    auto rightType = getRightBracketType(curr);
                    if (rightType == RightBracketType::Element_Normal) {
                    }
                    else if (rightType == RightBracketType::Element_Simple) {
                    }
                    else {
                        continue;
                    }
                    endOfHeader = curr - rightBracketIdentifiers[(size_t)rightType - 1].length();
                    getElementNameAndAttribute(newElement, { lastLeftBracket_ + 1, endOfHeader });
                    currentParent_->subElements_.push_back(std::move(newElement));
                    if (rightType == RightBracketType::Element_Normal) {
                        auto theNode = &(currentParent_->subElements_.back());
                        theNode->parent_ = currentParent_;
                        currentParent_ = theNode;
                    }
                }
                else if (leftBracketType_ == LeftBracketType::ElementFooter) {
                    currentParent_ = currentParent_->parent_;
                }
            }
            if (a - b > 1) {
                return false;
            }
        }
        if (a != b) {
            return false;
        }
        return true;
    }
};


class Printer {
public:
    size_t depth_;
public:
    void print(const Xml::Element &element, std::string &outString) {

        auto appendString = [](std::string &outString, std::string data, bool useQuotation = false) {
            if (useQuotation) outString.append("\"");

            for (size_t index = 0; index < data.length(); index++) {
                for (auto &p : transformTexts) {
                    if (p.first == data[index]) {
                        auto golden = p.second;
                        auto len = golden.length();
                        data.replace(index, 1, golden.c_str(), len);
                        break;
                    }
                }
            }
            outString.append(data.c_str());
            if (useQuotation) outString.append("\"");
        };

        outString.append(depth_ * tabSize, ' ');
        outString.append("<");
        appendString(outString, element.name_);
        for (const auto &attr : element.attributes_) {
            outString.append(" ");
            appendString(outString, attr.key_);
            outString.append("=");
            appendString(outString, attr.value_, true);
        }

        if (element.subElements_.size() > 0) {
            outString.append(">");
            outString.append("\n");
            depth_++;
            for (auto &subElement : element.subElements_) {
                print(subElement, outString);
            }
            depth_--;
            outString.append(depth_ * tabSize, ' ');
            outString.append("</");
            outString.append(element.name_.c_str());
            outString.append(">");
            outString.append("\n");
        }
        else {
            // No subview
            outString.append("/");
            outString.append(">");
            outString.append("\n");
        }
    }

};


}

bool Xml::load(const char *filename){
    std::vector<char> texts;
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) {return false;}
    auto startP = ifs.tellg();
    ifs.seekg(0, std::ios::end);
    auto fileSize = ifs.tellg() - startP;
    texts.resize(fileSize, 0);
    ifs.seekg(0, std::ios::beg);
    ifs.read(texts.data(), fileSize);

    ReferenceStream range;
    range.startPosition_ = texts.data();
    range.endPosition_ = &texts[(unsigned int) fileSize];

    Parser parser;
    parser.range_ = range;
    Xml::Element element;
    parser.currentParent_ = &element; // Root Element
    parser.pase();
    subElements_ = element.subElements_;

    return true;
}

bool Xml::save(const char *filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) {return false;}

    Xml::Element element;
    element.subElements_ = subElements_;
    Printer printer;
    printer.depth_ = 0;

    std::string outString = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    for (auto &subElement : subElements_) {
        printer.print(subElement, outString);
    }

    ofs.write(outString.c_str(), outString.length());
    ofs.close();
    return true;
}


}
