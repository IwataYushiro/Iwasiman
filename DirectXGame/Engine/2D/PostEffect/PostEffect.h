#pragma once
#include "Sprite.h"
//�|�X�g�G�t�F�N�g
class PostEffect :  public Sprite
{
public:
    //�R���X�g���N�^
    PostEffect();
    //������
    void Initialize(SpriteCommon* spCommon);
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
    
    //�`��
    void Draw(ID3D12GraphicsCommandList* cmdList);
    //�`��O����
    void PreDraw(ID3D12GraphicsCommandList* cmdList);

    //�`��㏈��
    void PostDraw(ID3D12GraphicsCommandList* cmdList);

private:
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> texBuff;
    //SRV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //�[�x�o�b�t�@
    ComPtr<ID3D12Resource> depthBuff;
    //RTV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;
    
    //��ʃN���A�J���[
    static const float clearcolor[4];

};

