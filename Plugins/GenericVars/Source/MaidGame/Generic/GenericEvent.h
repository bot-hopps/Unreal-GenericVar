// Copyright Liquid Fish. All Rights Reserved.

#pragma once

#include "Core/MaidCoreFwd.h"
#include "Generic/Generic.h"

#include "GenericEvent.Generated.h"

/**
 * Interface for objects that can receive and handle generic events
 *
 * This interface provides a standardized way for objects to respond to generic events
 * with arbitrary data payloads. Implement this interface on any object that needs
 * to participate in the generic event system.
 *
 * @see UGenericEvent, FGeneric
 */
UINTERFACE(Blueprintable)
class MAIDGAME_API UGenericEventHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for generic event handling functionality
 *
 * Classes implementing this interface can receive generic events with flexible
 * data payloads through the HandleEvent method. This allows for decoupled
 * communication between system components.
 */
class MAIDGAME_API IGenericEventHandler
{
	GENERATED_BODY()

public:
	/**
	 * Handles a generic event sent to this object
	 *
	 * This method is called when a generic event is dispatched to this object.
	 * Implement this method to define how your object responds to different event types.
	 *
	 * @param Source The object that originated the event (can be null for system events)
	 * @param EventName The name/type of event being handled
	 * @param Args Generic container holding event-specific data
	 * @return A generic value that can be used to provide feedback or results from event handling
	 *
	 * @note This is a BlueprintNativeEvent, meaning it can be overridden in Blueprints
	 *       while providing a default C++ implementation
	 *
	 * @see FGeneric, UObject
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Event", Meta = (AutoCreateRefTerm = "Args"))
	FGeneric HandleGenericEvent(UObject* Source, FName EventName, const FGeneric& Args);
	virtual FGeneric HandleGenericEvent_Implementation(UObject* Source, FName EventName, const FGeneric& Args) { return FGeneric::Null; }
};
