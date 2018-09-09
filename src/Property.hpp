#pragma once

#include <vector>
#include <array>
#include <functional>
#include <memory>
#include <string>

#include <cmath>
#include <functional>
#include <exception>

namespace Bead {

class Property{
public:
    virtual ~Property() { }
};


static const size_t c_maxUsage = 20;
static const size_t c_maxKey = 60;


class PropertyDatabase final{
public:
    PropertyDatabase() {
        mainDb_.fill({ nullptr });
        registerUsage("Current");
    }

    void usePreset(const PropertyDatabase &preset) {
        for (size_t i = 0; i < c_maxUsage; i++) {
            for (size_t j = 0; j < c_maxKey; j++) {
                if (!mainDb_[i][j]) {mainDb_[i][j] = preset.mainDb_[i][j];}
            }
        }
    }

    bool registerProperty(std::pair<std::string, std::function<Property*(const std::string&)>> newItem) {
        for (auto &item : registerProperty_) {if (item.first == newItem.first) return false;}
        registerProperty_.push_back(newItem);
        return true;
    }

    bool registerUsage(const std::string &key) {
        for (auto &item : usages_) {if (item == key) return false;}
        usages_.push_back(key);

        return true;
    }

    bool registerUsages(const std::initializer_list<const char *> &keys){
        for (auto &key : keys){registerUsage(key);}
    }

    void approachToUsage(const std::string &usage){
        auto indexUsage = getUsageIndex(usage);
        for (size_t propertyIndex = 0; propertyIndex < registerProperty_.size(); propertyIndex++) {
            mainDb_[0][propertyIndex] = mainDb_[indexUsage][propertyIndex];
            if (mainDb_[c_maxUsage - 1][propertyIndex]){mainDb_[0][propertyIndex] = mainDb_[c_maxUsage - 1][propertyIndex];}
        }
    }

    std::shared_ptr<Property> getProperty(const std::string &key){
        auto propertyIndex = getPropertyIndex(key);
        if (mainDb_[c_maxUsage - 1][propertyIndex]) {mainDb_[0][propertyIndex] = mainDb_[c_maxUsage - 1][propertyIndex];}
        return mainDb_[0][getPropertyIndex(key)];
    }

    bool setConstProperty(const std::string &key, const std::string &value){
        auto indexProperty = getPropertyIndex(key);
        if (mainDb_[c_maxUsage - 1][indexProperty]) mainDb_[c_maxUsage - 1][indexProperty] = nullptr;

        mainDb_[c_maxUsage - 1][indexProperty].reset(registerProperty_[indexProperty].second(value));
        return true;
    }

    bool setProperty(const std::string &usage, const std::string &key, const std::string &value){
        auto indexUsage = getUsageIndex(usage);
        auto indexProperty = getPropertyIndex(key);
        if (!mainDb_[indexUsage][indexProperty]) mainDb_[indexUsage][indexProperty].reset(registerProperty_[indexProperty].second(value));

        return true;
    }

private:
    size_t getUsageIndex(const std::string &usage){
        for (size_t index = 0; index < usages_.size(); index++) {if (usages_[index] == usage) return index;}
        return std::string::npos;
    }

    size_t getPropertyIndex(const std::string &property) {
        for (size_t index = 0; index < registerProperty_.size(); index++) {if (registerProperty_[index].first == property) return index;}
        return std::string::npos;
    }

private:
    std::vector<std::pair<std::string, std::function<Property*(const std::string&)>>> registerProperty_;
    std::array<std::array<std::shared_ptr<Property>, c_maxKey>, c_maxUsage> mainDb_;
private:
    std::vector<std::string> usages_;
};



class Color final : public Property {
public:
    explicit Color():Color(0u, 0){}

    explicit Color(std::string description) {
        try{
            auto alphaIndex = description.find(',');
            if (alphaIndex != std::string::npos) {
                alpha_ = stod(description.substr(alphaIndex + 1));
                description = description.substr(0, alphaIndex);
            }

            auto sharpIndex = description.find('#');
            if (sharpIndex != std::string::npos) {
                auto subString = description.substr(sharpIndex + 1);
                rgb_ = stoi(subString, nullptr, 16);
            }
            else {
                rgb_ = stoi(description, nullptr, 0);
            }
        }
        catch (std::exception &e){
            e.what();
        }
    }

