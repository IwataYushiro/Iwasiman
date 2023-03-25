#include "MyGame.h"

using namespace DirectX;

void MyGame::Initialize()
{
	Framework::Initialize();

	//Audio
	audio_ = new Audio();
	//imgui
	imguiManager_ = new ImGuiManager();

	Object3d::StaticInitialize(dxCommon_->GetDevice());
	ParticleManager::StaticInitialize(dxCommon_->GetDevice());

	//�v���C���[�֌W
	player_ = new Player();
	//�G�֌W
	enemy_ = new Enemy();

#pragma region Windows������
#pragma endregion
	// DirectX�����������@��������
	//imgui
	imguiManager_->Initialize(winApp_, dxCommon_);

	// DirectX�����������@�����܂�
	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	audio_->Initialize();
	sound = audio_->SoundLordWave("Resources/TestMusic.wav");
	//�����Đ��Ăяo����
	audio_->SoundPlayWave(audio_->GetXAudio2(), sound);

	//�����Ńe�N�X�`�����w�肵�悤
	UINT titleTex = 00;
	sprCommon_->LoadTexture(titleTex, "texture/title.png");
	spriteTitle_->Initialize(sprCommon_, titleTex);

	UINT howtoplayTex = 01;
	sprCommon_->LoadTexture(howtoplayTex, "texture/howtoplay.png");
	spriteHowToPlay_->Initialize(sprCommon_, howtoplayTex);

	UINT gameClearTex = 02;
	sprCommon_->LoadTexture(gameClearTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(sprCommon_, gameClearTex);

	UINT gameOverTex = 03;
	sprCommon_->LoadTexture(gameOverTex, "texture/gameover.png");
	spriteGameOver_->Initialize(sprCommon_, gameOverTex);

	//3D�I�u�W�F�N�g�֌W

	//3D�I�u�W�F�N�g����
	object3DPlayer_ = Object3d::Create();
	object3DEnemy_ = Object3d::Create();

	//OBJ�t�@�C�����烂�f���f�[�^��ǂݍ���
	modelPlayer_ = Model::LoadFromOBJ("player");
	modelEnemy_ = Model::LoadFromOBJ("enemy1");

	//�I�u�W�F�N�g�Ƀ��f���R�t����
	object3DPlayer_->SetModel(modelPlayer_);
	object3DEnemy_->SetModel(modelEnemy_);
	//�J�������R�Â�
	object3DPlayer_->SetCamera(camera_);
	object3DEnemy_->SetCamera(camera_);
	//�p�[�e�B�N��
	p1=ParticleManager::Create();
	p1->SetCamera(camera_);
	//�|�W�V����
	player_->Initialize(modelPlayer_, object3DPlayer_, input_, camera_);

	enemy_->Initialize(modelEnemy_, object3DEnemy_, camera_);
	//�G�Ɏ��@�̃A�h���X��n��
	enemy_->SetPlayer(player_);

	//�V�[��
	scene_ = title;

#pragma endregion
	// �`�揉���������@�����܂�
}

void MyGame::Update()
{

	// DirectX���t���[�������@��������
	// �X�V������������
	Framework::Update();

	switch (scene_)
	{
	case title:
		for (int i = 0; i < 20; i++)
		{
			//X,Y,Z�S��{-20.0f,20.0f}�Ń����_���ɕ��z
			const float md_pos = 40.0f;
			XMFLOAT3 pos{};
			pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
			//X,Y,Z�S��{0.1f,0.1f}�Ń����_���ɕ��z
			const float md_vel = 0.2f;
			XMFLOAT3 vel{};
			vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
			//�d�͂Ɍ����Ă�Y�̂�{0.001f,0}�Ń����_���ɕ��z
			XMFLOAT3 acc{};
			const float md_acc = 0.001f;
			acc.y = -(float)rand() / RAND_MAX * md_acc;

			//�ǉ�
			p1->Add(60, pos, vel, acc, 3.0f, 0.0f);
		}

		p1->Update();
		if (input_->TriggerKey(DIK_SPACE))
		{
			player_->Reset();
			enemy_->Reset();
			scene_ = howtoplay;

			break;
		}
		spriteTitle_->Update();
		break;

	case howtoplay:
		if (input_->TriggerKey(DIK_SPACE))
		{
			scene_ = stage;
			break;
		}
		spriteHowToPlay_->Update();
		break;

	case stage:
		//�J����
		camera_->Update();

		//���f���Ăяo����
		player_->Update();
		enemy_->Update();

		ChackAllCollisions();

		if (player_->IsDead())
		{
			scene_ = gameover;
			break;
		}
		if (enemy_->IsDead())
		{
			scene_ = clear;
			break;
		}
		break;

	case clear:
		if (input_->TriggerKey(DIK_SPACE))
		{
			scene_ = title;
			break;
		}
		spriteGameClear_->Update();

		break;
	case gameover:
		if (input_->TriggerKey(DIK_SPACE))
		{
			scene_ = title;
			break;
		}
		spriteGameOver_->Update();

		break;


	}

	//imgui
	imguiManager_->Update();
	// �����܂�
}

void MyGame::Draw()
{
	//�`��O����
	dxCommon_->PreDraw();


	//�X�v���C�g�`��O����
	sprCommon_->PreDraw();

	//�w�i�X�v���C�g
	switch (scene_)
	{
	case title:
		//�X�v���C�g�`��
		//spriteTitle_->Draw();
		break;
	
	case howtoplay:
		spriteHowToPlay_->Draw();
		break;
	
	case stage:

		break;
	
	case clear:
		spriteGameClear_->Draw();
		break;

	case gameover:
		spriteGameOver_->Draw();
		break;


	}

	//���f��
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());

	//���f���`��
	switch (scene_)
	{
	case title:

		break;
	case howtoplay:

		break;
	case stage:
player_->Draw();
	enemy_->Draw();

		break;
	case clear:

		break;
	case gameover:

		break;

	}
	
	//���f���`��㏈��
	Object3d::PostDraw();
	//�G�t�F�N�g
	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	switch (scene_)
	{
	case title:
		p1->Draw();
		break;
	case howtoplay:

		break;
	case stage:

		break;
	case clear:

		break;
	case gameover:

		break;

	}

	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();
	//�O�i�X�v���C�g

	//ImGui�̕\��
	imguiManager_->Draw();
	//�`��㏈��
	dxCommon_->PostDraw();
}

