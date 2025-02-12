// Fill out your copyright notice in the Description page of Project Settings.


#include "GlobalEventDispatcherHandler.h"

FGlobalEventDispatcherHandler::FGlobalEventDispatcherHandler()
{
}

FGlobalEventDispatcherHandler::~FGlobalEventDispatcherHandler()
{
    DelegateMap.Empty();
}

TSharedPtr<FGlobalEventDispatcherHandler> FGlobalEventDispatcherHandler::Get()
{
    static TSharedPtr<FGlobalEventDispatcherHandler> Instance;
    if (!Instance.IsValid())
    {
        Instance = MakeShareable(new FGlobalEventDispatcherHandler());
    }
	return Instance;
}

void FGlobalEventDispatcherHandler::RemoveListen(const void* Context)
{
    if (!Context) return;

    for (auto& Pair : DelegateMap)
    {
        Pair.Value.RemoveAll([Context](const TSharedPtr<IGlobalDelegate>& Delegate)
        {
            return Delegate.IsValid() && Delegate->GetOwner() == Context;
        });
    }
}