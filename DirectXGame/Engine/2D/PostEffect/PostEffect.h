#pragma once
#include "Sprite.h"
//�|�X�g�G�t�F�N�g
class PostEffect :  public Sprite
{
public:
    //�R���X�g���N�^
    PostEffect();
    //������
    void Initialize(SpriteCommon* spCommon, uint32_t textureIndex = UINT32_MAX);
    //�`��
    void Draw(ID3D12GraphicsCommandList* cmdList);

private:
    //�e�N�X�`���o�b�t�@
    ComPtr<ID3D12Resource> texBuff;
    //SRV�p�f�X�N���v�^�q�[�v
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;
};

