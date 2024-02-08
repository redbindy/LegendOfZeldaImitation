#pragma once

#include "framework.h"

#include "GameManager.h"

namespace GameObject
{
	enum
	{
		ATTACK_FRAME_COUNT = 4,
		MOVE_FRAME_COUNT = 2,
	};

	typedef DWORD state_t;
	enum
	{
		STATE_MOVE = 1 << 31,
		STATE_SIDE = 1 << 8,
		STATE_UP = 1 << 9,
		STATE_DOWN = 1 << 10
	};

	class Player final
	{
	public:
		Player();
		~Player();
		Player(const Player& other) = delete;
		Player& operator=(const Player& other) = delete;
		const vector_t& GetCurrPos() const;
		const animation_t& GetAnimation() const;
		void Move();
		void Attack();
		const animation_t& NextFrame();

	private:

		vector_t mCurrPos;

		animation_t mMoveSideSet;
		animation_t mMoveUpSet;
		animation_t mMoveDownSet;

		animation_t mAttackSideSet;
		animation_t mAttackUpSet;
		animation_t mAttackDownSet;

		state_t mState;
	};
}
