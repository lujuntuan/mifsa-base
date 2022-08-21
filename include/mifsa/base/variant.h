/*********************************************************************************
 *Copyright(C): Juntuan.Lu, 2020-2030, All rights reserved.
 *Author:  Juntuan.Lu
 *Version: 1.0
 *Date:  2022/04/01
 *Email: 931852884@qq.com
 *Description:
 *Others:
 *Function List:
 *History:
 **********************************************************************************/

#ifndef MIFSA_BASE_VARIANT_H
#define MIFSA_BASE_VARIANT_H

#include "mifsa/base/define.h"
#include <functional>
#include <initializer_list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

//#define VARIANT_OPERATOR_DISABLE_WRITABLE
//#define VARIANT_OPERATOR_ENABLE_THROW

MIFSA_NAMESPACE_BEGIN

class VariantList;
class VariantMap;

namespace VariantDeclare {
template <typename T>
int getType();
}

class MIFSA_EXPORT VariantValue {
protected:
    VariantValue() noexcept;
    virtual ~VariantValue() noexcept;
    virtual void parseOut(std::string& json, int depth) const noexcept = 0;
    virtual int type() const noexcept = 0;
    virtual bool isEqual(const VariantValue* value) const noexcept = 0;
    virtual bool isLess(const VariantValue* value) const noexcept = 0;
    virtual void* toPtr() const noexcept = 0;
    virtual bool toBool() const noexcept;
    virtual int toInt() const noexcept;
    virtual double toDouble() const noexcept;
    virtual const std::string& toString() const noexcept;
    virtual const VariantList& toList() const noexcept;
    virtual const VariantMap& toMap() const noexcept;

public:
    using ConstructCb = std::function<void*(void*)>;
    using DestructCb = std::function<void(void*)>;
    static int registerType(size_t hashCode, ConstructCb constructCb, DestructCb destructCb) noexcept;

private:
    friend class Variant;
    friend class VariantInt;
    friend class VariantDouble;
    friend class VariantParser;
    friend class VariantRegisterGlobal;
};

class MIFSA_EXPORT Variant final {
public:
    enum Type {
        TYPE_NULL = 0,
        TYPE_BOOL,
        TYPE_INT,
        TYPE_DOUBLE,
        TYPE_STRING,
        TYPE_LIST,
        TYPE_MAP,
        TYPE_CUSTOM_BEGIN = 100
    };
    enum ParseType {
        PARSE_UNKNOWN = 0,
        PARSE_OUT_SMALL,
        PARSE_OUT_BEAUTIFY,
        PARSE_IN_STANDARD,
        PARSE_IN_COMMENTS
    };
    using Shape = std::initializer_list<std::pair<std::string, Type>>;
    Variant() noexcept;
    Variant(std::nullptr_t) noexcept;
    Variant(bool value) noexcept;
    Variant(int value) noexcept;
    Variant(double value) noexcept;
    Variant(const std::string& value) noexcept;
    Variant(std::string&& value) noexcept;
    Variant(const char* value) noexcept;
    Variant(const VariantList& values) noexcept;
    Variant(VariantList&& values) noexcept;
    Variant(const VariantMap& values) noexcept;
    Variant(VariantMap&& values) noexcept;
    template <class T, class = decltype(&T::toVariant)>
    Variant(const T& t) noexcept
        : Variant(t.toVariant())
    {
    }
    template <class T, typename std::enable_if<std::is_constructible<Variant, decltype(*std::declval<T>().begin())>::value, int>::type = 0>
    Variant(const T& list) noexcept
        : Variant(VariantList(list.begin(), list.end()))
    {
    }
    template <class T, typename std::enable_if<std::is_constructible<std::string, decltype(std::declval<T>().begin()->first)>::value && std::is_constructible<Variant, decltype(std::declval<T>().begin()->second)>::value, int>::type = 0>
    Variant(const T& map) noexcept
        : Variant(VariantMap(map.begin(), map.end()))
    {
    }
    Variant(void* value) noexcept = delete;
    ~Variant() noexcept;
    int type() const noexcept;
    bool isValid() const noexcept;
    bool isNull() const noexcept;
    bool isBool() const noexcept;
    bool isInt() const noexcept;
    bool isDouble() const noexcept;
    bool isNumber() const noexcept;
    bool isString() const noexcept;
    bool isList() const noexcept;
    bool isMap() const noexcept;
    bool isCustom() const noexcept;
    bool hasShape(const Shape& shape, std::string* errorString = nullptr) const noexcept;
    bool toBool(bool defaultValue = false) const noexcept;
    int toInt(int defaultValue = 0) const noexcept;
    double toDouble(double defaultValue = 0) const noexcept;
    const std::string& toString(const std::string& defaultValue = std::string()) const noexcept;
    const char* toCString(const char* defaultValue = "") const noexcept;
    std::vector<int> toIntList() const noexcept;
    std::vector<double> toDoubleList() const noexcept;
    std::vector<std::string> toStringList() const noexcept;
    const VariantList& toList() const noexcept;
    const VariantMap& toMap() const noexcept;
    template <class T, class = decltype(&T::fromVariant)>
    T toCustom() noexcept
    {
        return T::fromVariant(*this);
    }
    template <typename T, typename std::enable_if<std::is_constructible<Variant, T>::value, int>::type = 0>
    void setValue(const T& value)
    {
        *this = Variant(value);
    }
    template <typename T, typename std::enable_if<std::is_constructible<Variant, T>::value, int>::type = 0>
    T value() const
    {
        if (!isCustom()) {
            return *static_cast<T*>(m_ptr->toPtr());
        } else {
            return T();
        }
    }
    template <typename T, typename std::enable_if<!std::is_constructible<Variant, T>::value, int>::type = 0>
    void setValue(const T& value)
    {
        int type = VariantDeclare::getType<T>();
        *this = Variant(type, (void*)&value);
    }
    template <typename T, typename std::enable_if<!std::is_constructible<Variant, T>::value, int>::type = 0>
    T value() const
    {
        if (isCustom()) {
            return *static_cast<T*>(m_ptr->toPtr());
        } else {
            return T();
        }
    }
    const Variant& operator[](size_t i) const noexcept;
    const Variant& operator[](const std::string& key) const noexcept;
#ifndef VARIANT_OPERATOR_DISABLE_WRITABLE
    inline Variant& operator[](size_t i)
    {
#ifdef VARIANT_OPERATOR_ENABLE_THROW
        return getSubValue(i, true);
#else
        return getSubValue(i, false);
#endif
    }
    inline Variant& operator[](const std::string& key)
    {
#ifdef VARIANT_OPERATOR_ENABLE_THROW
        return getSubValue(key, true);
#else
        return getSubValue(key, false);
#endif
    }
#endif
    bool operator==(const Variant& rhs) const noexcept;
    bool operator<(const Variant& rhs) const noexcept;
    bool operator!=(const Variant& rhs) const noexcept;
    bool operator<=(const Variant& rhs) const noexcept;
    bool operator>(const Variant& rhs) const noexcept;
    bool operator>=(const Variant& rhs) const noexcept;
    std::string toJson(ParseType parseType = PARSE_OUT_SMALL) const noexcept;
    bool saveJson(const std::string& filePath, ParseType parseType = PARSE_OUT_BEAUTIFY) const noexcept;
    static Variant fromJson(const std::string& json, std::string* errorString = nullptr, ParseType parseType = PARSE_IN_STANDARD) noexcept;
    static Variant readJson(const std::string& filePath, std::string* errorString = nullptr, ParseType parseType = PARSE_IN_STANDARD) noexcept;
    static std::vector<Variant> fromJsonMulti(const std::string& json, std::string* errorString = nullptr, size_t* stopPos = nullptr, ParseType parseType = PARSE_IN_STANDARD) noexcept;
    static std::vector<Variant> readJsonMulti(const std::string& filePath, std::string* errorString = nullptr, size_t* stopPos = nullptr, ParseType parseType = PARSE_IN_STANDARD) noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const Variant& data) noexcept;

