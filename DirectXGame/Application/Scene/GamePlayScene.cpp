#include "GamePlayScene.h"
#include "LevelLoaderJson.h"

#include "CollisionManager.h"
#include "MeshCollider.h"
#include "CollisionAttribute.h"
#include "EnumList.h"
#include "MyMath.h"

#include <cassert>
#include <sstream>
#include <iomanip>

using namespace DirectX;
using namespace IwasiEngine;
/*

*	GamyPlayScene.cpp

*	�Q�[���v���C�V�[��

*/

GamePlayScene::GamePlayScene(int stagenum) :stageNum_(stagenum) {}

void GamePlayScene::Initialize()
{
	//�C���X�^���X�擾
	dxCommon_ = DirectXCommon::GetInstance();		//DirectX���
	spCommon_ = SpriteCommon::GetInstance();		//�X�v���C�g���
	input_ = Input::GetInstance();					//���͏��
	audio_ = Audio::GetInstance();					//����
	sceneManager_ = SceneManager::GetInstance();	//�V�[���}�l�[�W���[
	imguiManager_ = ImGuiManager::GetInstance();	//ImGui�}�l�[�W���[
	colManager_ = CollisionManager::GetInstance();	//�R���W�����}�l�[�W���[

	//�t�@�N�g���[����
	enemyFactory_ = std::make_unique<EnemyFactory>();
	gimmickFactory_ = std::make_unique<GimmickFactory>();


	// �`�揉���������@��������
#pragma region �`�揉��������

	//�J��������
	camera_ = std::make_unique<Camera>();

	//�����f�[�^
	sound_ = audio_->SoundLoadWave("Resources/TestMusic.wav");

	//�����Đ��Ăяo����
	//audio_->SoundPlayWave(audio_->GetXAudio2(), sound,true);

	camera_->Reset();
	//�e���Z�b�g
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Reset();
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Reset();


	//���f���ǂݍ���
	LoadModel();
	//���x���f�[�^�ǂݍ���
	if (stageNum_ == SL_Stage1_Area1)LoadLVData("stage1");
	else if (stageNum_ == SL_Stage1_Area2)LoadLVData("stage2");
	else if (stageNum_ == SL_Stage1_Area3)LoadLVData("stage3_3");
	else if (stageNum_ == SL_Stage1_AreaBoss)LoadLVData("stageboss1");

	else if (stageNum_ == SL_StageTutorial_Area1)LoadLVData("tutorial");
	else if (stageNum_ == SL_StageTutorial_Area2)LoadLVData("tutorial2");
	else if (stageNum_ == SL_StageTutorial_Area3)LoadLVData("tutorial3_2");
	else if (stageNum_ == SL_StageTutorial_Final)LoadLVData("tutorialf");

	//�X�v���C�g
	LoadSprite();

	//���C�g�𐶐�
	lightGroup_ = LightGroup::Create();
	Object3d::SetLightGroup(lightGroup_.get());
	lightGroup_->SetCircleShadowActive(LightGroup::LN_0, true);

	//�p�[�e�B�N��
	particle1_ = Particle::LoadFromParticleTexture("particle8.png");
	pm_ = ParticleManager::Create();
	pm_->SetBlendMode(ParticleManager::BP_ADD);
	pm_->SetParticleModel(particle1_.get());
	pm_->SetCamera(camera_.get());

	//�C�[�W���O�X�^���o�C
	if (stageNum_ >= SL_StageTutorial_Area1)for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].Standby(false);
	easeFadeInOut_.Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeEyeGameStart_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easeTargetGameStart_[i].Standby(false);
	for (int i = 0; i < XYZ_Num; i++)easePlayerPositionGameStart_[i].Standby(false);
	easeReadyPosition_[XXY_X1].Standby(false);
	for (int i = 0; i < XY_Num; i++)easeTutorialListScale_[i].Standby(false);
}

void GamePlayScene::Update()
{
	//���S�t���O�̗������e�A�G���폜
	playerBullets_.remove_if(
		[](std::unique_ptr<PlayerBullet>& pbullet) { return pbullet->IsDead(); });
	enemyBullets_.remove_if(
		[](std::unique_ptr<EnemyBullet>& ebullet) { return ebullet->IsDead(); });

	/*players_.remove_if(
		[](std::unique_ptr<Player>& player) {return player->IsDead(); });*/

	enemys_.remove_if(
		[](std::unique_ptr<BaseEnemy>& enemy) {return enemy->IsDead(); });

	for (std::unique_ptr<Player>& player : players_)
	{
		//�ۉe�͏펞�\��
		SetUpCircleShadow(player->GetPosition());
	}
	//�V��(�w�i)
	for (std::unique_ptr<Object3d>& skydome : skydomes_)
	{
		//�V����]�p
		XMFLOAT3 rotSkydome = skydome->GetRotation();
		//�V��(�w�i)�͏펞��]����
		const float rotSpeed = -0.2f;
		rotSkydome.y += rotSpeed;
		skydome->SetRotation(rotSkydome);
		//�X�V
		skydome->Update();
	}

	if (isStart_)			UpdateIsStartGame();			//�Q�[���J�n��
	else if (isGamePlay_)	UpdateIsPlayGame();				//�Q�[���v���C��
	else if (isPause_)		UpdateIsPause();				//�|�[�Y��
	else if (isClear_)		UpdateIsStageClear();			//�X�e�[�W�N���A��
	else if (isGameOver_)	UpdateIsGameOver();				//�Q�[���I�[�o�[��
	else					UpdateIsQuitGame();				//�I����

	//�X�v���C�g�X�V
	spritePause_->Update();
	spritePauseInfo_->Update();
	spritePauseResume_->Update();
	spritePauseHint_->Update();
	spriteHintInfo_->Update();
	spritePauseStageSelect_->Update();
	spritePauseTitle_->Update();
	spritePauseUI_->Update();
	spriteDone_->Update();
	spriteReady_->Update();
	spriteGo_->Update();
	spriteFadeInOut_->Update();
	spriteLoad_->Update();
	spriteStageInfoNow_->Update();
	spriteCursor_->Update();
	spriteHowToPlayList_->Update();
	spriteStageName_->Update();

	//�`���[�g���A���֌W
	UpdateTutorial();
	UpdateTutorialSprite();

	//�X�V
	camera_->Update();				//�J����
	lightGroup_->Update();			//���C�g
	pm_->Update();					//�p�[�e�B�N��

}

