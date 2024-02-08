#pragma once

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

#include "framework.h"

#include <ddraw.h>

typedef struct
{
	const BYTE** scenes;
	DWORD nextFrame;
} animation_t;

namespace GameObject
{
	class Player;
}

namespace manager
{
	class GameManager final
	{
	public:

		static GameManager* CreateInstance(const HWND hWnd);
		static GameManager* GetInstance();
		static void DeleteInstance();

		void Draw();
		void OnKeyDown(const WPARAM keyCode);

		GameManager(const GameManager& other) = delete;
		GameManager& operator=(const GameManager& other) = delete;

		static int SurfaceWidth;
		static int SurfaceHeight;

	private:
		GameManager(const HWND hWnd);
		~GameManager();
		inline void convertClientIntoScreen(RECT* rt) const;
		void drawOnOffscreen(const BYTE* sceneData, const vector_t pos);
		void drawOnOffscreen(const BYTE* sceneData);
		void clearSurface();

		static GameManager* sInstance;

		LPDIRECTDRAW pDD;
		LPDIRECTDRAW7 pDD7;
		LPDIRECTDRAWSURFACE7 pDDPrimary;
		LPDIRECTDRAWCLIPPER pClipper;
		LPDIRECTDRAWSURFACE7 pDDBack;

		const HWND hMainWnd;

		GameObject::Player* mPlayer;

		const BYTE* mTitleZC;
		const BYTE* mMapZC;

		bool bStarted;
	};

	void GameManager::convertClientIntoScreen(RECT* rt) const
	{
		GetClientRect(hMainWnd, rt);
		ClientToScreen(hMainWnd, (LPPOINT)rt);
		ClientToScreen(hMainWnd, (LPPOINT) & (rt->right));
	}
}
