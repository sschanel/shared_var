#ifndef _SHARED_VAR_H_INCLUDED_
#define _SHARED_VAR_H_INCLUDED_

/**
 * shared_var
 * 
 * Copyright (c) 2012-2015 Scott Schanel http://github.com/sschanel/shared_var
 * 
 * License: MIT License
 * 
 * A lightweight variant class "shared_var".
 * Assign any value to an shared_var... except:
 * It's not for pointers.  It's for value types, not reference
 * or pointer types.
 *
 * Supports assignment (operator=), comparison (operator==),
 * type checking (is<T>()), and type casting (as<T>());
 * Supports nullptr_t, which is the equivalent of empty.
 *
 * Has special methods for assigning const char *, const wchar_t *.
 * Immutable and shared using std::shared_ptr.
 *
 * No lexicographic comparison operators.
 * No hash function.
 *
 * Not the same as boost::any.  Close though.
 * More analogous to a java Object.
 * 
 */

#include <memory>
#include <type_traits>

template <typename T>
struct enable_if_char : std::enable_if <
   std::is_same<char, T>::value ||
   std::is_same<wchar_t, T>::value, T > {};

template <typename T>
struct enable_if_holdable : std::enable_if <
   !std::is_same<class shared_var, T>::value &&
   !std::is_same<nullptr_t, T>::value &&
   !std::is_pointer<T>::value &&
   !std::is_array<T>::value &&
   !std::is_const<T>::value &&
   !std::is_reference<T>::value, T > {};

class shared_var {

   class holder_base {
   public:
      virtual ~holder_base() {}
      virtual bool equals(const holder_base * rhs) const = 0;
   };

   template <typename T>
   class holder : public holder_base {
   public:
      T value_;

      holder(const T& val)
         : value_(val) {
      }

      holder(T&& val)
         : value_(std::move(val)) {
      }

      bool equals(const holder_base * rhs) const {
         const holder<T> * rhs_downcast = dynamic_cast<const holder<T> *>(rhs);
         if (rhs_downcast != nullptr) {
            return rhs_downcast->value_ == value_;
         }
         return false;
      }
   };

   std::shared_ptr<const holder_base> p_;

   template <class T, class U = const typename enable_if_holdable<T>::type>
   void _hold(T&& val) {
      p_ = std::make_shared<holder<T>>(std::move(val));
   }

public:
   bool _equals(const shared_var& rhs) const {
      if (p_ == nullptr && rhs.p_ == nullptr) {
         return true;
      }
      else if (p_ != nullptr && rhs.p_ != nullptr) {
         return p_->equals(rhs.p_.get());
      }
      return false;
   }

   template <class T, class U = const typename enable_if_holdable<T>::type>
   bool _equals_val(const T& rhs) const {
      if (p_ == nullptr) {
         return false;
      }
      const shared_var::holder<std::decay<T>::type> * p_downcast =
         dynamic_cast<const shared_var::holder<std::decay<T>::type> *>(p_.get());

      if (p_downcast == nullptr) {
         return false;
      }
      return p_downcast->value_ == rhs;
   }
public:
   shared_var() {
   }

   // Other vars

   shared_var(const shared_var& rhs)
      : p_(rhs.p_) {
   }

   shared_var(shared_var&& val) {
      p_.swap(val.p_);
   }

   shared_var& operator=(const shared_var& rhs) {
      p_ = rhs.p_;
      return *this;
   }

   shared_var& operator=(shared_var&& rhs) {
      p_.swap(rhs.p_);
      return *this;
   }

   explicit shared_var(nullptr_t) {

   }

   shared_var& operator=(nullptr_t) {
      p_ = nullptr;
      return *this;
   }

   // C-string special handling
   template <class CharT, class U = typename enable_if_char<CharT>::type>
   explicit shared_var(const CharT * rhs) {
      _hold(std::basic_string<CharT>(rhs));
   }