void GamePlayScene::UpdateIsStartGame()
{
	//�C�[�W���O
	easeFadeInOut_.ease_in_out_quint();
	//�J�����C�[�W���O
	easeEyeGameStart_[XYZ_X].ease_in_out_quint();
	easeEyeGameStart_[XYZ_Y].ease_in_out_quint();
	easeEyeGameStart_[XYZ_Z].ease_in_sine();
	easeTargetGameStart_[XYZ_X].ease_in_out_quint();
	easeTargetGameStart_[XYZ_Y].ease_in_out_quint();
	easeTargetGameStart_[XYZ_Z].ease_in_sine();
	//����������X�g�\���C�[�W���O
	for (int i = 0; i < XY_Num; i++)easeTutorialListScale_[i].ease_in_out_bounce();
	spriteHowToPlayList_->SetSize({ easeTutorialListScale_[XY_X].num_X,easeTutorialListScale_[XY_Y].num_X });

	if (isEndReady_)//�S�[
	{
		//�S�[
		for (int i = 0; i < XYW_Num; i++)easeGoSizeAndAlpha_[i].ease_out_cubic();
		//�T�C�Y�A�J���[�Z�b�g
		spriteGo_->SetSize({ easeGoSizeAndAlpha_[XYW_X].num_X,easeGoSizeAndAlpha_[XYW_Y].num_X });
		spriteGo_->SetColor({ black_.x,black_.y, black_.z,easeGoSizeAndAlpha_[XYW_W].num_X });

	}
	else if (isStartReadyPart2_)//���f�B�[�����̈ʒu�ɒB������
	{
		//���f�B�[(��U�߂��Ă܂�����)
		easeReadyPosition_[XXY_X2].ease_in_back();
		//���W�Z�b�g
		spriteReady_->SetPosition({ easeReadyPosition_[XXY_X2].num_X,easeReadyPosition_[XXY_Y].start });
		//�ړ����I�������
		if (spriteReady_->GetPosition().x == easeReadyPosition_[XXY_X2].end)
		{
			//�S�[�𓮂���
			for (int i = 0; i < XYW_Num; i++)easeGoSizeAndAlpha_[i].Standby(false);
			isEndReady_ = true;
			isStartReadyPart2_ = false;
		}
	}
	else//�X�^�[�g�����f�B�[
	{
		//���f�B�[(���̂܂܍���)
		easeReadyPosition_[XXY_X1].ease_out_circ();
		//���W�Z�b�g
		spriteReady_->SetPosition({ easeReadyPosition_[XXY_X1].num_X,easeReadyPosition_[XXY_Y].start });
		//�ړ����I�������
		if (spriteReady_->GetPosition().x == easeReadyPosition_[XXY_X1].end)
		{
			//�������ʂ̃C�[�W���O�Ń��f�B�[�𓮂���
			easeReadyPosition_[XXY_X2].Standby(false);
			isStartReadyPart2_ = true;
		}

	}
	//�v���C���[���W�̃C�[�W���O
	for (int i = 0; i < XYZ_Num; i++)easePlayerPositionGameStart_[i].ease_in_out_quint();

	//�t�F�[�h�C���A�E�g
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.num_X });
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.num_X });//�X�e�[�W�J�n���ɏo��

	//�J�������Z�b�g
	camera_->SetEye({ easeEyeGameStart_[XYZ_X].num_X, easeEyeGameStart_[XYZ_Y].num_X, easeEyeGameStart_[XYZ_Z].num_X });
	camera_->SetTarget({ easeTargetGameStart_[XYZ_X].num_X, easeTargetGameStart_[XYZ_Y].num_X, easeTargetGameStart_[XYZ_Z].num_X });


	for (std::unique_ptr<Player>& player : players_)
	{
		const XMFLOAT2 dashOffsetXY = { -2.0f,1.0f };//�I�t�Z�b�g
		//���v���Z�b�g
		const ParticleManager::Preset smoke =
		{
			particle1_.get(),
			{player->GetPosition().x + dashOffsetXY.x,player->GetPosition().y + dashOffsetXY.y,player->GetPosition().z},
			{ 0.0f ,2.0f,0.0f },
			{ -3.0f,0.3f,0.3f },
			{ 0.0f,0.001f,0.0f },
			2,
			{ 1.0f, 0.0f },
			{MyMath::RandomMTFloat(0.9f,1.0f),MyMath::RandomMTFloat(0.2f,0.5f),0.0f,1.0f },
			{0.0f,0.0f,0.0f,1.0f}
		};
		//�p�[�e�B�N��
		pm_->ActiveX(smoke);

		//���W�Z�b�g
		player->SetPosition
		({
			easePlayerPositionGameStart_[XYZ_X].num_X,
			easePlayerPositionGameStart_[XYZ_Y].num_X,
			easePlayerPositionGameStart_[XYZ_Z].num_X
			});

		//�X�V(����͏o���Ȃ�)
		player->Update(false, false, true);

		//�X�e�[�W�J�n���̃C�[�W���O���I�������Q�[���J�n
		if (player->GetPosition().x == easePlayerPositionGameStart_[XYZ_X].end)
		{
			isStart_ = false;
			isGamePlay_ = true;
		}
	}

	//�X�V
	//�v���C���[������łȂ��Ƃ�
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<BaseEnemy>& enemy : enemys_)enemy->Update(true);					//�G
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();	//�G�e
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();				//�M�~�b�N
	//�v���C���[�̐����͊֌W�Ȃ�
	for (std::unique_ptr<Goal>& goal : goals_)goal->Update();				//�S�[��
	for (std::unique_ptr<Item>& item : items_)item->Update();				//�A�C�e��
	for (std::unique_ptr<Object3d>& object : objects_) object->Update();	//�n�`

}

void GamePlayScene::UpdateIsPlayGame()
{

	//���f���Ăяo����
	for (std::unique_ptr<Player>& player : players_)
	{

		//�`���[�g���A����{����
		if (stageNum_ == SL_StageTutorial_Area1)player->Update(false, false, false);
		//�`���[�g���A�������ړ����U�������p�X�e�[�W
		else if (stageNum_ == SL_StageTutorial_Area2)player->Update(true, false, false);
		//��{���
		else player->Update();

		//���@�����񂾂�Q�[���I�[�o�[(���S���o�X�L�b�v�ł����l)
		if (player->IsDead() || player->IsSkipDeadDirection())
		{
			isGameOver_ = true;
			isGamePlay_ = false;
		}
		//ImGui	
		imguiManager_->Begin();
		imguiManager_->End();
	}
	//���@�e�X�V
	for (std::unique_ptr<PlayerBullet>& playerBullet : playerBullets_) playerBullet->Update();
	//�G
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_)
	{
		enemy->Update();
		//�{�X���j�ŃX�e�[�W�N���A
		if (enemy->BossDead())
		{
			isClear_ = true;
			isGamePlay_ = false;
		}
	}
	//�G�e�X�V
	for (std::unique_ptr<EnemyBullet>& enemyBullet : enemyBullets_) enemyBullet->Update();
	//�M�~�b�N�X�V
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_)gimmick->Update();
	//�S�[��
	for (std::unique_ptr<Goal>& goal : goals_)
	{
		//�X�V
		goal->Update();
		//�S�[���ɐG�ꂽ��X�e�[�W�N���A
		if (goal->IsGoal())
		{
			isClear_ = true;
			isGamePlay_ = false;
		}

	}
	//�A�C�e���X�V
	for (std::unique_ptr<Item>& item : items_) item->Update();
	//�n�`�X�V
	for (std::unique_ptr<Object3d>& object : objects_) object->Update();

	//ImGui
	//imguiManager_->Begin();
	//camera_->DebugCamera();//�f�o�b�O�J����
	//imguiManager_->End();

	//���@�������Ă���Ƃ����������蔻����`�F�b�N����
	for (std::unique_ptr<Player>& player : players_)if (!player->IsBreak())colManager_->CheckAllCollisions();
	//Pause�@�\
	if (input_->TriggerKey(DIK_ESCAPE))
	{
		for (std::unique_ptr<Player>& player : players_)if (player->IsBreak())return;
		//�����ŃC�[�W���O�̏���
		for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(false);
		easeCursorPosX_.Standby(false);
		isBack_ = false;
		isPause_ = true;
		isGamePlay_ = false;
	}
}

