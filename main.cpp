#include "kmp.hpp"
#include "redpacket.hpp"
#include "search.hpp"
#include "shuffle.hpp"
#include "sort.hpp"

int main() {
  std::vector<int> bubble_src;
  for (int i = 15; i > 0; --i) {
    bubble_src.push_back(i);
  }
  std::vector<int> select_src = bubble_src;
  std::vector<int> quick_src = bubble_src;
  std::vector<int> heap_src = bubble_src;
  std::vector<int> shuffle_src = bubble_src;

  // shuffle about
  shuffle(shuffle_src);
  std::cout << "-------------------shuffle---------------------" << std::endl;
  show(shuffle_src);

  // sort about
  std::cout << "-------------------sort---------------------" << std::endl;
  std::cout << "origin :        ";
  show(bubble_src);

  bubble(bubble_src);
  std::cout << "bubble_sort :   ";
  show(bubble_src);

  select_sort(select_src);
  std::cout << "select_sort :   ";
  show(select_src);

  quick_sort(quick_src);
  std::cout << "quick_sort :    ";
  show(quick_src);

  heap_sort(heap_src);
  std::cout << "heap_sort :     ";
  show(heap_src);

  // search about
  std::cout << "----------------bin_search------------------" << std::endl;
  std::cout << "bin search result = " << bin_search(heap_src, 5) << std::endl;

  // kmp
  std::cout << "-------------------kmp---------------------" << std::endl;
  std::cout << "kmp result = " << kmp("xxabababca", "abababca") << std::endl;

  /////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////
  std::cout << "-----------------red_packet-------------------" << std::endl;
  red_packet(10, 1000);
}