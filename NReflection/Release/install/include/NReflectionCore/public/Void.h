#pragma once
#include "Type.h"
#include "TypeFactory.h"
#include "TypeManager.h"

namespace NLEngine
{

	template<>
	struct TTypeName<void>
	{
		static constexpr char Name[] = "void";
	};

	template<>
	struct TGetTypeInfo<void>
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

		typename TGetTypeInfo<void>::Type_& Info() { return *info_; }
	private:
		typename TGetTypeInfo<void>::Type_* info_;
		VoidFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TGetTypeInfo<void>::Type_>();
		}
	};

	template<>
	class TFactoryGetter<void>
	{
	public:
		using GetFactory = VoidFactory;
	};

}