void GamePlayScene::UpdateIsPause()
{
	//���j���[�̏��
	if (menuCount_ <= GPSPMI_Resume)menuCount_ = GPSPMI_Resume;
	else if (menuCount_ >= GPSPMI_Title)menuCount_ = GPSPMI_Title;
	//�I�𒆂̃��j���[�J���[
	const DirectX::XMFLOAT4 selectMenuColor = { 0.5f + infoColor_.x,0.1f,0.1f,1.0f };
	const DirectX::XMFLOAT4 otherMenuColor = { 1.0f,1.0f,1.0f,0.5f };
	//�|�[�Y�J���[
	const DirectX::XMFLOAT4 pauseColor = { 0.1f ,0.1f,0.5f + infoColor_.z,1.0f };
	//����w���X�v���C�g�̃J���[
	const DirectX::XMFLOAT4 doneColor = { 1.0f,1.0f,1.0f,0.5f + infoColor_.x };
	//�J���[�X�V
	UpdateChangeColor();

	//�C�[�W���O(�|�[�Y���ɏ������Ă�����������Ă����)
	FadeIn(black_);
	for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].ease_in_out_quint();
	easeCursorPosX_.ease_in_out_quint();

	//�|�W�V�����Z�b�g
	// 
	//�|�[�Y�֌W
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].num_X, pausePosY_[PMEN_Menu] });
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].num_X, pausePosY_[PMEN_Resume] });
	spritePauseHint_->SetPosition({ easePauseMenuPosX_[PMEN_Hint].num_X, pausePosY_[PMEN_Hint] });
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].num_X, pausePosY_[PMEN_StageSelect] });
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].num_X, pausePosY_[PMEN_Title] });
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].num_X, pausePosY_[PMEN_SelectSpace] });
	spriteHintInfo_->SetPosition({ easePauseMenuPosX_[PMEN_HintInfo].num_X, pausePosY_[PMEN_HintInfo] });
	spritePauseUI_->SetPosition({ easePauseMenuPosX_[PMEN_UI].num_X,pausePosY_[PMEN_UI] });

	//���j���[����
	if (input_->TriggerKey(DIK_W))menuCount_--;
	if (input_->TriggerKey(DIK_S))menuCount_++;

	//�F��ς���(�I�����Ă��郁�j���[�͋���)
	if (menuCount_ == GPSPMI_Resume)//�Q�[���ĊJ��I����
	{
		spritePauseResume_->SetColor(selectMenuColor);
		spritePauseHint_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Resume] });
	}
	else if (menuCount_ == GPSPMI_Hint)//�V�ѕ�������I����
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHint_->SetColor(selectMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Hint] });
	}
	else if (menuCount_ == GPSPMI_StageSelect)//�X�e�[�W�Z���N�g�J�ڂ�I����
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHint_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(selectMenuColor);
		spritePauseTitle_->SetColor(otherMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_StageSelect] });
	}
	else if (menuCount_ == GPSPMI_Title)//�^�C�g���J�ڂ�I����
	{
		spritePauseResume_->SetColor(otherMenuColor);
		spritePauseHint_->SetColor(otherMenuColor);
		spritePauseStageSelect_->SetColor(otherMenuColor);
		spritePauseTitle_->SetColor(selectMenuColor);
		spriteCursor_->SetPosition({ easeCursorPosX_.num_X,pausePosY_[PMEN_Title] });
	}

	//�f�t�H���g�J���[
	spritePause_->SetColor(pauseColor);
	spriteDone_->SetColor(doneColor);

	//���莞�̃X�v���C�g�̃C�[�W���O���I������瑀����󂯕t����
	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
	{
		if (input_->TriggerKey(DIK_SPACE))
		{
			if (menuCount_ == GPSPMI_Resume)//�Q�[���ĊJ
			{
				//�����ŃC�[�W���O�̏����B�������I�����W�ɓ��B���Ă��Ȃ��Ǝ󂯕t���Ȃ�
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;

			}
			else if (menuCount_ == GPSPMI_StageSelect)//�X�e�[�W�Z���N�g�֖߂�
			{
				//�����ŃC�[�W���O�̏����B�������I�����W�ɓ��B���Ă��Ȃ��Ǝ󂯕t���Ȃ�
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;
			}
			else if (menuCount_ == GPSPMI_Title)//�^�C�g���֖߂�
			{
				//�����ŃC�[�W���O�̏����B�������I�����W�ɓ��B���Ă��Ȃ��Ǝ󂯕t���Ȃ�
				if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].end)
				{
					for (int i = 0; i < PMEN_Num; i++)easePauseMenuPosX_[i].Standby(true);
					easeCursorPosX_.Standby(true);
				}
				isBack_ = true;
			}


		}
	}

	if (isBack_)
	{
		//�|�[�Y����������Ƃ���ʂ̖��邳��߂�
		if (menuCount_ == GPSPMI_Resume) FadeOut(black_);
		else if (menuCount_ == GPSPMI_StageSelect) FadeOut(black_);
		else if (menuCount_ == GPSPMI_Title) FadeOut(black_);

	}
	//���莞�̃X�v���C�g�̃C�[�W���O���I�������|�[�Y������
	if (spriteDone_->GetPosition().x == easePauseMenuPosX_[PMEN_SelectSpace].start)
	{
		if (isBack_)
		{
			if (menuCount_ == GPSPMI_Resume)//�Q�[���ĊJ��
			{
				isGamePlay_ = true;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isBack_ = false;
				isPause_ = false;
			}
			else if (menuCount_ == GPSPMI_StageSelect)//�X�e�[�W�Z���N�g�֖߂�
			{
				isQuit_ = true;
				isBack_ = false;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isPause_ = false;
			}
			else if (menuCount_ == GPSPMI_Title)//�^�C�g���֖߂�
			{
				isQuit_ = true;
				isBack_ = false;
				isFadeInPause_ = false;
				isFadeOutPause_ = false;
				isPause_ = false;
			}

		}

	}

}

void GamePlayScene::UpdateIsStageClear()
{
	for (std::unique_ptr<Player>& player : players_)player->Update();

	FadeIn(white_);//��������
	//���S�ɔ����Ȃ�����X�e�[�W�N���A�֑J��
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		sceneManager_->ChangeScene("STAGECLEAR", stageNum_);

	}
}

void GamePlayScene::UpdateIsGameOver()
{
	//�{�X�����j���Ă���ꍇ�͌Ăяo����Ȃ�
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_)if (enemy->BossDead()) return;
	FadeIn(deepRed_);//�Ԃ�����
	//���S�ɐԂ��Ȃ�����Q�[���I�[�o�[�֑J��
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		sceneManager_->ChangeScene("GAMEOVER", stageNum_);
	}
}

void GamePlayScene::UpdateIsQuitGame()
{
	FadeIn(black_);//��������
	//���S�ɍ����Ȃ�����e�V�[����
	if (spriteFadeInOut_->GetColor().w == easeFadeInOut_.start)
	{
		//�|�[�Y����X�e�[�W�Z���N�g�A�^�C�g����I�񂾂Ƃ�
		if (menuCount_ == GPSPMI_StageSelect)
		{
			//�X�e�[�W�I����
			if (stageNum_ <= SL_Stage1_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage1_SkyStage);
			else if (stageNum_ <= SL_Stage2_StageID)sceneManager_->ChangeScene("STAGESELECT", SSSMI_Stage2_SpaceStage);
			else sceneManager_->ChangeScene("STAGESELECT", SSSMI_StageTutorial_Tutorial);
		}
		else if (menuCount_ == GPSPMI_Title) sceneManager_->ChangeScene("TITLE", stageNum_);
	}


}

