#pragma once
#include "Type.h"
#include "TypeManager.h"
#include "Variable_traits.h"
#include <assert.h>

template<typename Type_T>
class UnKnowFactory
{
public:
	using Type_ = typename Type_T;
	static UnKnowFactory& Instance()
	{
		static UnKnowFactory inst;
		UnKnowFactory* instPtr = &inst;
		return inst;
	}

	typename TGetTypeInfo<Type_T>::Type_& Info() { return *info_; }
private:
	typename TGetTypeInfo<Type_T>::Type_* info_;
	UnKnowFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<Type_T>::Type_>();
		return;
		static_assert(!std::is_same_v<TGetTypeInfo<Type_T>::Type_, CUnknown<Type_T>>, "Error:UnknowType");
	}
};


template<typename Type_T, typename = void>
class TFactoryGetter
{
public:
	using GetFactory = UnKnowFactory<Type_T>;
	using Type_ = CUnknown<Type_T>;
	static_assert(!std::is_same_v<Type_, CUnknown<Type_T>>, "Error:UnknowType");
};

template<typename T>
auto& Register()
{
	using InnerT = typename VReflectionTraits::remove_const<T>::type;
	return TFactoryGetter<InnerT>::GetFactory::Instance();
}

template<typename T>
auto& GetType()
{
	using InnerT = typename VReflectionTraits::remove_const<T>::type;
	return TFactoryGetter<InnerT>::GetFactory::Instance().Info();
}