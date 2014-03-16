#include <string>
#include "variant.hpp"

class Foo {
  public:
    Foo (int) {}  // non-trivial constructor
};

int main () {
  Variant <Foo,int,std::string> variant;
  
  variant.setAt <0> (Foo (5));                            // set by index
  variant.setAt <1> (42);                                 // set by index
  variant.setAt <2> (std::string ("hallo welt"));         // set by index

  variant.set <Foo>         (Foo (5));                    // set by type
  variant.set <int>         (42);                         // set by type
  variant.set <std::string> (std::string ("hello world"));// set by type

  variant.get <std::string> ();                           // gives std::string&
  //variant.get <Foo> ();                                 // runtime error

  assert (variant.is <Foo> ()         == false);          // test by type           
  assert (variant.is <int> ()         == false);          // test by type 
  assert (variant.is <std::string> () == true);           // test by type

  assert ( variant.caseOf <bool> (                        // implicit type matching
              [] (Foo&)         { return false; }  
            , [] (int&)         { return false; }
            , [] (std::string&) { return true ; }
            )
         );

  //variant.set <bool> (false);          // compilation fails (bool is not in variant)
  //variant.setAt <2>   (12);            // compilation fails (incompatible types)
  //variant.setAt <5>   (12);            // compilation fails (index out of bounds)

  Variant <Foo,int,std::string> variant2 (variant);       // (deep) copy construction

  assert (variant2.is <std::string> () == true);

  Variant <Foo,int,std::string> variant3;                 

  variant3 = variant2;                                    // (deep) assignment
  
  variant2.release ();                                    // manual release

  assert (variant3.is <std::string> () == true);

  return 0;
}

