#pragma once
#include "Sprite.h"

//タイトルシーン
class TitleScene
{
public://メンバ関数
	//初期化
	void Initialize();
	//更新
	void Update();
	//描画
	void Draw();
	//終了
	void Finalize();

private://静的メンバ変数
	//スプライト基盤
	static SpriteCommon* spCommon_;

private://メンバ変数
	//Sprite
	Sprite* spriteTitle_ = new Sprite();
};
