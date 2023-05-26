#include "PostEffect.h"
#include <d3dx12.h>

using namespace DirectX;

PostEffect::PostEffect()
{
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//��\��
	if (GetInvisible())
	{
		return;
	}
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�
	spCommon_->PreDraw();
	//�e�N�X�`���R�}���h
	spCommon_->SetTextureCommands(textureIndex_);
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@�r���[(CBV�̐ݒ�R�}���h)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	//�`��R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);

}
