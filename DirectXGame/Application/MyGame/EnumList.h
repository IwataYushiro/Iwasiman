#pragma once
//enum�q�Ɂ@�����K���́@�R�[�h(�啶���q��)_Stage1_Area1�݂����Ȋ���
//stageNum=SL_Stahe_Area1 (cpp X)
//�Q�[���v���C�V�[��
//�X�e�[�W���X�g
enum StageList
{
	//�f�t�H���g
	SL_Default = 0,
	//�X�e�[�W1
	SL_Stage1_Area1 = 1,
	SL_Stage1_Area2 = 2,
	SL_Stage1_Area3 = 3,
	SL_Stage1_AreaBoss = 4,
	SL_Stage1_StageID = 10,
	//�X�e�[�W2
	SL_Stage2_Area1 = 11,
	SL_Stage2_Area2 = 12,
	SL_Stage2_Area3 = 13,
	SL_Stage2_AreaBoss = 14,
	SL_Stage2_StageID = 20,
	//�`���[�g���A��
	SL_StageTutorial_Area1 = 101,
	SL_StageTutorial_Area2 = 102,
	SL_StageTutorial_Area3 = 103,
	SL_StageTutorial_Final = 104,
	SL_StageTutorial_StageID = 110,
};
//�X�e�[�W�ԍ����X�g
enum StageNumberList
{
	SNL_Stage1 = 1,
	SNL_Stage2 = 2,
	SNL_Stage3 = 3,
	SNL_Stage4 = 4,
};

//�Q�[���V�[���̃e�N�X�`���C���f�b�N�X�ƃt�F�[�h�C���A�E�g�e�N�X�`���C���f�b�N�X
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

//�|�[�Y�p���j���[�C���f�b�N�X
enum GamePlayScenePauseMenuIndex
{
	GPSPMI_Resume = 0,
	GPSPMI_HowToPlay = 1,
	GPSPMI_StageSelect = 2,
	GPSPMI_Title = 3,
};


//�`���[�g���A���e�N�X�`���C���f�b�N�X
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

//�v���C���[�e�N�X�`���C���f�b�N�X
enum GamePlayScenePlayerTextureIndex
{
	GPSPTI_PlayerLifeBarTex = 1100,
	GPSPTI_PlayerHitEffectTex = 1101,
	GPSPTI_PlayerExplosionTex = 1102,
};
//�A�C�e���e�N�X�`���C���f�b�N�X
enum GamePlaySceneItemTextureIndex
{
	GPSITI_ItemJumpBar = 2000,
};

//�X�e�[�W�Z���N�g�V�[��

//�X�e�[�W�Z���N�g�p���j���[�C���f�b�N�X
enum StageSelectSceneMenuIndex
{
	SSSMI_StageTutorial_Tutorial = 0,
	SSSMI_Stage1_SkyStage = 1,
	SSSMI_Stage2_TowerStage = 2,
	SSSMI_Num = 3,//�z��p
};

//�^�C�g���V�[��

//�^�C�g���p���j���[�ԍ�
enum TitleSceneMenuIndex
{
	TSMI_Tutorial = 0,
	TSMI_StageSelect = 1,
};

//���̑��֗���
//���x���f�[�^�ǂݍ��ނ̂Ɏg��0�i���o�[�̑���
enum LevelDataTrueOrFalse { LDTOF_TRUE = 0 };