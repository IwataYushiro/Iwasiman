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
//ステージ番号リスト
enum StageNumberList
{
	SNL_Stage1 = 1,
	SNL_Stage2 = 2,
	SNL_Stage3 = 3,
	SNL_Stage4 = 4,
};

//ゲームシーンのテクスチャインデックスとフェードインアウトテクスチャインデックス
enum GamePlaySceneTextureIndex
{
	GPSTI_PauseTex = 0,
	GPSTI_PauseInfoTex = 1,
	GPSTI_PauseResumeTex = 2,
	GPSTI_PauseHowToPlayTex = 3,
	GPSTI_PauseStageSelectTex = 4,
	GPSTI_PauseTitleTex = 5,
	GPSTI_PauseDoneTex = 6,
	GPSTI_QuitHowToPlayTex = 7,
	GPSTI_ReadyTex = 8,
	GPSTI_GoTex = 9,
	GPSTI_LoadingTex = 10,
	GPSTI_StageInfoNowTex = 11,
	GPSTI_FadeInOutTex,

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

//プレイヤーテクスチャインデックス
enum GamePlayScenePlayerTextureIndex
{
	GPSPTI_PlayerLifeBarTex = 1100,
	GPSPTI_PlayerHitEffectTex = 1101,
	GPSPTI_PlayerExplosionTex = 1102,
};
//アイテムテクスチャインデックス
enum GamePlaySceneItemTextureIndex
{
	GPSITI_ItemJumpBar = 2000,
};

//ステージセレクトシーン

//ステージセレクト用メニューインデックス
enum StageSelectSceneMenuIndex
{
	SSSMI_StageTutorial_Tutorial = 0,
	SSSMI_Stage1_SkyStage = 1,
	SSSMI_Stage2_TowerStage = 2,
	SSSMI_Num = 3,//配列用
};

//タイトルシーン

//タイトル用メニュー番号
enum TitleSceneMenuIndex
{
	TSMI_Tutorial = 0,
	TSMI_StageSelect = 1,
};

//その他便利な
//レベルデータ読み込むのに使う0ナンバーの代わり
enum LevelDataTrueOrFalse { LDTOF_TRUE = 0 };