void MyGame::Finalize()
{
	//�I������
	imguiManager_->Finalize();
	audio_->Finalize();

	//���
	//�e�퉹��
	audio_->SoundUnLoad(&sound);
	//�����S��
	delete audio_;

	//�X�v���C�g
	delete spriteTitle_;
	delete spriteHowToPlay_;
	delete spriteGameClear_;
	delete spriteGameOver_;
	//�p�[�e�B�N��
	delete p1;
	//delete p2;
	//���f��
	//3D�I�u�W�F�N�g
	delete object3DPlayer_;
	delete object3DEnemy_;

	//3D���f��
	delete modelPlayer_;
	delete modelEnemy_;

	//��Ռn
	delete player_;
	delete enemy_;
	delete imguiManager_;


	Framework::Finalize();
}

//�Փ˔���Ɖ���
void MyGame::ChackAllCollisions() {

	//����Ώ�A,B�̍��W
	XMFLOAT3 posA, posB;
	// A,B�̍��W�̋����p
	XMFLOAT3 posAB;
	//����Ώ�A,B�̔��a
	float radiusA;
	float radiusB;
	float radiiusAB;

	//���@�e���X�g���擾
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	//�G�e���X�g���擾
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetEnemyBullets();

#pragma region ���@�ƓG�e�̓����蔻��
	//���ꂼ��̔��a
	radiusA = 1.0f;
	radiusB = 1.0f;

	//���@�̍��W
	posA = player_->GetWorldPosition();

	//���@�ƑS�Ă̓G�e�̓����蔻��
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//�G�e�̍��W
		posB = bullet->GetWorldPosition();
		//���WA,B�̋��������߂�
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//���Ƌ��̌�������
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			//���L�����̏Փˎ��R�[���o�b�N�֐����Ăяo��
			player_->OnCollision();
			//�G�e�̏Փˎ��R�[���o�b�N�֐����Ăяo��
			bullet->OnCollision();
		}
	}

#pragma endregion

#pragma region ���e�ƓG�̓����蔻��
	//���ꂼ��̔��a
	radiusA = 5.0f;
	radiusB = 1.0f;

	//�G�̍��W
	posA = enemy_->GetWorldPosition();

	//�G�ƑS�Ă̒e�̓����蔻��
	for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		//�e�̍��W
		posB = bullet->GetWorldPosition();
		//���WA,B�̋��������߂�
		posAB.x = (posB.x - posA.x) * (posB.x - posA.x);
		posAB.y = (posB.y - posA.y) * (posB.y - posA.y);
		posAB.z = (posB.z - posA.z) * (posB.z - posA.z);
		radiiusAB = (radiusA + radiusB) * (radiusA + radiusB);

		//���Ƌ��̌�������
		if (radiiusAB >= (posAB.x + posAB.y + posAB.z)) {
			//�G�L�����̏Փˎ��R�[���o�b�N�֐����Ăяo��
			enemy_->OnCollisionPlayer();
			//���@�e�̏Փˎ��R�[���o�b�N�֐����Ăяo��
			bullet->OnCollision();
		}
	}
#pragma endregion

}