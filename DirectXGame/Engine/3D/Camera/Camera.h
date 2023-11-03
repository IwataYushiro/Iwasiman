#pragma once
#include <DirectXMath.h>
#include <wrl.h>

/*

*	Camera.h

*	�J����

*/
class Camera
{
private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMMATRIX = DirectX::XMMATRIX;
public:
	//�R���X�g���N�^
	Camera();
	//�f�X�g���N�^
	virtual ~Camera();
	//�����l�Ƀ��Z�b�g
	void Reset();
	//�V�F�C�N
	//���_�V�F�C�N(���_�A�V�F�C�N���ԁA�ŏ����A�ő啝)
	XMFLOAT3 ShakeEye(XMFLOAT3 eye, int count, XMFLOAT3 min, XMFLOAT3 max);
	//�����_�V�F�C�N(�����_�A�V�F�C�N���ԁA�ŏ����A�ő啝)
	XMFLOAT3 ShakeTarget(XMFLOAT3 target, int count, XMFLOAT3 min, XMFLOAT3 max);
	//�X�V
	virtual void Update();
	//�r���[�s��X�V
	void UpdateViewMatrix();
	//�v���W�F�N�V�����s��X�V
	void UpdateProjectionMatrix();
	//�J�����ړ�(�ړ��l)
	void CameraMoveVector(const XMFLOAT3& move);
	//�J�������_�ړ�(�ړ��l)
	void CameraMoveVectorEye(const XMFLOAT3& move);
	//�J���������_�ړ�(�ړ��l)
	void CameraMoveVectorTarget(const XMFLOAT3& move);
	//�f�o�b�O�J����(���_�ƒ����_���ꏏ�ɓ�������)
	void DebugCamera(bool eyeTargetMix = false);
protected:
	// �r���[�s��
	XMMATRIX matView_ = {};
	// �ˉe�s��
	XMMATRIX matProjection_ = {};
	//�r���v���s��
	XMMATRIX matViewProjection_ = {};
	
	// �r���{�[�h�s��
	XMMATRIX matBillboard_ = {};
	// Y�����r���{�[�h�s��
	XMMATRIX matBillboardY_ = {};
	// ���_���W
	const XMFLOAT3 presetEye_ = { -10.0f, 0.0f, -100.0f };//�v���Z�b�g
	XMFLOAT3 eye_ = presetEye_;
	// �����_���W
	const XMFLOAT3 presetTarget_ = { -10.0f,0.0f,0.0f };//�v���Z�b�g
	XMFLOAT3 target_ = presetTarget_;
	// ������x�N�g��
	const XMFLOAT3 presetUp_ = { 0.0f,1.0f,0.0f };//�v���Z�b�g
	XMFLOAT3 up_ = presetUp_;

public://�A�N�Z�b�T
	//�r���[�s��Q�b�g
	const XMMATRIX& GetMatView() { return matView_; }
	
	//�v���W�F�N�V�����s��Q�b�g
	const XMMATRIX& GetMatProjection() { return matProjection_; }
	//�r���v���s��Q�b�g
	const XMMATRIX& GetMatViewProjection() { return matViewProjection_; }

	
	//�r���{�[�h�s��Q�b�g
	const XMMATRIX& GetMatBillboard() { return matBillboard_; }
	//Y�����r���{�[�h�s��Q�b�g
	const XMMATRIX& GetMatBillboardY() { return matBillboardY_; }

	//���_�Q�b�g
	const XMFLOAT3& GetEye() { return eye_; }
	//���_�Z�b�g
	void SetEye(const XMFLOAT3& eye);

	//�����_�Q�b�g
	const XMFLOAT3& GetTarget() { return target_; }
	//�����_�Z�b�g
	void SetTarget(const XMFLOAT3& target);

	//������x�N�g���Q�b�g
	const XMFLOAT3& GetUp() { return up_; }
	//������x�N�g���Z�b�g
	void SetUp(const XMFLOAT3& up);

};
