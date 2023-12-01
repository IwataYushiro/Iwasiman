#pragma once
#include "SpriteCommon.h"
#include "Input.h"
#include <DirectXMath.h>

/*

*	Sprite.h

*	1���̃X�v���C�g

*/
class Sprite
{
public: // �����o�֐�
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://�\���̗�
	//�}�e���A��
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;
	};

	//���W
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D�ϊ��s��
	};

//���_�ԍ�
	enum VertexNumber
	{
		LB,		//����
		LT,		//����
		RB,		//�E��
		RT,		//�E��
		verticesCount,//�v�f��
	};

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT2 uv;		//uv���W
	};

	

protected://�\���̗ނɊ֌W���郁���o�ϐ�
	//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ComPtr<ID3D12Resource> constBuffMaterial_ = nullptr;
	//�}�b�s���O�p�̃|�C���^
	ConstBufferDataMaterial* constMapMaterial_ = nullptr;
	
	//�A���J�[�|�C���g
	const XMFLOAT2 defaultAnchorPoint_ = { 0.0f,0.0f };
	XMFLOAT2 anchorPoint_ = defaultAnchorPoint_;
	//�\���T�C�Y
	const XMFLOAT2 defaultSize_ = { 100.0f,100.0f };
	XMFLOAT2 size_ = defaultSize_;
	
	//��]
	const float defalutRotationZ_ = 0.0f;
	float rotationZ_ = defalutRotationZ_;
	//���W
	const XMFLOAT2 defalutPosition_ = { 0.0f,0.0f };
	XMFLOAT2 position_ = defalutPosition_;
	//�F(RGBA)
	const XMFLOAT4 defalutColor_ = { 1.0f,1.0f,1.0f,1.0f };
	XMFLOAT4 color_ = defalutColor_;

	//���E�t���b�v
	bool isFlipX_ = false;
	//�㉺�t���b�v
	bool isFlipY_ = false;

	//��\���t���O
	bool isInvisible_ = false;

	//�e�N�X�`��������W
	const XMFLOAT2 defalutTextureLeftTop_ = { 0.0f,0.0f };
	XMFLOAT2 textureLeftTop_ = defalutTextureLeftTop_;
	//�e�N�X�`���؂�o���T�C�Y
	const XMFLOAT2 defaultTextureSize_ = { 100.0f,100.0f };
	XMFLOAT2 textureSize_ = defaultTextureSize_;

	//�ˉe�s��
	XMMATRIX matProjection_;

	//���[���h�ϊ��s��
	XMMATRIX matRot_;
	XMMATRIX matTrans_;
	XMMATRIX matWorld_ = DirectX::XMMatrixIdentity();

	//���\�[�X�f�X�N�̐ݒ�
	struct ResDescPreset
	{
		const UINT height = 1;
		const UINT16 arraysize = 1;
		const UINT16 mipLevels = 1;
		const UINT sampleCount = 1;

	};
	ResDescPreset resDescPreset_;

	//�v���W�F�N�V�����ϊ��s��̐ݒ�
	struct ProjectionPreset
	{
		const float viewLeft = 0.0f;
		const float viewRight = (float)WinApp::WINDOW_WIDTH;
		const float viewBottom = (float)WinApp::WINDOW_HEIGHT;
		const float viewTop = 0.0f;
		const float nearZ = 0.0f;
		const float farZ = 1.0f;
	};
	ProjectionPreset projectionPreset_;
	//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ComPtr<ID3D12Resource> constBuffTransform_ = nullptr;
	//�}�b�s���O�p�̃|�C���^
	ConstBufferDataTransform* constMapTransform_ = nullptr;

public://�����o�֐�

	//������(�X�v���C�g��ՁA�e�N�X�`���C���f�b�N�X)
	void Initialize(SpriteCommon* spCommon, const uint32_t textureIndex = UINT32_MAX);
	//�X�V
	void Update();
	//�`��
	void Draw()const;

	//�萔�o�b�t�@
	//�}�e���A��
	void CreateConstBufferMaterial();
	//3D���W
	void CreateConstBufferTransform();

