#pragma once

#include "Variable_traits.h"
#include "TypeFactory.h"
#include "Type.h"
#include <vector>
#include <tuple>
#include <optional>
#include "StaticMember.h"
#include "StaticFunctionMeta.h"

namespace NLEngine
{

	class CClassBase;

	class FStaticFunctionBase : public CType
	{
	public:
		FStaticFunctionBase(const TVariableTypeContainer<CType>& Result, std::vector<TVariableTypeContainer<CType>> Params, const std::string& Name, const std::string& RealName, const type_info& type_info_) :
			Result_(Result), Params_(Params), CType(Name, RealName, type_info_, 0, true) {
		};
		virtual std::string print() const
		{
			return name_;
		}
		const TVariableTypeContainer<CType>& GetResult() const { return Result_; };
		const std::vector<TVariableTypeContainer<CType>>& GetParams() const { return Params_; };
	protected:
		const TVariableTypeContainer<CType> Result_;
		std::vector<TVariableTypeContainer<CType>> Params_;
	};

	template<typename Ret, typename... Params>
	class StaticFunction : public FStaticFunctionBase
	{
	public:
		StaticFunction(const TVariableTypeContainer<CType>& Result, std::vector<TVariableTypeContainer<CType>> Params) :
			FStaticFunctionBase(Result, Params, typeid(Type_).name(), typeid(Type_).name(), typeid(Type_)) {
		}
		using Type_ = typename Ret(*)(Params...);
		using Ret_ = typename Ret;
		using ParamsTuple = typename std::tuple<Params...>;
	};

	template<typename Ret, typename... Params>
	class TStaticFuncFactory
	{
	public:
		using Type_ = typename Ret(*)(Params...);
		using args_tuple = typename std::tuple<Params...>;

		using RetT = typename VReflectionTraits::remove_const<Ret>::type;
		static constexpr bool RetConst = VReflectionTraits::remove_const<Ret>::is_const;
		TStaticFuncFactory()
		{
			info_ = GetTypeManager().GetOrRegistor<StaticFunction<Ret, Params...>>(TVariableTypeContainer<CType>(GetType<RetT>(), RetConst), GetParams());
		};
		static TStaticFuncFactory& Instance()
		{
			static TStaticFuncFactory inst;
			TStaticFuncFactory* instPtr = &inst;
			return inst;
		}
		StaticFunction<Ret, Params...>& Info() { return *info_; }
	private:
		typename StaticFunction<Ret, Params...>* info_;
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

	template<typename Ret, typename... Params>
	class TFactoryGetter<Ret(*)(Params...)>
	{
	public:
		using GetFactory = TStaticFuncFactory<Ret, Params...>;
	};

	class CStaticFunctionContainerBase : public CStaticMember
	{
	public:
		virtual ~CStaticFunctionContainerBase() {};
		CStaticFunctionContainerBase(const CClassBase* OwnerClass, const FStaticFunctionBase& CType, const std::string& Name) :OwnerClass_(OwnerClass), Type_(CType), Name_(Name) {}
		CStaticFunctionContainerBase(const CClassBase* OwnerClass, const FStaticFunctionBase& CType, const std::string& Name, const std::vector<CStaticFunctionMetaBase*>& IN_Metas) :OwnerClass_(OwnerClass), Type_(CType), Name_(Name), Metas(IN_Metas)
		{
			for (auto i : Metas)
			{
				i->AttachedtoType(this);
			}
			return;
		}
		const std::string& GetName() const { return Name_; };
		const FStaticFunctionBase& GetType() const { return Type_; };
		const CClassBase* GetOwnerPtr() const { return OwnerClass_; };
		template<typename... Params>
		bool CallFunc(Params&&... args) const
		{
			auto valRet = CallFuncAny({ args... });
			if (valRet)
			{
				return true;
			}
			return false;
		};
		template<typename Ret_T, typename... Params>
		std::optional<Ret_T> CallFunc(Params&&... args) const
		{
			auto valRet = CallFuncAny({ args ... });
			if (valRet)
			{
				return (Ret_T)valRet.value();
			}
			return std::nullopt;
		};
		virtual std::optional<CAny> CallFuncAny(const std::vector<AnyRef>& Params) const = 0;
		virtual std::string prints() const = 0;
	protected:
		const CClassBase* OwnerClass_;
		const FStaticFunctionBase& Type_;
		std::string Name_;
		std::vector<CStaticFunctionMetaBase*> Metas;
	};

	template<typename Ret, typename TClass, typename... Params>
	class TStaticFunctionContainer : public CStaticFunctionContainerBase
	{
	public:
		using param_ = typename std::tuple<Params...>;
		using class_ = typename TClass;
		using NoConstRet = typename VReflectionTraits::remove_const<Ret>::type;
		using Ret_ = typename Ret;
		virtual ~TStaticFunctionContainer() {};
		TStaticFunctionContainer(Ret(*FuncPtr_)(Params...), const CClassBase* OwnerClass, const FStaticFunctionBase& CType, const std::string& Name) :FuncPtr_(FuncPtr_), CStaticFunctionContainerBase(OwnerClass, CType, Name) {}
		TStaticFunctionContainer(Ret(*FuncPtr_)(Params...), const CClassBase* OwnerClass, const FStaticFunctionBase& CType, const std::string& Name, const std::vector<CStaticFunctionMetaBase*>& IN_Metas) :FuncPtr_(FuncPtr_), CStaticFunctionContainerBase(OwnerClass, CType, Name, IN_Metas) {}
		virtual std::optional<CAny> CallFuncAny(const std::vector<AnyRef>& Params) const override
		{
			if (Params.size() != Type_.GetParams().size()) return std::nullopt;
			for (size_t i = 0; i < Type_.GetParams().size(); i++)
			{
				if (!(Type_.GetParams()[i].InputRequire(Params[i].GetValType()))) return std::nullopt;
			}
			return CallFuncAnyImp(Params, std::make_index_sequence<std::tuple_size_v<param_>>());
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
		Ret(*FuncPtr_)(Params...);
		template<size_t... Idx>
		CAny CallFuncAnyImp(const std::vector<AnyRef>& params, std::index_sequence<Idx...>) const
		{
			if constexpr (std::is_same_v<NoConstRet, void>)
			{
				(*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
				return CAny();
			}
			else
			{
				if constexpr (VReflectionTraits::remove_reference<Ret_>::is_reference)
				{
					return CAny::GetRefAny((*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...));
				}
				else
				{
					return (*FuncPtr_)(BlackMagicVoidPtrCast<std::tuple_element_t<Idx, param_>>(params[Idx].GetValue())...);
				}
			}
		};
	};

}