void GamePlayScene::UpdateTutorial()
{
	//�`���[�g���A���̍X�V���ꊇ�ōs��
	if (stageNum_ == SL_StageTutorial_Area1)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			nullptr, nullptr, spriteTutorialInfo1_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		for (int i = 0; i < TIEN_Num; i++)easeInfoTutorial_[i].ease_out_expo();
		SettingTutorialEase(GPSSTEN_Active, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
	}
}

void GamePlayScene::FadeIn(const DirectX::XMFLOAT3& color)
{

	if (isPause_)//�|�[�Y���̏ꍇ
	{
		if (!isFadeInPause_)
		{
			//�����ŃX�^���o�C
			easeFadeInOutPause_.Standby(true);
			isFadeInPause_ = true;
		}
		else
		{
			//�����Ńt�F�[�h�C���A�E�g
			easeFadeInOutPause_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOutPause_.num_X });//�����x�����ς���

		}
	}
	else//�V�[���J�ڎ��̏ꍇ
	{
		if (!isFadeInScene_)
		{
			//�����ŃX�^���o�C
			easeFadeInOut_.Standby(true);
			isFadeInScene_ = true;
		}
		else
		{
			//�����Ńt�F�[�h�C���A�E�g
			const XMFLOAT3 negapozi = { 1.0f - color.x,1.0f - color.y,1.0f - color.z };
			easeFadeInOut_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOut_.num_X });//�����x�����ς���
			if (isQuit_)spriteLoad_->SetColor({ negapozi.x,negapozi.y,negapozi.z, easeFadeInOut_.num_X });//�l�K�|�W�̉��p
		}
	}
}

void GamePlayScene::FadeOut(const DirectX::XMFLOAT3& color)
{

	if (isPause_)//�|�[�Y���̏ꍇ�ɂ����g��Ȃ�
	{
		if (!isFadeOutPause_)
		{
			//�����ŃX�^���o�C
			easeFadeInOutPause_.Standby(false);
			isFadeOutPause_ = true;
		}
		else
		{
			//�����Ńt�F�[�h�C���A�E�g
			easeFadeInOutPause_.ease_in_out_quint();
			spriteFadeInOut_->SetColor({ color.x,color.y,color.z, easeFadeInOutPause_.num_X });//�����x�����ς���

		}
	}
}

void GamePlayScene::Draw()
{
	//���f��
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//���f���`��
	for (std::unique_ptr<Player>& player : players_)player->Draw();					   //���@
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->Draw();	   //���@�̒e
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->Draw();				   //�G
	for (std::unique_ptr<EnemyBullet>& ebullet : enemyBullets_)ebullet->Draw();		   //�G�̒e
	for (std::unique_ptr<BaseGimmick>& gimmick : gimmicks_) gimmick->Draw();		   //�M�~�b�N
	for (std::unique_ptr<Goal>& goal : goals_)goal->Draw();							   //�S�[��
	for (std::unique_ptr<Item>& item : items_)item->Draw();							   //�A�C�e��
	for (auto& skydome : skydomes_)skydome->Draw();									   //�V��(�w�i)
	for (auto& object : objects_)object->Draw();									   //�n�`

	//���f���`��㏈��
	Object3d::PostDraw();

	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	pm_->Draw();
	for (std::unique_ptr<Player>& player : players_)player->DrawParticle();						//���@�̃p�[�e�B�N��
	for (std::unique_ptr<PlayerBullet>& pbullet : playerBullets_)pbullet->DrawParticle();		//���@�̒e�̃p�[�e�B�N��
	for (std::unique_ptr<BaseEnemy>& enemy : enemys_) enemy->DrawParticle();					//�G�̃p�[�e�B�N��
	for (std::unique_ptr<Goal>& goal : goals_) goal->DrawParticle();							//�S�[���̃p�[�e�B�N��
	for (std::unique_ptr<Item>& item : items_)item->DrawParticle();								//�A�C�e���̃p�[�e�B�N��
	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//�X�v���C�g�`��O����
	spCommon_->PreDraw();
	//�O�i�X�v���C�g
	if (isPause_)//�|�[�Y��
	{
		//�t�F�[�h�C���A�E�g
		spriteFadeInOut_->Draw();

		//�|�[�Y�֌W�̃X�v���C�g
		spritePause_->Draw();
		spritePauseResume_->Draw();
		spritePauseHint_->Draw();
		if (menuCount_ == GPSPMI_Hint)spriteHintInfo_->Draw();//���j���[�J�[�\�����q���g�ɍ��킹���ꍇ�̂ݕ\��
		spritePauseStageSelect_->Draw();
		spritePauseTitle_->Draw();
		spritePauseUI_->Draw();
		spriteDone_->Draw();
		spriteCursor_->Draw();
		

	}
	else//�Q�[���v���C��
	{
		//�A�C�e���o�[
		for (std::unique_ptr<Item>& item : items_) { item->DrawSprite(); }

		for (std::unique_ptr<Player>& player : players_) {
			player->DrawSprite();

			if (!player->IsBreak())//���@�������Ă�ꍇ
			{
				//�|�[�Y�C���t�H
				if (isGamePlay_)spritePauseInfo_->Draw();

				//�`���[�g���A���C���t�H
				if (stageNum_ == SL_StageTutorial_Area1)
				{
					DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
						nullptr, nullptr, spriteTutorialInfo1_.get());
				}
				else if (stageNum_ == SL_StageTutorial_Area2)
				{
					DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
						spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
				}
				else if (stageNum_ == SL_StageTutorial_Area3)
				{
					DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
						spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
				}
				else if (stageNum_ == SL_StageTutorial_Final)
				{
					DrawTutorialSprite(spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
						spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
				}
				else//�`���[�g���A������Ȃ��ꍇ
				{
					//��������̃��X�g��`��
					spriteHowToPlayList_->Draw();
				}
			}
		}
		//�t�F�[�h�C���A�E�g�ƃ��[�h�ƌ��X�e�[�W
		spriteFadeInOut_->Draw();
		spriteLoad_->Draw();
		spriteStageInfoNow_->Draw();
		spriteStageName_->Draw();
		//���f�B�[�S�[
		spriteReady_->Draw();
		spriteGo_->Draw();
	}


}

void GamePlayScene::Finalize()
{
	//�I������
	audio_->Finalize();
	//���
	//�e�퉹��
	audio_->SoundUnLoad(&sound_);
	//�I�u�W�F�N�g�֌W
	objects_.clear();
	skydomes_.clear();
	//���f��
	models_.clear();

}

