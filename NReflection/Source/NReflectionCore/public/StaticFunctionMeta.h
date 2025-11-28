#pragma once
#include "NReflectionCoreAPI.h"
#include <type_traits>

namespace NLEngine
{

	class CStaticFunctionContainerBase;

	class NREFLECTIONCORE_API CStaticFunctionMetaBase
	{
	public:
		CStaticFunctionMetaBase() {};
		virtual ~CStaticFunctionMetaBase() = default;
		friend class CStaticFunctionContainerBase;
	protected:
		virtual void AttachedtoType(CStaticFunctionContainerBase* OP_Function)
		{
			return;
		}
		virtual void DetachFromType(CStaticFunctionContainerBase* OP_Function)
		{
			return;
		}
	};

}