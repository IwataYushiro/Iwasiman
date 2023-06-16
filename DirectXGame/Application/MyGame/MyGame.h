#pragma once
#include "Framework.h"
#include "PostEffect.h"

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
	PostEffect* pe1_ = nullptr;
	PostEffect* pe2_ = nullptr;
	PostEffect* pe3_ = nullptr;

	//切り替え用カウント
	int postCount = 0;

private:
	void PostInitialize();
	void PostDraw();
	void PostDelete();
		
};

