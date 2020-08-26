#include "kmp.hpp"
#include "lru.hpp"
#include "lru_t.hpp"
#include "redpacket.hpp"
#include "search.hpp"
#include "shuffle.hpp"
#include "sort.hpp"

class MyClass {
private:
  static int value_;
  int current_{0};

public:
  MyClass() {
    current_ = ++value_;
    std::cout << "MyClass - " << value_ << std::endl;
  }

  ~MyClass() { std::cout << "~MyClass - " << current_ << std::endl; }

  MyClass(const MyClass &) {
    current_ = ++value_;
    std::cout << "copy constructor - "
              << "MyClass(const MyClass &) - " << value_ << std::endl;
  };

  MyClass &operator=(const MyClass &) {
    current_ = ++value_;
    std::cout << "assignment constructor - "
              << "MyClass &operator=(const MyClass &) - " << value_
              << std::endl;
  };

  MyClass(MyClass &&) {
    current_ = ++value_;
    std::cout << "move copy constructor - "
              << "MyClass(const MyClass &) - " << value_ << std::endl;
  };

  MyClass &operator=(MyClass &&) {
    current_ = ++value_;
    std::cout << "move assignment constructor - "
              << "MyClass &operator=(const MyClass &) - " << value_
              << std::endl;
  };
};

int MyClass::value_ = 0;

void test_map() {
  std::unordered_map<int, MyClass> mm_;
  MyClass c;

  auto temp = std::pair<int, MyClass>(1, c);
  std::cout << "-----" << std::endl;
  mm_.insert(temp);
  std::cout << "-----" << std::endl;
  mm_.insert(std::pair<int, MyClass>(2, c));
  std::cout << "-----" << std::endl;

  for (auto &iter : mm_) {
    std::cout << iter.first << std::endl;
  }
}

int main() {
  test_map();

  shuffle_test();
  sort_test();
  search_test();
  kmp_test();
  red_packet_test();

  lru_t_test();
  lru_test();

  std::cout << std::chrono::seconds(-1).count() << std::endl;
}
