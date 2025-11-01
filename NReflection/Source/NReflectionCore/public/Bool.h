#pragma once
#include "Type.h"
#include "TypeFactory.h"

template<>
struct TypeName<bool>
{
	static constexpr char Name[] = "bool";
	inline static const auto RealName = typeid(bool).name();
};

class Bool : public Type
{
public:
	using Type_ = bool;
	Bool() : Type(TypeName<Type_>::Name, TypeName<Type_>::RealName,typeid(Type_), sizeof(Type_)) {};
	virtual bool BableToBinary() const override
	{
		return true;
	}
	virtual std::vector<uint8_t> ToBinary(const void* self) const override
	{
		std::vector<uint8_t> reuslt;
		reuslt.resize(size_);
		memcpy(reuslt.data(), self, size_);
		return reuslt;
	}
};

template<>
struct GetTypeInfo<bool>
{
	using Type_ = Bool;
	static constexpr size_t Size = sizeof(bool);
};

class BoolFactory
{
public:
	using Type_ = typename bool;
	static BoolFactory& Instance()
	{
		static BoolFactory inst;
		BoolFactory* instPtr = &inst;
		return inst;
	}

	typename GetTypeInfo<Type_>::Type_& Info() { return *info_; }
private:
	typename GetTypeInfo<Type_>::Type_* info_;
	BoolFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<GetTypeInfo<Type_>::Type_>();
	}
};

template<>
class FactoryGetter<bool>
{
public:
	using GetFactory = BoolFactory;
};