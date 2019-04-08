#include "run_suits.hpp"

#include <vector>
#include <chrono>
#include <random>
#include <cmath>

#include "gf256-3rd-party/gf256.h"

#include "GF256/GF256.hpp"

#include <unordered_set>
#include <cstdio>

namespace chr = std::chrono;

template <class T>
void doNotOptimizeAway (T&& datum) {
  asm volatile("" : "+r" (datum));
}

int get_msecs (const chr::steady_clock::duration &dur)
{
  return static_cast<int> (chr::duration_cast<chr::milliseconds> (dur).count ());
}


bool GF256::run_test_suit ()
{
  printf ("=================================TEST SUIT=================================\n");

  Element root = GF256::primitive_root ();

  std::unordered_set<Element> power_elements;
  std::unordered_set<Element> inverse_elements;

  printf ("SECTION: POWER\n");
  for (int i = 0; i < 255; i++)
    {
      Element power = root.pow (i);

      printf ("  x^%d = %s\n", i, to_string_as_polynom (power).c_str ());

      auto it = power_elements.find (power);
      if (it != power_elements.end ())
        {
          printf ("SECTION RESULT: POWER: ERROR: x is not a generator of a multiplicative group!\n");
          return false;
        }

      power_elements.insert (power);
    }

  printf ("SECTION RESULT: POWER: OK!\n");

  printf ("SECTION: MULTIPLICATION\n");

  inverse_elements.clear ();

  Element prod = GF256::neutral_mult_element ();

  for (int i = 1; i <= 255; i++)
    {
      Element prev_prod = prod;
      printf ("  x^%d = (%s) * x = %s", i,
              to_string_as_polynom (prev_prod).c_str (),
              to_string_as_polynom (prod *= root).c_str ());

      if (root.pow (i) != prod)
        {
          printf (" : ERROR\n");
          printf ("SECTION RESULT: MULTIPLICATION: ERROR: multiplication produced wrong result\n");
          return false;
        }
      else
        printf (" : OK\n");
      }

  printf ("SECTION RESULT: MULTIPLICATION: OK!\n");

  printf ("SECTION: INVERSE\n");
  for (auto &&element : power_elements)
    {
      Element inverse = element.inv ();
      Element prod = element * inverse;
      printf ("  (%s)^-1 = %s\n", to_string_as_polynom (element).c_str (), to_string_as_polynom (inverse).c_str ());
      printf ("  (%s) * (%s) = %s",
              to_string_as_polynom (element).c_str (),
              to_string_as_polynom (inverse).c_str (),
              to_string_as_polynom (prod).c_str ());

      if (prod == neutral_mult_element ())
        printf (" : OK!\n");
      else
        {
          printf (" : ERROR!\n");
          printf ("SECTION RESULT: INVERSE: ERROR: inverse * base != 1\n");
          return false;
        }

      auto it = inverse_elements.find (inverse);
      if (it != inverse_elements.end ())
        {
          printf ("SECTION RESULT: INVERSE: ERROR: inverse operation is not biective\n");
          return false;
        }

      inverse_elements.insert (inverse);
    }

  printf ("SECTION: DIVISION\n");

  Element div = GF256::neutral_mult_element ();
  Element inverse_root = root.inv ();
  for (int i = 1; i <= 255; i++)
    {
      Element prev_div = div;
      printf ("  x^-%d = (%s) / x = %s", i,
              to_string_as_polynom (prev_div).c_str (),
              to_string_as_polynom (div /= root).c_str ());

      if (inverse_root.pow (i) != div)
        {
          printf (" : ERROR\n");
          printf ("SECTION RESULT: DIVISION: ERROR: division produced wrong result\n");
          return false;
        }
      else
        printf (" : OK\n");
    }

  printf ("SECTION RESULT: DIVISION: OK!\n");

  printf ("SECTION: ADDITION\n");

  for (int max_i = 1; max_i <= 254; max_i++)
    {
      Element sum;
      for (int i = 0; i <= max_i; i++)
        sum += root.pow (i);

      Element power = root.pow (max_i + 1);
      Element numerator = power - 1;
      Element denomenator = root - 1;

      Element must_be_sum = numerator / denomenator;
      printf ("  1 + ... + x^%d = %s", max_i, to_string_as_polynom (sum).c_str ());

      if (sum != must_be_sum)
        {
          printf (" : ERROR\n");
          printf ("SECTION RESULT: ADDITION: ERROR: addition produced wrong result\n");
          return false;
        }
      else
        printf (" : OK\n");
    }

  printf ("SECTION RESULT: ADDITION: OK!\n");
  return true;
}

