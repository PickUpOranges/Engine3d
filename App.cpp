#include "App.h"
#include <algorithm>
#include "SimpleMath.h"
#include "imgui/imgui.h"
#include "Utils.h"
#include "Testing.h"
#include "TestModelProbe.h"
#include "Camera.h"
#include "Channels.h"


namespace dx = DirectX;

App::App()
	: 
	wnd(1280, 720, "Test App Class Obj"),
	light(wnd.Gfx(), { 10.0f,5.0f,0.0f })
{
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "A", dx::XMFLOAT3{ 5.5f,6.0f,-7.9f }, 0.0f, 0.0f));
	cameras.AddCamera(std::make_unique<Camera>(wnd.Gfx(), "B", dx::XMFLOAT3{ -13.5f,28.8f,-6.4f }, PI / 180.0f * 13.0f, PI / 180.0f * 61.0f));


	cameras.AddCamera(light.ShareCamera());


	cube.SetPos({ 10.0f,5.0f,6.0f });
	cube2.SetPos({ 10.0f,5.0f,14.0f });


	cube.LinkTechniques(rg);
	cube2.LinkTechniques(rg);
	light.LinkTechniques(rg);
	sponza.LinkTechniques(rg);
	cameras.LinkTechniques(rg);

	rg.BindShadowCamera(*light.ShareCamera());
}

App::~App()
{}

void App::HandleInput(float dt)
{

	// Handles the messages from mouse and keyboard.
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_F1:
			showDemoWindow = true;
			break;
		case VK_RETURN:
			savingDepth = true;
			break;
		}
	}

	if (wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed('W'))
		{
			cameras->Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cameras->Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cameras->Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cameras->Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('R'))
		{
			cameras->Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('F'))
		{
			cameras->Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cameras->Rotate((float)delta->x, (float)delta->y);
		}
	}
}

void App::DoFrame(float dt)
{

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	light.Bind(wnd.Gfx(), cameras->GetMatrix());
	rg.BindMainCamera(cameras.GetActiveCamera());


	light.Submit(Chan::main);
	cube.Submit(Chan::main);
	sponza.Submit(Chan::main);
	cube2.Submit(Chan::main);
	cameras.Submit(Chan::main);


	sponza.Submit(Chan::shadow);
	cube.Submit(Chan::shadow);
	sponza.Submit(Chan::shadow);
	cube2.Submit(Chan::shadow);

	rg.Execute(wnd.Gfx());

	if (savingDepth)
	{
		rg.DumpShadowMap(wnd.Gfx(), "shadow.png");
		savingDepth = false;
	}


	// imgui windows
	static MP sponzeProbe{ "Sponza" };
	sponzeProbe.SpawnWindow(sponza);
	
	
	cameras.SpawnWindow(wnd.Gfx());
	light.SpawnControlWindow();
	ShowImguiDemoWindow();
	cube.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	cube2.SpawnControlWindow(wnd.Gfx(), "Cube 2");
	rg.RenderWindows(wnd.Gfx());

	// present
	wnd.Gfx().EndFrame();
	rg.Reset();

}

void App::ShowImguiDemoWindow()
{
	if (showDemoWindow) {
		ImGui::ShowDemoWindow(&showDemoWindow);
	}
}

int App::Go()
{
	while (true)
	{
		// process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			// if return optional has value, means we're quitting so return exit code
			return *ecode;
		}

		// execute the game logic
		const auto dt = timer.Mark() * speed_factor;
		HandleInput(dt);
		DoFrame(dt);
	}
}