    explicit Color(uint32_t rgb, double alphaValue): rgb_(rgb), alpha_(alphaValue){ }

    static Property *parse(const std::string &description){
        return new Color(description);
    }


    uint32_t rgb_ = 0;
    double alpha_ = 1.0;
};




class Vector2d final : public std::array<double, 2>, public Property{
public:
    Vector2d() : Vector2d(0, 0){ }

    Vector2d(double _1, double _2): array<double, 2>({ _1, _2 }){ }

    Vector2d operator+(const Vector2d& a2) const{
        Vector2d a;
        for (typename Vector2d::size_type i = 0; i < size(); i++)a[i] = at(i) + a2[i];
        return a;
    }

    Vector2d operator-(const Vector2d& a2) const{
        Vector2d a;
        for (typename Vector2d::size_type i = 0; i < size(); i++) a[i] = at(i) - a2[i];
        return a;
    }

    Vector2d operator*(const double factor) const{
        Vector2d a;
        for (typename Vector2d::size_type i = 0; i < size(); i++)a[i] = at(i) * factor;
        return a;
    }

    Vector2d operator/(const double factor) const{
        Vector2d a;
        for (typename Vector2d::size_type i = 0; i < size(); i++)a[i] = at(i) / factor;
        return a;
    }

    void operator+=(const Vector2d& a2){
        for (typename Vector2d::size_type i = 0; i < size(); i++)at(i) += a2[i];
    }

    void operator-=(const Vector2d& a2){
        for (typename Vector2d::size_type i = 0; i < size(); i++)at(i) -= a2[i];
    }

    void operator*=(const double factor){
        for (typename Vector2d::size_type i = 0; i < size(); i++)at(i) *= factor;
    }

    void  operator/=(const double factor){
        for (typename Vector2d::size_type i = 0; i < size(); i++)at(i) /= factor;
    }

    static Property * parse(const std::string &description){
        auto ret = new Vector2d;
        try{
            auto f = description.find_first_of('{') + 1;
            auto l = description.find_last_of('}');
            auto str = description.substr(f, l - f);
            auto index = 0;
            for (auto lastOneElement = false; ;) {
                auto first = str.find_first_of(',');
                if (first == std::string::npos) {first = str.length();lastOneElement = true;}

                ret->at(index++) = stof(str.substr(0, first));

                if (lastOneElement) break;
                str = str.substr(first + 1, str.length() - first);
            }
        }
        catch (std::exception &e) {
            e.what();
        }

        return ret;
    }
};



class Rectangle final: public std::array<double, 4>, public Property {
public:
    Rectangle(double top = 0,double bottom = 0,double left = 0,double right = 0) : std::array<double, 4>({ top, bottom, left, right }) { }
    Rectangle(const Vector2d &vec) : Rectangle(0, vec[1], 0, vec[0]) {}

    static Property *parse(const std::string &description){
        auto ret = new Rectangle;
        try{
            auto f = description.find_first_of('{') + 1;
            auto l = description.find_last_of('}');
            auto str = description.substr(f, l - f);
            auto index = 0;
            for (auto lastOneElement = false; ;) {
                auto first = str.find_first_of(',');
                if (first == std::string::npos) {
                    first = str.length();
                    lastOneElement = true;
                }

                ret->at(index++) = stof(str.substr(0, first));

                if (lastOneElement) break;
                str = str.substr(first + 1, str.length() - first);
            }
        }
        catch (std::exception &e) {
            e.what();
        }

        return ret;
    }

    double getWidth() const{
        double width = right - left;
        if (width < 0) width *= -1;
        return width;
    }

    double getHeight() const{
        double height = bottom - top;
        if (height < 0) height *= -1;
        return height;
    }

    double &top = at(0);
    double &bottom = at(1);
    double &left = at(2);
    double &right = at(3);
};



class Scalar final : public Property{
public:
    Scalar(const double value): value_(value){}
    Scalar():Scalar(0){}

    double get() { return value_; }

    static Property *parse(const std::string &description){
        auto theValue = 0;
        try {
            theValue = stof(description);
        }
        catch (const std::exception &e){
            e.what();
        }

        return new Scalar(theValue);
    }

    double value_ = 0;
};



class Text final : public Property{
public:
    Text(const std::string &description): text(description){ }

    static Property *parse(const std::string &description){
        return new Text(description);
    }

    const char *get() { return text.c_str(); }

    std::string text;
};

}
