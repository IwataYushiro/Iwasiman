#pragma once
#include "Camera.h"
#include "Model.h"
#include "Object3d.h"
#include "ParticleManager.h"
#include <DirectXMath.h>
#include <list>
#include <memory>

/*

*	Goal.h

*	�S�[��

*/
class Goal :public IwasiEngine::Object3d
{
private:
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	//IwasiEngine::���ȗ�
	using Model = IwasiEngine::Model;
	using Camera = IwasiEngine::Camera;
	using CollisionInfo = IwasiEngine::CollisionInfo;
	using Particle = IwasiEngine::Particle;
	using ParticleManager = IwasiEngine::ParticleManager;
public:
	
	//����(�g�p���f���A�n�`���t���O)
	static std::unique_ptr<Goal> Create(const Model* model = nullptr, const bool isLandShape = false);
	//������(�n�`���t���O)
	bool Initialize(const bool isLandShape = false);
	//���Z�b�g
	void Reset();
	//�X�V
	void Update()override;

	//�]��
	void Trans();

	//�`��
	void Draw();
	//�p�[�e�B�N���`��
	void DrawParticle();

	//�Փ˂����o������Ăяo�����R�[���o�b�N�֐�(�R���W�������A���C�������A�T�u����)
	void OnCollision(const CollisionInfo& info, const unsigned short attribute, const unsigned short subAttribute)override;

private:
	//�S�[��������
	bool isGoal_ = false;
	//���a
	const float radius_ = 10.0f;
	//�p�[�e�B�N��
	std::unique_ptr<Particle> particle_ = nullptr;
	//�p�[�e�B�N���}�l�[�W���[
	std::unique_ptr<ParticleManager> pm_ = nullptr;

public: //�A�N�Z�b�T�A�C�����C���֐�
	//�S�[��������
	bool IsGoal() const { return isGoal_; }
};