#pragma once

#include "Variable_traits.h"
#include "TypeFactory.h"
#include "Type.h"
#include "ClassBase.h"
#include "Any.h"
#include <sstream>
#include <vector>
#include <tuple>
#include <optional>
#include "ClassMember.h"
#include "MemberFunctionMeta.h"

namespace NLEngine
{

	class CClassBase;

	class CMemberFunctionBase : public CType
	{
	public:
		CMemberFunctionBase(const TVariableTypeContainer<CType>& OwnerClass, const TVariableTypeContainer<CType>& Result, std::vector<TVariableTypeContainer<CType>> Params, const std::string& Name, const std::string& RealName, const type_info& type_info_) :
			OwnerClass_(OwnerClass), Result_(Result), Params_(Params), CType(Name, RealName, type_info_, 0, true) {
		};
		virtual std::string print() const
		{
			return name_;
		}

		const TVariableTypeContainer<CType>& GetTargetClass() const { return OwnerClass_; };
		const TVariableTypeContainer<CType>& GetResult() const { return Result_; };
		const std::vector<TVariableTypeContainer<CType>>& GetParams() const { return Params_; };
	protected:
		const TVariableTypeContainer<CType> OwnerClass_;
		const TVariableTypeContainer<CType> Result_;
		std::vector<TVariableTypeContainer<CType>> Params_;
	};

	template<typename Ret, typename TClass, typename... Params>
	class TMemberFunction : public CMemberFunctionBase
	{
	public:
		TMemberFunction(const TVariableTypeContainer<CType>& OwnerClass, const TVariableTypeContainer<CType>& Result, std::vector<TVariableTypeContainer<CType>> Params) :
			CMemberFunctionBase(OwnerClass, Result, Params, typeid(Type_).name(), typeid(Type_).name(), typeid(Type_)) {
		}
		using Type_ = typename Ret(TClass::*)(Params...);
		using Ret_ = typename Ret;
		using ParamsTuple = typename std::tuple<Params...>;
	};

	template<typename Ret, typename TClass, typename... Params>
	class TMemberFuncFactory
	{
	public:
		using Type_ = typename Ret(TClass::*)(Params...);
		using args_tuple = typename std::tuple<Params...>;