void GF256::run_benchmark_suit ()
{
  gf256_init ();

  printf ("==============================BENCHMARK SUIT==============================\n");
  printf ("Comparing with github.com/catid/gf256 implementation\n");

  std::srand (0);

  std::vector<Element> my_elements;
  std::vector<uint8_t>  his_elements;

  my_elements.reserve (10000);
  his_elements.reserve (10000);

  std::vector<Element> my_inv_elements;
  std::vector<uint8_t>  his_inv_elements;

  my_inv_elements.reserve (10000);
  his_inv_elements.reserve (10000);

  for (int i = 0; i < 10000; i++)
    {
      uint8_t byte = static_cast<uint8_t> (abs (std::rand ()) % 256);

      my_elements.emplace_back (byte);
      his_elements.emplace_back (byte);

      if (!byte)
          byte = 10;

      my_inv_elements.emplace_back (byte);
      his_inv_elements.emplace_back (byte);
    }

  chr::steady_clock clock;

  printf ("SECTION: ADDITION\n");
  printf ("  Perfoming 10^8 additions\n");

  auto begin = clock.now ();
  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        Element sum = my_elements[i] + my_elements[j];
        doNotOptimizeAway (sum);
      }
  auto end = clock.now ();

  auto my_dif = end - begin;

  begin = clock.now ();
  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        uint8_t sum = gf256_add (his_elements[i], his_elements[j]);
        doNotOptimizeAway (sum);
      }
  end = clock.now ();

  auto his_dif = end - begin;

  printf ("  GF256 time: %d\n", get_msecs (my_dif));
  printf ("  gf256-3rd-party time: %d\n", get_msecs (his_dif));

  printf ("SECTION: MULTIPLICATION\n");
  printf ("  Perfoming 10^8 multiplications\n");

  begin = clock.now ();
  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        Element prod = my_elements[i] * my_elements[j];
        doNotOptimizeAway (prod);
      }

  end = clock.now ();

  my_dif = end - begin;

  begin = clock.now ();
  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        uint8_t prod = gf256_mul (his_elements[i], his_elements[j]);
        doNotOptimizeAway (prod);
      }

  end = clock.now ();

  his_dif = end - begin;

  printf ("  GF256 time: %d\n", get_msecs (my_dif));
  printf ("  gf256-3rd-party time: %d\n", get_msecs (his_dif));

  printf ("SECTION: POWER\n");
  printf ("  Perfoming 10^8 powers\n");

  begin = clock.now ();


  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        Element power = my_elements[i].pow (j);
        doNotOptimizeAway (power);
      }

  end = clock.now ();

  my_dif = end - begin;

  printf ("  GF256 time: %d\n", get_msecs (my_dif));
  printf ("  gf256-3rd-party time: NOT IMPLEMENTED\n");

  printf ("SECTION: INVERSION\n");
  printf ("  Perfoming 10^8 inversions\n");

  begin = clock.now ();

  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        Element inv = my_inv_elements[j].inv ();
        doNotOptimizeAway (inv);
        doNotOptimizeAway (i);
      }

  end = clock.now ();

  my_dif = end - begin;

  begin = clock.now ();
  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 10000; j++)
      {
        uint8_t inv = gf256_inv (his_inv_elements[j]);
        doNotOptimizeAway (inv);
        doNotOptimizeAway (i);
      }

  end = clock.now ();

  his_dif = end - begin;

  printf ("  GF256 time: %d\n", get_msecs (my_dif));
  printf ("  gf256-3rd-party time: %d\n", get_msecs (his_dif));

  return;
}
