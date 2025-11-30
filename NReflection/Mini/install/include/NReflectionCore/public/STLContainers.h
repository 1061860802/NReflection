#pragma once

#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

#include "Class.h"

namespace NLEngine
{
	// =========================================================
	// 1. std::vector
	// =========================================================
	template<typename T>
	struct PostConstructorProcessUtility<TClass<std::vector<T>>>
	{
		PostConstructorProcessUtility() = delete;
		using TClassT = typename TClass<std::vector<T>>;
		using SizeType = typename std::vector<T>::size_type;

		static void PostConstructorProcess(TClassT* IN_Class, TClassInfo<typename TClassT::ClassT_>* IN_ClassInfo)
		{
			// Modifiers
			IN_Class->ApplyFunction(static_cast<void (std::vector<T>::*)(const T&)>(&std::vector<T>::push_back), "push_back");
			IN_Class->ApplyFunction(static_cast<void (std::vector<T>::*)()>(&std::vector<T>::pop_back), "pop_back");
			IN_Class->ApplyFunction(static_cast<void (std::vector<T>::*)()>(&std::vector<T>::clear), "clear");

			// Capacity (resize/reserve 有多个重载，这里取单参数版本)
			IN_Class->ApplyFunction(static_cast<void (std::vector<T>::*)(SizeType)>(&std::vector<T>::resize), "resize");
			IN_Class->ApplyFunction(static_cast<void (std::vector<T>::*)(SizeType)>(&std::vector<T>::reserve), "reserve");

			// Observers (const 函数)
			IN_Class->ApplyFunction(static_cast<SizeType(std::vector<T>::*)() const>(&std::vector<T>::size), "size");
			IN_Class->ApplyFunction(static_cast<bool (std::vector<T>::*)() const>(&std::vector<T>::empty), "empty");
		}
	};

	// =========================================================
	// 2. std::deque (双端队列)
	// =========================================================
	template<typename T>
	struct PostConstructorProcessUtility<TClass<std::deque<T>>>
	{
		PostConstructorProcessUtility() = delete;
		using TClassT = typename TClass<std::deque<T>>;
		using SizeType = typename std::deque<T>::size_type;

		static void PostConstructorProcess(TClassT* IN_Class, TClassInfo<typename TClassT::ClassT_>* IN_ClassInfo)
		{
			// Front/Back Ops
			IN_Class->ApplyFunction(static_cast<void (std::deque<T>::*)(const T&)>(&std::deque<T>::push_back), "push_back");
			IN_Class->ApplyFunction(static_cast<void (std::deque<T>::*)(const T&)>(&std::deque<T>::push_front), "push_front");
			IN_Class->ApplyFunction(static_cast<void (std::deque<T>::*)()>(&std::deque<T>::pop_back), "pop_back");
			IN_Class->ApplyFunction(static_cast<void (std::deque<T>::*)()>(&std::deque<T>::pop_front), "pop_front");
			IN_Class->ApplyFunction(static_cast<void (std::deque<T>::*)()>(&std::deque<T>::clear), "clear");

			// Observers
			IN_Class->ApplyFunction(static_cast<SizeType(std::deque<T>::*)() const>(&std::deque<T>::size), "size");
			IN_Class->ApplyFunction(static_cast<bool (std::deque<T>::*)() const>(&std::deque<T>::empty), "empty");
		}
	};

	// =========================================================
	// 3. std::list (双向链表)
	// =========================================================
	template<typename T>
	struct PostConstructorProcessUtility<TClass<std::list<T>>>
	{
		PostConstructorProcessUtility() = delete;
		using TClassT = typename TClass<std::list<T>>;
		using SizeType = typename std::list<T>::size_type;

		static void PostConstructorProcess(TClassT* IN_Class, TClassInfo<typename TClassT::ClassT_>* IN_ClassInfo)
		{
			// Modifiers
			IN_Class->ApplyFunction(static_cast<void (std::list<T>::*)(const T&)>(&std::list<T>::push_back), "push_back");
			IN_Class->ApplyFunction(static_cast<void (std::list<T>::*)(const T&)>(&std::list<T>::push_front), "push_front");
			IN_Class->ApplyFunction(static_cast<void (std::list<T>::*)()>(&std::list<T>::pop_back), "pop_back");
			IN_Class->ApplyFunction(static_cast<void (std::list<T>::*)()>(&std::list<T>::pop_front), "pop_front");
			IN_Class->ApplyFunction(static_cast<void (std::list<T>::*)()>(&std::list<T>::clear), "clear");

			// List 特有的按值删除
			IN_Class->ApplyFunction(static_cast<void (std::list<T>::*)(const T&)>(&std::list<T>::remove), "remove");

			// Observers
			IN_Class->ApplyFunction(static_cast<SizeType(std::list<T>::*)() const>(&std::list<T>::size), "size");
		}
	};

