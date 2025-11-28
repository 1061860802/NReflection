#pragma once
#pragma once
#include "Type.h"
#include "BaseNumeric.h"
#include "TypeManager.h"
#include "TypeFactory.h"

namespace NLEngine
{
	class CArrayBase : public CType
	{
	public:
		CArrayBase(const std::string& name, const std::string& realname, const type_info& type_info_, size_t size, size_t In_ArraySize, const TVariableTypeContainer<CType> Inner) : CType(name, realname, type_info_, size), ArraySize(In_ArraySize), Inner(Inner)
		{
		};
		const TVariableTypeContainer<CType> Inner;
		const size_t ArraySize;
	};

	template<typename _InType_, size_t In_ArraySize>
	class TArray : public CArrayBase
	{
	public:
		TArray(const std::string& name, size_t size, const TVariableTypeContainer<CType> Inner) : CArrayBase(name, typeid(_InType_[In_ArraySize]).name(), typeid(_InType_[In_ArraySize]), size, In_ArraySize, Inner)
		{
		};
		virtual bool BableToBinary() const override
		{
			if constexpr (std::is_trivially_copyable_v<_InType_>)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		virtual std::vector<uint8_t> ToBinary(const void* self) const override
		{
			if (BableToBinary())
			{
				std::vector<uint8_t> reuslt;
				reuslt.resize(size_ * In_ArraySize);
				memcpy(reuslt.data(), self, size_ * In_ArraySize);
				return reuslt;
			}
			return std::vector<uint8_t>();
		}
	};

	template<typename _InType_, size_t In_ArraySize>
	struct TTypeName<const _InType_[In_ArraySize]>
	{
		inline static const auto Name = ("const_" + std::string(typeid(_InType_).name()) + "[" + std::to_string(In_ArraySize) + "]");
	};

	template<typename _InType_, size_t In_ArraySize>
	struct TTypeName<_InType_[In_ArraySize]>
	{
		inline static const auto Name = (std::string(typeid(_InType_).name()) + "[" + std::to_string(In_ArraySize) + "]");
	};

	template<typename _InType_, size_t In_ArraySize>
	struct TGetTypeInfo<_InType_[In_ArraySize]>
	{
		using Type_ = TArray<_InType_, In_ArraySize>;
		static constexpr size_t Size = sizeof(_InType_[In_ArraySize]);
	};

	template<typename _InType_, size_t In_ArraySize>
	class TArrayFactory
	{
	public:
		using Type_ = _InType_;
		static TArrayFactory& Instance()
		{
			static TArrayFactory inst;
			TArrayFactory* instPtr = &inst;
			return inst;
		}
		typename TGetTypeInfo<_InType_[In_ArraySize]>::Type_& Info() { return *info_; }
	private:
		typename TGetTypeInfo<_InType_[In_ArraySize]>::Type_* info_;
		TArrayFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<_InType_[In_ArraySize]>::Type_>(TTypeName<Type_[In_ArraySize]>::Name, sizeof(_InType_[In_ArraySize]), GetType_container<_InType_>());
		}
	};

	template<typename _InType_, size_t In_ArraySize>
	class TFactoryGetter<_InType_[In_ArraySize]>
	{
	public:
		using GetFactory = TArrayFactory<_InType_, In_ArraySize>;
	};
}