#pragma once
enum StageList
{
	//ステージ1
	Stage1_Area1 = 0,
	Stage1_Area2 = 1,
	Stage1_Area3 = 2,
	Stage1_AreaBoss = 3,
	Stage1_StageID = 9,
	//ステージ2
	Stage2_Area1 = 10,
	Stage2_Area2 = 11,
	Stage2_Area3 = 12,
	Stage2_AreaBoss = 13,
	Stage2_StageID = 19,

	StageTutorial_Area1 = 100,
	StageTutorial_Area2 = 101,
	StageTutorial_Area3 = 102,
	StageTutorial_Final = 103,
	StageTutorial_StageID = 109,
};

enum ReturnStageSerectAndTitle
{
	RSSAT_StageTutorial_Tutorial = 0,
	RSSAT_Stage1_SkyStage = 1,
	RSSAT_Stage2_TowerStage = 2,
};
