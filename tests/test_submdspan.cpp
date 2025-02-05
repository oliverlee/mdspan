//@HEADER
// ************************************************************************
//
//                        Kokkos v. 4.0
//       Copyright (2022) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Part of Kokkos, under the Apache License v2.0 with LLVM Exceptions.
// See https://kokkos.org/LICENSE for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//@HEADER
#include <mdspan/mdspan.hpp>
#include <vector>

#include <gtest/gtest.h>

#include <utility>

#include "offload_utils.hpp"
#include "foo_customizations.hpp"

namespace KokkosEx = MDSPAN_IMPL_STANDARD_NAMESPACE::MDSPAN_IMPL_PROPOSED_NAMESPACE;

_MDSPAN_INLINE_VARIABLE constexpr auto dyn = Kokkos::dynamic_extent;

TEST(TestSubmdspanLayoutRightStaticSizedRankReducing3Dto1D, test_submdspan_layout_right_static_sized_rank_reducing_3d_to_1d) {
  std::vector<int> d(2 * 3 * 4, 0);
  Kokkos::mdspan<int, Kokkos::extents<size_t,2, 3, 4>> m(d.data());
  __MDSPAN_OP(m, 1, 1, 1) = 42;
  auto sub0 = KokkosEx::submdspan(m, 1, 1, Kokkos::full_extent);
  static_assert(decltype(sub0)::rank()==1,"unexpected submdspan rank");
  ASSERT_EQ(sub0.rank(),         1);
  ASSERT_EQ(sub0.rank_dynamic(), 0);
  ASSERT_EQ(sub0.extent(0),      4);
  ASSERT_EQ((__MDSPAN_OP(sub0, 1)), 42);
}

TEST(TestSubmdspanLayoutLeftStaticSizedRankReducing3Dto1D, test_submdspan_layout_left_static_sized_rank_reducing_3d_to_1d) {
  std::vector<int> d(2 * 3 * 4, 0);
  Kokkos::mdspan<int, Kokkos::extents<size_t,2, 3, 4>, Kokkos::layout_left> m(d.data());
  __MDSPAN_OP(m, 1, 1, 1) = 42;
  auto sub0 = KokkosEx::submdspan(m, 1, 1, Kokkos::full_extent);
  ASSERT_EQ(sub0.rank(),         1);
  ASSERT_EQ(sub0.rank_dynamic(), 0);
  ASSERT_EQ(sub0.extent(0),      4);
  ASSERT_EQ((__MDSPAN_OP(sub0, 1)), 42);
}

TEST(TestSubmdspanLayoutRightStaticSizedRankReducingNested3Dto0D, test_submdspan_layout_right_static_sized_rank_reducing_nested_3d_to_0d) {
  std::vector<int> d(2 * 3 * 4, 0);
  Kokkos::mdspan<int, Kokkos::extents<size_t,2, 3, 4>> m(d.data());
  __MDSPAN_OP(m, 1, 1, 1) = 42;
  auto sub0 = KokkosEx::submdspan(m, 1, Kokkos::full_extent, Kokkos::full_extent);
  ASSERT_EQ(sub0.rank(),         2);
  ASSERT_EQ(sub0.rank_dynamic(), 0);
  ASSERT_EQ(sub0.extent(0),      3);
  ASSERT_EQ(sub0.extent(1),      4);
  ASSERT_EQ((__MDSPAN_OP(sub0, 1, 1)), 42);
  auto sub1 = KokkosEx::submdspan(sub0, 1, Kokkos::full_extent);
  ASSERT_EQ(sub1.rank(),         1);
  ASSERT_EQ(sub1.rank_dynamic(), 0);
  ASSERT_EQ(sub1.extent(0),      4);
  ASSERT_EQ((__MDSPAN_OP(sub1,1)),42);
  auto sub2 = KokkosEx::submdspan(sub1, 1);
  ASSERT_EQ(sub2.rank(),         0);
  ASSERT_EQ(sub2.rank_dynamic(), 0);
  ASSERT_EQ((__MDSPAN_OP0(sub2)), 42);
}

