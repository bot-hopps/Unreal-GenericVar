// Copyright Liquid Fish. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

namespace MaidTraits_Private
{
	namespace SubclassOf
	{
		template<typename T> constexpr bool TIsSubclassOf = false;
		template<typename T> constexpr bool TIsSubclassOf<TSubclassOf<T>> = true;

		template<typename T, typename = void>
		struct TSubclassOfType { using Type = T; };

		template<typename T>
		struct TSubclassOfType<TSubclassOf<T>, std::enable_if_t<TIsSubclassOf<TSubclassOf<T>>>>
		{
			using Type = T;
		};
	}
	namespace Struct
	{
		template<typename T, typename = void>
		struct THasStaticStruct : std::false_type {};

		template<typename T>
		struct THasStaticStruct<T, std::void_t<decltype(&T::StaticStruct)>>
			: std::is_convertible<decltype(T::StaticStruct()), UScriptStruct*> {
		};

		template<typename T, typename = void>
		struct TIsForceInitConstructible : std::false_type {};

		template<typename T>
		struct TIsForceInitConstructible <T,
			std::void_t<decltype(T(EForceInit::ForceInit))>> : std::true_type {};
	}
	namespace Enum
	{
		template<typename T> constexpr bool TIsEnumAsByte = false;
		template<typename T> constexpr bool TIsEnumAsByte<TEnumAsByte<T>> = true;

		template<typename T, int> struct TUnderlyingEnumImpl { using Type = void; };
		template<typename T> struct TUnderlyingEnumImpl<T, 0> { using Type = typename T::EnumType; };
		template<typename T> struct TUnderlyingEnumImpl<T, 1> { using Type = T; };

		template<typename T, typename = void> struct TUnderlyingEnum { using Type = void; };
		template<typename T> struct TUnderlyingEnum<T, std::void_t<decltype(sizeof(T))>>
		{
			using Type = typename TUnderlyingEnumImpl<T,
				TIsEnumAsByte<T> ? 0 : (std::is_enum_v<T> ? 1 : 2)
			>::Type;
		};

		template<typename T, int> struct TUnderlyingTypeImpl { using Type = void; };
		template<typename T> struct TUnderlyingTypeImpl<T, 0> { using Type = std::underlying_type_t<typename TUnderlyingEnum<T>::Type>; };
		template<typename T> struct TUnderlyingTypeImpl<T, 1> { using Type = std::underlying_type_t<T>; };

		template<typename T, typename = void> struct TUnderlyingType { using Type = void; };
		template<typename T> struct TUnderlyingType<T, std::void_t<decltype(sizeof(T))>>
		{
			using Type = typename TUnderlyingTypeImpl<T,
				TIsEnumAsByte<T> ? 0 : (std::is_enum_v<T> ? 1 : 2)
			>::Type;
		};

		template<typename T>
		auto TestStaticEnum(int) -> decltype(StaticEnum<T>(), std::true_type{});

		template<typename T>
		auto TestStaticEnum(...) -> std::false_type;

		template<typename T, typename = void>
		struct THasStaticEnum : std::false_type {};

		template<typename T>
		struct THasStaticEnum<T, std::enable_if_t<std::is_same_v<decltype(TestStaticEnum<T>(0)), std::true_type>> >
			: decltype(TestStaticEnum<T>(0)) {};

		template<typename T, typename = void>
		struct TIsUEnum : std::false_type {};

		template<typename T>
		struct TIsUEnum<T, std::enable_if_t<std::is_enum_v<T>&& THasStaticEnum<T>::value>>
			: std::true_type {
		};

		template<typename T>
		struct TIsUEnum<T, std::enable_if_t<TIsEnumAsByte<T>&& std::is_enum_v<typename TUnderlyingEnum<T>::Type>&& THasStaticEnum<typename TUnderlyingEnum<T>::Type>::value>>
			: std::true_type {
		};
	}
}

namespace Maid
{
	template<typename T>
	constexpr bool TIsSubclassOf = MaidTraits_Private::SubclassOf::TIsSubclassOf<T>;

	template<typename T>
	using TSubclassOfType = typename MaidTraits_Private::SubclassOf::TSubclassOfType<T>::Type;

	template<typename T>
	constexpr bool TIsUStruct = MaidTraits_Private::Struct::THasStaticStruct<T>::value;

	template<typename T>
	constexpr bool TIsForceInitConstructible = MaidTraits_Private::Struct::TIsForceInitConstructible<T>::value;

	template<typename T>
	constexpr bool TIsEnumAsByte = MaidTraits_Private::Enum::TIsEnumAsByte<T>;

	template<typename T>
	using TUnderlyingEnum = typename MaidTraits_Private::Enum::TUnderlyingEnum<T>::Type;

	template<typename T>
	using TUnderlyingType = typename MaidTraits_Private::Enum::TUnderlyingType<T>::Type;

	template<typename T>
	constexpr bool THasStaticEnum = MaidTraits_Private::Enum::THasStaticEnum<T>::value;

	template<typename T>
	constexpr bool TIsUEnum = MaidTraits_Private::Enum::TIsUEnum<T>::value;
}
