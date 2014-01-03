#include <cassert>
#include <tuple>
#include "variant.hpp"

struct Unit {};

template <typename t>
struct List {
  typedef Unit                     Nil;
  typedef std::tuple <t, List <t>> Cons;

  Variant <Nil, Cons> constructor;

  static List nil () {
    List l;
    l.constructor.template set <Nil> (Nil ());
    return l;
  }

  static List cons (const t& x, const List& xs) {
    List l;
    l.constructor.template set <Cons> (Cons (x,xs));
    return l;
  }
};

template <typename t>
unsigned int length (List <t>& list) {
  return list.constructor.template caseOf <unsigned int> (

        [] (typename List <t>::Nil&) { 
          return 0; 
        }

      , [] (typename List <t>::Cons& arguments) { 
          int&      x  = std::get <0> (arguments);
          List <t>& xs = std::get <1> (arguments);

          return 1 + length (xs);
        }
      );
}

int main () {

  List <int> list = List<int>::cons (10, List <int>::cons (20, List <int>::nil ()));

  assert (length <int> (list) == 2);

  return 0;
}