void GamePlayScene::LoadLVData(const std::string& stagePath)
{
	// ���x���f�[�^�̓ǂݍ���
	levelData_ = LevelLoader::LoadFile(stagePath);
	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	for (LevelData::ObjectData& objectData : levelData_->objects) {

		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}
		//�v���C���[
		if (objectData.objectType.find("PLAYER") == LDTOF_TRUE)
		{
			//�v���C���[������
			std::unique_ptr<Player> newplayer;
			//�C�[�W���O�p�̃|�W�V����
			DirectX::XMFLOAT3 endEasePlayerPosition;
			//�J�n���|�W�V���������߂�I�t�Z�b�g
			const DirectX::XMFLOAT3 offsetPlayerPosition = { -100.0f,100.0f,60.0f };
			//�C�[�W���O�̎���
			const float maxTime = 4.0f;

			//����
			newplayer = Player::Create(&modelPlayerList_, this);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newplayer->SetPosition(pos);
			endEasePlayerPosition = newplayer->GetPosition();//�ŏI�C�[�W���O�|�W�V�����Z�b�g
			startEasePlayerPosition_ = endEasePlayerPosition + offsetPlayerPosition;
			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newplayer->SetRotation(rot);
			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newplayer->SetScale(scale);
			//�����ŊJ�n���v���C���[���o�Z�b�g
			easePlayerPositionGameStart_[XYZ_X].SetEasing(startEasePlayerPosition_.x, endEasePlayerPosition.x, maxTime);
			easePlayerPositionGameStart_[XYZ_Y].SetEasing(startEasePlayerPosition_.y, endEasePlayerPosition.y, maxTime);
			easePlayerPositionGameStart_[XYZ_Z].SetEasing(startEasePlayerPosition_.z, endEasePlayerPosition.z, maxTime);

			//�J�����Z�b�g
			newplayer->SetCamera(camera_.get());

			//�X�V
			newplayer->Update();

			//���X�g�ɓo�^
			players_.push_back(std::move(newplayer));
		}
		//�G
		else if (objectData.objectType.find("ENEMY") == LDTOF_TRUE)
		{
			//�G������
			std::unique_ptr<BaseEnemy> newenemy;
			std::unique_ptr<Player>& player = players_.front();

			newenemy = enemyFactory_->CreateEnemy(objectData.objectPattern,
				model, modelEnemyBullet_.get(), player.get(), this);

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newenemy->SetPosition(pos);
			newenemy->SetStartPos(pos);//�������W�̃Z�b�g
			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newenemy->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newenemy->SetScale(scale);

			//�J�����Z�b�g
			newenemy->SetCamera(camera_.get());

			//�X�V
			newenemy->Update();

			//���X�g�ɓo�^
			enemys_.push_back(std::move(newenemy));

		}
		//�d�|��
		else if (objectData.objectType.find("GIMMICK") == LDTOF_TRUE)
		{
			//�G������
			std::unique_ptr<BaseGimmick> newGimmick;
			std::unique_ptr<Player>& player = players_.front();

			newGimmick = gimmickFactory_->CreateGimmick(objectData.objectPattern, model, player.get());

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newGimmick->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newGimmick->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			//�J�����Z�b�g
			newGimmick->SetCamera(camera_.get());

			//�X�V
			newGimmick->Update();

			//���X�g�ɓo�^
			gimmicks_.push_back(std::move(newGimmick));
		}
		//������
		else if (objectData.objectType.find("MOVEGROUND") == LDTOF_TRUE)
		{
			//�G������
			std::unique_ptr<BaseGimmick> newGimmick;
			std::unique_ptr<Player>& player = players_.front();

			newGimmick = gimmickFactory_->CreateMoveGround(objectData.objectPattern, model, player.get());

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newGimmick->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newGimmick->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newGimmick->SetScale(scale);

			//�J�����Z�b�g
			newGimmick->SetCamera(camera_.get());

			//�X�V
			newGimmick->Update();

			//���X�g�ɓo�^
			gimmicks_.push_back(std::move(newGimmick));
		}
		//�S�[��
		else if (objectData.objectType.find("GOAL") == LDTOF_TRUE)
		{
			//�S�[��������
			std::unique_ptr<Goal> newgoal;
			newgoal = Goal::Create(model);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newgoal->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newgoal->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newgoal->SetScale(scale);

			//�J�����Z�b�g
			newgoal->SetCamera(camera_.get());

			//�X�V
			newgoal->Update();

			//���X�g�ɓo�^
			goals_.push_back(std::move(newgoal));
		}
		//�A�C�e��
		else if (objectData.objectType.find("ITEM") == LDTOF_TRUE)
		{
			//�A�C�e��������
			std::unique_ptr<Item> newitem;
			std::unique_ptr<Player>& player = players_.front();
			//�W�����v
			if (objectData.objectPattern.find("JUMP") == LDTOF_TRUE)
				newitem = Item::Create(modelItemJump_.get(), player.get(), SUBCOLLISION_ATTR_ITEM_JUMP);
			//�񕜃A�C�e��
			else if (objectData.objectPattern.find("HEAL") == LDTOF_TRUE)
				newitem = Item::Create(modelItemHeal_.get(), player.get(), SUBCOLLISION_ATTR_ITEM_HEAL);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newitem->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newitem->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newitem->SetScale(scale);

			//�J�����Z�b�g
			newitem->SetCamera(camera_.get());

			//�X�V
			newitem->Update();

			//���X�g�ɓo�^
			items_.push_back(std::move(newitem));
		}
		//�����蔻�肪�Ȃ��I�u�W�F�N�g
		else if (objectData.objectType.find("NONE") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr<Object3d> newObject = Object3d::Create();
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());

			// �z��ɓo�^
			objects_.push_back(std::move(newObject));
		}
		//�V��
		else if (objectData.objectType.find("SKYDOME") == LDTOF_TRUE)
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr<Object3d> newObject = Object3d::Create();
			//�I�u�W�F�N�g�Ƀ��f���R�t����
			newObject->SetModel(model);

			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());

			// �z��ɓo�^
			skydomes_.push_back(std::move(newObject));
		}
		//�n�`
		else
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			std::unique_ptr<TouchableObject> newObject = TouchableObject::Create(model, true);
			// ���W
			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, objectData.trans);
			newObject->SetPosition(pos);

			// ��]�p
			DirectX::XMFLOAT3 rot;
			DirectX::XMStoreFloat3(&rot, objectData.rot);
			newObject->SetRotation(rot);

			// �X�P�[�����O
			DirectX::XMFLOAT3 scale;
			DirectX::XMStoreFloat3(&scale, objectData.scale);
			newObject->SetScale(scale);

			//�J�����Z�b�g
			newObject->SetCamera(camera_.get());

			// �z��ɓo�^
			objects_.push_back(std::move(newObject));
		}

	}

}

void GamePlayScene::UpdateChangeColor()
{
	//�F��ς���X�s�[�h
	const float speedColor = 0.02f;

	if (isColorReverse_)
	{
		//���]������
		infoColor_.x -= speedColor;
		infoColor_.y -= speedColor;
		infoColor_.z -= speedColor;
	}

	else
	{
		//�F�͎����I�ɑ���
		infoColor_.x += speedColor;
		infoColor_.y += speedColor;
		infoColor_.z += speedColor;

	}

	const DirectX::XMFLOAT2 maxAndMinSpeedColor = { 0.5f,0.0f };//{max,min}

	if (infoColor_.x >= maxAndMinSpeedColor.x)//Max�l�ɓ��B�����甽�]
	{
		isColorReverse_ = true;
	}
	if (infoColor_.x <= maxAndMinSpeedColor.y)//Min�l�ɓ��B�����甽�]��߂�
	{
		isColorReverse_ = false;
	}
}

void GamePlayScene::AddPlayerBullet(std::unique_ptr<PlayerBullet> playerBullet)
{
	//���X�g�ɓo�^
	playerBullets_.push_back(std::move(playerBullet));
}

void GamePlayScene::AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet)
{
	//���X�g�ɓo�^
	enemyBullets_.push_back(std::move(enemyBullet));
}

