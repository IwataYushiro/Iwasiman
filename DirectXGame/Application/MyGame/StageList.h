#pragma once
//enum倉庫?　命名規則は　コード(大文字繋げ)_Stage1_Area1みたいな感じ

//ゲームプレイシーン

//ステージリスト
enum StageList//stageNum=SL_Stahe_Area1
{
	//デフォルト
	SL_Default = 0,
	//ステージ1
	SL_Stage1_Area1 = 1,
	SL_Stage1_Area2 = 2,
	SL_Stage1_Area3 = 3,
	SL_Stage1_AreaBoss = 4,
	SL_Stage1_StageID = 10,
	//ステージ2
	SL_Stage2_Area1 = 11,
	SL_Stage2_Area2 = 12,
	SL_Stage2_Area3 = 13,
	SL_Stage2_AreaBoss = 14,
	SL_Stage2_StageID = 20,
	//チュートリアル
	SL_StageTutorial_Area1 = 101,
	SL_StageTutorial_Area2 = 102,
	SL_StageTutorial_Area3 = 103,
	SL_StageTutorial_Final = 104,
	SL_StageTutorial_StageID = 110,
};

//ポーズ用テクスチャインデックス


//ポーズ用メニューインデックス
enum PauseMenuIndex
{
	PMI_Continue = 0,
	PMI_StageSelect = 1,
	PMI_Title = 2,
};


//チュートリアルテクスチャインデックス
enum TutorialTextureIndex
{

};

//アイテムテクスチャインデックス
enum ItemTextureIndex
{

};

//タイトルシーン

//タイトル用テクスチャインデックス
enum TitleSceneTextureIndex
{
	TSTI_TitleTex = 0,
	TSTI_TitleDoneTex = 1,
	TSTI_MenuTex = 2,
	TSTI_MenuTutorialTex = 3,
	TSTI_MenuStageSerectTex = 4,
	TSTI_MenuDoneTex = 5,
	TSTI_BackTitleTex = 6,
};

//タイトル用メニュー番号
enum TitleSceneMenuIndex
{
	TSMI_Tutorial = 0,
	TSMI_StageSelect = 1,
};

//ステージセレクトシーン

//ステージセレクト用テクスチャインデックス
enum StageSelectSceneTextureIndex
{
	SSSTI_MenuTex = 0,
	SSSTI_MenuTutorialTex = 1,
	SSSTI_Menustage1Tex = 2,
	SSSTI_Menustage2Tex = 3,
	SSSTI_MenuDoneTex = 4,
	SSSTI_BackTitleTex  = 5,
};

//ステージセレクト用メニューインデックス
enum StageSelectSceneMenuIndex
{
	SSSMI_StageTutorial_Tutorial = 0,
	SSSMI_Stage1_SkyStage = 1,
	SSSMI_Stage2_TowerStage = 2,
};

//ステージクリアシーン

//
enum StageClearSceneTextureIndex
{
	SCSTI_StageTutorial_Tutorial = 0,
	SCSTI_Stage1_SkyStage = 1,
	SCSTI_Stage2_TowerStage = 2,
};

//ゲームオーバーシーン

//ゲームオーバー用テクスチャインデックス
enum GameOverSceneTextureIndex
{
	GOSTI_MenuTex = 0,
	GOSTI_ContinueTex = 1,
	GOSTI_StageSelectTex = 2,
	GOSTI_TitleTex = 3,
	GOSTI_MenuDoneTex = 4,
};

//ゲームオーバー用メニューインデックス
enum GameOverSceneMenuIndex
{
	GOSMI_Continue = 0,
	GOSMI_StageSelect = 1,
	GOSMI_Title = 2,
};

