#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include "Easing.h"
#include "Sprite.h"

#include <DirectXMath.h>
#include <list>
#include <memory>
#include "Gauge.h"

class CollisionManager;

class Earth :public Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	const float MUTEKI_COUNT = 60.0f;
public:
	~Earth();
	static std::unique_ptr<Earth> Create(Model* model = nullptr);

	//������
	bool Initialize()override;
	void Reset();
	//�X�V
	void Update()override;

	//�]��
	void Trans();

	//���[���h���W���擾
	XMFLOAT3 GetWorldPosition();

	//�`��
	void Draw();

	//UI�`��
	void DrawSprite();

	//��n��
	void Finalize();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)override;

private:
	static CollisionManager* colManager_;
	//�|�W�V����
	XMFLOAT3 pos;
	XMFLOAT3 scale;

	bool isHit_ = false;
	bool isDead_ = false;
	int mutekiCount = 0;
	int life_ = 3;
	int maxLife_ = 0;

	float radius_ = 50.0f;

	//�C�[�W���O
	float count = 0.0f;
	const float timer = MUTEKI_COUNT / 60.0f;
	Easing ease = Easing(1.0f, 0.0f, timer);
	//�X�v���C�g
	SpriteCommon* spCommon_ = SpriteCommon::GetInstance();
	Sprite* spriteHit_ = new Sprite();

	//HP�Q�[�W
	Gauge* hpGauge_ = {};

	//HP�Q�[�W�̈ʒu(����p)
	XMFLOAT2 positionHPGauge_ = {0,0};

	//HP�p�C�[�W���O�ő厞��
	float maxTimeHP_ = 30.0f;

public: //�A�N�Z�b�T�A�C�����C���֐�
	bool IsDead() const { return isDead_; }
	//���C�t
	void SetLife(int life) { this->life_ = life; }
	const int& GetLife()const { return life_; }

	//HP�Q�[�W�̎擾
	Gauge* GetHPGauge() const {
		return hpGauge_;
	}
};
#pragma once
