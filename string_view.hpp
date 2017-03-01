// -*- C++ -*-
//===------------------------ string_view ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// This is a clone of the libcxx std::experimental::string_view (on <experimental/string_view>)
// sighly edited (by Denilson das Merces Amorim <dma_2012@hotmail.com>) to be independent of the
// libcxx library stack, so this can be used as a library on other compilers.
// This should be useful/fun until all the compilers have a string_view implementation.

#ifndef _LIBCPP_STRVIEWCLONE_LFTS_STRING_VIEW
#define _LIBCPP_STRVIEWCLONE_LFTS_STRING_VIEW

// Some LIBCPP macros left here because they are somewhat useful.
// (renamed to LIBCPP_STRVIEWCLONE, 'course)
#include <cassert>
#define __LIBCPP_STRVIEWCLONE_throw(ex) throw (ex)              // Might undefine this if -fno-exceptions? Dunno.
#define _LIBCPP_STRVIEWCLONE_ASSERT(x, m) assert(x && m)        // Translate libcxx assertion to C assert
#if __cplusplus < 201402L
#   define _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11
#else
#   define _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 constexpr
#endif

/*
string_view synopsis
namespace std {
 namespace experimental {
  inline namespace library_fundamentals_v1 {
    // 7.2, Class template basic_string_view
    template<class charT, class traits = char_traits<charT>>
        class basic_string_view;
    // 7.9, basic_string_view non-member comparison functions
    template<class charT, class traits>
    constexpr bool operator==(basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator!=(basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator< (basic_string_view<charT, traits> x,
                                 basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator> (basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator<=(basic_string_view<charT, traits> x,
                                 basic_string_view<charT, traits> y) noexcept;
    template<class charT, class traits>
    constexpr bool operator>=(basic_string_view<charT, traits> x,
                              basic_string_view<charT, traits> y) noexcept;
    // see below, sufficient additional overloads of comparison functions
    // 7.10, Inserters and extractors
    template<class charT, class traits>
      basic_ostream<charT, traits>&
        operator<<(basic_ostream<charT, traits>& os,
                   basic_string_view<charT, traits> str);
    // basic_string_view typedef names
    typedef basic_string_view<char> string_view;
    typedef basic_string_view<char16_t> u16string_view;
    typedef basic_string_view<char32_t> u32string_view;
    typedef basic_string_view<wchar_t> wstring_view;
    template<class charT, class traits = char_traits<charT>>
    class basic_string_view {
      public:
      // types
      typedef traits traits_type;
      typedef charT value_type;
      typedef charT* pointer;
      typedef const charT* const_pointer;
      typedef charT& reference;
      typedef const charT& const_reference;
      typedef implementation-defined const_iterator;
      typedef const_iterator iterator;
      typedef reverse_iterator<const_iterator> const_reverse_iterator;
      typedef const_reverse_iterator reverse_iterator;
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      static constexpr size_type npos = size_type(-1);
      // 7.3, basic_string_view constructors and assignment operators
      constexpr basic_string_view() noexcept;
      constexpr basic_string_view(const basic_string_view&) noexcept = default;
      basic_string_view& operator=(const basic_string_view&) noexcept = default;
      template<class Allocator>
      basic_string_view(const std::basic_string<charT, traits, Allocator>& str) noexcept;
      constexpr basic_string_view(const charT* str);
      constexpr basic_string_view(const charT* str, size_type len);
      // 7.4, basic_string_view iterator support
      constexpr const_iterator begin() const noexcept;
      constexpr const_iterator end() const noexcept;
      constexpr const_iterator cbegin() const noexcept;
      constexpr const_iterator cend() const noexcept;
      const_reverse_iterator rbegin() const noexcept;
      const_reverse_iterator rend() const noexcept;
      const_reverse_iterator crbegin() const noexcept;
      const_reverse_iterator crend() const noexcept;
      // 7.5, basic_string_view capacity
      constexpr size_type size() const noexcept;
      constexpr size_type length() const noexcept;
      constexpr size_type max_size() const noexcept;
      constexpr bool empty() const noexcept;
      // 7.6, basic_string_view element access
      constexpr const_reference operator[](size_type pos) const;
      constexpr const_reference at(size_type pos) const;
      constexpr const_reference front() const;
      constexpr const_reference back() const;
      constexpr const_pointer data() const noexcept;
      // 7.7, basic_string_view modifiers
      constexpr void clear() noexcept;
      constexpr void remove_prefix(size_type n);
      constexpr void remove_suffix(size_type n);
      constexpr void swap(basic_string_view& s) noexcept;
      // 7.8, basic_string_view string operations
      template<class Allocator>
      explicit operator std::basic_string<charT, traits, Allocator>() const;
      template<class Allocator = allocator<charT>>
      std::basic_string<charT, traits, Allocator> to_string(
        const Allocator& a = Allocator()) const;
      size_type copy(charT* s, size_type n, size_type pos = 0) const;
      constexpr basic_string_view substr(size_type pos = 0, size_type n = npos) const;
      constexpr int compare(basic_string_view s) const noexcept;
      constexpr int compare(size_type pos1, size_type n1, basic_string_view s) const;
      constexpr int compare(size_type pos1, size_type n1,
                            basic_string_view s, size_type pos2, size_type n2) const;
      constexpr int compare(const charT* s) const;
      constexpr int compare(size_type pos1, size_type n1, const charT* s) const;
      constexpr int compare(size_type pos1, size_type n1,
                            const charT* s, size_type n2) const;
      constexpr size_type find(basic_string_view s, size_type pos = 0) const noexcept;
      constexpr size_type find(charT c, size_type pos = 0) const noexcept;
      constexpr size_type find(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find(const charT* s, size_type pos = 0) const;
      constexpr size_type rfind(basic_string_view s, size_type pos = npos) const noexcept;
      constexpr size_type rfind(charT c, size_type pos = npos) const noexcept;
      constexpr size_type rfind(const charT* s, size_type pos, size_type n) const;
      constexpr size_type rfind(const charT* s, size_type pos = npos) const;
      constexpr size_type find_first_of(basic_string_view s, size_type pos = 0) const noexcept;
      constexpr size_type find_first_of(charT c, size_type pos = 0) const noexcept;
      constexpr size_type find_first_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_first_of(const charT* s, size_type pos = 0) const;
      constexpr size_type find_last_of(basic_string_view s, size_type pos = npos) const noexcept;
      constexpr size_type find_last_of(charT c, size_type pos = npos) const noexcept;
      constexpr size_type find_last_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_last_of(const charT* s, size_type pos = npos) const;
      constexpr size_type find_first_not_of(basic_string_view s, size_type pos = 0) const noexcept;
      constexpr size_type find_first_not_of(charT c, size_type pos = 0) const noexcept;
      constexpr size_type find_first_not_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_first_not_of(const charT* s, size_type pos = 0) const;
      constexpr size_type find_last_not_of(basic_string_view s, size_type pos = npos) const noexcept;
      constexpr size_type find_last_not_of(charT c, size_type pos = npos) const noexcept;
      constexpr size_type find_last_not_of(const charT* s, size_type pos, size_type n) const;
      constexpr size_type find_last_not_of(const charT* s, size_type pos = npos) const;
     private:
      const_pointer data_;  // exposition only
      size_type     size_;  // exposition only
    };
  }  // namespace fundamentals_v1
 }  // namespace experimental
  // 7.11, Hash support
  template <class T> struct hash;
  template <> struct hash<experimental::string_view>;
  template <> struct hash<experimental::u16string_view>;
  template <> struct hash<experimental::u32string_view>;
  template <> struct hash<experimental::wstring_view>;
}  // namespace std
*/

