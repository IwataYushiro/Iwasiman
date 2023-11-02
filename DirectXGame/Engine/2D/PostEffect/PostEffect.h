#pragma once
#include "Sprite.h"

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
private://�ÓI�����o�ϐ�
    //�x�[�X�f�B���N�g��
    static const std::string baseDirectory_;
    //���_�V�F�[�_�[
    static const std::string directoryVS_;
    //�s�N�Z���V�F�[�_�[
    static const std::string directoryPS_;
    //��ʃN���A�J���[
    static const float clearcolor_[4];

public:
    //�R���X�g���N�^
    PostEffect();
    //������(�X�v���C�g��ՁA�g�p�|�X�g�G�t�F�N�g�p�X)
    void Initialize(SpriteCommon* spCommon, const std::string& fileName="None");
    //�X�V
    void Update();
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

    //�`��(�R�}���h���X�g)
    void Draw(ID3D12GraphicsCommandList* cmdList);
    //�`��O����(�R�}���h���X�g)
    void PreDraw(ID3D12GraphicsCommandList* cmdList);

    //�`��㏈��(�R�}���h���X�g)
    void PostDraw(ID3D12GraphicsCommandList* cmdList);

private:
    //���_�f�[�^
    Vertex verticesPost_[verticesCount] = {
        {{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},	//����
        {{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},	//����
        {{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},	//�E��
        {{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},	//�E��
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
    SpriteCommon* spCommon_ = nullptr;
    //���_�o�b�t�@
    ComPtr<ID3D12Resource> vertBuff_;
    //���_�o�b�t�@�r���[
    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> texBuff_[2];
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
    XMFLOAT4 color_ = { 1.0f,1.0f,1.0f,1.0f };
public:
    //�J���[�Z�b�g
    void SetColor(const XMFLOAT4& color) { color_ = color; }
    //�J���[�Q�b�g
    const XMFLOAT4& GetColor()const { return color_; }
};
