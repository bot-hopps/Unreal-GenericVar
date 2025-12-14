// Copyright Liquid Fish. All Rights Reserved.

#include "Generic/Generic.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"

/**
 * Comprehensive test suite for FGeneric container type
 * Tests all major functionality including basic types, objects, arrays, and edge cases
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGenericComprehensiveTest, "MaidGame.Generic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter | EAutomationTestFlags::HighPriority)

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

    // Final summary
    AddInfo(TEXT("FGeneric comprehensive test completed successfully"));
    return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS