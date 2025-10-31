#pragma once
#include "Type.h"
#include "TypeFactory.h"
#include "TypeManager.h"

template<>
struct TypeName<void>
{
	static constexpr char Name[] = "void";
};

template<>
struct GetTypeInfo<void>
{
	using Type_ = Void;
	static constexpr size_t Size = 0;
};

class VoidFactory
{
public:
	using Type_ = void;
	static VoidFactory& Instance()
	{
		static VoidFactory inst;
		VoidFactory* instPtr = &inst;
		return inst;
	}

	typename GetTypeInfo<void>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<void>::Type_* info_;
	VoidFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<void>::Type_>();
	}
};

template<>
class FactoryGetter<void>
{
public:
	using GetFactory = VoidFactory;
};