	// =========================================================
	// 4. std::set / std::unordered_set
	//    (抽取公共宏或逻辑以避免重复代码)
	// =========================================================

	// Helper macro to implement Set registration (Works for set and unordered_set)
#define REGISTER_SET_CONTAINER(CONTAINER_TYPE) \
	template<typename T> \
	struct PostConstructorProcessUtility<TClass<CONTAINER_TYPE<T>>> \
	{ \
		PostConstructorProcessUtility() = delete; \
		using TClassT = typename TClass<CONTAINER_TYPE<T>>; \
		using SizeType = typename CONTAINER_TYPE<T>::size_type; \
		using Iterator = typename CONTAINER_TYPE<T>::iterator; \
		static void PostConstructorProcess(TClassT* IN_Class, TClassInfo<typename TClassT::ClassT_>* IN_ClassInfo) \
		{ \
			/* Insert: Returns pair<iterator, bool> */ \
			IN_Class->ApplyFunction( \
				static_cast<std::pair<Iterator, bool> (CONTAINER_TYPE<T>::*)(const T&)>(&CONTAINER_TYPE<T>::insert), \
				"insert" \
			); \
			/* Erase: Returns number of elements removed (size_type) */ \
			IN_Class->ApplyFunction( \
				static_cast<SizeType (CONTAINER_TYPE<T>::*)(const T&)>(&CONTAINER_TYPE<T>::erase), \
				"erase" \
			); \
			/* Count: Checks if exists */ \
			IN_Class->ApplyFunction( \
				static_cast<SizeType (CONTAINER_TYPE<T>::*)(const T&) const>(&CONTAINER_TYPE<T>::count), \
				"count" \
			); \
			IN_Class->ApplyFunction(static_cast<void (CONTAINER_TYPE<T>::*)()>(&CONTAINER_TYPE<T>::clear), "clear"); \
			IN_Class->ApplyFunction(static_cast<SizeType (CONTAINER_TYPE<T>::*)() const>(&CONTAINER_TYPE<T>::size), "size"); \
			IN_Class->ApplyFunction(static_cast<bool (CONTAINER_TYPE<T>::*)() const>(&CONTAINER_TYPE<T>::empty), "empty"); \
		} \
	};

	REGISTER_SET_CONTAINER(std::set)
		REGISTER_SET_CONTAINER(std::unordered_set)

		// =========================================================
		// 5. std::map / std::unordered_map
		// =========================================================

#define REGISTER_MAP_CONTAINER(CONTAINER_TYPE) \
	template<typename Key, typename Value> \
	struct PostConstructorProcessUtility<TClass<CONTAINER_TYPE<Key, Value>>> \
	{ \
		PostConstructorProcessUtility() = delete; \
		using TClassT = typename TClass<CONTAINER_TYPE<Key, Value>>; \
		using SizeType = typename CONTAINER_TYPE<Key, Value>::size_type; \
		using PairType = typename CONTAINER_TYPE<Key, Value>::value_type; /* pair<const Key, Value> */ \
		using Iterator = typename CONTAINER_TYPE<Key, Value>::iterator; \
		static void PostConstructorProcess(TClassT* IN_Class, TClassInfo<typename TClassT::ClassT_>* IN_ClassInfo) \
		{ \
			/* Insert: Requires std::pair<const Key, Value> */ \
			IN_Class->ApplyFunction( \
				static_cast<std::pair<Iterator, bool> (CONTAINER_TYPE<Key, Value>::*)(const PairType&)>(&CONTAINER_TYPE<Key, Value>::insert), \
				"insert" \
			); \
			/* Erase: By Key */ \
			IN_Class->ApplyFunction( \
				static_cast<SizeType (CONTAINER_TYPE<Key, Value>::*)(const Key&)>(&CONTAINER_TYPE<Key, Value>::erase), \
				"erase" \
			); \
			/* Count: Check key existence */ \
			IN_Class->ApplyFunction( \
				static_cast<SizeType (CONTAINER_TYPE<Key, Value>::*)(const Key&) const>(&CONTAINER_TYPE<Key, Value>::count), \
				"count" \
			); \
			IN_Class->ApplyFunction(static_cast<void (CONTAINER_TYPE<Key, Value>::*)()>(&CONTAINER_TYPE<Key, Value>::clear), "clear"); \
			IN_Class->ApplyFunction(static_cast<SizeType (CONTAINER_TYPE<Key, Value>::*)() const>(&CONTAINER_TYPE<Key, Value>::size), "size"); \
			IN_Class->ApplyFunction(static_cast<bool (CONTAINER_TYPE<Key, Value>::*)() const>(&CONTAINER_TYPE<Key, Value>::empty), "empty"); \
		} \
	};

		REGISTER_MAP_CONTAINER(std::map)
		REGISTER_MAP_CONTAINER(std::unordered_map)

		// Clean up macros
#undef REGISTER_SET_CONTAINER
#undef REGISTER_MAP_CONTAINER
}