TEST(TestSubmdspanLayoutRightStaticSizedPairs, test_submdspan_layout_right_static_sized_pairs) {
  std::vector<int> d(2 * 3 * 4, 0);
  Kokkos::mdspan<int, Kokkos::extents<size_t,2, 3, 4>> m(d.data());
  __MDSPAN_OP(m, 1, 1, 1) = 42;
  auto sub0 = KokkosEx::submdspan(m, std::pair<int,int>{1, 2}, std::pair<int,int>{1, 3}, std::pair<int,int>{1, 4});
  ASSERT_EQ(sub0.rank(),         3);
  ASSERT_EQ(sub0.rank_dynamic(), 3);
  ASSERT_EQ(sub0.extent(0),      1);
  ASSERT_EQ(sub0.extent(1),      2);
  ASSERT_EQ(sub0.extent(2),      3);
  ASSERT_EQ((__MDSPAN_OP(sub0, 0, 0, 0)), 42);
}

TEST(TestSubmdspanLayoutRightStaticSizedTuples, test_submdspan_layout_right_static_sized_tuples) {
  std::vector<int> d(2 * 3 * 4, 0);
  Kokkos::mdspan<int, Kokkos::extents<size_t,2, 3, 4>> m(d.data());
  __MDSPAN_OP(m, 1, 1, 1) = 42;
  auto sub0 = KokkosEx::submdspan(m, std::tuple<int,int>{1, 2}, std::tuple<int,int>{1, 3}, std::tuple<int,int>{1, 4});
  ASSERT_EQ(sub0.rank(),         3);
  ASSERT_EQ(sub0.rank_dynamic(), 3);
  ASSERT_EQ(sub0.extent(0),      1);
  ASSERT_EQ(sub0.extent(1),      2);
  ASSERT_EQ(sub0.extent(2),      3);
  ASSERT_EQ((__MDSPAN_OP(sub0, 0, 0, 0)),       42);
}


//template<class LayoutOrg, class LayoutSub, class ExtentsOrg, class ExtentsSub, class ... SubArgs>


template<size_t ... Args>
using args_t = std::index_sequence<Args...>;

