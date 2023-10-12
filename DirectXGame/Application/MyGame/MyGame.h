#pragma once
#include "Framework.h"
#include "PostEffect.h"
#include <list>
#include <memory>

/*

*	MyGame.h

*	ゲーム全体

*/
class MyGame :public Framework
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
	
private:
	//合計
	const int POST_NUM = 12;
	PostEffect* pe;

	PostEffect* pe1 = nullptr;
	PostEffect* pe2 = nullptr;
	//切り替え用カウント
	int postCount = 0;
	int Variation = 0;
private:
	void PostInitialize();
	void PostDraw();
	void PostDelete();
		
};