#include <string>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <iomanip>
#include <cstddef>

namespace libcxx_strviewclone {

    template<class _CharT, class _Traits = std::char_traits<_CharT> >
    class  basic_string_view {
    public:
        // types
        typedef _Traits                                    traits_type;
        typedef _CharT                                     value_type;
        typedef const _CharT*                              pointer;
        typedef const _CharT*                              const_pointer;
        typedef const _CharT&                              reference;
        typedef const _CharT&                              const_reference;
        typedef const_pointer                              const_iterator; // See [string.view.iterators]
        typedef const_iterator                             iterator;
        typedef std::reverse_iterator<const_iterator>    const_reverse_iterator;
        typedef const_reverse_iterator                     reverse_iterator;
        typedef size_t                                     size_type;
        typedef ptrdiff_t                                  difference_type;
        static constexpr const size_type npos = -1; // size_type(-1);

        // [string.view.cons], construct/copy
        constexpr inline
        basic_string_view() noexcept : __data (nullptr), __size(0) {}

        constexpr inline
        basic_string_view(const basic_string_view&) noexcept = default;

        inline
        basic_string_view& operator=(const basic_string_view&) noexcept = default;

        template<class _Allocator>
        inline
        basic_string_view(const std::basic_string<_CharT, _Traits, _Allocator>& __str) noexcept
            : __data (__str.data()), __size(__str.size()) {}

