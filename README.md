GF256 Implementation.

TO USE:
Drop GF256 directory into your project and include "GF256/GF256.hpp" where needed

TO TEST:
Execute the following (qmake needed):

$ cd build-dir
$ qmake pro-file-dir/GF256.pro
$ make
$ ./GF256 -t

DOCUMENTATION:
GF256::Element represents an element of Galois Field of order 256.

Following operators are available in GF256 namespace:
+, -, *, /,
+=, -=, *=, /=,
==, !=, <=, >=, <, >

Following functions are available in GF256 namespace and Element class scope:
Element::pow (int power)                   // el^power
Element::inv ()                            // el^-1
Element neutral_mult_element ()            // returns 1;
Element zero_element ()                    // returns 0;
Element primitive_root ()                  // returns an element powers of which generate entire multiplicative group of GF256
std::string to_string_as_polynom (Element) // Returns polynomial representation of an element

Also a std::hash specialization is present
