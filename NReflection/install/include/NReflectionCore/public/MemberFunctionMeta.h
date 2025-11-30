#pragma once
#include "NReflectionCoreAPI.h"
#include <type_traits>

namespace NLEngine
{

	class CMemberFunctionContainerBase;

	class NREFLECTIONCORE_API CMemberFunctionMetaBase
	{
	public:
		CMemberFunctionMetaBase() {};
		virtual ~CMemberFunctionMetaBase() = default;
		friend class CMemberFunctionContainerBase;
	protected:
		virtual void AttachedtoType(CMemberFunctionContainerBase* OP_Function)
		{
			return;
		}
		virtual void DetachFromType(CMemberFunctionContainerBase* OP_Function)
		{
			return;
		}
	};

}