// Copyright Liquid Fish. All Rights Reserved.

using UnrealBuildTool;

public class MaidGame : ModuleRules
{
	public MaidGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UMG", "DeveloperSettings", "NetCore", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "SlateCore", "Slate", "AssetRegistry", "MovieScene", "RHI" });

		DynamicallyLoadedModuleNames.AddRange(new string[] { });

		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "DesktopPlatform", "BlueprintGraph", "SourceControl" });
		}

		PublicIncludePaths.AddRange(new string[] { ModuleDirectory });

		PatchGenericTypes();
	}

	private void PatchGenericTypes()
	{
		string GenericFilePath = System.IO.Path.Combine(ModuleDirectory, "Generic", "Generic.h");

		if (System.IO.File.Exists(GenericFilePath))
		{
			string FileContent = System.IO.File.ReadAllText(GenericFilePath);
			string ModifiedContent = FileContent;

            var DiffProperties = new[] { "FVector2f", "FVector3f", "FVector4f", "FMatrix44f", "FBox3f", "FBox2f" };

            foreach (var Property in DiffProperties)
            {
                string CommentedProperty = "\t//UPROPERTY() " + Property;
                string UncommentedProperty = "\tUPROPERTY() " + Property;

                if (Target.Version.MajorVersion >= 5)
                {
                    ModifiedContent = ModifiedContent.Replace(CommentedProperty, UncommentedProperty);
                }
                else
                {
                    ModifiedContent = ModifiedContent.Replace(UncommentedProperty, CommentedProperty);
                }
            }

            if (Target.Version.MajorVersion >= 5)
			{
				ModifiedContent = ModifiedContent.Replace("\t//template<> struct FUncommentCheck", "\ttemplate<> struct FUncommentCheck");
			}
			else
			{
                ModifiedContent = ModifiedContent.Replace("\ttemplate<> struct FUncommentCheck", "\t//template<> struct FUncommentCheck");
            }

			if (FileContent != ModifiedContent)
			{
				System.IO.FileAttributes Attributes = System.IO.File.GetAttributes(GenericFilePath);
				if ((Attributes & System.IO.FileAttributes.ReadOnly) == System.IO.FileAttributes.ReadOnly)
				{
					System.IO.File.SetAttributes(GenericFilePath, Attributes & ~System.IO.FileAttributes.ReadOnly);
				}

				System.IO.File.WriteAllText(GenericFilePath, ModifiedContent);
				if (Target.Version.MajorVersion >= 5)
				{
					System.Console.WriteLine("[MaidGame] Generic.h has been upgraded to UE5");
				}
				else
				{
					System.Console.WriteLine("[MaidGame] Generic.h has been downgraded to UE4");
				}
			}
		}
	}
}

// Legacy Maid module
public class Maid : MaidGame
{
	public Maid(ReadOnlyTargetRules Target) : base(Target)
	{
		PublicDefinitions.Add("MAIDGAME_API=MAID_API");
		PublicDefinitions.Add("MaidGame=Maid");
		PrivateDefinitions.Add("LEGACY_MAID_MODULES=1");
	}
}
