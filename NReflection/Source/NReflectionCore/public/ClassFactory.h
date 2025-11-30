#pragma once
#include "class.h"
#include "TypeManager.h"

namespace NLEngine
{ 
	template<typename Type_T>
	class ClassFactory
	{
	public:
		using Type_ = typename Type_T;
		static ClassFactory& Instance()
		{
			static ClassFactory inst;
			ClassFactory* instPtr = &inst;
			return inst;
		}
		TClass<Type_T>& Info() 
		{ 
			info_->PostConstructorProcess();
			return *info_;
		}
	private:
		typename TClass<Type_T>* info_;
		ClassFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TClass<Type_T>>();
		};
	};

	template<typename T>
	class TFactoryGetter<T, std::enable_if_t<std::is_class_v<T> && (!VReflectionTraits::remove_const<T>::is_const)>>
	{
	public:
		using GetFactory = ClassFactory<T>;
	};

	template<typename T>
	struct TGetTypeInfo<T, std::enable_if_t<std::is_class_v<T> && (!VReflectionTraits::remove_const<T>::is_const)>>
	{
		using Type_ = TClass<T>;
		static constexpr size_t Size = sizeof(T);
	};
}