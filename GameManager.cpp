#include "GameManager.h"
#include "BitmapUtil.h"
#include "Player.h"

namespace manager
{
	GameManager* GameManager::sInstance = nullptr;

	int GameManager::SurfaceHeight;
	int GameManager::SurfaceWidth;

	GameManager::GameManager(const HWND hWnd)
		: pDD(nullptr)
		, pDD7(nullptr)
		, pDDPrimary(nullptr)
		, pClipper(nullptr)
		, pDDBack(nullptr)
		, hMainWnd(hWnd)
		, bStarted(false)
	{
		assert(hWnd != nullptr);

		enum
		{
			MESSAGE_LENGTH = 128
		};

		const TCHAR* messageFormat = TEXT("%s\r\nError Code: %#X");
		TCHAR errorMessage[MESSAGE_LENGTH];

		HRESULT hr = DirectDrawCreate(nullptr, &pDD, nullptr);
		if (FAILED(hr))
		{
			wsprintf(errorMessage, messageFormat, TEXT("DDraw 개체 생성에 실패했습니다."), hr);
			MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

			goto INIT_FAILED;
		}

		hr = pDD->QueryInterface(IID_IDirectDraw7, (LPVOID*)&pDD7);
		if (FAILED(hr))
		{
			wsprintf(errorMessage, messageFormat, TEXT("DDraw 버전을 갖고 오는데 실패했습니다."), hr);
			MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

			goto INIT_FAILED;
		}

		hr = pDD7->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
		if (FAILED(hr))
		{
			wsprintf(errorMessage, messageFormat, TEXT("협력수준 설정에 실패했습니다."), hr);
			MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

			goto INIT_FAILED;
		}

		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		messageFormat = TEXT("%s 초기화에 실패했습니다.\r\nError Code: %#X");

		hr = pDD7->CreateSurface(&ddsd, &pDDPrimary, nullptr);
		if (FAILED(hr))
		{
			wsprintf(errorMessage, messageFormat, TEXT("CreateSurface - primary"), hr);
			MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

			goto INIT_FAILED;
		}

		hr = pDD->CreateClipper(0, &pClipper, nullptr);
		if (FAILED(hr))
		{
			wsprintf(errorMessage, messageFormat, TEXT("CreateClipper"), hr);
			MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

			goto INIT_FAILED;
		}
		else
		{
			hr = pClipper->SetHWnd(0, hWnd);
			if (FAILED(hr))
			{
				wsprintf(errorMessage, messageFormat, TEXT("SetHWnd"), hr);
				MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

				goto INIT_FAILED;
			}
			else
			{
				hr = pDDPrimary->SetClipper(pClipper);
				if (FAILED(hr))
				{
					wsprintf(errorMessage, messageFormat, TEXT("SetClipper"), hr);
					MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

					goto INIT_FAILED;
				}
			}
		}

		RECT rt;
		GetClientRect(hWnd, &rt);

		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
		ddsd.dwWidth = rt.right;
		ddsd.dwHeight = rt.bottom;

		hr = pDD7->CreateSurface(&ddsd, &pDDBack, nullptr);
		if (FAILED(hr))
		{
			wsprintf(errorMessage, messageFormat, TEXT("CreateSurface - offscreen"), hr);
			MessageBox(hWnd, errorMessage, ERR_CAPTION, MB_OK);

			goto INIT_FAILED;
		}

		SurfaceWidth = ddsd.dwWidth;
		SurfaceHeight = ddsd.dwHeight;

		mTitleZC = LoadZC_malloc("./Resources/Title.zc");
		mMapZC = LoadZC_malloc("./Resources/Map.zc");
		mPlayer = new GameObject::Player();

		return;

	INIT_FAILED:
		SendMessage(hWnd, WM_DESTROY, 0, 0);
	}

	GameManager::~GameManager()
	{
		delete[] mTitleZC;
		delete[] mMapZC;
		delete mPlayer;

		pDDBack->Release();
		pClipper->Release();
		pDDPrimary->Release();
		pDD7->Release();
		pDD->Release();
	}

	GameManager* GameManager::CreateInstance(const HWND hWnd)
	{
		assert(hWnd != nullptr);

		sInstance = new GameManager(hWnd);

		return sInstance;
	}

	GameManager* GameManager::GetInstance()
	{
		assert(sInstance != nullptr);

		return sInstance;
	}

	void GameManager::DeleteInstance()
	{
		delete sInstance;
	}

	void GameManager::Draw()
	{
		if (!bStarted)
		{
			drawOnOffscreen(mTitleZC);
		}
		else
		{
			// clearSurface();

			drawOnOffscreen(mMapZC);

			const animation_t& playerAnimation = mPlayer->NextFrame();
			drawOnOffscreen(playerAnimation.scenes[playerAnimation.nextFrame], mPlayer->GetCurrPos());
		}

		RECT rt;
		convertClientIntoScreen(&rt);

		pDDPrimary->Blt(&rt, pDDBack, nullptr, DDBLT_WAIT, nullptr);
	}

	void GameManager::OnKeyDown(const WPARAM keyCode)
	{
		switch (keyCode)
		{
		case VK_RETURN:
			bStarted = true;
			return;

		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			mPlayer->Move();
			return;

		case VK_SPACE:
			mPlayer->Attack();
			return;

		default:
			return;
		}
	}

	void GameManager::drawOnOffscreen(const BYTE* sceneData, const vector_t pos)
	{
		assert(sceneData != nullptr);

		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);

		HRESULT hr = pDDBack->Lock(nullptr, &ddsd, DDLOCK_WAIT, nullptr);
		assert(SUCCEEDED(hr));
		{
			const compression_format_t* pCpfmt = (const compression_format_t*)sceneData;

			while (pCpfmt->pixel != 0)
			{
				pixel_t* pDest = (pixel_t*)((BYTE*)ddsd.lpSurface + ddsd.lPitch * (pCpfmt->rowNumber + pos.y));

				const pixel_t* const P_END = (pDest + pos.x) + pCpfmt->endIndex + 1;
				for (pDest += (pos.x + pCpfmt->startIndex); pDest < P_END; ++pDest)
				{
					*pDest = pCpfmt->pixel;
				}

				++pCpfmt;
			}
		}
		hr = pDDBack->Unlock(nullptr);
		assert(SUCCEEDED(hr));
	}

	void GameManager::drawOnOffscreen(const BYTE* sceneData)
	{
		drawOnOffscreen(sceneData, { 0, 0 });
	}

	void GameManager::clearSurface()
	{
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);

		HRESULT hr = pDDBack->Lock(nullptr, &ddsd, DDLOCK_WAIT, nullptr);
		assert(SUCCEEDED(hr));
		{
			for (BYTE* pRow = (BYTE*)ddsd.lpSurface + (ddsd.dwHeight - 1) * ddsd.lPitch; pRow >= ddsd.lpSurface; pRow -= ddsd.lPitch)
			{
				pixel_t* pCol = (pixel_t*)pRow;
				const pixel_t* const P_END = (pixel_t*)pRow + ddsd.dwWidth;

				for (; pCol < P_END; ++pCol)
				{
					*pCol = 0;
				}
			}
		}
		hr = pDDBack->Unlock(nullptr);
		assert(SUCCEEDED(hr));
	}
}
