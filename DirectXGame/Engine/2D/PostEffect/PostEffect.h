#pragma once
#include "Sprite.h"
//�|�X�g�G�t�F�N�g
class PostEffect :  public Sprite
{
public:
    //�R���X�g���N�^
    PostEffect();


    //�`��
    void Draw(ID3D12GraphicsCommandList* cmdList);
};

