#pragma once
#include "GCSystemBase.h"
#include "Type.h"
#include "TypeManager.h"
#include <vector>
#include <set>
#include <shared_mutex>

namespace NLEngine
{

	class GCSystem
	{
	public:
		GCSystem();
		~GCSystem();
		static void ShardLock();
		static void ShardUnLock();
		static void GCCollect();
	private:
		struct GCRootObject
		{
			GCRootObject(void* In_Ptr, CType* In_PtrType) :Ptr(In_Ptr), PtrType(In_PtrType) {};
			void* Ptr;
			CType* PtrType;
		};
		std::shared_mutex Mutex;
		std::vector<GCRootObject> Roots;
		std::set<IGCObjectInterface*> AllObjects;
	};

	GCSystem::GCSystem()
	{
	}

	GCSystem::~GCSystem()
	{
	}

}