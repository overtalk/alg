#pragma once

#include "head.hpp"

class DataTypeGenerator final
{
public:
    using DataType = uint16_t;

    static DataType GenerateType()
    {
        return ++current_type_;
    }

private:
    inline static DataType current_type_{0U};
};

class Base
{
private:
    DataTypeGenerator::DataType type_id_{0U};

public:
    Base(DataTypeGenerator::DataType type_id)
        : type_id_(type_id)
    {
    }

    virtual ~Base() = default;

    template<typename T>
    typename T::Ptr Cast()
    {
        if (type_id_ != T::type_)
            return nullptr;

        return dynamic_cast<typename T::Ptr>(this);
    }
};

template<typename T>
class Any final : public Base
{
public:
    using Ptr = Any*;
    //    using Type = std::enable_if_t<!std::is_same_v<T, std::string> && std::is_same_v<T, std::decay_t<T>>, T>;
    //    using ArgType = std::conditional_t<std::is_arithmetic_v<Type>, const Type, const Type&>;
    //    using RetType = ArgType;

    using Type = T;
    using ArgType = Type;
    using RetType = ArgType;

    inline static const DataTypeGenerator::DataType type_{DataTypeGenerator::GenerateType()};

private:
    inline static const Type default_data_{Type()};
    Type data_{default_data_};

public:
    Any(ArgType data)
        : Base(type_)
        , data_(data)
    {
    }

    ~Any() = default;

    void SetValue(ArgType data)
    {
        data_ = data;
    }

    RetType GetValue()
    {
        return data_;
    }

    inline static RetType GetDefaultValue()
    {
        return default_data_;
    }
};