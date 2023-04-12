#pragma once
#include "Sprite.h"
#include "BaseScene.h"

//タイトルシーン
class TitleScene :public BaseScene
{
public://メンバ関数
	//初期化
	void Initialize() override;
	//更新
	void Update() override;
	//描画
	void Draw() override;
	//終了
	void Finalize() override;

private://静的メンバ変数
	//スプライト基盤
	static SpriteCommon* spCommon_;

private://メンバ変数
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
};
