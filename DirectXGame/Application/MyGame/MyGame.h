#pragma once
#include "Framework.h"
#include "GamePlayScene.h"
#include "TitleScene.h"

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
};

