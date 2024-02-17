#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include <list>
#include <memory>

/*

*	MyGame.h

*	ゲーム全体

*/
class MyGame :public IwasiEngine::Framework
{
public:

	//初期化
	void Initialize() override;

	//更新
	void Update() override;

	//描画
	void Draw() override;

	//終了
	void Finalize() override;

};

