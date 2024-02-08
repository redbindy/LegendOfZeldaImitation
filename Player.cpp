#include "Player.h"

GameObject::Player::Player()
	: mCurrPos({ 100, 100 })
	, mState(STATE_MOVE | STATE_SIDE)
{
	mMoveSideSet.nextFrame = MOVE_FRAME_COUNT - 1;
	mMoveSideSet.scenes = new const BYTE * [MOVE_FRAME_COUNT];
	mMoveSideSet.scenes[0] = LoadZC_malloc("./Resources/LinkFrame/MoveSide0.zc");
	mMoveSideSet.scenes[1] = LoadZC_malloc("./Resources/LinkFrame/MoveSide1.zc");

	mMoveUpSet.nextFrame = MOVE_FRAME_COUNT - 1;
	mMoveUpSet.scenes = new const BYTE * [MOVE_FRAME_COUNT];
	mMoveUpSet.scenes[0] = LoadZC_malloc("./Resources/LinkFrame/MoveUp0.zc");
	mMoveUpSet.scenes[1] = LoadZC_malloc("./Resources/LinkFrame/MoveUp1.zc");

	mMoveDownSet.nextFrame = MOVE_FRAME_COUNT - 1;
	mMoveDownSet.scenes = new const BYTE * [MOVE_FRAME_COUNT];
	mMoveDownSet.scenes[0] = LoadZC_malloc("./Resources/LinkFrame/MoveDown0.zc");
	mMoveDownSet.scenes[1] = LoadZC_malloc("./Resources/LinkFrame/MoveDown1.zc");

	mAttackSideSet.nextFrame = ATTACK_FRAME_COUNT - 1;
	mAttackSideSet.scenes = new const BYTE * [ATTACK_FRAME_COUNT];
	mAttackSideSet.scenes[0] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackSide0.zc");
	mAttackSideSet.scenes[1] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackSide1.zc");
	mAttackSideSet.scenes[2] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackSide2.zc");
	mAttackSideSet.scenes[3] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackSide3.zc");

	mAttackUpSet.nextFrame = ATTACK_FRAME_COUNT - 1;
	mAttackUpSet.scenes = new const BYTE * [ATTACK_FRAME_COUNT];
	mAttackUpSet.scenes[0] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackUp0.zc");
	mAttackUpSet.scenes[1] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackUp1.zc");
	mAttackUpSet.scenes[2] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackUp2.zc");
	mAttackUpSet.scenes[3] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackUp3.zc");

	mAttackDownSet.nextFrame = ATTACK_FRAME_COUNT - 1;
	mAttackDownSet.scenes = new const BYTE * [ATTACK_FRAME_COUNT];
	mAttackDownSet.scenes[0] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackDown0.zc");
	mAttackDownSet.scenes[1] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackDown1.zc");
	mAttackDownSet.scenes[2] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackDown2.zc");
	mAttackDownSet.scenes[3] = LoadZC_malloc("./Resources/LinkFrame/LinkAttackDown3.zc");
}

GameObject::Player::~Player()
{
	for (int i = 0; i < MOVE_FRAME_COUNT; ++i)
	{
		delete[] mMoveSideSet.scenes[i];
		delete[] mMoveUpSet.scenes[i];
		delete[] mMoveDownSet.scenes[i];
	}
	delete[] mMoveSideSet.scenes;
	delete[] mMoveUpSet.scenes;
	delete[] mMoveDownSet.scenes;

	for (int i = 0; i < ATTACK_FRAME_COUNT; ++i)
	{
		delete[] mAttackSideSet.scenes[i];
		delete[] mAttackUpSet.scenes[i];
		delete[] mAttackDownSet.scenes[i];
	}
	delete[] mAttackSideSet.scenes;
	delete[] mAttackUpSet.scenes;
	delete[] mAttackDownSet.scenes;
}

const vector_t& GameObject::Player::GetCurrPos() const
{
	return mCurrPos;
}

void GameObject::Player::Move()
{
	if ((mState & STATE_MOVE) == 0)
	{
		return;
	}

	if ((GetAsyncKeyState(VK_UP) & 0x0001))
	{
		if (mCurrPos.y > 0)
		{
			--mCurrPos.y;
		}

		mState = STATE_MOVE | STATE_UP;
		mMoveUpSet.nextFrame = (mMoveUpSet.nextFrame + 1) % MOVE_FRAME_COUNT;
	}

	if ((GetAsyncKeyState(VK_DOWN) & 0x0001))
	{
		if (mCurrPos.y < manager::GameManager::SurfaceHeight - 1)
		{
			++mCurrPos.y;
		}

		mState = STATE_MOVE | STATE_DOWN;
		mMoveDownSet.nextFrame = (mMoveDownSet.nextFrame + 1) % MOVE_FRAME_COUNT;
	}

	if ((GetAsyncKeyState(VK_LEFT) & 0x0001))
	{
		if (mCurrPos.x > 0)
		{
			--mCurrPos.x;
		}

		mState = STATE_MOVE | STATE_SIDE;
		mMoveSideSet.nextFrame = (mMoveSideSet.nextFrame + 1) % MOVE_FRAME_COUNT;
	}

	if ((GetAsyncKeyState(VK_RIGHT) & 0x0001))
	{
		if (mCurrPos.x < manager::GameManager::SurfaceWidth - 1)
		{
			++mCurrPos.x;
		}

		mState = STATE_MOVE | STATE_SIDE;
		mMoveSideSet.nextFrame = (mMoveSideSet.nextFrame + 1) % MOVE_FRAME_COUNT;
	}
}

void GameObject::Player::Attack()
{
	mState &= ~STATE_MOVE;
}	

const animation_t& GameObject::Player::NextFrame()
{
	if ((mState & STATE_MOVE) != 0)
	{
		switch (mState & ~STATE_MOVE)
		{
		case STATE_SIDE:
			return mMoveSideSet;

		case STATE_UP:
			return mMoveUpSet;

		case STATE_DOWN:
			return mMoveDownSet;

		default:
			assert(false);
		}
	}
	else
	{
		switch (mState)
		{
		case STATE_SIDE:
			mAttackSideSet.nextFrame = (mAttackSideSet.nextFrame + 1) % ATTACK_FRAME_COUNT;

			if (mAttackSideSet.nextFrame == ATTACK_FRAME_COUNT - 1)
			{
				mState |= STATE_MOVE;
			}

			return mAttackSideSet;

		case STATE_UP:
			mAttackUpSet.nextFrame = (mAttackUpSet.nextFrame + 1) % ATTACK_FRAME_COUNT;

			if (mAttackUpSet.nextFrame == ATTACK_FRAME_COUNT - 1)
			{
				mState |= STATE_MOVE;
			}

			return mAttackUpSet;

		case STATE_DOWN:
			mAttackDownSet.nextFrame = (mAttackDownSet.nextFrame + 1) % ATTACK_FRAME_COUNT;

			if (mAttackDownSet.nextFrame == ATTACK_FRAME_COUNT - 1)
			{
				mState |= STATE_MOVE;
			}

			return mAttackDownSet;

		default:
			assert(false);
		}
	}
}