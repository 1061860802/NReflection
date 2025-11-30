#pragma once
#include "tuple"

namespace NLEngine
{

	namespace VReflectionTraits
	{
		template<typename T>
		struct remove_reference
		{
			using type = typename T;
			static constexpr bool is_reference = false;
			static constexpr bool is_lref = false;
			static constexpr bool is_rref = false;
		};

		template<typename T>
		struct remove_reference<T&>
		{
			using type = typename T;
			static constexpr bool is_reference = true;
			static constexpr bool is_lref = true;
			static constexpr bool is_rref = false;
		};

		template<typename T>
		struct remove_reference<T&&>
		{
			using type = typename T;
			static constexpr bool is_reference = true;
			static constexpr bool is_lref = false;
			static constexpr bool is_rref = true;
		};

		template<typename T>
		struct remove_pointer
		{
			using type = T;
			static constexpr bool is_pointer = false;
		};

		template<typename T>
		struct remove_pointer<T*>
		{
			using type = T;
			static constexpr bool is_pointer = true;
		};

		template<typename T>
		struct remove_const
		{
			using type = T;
			static constexpr bool is_const = false;
		};

		template<typename T>
		struct remove_const<const T>
		{
			using type = T;
			static constexpr bool is_const = true;
		};

		template<typename T>
		struct function_traits
		{
			static constexpr bool is_func = false;
		};

		template<typename Ret, typename... Params>
		struct function_traits<Ret(*)(Params...)>
		{
			using return_type = Ret;
			using param = std::tuple<Params...>;
			static constexpr bool is_class = false;
			static constexpr bool is_func = true;
		};

		template<typename Ret, typename TClass, typename... Params>
		struct function_traits<Ret(TClass::*)(Params...)>
		{
			using return_type = Ret;
			using param = std::tuple<Params...>;
			using class_type = TClass;
			static constexpr bool is_class = true;
			static constexpr bool is_func = true;
		};

		template<typename T>
		struct variable_traits;

		template<typename Ret, typename TClass>
		struct variable_traits<Ret(TClass::*)>
		{
			using return_type = Ret;
			using class_type = TClass;
			static constexpr bool is_class = true;
		};

		template<typename T>
		struct Array_Size
		{
			using type = T;
			static constexpr bool is_array = false;
		};

		template<typename Ret, size_t T_ArraySize>
		struct Array_Size<Ret[T_ArraySize]>
		{
			using type = Ret;
			static constexpr size_t ArraySize = T_ArraySize;
			static constexpr bool is_array = true;
		};
	}

	template<typename Ret_T>
	Ret_T BlackMagicVoidPtrCast(void* IN_Ptr)
	{
		using NoReferenceT = typename VReflectionTraits::remove_reference<Ret_T>::type;
		NoReferenceT* ValPtr = (NoReferenceT*)IN_Ptr;
		return (Ret_T)*ValPtr;
	}

	template<typename IN_T>
	constexpr bool IsPod()
	{
		if constexpr (std::is_trivial_v<IN_T> && std::is_standard_layout_v<IN_T>)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

}