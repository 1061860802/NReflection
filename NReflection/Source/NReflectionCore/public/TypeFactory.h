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

	typename GetTypeInfo<Type_T>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<Type_T>::Type_* info_;
	UnKnowFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<Type_T>::Type_>();
		return;
		static_assert(!std::is_same_v<GetTypeInfo<Type_T>::Type_, Unknown<Type_T>>, "Error:UnknowType");
	}
};


template<typename Type_T, typename = void>
class FactoryGetter
{
public:
	using GetFactory = UnKnowFactory<Type_T>;
	using Type_ = Unknown<Type_T>;
	static_assert(!std::is_same_v<Type_, Unknown<Type_T>>, "Error:UnknowType");
};

template<typename T>
auto& Register()
{
	using InnerT = typename VReflectionTraits::remove_const<T>::type;
	return FactoryGetter<InnerT>::GetFactory::Instance();
}

template<typename T>
auto& GetType()
{
	using InnerT = typename VReflectionTraits::remove_const<T>::type;
	return FactoryGetter<InnerT>::GetFactory::Instance().Info();
}