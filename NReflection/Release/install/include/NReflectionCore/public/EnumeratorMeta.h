#pragma once
#include "NReflectionCoreAPI.h"
#include <type_traits>

namespace NLEngine
{

	class CEnumeratorBase;

	class NREFLECTIONCORE_API CEnumeratorMetaBase
	{
	public:
		CEnumeratorMetaBase() {};
		virtual ~CEnumeratorMetaBase() = default;
		friend class CEnumeratorBase;
	protected:
		virtual void AttachedtoType(CEnumeratorBase* OP_Function)
		{
			return;
		}
		virtual void DetachFromType(CEnumeratorBase* OP_Function)
		{
			return;
		}
	};

}