// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Comedia : ModuleRules
{
	public Comedia(TargetInfo Target)
	{
		//PublicIncludePaths.AddRange(new string[] { "PaperSurface/Public" });
		//PrivateIncludePaths.AddRange(new string[] { "PaperSurface/Private" });

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ComediaEngine" });

		PrivateDependencyModuleNames.AddRange(new string[] { "RHI", "RenderCore", "ShaderCore" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}
}
