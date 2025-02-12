// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEventDispatcherTypes.h"
#include "Templates/SharedPointer.h"
#include "Templates/Function.h"

// Auxiliary functions for type checking.
template<typename T>
struct IsPointer : std::integral_constant<bool, std::is_pointer<T>::value> {};

template<typename T>
constexpr bool IsPointerV = IsPointer<T>::value;

template<typename... Args>
struct AllAreNotPointer;

template<>
struct AllAreNotPointer<> : std::true_type {};

template<typename First, typename... Rest>
struct AllAreNotPointer<First, Rest...>: std::integral_constant<bool, !IsPointerV<First>&& AllAreNotPointer<Rest...>::value> {};


//Macros to simplify AddListener and Commit calls.

//Macros for listening to the event.
#define GLOBAL_EVENT_LISTEN(Context, Namespace, EventName, Func) if (FGlobalEventDispatcherHandler::Get().IsValid() && Context) FGlobalEventDispatcherHandler::Get()->AddListen(Context, Namespace, EventName, Func)

//Macros for calling an event without arguments.
#define GLOBAL_EVENT_COMMIT(Namespace, EventName) if (FGlobalEventDispatcherHandler::Get().IsValid()) FGlobalEventDispatcherHandler::Get()->Commit(Namespace, EventName)

//Macros for calling an event with arguments.
#define GLOBAL_EVENT_COMMIT_ARGS(Namespace, EventName, ...) if (FGlobalEventDispatcherHandler::Get().IsValid()) FGlobalEventDispatcherHandler::Get()->Commit(Namespace, EventName, __VA_ARGS__)

//Macros for deleting an associated event.
#define GLOBAL_EVENT_REMOVE_LISTEN(Context) if (FGlobalEventDispatcherHandler::Get().IsValid() && Context) FGlobalEventDispatcherHandler::Get()->RemoveListen(Context)


//The main class of the event handler. Use the macros "GLOBAL_EVENT_LISTEN", "GLOBAL_EVENT_REMOVE", "GLOBAL_EVENT_COMMIT", "GLOBAL_EVENT_COMMIT_ARGD" to interact with the handler.
class GLOBALEVENTDISPATCHER_API FGlobalEventDispatcherHandler
{
public:
    FGlobalEventDispatcherHandler();
    ~FGlobalEventDispatcherHandler();

	static TSharedPtr<FGlobalEventDispatcherHandler> Get();

    template<typename ClassType, typename... Args>
    void AddListen(ClassType* Context, const FName& Namespace, const FName& EventName, void (ClassType::* Func)(Args...))
    {
        static_assert(AllAreNotPointer<Args...>::value, "Arguments to AddListen cannot be pointers");

        if (!Context || !Func) return;

        TSharedPtr<IGlobalDelegate> DelegatePtr = MakeShareable(new TGlobalDelegate<ClassType, Args...>(Context, Func));
        FDelegateKey Key = { Namespace, EventName };

        DelegateMap.FindOrAdd(Key).Add(DelegatePtr);
    }

    template<typename... Args>
    void Commit(const FName& Namespace, const FName& EventName, Args... args)
    {
        static_assert(AllAreNotPointer<Args...>::value, "Arguments to Commit cannot be pointers");

        FDelegateKey Key = { Namespace, EventName };
        if (TArray<TSharedPtr<IGlobalDelegate>>* DelegateArray = DelegateMap.Find(Key))
        {
            TArray<TSharedPtr<IGlobalDelegate>> DelegatesCopy = *DelegateArray;

            if constexpr (sizeof...(args) > 0)
            {
                TArray<FUniversalVariant> ParamsArray;
                (ParamsArray.Add(FUniversalVariant(args)), ...); //Saving arguments to an array

                for (TSharedPtr<IGlobalDelegate>& DelegatePtr : DelegatesCopy)
                    if (DelegatePtr.IsValid())
                        DelegatePtr->Execute(ParamsArray);
            }
            else
            {
                for (TSharedPtr<IGlobalDelegate>& DelegatePtr : DelegatesCopy)
                    if (DelegatePtr.IsValid())
                        DelegatePtr->Execute({});
            }
        }
    }

    void RemoveListen(const void* Context);

private:
    TMap<FDelegateKey, TArray<TSharedPtr<IGlobalDelegate>>> DelegateMap;
};
