#pragma once
#include "SpriteCommon.h"
#include "Input.h"
#include <DirectXMath.h>

//1��1���̃X�v���C�g�N���X

class Sprite
{
public: // �����o�֐�
	//�G�C���A�X�e���v���[�g�Ƃ��ŐF�X�ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

private://�\���̗�
	//�}�e���A��
	struct ConstBufferDataMaterial
	{
		XMFLOAT4 color;
	};

	//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	//�}�b�s���O�p�̃|�C���^
	ConstBufferDataMaterial* constMapMaterial = nullptr;

	//�}�e���A��
	struct ConstBufferDataTransform
	{
		XMMATRIX mat;	//3D�ϊ��s��
	};

	//�A���J�[�|�C���g
	XMFLOAT2 anchorPoint_ = { 0.0f,0.0f };
	//�\���T�C�Y
	XMFLOAT2 size_ = { 100.0f,100.0f };
	
	//���_�ԍ�
	enum VertexNumber
	{
		LB,		//����
		LT,		//����
		RB,		//�E��
		RT,		//�E��
		verticesCount,//�v�f��
	};

	//��]
	float rotationZ_ = 0.0f;
	//���W
	XMFLOAT2 position_ = { 0.0f,0.0f };
	//�F(RGBA)
	XMFLOAT4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	//���E�t���b�v
	bool isFlipX_ = false;
	//�㉺�t���b�v
	bool isFlipY_ = false;

	//��\���t���O
	bool isInvisible_ = false;

	//�e�N�X�`��������W
	XMFLOAT2 textureLeftTop_ = { 0.0f,0.0f };
	//�e�N�X�`���؂�o���T�C�Y
	XMFLOAT2 textureSize_ = { 100.0f,100.0f };

	//�ˉe�s��
	XMMATRIX matProjection;

	//���[���h�ϊ��s��
	XMMATRIX matRot;
	XMMATRIX matTrans;
	XMMATRIX matWorld = DirectX::XMMatrixIdentity();


	//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
	ComPtr<ID3D12Resource> constBuffTransform = nullptr;
	//�}�b�s���O�p�̃|�C���^
	ConstBufferDataTransform* constMapTransform = nullptr;

	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;		//xyz���W
		XMFLOAT2 uv;		//uv���W
	};


public://�����o�֐�

	//������
	void Initialize(SpriteCommon* spCommon, uint32_t textureIndex = UINT32_MAX);
	//�X�V
	void Update();
	//�`��
	void Draw();

	//�萔�o�b�t�@
	//�}�e���A��
	void CreateConstBufferMaterial();
	//3D���W
	void CreateConstBufferTransform();

private://�����o�ϐ�
	
	//���_�f�[�^
	Vertex vertices[verticesCount] = {
		{{  0.0f,size_.y,0.0f},{0.0f,1.0f}},	//����
		{{  0.0f,  0.0f,0.0f},{0.0f,0.0f}},		//����
		{{size_.x,size_.y,0.0f},{1.0f,1.0f}},	//�E��
		{{size_.x,  0.0f,0.0f},{1.0f,0.0f}},	//�E��
	};

	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	Vertex* vertMap = nullptr;
	//�C���v�b�g
	Input* input_ = nullptr;
	//�X�v���C�g���
	SpriteCommon* spCommon_ = nullptr;

	//���_�o�b�t�@�̃��\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};

	//���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	//���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//�e�N�X�`���ԍ�
	uint32_t textureIndex_ = 0;

public://�A�N�Z�b�T�u����
	//�A���J�[�|�C���g
	void SetAnchorPoint(const XMFLOAT2& anchorPoint) { anchorPoint_ = anchorPoint; }

	const XMFLOAT2& GetAnchorPoint()const { return anchorPoint_; }

	//�X�P�[�����O
	void SetSize(const XMFLOAT2& size) { size_ = size; }

	const XMFLOAT2& GetSize()const { return size_; }

	//���W
	void SetPosition(const XMFLOAT2& position) { position_ = position; }

	const XMFLOAT2& GetPosition()const { return position_; }

	//��]
	void SetRotationZ(const float& rotationZ) { rotationZ_ = rotationZ; }

	const float& GetRotationZ()const { return rotationZ_; }

	//�F
	void SetColor(const XMFLOAT4& color) { color_ = color; }

	const XMFLOAT4& GetColor()const { return color_; }

	//���E�t���b�v
	void SetFlipX(const bool& isFlipX) { isFlipX_ = isFlipX; }

	const bool& GetFlipX()const { return isFlipX_; }
	
	//�㉺�t���b�v
	void SetFlipY(const bool& isFlipY) { isFlipY_ = isFlipY; }

	const bool& GetFlipY()const { return isFlipY_; }

	//���o��
	void SetInvisible(const bool& isInvisible) { isInvisible_ = isInvisible; }

	const bool& GetInvisible()const { return isInvisible_; }

	//�e�N�X�`���ԍ��擾
	void SetTextureIndex(const uint32_t& textureIndex) { textureIndex_ = textureIndex; }

	const uint32_t& GetTextureIndex()const { return textureIndex_; }

	//�e�N�X�`��������W
	void SetTextureLeftTop(const XMFLOAT2& textureLeftTop) { textureLeftTop_ = textureLeftTop; }

	const XMFLOAT2& GetTextureLeftTop()const { return textureLeftTop_; }

	//�e�N�X�`���؂�o���T�C�Y
	void SetTextureSize(const XMFLOAT2& textureSize) { textureSize_ = textureSize; }

	const XMFLOAT2& GetTextureSize()const { return textureSize_; }

private:
	//�e�N�X�`���T�C�Y���C���[�W�ɍ��킹��
	void AdjustTextureSize();

};
