using UnrealBuildTool;
using System.Collections.Generic;

public class CPMServerTarget : TargetRules
{
	public CPMServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;

        // This should be your option here
		ExtraModuleNames.AddRange( new string[] { "GAME" } );
		
		bUseLoggingInShipping = true;
	}
}
