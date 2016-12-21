

namespace boost{
   namespace mpl
   {
      template <bool B> struct bool_;
      template <class I, I val> struct integral_c;
      struct integral_c_tag;
   }
}


namespace boost{

   template <class T, T val>
   struct integral_constant
   {
      typedef mpl::integral_c_tag tag;
      typedef T value_type;
      typedef integral_constant<T, val> type;
      static const T value = val;
      //
      // This helper function is just to disable type-punning
      // warnings from GCC:
      //
      template <class U>
      static U& dereference(U* p) { return *p; }

      operator const mpl::integral_c<T, val>& ()const
      {
         static const char data[sizeof(long)] = { 0 };
         return dereference(reinterpret_cast<const mpl::integral_c<T, val>*>(&data));
      }
      constexpr operator T()const { return val; }
   };

   template <class T, T val>
   T const integral_constant<T, val>::value;

   template <bool val>
   struct integral_constant<bool, val>
   {
      typedef mpl::integral_c_tag tag;
      typedef bool value_type;
      typedef integral_constant<bool, val> type;
      static const bool value = val;
      //
      // This helper function is just to disable type-punning
      // warnings from GCC:
      //
      template <class T>
      static T& dereference(T* p) { return *p; }

      operator const mpl::bool_<val>& ()const
      {
         static const char data = 0;
         return dereference(reinterpret_cast<const mpl::bool_<val>*>(&data));
      }
      constexpr operator bool()const { return val; }
   };

   template <bool val>
   bool const integral_constant<bool, val>::value;

   typedef integral_constant<bool, true> true_type;
   typedef integral_constant<bool, false> false_type;

}

namespace boost {

   //  convert a type T to a non-cv-qualified type - remove_cv<T>
template <class T> struct remove_cv{ typedef T type; };
template <class T> struct remove_cv<T const>{ typedef T type;  };
template <class T> struct remove_cv<T volatile>{ typedef T type; };
template <class T> struct remove_cv<T const volatile>{ typedef T type; };

// #if !defined(BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <class T, std::size_t N> struct remove_cv<T const[N]>{ typedef T type[N]; };
template <class T, std::size_t N> struct remove_cv<T const volatile[N]>{ typedef T type[N]; };
template <class T, std::size_t N> struct remove_cv<T volatile[N]>{ typedef T type[N]; };
// #endif


} // namespace boost


namespace boost {

namespace type_traits_detail {

    template <typename T, bool b>
    struct add_rvalue_reference_helper
    { typedef T   type; };

    template <typename T>
    struct add_rvalue_reference_helper<T, true>
    {
        typedef T&&   type;
    };

    template <typename T>
    struct add_rvalue_reference_imp
    {
       typedef typename boost::type_traits_detail::add_rvalue_reference_helper
                  <T, (is_void<T>::value == false && is_reference<T>::value == false) >::type type;
    };

}

template <class T> struct add_rvalue_reference
{
   typedef typename boost::type_traits_detail::add_rvalue_reference_imp<T>::type type;
};


template <typename T>
typename add_rvalue_reference<T>::type declval() BOOST_NOEXCEPT; // as unevaluated operand


}  // namespace boost


// namespace std {
//
// 	struct true_type{};
// 	struct false_type{};
//
// 	template <class I> class is_integer{
// 	public:
// 		typedef false_type value;
// 	};
//
// 	template <> class is_integer <unsigned int>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <signed int>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <short unsigned int>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <short signed int>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <char>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <signed char>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <unsigned char>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <long unsigned int>{
// 	public:
// 		typedef true_type value;
// 	};
//
// 	template <> class is_integer <long signed int>{
// 	public:
// 		typedef true_type value;
// 	};
//
//
//
// }
