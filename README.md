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

    variant.setAt <2> (std::string ("hallo welt"));

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

## Other approaches

- [Boost.Variant] (http://www.boost.org/doc/libs/1_55_0/doc/html/variant.html)
implements type-safe variants using visitor classes for functions on variants.