//private://�����o�ϐ�
protected:
	//���_�f�[�^�̃v���Z�b�g
	const Vertex presetVertices_[verticesCount] = {
		{{  0.0f,size_.y,0.0f},{0.0f,1.0f}},	//����
		{{  0.0f,  0.0f,0.0f},{0.0f,0.0f}},		//����
		{{size_.x,size_.y,0.0f},{1.0f,1.0f}},	//�E��
		{{size_.x,  0.0f,0.0f},{1.0f,0.0f}},	//�E��
	};
	//���_�f�[�^
	Vertex vertices_[verticesCount] = {
		presetVertices_[LB],		//����
		presetVertices_[LT],		//����
		presetVertices_[RB],		//�E��
		presetVertices_[RT]			//�E��
	};

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	Vertex* vertMap_ = nullptr;
	//�C���v�b�g
	Input* input_ = nullptr;
	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;

	//���_�o�b�t�@�̃��\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc_{};

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_{};

	//�e�N�X�`���ԍ�
	uint32_t textureIndex_ = 0;

public://�A�N�Z�b�T�u����
	//�A���J�[�|�C���g�Z�b�g
	void SetAnchorPoint(const XMFLOAT2& anchorPoint) { anchorPoint_ = anchorPoint; }
	//�A���J�[�|�C���g�Q�b�g
	const XMFLOAT2& GetAnchorPoint()const { return anchorPoint_; }

	//�X�P�[�����O�Z�b�g
	void SetSize(const XMFLOAT2& size) { size_ = size; }
	//�X�P�[�����O�Q�b�g
	const XMFLOAT2& GetSize()const { return size_; }

	//�|�W�V�����Z�b�g
	void SetPosition(const XMFLOAT2& position){ position_ = position; }
	//�|�W�V����X�Z�b�g
	void SetPositionX(const float position) { position_.x = position; }
	//�|�W�V����Y�Z�b�g
	void SetPositionY(const float position) { position_.y = position; }
	//�|�W�V�����Q�b�g
	const XMFLOAT2& GetPosition()const { return position_; }

	//���[�e�[�V�����Z�b�g(Z��)
	void SetRotationZ(const float& rotationZ) { rotationZ_ = rotationZ; }
	//���[�e�[�V�����Q�b�g(Z��)
	const float& GetRotationZ()const { return rotationZ_; }

	//�J���[�Z�b�g
	void SetColor(const XMFLOAT4& color) { color_ = color; }
	//�J���[�Q�b�g
	const XMFLOAT4& GetColor()const { return color_; }

	//���E�t���b�v�Z�b�g
	void SetFlipX(const bool& isFlipX) { isFlipX_ = isFlipX; }
	//���E�t���b�v�Q�b�g
	const bool& GetFlipX()const { return isFlipX_; }
	
	//�㉺�t���b�v�Z�b�g
	void SetFlipY(const bool& isFlipY) { isFlipY_ = isFlipY; }
	//�㉺�t���b�v�Q�b�g
	const bool& GetFlipY()const { return isFlipY_; }

	//���o���Z�b�g
	void SetInvisible(const bool& isInvisible) { isInvisible_ = isInvisible; }
	//���o���Q�b�g
	const bool& GetInvisible()const { return isInvisible_; }

	//�e�N�X�`���ԍ��Z�b�g
	void SetTextureIndex(const uint32_t& textureIndex) { textureIndex_ = textureIndex; }
	//�e�N�X�`���ԍ��Q�b�g
	const uint32_t& GetTextureIndex()const { return textureIndex_; }

	//�e�N�X�`��������W�Z�b�g
	void SetTextureLeftTop(const XMFLOAT2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }
	//�e�N�X�`��������W�Q�b�g
	const XMFLOAT2& GetTextureLeftTop()const { return textureLeftTop_; }

	//�e�N�X�`���؂�o���T�C�Y�Z�b�g
	void SetTextureSize(const XMFLOAT2& textureSize) { textureSize_ = textureSize; }
	//�e�N�X�`���؂�o���T�C�Y�Q�b�g
	const XMFLOAT2& GetTextureSize()const { return textureSize_; }

protected:
	//�e�N�X�`���T�C�Y���C���[�W�ɍ��킹��
	void AdjustTextureSize();

};
