#include "TitleScene.h"

SpriteCommon* TitleScene::spCommon_ = SpriteCommon::GetInstance();

void TitleScene::Initialize()
{
	UINT titleTex = 00;
	spCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(spCommon_, titleTex);
}

void TitleScene::Update()
{
	/*if (input_->TriggerKey(DIK_SPACE))
	{
		player_->Reset();
		enemy_->Reset();
		scene_ = howtoplay;

		break;
	}*/
	spriteTitle_->Update();
}

void TitleScene::Draw()
{
	//スプライト描画前処理
	spCommon_->PreDraw();
	//スプライト描画
	spriteTitle_->Draw();
}

void TitleScene::Finalize()
{
	//スプライト
	delete spriteTitle_;
}
