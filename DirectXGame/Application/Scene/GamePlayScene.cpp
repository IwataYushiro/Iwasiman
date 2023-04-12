#include "GamePlayScene.h"
#include "TitleScene.h"

using namespace DirectX;

DirectXCommon* GamePlayScene::dxCommon_ = DirectXCommon::GetInstance();
SpriteCommon* GamePlayScene::spCommon_ = SpriteCommon::GetInstance();
Input* GamePlayScene::input_ = Input::GetInstance();
Audio* GamePlayScene::audio_ = Audio::GetInstance();
SceneManager* GamePlayScene::sceneManager_ = SceneManager::GetInstance();
ImGuiManager* GamePlayScene::imguiManager_ = ImGuiManager::GetInstance();

void GamePlayScene::Initialize()
{
	//�J����
	camera_ = new Camera();
	//�v���C���[�֌W
	player_ = new Player();
	//�G�֌W
	enemy_ = new Enemy();

	// �`�揉���������@��������
#pragma region �`�揉��������
	//�����f�[�^
	sound = audio_->SoundLoadWave("Resources/TestMusic.wav");
	//�����Đ��Ăяo����
	audio_->SoundPlayWave(audio_->GetXAudio2(), sound);

	UINT gameClearTex = 02;
	spCommon_->LoadTexture(gameClearTex, "texture/gameclear.png");
	spriteGameClear_->Initialize(spCommon_, gameClearTex);
	spriteGameClear_->SetAnchorPoint({ 0.5f,0.5f });

	UINT gameOverTex = 03;
	spCommon_->LoadTexture(gameOverTex, "texture/gameover.png");
	spriteGameOver_->Initialize(spCommon_, gameOverTex);
	spriteGameOver_->SetAnchorPoint({ -0.5f,-0.5f });

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
	particle1_ = Particle::LoadFromParticleTexture("particle6.png");
	pm1_ = ParticleManager::Create();
	pm1_->SetParticleModel(particle1_);
	pm1_->SetCamera(camera_);

	particle2_ = Particle::LoadFromParticleTexture("particle5.png");
	pm2_ = ParticleManager::Create();
	pm2_->SetParticleModel(particle2_);
	pm2_->SetCamera(camera_);
	//�|�W�V����
	player_->Initialize(modelPlayer_, object3DPlayer_, input_, camera_);

	enemy_->Initialize(modelEnemy_, object3DEnemy_, camera_);
	//�G�Ɏ��@�̃A�h���X��n��
	enemy_->SetPlayer(player_);

}

void GamePlayScene::Update()
{
	pm1_->Active(particle1_, 120.0f, 0.2f, 0.001f, 15, { 10.0f, 0.0f });
	pm2_->Active(particle2_, 100.0f, 0.2f, 0.001f, 5, { 6.0f,0.0f });

	//���f���Ăяo����
	player_->Update();
	enemy_->Update();

	ChackAllCollisions();

	spriteGameClear_->Update();
	spriteGameOver_->Update();

	//�J����
	camera_->Update();

	pm1_->Update();
	pm2_->Update();

	if (input_->TriggerKey(DIK_RETURN))
	{
		BaseScene* scene = new TitleScene();

		sceneManager_->SetNextScene(scene);
	}
}

void GamePlayScene::Draw()
{

	//�X�v���C�g�`��O����
	spCommon_->PreDraw();

	//�w�i�X�v���C�g

	spriteGameClear_->Draw();
	spriteGameOver_->Draw();

	//�G�t�F�N�g
	//�G�t�F�N�g�`��O����
	ParticleManager::PreDraw(dxCommon_->GetCommandList());

	//�G�t�F�N�g�`��
	pm1_->Draw();
	pm2_->Draw();

	//�G�t�F�N�g�`��㏈��
	ParticleManager::PostDraw();

	//���f��
	//���f���`��O����
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//���f���`��
	player_->Draw();
	enemy_->Draw();

	//���f���`��㏈��
	Object3d::PostDraw();

	//�O�i�X�v���C�g

	//ImGui�̕\��

	imguiManager_->Draw();
}

void GamePlayScene::Finalize()
{
	//�I������
	audio_->Finalize();
	//���
	//�e�퉹��
	audio_->SoundUnLoad(&sound);
	//�X�v���C�g
	delete spriteTitle_;
	delete spriteHowToPlay_;
	delete spriteGameClear_;
	delete spriteGameOver_;
	//�p�[�e�B�N��
	delete particle1_;
	delete pm1_;
	delete particle2_;
	delete pm2_;
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

}
//�Փ˔���Ɖ���
void GamePlayScene::ChackAllCollisions() {

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