using submdspan_test_types =
  ::testing::Types<
      // LayoutLeft to LayoutLeft
      std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,1>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,1>, std::pair<int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,0>, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, std::pair<int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,1>, Kokkos::full_extent_t, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,3>, Kokkos::full_extent_t, Kokkos::full_extent_t, std::pair<int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, std::pair<int,int>, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,1>, Kokkos::full_extent_t, int, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,1>, std::pair<int,int>, int, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,3>, Kokkos::full_extent_t, Kokkos::full_extent_t, std::pair<int,int>, int, int, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, std::pair<int,int>, int, int, int, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,1>, Kokkos::full_extent_t, int, int, int ,int, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_left, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,1>, std::pair<int,int>, int, int, int, int, int>
    // LayoutRight to LayoutRight
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,1>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,1>, std::pair<int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,0>, int>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,2>, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,1>, int, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,3>, std::pair<int,int>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,2>, int, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,3>, args_t<10,20,30>,    Kokkos::dextents<size_t,1>, int, int, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,3>, int, int, int, std::pair<int,int>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,2>, int, int, int, int, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::dextents<size_t,6>, args_t<6,4,5,6,7,8>, Kokkos::dextents<size_t,1>, int, int, int, int, int, Kokkos::full_extent_t>
    // LayoutRight to LayoutRight Check Extents Preservation
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10>,           args_t<10>,          Kokkos::extents<size_t,10>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10>,           args_t<10>,          Kokkos::extents<size_t,dyn>, std::pair<int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10>,           args_t<10>,          Kokkos::extents<size_t>, int>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10,20>,        args_t<10,20>,       Kokkos::extents<size_t,10,20>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10,20>,        args_t<10,20>,       Kokkos::extents<size_t,dyn,20>, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10,20>,        args_t<10,20>,       Kokkos::extents<size_t,20>, int, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10,20,30>,     args_t<10,20,30>,    Kokkos::extents<size_t,dyn,20,30>, std::pair<int,int>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10,20,30>,     args_t<10,20,30>,    Kokkos::extents<size_t,dyn,30>, int, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,10,20,30>,     args_t<10,20,30>,    Kokkos::extents<size_t,30>, int, int, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,6,4,5,6,7,8>,  args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,dyn,7,8>, int, int, int, std::pair<int,int>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,6,4,5,6,7,8>,  args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,dyn,8>, int, int, int, int, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_right, Kokkos::extents<size_t,6,4,5,6,7,8>,  args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,8>, int, int, int, int, int, Kokkos::full_extent_t>
    // LayoutLeft to LayoutStride
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::dextents<size_t,1>,          args_t<10>,          Kokkos::dextents<size_t,1>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,1>, KokkosEx::strided_slice<int,int,int>, int>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, std::pair<int,int>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, KokkosEx::strided_slice<int,int,int>, std::pair<int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, KokkosEx::strided_slice<int,int,int>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::extents<size_t,6,4,5,6,7,8>, args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,6,dyn,8>, Kokkos::full_extent_t, int, std::pair<int,int>, int, int, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_left, Kokkos::layout_stride,  Kokkos::extents<size_t,6,4,5,6,7,8>, args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,4,dyn,7>, int, Kokkos::full_extent_t, std::pair<int,int>, int, Kokkos::full_extent_t, int>
    // layout_right to layout_stride
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,1>,          args_t<10>,          Kokkos::dextents<size_t,1>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,1>,          args_t<10>,          Kokkos::extents<size_t,0>,  KokkosEx::strided_slice<int,std::integral_constant<int,0>,std::integral_constant<int,0>>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,1>, KokkosEx::strided_slice<int,int,int>, int>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, std::pair<int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, std::pair<int,int>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, KokkosEx::strided_slice<int,int,int>, std::pair<int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,2>, KokkosEx::strided_slice<int,int,int>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::extents<size_t,6,4,5,6,7,8>, args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,6,dyn,8>, Kokkos::full_extent_t, int, std::pair<int,int>, int, int, Kokkos::full_extent_t>
    , std::tuple<Kokkos::layout_right, Kokkos::layout_stride, Kokkos::extents<size_t,6,4,5,6,7,8>, args_t<6,4,5,6,7,8>, Kokkos::extents<size_t,4,dyn,7>, int, Kokkos::full_extent_t, std::pair<int,int>, int, Kokkos::full_extent_t, int>
    // Testing of customization point design
    , std::tuple<Foo::layout_foo, Foo::layout_foo, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,1>, Kokkos::full_extent_t>
    , std::tuple<Foo::layout_foo, Foo::layout_foo, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,1>, std::pair<int,int>>
    , std::tuple<Foo::layout_foo, Foo::layout_foo, Kokkos::dextents<size_t,1>, args_t<10>,          Kokkos::dextents<size_t,0>, int>
    , std::tuple<Foo::layout_foo, Foo::layout_foo, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,2>, Kokkos::full_extent_t, Kokkos::full_extent_t>
    , std::tuple<Foo::layout_foo, Foo::layout_foo, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,2>, std::pair<int,int>, Kokkos::full_extent_t>
    , std::tuple<Foo::layout_foo, Foo::layout_foo, Kokkos::dextents<size_t,2>, args_t<10,20>,       Kokkos::dextents<size_t,1>, int, Kokkos::full_extent_t>
    , std::tuple<Foo::layout_foo, Kokkos::layout_stride, Kokkos::dextents<size_t,1>,          args_t<10>,          Kokkos::dextents<size_t,1>, KokkosEx::strided_slice<int,int,int>>
    , std::tuple<Foo::layout_foo, Kokkos::layout_stride, Kokkos::dextents<size_t,2>,          args_t<10,20>,       Kokkos::dextents<size_t,1>, KokkosEx::strided_slice<int,int,int>, int>
    >;

template<class T> struct TestSubMDSpan;

