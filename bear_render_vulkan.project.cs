using BearBuildTool.Projects;
using System.IO;
using System;
public class bear_render_vulkan :Project
{
	public bear_render_vulkan(string ProjectPath)
	{
		PCHFile=Path.Combine(ProjectPath,"source","vulkanPCH.cpp");
		PCHIncludeFile="vulkanPCH.h";
		AddSourceFiles(Path.Combine(ProjectPath,"source"),true);
		Projects.Private.Add("bear_core");
		Projects.Private.Add("bear_graphics");
		Projects.Private.Add("vulkan");
	}
} 