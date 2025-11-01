#pragma once
#include <iostream>
#include <string>

template<typename T, typename = void>
struct has_tostring
{
	static constexpr bool tostring = false;
};

template<typename T>
struct has_tostring<T, std::void_t<decltype(std::to_string(std::declval<T>()))>>
{
	static constexpr bool tostring = true;
};

template<typename T, typename = void>
struct UseCustomPrint
{
	static constexpr bool bUseCustomPrint = false;
};

template<typename T>
struct UseCustomPrint<T, std::void_t<decltype(ToString(std::declval<T>()))>>
{
	static constexpr bool bUseCustomPrint = true;
};

template<typename Type_T>
std::string& print(std::string& In, Type_T& Target)
{
	if constexpr (UseCustomPrint<Type_T>::bUseCustomPrint)
	{
		return In.append(ToString(Target));
	}
	else if constexpr (has_tostring<Type_T>::tostring)
	{
		return In.append(std::to_string(Target));
	}
	else if constexpr (std::is_class_v<Type_T>)
	{
		return In.append(GetType<Type_T>().print(&Target));
	}
	return In;
}

