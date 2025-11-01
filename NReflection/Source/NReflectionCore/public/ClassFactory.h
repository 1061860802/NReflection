#pragma once
#include "class.h"
#include "TypeManager.h"

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
	Class<Type_T>& Info() { return *info_; }
private:
	typename Class<Type_T>* info_;
	ClassFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<Class<Type_T>>();
	};
};

template<typename T>
class FactoryGetter<T, std::enable_if_t<std::is_class_v<T>&&(!VReflectionTraits::remove_const<T>::is_const)>>
{
public:
	using GetFactory =  ClassFactory<T>;
};

template<typename T>
struct GetTypeInfo<T, std::enable_if_t<std::is_class_v<T>&&(!VReflectionTraits::remove_const<T>::is_const)>>
{
	using Type_ = Class<T>;
	static constexpr size_t Size = sizeof(T);
};