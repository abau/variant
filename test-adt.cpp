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
unsigned int length (const List <t>& list) {

  if (list.constructor.template is <typename List <t>::Nil> ()) {
    return 0;
  }
  else if (list.constructor.template is <typename List <t>::Cons> ()) {
    int      x;
    List <t> xs;

    std::tie (x,xs) = *list.constructor.template get <typename List <t>::Cons> ();

    return 1 + length (xs);
  }
}

int main () {

  List <int> list = List<int>::cons (10, List <int>::cons (20, List <int>::nil ()));

  assert (length <int> (list) == 2);

  return 0;
}

