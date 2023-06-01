#include "PostEffect.h"
#include <d3dx12.h>
#include "WinApp.h"

using namespace DirectX;

PostEffect::PostEffect()
{
}

void PostEffect::Initialize()
{
	HRESULT result;
	//��ՃN���X�Ƃ��Ă̏�����
	Sprite::Initialize(spCommon_);
	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
		WinApp::window_width,
		(UINT)WinApp::window_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
	
	//�e�N�X�`���o�b�t�@����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));

	//�e�N�X�`���ԃN���A
	//��f��(1280 x 720 = 921600�s�N�Z��)
	const UINT pixelCount = WinApp::window_width * WinApp::window_height;
	//�摜�ꖇ���̃f�[�^�T�C�Y
	const UINT rowPitch = sizeof(UINT) * WinApp::window_width;
	//�摜�S�̂̃f�[�^�T�C�Y
	const UINT depthPitch = rowPitch * WinApp::window_height;
	//�摜�C���[�W
	UINT* img = new UINT[pixelCount];
	for (int i = 0; i < pixelCount; i++) { img[i] = 0xff0000ff; }

	//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
	result = texBuff->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
	assert(SUCCEEDED(result));
	delete[] img;
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
