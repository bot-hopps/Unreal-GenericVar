// Copyright Liquid Fish. All Rights Reserved.

#pragma once

#include "Core/MaidCore.h"
#include "Generic/Generic.h"

void MAIDGAME_API LogGenericValueDetails(const TCHAR* VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix = TEXT(""));
void MAIDGAME_API LogGenericValueDetails(const FName& VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix = TEXT(""));
void MAIDGAME_API LogGenericValueDetails(const FString& VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix = TEXT(""));

FString MAIDGAME_API DumpGenericValue(const TCHAR* VariableName, const FGeneric& VariableValue);
