// Copyright Liquid Fish. All Rights Reserved.

#include "Generic/GenericDebugUtils.h"

FString DumpGenericValue(const TCHAR* VariableName, const FGeneric& VariableValue)
{
	const FString NamePrefix = (VariableName && *VariableName)
		? FString::Printf(TEXT("[%s] = "), VariableName)
		: FString();
	const TCHAR* Prefix = *NamePrefix;

	if (!VariableValue.GetStringData().IsEmpty())
	{
		return FString::Printf(TEXT("%s\"%s\" (String)"), Prefix, *VariableValue.GetStringData());
	}

	if (const int32 BinarySize = VariableValue.GetPlainSize())
	{
		const uint8* Data = static_cast<const uint8*>(VariableValue.GetPlainData());
		FString HexString;
		for (int32 i = 0; i < BinarySize; ++i)
		{
			HexString += FString::Printf(TEXT("%02X "), Data[i]);
		}
		HexString.TrimEndInline();

		const FString TypeInfo = FString::Printf(TEXT("Binary (%d bytes): %s"), BinarySize, *HexString);

		if (BinarySize == 4)
		{
			return FString::Printf(TEXT("%s%s | float: %f, int32: %d, uint32: %u"),
				Prefix, *TypeInfo,
				VariableValue.As<float>(), VariableValue.As<int32>(), VariableValue.As<uint32>());
		}
		if (BinarySize == 8)
		{
			return FString::Printf(TEXT("%s%s | double: %lf, int64: %lld, uint64: %llu"),
				Prefix, *TypeInfo,
				VariableValue.As<double>(), VariableValue.As<int64>(), VariableValue.As<uint64>());
		}
		if (BinarySize == 2)
		{
			return FString::Printf(TEXT("%s%s | int16: %d, uint16: %u"),
				Prefix, *TypeInfo,
				VariableValue.As<int16>(), VariableValue.As<uint16>());
		}
		if (BinarySize == 1)
		{
			return FString::Printf(TEXT("%s%s | int8: %d, uint8: %u, bool: %s"),
				Prefix, *TypeInfo,
				VariableValue.As<int8>(), VariableValue.As<uint8>(),
				VariableValue.As<bool>() ? TEXT("true") : TEXT("false"));
		}

		return FString::Printf(TEXT("%s%s"), Prefix, *TypeInfo);
	}

	return NamePrefix.IsEmpty() ? FString(TEXT("null")) : FString::Printf(TEXT("%snull"), Prefix);
}

void LogGenericValueDetails(const TCHAR* VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix)
{
#if !NO_LOGGING
	UE_LOG(LogMaid, Log, TEXT("%s%s"), LogPrefix, *DumpGenericValue(VariableName, VariableValue));
#endif
}

void LogGenericValueDetails(const FName& VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix)
{
	LogGenericValueDetails(*VariableName.ToString(), VariableValue, LogPrefix);
}

void LogGenericValueDetails(const FString& VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix)
{
	LogGenericValueDetails(*VariableName, VariableValue, LogPrefix);
}
