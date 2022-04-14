#include <list>

#include "GUI/GUI.h"
#include "GUI/TopMenuWindow.h"
#include "GUI/CreateConnectionWindow.h"
#include "DCSConnection/DCSConnection.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class MainLoop
{
public:
	MainLoop(const std::string & windowTitle, unsigned width, unsigned height)
	{
		m_windowTitle = windowTitle;
		WNDCLASSEX m_windowClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
		::RegisterClassEx(&m_windowClass);
		HWND hwnd = ::CreateWindow(m_windowClass.lpszClassName, _T(windowTitle.c_str()), WS_OVERLAPPEDWINDOW, 0, 0, width, height, NULL, NULL, m_windowClass.hInstance, NULL);

		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			::UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
			return;
		}

		::ShowWindow(hwnd, SW_SHOWDEFAULT);
		::UpdateWindow(hwnd);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.WantCaptureKeyboard = true;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.KeyMap[ImGuiKey_O] = 'O';

		ImGui::StyleColorsDark();

		ImGui_ImplDX11_Init(hwnd, m_pd3dDevice, m_pd3dDeviceContext);

		auto dcsConnection = std::make_shared<DCSConnection>();
		auto bindAddElement = std::bind(&MainLoop::AddElement, this, std::placeholders::_1);
		m_elements.push_back(std::make_shared<GUI::TopMenuWindow>(dcsConnection, bindAddElement));
		m_elements.push_back(std::make_shared<GUI::CreateConnectionWindow>(dcsConnection, bindAddElement));
	}

	~MainLoop()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(m_hwnd);
		::UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
	}

public:
	ID3D11Device* GetDevice() 
	{ 
		return m_pd3dDevice;
	}
	ID3D11DeviceContext* GetContext() 
	{ 
		return m_pd3dDeviceContext;
	}
	IDXGISwapChain* GetSwapChain() 
	{ 
		return m_pSwapChain;
	}
	ID3D11RenderTargetView* GetRenderTargetView() 
	{ 
		return m_mainRenderTargetView;
	}
	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_mainRenderTargetView);
		pBackBuffer->Release();
	}
	void CleanupRenderTarget()
	{
		if (m_mainRenderTargetView) { m_mainRenderTargetView->Release(); m_mainRenderTargetView = NULL; }
	}

public:

	void AddElement(const std::shared_ptr<GUI::Element>& element)
	{
		auto findByName = [&element](auto pElement)
		{
			return element->GetName() == pElement->GetName();
		};

		if (element && std::find_if(m_elements.begin(), m_elements.end(), findByName) == m_elements.end())
		{
			m_elements.push_back(element);
		}
	}

	void EraseClosedElements()
	{
		for (auto elementIt = m_elements.begin(); elementIt != m_elements.end();)
		{
			auto element = *elementIt;
			if (element->IsOpened())
			{
				element->Show();
				++elementIt;
			}
			else
			{
				elementIt = m_elements.erase(elementIt);
			}
		}
	}

	void Run()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		bool done = false;
		while (!done)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;

			ImGui_ImplDX11_NewFrame();

			EraseClosedElements();

			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			m_pd3dDeviceContext->OMSetRenderTargets(1, &m_mainRenderTargetView, NULL);
			m_pd3dDeviceContext->ClearRenderTargetView(m_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			m_pSwapChain->Present(1, 0); // Present with vsync
		}
	}

private:
	bool CreateDeviceD3D(HWND hWnd) 
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;

		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pd3dDevice, &featureLevel, &m_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}
	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = NULL; }
		if (m_pd3dDeviceContext) { m_pd3dDeviceContext->Release(); m_pd3dDeviceContext = NULL; }
		if (m_pd3dDevice) { m_pd3dDevice->Release(); m_pd3dDevice = NULL; }
	}

private:
	ID3D11Device*            m_pd3dDevice = NULL;
	ID3D11DeviceContext*     m_pd3dDeviceContext = NULL;
	IDXGISwapChain*          m_pSwapChain = NULL;
	ID3D11RenderTargetView*  m_mainRenderTargetView = NULL;

	WNDCLASSEX m_windowClass;
	HWND m_hwnd;
	std::string m_windowTitle;

	std::shared_ptr<DCSConnection> m_dcsConnection;
	std::list<std::shared_ptr<GUI::Element>> m_elements;
};

MainLoop* ml = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam < 256)
		{
			if (wParam == VK_CONTROL)
			{
				ImGui::GetIO().KeyCtrl = true;
			}
			else
			{				
				ImGui::GetIO().KeysDown[wParam] = true;
			}
		}
		return 0;
	case WM_CLOSE:
		::PostQuitMessage(0);
		return 0;
	case WM_SIZE:
		if (ml && ml->GetDevice() != NULL && wParam != SIZE_MINIMIZED)
		{
			ml->CleanupRenderTarget();
			ml->GetSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			ml->CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	ml = new MainLoop("Monitor", 900, 600);
	ml->Run();
	return 0;
}
