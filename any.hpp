#pragma once

#include "head.hpp"

class DataTypeGenerator final {
public:
  using Ptr = DataTypeGenerator *;
  using DataType = uint16_t;

private:
  inline static DataType current_type_{0U};

public:
  static DataType GenerateType() { return ++current_type_; }
};

class Base {
public:
  using Ptr = Base *;

private:
  DataTypeGenerator::DataType type_id_{0U};

public:
  Base(DataTypeGenerator::DataType type_id) : type_id_(type_id) {}

  virtual ~Base() = default;

  template <typename T> typename T::Ptr Cast() {
    if (type_id_ != T::type_id_) {
      return nullptr;
    }

    return dynamic_cast<typename T::Ptr>(this);
  }
};

template <typename T> class Any final : public Base {};