# Unreal Generic Variables

A standalone, type-safe generic variable system for Unreal Engine, extracted from MaidGameFramework. This plugin provides a universal container type that supports seamless data exchange between C++ and Blueprint systems with full type safety.

## Overview

The `FGeneric` struct is a universal container designed to store values of diverse types while maintaining full Blueprint-C++ interoperability. It intelligently selects storage strategies based on data type characteristics:

- **Plain Data Types**: Direct memory storage for optimal performance
- **Complex Types**: Serialized text storage using UE's property system

## Key Features

### Type Safety & Compatibility
- Compile-time type checking with runtime validation
- Automatic UE4/UE5 compatibility through version detection
- Full support for Unreal's reflection system

### Storage Optimization
- Dual storage strategy: memory-efficient for POD types, serialized for complex types
- Data caching system to avoid repetitive text parsing
- Memory alignment safe with endianness awareness

### Blueprint Integration
- Complete Blueprint-C++ bidirectional data exchange
- Editor visualization with pin type information
- Asset dependency tracking for packaging

### Performance
- Zero-overhead for plain data types
- Optimized property access through reflection caching
- Efficient memory management with move semantics

## Supported Data Types

### Primary Data Types
- Boolean, integers (8-64 bit), floating point (float/double)
- FName, FString, UObject pointers, TSubclassOf
- Soft object and class pointers

### Common Struct Types
- FVector, FVector2D, FVector4, FIntVector, FIntPoint
- FRotator, FQuat, FTransform, FMatrix
- FLinearColor, FColor, FPlane, FGuid
- UE5-specific types (FVector2f, FVector3f, FVector4f, FMatrix44f)

### Container Types
- TArray for fundamental types (int32, float, FString, FName, UObject*)

## Architecture

### Reflection Host Pattern
The system uses an "Impostor/Reflection Host" design pattern through the `FGenericPropJunkPrivate` struct. This non-functional container hosts UPROPERTY declarations, causing the Unreal Header Tool to generate corresponding FProperty metadata for each type.

### Storage Strategies
- **Plain Data**: POD structures without pointers, strings, or virtual functions
- **Serialized Data**: Complex types using UE's property export/import system

## Usage Examples

### C++ Usage
```C++
// Basic assignment and conversion
FGeneric Foo = FLinearColor::White;
float MyFloat = Foo.As<float>();

// Object handling
FGeneric ObjVar = MyActor;
UObject* RetrievedObj = ObjVar;

// Struct support
FGeneric TransformVar = GetActorTransform();
FTransform RetrievedTransform = (FTransform)TransformVar;
```

### Blueprint Integration
The system provides full Blueprint support through `UGenericStatics` helper functions, enabling:
- Type-safe variable passing between C++ and Blueprints
- Dynamic configuration systems
- Save game data serialization

## Installation

### For Blueprint Projects (Recommended for non-programmers)
1. Download the latest release from the [Releases](../../releases) page
2. Extract the contents to your project's `Plugins` directory
3. Restart the Unreal Editor - the plugin will auto-enable

### For C++ Projects (Advanced users)
1. Clone this repository to your project's `Plugins` directory
2. Right-click your `.uproject` file and select "Generate Visual Studio project files"
3. Build your project in Visual Studio
4. The plugin will be automatically enabled

### Verification
After installation, you should see:
- New pin types available in Blueprint editor
- Generic struct properties in the details panel
- No compilation errors in the output log

## Version Compatibility

- **UE 4.26+**: Full support with automatic type adaptation
- **UE 5.0+**: Enhanced with native UE5 types (FVector2f, FVector3f, etc.)
- Automatic version detection and code patching at build time

## Technical Details

### Memory Management
- Plain data types: Direct memory storage in `TArray<uint8> PlainData`
- Complex types: Serialized text storage in `FString Data`
- Automatic cache management for performance optimization

### Editor Integration
- **Advanced Pin Type Visualization** - Full type selector with blueprint schema integration
- **Dynamic Property Customization** - Real-time property editing with struct regeneration
- **Asset Reference Management** - Automatic dependency tracking and cleanup

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing

Issues and pull requests are welcome for this standalone plugin.

## Author

**Liquid Fish** - Original creator of MaidGameFramework and this extraction

## Temporary Icon Copyright Notice

The `Icon128.png` file included in this repository is a temporary placeholder image sourced from an anime screenshot. This image is used solely for development environment display purposes and is not included in any distribution builds.

### Copyright Status
- This icon is not owned by the project maintainers
- It will be replaced with original/appropriately licensed artwork in future versions
- The image is used under fair use principles for non-commercial development purposes

### Removal Policy
If you are the copyright holder of this image and wish to have it removed, please contact us immediately. We will promptly replace it with alternative artwork.

### Technical Implementation
- The icon is excluded from all packaging builds via appropriate configuration
- It serves only as an editor-side visual reference
- No runtime dependency on this asset exists

We acknowledge that this temporary solution is not ideal and are actively working to replace it with original content.