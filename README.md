# Weekend project: type-safe, memory-managed sum-types in C++11

Although C++11 provides unrestricted unions, repetitive boilerplate code is needed to
aggregate classes with non-trivial constructors/destructors.
[variant.hpp] (https://github.com/apunktbau/variant/tree/master/variant.hpp) provides type-safe,
memory-managed sum-types using variadic templated unions.

    Variant <bool,int,std::string> variant;

defines a sum-type over `bool`, `int` and `std::string`.
The types of a `Variant` may have non-trivial, parametrized constructors.
The only requirement is a copy-constructor.

There are two ways to set a value. 
By index

    variant.setAt <2> (std::string ("hello world"));

or by type

    variant.set <std::string> (std::string ("hello world"));

. Both methods are statically type-safe. 
To differentiate between values of the same type `setAt` must be used.

Call `T* get <T>` to retrieve a value from a variant:

    variant.get <std::string> ();

`get <T>` results in a run-time error if the variant holds a value of a different type.

Call `bool is <T>` to check against the currently stored type:

    if (variant.is <std::string> ()) { ... }

The `Variant` template is memory-managed: the currently allocated value is deleted if
the variant's destructor is called.
`void release ()` deletes the current value explicitly:

    variant.release ();

See [test-variant.cpp] (https://github.com/apunktbau/variant/tree/master/test-variant.cpp) 
for more code.

## Fake Algebraic Data Types ##

Consider the following definitions in Haskell:

    data List a = Nil | Cons a (List a)

    length list = case list of
      Nil       -> 0
      Cons x xs -> 1 + (length xs)

Let's emulate this in C++11 by using `Variant`.
In order to represent a Haskell constructor without arguments, we introduce a `Unit` type:

    struct Unit {};

`List` becomes a template with one argument:

    template <typename t>
    struct List {

For brevity, we define a type alias for each constructor:

    typedef Unit                     Nil;
    typedef std::tuple <t, List <t>> Cons;

Note that we use `std::tuple` to emulate Haskell constructors with more than one argument.

    Variant <Nil, Cons> constructor;

defines the actual sum type.

For convinience, we define a static function for each constructor.

    static List nil () {
      List l;
      l.constructor.template set <Nil> (Nil ());
      return l;
    }

builds an empty list and

    static List cons (const t& x, const List& xs) {
      List l;
      l.constructor.template set <Cons> (Cons (x,xs));
      return l;
    }

wraps the `Cons` constructor.

We define a `lenght` function on `List <t>`:

    template <typename t>
    unsigned int length (const List <t>& list) {

The base case is rather simple:

    if (list.constructor.template is <typename List <t>::Nil> ()) {
      return 0;
    }

We check the variant's content and return constant `0` if an empty list was passed.
In the recursive case we extract the arguments `x` and `xs` of the `Cons` constructor and
call `length` recursively.

    else if (list.constructor.template is <typename List <t>::Cons> ()) {
      int      x;
      List <t> xs;

      std::tie (x,xs) = *list.constructor.template get <typename List <t>::Cons> ();

      return 1 + length (xs);
    }

Now we can call `length` on actual lists:

    int main () {

      List <int> list = List<int>::cons (10, List <int>::cons (20, List <int>::nil ()));

      assert (length <int> (list) == 2);

      return 0;
    }

See [test-adt.cpp] (https://github.com/apunktbau/variant/tree/master/test-adt.cpp) 
for the full code.

## Other approaches

- [Boost.Variant] (http://www.boost.org/doc/libs/1_55_0/doc/html/variant.html)
implements type-safe variants using visitor classes for functions on variants.
