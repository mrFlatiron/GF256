#include <iostream>
#include <cstring>

#include "run_suits.hpp"

int main (int argc, char *argv[])
{
  const char *usage_string = "Usage: GF256 <option>\n"
                             "OPTIONS:\n"
                             "-h\tPrint implementation details\n"
                             "-t\tRun test suit\n"
                             "-b\tRun benchmark suit";
  if (argc != 2)
    {
      printf ("%s\n", usage_string);
      return 1;
    }

  if (strcmp (argv[1], "-h") == 0)
    {
      printf ("============================IMPLEMENTATION INFO============================\n");
      printf ("This GF256 is implemented as a Z[x] / (x^8 + x^7 + x^6 + x + 1) factor ring\n");
      printf ("Primitive root is x\n");
      printf ("===========================================================================\n");
      return 0;
    }

  if (strcmp (argv[1], "-t") == 0)
    {
      if (GF256::run_test_suit ())
        {
          printf ("===========================================================================\n");
          printf ("TEST RESULT: OK!\n");
          return 0;
        }
      printf ("===========================================================================\n");
      printf ("TEST RESULT: ERROR!\n");
      return 1;
    }

  if (strcmp (argv[1], "-b") == 0)
    {
      GF256::run_benchmark_suit ();
      return 0;
    }

  printf ("%s\n", usage_string);
  return 1;
}
