// Copyright Liquid Fish. All Rights Reserved.

#include "Generic/GenericDebugUtils.h"

void LogGenericValueDetails(const FName& VariableName, const FGeneric& VariableValue, const TCHAR* LogPrefix)
{
#if !NO_LOGGING
	if (!VariableValue.GetStringData().IsEmpty())
	{
		UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = \"%s\" (String)"),
			LogPrefix, *VariableName.ToString(), *VariableValue.GetStringData());
	}
	else if (int32 BinarySize = VariableValue.GetPlainSize())
	{
		// Convert binary data to hex string
		const uint8* Data = static_cast<const uint8*>(VariableValue.GetPlainData());
		FString HexString;
		for (int32 i = 0; i < BinarySize; ++i)
		{
			HexString += FString::Printf(TEXT("%02X "), Data[i]);
		}
		HexString.TrimEndInline();

		FString TypeInfo = FString::Printf(TEXT("Binary (%d bytes): %s"), BinarySize, *HexString);

		// Try to interpret as common types using As<>() method
		if (BinarySize == 4)
		{
			float AsFloat = VariableValue.As<float>();
			int32 AsInt32 = VariableValue.As<int32>();
			uint32 AsUInt32 = VariableValue.As<uint32>();
			UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = %s | float: %f, int32: %d, uint32: %u"),
				LogPrefix, *VariableName.ToString(), *TypeInfo, AsFloat, AsInt32, AsUInt32);
		}
		else if (BinarySize == 8)
		{
			double AsDouble = VariableValue.As<double>();
			int64 AsInt64 = VariableValue.As<int64>();
			uint64 AsUInt64 = VariableValue.As<uint64>();
			UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = %s | double: %lf, int64: %lld, uint64: %llu"),
				LogPrefix, *VariableName.ToString(), *TypeInfo, AsDouble, AsInt64, AsUInt64);
		}
		else if (BinarySize == 2)
		{
			int16 AsInt16 = VariableValue.As<int16>();
			uint16 AsUInt16 = VariableValue.As<uint16>();
			UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = %s | int16: %d, uint16: %u"),
				LogPrefix, *VariableName.ToString(), *TypeInfo, AsInt16, AsUInt16);
		}
		else if (BinarySize == 1)
		{
			int8 AsInt8 = VariableValue.As<int8>();
			uint8 AsUInt8 = VariableValue.As<uint8>();
			bool AsBool = VariableValue.As<bool>();
			UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = %s | int8: %d, uint8: %u, bool: %s"),
				LogPrefix, *VariableName.ToString(), *TypeInfo, AsInt8, AsUInt8, AsBool ? TEXT("true") : TEXT("false"));
		}
		else
		{
			// Generic binary data
			UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = %s"),
				LogPrefix, *VariableName.ToString(), *TypeInfo);
		}
	}
	else
	{
		// Generic binary data
		UE_LOG(LogMAID, Log, TEXT("%sStackValue: [%s] = null"), 
			LogPrefix, *VariableName.ToString());
	}
#endif
}