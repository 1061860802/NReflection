#pragma once
#include "NReflectionCoreAPI.h"
#include <type_traits>

namespace NLEngine
{

	class CStaticVariableBase;

	class NREFLECTIONCORE_API CStaticVariableMetaBase
	{
	public:
		CStaticVariableMetaBase() {};
		virtual ~CStaticVariableMetaBase() = default;
		friend class CStaticVariableBase;
	protected:
		virtual void AttachedtoType(CStaticVariableBase* OP_Function)
		{
			return;
		}
		virtual void DetachFromType(CStaticVariableBase* OP_Function)
		{
			return;
		}
	};

}