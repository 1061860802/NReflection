#pragma once
#include "Enum.h"
#include "TypeManager.h"

namespace NLEngine
{
	template<typename Type_T>
	class EnumFactory
	{
	public:
		using Type_ = typename Type_T;
		static EnumFactory& Instance()
		{
			static EnumFactory inst;
			EnumFactory* instPtr = &inst;
			return inst;
		}
		TEnum<Type_T>& Info()
		{
			info_->PostConstructorProcess();
			return *info_;
		}
	private:
		typename TEnum<Type_T>* info_;
		EnumFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TEnum<Type_T>>();
		};
	};

	template<typename T>
	class TFactoryGetter<T, std::enable_if_t<std::is_enum_v<T> && (!VReflectionTraits::remove_const<T>::is_const)>>
	{
	public:
		using GetFactory = EnumFactory<T>;
	};

	template<typename T>
	struct TGetTypeInfo<T, std::enable_if_t<std::is_enum_v<T> && (!VReflectionTraits::remove_const<T>::is_const)>>
	{
		using Type_ = TEnum<T>;
		static constexpr size_t Size = sizeof(T);
	};
}