void GamePlayScene::LoadModel()
{
	//���f���ǂݍ���Ń}�b�v�ɓo�^����
	ModelMapping(modelPlayer_, "playeridle");					//���@���f��
	ModelMapping(modelPlayerBullet_, "playerbullet");			//���@�e���f��
	ModelMapping(modelPlayerDash_, "playerdash");				//���@�_�b�V�����f��
	ModelMapping(modelPlayerJump_, "playerjump");				//���@�W�����v���f��
	ModelMapping(modelPlayerAttack_, "playerattack");			//���@�U�����f��
	ModelMapping(modelPlayerHit_, "playerhit");					//���@���S���f��
	ModelMapping(modelEnemy1_, "enemy1");						//�ʏ�G���f��
	ModelMapping(modelEnemy2_, "enemy2");						//�h�b�X�����G���f��
	ModelMapping(modelEnemyDanger_, "enemydanger");				//�댯�ȓG���f��
	ModelMapping(modelEnemyBullet_, "enemybullet");				//�G�e���f��
	ModelMapping(modelBoss1_, "boss1");							//�X�e�[�W1�̃{�X���f��
	ModelMapping(modelBossCore1_, "core1");						//�X�e�[�W1�̃{�X�̎���ɂ���R�A�̃��f��
	ModelMapping(modelGoal_, "sphere");							//�S�[�����f��
	ModelMapping(modelItemJump_, "itemjump");					//�W�����v�͋����A�C�e�����f��
	ModelMapping(modelItemHeal_, "itemheal");					//���C�t�񕜃A�C�e�����f��
	ModelMapping(modelSpike_, "spikeball");						//�g�Q�̃��f��
	ModelMapping(modelBoxUpDown_, "boxud");						//�㉺���鏰�̃��f��
	ModelMapping(modelStageT_, "skydomet");						//�`���[�g���A���X�e�[�W���f��(�V��)
	ModelMapping(modelStage1_, "skydome");						//�X�e�[�W1���f��(�V��)
	ModelMapping(modelStage2_, "skydome2");						//�X�e�[�W2���f��(�V��)
	ModelMapping(modelGround_, "ground");						//���̃��f��
	ModelMapping(modelSphere_, "sphere2");						//�����f��
	ModelMapping(modelBox_, "ground2");							//AABB�����f��

	//���@�Ɏg�����f��
	modelPlayerList_.playerModel = modelPlayer_.get();				//��{�Ԑ�
	modelPlayerList_.playerBullet = modelPlayerBullet_.get();		//�e
	modelPlayerList_.playerHit = modelPlayerHit_.get();				//�q�b�g
	modelPlayerList_.playerMove = modelPlayerDash_.get();			//�_�b�V��
	modelPlayerList_.playerJump = modelPlayerJump_.get();			//�W�����v
	modelPlayerList_.playerAttack = modelPlayerAttack_.get();		//�U��
}

void GamePlayScene::SettingTutorialEase(const int num, Sprite* s1, Sprite* s2,
	Sprite* s3, Sprite* s4, Sprite* s5, Sprite* s6)const
{
	//�`���[�g���A���C�[�W���O�̃Z�b�e�B���O
	switch (num)
	{
	case GPSSTEN_Stundby://�X�^���o�C��
		if (s1 != nullptr)s1->SetPosition({ easeInfoTutorial_[TIEN_Move].start,tutorialInfoPosY_[TIEN_Move] });
		if (s2 != nullptr)s2->SetPosition({ easeInfoTutorial_[TIEN_Dash].start,tutorialInfoPosY_[TIEN_Dash] });
		if (s3 != nullptr)s3->SetPosition({ easeInfoTutorial_[TIEN_Jump].start,tutorialInfoPosY_[TIEN_Jump] });
		if (s4 != nullptr)s4->SetPosition({ easeInfoTutorial_[TIEN_MoveBack].start,tutorialInfoPosY_[TIEN_MoveBack] });
		if (s5 != nullptr)s5->SetPosition({ easeInfoTutorial_[TIEN_Attack].start,tutorialInfoPosY_[TIEN_Attack] });
		if (s6 != nullptr)s6->SetPosition({ easeInfoTutorial_[TIEN_Info].start,tutorialInfoPosY_[TIEN_Info] });
		break;
	case GPSSTEN_Active://�C�[�W���O�J�n��

		if (s1 != nullptr)s1->SetPosition({ easeInfoTutorial_[TIEN_Move].num_X,tutorialInfoPosY_[TIEN_Move] });
		if (s2 != nullptr)s2->SetPosition({ easeInfoTutorial_[TIEN_Dash].num_X,tutorialInfoPosY_[TIEN_Dash] });
		if (s3 != nullptr)s3->SetPosition({ easeInfoTutorial_[TIEN_Jump].num_X,tutorialInfoPosY_[TIEN_Jump] });
		if (s4 != nullptr)s4->SetPosition({ easeInfoTutorial_[TIEN_MoveBack].num_X,tutorialInfoPosY_[TIEN_MoveBack] });
		if (s5 != nullptr)s5->SetPosition({ easeInfoTutorial_[TIEN_Attack].num_X,tutorialInfoPosY_[TIEN_Attack] });
		if (s6 != nullptr)s6->SetPosition({ easeInfoTutorial_[TIEN_Info].num_X,tutorialInfoPosY_[TIEN_Info] });
		break;
	}

}

void GamePlayScene::UpdateTutorialSprite()
{
	if (stageNum_ >= SL_StageTutorial_Area1)//�`���[�g���A���X�e�[�W�ȊO���Ɖ����N����Ȃ�
	{
		//�`���[�g���A�������J���[
		const DirectX::XMFLOAT4 infoColorRed = { 0.1f + infoColor_.x,0.0f,0.0f,1.0f };
		const DirectX::XMFLOAT4 infoColorBlue = { 0.0f,0.0f,0.1f + infoColor_.z,1.0f };

		//������������J���[
		const DirectX::XMFLOAT4 howToPlayColorActiveRed = { 1.0f,0.1f,0.1f,1.0f };
		const DirectX::XMFLOAT4 howToPlayColorActiveBlue = { 0.1f,0.1f,1.0f,1.0f };
		const DirectX::XMFLOAT4 howToPlayColorDefault = { 0.01f,0.01f,0.01f,1.0f };
		//�J���[�X�V
		UpdateChangeColor();

		//�J���[�Z�b�g
		spriteTutorialHTPDash_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPMove_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPJump_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPMoveBack_->SetColor(howToPlayColorDefault);
		spriteTutorialHTPAttack_->SetColor(howToPlayColorDefault);

		//�_�b�V��
		if (input_->PushKey(DIK_LSHIFT) || input_->PushKey(DIK_RSHIFT))
		{
			if (input_->PushKey(DIK_A))spriteTutorialHTPDash_->SetColor(howToPlayColorActiveBlue);
			if (input_->PushKey(DIK_D))spriteTutorialHTPDash_->SetColor(howToPlayColorActiveRed);
		}
		else//�ʏ�ړ�
		{
			if (input_->PushKey(DIK_A))spriteTutorialHTPMove_->SetColor(howToPlayColorActiveBlue);
			if (input_->PushKey(DIK_D))spriteTutorialHTPMove_->SetColor(howToPlayColorActiveRed);
		}
		//�W�����v
		if (input_->PushKey(DIK_SPACE))spriteTutorialHTPJump_->SetColor(howToPlayColorActiveRed);
		//�����W�����v
		if (input_->PushKey(DIK_W))spriteTutorialHTPMoveBack_->SetColor(howToPlayColorActiveRed);
		if (input_->PushKey(DIK_S))spriteTutorialHTPMoveBack_->SetColor(howToPlayColorActiveBlue);
		//�U��
		if (input_->PushKey(DIK_L))spriteTutorialHTPAttack_->SetColor(howToPlayColorActiveRed);
		//�J���[�Z�b�g
		spriteTutorialInfo1_->SetColor(infoColorRed);
		spriteTutorialInfo2_->SetColor(infoColorRed);
		spriteTutorialInfo3_->SetColor(infoColorRed);
		spriteTutorialInfo4_->SetColor(infoColorBlue);
	}
	//�`���[�g���A���X�v���C�g���X�V
	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();
	
	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();
}

