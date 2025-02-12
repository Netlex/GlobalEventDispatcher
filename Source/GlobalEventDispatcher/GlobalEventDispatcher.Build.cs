// Some copyright should be here...

using UnrealBuildTool;

public class GlobalEventDispatcher : ModuleRules
{
	public GlobalEventDispatcher(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
            });
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
			});
	
	}
}
