#ifndef GF256_HPP
#define GF256_HPP

#include "impl/representations.hpp"

#include <cstddef>

namespace GF256
{

class Element
{
  unsigned char m_additive_rep = 0;
  unsigned char m_mult_rep = add_to_mult_rep[0];

public:
  constexpr Element () {}

  constexpr Element (int i)
  {
    if (i == 0)
      return;

    if (i == 1)
      {
        *this = Element (static_cast<unsigned char> (1));
        return;
      }

    int real_i = (i % 2 == 0) ? 0 : 1;

    *this = Element (static_cast<unsigned char> (real_i));
    return;
  }

  constexpr Element pow (int power) const
  {
    if (m_mult_rep == 255)
      return Element ();

    int result_power = (m_mult_rep * power) % 255;

    if (result_power < 0)
      result_power = 255 - result_power;

    return Element::from_mult_rep (result_power);
  }

  constexpr Element inv () const
  {
    if (m_mult_rep == 255)
      std::terminate (); // zero element has no inverse;

    if (m_mult_rep == 0)
      return *this;

    return Element::from_mult_rep (255 - m_mult_rep);
  }

  constexpr Element &operator *= (const Element &rhs)
  {
    *this = (*this) * rhs;
    return *this;
  }

  constexpr Element &operator += (const Element &rhs)
  {
    *this = (*this) + rhs;
    return *this;
  }

  constexpr Element &operator /= (const Element &rhs)
  {
    *this = (*this) / rhs;
    return *this;
  }

  constexpr Element &operator -= (const Element &rhs)
  {
    *this = (*this) - rhs;
    return *this;
  }

  constexpr explicit Element (unsigned char additive_rep)
    : m_additive_rep (additive_rep),
      m_mult_rep (add_to_mult_rep[additive_rep]) {}

private:

  constexpr Element (unsigned char additive_rep, unsigned char mult_rep)
    : m_additive_rep (additive_rep),
      m_mult_rep (mult_rep) {}

  static constexpr Element from_mult_rep (unsigned char mult_rep)
  {
    Element el;
    el.m_mult_rep = mult_rep;
    el.m_additive_rep = mult_to_add_rep[mult_rep];
    return el;
  }

  friend struct std::hash<Element>;

  friend constexpr Element primitive_root ();
  friend constexpr Element neutral_mult_element ();

  friend std::string to_string_as_polynom (const Element &);
  friend constexpr bool operator == (const Element &, const Element &);
  friend constexpr bool operator != (const Element &, const Element &);
  friend constexpr bool operator < (const Element &, const Element &);
  friend constexpr bool operator > (const Element &, const Element &);
  friend constexpr bool operator <= (const Element &, const Element &);
  friend constexpr bool operator >= (const Element &, const Element &);
  friend constexpr Element operator * (const Element &, const Element &);
  friend constexpr Element operator + (const Element &, const Element &);
  friend constexpr Element operator / (const Element &, const Element &);
  friend constexpr Element operator - (const Element &, const Element &);
};

inline constexpr Element primitive_root ()       {return Element (static_cast<unsigned char> (2));}
inline constexpr Element neutral_mult_element () {return 1;}
inline constexpr Element zero_element ()         {return 0;}

inline constexpr bool operator == (const Element &lhs, const Element &rhs)
{
  return lhs.m_additive_rep == rhs.m_additive_rep;
}

inline constexpr bool operator != (const Element &lhs, const Element &rhs)
{
  return lhs.m_additive_rep != rhs.m_additive_rep;
}

inline constexpr bool operator < (const Element &lhs, const Element &rhs)
{
  return lhs.m_additive_rep < rhs.m_additive_rep;
}

inline constexpr bool operator > (const Element &lhs, const Element &rhs)
{
  return lhs.m_additive_rep > rhs.m_additive_rep;
}

inline constexpr bool operator <= (const Element &lhs, const Element &rhs)
{
  return lhs.m_additive_rep <= rhs.m_additive_rep;
}

inline constexpr bool operator >= (const Element &lhs, const Element &rhs)
{
  return lhs.m_additive_rep >= rhs.m_additive_rep;
}

inline constexpr Element operator* (const Element &lhs, const Element &rhs)
{
  if (lhs.m_mult_rep == 255 || rhs.m_mult_rep == 255)
    return Element ();

  int sum_of_powers = lhs.m_mult_rep + rhs.m_mult_rep;

  if (sum_of_powers < 255)
    return Element::from_mult_rep (sum_of_powers);

  return Element::from_mult_rep (sum_of_powers - 255);
}

inline constexpr Element operator + (const Element &lhs, const Element &rhs)
{
  return Element (static_cast<unsigned char> (lhs.m_additive_rep ^ rhs.m_additive_rep));
}

inline constexpr Element operator / (const Element &lhs, const Element &rhs)
{
  if (lhs.m_mult_rep == 255)
    return Element ();

  if (rhs.m_mult_rep == 255)
    std::terminate (); //Inverse of zero element

  int dif_of_powers = lhs.m_mult_rep - rhs.m_mult_rep;

  if (dif_of_powers < 0)
    return Element::from_mult_rep (255 + dif_of_powers);

  return Element::from_mult_rep (dif_of_powers);
}

inline constexpr Element operator - (const Element &lhs, const Element &rhs)
{
  return lhs + rhs;
}

inline constexpr Element pow (const Element &base, int power)
{
  return base.pow (power);
}

inline constexpr Element inv (const Element &src)
{
  return src.inv ();
}

inline std::string to_string_as_polynom (const Element &el)
{
  std::string retval;

  bool something_dumped = false;
  for (int i = 7; i >= 0; i--)
    {
      int byte_status = (el.m_additive_rep >> i) & 1;
      if (!byte_status)
        continue;

      if (something_dumped)
        retval.append (" + ");

      if (i > 1)
        {
          retval.append ("x^");
          retval.append (std::to_string (i));

          something_dumped = true;
        }

      if (i == 1)
        {
          retval.append ("x");
          something_dumped = true;
        }

      if (i == 0)
        {
          retval.append ("1");
          something_dumped = true;
        }
    }

  if (retval.empty ())
    return "0";

  return retval;
}

} //namespace GF256

namespace std
{
template<>
struct hash<GF256::Element>
{
  constexpr size_t operator () (const GF256::Element &src) const
  {
    return static_cast<size_t> (src.m_additive_rep);
  }
};
}

#endif // GF256_HPP