void GamePlayScene::DrawTutorialSprite(const Sprite* s1, const Sprite* s2,
	const Sprite* s3, const Sprite* s4, const Sprite* s5, const Sprite* s6)const
{
	//�`���[�g���A���p�̃X�v���C�g���ꊇ�`��
	if (s1 != nullptr)s1->Draw();
	if (s2 != nullptr)s2->Draw();
	if (s3 != nullptr)s3->Draw();
	if (s4 != nullptr)s4->Draw();
	if (s5 != nullptr)s5->Draw();
	if (s6 != nullptr)s6->Draw();
}

void GamePlayScene::LoadStageNameSprite()
{
	//�X�e�[�W���ƂɃX�e�[�W����ύX
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-1.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-1.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-2.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-3.png");
	else if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(GPSTI_StageNameTex, "texture/stagename/2-4.png");
	//�A���J�[�|�C���g�Ȃǂ��Z�b�e�B���O
	spriteStageName_->Initialize(spCommon_, GPSTI_StageNameTex);
	spriteStageName_->SetPosition(stageNamePos_);
	spriteStageName_->SetAnchorPoint(ANCHOR_POINT_CENTRAL);
	spriteStageName_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������
	spriteStageName_->Update();
}

void GamePlayScene::LoadTutorialSprite()
{
	//�`���[�g���A�����̐��������X�v���C�g
	spCommon_->LoadTexture(GPSTTI_TutorialInfo1Tex, "texture/info/tinfo1.png");//1
	spriteTutorialInfo1_->Initialize(spCommon_, GPSTTI_TutorialInfo1Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo2Tex, "texture/info/tinfo2.png");//2
	spriteTutorialInfo2_->Initialize(spCommon_, GPSTTI_TutorialInfo2Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo3Tex, "texture/info/tinfo3.png");//3
	spriteTutorialInfo3_->Initialize(spCommon_, GPSTTI_TutorialInfo3Tex);
	spCommon_->LoadTexture(GPSTTI_TutorialInfo4Tex, "texture/info/tinfo4.png");//4
	spriteTutorialInfo4_->Initialize(spCommon_, GPSTTI_TutorialInfo4Tex);

	//�`���[�g���A�����̑������
	spCommon_->LoadTexture(GPSTTI_HowToPlayMoveTex, "texture/info/moveinfo.png");//�ړ��@1~
	spriteTutorialHTPMove_->Initialize(spCommon_, GPSTTI_HowToPlayMoveTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayDashTex, "texture/info/dashinfo.png");//�_�b�V���@1~
	spriteTutorialHTPDash_->Initialize(spCommon_, GPSTTI_HowToPlayDashTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayJumpTex, "texture/info/jumpinfo.png");//�W�����v�@1~
	spriteTutorialHTPJump_->Initialize(spCommon_, GPSTTI_HowToPlayJumpTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayBackMoveTex, "texture/info/backmoveinfo2.png");//��O�A�����W�����v�@2~
	spriteTutorialHTPMoveBack_->Initialize(spCommon_, GPSTTI_HowToPlayBackMoveTex);
	spCommon_->LoadTexture(GPSTTI_HowToPlayAttackTex, "texture/info/attackinfo2.png");//�U���@3~
	spriteTutorialHTPAttack_->Initialize(spCommon_, GPSTTI_HowToPlayAttackTex);

	//�`���[�g���A���̐i�񂾖ʂɂ���ă`���[�g���A������ς���
	if (stageNum_ == SL_StageTutorial_Area1)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			nullptr, nullptr, spriteTutorialInfo1_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area2)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), nullptr, spriteTutorialInfo2_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Area3)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo3_.get());
	}
	else if (stageNum_ == SL_StageTutorial_Final)
	{
		SettingTutorialEase(GPSSTEN_Stundby, spriteTutorialHTPMove_.get(), spriteTutorialHTPDash_.get(), spriteTutorialHTPJump_.get(),
			spriteTutorialHTPMoveBack_.get(), spriteTutorialHTPAttack_.get(), spriteTutorialInfo4_.get());
	}
	else//�`���[�g���A���X�e�[�W�ȊO�̏ꍇ
	{
		//�J���[�Z�b�g
		const DirectX::XMFLOAT4 howToPlayColor = { 1.0f,0.15f,0.15f,1.0f };
		//�J���[�Z�b�g
		spriteTutorialHTPDash_->SetColor(howToPlayColor);
		spriteTutorialHTPMove_->SetColor(howToPlayColor);
		spriteTutorialHTPJump_->SetColor(howToPlayColor);
		spriteTutorialHTPMoveBack_->SetColor(howToPlayColor);
		spriteTutorialHTPAttack_->SetColor(howToPlayColor);
	}
}

void GamePlayScene::SetUpCircleShadow(const DirectX::XMFLOAT3& pos)
{
	const DirectX::XMVECTOR dir = { 0.0f,1.0f,0.0f,0.0f };			//���e����
	const DirectX::XMFLOAT3 casterPosOffset = { -0.5f,0.0f,0.0f };	//�L���X�^�[���W�̃I�t�Z�b�g
	//�L���X�^�[���W
	const DirectX::XMFLOAT3 casterPos =
	{
		pos.x + casterPosOffset.x,
		pos.y + casterPosOffset.y,
		pos.z + casterPosOffset.z
	};
	const DirectX::XMFLOAT3 atten = { 0.5f,0.6f,0.0f };				//���������W��
	const DirectX::XMFLOAT2 factorAngle = { 0.2f,0.5f };			//�p�x�����W��

	//�V���h�E�̃Z�b�g
	lightGroup_->SetCircleShadowDir(LightGroup::LN_0, dir);
	lightGroup_->SetCircleShadowCasterPos(LightGroup::LN_0, casterPos);
	lightGroup_->SetCircleShadowAtten(LightGroup::LN_0, atten);
	lightGroup_->SetCircleShadowFactorAngleCos(LightGroup::LN_0, factorAngle);
}

