#pragma once
//enum倉庫　命名規則は　コード(大文字繋げ)_Stage1_Area1みたいな感じ
//stageNum=SL_Stahe_Area1 (cpp X)
//ゲームプレイシーン
//ステージリスト
enum StageList
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

//ポーズ用テクスチャインデックスとフェードインアウトテクスチャインデックス
enum GamePlayScenePauseTextureIndex
{
	GPSPTI_PauseTex = 0,
	GPSPTI_PauseInfoTex = 1,
	GPSPTI_PauseResumeTex = 2,
	GPSPTI_PauseHowToPlayTex = 3,
	GPSPTI_PauseStageSelectTex = 4,
	GPSPTI_PauseTitleTex = 5,
	GPSPTI_PauseDoneTex = 6,
	GPSPTI_QuitHowToPlayTex=7,
	GPSTI_FadeInOutTex = 8,

};

//ポーズ用メニューインデックス
enum GamePlayScenePauseMenuIndex
{
	GPSPMI_Resume = 0,
	GPSPMI_HowToPlay = 1,
	GPSPMI_StageSelect = 2,
	GPSPMI_Title = 3,
};


//チュートリアルテクスチャインデックス
enum GamePlaySceneTutorialTextureIndex
{
	GPSTTI_TutorialInfo1Tex = 1000,
	GPSTTI_TutorialInfo2Tex = 1001,
	GPSTTI_TutorialInfo3Tex = 1002,
	GPSTTI_TutorialInfo4Tex = 1003,
	GPSTTI_HowToPlayMoveTex = 1004,
	GPSTTI_HowToPlayDashTex = 1005,
	GPSTTI_HowToPlayJumpTex = 1006,
	GPSTTI_HowToPlayBackMoveTex = 1007,
	GPSTTI_HowToPlayAttackTex = 1008,
	GPSTTI_TutorialInfoHowToPlayTex = 1009,
};

//アイテムテクスチャインデックス
enum GamePlaySceneItemTextureIndex
{
	GPSITI_ItemJumpBar = 2000,
};

//チュートリアル用のイージング状態
enum GamePlaySceneSettingTutorialEasingNum
{
	GPSSTEN_Stundby = 0,
	GPSSTEN_Active = 1,
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
	TSTI_FadeInOutTex = 7,
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
	SSSTI_BackTitleTex = 5,
	SSSTI_FadeInOutTex = 6,
};

//ステージセレクト用メニューインデックス
enum StageSelectSceneMenuIndex
{
	SSSMI_StageTutorial_Tutorial = 0,
	SSSMI_Stage1_SkyStage = 1,
	SSSMI_Stage2_TowerStage = 2,
};

//ステージクリアシーン

//ステージクリア用テクスチャインデックス
enum StageClearSceneTextureIndex
{
	SCSTI_MenuTex = 0,
	SCSTI_ContinueTex = 1,
	SCSTI_StageSelectTex = 2,
	SCSTI_TitleTex = 3,
	SCSTI_MenuDoneTex = 4,
	SCSTI_FadeInOutTex = 5,
};
//ステージクリア用メニューインデックス
enum StageClearSceneMenuIndex
{
	SCSMI_Continue = 0,
	SCSMI_StageSelect = 1,
	SCSMI_Title = 2,
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
	GOSTI_FadeInOutTex = 5,
};

//ゲームオーバー用メニューインデックス
enum GameOverSceneMenuIndex
{
	GOSMI_Continue = 0,
	GOSMI_StageSelect = 1,
	GOSMI_Title = 2,
};

