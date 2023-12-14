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
private:
	using PostEffect = IwasiEngine::PostEffect;
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
	//ポストエフェクト
	std::unique_ptr<PostEffect> pe_;

private:
	void PostInitialize();	//ポストエフェクト初期化
	void PostDraw();		//ポストエフェクト描画
		
};