        constexpr inline
        basic_string_view(const _CharT* __s, size_type __len)
            : __data(__s), __size(__len)
        {
//             _LIBCPP_STRVIEWCLONE_ASSERT(__len == 0 || __s != nullptr, "string_view::string_view(_CharT *, size_t): received nullptr");
        }

        constexpr inline
        basic_string_view(const _CharT* __s)
            : __data(__s), __size(_Traits::length(__s)) {}

        // [string.view.iterators], iterators
        constexpr inline
        const_iterator begin()  const noexcept { return cbegin(); }

        constexpr inline
        const_iterator end()    const noexcept { return cend(); }

        constexpr inline
        const_iterator cbegin() const noexcept { return __data; }

        constexpr inline
        const_iterator cend()   const noexcept { return __data + __size; }

        inline
        const_reverse_iterator rbegin()   const noexcept { return const_reverse_iterator(cend()); }

        inline
        const_reverse_iterator rend()     const noexcept { return const_reverse_iterator(cbegin()); }

        inline
        const_reverse_iterator crbegin()  const noexcept { return const_reverse_iterator(cend()); }

        inline
        const_reverse_iterator crend()    const noexcept { return const_reverse_iterator(cbegin()); }

        // [string.view.capacity], capacity
        constexpr inline
        size_type size()     const noexcept { return __size; }

        constexpr inline
        size_type length()   const noexcept { return __size; }

        constexpr inline
        size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }

        constexpr bool inline
        empty()         const noexcept { return __size == 0; }

        // [string.view.access], element access
        constexpr inline
        const_reference operator[](size_type __pos) const { return __data[__pos]; }

        constexpr inline
        const_reference at(size_type __pos) const
        {
            return __pos >= size()
                ? (__LIBCPP_STRVIEWCLONE_throw(std::out_of_range("string_view::at")), __data[0])
                : __data[__pos];
        }

        constexpr inline
        const_reference front() const
        {
            return _LIBCPP_STRVIEWCLONE_ASSERT(!empty(), "string_view::front(): string is empty"), __data[0];
        }

        constexpr inline
        const_reference back() const
        {
            return _LIBCPP_STRVIEWCLONE_ASSERT(!empty(), "string_view::back(): string is empty"), __data[__size-1];
        }

        constexpr inline
        const_pointer data() const noexcept { return __data; }

