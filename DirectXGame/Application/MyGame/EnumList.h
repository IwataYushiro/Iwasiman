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

//�|�[�Y�p�e�N�X�`���C���f�b�N�X�ƃt�F�[�h�C���A�E�g�e�N�X�`���C���f�b�N�X
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

//�A�C�e���e�N�X�`���C���f�b�N�X
enum GamePlaySceneItemTextureIndex
{
	GPSITI_ItemJumpBar = 2000,
};

//�`���[�g���A���p�̃C�[�W���O���
enum GamePlaySceneSettingTutorialEasingNum
{
	GPSSTEN_Stundby = 0,
	GPSSTEN_Active = 1,
};


//�^�C�g���V�[��

//�^�C�g���p�e�N�X�`���C���f�b�N�X
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

//�^�C�g���p���j���[�ԍ�
enum TitleSceneMenuIndex
{
	TSMI_Tutorial = 0,
	TSMI_StageSelect = 1,
};

//�X�e�[�W�Z���N�g�V�[��

//�X�e�[�W�Z���N�g�p�e�N�X�`���C���f�b�N�X
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

//�X�e�[�W�Z���N�g�p���j���[�C���f�b�N�X
enum StageSelectSceneMenuIndex
{
	SSSMI_StageTutorial_Tutorial = 0,
	SSSMI_Stage1_SkyStage = 1,
	SSSMI_Stage2_TowerStage = 2,
};

//�X�e�[�W�N���A�V�[��

//�X�e�[�W�N���A�p�e�N�X�`���C���f�b�N�X
enum StageClearSceneTextureIndex
{
	SCSTI_MenuTex = 0,
	SCSTI_ContinueTex = 1,
	SCSTI_StageSelectTex = 2,
	SCSTI_TitleTex = 3,
	SCSTI_MenuDoneTex = 4,
	SCSTI_FadeInOutTex = 5,
};
//�X�e�[�W�N���A�p���j���[�C���f�b�N�X
enum StageClearSceneMenuIndex
{
	SCSMI_Continue = 0,
	SCSMI_StageSelect = 1,
	SCSMI_Title = 2,
};
//�Q�[���I�[�o�[�V�[��

//�Q�[���I�[�o�[�p�e�N�X�`���C���f�b�N�X
enum GameOverSceneTextureIndex
{
	GOSTI_MenuTex = 0,
	GOSTI_ContinueTex = 1,
	GOSTI_StageSelectTex = 2,
	GOSTI_TitleTex = 3,
	GOSTI_MenuDoneTex = 4,
	GOSTI_FadeInOutTex = 5,
};

//�Q�[���I�[�o�[�p���j���[�C���f�b�N�X
enum GameOverSceneMenuIndex
{
	GOSMI_Continue = 0,
	GOSMI_StageSelect = 1,
	GOSMI_Title = 2,
};

