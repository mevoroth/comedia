using UnrealBuildTool;

public class ComediaEngine : ModuleRules
{
	public ComediaEngine(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "RHI", "RenderCore", "ShaderCore" });
	}
}
