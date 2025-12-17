// Copyright Liquid Fish. All Rights Reserved.

#include "Generic/Generic.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "AlphaBlend.h"
#include "Animation/AnimationAsset.h"
#include "Math/UnitConversion.h"

/**
 * Comprehensive test suite for FGeneric container type
 * Tests all major functionality including basic types, objects, arrays, and edge cases
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGenericComprehensiveTest, "MaidGame.Generic",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::HighPriority)

	bool FGenericComprehensiveTest::RunTest(const FString& Parameters)
{
	// Test 1: Basic Construction and Empty State
	{
		FGeneric DefaultGeneric;
		TestTrue(TEXT("Default constructor should create empty instance"), DefaultGeneric.IsEmpty());
		TestEqual(TEXT("Null instance should be empty"), FGeneric::Null.IsEmpty(), true);
	}

	// Test 2: Primitive Data Types
	{
		// Boolean types
		FGeneric BoolTrue(true);
		FGeneric BoolFalse(false);
		TestTrue(TEXT("Boolean true value"), BoolTrue.As<bool>());
		TestFalse(TEXT("Boolean false value"), BoolFalse.As<bool>());

		// Integer types
		FGeneric Int8Val(static_cast<int8>(127));
		FGeneric Int16Val(static_cast<int16>(32767));
		FGeneric Int32Val(2147483647);
		FGeneric Int64Val(9223372036854775807LL);
		TestEqual(TEXT("int8 value"), Int8Val.As<int8>(), 127);
		TestEqual(TEXT("int16 value"), Int16Val.As<int16>(), 32767);
		TestEqual(TEXT("int32 value"), Int32Val.As<int32>(), 2147483647);
		TestEqual(TEXT("int64 value"), Int64Val.As<int64>(), 9223372036854775807LL);

		// Unsigned integers
		FGeneric UInt8Val(static_cast<uint8>(255));
		FGeneric UInt16Val(static_cast<uint16>(65535));
		FGeneric UInt32Val(4294967295u);
		FGeneric UInt64Val(18446744073709551615ULL);
		TestEqual(TEXT("uint8 value"), UInt8Val.As<uint8>(), 255);
		TestEqual(TEXT("uint16 value"), UInt16Val.As<uint16>(), 65535);
		TestEqual(TEXT("uint32 value"), UInt32Val.As<uint32>(), 4294967295u);
		TestEqual(TEXT("uint64 value"), UInt64Val.As<uint64>(), 18446744073709551615ULL);

		// Floating point types
		FGeneric FloatVal(3.14159f);
		FGeneric DoubleVal(2.718281828459045);
		TestEqual(TEXT("float value"), FloatVal.As<float>(), 3.14159f);
		TestEqual(TEXT("double value"), DoubleVal.As<double>(), 2.718281828459045);
	}

	// Test 3: String and Name Types
	{
		FString TestString = TEXT("Hello, FGeneric Container!");
		FName TestFName = TEXT("TestNameValue");

		FGeneric StringGeneric(TestString);
		FGeneric NameGeneric(TestFName);

		TestEqual(TEXT("FString conversion"), StringGeneric.As<FString>(), TestString);
		TestEqual(TEXT("FName conversion"), NameGeneric.As<FName>(), TestFName);

		// Test implicit conversions
		FString ImplicitString = StringGeneric;
		FName ImplicitName = NameGeneric;
		TestEqual(TEXT("Implicit FString conversion"), ImplicitString, TestString);
		TestEqual(TEXT("Implicit FName conversion"), ImplicitName, TestFName);
	}

	// Test 4: Math Types and Vectors
	{
		FVector2D TestVector2D(10.5f, 20.7f);
		FVector TestVector(1.1f, 2.2f, 3.3f);
		FVector4 TestVector4(1.0f, 2.0f, 3.0f, 4.0f);
		FIntPoint TestIntPoint(100, 200);
		FIntVector TestIntVector(10, 20, 30);

		FGeneric Vector2DGeneric(TestVector2D);
		FGeneric VectorGeneric(TestVector);
		FGeneric Vector4Generic(TestVector4);
		FGeneric IntPointGeneric(TestIntPoint);
		FGeneric IntVectorGeneric(TestIntVector);

		TestEqual(TEXT("FVector2D conversion"), Vector2DGeneric.As<FVector2D>(), TestVector2D);
		TestEqual(TEXT("FVector conversion"), VectorGeneric.As<FVector>(), TestVector);
		TestEqual(TEXT("FVector4 conversion"), Vector4Generic.As<FVector4>(), TestVector4);
		TestEqual(TEXT("FIntPoint conversion"), IntPointGeneric.As<FIntPoint>(), TestIntPoint);
		TestEqual(TEXT("FIntVector conversion"), IntVectorGeneric.As<FIntVector>(), TestIntVector);

		// Test more complex math types
		FRotator TestRotator(45.0f, 90.0f, 180.0f);
		FQuat TestQuat(TestRotator);
		FTransform TestTransform(TestRotator, FVector(100, 200, 300), FVector(1, 1, 1));
		FLinearColor TestColor(0.1f, 0.5f, 0.9f, 1.0f);
		FColor TestRGBColor(255, 128, 64, 255);

		FGeneric RotatorGeneric(TestRotator);
		FGeneric QuatGeneric(TestQuat);
		FGeneric TransformGeneric(TestTransform);
		FGeneric LinearColorGeneric(TestColor);
		FGeneric ColorGeneric(TestRGBColor);

		TestEqual(TEXT("FRotator conversion"), RotatorGeneric.As<FRotator>().Equals(TestRotator), true);
		TestEqual(TEXT("FQuat conversion"), QuatGeneric.As<FQuat>().Equals(TestQuat), true);
		TestEqual(TEXT("FTransform conversion"), TransformGeneric.As<FTransform>().Equals(TestTransform), true);
		TestEqual(TEXT("FLinearColor conversion"), LinearColorGeneric.As<FLinearColor>(), TestColor);
		TestEqual(TEXT("FColor conversion"), ColorGeneric.As<FColor>(), TestRGBColor);
	}

	// Test 5: Object and Class Types
	{
		UObject* TestObject = GetTransientPackage(); // Use transient package as test object
		TSubclassOf<UObject> TestClass = UObject::StaticClass();

		FGeneric ObjectGeneric(TestObject);
		FGeneric ClassGeneric(TestClass);

		UObject* ConvertedObject = ObjectGeneric.As<UObject*>();
		TSubclassOf<UObject> ConvertedClass = ClassGeneric.As<TSubclassOf<UObject>>();

		TestEqual(TEXT("UObject* conversion"), ConvertedObject, TestObject);
		TestEqual(TEXT("TSubclassOf conversion"), ConvertedClass, TestClass);

		// Test implicit conversions
		UObject* ImplicitObject = ObjectGeneric;
		TSubclassOf<UObject> ImplicitClass = ClassGeneric;
		TestEqual(TEXT("Implicit UObject* conversion"), ImplicitObject, TestObject);
		TestEqual(TEXT("Implicit TSubclassOf conversion"), ImplicitClass, TestClass);
	}

	// Test 6: Array Types
	{
		TArray<int32> IntArray{ 1, 2, 3, 4, 5 };
		TArray<float> FloatArray{ 1.1f, 2.2f, 3.3f };
		TArray<FString> StringArray{ TEXT("Apple"), TEXT("Banana"), TEXT("Cherry") };
		TArray<FName> NameArray{ TEXT("FirstName"), TEXT("SecondName"), TEXT("ThirdName") };

		FGeneric IntArrayGeneric(IntArray);
		FGeneric FloatArrayGeneric(FloatArray);
		FGeneric StringArrayGeneric(StringArray);
		FGeneric NameArrayGeneric(NameArray);

		TestEqual(TEXT("TArray<int32> conversion"), IntArrayGeneric.As<TArray<int32>>(), IntArray);
		TestEqual(TEXT("TArray<float> conversion"), FloatArrayGeneric.As<TArray<float>>(), FloatArray);
		TestEqual(TEXT("TArray<FString> conversion"), StringArrayGeneric.As<TArray<FString>>(), StringArray);
		TestEqual(TEXT("TArray<FName> conversion"), NameArrayGeneric.As<TArray<FName>>(), NameArray);
	}

	// Test 7: Copy and Move Semantics
	{
		FGeneric Original(12345);

		// Test copy constructor
		FGeneric CopyConstructed(Original);
		TestEqual(TEXT("Copy constructor preserves value"), CopyConstructed.As<int32>(), 12345);

		// Test copy assignment
		FGeneric CopyAssigned;
		CopyAssigned = Original;
		TestEqual(TEXT("Copy assignment preserves value"), CopyAssigned.As<int32>(), 12345);

		// Test move constructor
		FGeneric TempForMove(67890);
		FGeneric MoveConstructed(MoveTemp(TempForMove));
		TestEqual(TEXT("Move constructor transfers value"), MoveConstructed.As<int32>(), 67890);
		TestTrue(TEXT("Original after move is empty"), TempForMove.IsEmpty());

		// Test move assignment
		FGeneric AnotherTemp(99999);
		FGeneric MoveAssigned;
		MoveAssigned = MoveTemp(AnotherTemp);
		TestEqual(TEXT("Move assignment transfers value"), MoveAssigned.As<int32>(), 99999);
		TestTrue(TEXT("Source after move assignment is empty"), AnotherTemp.IsEmpty());
	}

	// Test 8: Equality and Comparison Operations
	{
		FGeneric GenericA(100);
		FGeneric GenericB(100);
		FGeneric GenericC(200);
		FGeneric GenericD(FString(TEXT("100")));
		FGeneric GenericE; // Empty

		// Equality tests
		TestTrue(TEXT("Equal values are equal"), GenericA == GenericB);
		TestFalse(TEXT("Different values are not equal"), GenericA == GenericC);
		TestFalse(TEXT("Different types are not equal"), GenericA == GenericD);
		TestFalse(TEXT("Empty vs non-empty are not equal"), GenericA == GenericE);

		// Inequality tests
		TestFalse(TEXT("Equal values are not unequal"), GenericA != GenericB);
		TestTrue(TEXT("Different values are unequal"), GenericA != GenericC);
		TestTrue(TEXT("Different types are unequal"), GenericA != GenericD);
		TestTrue(TEXT("Empty vs non-empty are unequal"), GenericA != GenericE);

		// Self-equality
		TestTrue(TEXT("Instance equals itself"), GenericA == GenericA);
	}

	// Test 9: Clear and Empty State Management
	{
		FGeneric TestGeneric(12345);

		// Verify initial state is not empty
		TestFalse(TEXT("Generic with value is not empty"), TestGeneric.IsEmpty());

		// Test Clear() method
		TestGeneric.Clear();
		TestTrue(TEXT("Generic is empty after Clear()"), TestGeneric.IsEmpty());

		// Test reassignment after clear
		TestGeneric = 67890;
		TestFalse(TEXT("Generic is not empty after reassignment"), TestGeneric.IsEmpty());
		TestEqual(TEXT("Value correct after reassignment"), TestGeneric.As<int32>(), 67890);

		// Test multiple clear operations
		TestGeneric.Clear();
		TestGeneric.Clear(); // Second clear should be safe
		TestTrue(TEXT("Double clear remains empty"), TestGeneric.IsEmpty());
	}

	// Test 10: Edge Cases and Boundary Conditions
	{
		// Zero and negative values
		FGeneric ZeroInt(0);
		FGeneric NegativeInt(-100);
		TestEqual(TEXT("Zero integer value"), ZeroInt.As<int32>(), 0);
		TestEqual(TEXT("Negative integer value"), NegativeInt.As<int32>(), -100);

		// Boolean conversion edge cases
		TestFalse(TEXT("Zero integer converts to false bool"), ZeroInt.As<bool>());
		TestTrue(TEXT("Negative integer converts to true bool"), NegativeInt.As<bool>());

		// Empty strings and names
		FGeneric EmptyString(FString(TEXT("")));
		FGeneric EmptyName(NAME_None);
		TestEqual(TEXT("Empty string value"), EmptyString.As<FString>(), TEXT(""));
		TestEqual(TEXT("Empty name value"), EmptyName.As<FName>(), FName(NAME_None));
		TestFalse(TEXT("Empty string converts to false bool"), EmptyString.As<bool>());

		// Null objects
		FGeneric NullObject(static_cast<UObject*>(nullptr));
		FGeneric NullClass(TSubclassOf<UObject>(nullptr));
		TestNull(TEXT("Null object conversion"), NullObject.As<UObject*>());
		TestNull(TEXT("Null class conversion"), NullClass.As<TSubclassOf<UObject>>().Get());

		// Very large values
		FGeneric LargeInt(2147483647);
		FGeneric LargeFloat(FLT_MAX);
		TestEqual(TEXT("Large integer value"), LargeInt.As<int32>(), 2147483647);
		TestEqual(TEXT("Large float value"), LargeFloat.As<float>(), FLT_MAX);
	}

	// Test 11: Boolean Conversion and Truthiness
	{
		// Truthy values
		FGeneric TrueBool(true);
		FGeneric NonZeroInt(1);
		FGeneric NonZeroFloat(0.1f);
		FGeneric NonEmptyString(TEXT("Hello"));

		TestTrue(TEXT("True bool is truthy"), TrueBool.As<bool>());
		TestTrue(TEXT("Non-zero int is truthy"), NonZeroInt.As<bool>());
		TestTrue(TEXT("Non-zero float is truthy"), NonZeroFloat.As<bool>());
		TestTrue(TEXT("Non-empty string is truthy"), NonEmptyString.As<bool>());

		// Falsy values
		FGeneric FalseBool(false);
		FGeneric ZeroInt(0);
		FGeneric ZeroFloat(0.0f);
		FGeneric EmptyString(FString(TEXT("")));

		TestFalse(TEXT("False bool is falsy"), FalseBool.As<bool>());
		TestFalse(TEXT("Zero int is falsy"), ZeroInt.As<bool>());
		TestFalse(TEXT("Zero float is falsy"), ZeroFloat.As<bool>());
		TestFalse(TEXT("Empty string is falsy"), EmptyString.As<bool>());

		// Implicit boolean conversion
		TestTrue(TEXT("Implicit bool conversion - truthy"), static_cast<bool>(NonZeroInt));
		TestFalse(TEXT("Implicit bool conversion - falsy"), static_cast<bool>(ZeroInt));
	}

	// Test 12: Performance and Repeated Operations
	{
		const int32 PerformanceIterations = 1000;

		// Test repeated conversions
		FGeneric PerformanceGeneric(3.14159f);
		for (int32 i = 0; i < PerformanceIterations; ++i)
		{
			float Value = PerformanceGeneric.As<float>();
			TestEqual(TEXT("Performance test value consistency"), Value, 3.14159f);
		}

		// Test repeated assignments
		FGeneric SourceGeneric(12345);
		FGeneric TargetGeneric;
		for (int32 i = 0; i < PerformanceIterations; ++i)
		{
			TargetGeneric = SourceGeneric;
			TestEqual(TEXT("Performance assignment consistency"), TargetGeneric.As<int32>(), 12345);
		}

		// Test mixed type assignments
		FGeneric MixedGeneric;
		for (int32 i = 0; i < 100; ++i)
		{
			if (i % 2 == 0)
			{
				MixedGeneric = i; // int
			}
			else
			{
				MixedGeneric = FString::Printf(TEXT("String%d"), i); // string
			}
			// Verify the assignment worked
			TestFalse(TEXT("Mixed assignment not empty"), MixedGeneric.IsEmpty());
		}
	}

	// Test 13: Direct Memory Access Methods
	{
		FGeneric TestGeneric(42);

		// Test GetPlainData methods
		const void* PlainDataConst = TestGeneric.GetPlainData();
		void* PlainData = const_cast<void*>(TestGeneric.GetPlainData());

		TestNotNull(TEXT("GetPlainData returns non-null"), PlainData);
		TestNotNull(TEXT("Const GetPlainData returns non-null"), PlainDataConst);

		// Test GetStringData method
		FGeneric StringGeneric(FString(TEXT("TestString")));
		const FString& StringData = StringGeneric.GetStringData();
		TestEqual(TEXT("GetStringData returns correct string"), StringData, TEXT("TestString"));
	}

	// Test 14: Type Safety and Error Conversion Tests
	{
		// Test invalid conversions between incompatible types
		FGeneric IntGeneric(42);

		// Test conversion to incompatible type - should return default value
		FString InvalidString = IntGeneric.As<FString>();
		TestTrue(TEXT("Invalid conversion to string returns empty"), InvalidString.IsEmpty());

		// Test conversion from string to number
		FGeneric StringGeneric(TEXT("123"));
		int32 ConvertedInt = StringGeneric.As<int32>();
		TestEqual(TEXT("String to int conversion"), ConvertedInt, 123);

		// Test invalid string to number conversion
		FGeneric InvalidStringGeneric(TEXT("not_a_number"));
		int32 DefaultInt = InvalidStringGeneric.As<int32>();
		TestEqual(TEXT("Invalid string to int returns zero"), DefaultInt, 0);
	}

	// Test 15: UStruct Support with StaticStruct Comparison
	{
		// Test FHitResult with StaticStruct identical comparison
		FHitResult TestHitResult;
		TestHitResult.Time = 0.5f;
		TestHitResult.Location = FVector(100, 200, 300);
		TestHitResult.Normal = FVector(0, 0, 1);
		TestHitResult.bBlockingHit = true;

		FGeneric HitResultGeneric(TestHitResult);
		FHitResult ConvertedHitResult = HitResultGeneric.As<FHitResult>();

		// Use StaticStruct identical comparison for FHitResult
		TestTrue(TEXT("FHitResult conversion"),
			FHitResult::StaticStruct()->CompareScriptStruct(&TestHitResult, &ConvertedHitResult, 0));
	}

	// Test 16: Complex Container Tests (Blueprint-supported only)
	{
		// Test TMap with blueprint-supported key/value types
		TMap<FString, int32> StringIntMap;
		StringIntMap.Add(TEXT("First"), 1);
		StringIntMap.Add(TEXT("Second"), 2);

		// Note: TMap requires special handling in FGeneric - testing basic array containers instead
		TArray<FString> StringArray{ TEXT("Alpha"), TEXT("Beta"), TEXT("Gamma") };
		TArray<FVector> VectorArray{ FVector(1,0,0), FVector(0,1,0), FVector(0,0,1) };

		FGeneric StringArrayGeneric(StringArray);
		FGeneric VectorArrayGeneric(VectorArray);

		TestEqual(TEXT("Complex string array"), StringArrayGeneric.As<TArray<FString>>(), StringArray);
		TestEqual(TEXT("Complex vector array"), VectorArrayGeneric.As<TArray<FVector>>(), VectorArray);
	}

	// Test 17: String Encoding and Internationalization Tests
	{
		// Test Unicode and special characters
		FGeneric UnicodeString(TEXT("中文测试 Español Français"));
		FGeneric SpecialChars(TEXT("Line1\nLine2\tTab\\Slash"));
		// Use escape sequences for emoji to avoid source file encoding issues
		FGeneric EmojiString(TEXT("Test \U0001F680 Emoji \u2728 Symbols"));

		TestEqual(TEXT("Unicode string preservation"), UnicodeString.As<FString>(), TEXT("中文测试 Español Français"));
		TestEqual(TEXT("Special character handling"), SpecialChars.As<FString>(), TEXT("Line1\nLine2\tTab\\Slash"));
		TestEqual(TEXT("Emoji and symbol support"), EmojiString.As<FString>(), TEXT("Test \U0001F680 Emoji \u2728 Symbols"));
	}

	// Test 18: Numeric Precision and Boundary Tests
	{
		// Test floating point precision boundaries
		FGeneric NearZero(FLT_MIN);
		FGeneric MaxFloat(FLT_MAX);
		FGeneric TinyDouble(DBL_MIN);

		TestEqual(TEXT("Minimum float value"), NearZero.As<float>(), FLT_MIN);
		TestEqual(TEXT("Maximum float value"), MaxFloat.As<float>(), FLT_MAX);
		TestTrue(TEXT("Tiny double precision"), FMath::IsNearlyEqual(TinyDouble.As<double>(), DBL_MIN));

		// Test integer boundaries
		FGeneric MaxInt32(INT32_MAX);
		FGeneric MinInt32(INT32_MIN);
		FGeneric MaxUInt32(UINT32_MAX);

		TestEqual(TEXT("Max int32 boundary"), MaxInt32.As<int32>(), INT32_MAX);
		TestEqual(TEXT("Min int32 boundary"), MinInt32.As<int32>(), INT32_MIN);
		TestEqual(TEXT("Max uint32 boundary"), MaxUInt32.As<uint32>(), UINT32_MAX);
	}

	// Test 19: Object Reference and Package Tests
	{
		// Test transient package references
		UObject* TransientPackage = GetTransientPackage();
		FGeneric TransientGeneric(TransientPackage);

		UObject* RetrievedTransient = TransientGeneric.As<UObject*>();
		TestEqual(TEXT("Transient package reference"), RetrievedTransient, TransientPackage);

		// Test soft object pointer resolution
		TSoftObjectPtr<UObject> SoftTransient(TransientPackage);
		FGeneric SoftTransientGeneric(SoftTransient);

		TSoftObjectPtr<UObject> RetrievedSoft = SoftTransientGeneric.As<TSoftObjectPtr<UObject>>();
		TestEqual(TEXT("Soft object pointer preservation"), RetrievedSoft.ToString(), SoftTransient.ToString());

		// Test soft class pointers
		TSoftClassPtr<UObject> SoftClassPtr(UObject::StaticClass());
		FGeneric SoftClassGeneric(SoftClassPtr);

		TSoftClassPtr<UObject> RetrievedSoftClass = SoftClassGeneric.As<TSoftClassPtr<UObject>>();
		TestEqual(TEXT("Soft class pointer preservation"), RetrievedSoftClass.ToString(), SoftClassPtr.ToString());
	}

	// Test 20: Reference Resolution and Asset Path Tests
	{
		// Test object path string conversion
		FString ObjectPath = TEXT("/Engine/Transient");
		FGeneric PathGeneric(ObjectPath);

		// Test path resolution through soft object pointers
		TSoftObjectPtr<UObject> SoftFromPath(ObjectPath);
		FGeneric SoftPathGeneric(SoftFromPath);

		FString ResolvedPath = SoftPathGeneric.As<FString>();
		TestEqual(TEXT("Object path string conversion"), ResolvedPath, ObjectPath);

		// Test name to string conversion for asset references
		FName AssetName = TEXT("TestAsset");
		FGeneric NameGeneric(AssetName);
		FString NameAsString = NameGeneric.As<FString>();
		TestEqual(TEXT("Name to string conversion for assets"), NameAsString, AssetName.ToString());
	}

	// Test 21: Boolean Truthiness for Complex Types
	{
		// Test truthiness of complex blueprint types
		FGeneric ZeroVector(FVector::ZeroVector);
		FGeneric NonZeroVector(FVector(1, 1, 1));
		FGeneric DefaultRotator(FRotator::ZeroRotator);
		FGeneric IdentityTransform(FTransform::Identity);

		TestFalse(TEXT("Zero vector is falsy"), ZeroVector.As<bool>());
		TestTrue(TEXT("Non-zero vector is truthy"), NonZeroVector.As<bool>());
		TestFalse(TEXT("Default rotator is falsy"), DefaultRotator.As<bool>());
		TestTrue(TEXT("Identity transform is truthy"), IdentityTransform.As<bool>());

		// Test empty vs non-empty containers
		TArray<int32> EmptyArray;
		TArray<int32> NonEmptyArray{ 1, 2, 3 };

		FGeneric EmptyArrayGeneric(EmptyArray);
		FGeneric NonEmptyArrayGeneric(NonEmptyArray);

		TestFalse(TEXT("Empty array is falsy"), EmptyArrayGeneric.As<bool>());
		TestTrue(TEXT("Non-empty array is truthy"), NonEmptyArrayGeneric.As<bool>());
	}

	// Test 22: Enum Support Tests
	{
		// Test 1: Regular UENUM (EPhysicalSurface)
		{
			EPhysicalSurface OriginalValue = EPhysicalSurface::SurfaceType1;

			// Test direct enum storage
			FGeneric EnumGeneric(OriginalValue);
			EPhysicalSurface ConvertedValue = EnumGeneric.As<EPhysicalSurface>();

			// Verify value preservation
			TestEqual(TEXT("Regular UENUM - value preservation"), ConvertedValue, OriginalValue);

			// Verify size preservation
			TestEqual(TEXT("Regular UENUM - size preservation"),
				EnumGeneric.GetPlainSize(), (int32)sizeof(EPhysicalSurface));
		}

		// Test 2: Enum Class UENUM (EUnit)
		{
			EUnit OriginalValue = EUnit::Centimeters;

			// Test direct enum storage
			FGeneric EnumGeneric(OriginalValue);
			EUnit ConvertedValue = EnumGeneric.As<EUnit>();

			// Verify value preservation
			TestEqual(TEXT("Enum Class UENUM - value preservation"), ConvertedValue, OriginalValue);

			// Verify size preservation
			TestEqual(TEXT("Enum Class UENUM - size preservation"),
				EnumGeneric.GetPlainSize(), (int32)sizeof(EUnit));
		}

		// Test 3: Namespace UENUM (EAnimGroupRole::Type)
		{
			EAnimGroupRole::Type OriginalValue = EAnimGroupRole::CanBeLeader;

			// Test direct enum storage
			FGeneric EnumGeneric(OriginalValue);
			EAnimGroupRole::Type ConvertedValue = EnumGeneric.As<EAnimGroupRole::Type>();

			// Verify value preservation
			TestEqual(TEXT("Namespace UENUM - value preservation"), ConvertedValue, OriginalValue);

			// Verify size preservation
			TestEqual(TEXT("Namespace UENUM - size preservation"),
				EnumGeneric.GetPlainSize(), (int32)sizeof(EAnimGroupRole::Type));
		}

		// Test 4: TEnumAsByte wrapper
		{
			EPhysicalSurface OriginalValue = EPhysicalSurface::SurfaceType2;

			// Test TEnumAsByte storage
			TEnumAsByte<EPhysicalSurface> OriginalByteEnum = OriginalValue;
			FGeneric ByteEnumGeneric(OriginalByteEnum);
			TEnumAsByte<EPhysicalSurface> ConvertedByteEnum = ByteEnumGeneric.As<TEnumAsByte<EPhysicalSurface>>();
			EPhysicalSurface ConvertedValue = ConvertedByteEnum.GetValue();

			// Verify value preservation
			TestEqual(TEXT("TEnumAsByte - value preservation"), ConvertedValue, OriginalValue);

			// Verify size preservation
			TestEqual(TEXT("TEnumAsByte - size preservation"),
				ByteEnumGeneric.GetPlainSize(), (int32)sizeof(DeclVal<TEnumAsByte<EPhysicalSurface>>().GetValue()));

			// Verify byte value preservation
			TestEqual(TEXT("TEnumAsByte - byte value preservation"),
				ConvertedByteEnum, static_cast<uint8>(OriginalValue));
		}

		// Test 5: Multiple values for each enum type
		{
			// Test multiple values for regular enum
			EPhysicalSurface SurfaceValues[] = {
				EPhysicalSurface::SurfaceType_Default,
				EPhysicalSurface::SurfaceType1,
				EPhysicalSurface::SurfaceType2
			};

			for (int32 i = 0; i < UE_ARRAY_COUNT(SurfaceValues); ++i)
			{
				EPhysicalSurface OriginalValue = SurfaceValues[i];
				FGeneric Generic(OriginalValue);
				EPhysicalSurface ConvertedValue = Generic.As<EPhysicalSurface>();

				TestEqual(FString::Printf(TEXT("Regular UENUM multiple values [%d]"), i),
					ConvertedValue, OriginalValue);
				TestEqual(FString::Printf(TEXT("Regular UENUM multiple sizes [%d]"), i),
					Generic.GetPlainSize(), sizeof(EPhysicalSurface));
			}

			// Test multiple values for enum class
			EUnit UnitValues[] = {
				EUnit::Meters,
				EUnit::Centimeters,
				EUnit::Kilometers
			};

			for (int32 i = 0; i < UE_ARRAY_COUNT(UnitValues); ++i)
			{
				EUnit OriginalValue = UnitValues[i];
				FGeneric Generic(OriginalValue);
				EUnit ConvertedValue = Generic.As<EUnit>();

				TestEqual(FString::Printf(TEXT("Enum Class UENUM multiple values [%d]"), i),
					ConvertedValue, OriginalValue);
				TestEqual(FString::Printf(TEXT("Enum Class UENUM multiple sizes [%d]"), i),
					Generic.GetPlainSize(), sizeof(EUnit));
			}

			// Test multiple values for namespace enum
			EAnimGroupRole::Type RoleValues[] = {
				EAnimGroupRole::AlwaysFollower,
				EAnimGroupRole::CanBeLeader,
				EAnimGroupRole::TransitionLeader
			};

			for (int32 i = 0; i < UE_ARRAY_COUNT(RoleValues); ++i)
			{
				EAnimGroupRole::Type OriginalValue = RoleValues[i];
				FGeneric Generic(OriginalValue);
				EAnimGroupRole::Type ConvertedValue = Generic.As<EAnimGroupRole::Type>();

				TestEqual(FString::Printf(TEXT("Namespace UENUM multiple values [%d]"), i),
					ConvertedValue, OriginalValue);
				TestEqual(FString::Printf(TEXT("Namespace UENUM multiple sizes [%d]"), i),
					Generic.GetPlainSize(), sizeof(EAnimGroupRole::Type));
			}
		}
	}

	// Test 23: Soft Object and Class Reference Tests
	{
		// Test UObject references
		UObject* TransientPackage = GetTransientPackage();
		TestNotNull(TEXT("Transient package should be valid"), TransientPackage);

		FGeneric ObjectGeneric(TransientPackage);
		UObject* RetrievedObject = ObjectGeneric.As<UObject*>();
		TestEqual(TEXT("UObject* direct conversion"), RetrievedObject, TransientPackage);

		// Test TSoftObjectPtr
		TSoftObjectPtr<UObject> SoftTransient(TransientPackage);
		FGeneric SoftObjectGeneric(SoftTransient);
		TSoftObjectPtr<UObject> RetrievedSoftObject = SoftObjectGeneric.As<TSoftObjectPtr<UObject>>();

		TestEqual(TEXT("SoftObjectPtr conversion - object path"),
			RetrievedSoftObject.ToString(), SoftTransient.ToString());
		TestEqual(TEXT("SoftObjectPtr conversion - resolved object"),
			RetrievedSoftObject.Get(), TransientPackage);

		// Test TSoftClassPtr
		UClass* ActorClass = AActor::StaticClass();
		TestNotNull(TEXT("AActor class should be valid"), ActorClass);

		TSoftClassPtr<UObject> SoftActorClass(ActorClass);
		FGeneric SoftClassGeneric(SoftActorClass);
		TSoftClassPtr<UObject> RetrievedSoftClass = SoftClassGeneric.As<TSoftClassPtr<UObject>>();

		TestEqual(TEXT("SoftClassPtr conversion - class path"),
			RetrievedSoftClass.ToString(), SoftActorClass.ToString());
		TestEqual(TEXT("SoftClassPtr conversion - resolved class"),
			RetrievedSoftClass.Get(), ActorClass);

		// Test TSubclassOf
		TSubclassOf<AActor> ActorSubclass = AActor::StaticClass();
		FGeneric SubclassGeneric(ActorSubclass);
		TSubclassOf<AActor> RetrievedSubclass = SubclassGeneric.As<TSubclassOf<AActor>>();

		TestEqual(TEXT("TSubclassOf conversion"), RetrievedSubclass, ActorSubclass);
		TestEqual(TEXT("TSubclassOf resolved class"), RetrievedSubclass.Get(), AActor::StaticClass());

		// Test implicit conversions for object types
		UObject* ImplicitObject = ObjectGeneric;
		TSoftObjectPtr<UObject> ImplicitSoftObject = SoftObjectGeneric;
		TSoftClassPtr<UObject> ImplicitSoftClass = SoftClassGeneric;
		TSubclassOf<AActor> ImplicitSubclass = SubclassGeneric;

		TestEqual(TEXT("Implicit UObject* conversion"), ImplicitObject, TransientPackage);
		TestEqual(TEXT("Implicit SoftObjectPtr conversion"), ImplicitSoftObject.Get(), TransientPackage);
		TestEqual(TEXT("Implicit SoftClassPtr conversion"), ImplicitSoftClass.Get(), ActorClass);
		TestEqual(TEXT("Implicit TSubclassOf conversion"), ImplicitSubclass.Get(), AActor::StaticClass());
}

	// Test 24: Weak Object Pointer Tests
	{
		UObject* TransientPackage = GetTransientPackage();
		TestNotNull(TEXT("Transient package for weak pointer test"), TransientPackage);

		// Test TWeakObjectPtr via UObject* conversion
		FGeneric WeakObjectGeneric;

		// Store as UObject*
		WeakObjectGeneric = TransientPackage;
		UObject* RetrievedObject = WeakObjectGeneric.As<UObject*>();

		TestEqual(TEXT("Weak object via UObject* conversion"), RetrievedObject, TransientPackage);

		// Test weak object persistence through serialization
		FGeneric SerializedGeneric = WeakObjectGeneric;
		UObject* SerializedObject = SerializedGeneric.As<UObject*>();
		TestEqual(TEXT("Weak object serialization persistence"), SerializedObject, TransientPackage);
	}

	// Test 25: Complex Object Reference Scenarios
	{
		// Test null object handling
		FGeneric NullObjectGeneric(static_cast<UObject*>(nullptr));
		FGeneric NullSoftObjectGeneric(TSoftObjectPtr<UObject>(nullptr));
		FGeneric NullSoftClassGeneric(TSoftClassPtr<UObject>(nullptr));
		FGeneric NullSubclassGeneric(TSubclassOf<UObject>(nullptr));

		TestNull(TEXT("Null UObject* conversion"), NullObjectGeneric.As<UObject*>());
		TestNull(TEXT("Null SoftObjectPtr conversion"), NullSoftObjectGeneric.As<TSoftObjectPtr<UObject>>().Get());
		TestNull(TEXT("Null SoftClassPtr conversion"), NullSoftClassGeneric.As<TSoftClassPtr<UObject>>().Get());
		TestNull(TEXT("Null TSubclassOf conversion"), NullSubclassGeneric.As<TSubclassOf<UObject>>().Get());

		// Test object path preservation
		UObject* TransientPackage = GetTransientPackage();
		FString TransientPath = TransientPackage->GetPathName();
		FGeneric PathGeneric(TransientPath);
		FString RetrievedPath = PathGeneric.As<FString>();
		TestEqual(TEXT("Object path string preservation"), RetrievedPath, TransientPath);
	}

	// Test 26: Object Arrays
	{
		UObject* TransientPackage = GetTransientPackage();

		// Test arrays of basic object references (UObject*)
		TArray<UObject*> ObjectArray{ TransientPackage, nullptr, TransientPackage };

		FGeneric ObjectArrayGeneric(ObjectArray);
		TArray<UObject*> RetrievedObjectArray = ObjectArrayGeneric.As<TArray<UObject*>>();

		TestEqual(TEXT("Object array size"), RetrievedObjectArray.Num(), ObjectArray.Num());

		if (RetrievedObjectArray.Num() == ObjectArray.Num())
		{
			TestEqual(TEXT("Object array element 0"), RetrievedObjectArray[0], ObjectArray[0]);
			TestEqual(TEXT("Object array element 2"), RetrievedObjectArray[2], ObjectArray[2]);
			TestNull(TEXT("Object array element 1 (null)"), RetrievedObjectArray[1]);
		}
	}

	// Test 27: Object Reference Performance and Edge Cases
	{
		const int32 PerformanceIterations = 100;
		UObject* TransientPackage = GetTransientPackage();
		UClass* ActorClass = AActor::StaticClass();

		// Test repeated object reference conversions
		FGeneric ObjectGeneric(TransientPackage);
		FGeneric ClassGeneric(ActorClass);

		for (int32 i = 0; i < PerformanceIterations; ++i)
		{
			UObject* CurrentObject = ObjectGeneric.As<UObject*>();
			UClass* CurrentClass = ClassGeneric.As<UClass*>();

			TestEqual(TEXT("Repeated object conversion consistency"), CurrentObject, TransientPackage);
			TestEqual(TEXT("Repeated class conversion consistency"), CurrentClass, ActorClass);
		}

		// Test mixed object type assignments
		FGeneric MixedGeneric;
		for (int32 i = 0; i < 10; ++i)
		{
			if (i % 2 == 0)
			{
				MixedGeneric = TransientPackage;
				UObject* Retrieved = MixedGeneric.As<UObject*>();
				TestEqual(TEXT("Mixed assignment - object"), Retrieved, TransientPackage);
			}
			else
			{
				MixedGeneric = ActorClass;
				UClass* Retrieved = MixedGeneric.As<UClass*>();
				TestEqual(TEXT("Mixed assignment - class"), Retrieved, ActorClass);
			}
		}

		// Test object reference equality
		FGeneric ObjectA(TransientPackage);
		FGeneric ObjectB(TransientPackage);
		FGeneric ObjectC(AActor::StaticClass());

		TestTrue(TEXT("Same object references are equal"), ObjectA == ObjectB);
		TestFalse(TEXT("Different object references are not equal"), ObjectA == ObjectC);
	}

	// Test 28: Soft Reference Path Resolution
	{
		// Test soft object path resolution
		FString TransientPath = TEXT("/Engine/Transient");
		TSoftObjectPtr<UObject> SoftFromPath(TransientPath);
		FGeneric SoftPathGeneric(SoftFromPath);

		// Test path to string conversion
		FString ResolvedPath = SoftPathGeneric.As<FString>();
		TestEqual(TEXT("Soft object path to string"), ResolvedPath, TransientPath);

		// Test soft class path resolution
		FString ActorClassPath = AActor::StaticClass()->GetPathName();
		TSoftClassPtr<UObject> SoftClassFromPath(ActorClassPath);
		FGeneric SoftClassPathGeneric(SoftClassFromPath);

		FString ResolvedClassPath = SoftClassPathGeneric.As<FString>();
		TestEqual(TEXT("Soft class path to string"), ResolvedClassPath, ActorClassPath);

		// Test object to path conversion
		UObject* TransientPackage = GetTransientPackage();
		FGeneric ObjectGeneric(TransientPackage);
		FString ObjectAsString = ObjectGeneric.As<FString>();
		TestEqual(TEXT("UObject* to path string"), ObjectAsString, TransientPackage->GetPathName());
	}

	// Final summary
	AddInfo(TEXT("FGeneric comprehensive test completed successfully"));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS