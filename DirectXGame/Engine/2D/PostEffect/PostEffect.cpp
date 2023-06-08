#include "PostEffect.h"
#include <d3dx12.h>
#include "WinApp.h"

using namespace DirectX;
//�ÓI�����o�ϐ��̎���
const float PostEffect::clearcolor[4] = { 0.25f,0.5f,0.1f,0.0f };

PostEffect::PostEffect()
{
}

void PostEffect::Initialize(SpriteCommon* spCommon, uint32_t textureIndex)
{
	assert(spCommon);
	this->spCommon_ = spCommon;

	//��ՃN���X�Ƃ��Ă̏�����
	Sprite::Initialize(spCommon_, textureIndex);
	//�e�N�X�`������
	CreateTexture();
	// SRV����
	CreateSRV();
	// RTV����
	CreateRTV();
	// �[�x�o�b�t�@����
	CreateDepthBuffer();
	// DSV����
	CreateDSV();
}

void PostEffect::CreateTexture()
{
	HRESULT result;
	//�e�N�X�`�����\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::window_width,
		(UINT)WinApp::window_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	//�e�N�X�`���o�b�t�@����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0),
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearcolor),
		IID_PPV_ARGS(&texBuff));
	assert(SUCCEEDED(result));
	{

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
}

void PostEffect::CreateSRV()
{
	HRESULT result;
	//SRV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = 1;
	//SRV�p�f�X�N���v�^�q�[�v����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));

	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	//�f�X�N���v�^�q�[�v��SRV�쐬
	spCommon_->GetDxCommon()->GetDevice()->CreateShaderResourceView(
		texBuff.Get()/*�r���[�Ɗ֘A�t����o�b�t�@*/, &srvDesc,
		descHeapSRV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::CreateRTV()
{
	HRESULT result;
	//RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = 1;
	//RTV�p�f�X�N���v�^�q�[�v����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV));
	assert(SUCCEEDED(result));

	//�����_�^�[�Q�b�g�r���[�ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	//�f�X�N���v�^�q�[�v��RTV�쐬
	spCommon_->GetDxCommon()->GetDevice()->CreateRenderTargetView(
	texBuff.Get(), &rtvDesc, descHeapRTV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::CreateDepthBuffer()
{
	HRESULT result;

	//�[�x�o�b�t�@���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::window_width,
		WinApp::window_height,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	//�[�x�o�b�t�@����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, &depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0),
		IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));
}

void PostEffect::CreateDSV()
{
	HRESULT result;

	//DSV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescHeapDesc.NumDescriptors = 1;
	//DSV�p�f�X�N���v�^�q�[�v����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&dsvDescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v��DSV�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	spCommon_->GetDxCommon()->GetDevice()->CreateDepthStencilView(
	depthBuff.Get(), &dsvDesc, descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//��\��
	if (GetInvisible())
	{
		return;
	}
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�
	spCommon_->GetDxCommon()->GetCommandList()->SetPipelineState(spCommon_->GetPipelineState());
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootSignature(spCommon_->GetRootSignature());
	//�v���~�e�B�u�`��̐ݒ�R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	spCommon_->GetDxCommon()->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�e�N�X�`���R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);

	//�萔�o�b�t�@�r���[(CBV�̐ݒ�R�}���h)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(2, constBuffTransform->GetGPUVirtualAddress());
	//�`��R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);

}

void PostEffect::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	//���\�[�X�o���A�ύX(�V�F�[�_�[���\�[�X���`��\)
	cmdList->ResourceBarrier(1,
	&CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
	D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
	D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	//RTV�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		descHeapRTV->GetCPUDescriptorHandleForHeapStart();
	//DSV�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	//�����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);
	//�r���[�|�[�g�ݒ�
	cmdList->RSSetViewports(1, &CD3DX12_VIEWPORT(0.0f, 0.0f,
		WinApp::window_width, WinApp::window_height));
	//�V�U�[�ݒ�
	cmdList->RSSetScissorRects(1, &CD3DX12_RECT(0, 0,
		WinApp::window_width, WinApp::window_height));
	//�S��ʃN���A
	cmdList->ClearRenderTargetView(rtvH, clearcolor, 0, nullptr);
	//�[�x�o�b�t�@�N���A
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void PostEffect::PostDraw(ID3D12GraphicsCommandList* cmdList)
{
	//���\�[�X�o���A�ύX(�`��\���V�F�[�_�[���\�[�X)
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(texBuff.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

}
