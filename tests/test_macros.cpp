#include <mdspan/mdspan.hpp>
#include <gtest/gtest.h>

TEST(mdspan_macros, precondition_violation)
{
#if defined(_MDSPAN_HAS_CUDA) || defined(_MDSPAN_HAS_HIP) || defined(_MDSPAN_HAS_SYCL)
  constexpr auto msg = "";
#else
  constexpr auto msg = "hello, world!";
#endif

  ASSERT_DEATH(MDSPAN_IMPL_PRECONDITION(false and "hello, world!"), msg);
}

TEST(mdspan_macros, precondition_check_constexpr_invocable)
{
  struct fn
  {
    constexpr auto operator()() const
    {
      MDSPAN_IMPL_PRECONDITION(1 + 1 == 2);
      return 42;
    }
  };

  static constexpr auto x = fn{}();
  (void)x;
}