template<class LayoutOrg, class LayoutSub, class ExtentsOrg, class ExtentsSub, class ... SubArgs, size_t ... ConstrArgs>
struct TestSubMDSpan<
  std::tuple<LayoutOrg,
             LayoutSub,
             ExtentsOrg,
	     std::index_sequence<ConstrArgs...>,
             ExtentsSub,
             SubArgs...>>
  : public ::testing::Test {

  using mds_org_t = Kokkos::mdspan<int, ExtentsOrg, LayoutOrg>;
  using mds_sub_t = Kokkos::mdspan<int, ExtentsSub, LayoutSub>;
  using map_t = typename mds_org_t::mapping_type;

  using mds_sub_deduced_t = decltype(KokkosEx::submdspan(mds_org_t(nullptr, map_t()), SubArgs()...));
  using sub_args_t = std::tuple<SubArgs...>;

  MDSPAN_INLINE_FUNCTION
  static int create_slice_arg(int) {
    return 2;
  }
  MDSPAN_INLINE_FUNCTION
  static auto create_slice_arg(std::pair<int,int>) {
    return std::pair<int,int>(1,3);
  }
  MDSPAN_INLINE_FUNCTION
  static auto create_slice_arg(KokkosEx::strided_slice<int,int,int>) {
    return KokkosEx::strided_slice<int,int,int>{1,3,2};
  }
  template<int Ext, int Stride>
  MDSPAN_INLINE_FUNCTION
  static auto create_slice_arg(KokkosEx::strided_slice<int,std::integral_constant<int, Ext>, std::integral_constant<int, Stride>>) {
    return KokkosEx::strided_slice<int,std::integral_constant<int, Ext>, std::integral_constant<int, Stride>>{1,std::integral_constant<int, Ext>(), std::integral_constant<int, Ext>()};
  }
  MDSPAN_INLINE_FUNCTION
  static auto create_slice_arg(Kokkos::full_extent_t) {
    return Kokkos::full_extent;
  }

  template<class SrcExtents, class SubExtents, class ... SliceArgs>
  MDSPAN_INLINE_FUNCTION
  static bool match_expected_extents(int src_idx, int sub_idx, SrcExtents src_ext, SubExtents sub_ext, int, SliceArgs ... slices) {
    return match_expected_extents(++src_idx, sub_idx, src_ext, sub_ext, slices...);
  }
  template<class SrcExtents, class SubExtents, class ... SliceArgs>
  MDSPAN_INLINE_FUNCTION
  static bool match_expected_extents(int src_idx, int sub_idx, SrcExtents src_ext, SubExtents sub_ext, std::pair<int,int> p, SliceArgs ... slices) {
    using idx_t = typename SubExtents::index_type;
    return (sub_ext.extent(sub_idx)==static_cast<idx_t>(p.second-p.first)) && match_expected_extents(++src_idx, ++sub_idx, src_ext, sub_ext, slices...);
  }
  template<class SrcExtents, class SubExtents, class ... SliceArgs>
  MDSPAN_INLINE_FUNCTION
  static bool match_expected_extents(int src_idx, int sub_idx, SrcExtents src_ext, SubExtents sub_ext,
                                     KokkosEx::strided_slice<int,int,int> p, SliceArgs ... slices) {
    using idx_t = typename SubExtents::index_type;
    return (sub_ext.extent(sub_idx)==static_cast<idx_t>((p.extent+p.stride-1)/p.stride)) && match_expected_extents(++src_idx, ++sub_idx, src_ext, sub_ext, slices...);
  }
  template<class SrcExtents, class SubExtents, class ... SliceArgs>
  MDSPAN_INLINE_FUNCTION
  static bool match_expected_extents(int src_idx, int sub_idx, SrcExtents src_ext, SubExtents sub_ext,
                                     KokkosEx::strided_slice<int,std::integral_constant<int, 0>,std::integral_constant<int,0>>, SliceArgs ... slices) {
    return (sub_ext.extent(sub_idx)==0) && match_expected_extents(++src_idx, ++sub_idx, src_ext, sub_ext, slices...);
  }
  template<class SrcExtents, class SubExtents, class ... SliceArgs>
  MDSPAN_INLINE_FUNCTION
  static bool match_expected_extents(int src_idx, int sub_idx, SrcExtents src_ext, SubExtents sub_ext, Kokkos::full_extent_t, SliceArgs ... slices) {
    return (sub_ext.extent(sub_idx)==src_ext.extent(src_idx)) && match_expected_extents(++src_idx, ++sub_idx, src_ext, sub_ext, slices...);
  }
  template<class SrcExtents, class SubExtents>
  MDSPAN_INLINE_FUNCTION
  static bool match_expected_extents(int, int, SrcExtents, SubExtents) { return true; }

  static void run() {
    typename mds_org_t::mapping_type map(typename mds_org_t::extents_type(ConstrArgs...));
    int data[25000];
    mds_org_t src(data, map);
    size_t* result = allocate_array<size_t>(1);

    dispatch([=] _MDSPAN_HOST_DEVICE () {
      auto sub = KokkosEx::submdspan(src, create_slice_arg(SubArgs())...);
      bool match = match_expected_extents(0, 0, src.extents(), sub.extents(), create_slice_arg(SubArgs())...);
      result[0] = match?1:0;
    });
    EXPECT_EQ(result[0], 1);
    free_array(result);
  }

};


TYPED_TEST_SUITE(TestSubMDSpan, submdspan_test_types);

TYPED_TEST(TestSubMDSpan, submdspan_return_type) {
  static_assert(std::is_same<typename TestFixture::mds_sub_t,
                             typename TestFixture::mds_sub_deduced_t>::value,
                "SubMDSpan: wrong return type");
  __MDSPAN_TESTS_RUN_TEST(TestFixture::run());
}