        // [string.view.modifiers], modifiers:
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        void clear() noexcept
        {
            __data = nullptr;
            __size = 0;
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        void remove_prefix(size_type __n) noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n <= size(), "remove_prefix() can't remove more than size()");
            __data += __n;
            __size -= __n;
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        void remove_suffix(size_type __n) noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n <= size(), "remove_suffix() can't remove more than size()");
            __size -= __n;
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        void swap(basic_string_view& __other) noexcept
        {
            const value_type *__p = __data;
            __data = __other.__data;
            __other.__data = __p;

            size_type __sz = __size;
            __size = __other.__size;
            __other.__size = __sz;
//             std::swap( __data, __other.__data );
//             std::swap( __size, __other.__size );
        }

        // [string.view.ops], string operations:
        template<class _Allocator>
        inline
        explicit operator std::basic_string<_CharT, _Traits, _Allocator>() const
        { return std::basic_string<_CharT, _Traits, _Allocator>( begin(), end()); }

        template<class _Allocator = std::allocator<_CharT> >
        inline
        std::basic_string<_CharT, _Traits, _Allocator>
        to_string( const _Allocator& __a = _Allocator()) const
        { return std::basic_string<_CharT, _Traits, _Allocator> ( begin(), end(), __a ); }

        size_type copy(_CharT* __s, size_type __n, size_type __pos = 0) const
        {
            if ( __pos > size())
                __LIBCPP_STRVIEWCLONE_throw(std::out_of_range("string_view::copy"));
            size_type __rlen = std::min( __n, size() - __pos );
            std::copy_n(begin() + __pos, __rlen, __s );
            return __rlen;
        }

        constexpr
        basic_string_view substr(size_type __pos = 0, size_type __n = npos) const
        {
//             if (__pos > size())
//                 throw out_of_range("string_view::substr");
//             size_type __rlen = std::min( __n, size() - __pos );
//             return basic_string_view(data() + __pos, __rlen);
            return __pos > size()
                ? (__LIBCPP_STRVIEWCLONE_throw((std::out_of_range("string_view::substr"))), basic_string_view())
                : basic_string_view(data() + __pos, std::min(__n, size() - __pos));
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 int compare(basic_string_view __sv) const noexcept
        {
            size_type __rlen = std::min( size(), __sv.size());
            int __retval = _Traits::compare(data(), __sv.data(), __rlen);
            if ( __retval == 0 ) // first __rlen chars matched
                __retval = size() == __sv.size() ? 0 : ( size() < __sv.size() ? -1 : 1 );
            return __retval;
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        int compare(size_type __pos1, size_type __n1, basic_string_view __sv) const
        {
            return substr(__pos1, __n1).compare(__sv);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        int compare(                       size_type __pos1, size_type __n1, 
                    basic_string_view _sv, size_type __pos2, size_type __n2) const
        {
            return substr(__pos1, __n1).compare(_sv.substr(__pos2, __n2));
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        int compare(const _CharT* __s) const
        {
            return compare(basic_string_view(__s));
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        int compare(size_type __pos1, size_type __n1, const _CharT* __s) const
        {
            return substr(__pos1, __n1).compare(basic_string_view(__s));
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        int compare(size_type __pos1, size_type __n1, const _CharT* __s, size_type __n2) const
        {
            return substr(__pos1, __n1).compare(basic_string_view(__s, __n2));
        }

        // find
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find(basic_string_view __s, size_type __pos = 0) const noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find(): received nullptr");
            return this->__str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find(_CharT __c, size_type __pos = 0) const noexcept
        {
            return this->__str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find(const _CharT* __s, size_type __pos, size_type __n) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n == 0 || __s != nullptr, "string_view::find(): received nullptr");
            return this->__str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find(const _CharT* __s, size_type __pos = 0) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s != nullptr, "string_view::find(): received nullptr");
            return this->__str_find<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
        }

        // rfind
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type rfind(basic_string_view __s, size_type __pos = npos) const noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find(): received nullptr");
            return this->__str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type rfind(_CharT __c, size_type __pos = npos) const noexcept
        {
            return this->__str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n == 0 || __s != nullptr, "string_view::rfind(): received nullptr");
            return this->__str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type rfind(const _CharT* __s, size_type __pos=npos) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s != nullptr, "string_view::rfind(): received nullptr");
            return this->__str_rfind<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
        }

        // find_first_of
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_of(basic_string_view __s, size_type __pos = 0) const noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_first_of(): received nullptr");
            return this->__str_find_first_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_of(_CharT __c, size_type __pos = 0) const noexcept
        { return find(__c, __pos); }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n == 0 || __s != nullptr, "string_view::find_first_of(): received nullptr");
            return this->__str_find_first_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_of(const _CharT* __s, size_type __pos=0) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s != nullptr, "string_view::find_first_of(): received nullptr");
            return this->__str_find_first_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
        }

        // find_last_of
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_of(basic_string_view __s, size_type __pos=npos) const noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_last_of(): received nullptr");
            return this->__str_find_last_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_of(_CharT __c, size_type __pos = npos) const noexcept
        { return rfind(__c, __pos); }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n == 0 || __s != nullptr, "string_view::find_last_of(): received nullptr");
            return this->__str_find_last_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_of(const _CharT* __s, size_type __pos=npos) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s != nullptr, "string_view::find_last_of(): received nullptr");
            return this->__str_find_last_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
        }

        // find_first_not_of
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_not_of(basic_string_view __s, size_type __pos=0) const noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_first_not_of(): received nullptr");
            return this->__str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_not_of(_CharT __c, size_type __pos=0) const noexcept
        {
            return this->__str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_not_of(const _CharT* __s, size_type __pos, size_type __n) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n == 0 || __s != nullptr, "string_view::find_first_not_of(): received nullptr");
            return this->__str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_first_not_of(const _CharT* __s, size_type __pos=0) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s != nullptr, "string_view::find_first_not_of(): received nullptr");
            return this->__str_find_first_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
        }

        // find_last_not_of
        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_not_of(basic_string_view __s, size_type __pos=npos) const noexcept
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_last_not_of(): received nullptr");
            return this->__str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s.data(), __pos, __s.size());
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_not_of(_CharT __c, size_type __pos=npos) const noexcept
        {
            return this->__str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __c, __pos);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_not_of(const _CharT* __s, size_type __pos, size_type __n) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__n == 0 || __s != nullptr, "string_view::find_last_not_of(): received nullptr");
            return this->__str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, __n);
        }

        _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        size_type find_last_not_of(const _CharT* __s, size_type __pos=npos) const
        {
            _LIBCPP_STRVIEWCLONE_ASSERT(__s != nullptr, "string_view::find_last_not_of(): received nullptr");
            return this->__str_find_last_not_of<value_type, size_type, traits_type, npos>
                (data(), size(), __s, __pos, traits_type::length(__s));
        }

    private:
        //
        // Some function taken from libcxx internals and put here, as private methods of string_view.
        //

        // __str_find
        template<class /*_CharT*/, class _SizeT, class /*_Traits*/, _SizeT __npos>
        static _SizeT _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        __str_find(const _CharT *__p, _SizeT __sz, 
               const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
        {
            if (__pos > __sz || __sz - __pos < __n)
                return __npos;
            if (__n == 0)
                return __pos;
            const _CharT* __r = 
                std::search(__p + __pos, __p + __sz,
                        __s, __s + __n, _Traits::eq);
            if (__r == __p + __sz)
                return __npos;
            return static_cast<_SizeT>(__r - __p);
        }

        // __str_rfind
        template<class /*_CharT*/, class _SizeT, class /*_Traits*/, _SizeT __npos>
        static _SizeT _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        __str_rfind(const _CharT *__p, _SizeT __sz, 
                const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
        {
            __pos = std::min(__pos, __sz);
            if (__n < __sz - __pos)
                __pos += __n;
            else
                __pos = __sz;
            const _CharT* __r = std::find_end(
                    __p, __p + __pos, __s, __s + __n, _Traits::eq);
            if (__n > 0 && __r == __p + __pos)
                return __npos;
            return static_cast<_SizeT>(__r - __p);
        }

        // __str_find_first_of
        template<class /*_CharT*/, class _SizeT, class /*_Traits*/, _SizeT __npos>
        static _SizeT _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        __str_find_first_of(const _CharT *__p, _SizeT __sz,
                        const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
        {
            if (__pos >= __sz || __n == 0)
                return __npos;
            const _CharT* __r = std::find_first_of
                (__p + __pos, __p + __sz, __s, __s + __n, _Traits::eq );
            if (__r == __p + __sz)
                return __npos;
            return static_cast<_SizeT>(__r - __p);
        }


        // __str_find_last_of
        template<class /*_CharT*/, class _SizeT, class /*_Traits*/, _SizeT __npos>
        static _SizeT _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        __str_find_last_of(const _CharT *__p, _SizeT __sz,
                       const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
            {
            if (__n != 0)
            {
                if (__pos < __sz)
                    ++__pos;
                else
                    __pos = __sz;
                for (const _CharT* __ps = __p + __pos; __ps != __p;)
                {
                    const _CharT* __r = _Traits::find(__s, __n, *--__ps);
                    if (__r)
                        return static_cast<_SizeT>(__ps - __p);
                }
            }
            return __npos;
        }


        // __str_find_first_not_of
        template<class /*_CharT*/, class _SizeT, class /*_Traits*/, _SizeT __npos>
        static _SizeT _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        __str_find_first_not_of(const _CharT *__p, _SizeT __sz,
                            const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
        {
            if (__pos < __sz)
            {
                const _CharT* __pe = __p + __sz;
                for (const _CharT* __ps = __p + __pos; __ps != __pe; ++__ps)
                    if (_Traits::find(__s, __n, *__ps) == 0)
                        return static_cast<_SizeT>(__ps - __p);
            }
            return __npos;
        }


        // __str_find_last_not_of
        template<class /*_CharT*/, class _SizeT, class /*_Traits*/, _SizeT __npos>
        static _SizeT _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
        __str_find_last_not_of(const _CharT *__p, _SizeT __sz,
                           const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
        {
            if (__pos < __sz)
                ++__pos;
            else
                __pos = __sz;
            for (const _CharT* __ps = __p + __pos; __ps != __p;)
                if (_Traits::find(__s, __n, *--__ps) == 0)
                    return static_cast<_SizeT>(__ps - __p);
            return __npos;
        }

    private:
        const   value_type* __data;
        size_type           __size;
    };


    // [string.view.comparison]
    // operator ==
    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator==(basic_string_view<_CharT, _Traits> __lhs,
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        if ( __lhs.size() != __rhs.size()) return false;
        return __lhs.compare(__rhs) == 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator==(basic_string_view<_CharT, _Traits> __lhs,
                    typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
    {
        if ( __lhs.size() != __rhs.size()) return false;
        return __lhs.compare(__rhs) == 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator==(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs, 
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        if ( __lhs.size() != __rhs.size()) return false;
        return __lhs.compare(__rhs) == 0;
    }


    // operator !=
    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator!=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        if ( __lhs.size() != __rhs.size())
            return true;
        return __lhs.compare(__rhs) != 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator!=(basic_string_view<_CharT, _Traits> __lhs,
                    typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
    {
        if ( __lhs.size() != __rhs.size())
            return true;
        return __lhs.compare(__rhs) != 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator!=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs, 
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        if ( __lhs.size() != __rhs.size())
            return true;
        return __lhs.compare(__rhs) != 0;
    }


    // operator <
    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator<(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) < 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator<(basic_string_view<_CharT, _Traits> __lhs,
                    typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
    {
        return __lhs.compare(__rhs) < 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator<(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs, 
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) < 0;
    }


    // operator >
    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator> (basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) > 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator>(basic_string_view<_CharT, _Traits> __lhs,
                    typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
    {
        return __lhs.compare(__rhs) > 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator>(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs, 
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) > 0;
    }


    // operator <=
    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator<=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) <= 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator<=(basic_string_view<_CharT, _Traits> __lhs,
                    typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
    {
        return __lhs.compare(__rhs) <= 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator<=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs, 
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) <= 0;
    }


    // operator >=
    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator>=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) >= 0;
    }


    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator>=(basic_string_view<_CharT, _Traits> __lhs,
                    typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
    {
        return __lhs.compare(__rhs) >= 0;
    }

    template<class _CharT, class _Traits>
    _LIBCPP_STRVIEWCLONE_CONSTEXPR_AFTER_CXX11 inline
    bool operator>=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs, 
                    basic_string_view<_CharT, _Traits> __rhs) noexcept
    {
        return __lhs.compare(__rhs) >= 0;
    }


    // [string.view.io]
    template<class _CharT, class _Traits> inline
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& __os, basic_string_view<_CharT, _Traits> __sv)
    {
        auto& s = __sv;
        copy(s.begin(), s.end(), std::ostream_iterator<char>(__os));
        return __os;
    }

  typedef basic_string_view<char>     string_view;
  typedef basic_string_view<char16_t> u16string_view;
  typedef basic_string_view<char32_t> u32string_view;
  typedef basic_string_view<wchar_t>  wstring_view;

}
namespace std  {

/*
// [string.view.hash]
// Shamelessly stolen from <string>
template<class _CharT, class _Traits>
struct  hash<std::experimental::basic_string_view<_CharT, _Traits> >
    : public unary_function<std::experimental::basic_string_view<_CharT, _Traits>, size_t>
{
    size_t operator()(const std::experimental::basic_string_view<_CharT, _Traits>& __val) const noexcept;
};

template<class _CharT, class _Traits>
size_t
hash<std::experimental::basic_string_view<_CharT, _Traits> >::operator()(
        const std::experimental::basic_string_view<_CharT, _Traits>& __val) const noexcept
{
    return __do_string_hash(__val.data(), __val.data() + __val.size());
}
*/

/*
template <class _CharT, class _Traits>
__quoted_output_proxy<_CharT, const _CharT *, _Traits>
quoted ( std::experimental::basic_string_view <_CharT, _Traits> __sv,
             _CharT __delim = _CharT('"'), _CharT __escape=_CharT('\\'))
{
    return __quoted_output_proxy<_CharT, const _CharT *, _Traits> 
         ( __sv.data(), __sv.data() + __sv.size(), __delim, __escape );
}
*/

}

#endif // _LIBCPP_STRVIEWCLONE_LFTS_STRING_VIEW