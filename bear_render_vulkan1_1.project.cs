using BearBuildTool.Projects;
using System.IO;
using System;
public class bear_render_vulkan1_1 :Project
{
	public bear_render_vulkan1_1(string ProjectPath)
	{
		PCHFile=Path.Combine(ProjectPath,"source", "VKPCH.cpp");
		PCHIncludeFile= "VKPCH.h";
		AddSourceFiles(Path.Combine(ProjectPath,"source"),true);
		Projects.Private.Add("bear_core");
		Projects.Private.Add("bear_graphics");
		Projects.Private.Add("vulkan");
		Defines.Private.Add("VK_11");
		Defines.Private.Add("RTX");
	}
} 