   template <class CharT, class U = typename enable_if_char<CharT>::type>
   shared_var& operator=(const CharT * rhs) {
      _hold(std::basic_string<CharT>(rhs));
      return *this;
   }

   // generic "holdable" types:
   template <class T, class U = const typename enable_if_holdable<T>::type>
   explicit shared_var(const T& val) {
      _hold(T(val));
   }

   template <class T, class U = const typename enable_if_holdable<T>::type>
   explicit shared_var(T&& val) {
      _hold(std::move(val));
   }   

   template <class T, class U = const typename enable_if_holdable<T>::type>
   shared_var& operator=(const T& rhs) {
      _hold(T(rhs));
   }

   template <class T, class U = const typename enable_if_holdable<T>::type>
   shared_var& operator=(T&& rhs) {
      _hold(std::move(rhs));
      return *this;
   }



   // as, is, empty
   template <class T>
   const T& as() const {
      const holder<T> * p_downcast = dynamic_cast<const holder<std::decay<T>::type> *>(p_.get());
      if (p_downcast != nullptr) {
         return p_downcast->value_;
      }
      static const T defaultValue = T();
      return defaultValue;
   }

   template <class T>
   const T& as(const T& def) const {
      const holder<T> * p_downcast = dynamic_cast<const holder<std::decay<T>::type> *>(p_.get());
      if (p_downcast != nullptr) {
         return p_downcast->value_;
      }
      return def;
   }

   template <class T>
   bool is() const {

      if (std::_Is_nullptr_t<T>::value) {
         return p_ == nullptr;
      }
      return nullptr != dynamic_cast<const holder<std::decay<T>::type> *>(p_.get());
   }

   bool empty() const {
      return p_ == nullptr;
   }
};

bool operator==(const shared_var& lhs, const shared_var& rhs) {
   return lhs._equals(rhs);
}

bool operator!=(const shared_var& lhs, const shared_var& rhs) {
   return !lhs._equals(rhs);
}


// compare with anything.

template <class T, class U = const typename enable_if_holdable<T>::type>
bool operator==(const shared_var& lhs, const T& rhs) {
   return lhs._equals_val(rhs);
}

template <class T, class U = const typename enable_if_holdable<T>::type>
bool operator!=(const shared_var& lhs, const T& rhs) {
   return !lhs._equals_val(rhs);
}


template <class T, class U = const typename enable_if_holdable<T>::type>
bool operator==(const T& lhs, const shared_var& rhs) {
   return rhs._equals_val(lhs);
}

template <class T, class U = const typename enable_if_holdable<T>::type>
bool operator!=(const T& lhs, const shared_var& rhs) {
   return !rhs._equals_val(lhs);
}


// null pointer.
bool operator==(nullptr_t, const shared_var& rhs) {
   return rhs.empty();
}

bool operator==(const shared_var& lhs, nullptr_t) {
   return lhs.empty();
}

bool operator!=(nullptr_t, const shared_var& rhs) {
   return !rhs.empty();
}

bool operator!=(const shared_var& lhs, nullptr_t) {
   return !lhs.empty();
}

// For C-strings
template <class CharT, class U = typename enable_if_char<CharT>::type>
bool operator==(const shared_var& lhs, const CharT * rhs) {
   return lhs == std::basic_string<CharT>(rhs);
}

template <class CharT, class U = typename enable_if_char<CharT>::type>
bool operator==(const CharT * lhs, const shared_var& rhs) {
   return rhs == std::basic_string<CharT>(lhs);
}

template <class CharT, class U = typename enable_if_char<CharT>::type>
bool operator!=(const shared_var& lhs, const CharT * rhs) {
   return !(operator==(lhs, rhs));
}

template <class CharT, class U = typename enable_if_char<CharT>::type>
bool operator!=(const CharT * lhs, const shared_var& rhs) {
   return !(operator==(rhs, lhs));
}

#endif // _SHARED_VAR_H_INCLUDED_
