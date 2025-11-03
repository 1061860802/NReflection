#pragma once
#include "Type.h"
#include "TypeFactory.h"

template<>
struct TTypeName<bool>
{
	static constexpr char Name[] = "bool";
	inline static const auto RealName = typeid(bool).name();
};

class CBool : public CType
{
public:
	using Type_ = bool;
	CBool() : CType(TTypeName<Type_>::Name, TTypeName<Type_>::RealName,typeid(Type_), sizeof(Type_)) {};
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
struct TGetTypeInfo<bool>
{
	using Type_ = CBool;
	static constexpr size_t Size = sizeof(bool);
};

class CBoolFactory
{
public:
	using Type_ = typename bool;
	static CBoolFactory& Instance()
	{
		static CBoolFactory inst;
		CBoolFactory* instPtr = &inst;
		return inst;
	}

	typename TGetTypeInfo<Type_>::Type_& Info() { return *info_; }
private:
	typename TGetTypeInfo<Type_>::Type_* info_;
	CBoolFactory()
	{
		info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<Type_>::Type_>();
	}
};

template<>
class TFactoryGetter<bool>
{
public:
	using GetFactory = CBoolFactory;
};