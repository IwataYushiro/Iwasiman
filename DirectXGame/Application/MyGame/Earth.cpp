#include "Earth.h"
#include "SphereCollider.h"
#include <cassert>
#include "CollisionAttribute.h"
#include "CollisionManager.h"

using namespace DirectX;
CollisionManager* Earth::colManager_ = CollisionManager::GetInstance();

Earth::~Earth()
{
	delete spriteHit_;
}

std::unique_ptr<Earth> Earth::Create(Model* model)
{
	//�C���X�^���X����
	std::unique_ptr<Earth> ins = std::make_unique<Earth>();
	if (ins == nullptr) return nullptr;

	//������
	if (!ins->Initialize())
	{
		ins.release();
		assert(0);
	}
	//���f���̃Z�b�g
	if (model) ins->SetModel(model);
	return ins;
}

bool Earth::Initialize()
{
	if (!Object3d::Initialize()) return false;
	life_ = 5;
	maxLife_ = life_;
	isDead_ = false;
	isHit_ = false;
	mutekiCount = 0;

	//�R���C�_�[�ǉ�
	SetCollider(new SphereCollider(XMVECTOR{ 0.0f,0.0f,0.0f,0.0f }, radius_));
	collider->SetAttribute(COLLISION_ATTR_EARTH);
	collider->SetSubAttribute(SUBCOLLISION_ATTR_NONE);

	//�X�v���C�g�֌W
	spCommon_->LoadTexture(30, "texture/pdamage.png");
	spriteHit_->Initialize(spCommon_, 30);

	spriteHit_->Update();

#pragma region HP�X�v���C�g
	hpGauge_ = new Gauge();
	hpGauge_->Initialize();

	hpGauge_->SetRestMax(static_cast<float>(life_));
	hpGauge_->SetRest(static_cast<float>(life_));
	hpGauge_->SetMaxTime(maxTimeHP_);

	hpGauge_->SetPosition({ 940,64 });
	hpGauge_->SetSize({ 1,1 });
#pragma endregion

	return true;
}

void Earth::Reset()
{
	life_ = 5;
	isDead_ = false;
	isHit_ = false;
	mutekiCount = 0;
}

void Earth::Update()
{
	if (!isDead_)
	{
		if (life_ <= 0)
		{
			isDead_ = true;
		}
		
		if (isHit_)
		{
			camera_->ShakeEye({ 0.0f, 6.0f, -115.0f }, 10, { -5.0f,1.0f,-130.0f }, { 5.0f,11.0f,-100.0f });
			camera_->ShakeTarget({ 0.0f,5.0f,0.0f }, 10, { -5.0f,0.0f,-5.0f }, { 5.0f,10.0f,5.0f });
			camera_->Update();

			ease.ease_out_cubic();
			spriteHit_->SetColor({ 1.0f, 1.0f,1.0f, ease.num_X });

			mutekiCount++;
		}
		else
		{
			camera_->Reset();
			spriteHit_->SetColor({ 1.0f, 1.0f,1.0f, ease.start });
		}	

		if (mutekiCount == MUTEKI_COUNT)
		{
			
			isHit_ = false;
			mutekiCount = 0;
		}

	}
	rotation.y += 2.0f;
	camera_->Update();
	UpdateWorldMatrix();
	collider->Update();

	//�s��X�V��
	Object3d::Update();
	spriteHit_->Update();

#pragma region HP�X�v���C�g
	hpGauge_->SetRest(static_cast<float>(life_));
	//�ʏ�͗΁A�s���`�Ő�
	if (life_ <= maxLife_ / 2) {
		hpGauge_->GetRestSprite()->
			SetColor({ 0.7f,0.2f,0.2f,1.0f });
	}
	else {
		hpGauge_->GetRestSprite()->
			SetColor({ 0.2f,0.7f,0.2f,1.0f });
	}

	hpGauge_->Update();
#pragma endregion
}

void Earth::Trans()
{
	XMMATRIX world;
	//�s��X�V
	world = XMMatrixIdentity();
	XMMATRIX matWorld = XMMatrixIdentity();

	XMMATRIX matScale = XMMatrixScaling(Object3d::GetScale().x, Object3d::GetScale().y, Object3d::GetScale().z);

	XMMATRIX matRot = XMMatrixRotationZ(Object3d::GetRotation().z)
		* XMMatrixRotationX(Object3d::GetRotation().x) * XMMatrixRotationY(Object3d::GetRotation().y);

	XMMATRIX matTrans = XMMatrixTranslation(Object3d::GetPosition().x,
		Object3d::GetPosition().y, Object3d::GetPosition().z);

	//����
	matWorld = matScale * matRot * matTrans;

	world = matWorld;
	Object3d::SetWorld(world);
}

XMFLOAT3 Earth::GetWorldPosition()
{
	//���[���h���W���擾
	XMFLOAT3 worldPos;

	//���[���h�s��̕��s�ړ��������擾
	worldPos.x = Object3d::GetPosition().x;
	worldPos.y = Object3d::GetPosition().y;
	worldPos.z = Object3d::GetPosition().z;

	return worldPos;
}

void Earth::Draw()
{
	Object3d::Draw();
}

void Earth::DrawSprite()
{
	if (isHit_)spriteHit_->Draw();
	hpGauge_->Draw();
}

void Earth::Finalize() {
	hpGauge_->Finalize();
}

void Earth::OnCollision(const CollisionInfo& info, unsigned short attribute, unsigned short subAttribute)
{
	if (attribute == COLLISION_ATTR_ENEMYS)
	{
		if (isHit_)return;

		if (subAttribute == SUBCOLLISION_ATTR_NONE)
		{
			life_--;
			ease.Standby(false);
			isHit_ = true;

			//�_���[�W�󂯂���HP�̕ϓ������s
			hpGauge_->SetRest(static_cast<float>(life_));
			hpGauge_->DecisionFluctuation();
			hpGauge_->SetIsFluct(true);
		}
		else if (subAttribute == SUBCOLLISION_ATTR_BULLET)
		{
			//life_--;
			// isHit_ = true;
		}
	}

}
