#pragma once
#include "Sprite.h"

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	PostEffect.h

	*	�|�X�g�G�t�F�N�g

	*/
	class PostEffect
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
			XMFLOAT4 color; //�F(RGBA)
			float power;    //�|�X�g�G�t�F�N�g�̋���
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
		//��ʃN���A�J���[
		enum ClearColorRGBW
		{
			CCRGBW_R = 0,
			CCRGBW_G = 1,
			CCRGBW_B = 2,
			CCRGBW_W = 3,
			CCRGBW_Num = 4,
		};

		//���[�g�p�����[�^�C���f�b�N�X
		enum RootParameterIndex
		{
			RPI_ConstBuffMaterial = 0,
			RPI_TexBuff0 = 1,
			RPI_ConstBuffTransform = 2,
			RPI_TexBuff1 = 3,
			RPI_Num = 4,
		};
	private://�ÓI�����o�ϐ�
		//�x�[�X�f�B���N�g��
		static const std::string baseDirectory_;
		//���_�V�F�[�_�[
		static const std::string directoryVS_;
		//�s�N�Z���V�F�[�_�[
		static const std::string directoryPS_;

		static const float clearColor_[CCRGBW_Num];

	public:
		//�R���X�g���N�^
		PostEffect();
		//������(�g�p�|�X�g�G�t�F�N�g�p�X)
		void Initialize(const std::string& fileName = "None");
		//�X�V
		void Update();

		//�`��(�R�}���h���X�g)
		void Draw();
		//�`��O����(�R�}���h���X�g)
		void PreDraw(ID3D12GraphicsCommandList* cmdList);

		//�`��㏈��(�R�}���h���X�g)
		void PostDraw(ID3D12GraphicsCommandList* cmdList);

	private:
		//���_�f�[�^�̃v���Z�b�g
		const Vertex presetVerticesPost_[verticesCount] = {
			{{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},	//����
			{{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},	//����
			{{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},	//�E��
			{{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},	//�E��
		};
		//���_�f�[�^
		Vertex verticesPost_[verticesCount] = {
			presetVerticesPost_[LB],	//����
			presetVerticesPost_[LT],	//����
			presetVerticesPost_[RB],	//�E��
			presetVerticesPost_[RT],	//�E��
		};

		//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
		ComPtr<ID3D12Resource> constBuffMaterialPost_ = nullptr;
		//�}�b�s���O�p�̃|�C���^
		ConstBufferDataMaterial* constMapMaterialPost_ = nullptr;
		//�萔�o�b�t�@��GPU���\�[�X�̃|�C���^
		ComPtr<ID3D12Resource> constBuffTransformPost_ = nullptr;
		//�}�b�s���O�p�̃|�C���^
		ConstBufferDataTransform* constMapTransformPost_ = nullptr;

		//�X�v���C�g���
		const SpriteCommon* spCommon_ = nullptr;
		//���_�o�b�t�@
		ComPtr<ID3D12Resource> vertBuff_;
		//���_�o�b�t�@�r���[
		D3D12_VERTEX_BUFFER_VIEW vbView_{};
		//���\�[�X�f�X�N�̐ݒ�
		struct ResDescPreset
		{
			const UINT16 arraysize = 1;
			const UINT16 mipLevels = 0;
			const UINT sampleCount = 1;
			const UINT sampleQuality = 0;
		};
		ResDescPreset resDescPreset_;

		//�e�N�X�`���o�b�t�@
		enum TexBuffCount
		{
			TBC_TexBuff0 = 0,
			TBC_TexBuff1 = 1,
			TBC_Num = 2,
		};
		ComPtr<ID3D12Resource> texBuff_[TBC_Num];
		//SRV�p�f�X�N���v�^�q�[�v
		ComPtr<ID3D12DescriptorHeap> descHeapSRV_;
		//�[�x�o�b�t�@
		ComPtr<ID3D12Resource> depthBuff_;
		//RTV�p�f�X�N���v�^�q�[�v
		ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
		//DSV�p�f�X�N���v�^�q�[�v
		ComPtr<ID3D12DescriptorHeap> descHeapDSV_;
		//�O���t�B�b�N�X�p�C�v���C��
		ComPtr<ID3D12PipelineState> pipelineState_;
		//���[�g�V�O�l�`��
		ComPtr<ID3D12RootSignature> rootSignature_;

		//�F(RGBA)
		const XMFLOAT4 defaultColor_ = { 1.0f,1.0f,1.0f,1.0f };
		XMFLOAT4 color_ = defaultColor_;
		//�|�X�g�G�t�F�N�g�̋���(�O���[�X�P�[���̔Z���Ƃ��@1.0f�ōő�)
		const float defaultPower_ = 1.0f;
		float power_ = defaultPower_;

	public://�A�N�Z�b�T�u����
		//�J���[�Z�b�g
		void SetColor(const XMFLOAT4& color) { color_ = color; }
		//�J���[�Q�b�g
		const XMFLOAT4& GetColor()const { return color_; }
		//�p���[�Z�b�g
		void SetPower(const float power) { power_ = power; }
		//�p���[�Q�b�g
		const float GetPower()const { return power_; }

	private://�J�v�Z���������o�֐�
		//���_�o�b�t�@����
		void CreateVertexBuffer();
		//�萔�o�b�t�@����
		void CreateConstBuffer();
		//�}�e���A��
		void CreateConstBufferMaterialPost();
		//���W
		void CreateConstBufferTransformPost();
		//�e�N�X�`������
		void CreateTexture();
		// SRV����
		void CreateSRV();
		// RTV����
		void CreateRTV();
		// �[�x�o�b�t�@����
		void CreateDepthBuffer();
		// DSV����
		void CreateDSV();
		//�p�C�v���C������(�|�X�g�G�t�F�N�g�t�@�C���p�X)
		void CreateGraphicsPipelineState(const std::string& fileName);
	};
}