private:
    Variant(int type, void* value) noexcept;
    Variant& getSubValue(size_t i, bool canThrow);
    Variant& getSubValue(const std::string& key, bool canThrow);

private:
    friend class VariantParser;
    std::shared_ptr<VariantValue> m_ptr;
};

class MIFSA_EXPORT VariantList : public std::vector<Variant> {
    using std::vector<Variant>::vector;

public:
    VariantList() noexcept = default;
    VariantList(const Variant& values) noexcept;
    VariantList(Variant&& values) noexcept;
    template <class T>
    VariantList(T&& list) noexcept
        : std::vector<Variant>(std::forward<T>(list))
    {
    }
    bool contains(const Variant& data) const noexcept;
    const Variant& value(int index, const Variant& defaultValue = Variant()) const noexcept;
    std::string toJson(Variant::ParseType parseType = Variant::PARSE_OUT_SMALL) const noexcept;
    bool saveJson(const std::string& filePath, Variant::ParseType parseType = Variant::PARSE_OUT_BEAUTIFY) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const VariantList& data) noexcept;
};

class MIFSA_EXPORT VariantMap : public std::map<std::string, Variant> {
    using std::map<std::string, Variant>::map;

public:
    VariantMap() noexcept = default;
    VariantMap(const Variant& values) noexcept;
    VariantMap(Variant&& values) noexcept;
    template <class T>
    VariantMap(T&& map) noexcept
        : std::map<std::string, Variant>(std::forward<T>(map))
    {
    }
    template <class T>
    void insert(const std::string& key, T&& data) noexcept
    {
        remove(key);
        emplace(key, std::forward<T>(data));
    }
    bool remove(const std::string& key) noexcept;
    bool contains(const std::string& key) const noexcept;
    const Variant& value(const std::string& key, const Variant& defaultValue = Variant()) const noexcept;
    std::string toJson(Variant::ParseType parseType = Variant::PARSE_OUT_SMALL) const noexcept;
    bool saveJson(const std::string& filePath, Variant::ParseType parseType = Variant::PARSE_OUT_BEAUTIFY) const noexcept;
    MIFSA_EXPORT friend std::ostream& operator<<(std::ostream& ostream, const VariantMap& data) noexcept;
};

#define VARIANT_DECLARE_TYPE(T, S)                             \
    namespace VariantDeclare {                                 \
        struct S##Register {                                   \
            int type;                                          \
            S##Register()                                      \
            {                                                  \
                type = VariantValue::registerType(             \
                    typeid(T).hash_code(),                     \
                    [](void* value) {                          \
                        return new T(*static_cast<T*>(value)); \
                    },                                         \
                    [](void* ptr) {                            \
                        delete static_cast<T*>(ptr);           \
                    });                                        \
            }                                                  \
        };                                                     \
        template <>                                            \
        inline int getType<T>()                                \
        {                                                      \
            static S##Register reg;                            \
            return reg.type;                                   \
        }                                                      \
    }

MIFSA_NAMESPACE_END

#endif // MIFSA_BASE_VARIANT_H