		using RetT = typename VReflectionTraits::remove_const<Ret>::type;
		static constexpr bool RetConst = VReflectionTraits::remove_const<Ret>::is_const;
		using ClassT = typename VReflectionTraits::remove_const<TClass>::type;
		static constexpr bool ClassConst = VReflectionTraits::remove_const<TClass>::is_const;
		TMemberFuncFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<TMemberFunction<Ret, TClass, Params...>>(TVariableTypeContainer<CType>(GetType<ClassT>(), ClassConst), TVariableTypeContainer<CType>(GetType<RetT>(), RetConst), GetParams());
		};
		static TMemberFuncFactory& Instance()
		{
			static TMemberFuncFactory inst;
			TMemberFuncFactory* instPtr = &inst;
			return inst;
		}
		TMemberFunction<Ret, TClass, Params...>& Info() { return *info_; }
	private:
		typename TMemberFunction<Ret, TClass, Params...>* info_;
		static std::vector<TVariableTypeContainer<CType>> GetParams()
		{
			std::vector<TVariableTypeContainer<CType>> Result;
			cvtTypeList2Vector<0>(Result);
			return Result;
		};
		template<size_t I>
		static void cvtTypeList2Vector(std::vector<TVariableTypeContainer<CType>>& Inputs)
		{
			if constexpr (I == std::tuple_size_v<args_tuple>)
			{
				return;
			}
			else
			{
				using TT = typename VReflectionTraits::remove_const<std::tuple_element_t<I, args_tuple>>::type;
				static constexpr bool TTIsConst = VReflectionTraits::remove_const<std::tuple_element_t<I, args_tuple>>::is_const;
				TVariableTypeContainer<CType> TC_(GetType<TT>(), TTIsConst);
				Inputs.push_back(TC_);
				cvtTypeList2Vector<I + 1>(Inputs);
			}
		}
	};

	template<typename Ret, typename TClass, typename... Params>
	class TFactoryGetter<Ret(TClass::*)(Params...)>
	{
	public:
		using GetFactory = TMemberFuncFactory<Ret, TClass, Params...>;
	};

	template<typename Ret, typename TClass, typename... Params>
	class TFactoryGetter<Ret(TClass::*)(Params...) const>
	{
	public:
		using GetFactory = TMemberFuncFactory<Ret, const TClass, Params...>;
	};

	class CMemberFunctionContainerBase : public CClassMember
	{
	public:
		virtual ~CMemberFunctionContainerBase()
		{
			for (auto i : Metas)
			{
				i->DetachFromType(this);
			}
		};
		CMemberFunctionContainerBase(const CMemberFunctionBase& CType, const std::string& Name, const CClassBase* OwnerClass) :Type_(CType), Name_(Name), OwnerClass_(OwnerClass) {}
		CMemberFunctionContainerBase(const CMemberFunctionBase& CType, const std::string& Name, const CClassBase* OwnerClass, const std::vector<CMemberFunctionMetaBase*>& IN_Metas) :Type_(CType), Name_(Name), OwnerClass_(OwnerClass), Metas(IN_Metas)
		{
			for (auto i : Metas)
			{
				i->AttachedtoType(this);
			}
		}
		void ApplyMeta(CMemberFunctionMetaBase* IN_Meta)
		{
			Metas.push_back(IN_Meta);
			IN_Meta->AttachedtoType(this);
		}
		void ApplyMetas(const std::vector<CMemberFunctionMetaBase*>& IN_Metas)
		{
			for (auto i : IN_Metas)
			{
				Metas.push_back(i);
				i->AttachedtoType(this);
			}
		}
		const std::string& GetName() const { return Name_; };
		const CMemberFunctionBase& GetType() const { return Type_; };
		const CClassBase* GetOwnerPtr() const { return OwnerClass_; };
		template<typename T, typename... Params>
		bool CallFunc(T* Caller, Params&&... args) const
		{
			CClassBase* realType = static_cast<CClassBase*>(GetObjRealType(Caller));
			void* realPtr = realType->CastFromParent(&NLEngine::GetType<T>(), Caller);
			if (realPtr == nullptr)
			{
				return false;
			}
			void* Ptr = realType->CastToParent(OwnerClass_, realPtr);
			if (Ptr == nullptr)
			{
				return false;
			}
			auto valRet = CallFuncAny(Ptr, { args... });
			if (valRet)
			{
				return true;
			}
			return false;
		};
		template<typename Ret_T, typename T, typename... Params>
		std::optional<typename VReflectionTraits::remove_reference<typename Ret_T>::refToPtr> CallFuncWithRet(T* Caller, Params&&... args) const
			/*
			 * Calls a reflected function and returns its result wrapped in optional.
			 *
			 * IMPORTANT REFERENCE HANDLING:
			 * - If Ret_T is a reference type, it will be converted to pointer in return
			 * - This is because std::optional cannot contain reference types
			 * - Caller must still specify Ret_T as reference (e.g., int&) for type matching
			 * - But actual return will be optional containing pointer (e.g., std::optional<int*>)
			 *
			 * @tparam Ret_T Expected return type (use reference if function returns reference)
			 * @tparam T Type of object containing the function
			 * @tparam Params Parameter types for the function
			 *
			 * @param Caller Object instance to call the function on
			 * @param args Arguments to pass to the function
			 *
			 * @return std::optional containing:
			 *         - Pointer if Ret_T is reference (due to optional reference limitation)
			 *         - Direct value if Ret_T is not reference
			 *         - std::nullopt if call fails
			 */
		{
			using RealRet_ = typename VReflectionTraits::remove_reference<typename Ret_T>::refToPtr;
			CClassBase* realType = static_cast<CClassBase*>(GetObjRealType(Caller));
			void* realPtr = realType->CastFromParent(&NLEngine::GetType<T>(), Caller);
			if (realPtr == nullptr)
			{
				return std::nullopt;
			}
			void* Ptr = realType->CastToParent(OwnerClass_, realPtr);
			if (Ptr == nullptr)
			{
				return std::nullopt;
			}
			auto valRet = CallFuncAny(Ptr, { args ... });
			if (valRet)
			{
				if (&valRet->GetValType().GetTypeConst() == GetStaticType<Ret_T>())
				{
					if constexpr (VReflectionTraits::remove_reference<typename Ret_T>::is_reference)
					{
						return (RealRet_)valRet->GetValue();
					}
					else
					{
						return *(RealRet_*)valRet->GetValue();
					}
				}
				return std::nullopt;
			}
			return std::nullopt;
		};
		virtual std::optional<CAny> CallFuncAny(void* Caller, const std::vector<AnyRef>& Params) const = 0;
		virtual std::optional<CAny> CallFuncAny(const AnyRef& Caller, const std::vector<AnyRef>& Params) const = 0;
		virtual std::string prints() const = 0;
	protected:
		const CClassBase* OwnerClass_;
		const CMemberFunctionBase& Type_;
		std::string Name_;
		std::vector<CMemberFunctionMetaBase*> Metas;
	};

	template<typename Ret, typename TClass, typename... Params>
	class TMemberFunctionContainer : public CMemberFunctionContainerBase
	{
	public:
		using param_ = typename std::tuple<Params...>;
		using class_ = typename TClass;
		using NoConstRet = typename VReflectionTraits::remove_const<Ret>::type;
		using Ret_ = typename Ret;
		virtual ~TMemberFunctionContainer() {};
		TMemberFunctionContainer(Ret(TClass::* FuncPtr_)(Params...), const CClassBase* OwnerClass, const CMemberFunctionBase& CType, const std::string& Name) :FuncPtr_(FuncPtr_), CMemberFunctionContainerBase(CType, Name, OwnerClass) {}
		TMemberFunctionContainer(Ret(TClass::* FuncPtr_)(Params...), const CClassBase* OwnerClass, const CMemberFunctionBase& CType, const std::string& Name, const std::vector<CMemberFunctionMetaBase*>& IN_Metas) :FuncPtr_(FuncPtr_), CMemberFunctionContainerBase(CType, Name, OwnerClass, IN_Metas) {}
		virtual std::string prints() const override
		{
			std::stringstream ss;
			ss << "<CFunction:";
			ss << GetName() << ">";
			ss << Type_.print() << ";\n";
			return ss.str();
		};
		virtual std::optional<CAny> CallFuncAny(void* Caller, const std::vector<AnyRef>& Params) const override
		{
			if (Params.size() != Type_.GetParams().size()) return std::nullopt;
			class_* CallerPtr = (class_*)Caller;
			for (size_t i = 0; i < Type_.GetParams().size(); i++)
			{
				if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType())))
				{
					std::cout << "实参" << std::string(Params[i].GetValType()) << "与形参" << std::string(Type_.GetParams()[i]) << "不兼容" << std::endl;
					return std::nullopt;
				}
			}
			return CallFuncAnyImp(*CallerPtr, Params, std::make_index_sequence<std::tuple_size_v<param_>>());
		}
		virtual std::optional<CAny> CallFuncAny(const AnyRef& Caller, const std::vector<AnyRef>& Params) const override
		{
			if (Params.size() != Type_.GetParams().size()) return std::nullopt;
			const CClassBase* InCallerType = dynamic_cast<const CClassBase*>(&Caller.GetValType().GetTypeConst());
			if (InCallerType == nullptr) return std::nullopt;
			void* Ptr = InCallerType->CastToParent(static_cast<const CClassBase*>(&Type_.GetTargetClass().GetTypeConst()), Caller.GetValue());
			if (Ptr == nullptr) return std::nullopt;
			class_* CallerPtr = (class_*)Ptr;
			for (size_t i = 0; i < Type_.GetParams().size(); i++)
			{
				if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType())))
				{
					std::cout << "实参" << std::string(Params[i].GetValType()) << "与形参" << std::string(Type_.GetParams()[i]) << "不兼容" << std::endl;
					return std::nullopt;
				}
			}
			return CallFuncAnyImp(*CallerPtr, Params, std::make_index_sequence<std::tuple_size_v<param_>>());
		}
	protected:
		Ret(TClass::* FuncPtr_)(Params...);
		template<size_t... Idx>
		CAny CallFuncAnyImp(class_& caller, const std::vector<AnyRef>& params, std::index_sequence<Idx...>) const
		{
			if constexpr (std::is_same_v<NoConstRet, void>)
			{
				(caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
				return CAny();
			}
			else
			{
				if constexpr (VReflectionTraits::remove_reference<Ret_>::is_reference)
				{
					return CAny::GetRefAny((caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...));
				}
				else
				{
					return (caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
				}
			}
		};
	};

	template<typename Ret, typename TClass, typename... Params>
	struct TMemberFunctionContainerConst : public CMemberFunctionContainerBase
	{
	public:
		using param_ = typename std::tuple<Params...>;
		using class_ = typename TClass;
		using NoConstRet = typename VReflectionTraits::remove_const<Ret>::type;
		using Ret_ = typename Ret;
		virtual ~TMemberFunctionContainerConst() {};
		TMemberFunctionContainerConst(Ret(TClass::* FuncPtr_)(Params...) const, const CClassBase* OwnerClass, const CMemberFunctionBase& CType, const std::string& Name) :FuncPtr_(FuncPtr_), CMemberFunctionContainerBase(CType, Name, OwnerClass) {}
		TMemberFunctionContainerConst(Ret(TClass::* FuncPtr_)(Params...) const, const CClassBase* OwnerClass, const CMemberFunctionBase& CType, const std::string& Name, const std::vector<CMemberFunctionMetaBase*>& IN_Metas) :FuncPtr_(FuncPtr_), CMemberFunctionContainerBase(CType, Name, OwnerClass, IN_Metas) {}
		virtual std::optional<CAny> CallFuncAny(void* Caller, const std::vector<AnyRef>& Params) const override
		{
			if (Params.size() != Type_.GetParams().size()) return std::nullopt;
			class_* CallerPtr = (class_*)Caller;
			for (size_t i = 0; i < Type_.GetParams().size(); i++)
			{
				if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType())))
				{
					std::cout << "实参" << std::string(Params[i].GetValType()) << "与形参" << std::string(Type_.GetParams()[i]) << "不兼容" << std::endl;
					return std::nullopt;
				}
			}
			return CallFuncAnyImp(*CallerPtr, Params, std::make_index_sequence<std::tuple_size_v<param_>>());
		}
		virtual std::optional<CAny> CallFuncAny(const AnyRef& Caller, const std::vector<AnyRef>& Params) const override
		{
			if (Params.size() != Type_.GetParams().size()) return std::nullopt;
			const CClassBase* InCallerType = dynamic_cast<const CClassBase*>(&Caller.GetValType().GetTypeConst());
			if (InCallerType == nullptr) return std::nullopt;
			void* Ptr = InCallerType->CastToParent(static_cast<const CClassBase*>(&Type_.GetTargetClass().GetTypeConst()), Caller.GetValue());
			if (Ptr == nullptr) return std::nullopt;
			class_* CallerPtr = (class_*)Ptr;
			for (size_t i = 0; i < Type_.GetParams().size(); i++)
			{
				if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType())))
				{
					std::cout << "实参" << std::string(Params[i].GetValType()) << "与形参" << std::string(Type_.GetParams()[i]) << "不兼容" << std::endl;
					return std::nullopt;
				}
			}
			return CallFuncAnyImp(*CallerPtr, Params, std::make_index_sequence<std::tuple_size_v<param_>>());
		}
		virtual std::string prints() const override
		{
			std::stringstream ss;
			ss << "<CFunction:";
			ss << GetName() << ">";
			ss << Type_.print() << ";\n";
			return ss.str();
		};
	protected:
		Ret(TClass::* FuncPtr_)(Params...) const;
		template<size_t... Idx>
		CAny CallFuncAnyImp(class_& caller, const std::vector<AnyRef>& params, std::index_sequence<Idx...>) const
		{
			if constexpr (std::is_same_v<NoConstRet, void>)
			{
				(caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
				return CAny();
			}
			else
			{
				if constexpr (VReflectionTraits::remove_reference<Ret_>::is_reference)
				{
					return CAny::GetRefAny((caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...));
				}
				else
				{
					return (caller.*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
				}
			}
		};
	};
}