#pragma once
#include "NReflectionCoreAPI.h"
#include <type_traits>

namespace NLEngine
{

	class CMemberVariableBase;

	class NREFLECTIONCORE_API CMemberVariableMetaBase
	{
	public:
		CMemberVariableMetaBase() {};
		virtual ~CMemberVariableMetaBase() = default;
		friend class CMemberVariableBase;
	protected:
		virtual void AttachedtoType(CMemberVariableBase* OP_Function)
		{
			return;
		}
		virtual void DetachFromType(CMemberVariableBase* OP_Function)
		{
			return;
		}
	};

}