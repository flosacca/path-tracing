#pragma once
#include <type_traits>

#if __cplusplus < 201703L
#define static_assert(v, ...) static_assert(v, "" __VA_ARGS__)
#endif

static_assert(__cplusplus >= 201402L);

namespace meta {
using size_t = std::size_t;

// const-volatile modifications:
template <class T>
using remove_const = typename std::remove_const<T>::type;
template <class T>
using remove_volatile = typename std::remove_volatile<T>::type;
template <class T>
using remove_cv = typename std::remove_cv<T>::type;
template <class T>
using add_const = typename std::add_const<T>::type;
template <class T>
using add_volatile = typename std::add_volatile<T>::type;
template <class T>
using add_cv = typename std::add_cv<T>::type;

// reference modifications:
template <class T>
using remove_reference = typename std::remove_reference<T>::type;
template <class T>
using add_lvalue_reference = typename std::add_lvalue_reference<T>::type;
template <class T>
using add_rvalue_reference = typename std::add_rvalue_reference<T>::type;

// sign modifications:
template <class T>
using make_signed = typename std::make_signed<T>::type;
template <class T>
using make_unsigned = typename std::make_unsigned<T>::type;

// array modifications:
template <class T>
using remove_extent = typename std::remove_extent<T>::type;
template <class T>
using remove_all_extents = typename std::remove_all_extents<T>::type;

// pointer modifications:
template <class T>
using remove_pointer = typename std::remove_pointer<T>::type;
template <class T>
using add_pointer = typename std::add_pointer<T>::type;

// other transformations:
template <class T>
using decay = typename std::decay<T>::type;
template <bool b, class T = void>
using enable_if = typename std::enable_if<b, T>::type;
template <bool b, class T, class F>
using conditional = typename std::conditional<b, T, F>::type;
template <class... T>
using common_type = typename std::common_type<T...>::type;
template <class T>
using underlying_type = typename std::underlying_type<T>::type;

// primary type categories:
template <class T>
constexpr bool is_void = std::is_void<T>::value;
template <class T>
constexpr bool is_null_pointer = std::is_null_pointer<T>::value;
template <class T>
constexpr bool is_integral = std::is_integral<T>::value;
template <class T>
constexpr bool is_floating_point = std::is_floating_point<T>::value;
template <class T>
constexpr bool is_array = std::is_array<T>::value;
template <class T>
constexpr bool is_pointer = std::is_pointer<T>::value;
template <class T>
constexpr bool is_lvalue_reference = std::is_lvalue_reference<T>::value;
template <class T>
constexpr bool is_rvalue_reference = std::is_rvalue_reference<T>::value;
template <class T>
constexpr bool is_member_object_pointer = std::is_member_object_pointer<T>::value;
template <class T>
constexpr bool is_member_function_pointer = std::is_member_function_pointer<T>::value;
template <class T>
constexpr bool is_enum = std::is_enum<T>::value;
template <class T>
constexpr bool is_union = std::is_union<T>::value;
template <class T>
constexpr bool is_class = std::is_class<T>::value;
template <class T>
constexpr bool is_function = std::is_function<T>::value;

// composite type categories:
template <class T>
constexpr bool is_reference = std::is_reference<T>::value;
template <class T>
constexpr bool is_arithmetic = std::is_arithmetic<T>::value;
template <class T>
constexpr bool is_fundamental = std::is_fundamental<T>::value;
template <class T>
constexpr bool is_object = std::is_object<T>::value;
template <class T>
constexpr bool is_scalar = std::is_scalar<T>::value;
template <class T>
constexpr bool is_compound = std::is_compound<T>::value;
template <class T>
constexpr bool is_member_pointer = std::is_member_pointer<T>::value;

// type properties:
template <class T>
constexpr bool is_const = std::is_const<T>::value;
template <class T>
constexpr bool is_volatile = std::is_volatile<T>::value;
template <class T>
constexpr bool is_trivial = std::is_trivial<T>::value;
template <class T>
constexpr bool is_trivially_copyable = std::is_trivially_copyable<T>::value;
template <class T>
constexpr bool is_standard_layout = std::is_standard_layout<T>::value;
template <class T>
constexpr bool is_empty = std::is_empty<T>::value;
template <class T>
constexpr bool is_polymorphic = std::is_polymorphic<T>::value;
template <class T>
constexpr bool is_abstract = std::is_abstract<T>::value;
template <class T>
constexpr bool is_final = std::is_final<T>::value;
template <class T>
constexpr bool is_signed = std::is_signed<T>::value;
template <class T>
constexpr bool is_unsigned = std::is_unsigned<T>::value;
template <class T, class... Args>
constexpr bool is_constructible = std::is_constructible<T, Args...>::value;
template <class T>
constexpr bool is_default_constructible = std::is_default_constructible<T>::value;
template <class T>
constexpr bool is_copy_constructible = std::is_copy_constructible<T>::value;
template <class T>
constexpr bool is_move_constructible = std::is_move_constructible<T>::value;
template <class T, class U>
constexpr bool is_assignable = std::is_assignable<T, U>::value;
template <class T>
constexpr bool is_copy_assignable = std::is_copy_assignable<T>::value;
template <class T>
constexpr bool is_move_assignable = std::is_move_assignable<T>::value;
template <class T>
constexpr bool is_destructible = std::is_destructible<T>::value;
template <class T, class... Args>
constexpr bool is_trivially_constructible = std::is_trivially_constructible<T, Args...>::value;
template <class T>
constexpr bool is_trivially_default_constructible = std::is_trivially_default_constructible<T>::value;
template <class T>
constexpr bool is_trivially_copy_constructible = std::is_trivially_copy_constructible<T>::value;
template <class T>
constexpr bool is_trivially_move_constructible = std::is_trivially_move_constructible<T>::value;
template <class T, class U>
constexpr bool is_trivially_assignable = std::is_trivially_assignable<T, U>::value;
template <class T>
constexpr bool is_trivially_copy_assignable = std::is_trivially_copy_assignable<T>::value;
template <class T>
constexpr bool is_trivially_move_assignable = std::is_trivially_move_assignable<T>::value;
template <class T>
constexpr bool is_trivially_destructible = std::is_trivially_destructible<T>::value;
template <class T, class... Args>
constexpr bool is_nothrow_constructible = std::is_nothrow_constructible<T, Args...>::value;
template <class T>
constexpr bool is_nothrow_default_constructible = std::is_nothrow_default_constructible<T>::value;
template <class T>
constexpr bool is_nothrow_copy_constructible = std::is_nothrow_copy_constructible<T>::value;
template <class T>
constexpr bool is_nothrow_move_constructible = std::is_nothrow_move_constructible<T>::value;
template <class T, class U>
constexpr bool is_nothrow_assignable = std::is_nothrow_assignable<T, U>::value;
template <class T>
constexpr bool is_nothrow_copy_assignable = std::is_nothrow_copy_assignable<T>::value;
template <class T>
constexpr bool is_nothrow_move_assignable = std::is_nothrow_move_assignable<T>::value;
template <class T>
constexpr bool is_nothrow_destructible = std::is_nothrow_destructible<T>::value;
template <class T>
constexpr bool has_virtual_destructor = std::has_virtual_destructor<T>::value;

// type property queries:
template <class T>
constexpr size_t alignment_of = std::alignment_of<T>::value;
template <class T>
constexpr size_t rank = std::rank<T>::value;
template <class T, unsigned I = 0>
constexpr size_t extent = std::extent<T, I>::value;

// type relations:
template <class T, class U>
constexpr bool is_same = std::is_same<T, U>::value;
template <class Base, class Derived>
constexpr bool is_base_of = std::is_base_of<Base, Derived>::value;
template <class From, class To>
constexpr bool is_convertible = std::is_convertible<From, To>::value;
} // namespace meta
