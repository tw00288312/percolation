#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

#include "BasicWindow.h"
#include "Percolation.h"
#include <vector>

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class MainWindow : public BaseWindow<MainWindow>
{
	int n = 10;
	ID2D1Factory            *pFactory = nullptr;
	ID2D1HwndRenderTarget   *pRenderTarget = nullptr;
	ID2D1SolidColorBrush    *pWhiteBrush = nullptr;
	ID2D1SolidColorBrush    *pBlackBrush = nullptr;
	ID2D1SolidColorBrush    *pBlueBrush = nullptr;
	std::vector<std::vector<D2D1_RECT_F >> gridRect;
	Percolation percolation;

	void    CalculateLayout();
	HRESULT CreateGraphicsResources();
	void    DiscardGraphicsResources();
	void    OnPaint();
	void    Resize();

public:

	MainWindow() : gridRect(n, std::vector<D2D1_RECT_F>(n)), percolation(n)
	{
		percolation.open(0, 0);
		percolation.open(6, 2);
		percolation.open(3, 7);
		for (int i = 0; i < n-1; ++i)
		{
			percolation.open(i, i+1);
		}
		for (int i = 0; i < n - 2; ++i)
		{
			percolation.open(i, i + 2);
		}

	}

	PCWSTR  ClassName() const { return L"Percplation Visualizer"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

// Recalculate drawing layout when the size of the window changes.

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radius = min(x, y);
		int space = 10;
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
			{
				float intervalH = (size.width - 2 * space) / n;
				float intervalV = (size.height - 2 * space) / n;

				float left = space + i * intervalH;
				float right = left + intervalH;
				float bottom = space + j * intervalV;
				float top = bottom + intervalV;

				gridRect[i][j] = D2D1::RectF(left, top, right, bottom);
			}
	}
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);

		if (SUCCEEDED(hr))
		{
			const D2D1_COLOR_F whiteColor = D2D1::ColorF(1.0f, 1.0f, 1.0f);
			const D2D1_COLOR_F blackColor = D2D1::ColorF(0.0f, 0.0f, 0.0f);
			const D2D1_COLOR_F blueColor = D2D1::ColorF(0.0f, 0.0f, 0.5f);
			hr = pRenderTarget->CreateSolidColorBrush(whiteColor, &pWhiteBrush);
			hr = pRenderTarget->CreateSolidColorBrush(blackColor, &pBlackBrush);
			hr = pRenderTarget->CreateSolidColorBrush(blueColor, &pBlueBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
	SafeRelease(&pWhiteBrush);
	SafeRelease(&pBlackBrush);
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
			{
				ID2D1SolidColorBrush* colorBrush = nullptr;

				if (percolation.isFull(i, j))
					colorBrush = pBlueBrush;
				else if (percolation.isOpen(i, j))
					colorBrush = pWhiteBrush;
				else
					colorBrush = pBlackBrush;

				pRenderTarget->FillRectangle(gridRect[i][j], colorBrush);
			}

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void MainWindow::Resize()
{
	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	MainWindow win;

	if (!win.Create(L"Percolation Visualizer", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;  // Fail CreateWindowEx.
		}
		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(&pFactory);
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;



	case WM_SIZE:
		Resize();
		return 0;
	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}