void GamePlayScene::LoadSprite()
{
	//�ǂ̃L�Ń|�[�Y����̂����������X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseInfoTex, "texture/pauseinfo.png");
	spritePauseInfo_->Initialize(spCommon_, GPSTI_PauseInfoTex);

	//�|�[�Y���̃X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseTex, "texture/pause.png");
	spritePause_->Initialize(spCommon_, GPSTI_PauseTex);
	spritePause_->SetPosition({ easePauseMenuPosX_[PMEN_Menu].start,pausePosY_[PMEN_Menu] });

	//�|�[�Y���ɃQ�[���ĊJ���邩���������X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseResumeTex, "texture/resume.png");
	spritePauseResume_->Initialize(spCommon_, GPSTI_PauseResumeTex);
	spritePauseResume_->SetPosition({ easePauseMenuPosX_[PMEN_Resume].start,pausePosY_[PMEN_Resume] });

	//�|�[�Y���Ƀq���g���m�F���邩���������X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseHowToPlayTex, "texture/hint.png");
	spritePauseHint_->Initialize(spCommon_, GPSTI_PauseHowToPlayTex);
	spritePauseHint_->SetPosition({ easePauseMenuPosX_[PMEN_Hint].start,pausePosY_[PMEN_Hint] });

	//�|�[�Y���ɃX�e�[�W�Z���N�g�֖߂邩���������X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseStageSelectTex, "texture/backstageselect.png");
	spritePauseStageSelect_->Initialize(spCommon_, GPSTI_PauseStageSelectTex);
	spritePauseStageSelect_->SetPosition({ easePauseMenuPosX_[PMEN_StageSelect].start,pausePosY_[PMEN_StageSelect] });

	//�|�[�Y���Ƀ^�C�g���֖߂邩���������X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseTitleTex, "texture/backtitle.png");
	spritePauseTitle_->Initialize(spCommon_, GPSTI_PauseTitleTex);
	spritePauseTitle_->SetPosition({ easePauseMenuPosX_[PMEN_Title].start,pausePosY_[PMEN_Title] });

	//���j���[����X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseUITex, "texture/menuui.png");
	spritePauseUI_->Initialize(spCommon_, GPSTI_PauseUITex);
	spritePauseUI_->SetPosition({ easePauseMenuPosX_[PMEN_UI].start,pausePosY_[PMEN_UI] });

	//����\���̃X�v���C�g
	spCommon_->LoadTexture(GPSTI_PauseDoneTex, "texture/space.png");
	spriteDone_->Initialize(spCommon_, GPSTI_PauseDoneTex);
	spriteDone_->SetPosition({ easePauseMenuPosX_[PMEN_SelectSpace].start,pausePosY_[PMEN_SelectSpace] });

	//Ready�\�L�����p�̃X�v���C�g
	spCommon_->LoadTexture(GPSTI_ReadyTex, "texture/ready2.png");
	spriteReady_->Initialize(spCommon_, GPSTI_ReadyTex);
	spriteReady_->SetPosition({ easeReadyPosition_[XXY_X1].start,easeReadyPosition_[XXY_Y].start });
	spriteReady_->SetColor({ black_.x,black_.y,black_.z,easeFadeInOut_.start });//�^����

	//Go�\�L�����p�̃X�v���C�g
	spCommon_->LoadTexture(GPSTI_GoTex, "texture/go.png");
	spriteGo_->Initialize(spCommon_, GPSTI_GoTex);
	spriteGo_->SetSize({ easeGoSizeAndAlpha_[XYW_X].start,easeGoSizeAndAlpha_[XYW_Y].start });
	spriteGo_->SetPosition({ goPosition_[XY_X],goPosition_[XY_Y] });

	const XMFLOAT2 centralAnchorPoint = { 0.5f,0.5f };//�A���J�[�|�C���g�͒���
	spriteGo_->SetAnchorPoint(centralAnchorPoint);
	spriteGo_->SetColor({ black_.x,black_.y,black_.z,easeGoSizeAndAlpha_[XYW_W].start });

	//�t�F�[�h�C���A�E�g�X�v���C�g
	spCommon_->LoadTexture(GPSTI_FadeInOutTex, "texture/fade.png");
	spriteFadeInOut_->Initialize(spCommon_, GPSTI_FadeInOutTex);
	spriteFadeInOut_->SetColor({ white_.x,white_.y,white_.z, easeFadeInOut_.start });//�^����

	//���[�h�X�v���C�g
	spCommon_->LoadTexture(GPSTI_LoadingTex, "texture/load.png");
	spriteLoad_->Initialize(spCommon_, GPSTI_LoadingTex);
	spriteLoad_->SetPosition(loadPos_);
	spriteLoad_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//���݃X�e�[�W�X�v���C�g
	const int32_t remainderNum = stageNum_ % 10;//�]��ɂ���ăX�v���C�g��ς���
	if (remainderNum == SNL_Stage1) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage1.png");
	else if (remainderNum == SNL_Stage2) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage2.png");
	else if (remainderNum == SNL_Stage3) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stage3.png");
	else if (remainderNum == SNL_Stage4) spCommon_->LoadTexture(GPSTI_StageInfoNowTex, "texture/stagef.png");
	spriteStageInfoNow_->Initialize(spCommon_, GPSTI_StageInfoNowTex);
	spriteStageInfoNow_->SetPosition(stageInfoNowPos_);
	spriteStageInfoNow_->SetColor({ black_.x,black_.y,black_.z, easeFadeInOut_.end });//������

	//�J�[�\���X�v���C�g
	spCommon_->LoadTexture(GPSTI_CursorTex, "texture/cursor.png");
	spriteCursor_->Initialize(spCommon_, GPSTI_CursorTex);
	spriteCursor_->SetPosition({ easeCursorPosX_.start,pausePosY_[PMEN_Resume] });

	//�V�ѕ��������X�g�̃X�v���C�g
	spCommon_->LoadTexture(GPSTI_HowToPlayListTex, "texture/info/howtoplaylist2.png");
	spriteHowToPlayList_->Initialize(spCommon_, GPSTI_HowToPlayListTex);
	const XMFLOAT2 howToPlayListPosition = { 30.0f,70.0f };
	spriteHowToPlayList_->SetPosition(howToPlayListPosition);
	//�C�[�W���O��������
	easeTutorialListScale_[XY_X].SetEasing(0.0f, spriteHowToPlayList_->GetSize().x, presetEaseTutorialListScale_[XY_X].maxtime);
	easeTutorialListScale_[XY_Y].SetEasing(0.0f, spriteHowToPlayList_->GetSize().y, presetEaseTutorialListScale_[XY_Y].maxtime);
	spriteHowToPlayList_->SetSize({ easeTutorialListScale_[XY_X].start,easeTutorialListScale_[XY_Y].start });
	
	//�q���g�̓��e�̓X�e�[�W���ƂɈႤ
	if (stageNum_ == SL_StageTutorial_Area1)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-1.png");
	else if (stageNum_ == SL_StageTutorial_Area2)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-2.png");
	else if (stageNum_ == SL_StageTutorial_Area3)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-3.png");
	else if (stageNum_ == SL_StageTutorial_Final)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/1-4.png");
	else if (stageNum_ == SL_Stage1_Area1)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-1.png");
	else if (stageNum_ == SL_Stage1_Area2)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-2.png");
	else if (stageNum_ == SL_Stage1_Area3)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-3.png");
	else if (stageNum_ == SL_Stage1_AreaBoss)spCommon_->LoadTexture(GPSTI_HintInfoTex, "texture/stagehint/2-4.png");
	spriteHintInfo_->Initialize(spCommon_, GPSTI_HintInfoTex);
	spriteHintInfo_->SetPosition({ easePauseMenuPosX_[PMEN_HintInfo].start,pausePosY_[PMEN_HintInfo] });

	//�X�e�[�W���X�v���C�g
	LoadStageNameSprite();

	//�`���[�g���A���X�v���C�g
	LoadTutorialSprite();

	//�e�X�v���C�g���X�V
	spritePause_->Update();
	spritePauseInfo_->Update();
	spritePauseResume_->Update();
	spritePauseHint_->Update();
	spritePauseStageSelect_->Update();
	spritePauseTitle_->Update();
	spritePauseUI_->Update();
	spriteDone_->Update();
	spriteReady_->Update();
	spriteGo_->Update();
	spriteFadeInOut_->Update();

	spriteTutorialInfo1_->Update();
	spriteTutorialInfo2_->Update();
	spriteTutorialInfo3_->Update();
	spriteTutorialInfo4_->Update();
	spriteHintInfo_->Update();

	spriteTutorialHTPMove_->Update();
	spriteTutorialHTPDash_->Update();
	spriteTutorialHTPJump_->Update();
	spriteTutorialHTPMoveBack_->Update();
	spriteTutorialHTPAttack_->Update();

}
