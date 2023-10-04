#pragma once
//enum�q��?�@�����K���́@�R�[�h(�啶���q��)_Stage1_Area1�݂����Ȋ���

//�Q�[���v���C�V�[��

//�X�e�[�W���X�g
enum StageList//stageNum=SL_Stahe_Area1
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

//�|�[�Y�p�e�N�X�`���C���f�b�N�X


//�|�[�Y�p���j���[�C���f�b�N�X
enum PauseMenuIndex
{
	PMI_Continue = 0,
	PMI_StageSelect = 1,
	PMI_Title = 2,
};


//�`���[�g���A���e�N�X�`���C���f�b�N�X
enum TutorialTextureIndex
{

};

//�A�C�e���e�N�X�`���C���f�b�N�X
enum ItemTextureIndex
{

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
	SSSTI_BackTitleTex  = 5,
};

//�X�e�[�W�Z���N�g�p���j���[�C���f�b�N�X
enum StageSelectSceneMenuIndex
{
	SSSMI_StageTutorial_Tutorial = 0,
	SSSMI_Stage1_SkyStage = 1,
	SSSMI_Stage2_TowerStage = 2,
};

//�X�e�[�W�N���A�V�[��

//
enum StageClearSceneTextureIndex
{
	SCSTI_StageTutorial_Tutorial = 0,
	SCSTI_Stage1_SkyStage = 1,
	SCSTI_Stage2_TowerStage = 2,
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
};

//�Q�[���I�[�o�[�p���j���[�C���f�b�N�X
enum GameOverSceneMenuIndex
{
	GOSMI_Continue = 0,
	GOSMI_StageSelect = 1,
	GOSMI_Title = 2,
};

