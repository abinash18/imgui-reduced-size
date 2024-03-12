#include "hello_imgui/hello_imgui.h"
#include "hello_imgui/runner_params.h"
#include "implot.h"
#include "main_pch.h"

using namespace ImGui;

int main(int, char*[])
{
	auto guiFunction = [&]()
	{
		ImPlot::ShowDemoWindow();
		ImGui::ShowDemoWindow();
	};

	auto implotContext = ImPlot::CreateContext();
	HelloImGui::Run(guiFunction, "Dashboard", true, true, {2500, 2500}, false);
	ImPlot::DestroyContext(implotContext);

	return 0;
}
