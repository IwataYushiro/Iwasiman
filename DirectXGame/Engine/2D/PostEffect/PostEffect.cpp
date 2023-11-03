#include "PostEffect.h"
#include <d3dx12.h>
#include "WinApp.h"
#include <d3dcompiler.h>
#include "Input.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

/*

*	PostEffect.cpp

*	�|�X�g�G�t�F�N�g

*/

//�ÓI�����o�ϐ��̎���
const float PostEffect::clearColor_[PostEffect::ClearColorRGBW::CCRGBW_Num] = { 0.25f,0.5f,0.1f,0.0f };

const std::string PostEffect::baseDirectory_ = "Resources/shader/PostEffect/";
const std::string PostEffect::directoryVS_ = "VS.hlsl";
const std::string PostEffect::directoryPS_ = "PS.hlsl";

PostEffect::PostEffect()
{
}

void PostEffect::Initialize(SpriteCommon* spCommon, const std::string& fileName)
{
	assert(spCommon);
	this->spCommon_ = spCommon;

	//���_�o�b�t�@
	CreateVertexBuffer();
	//�萔�o�b�t�@
	CreateConstBuffer();
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
	//�O���t�B�b�N�X�p�C�v���C��
	CreateGraphicsPipelineState(fileName);
}

void PostEffect::Update()
{
}

void PostEffect::CreateVertexBuffer()
{
	HRESULT result;
	
	//�T�C�Y
	//UINT sizeVB = static_cast<UINT>(sizeof(verticesPost[0]) * _countof(verticesPost));

	//���_�o�b�t�@
	D3D12_HEAP_PROPERTIES heapProp{};	//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;	//GPU�]���p

	const CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer(sizeof(Vertex) * verticesCount);
	//���\�[�X�ݒ�
	//���_�o�b�t�@����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&buffer,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));

	assert(SUCCEEDED(result));

	//�f�[�^�]��
	Vertex* vertMapPost = nullptr;
	//GPU��̃o�b�t�@�ɑΉ��������z���������擾
	result = vertBuff_->Map(0, nullptr, (void**)&vertMapPost);
	assert(SUCCEEDED(result));

	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(verticesPost_); i++)
	{
		vertMapPost[i] = verticesPost_[i];	//���W���R�s�[
	}
	//�q���������
	vertBuff_->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[
	//GPU���z�A�h���X
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView_.SizeInBytes = sizeof(Vertex) * verticesCount;
	//���_����̃T�C�Y
	vbView_.StrideInBytes = sizeof(Vertex);
}

void PostEffect::CreateConstBuffer()
{
	//�萔�o�b�t�@
	//�}�e���A��
	CreateConstBufferMaterialPost();
	constMapMaterialPost_->color = this->color_;
	

	//�ϊ��s��
	CreateConstBufferTransformPost();
	constMapTransformPost_->mat = XMMatrixIdentity();
	
	constBuffMaterialPost_->Unmap(0, nullptr);
	constBuffTransformPost_->Unmap(0, nullptr);
}

void PostEffect::CreateConstBufferMaterialPost()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	
	const CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff);
	//�萔�o�b�t�@�̐���
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&buffer,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterialPost_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffMaterialPost_->Map(0, nullptr, (void**)&constMapMaterialPost_);//�}�b�s���O
	assert(SUCCEEDED(result));
}

void PostEffect::CreateConstBufferTransformPost()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p

	const CD3DX12_RESOURCE_DESC buffer = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataTransform) + 0xff) & ~0xff);
	//���\�[�X�ݒ�
	//�萔�o�b�t�@�̐���
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&buffer, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffTransformPost_));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = constBuffTransformPost_->Map(0, nullptr, (void**)&constMapTransformPost_);//�}�b�s���O
	assert(SUCCEEDED(result));
}

void PostEffect::CreateTexture()
{
	HRESULT result;
	//�e�N�X�`�����\�[�X�ݒ�
	const CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::WINDOW_WIDTH,
		(UINT)WinApp::WINDOW_HEIGHT,
		resDescPreset_.arraysize, resDescPreset_.mipLevels, resDescPreset_.sampleCount,
		resDescPreset_.sampleQuality, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	CD3DX12_HEAP_PROPERTIES CHP = CD3DX12_HEAP_PROPERTIES(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);
	CD3DX12_CLEAR_VALUE CCV = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);

	//�e�N�X�`���o�b�t�@����
	for (int i = 0; i < TBC_Num; i++)
	{
		result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
			&CHP,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&CCV,
			IID_PPV_ARGS(&texBuff_[i]));
		assert(SUCCEEDED(result));
		{
			//�e�N�X�`���ԃN���A
			//��f��(1280 x 720 = 921600�s�N�Z��)
			const UINT pixelCount = WinApp::WINDOW_WIDTH * WinApp::WINDOW_HEIGHT;
			//�摜�ꖇ���̃f�[�^�T�C�Y
			const UINT rowPitch = sizeof(UINT) * WinApp::WINDOW_WIDTH;
			//�摜�S�̂̃f�[�^�T�C�Y
			const UINT depthPitch = rowPitch * WinApp::WINDOW_HEIGHT;
			//�摜�C���[�W
			UINT* img = new UINT[pixelCount];
			const UINT imgColor = 0xff0000ff;
			for (int j = 0; j < pixelCount; j++) { img[j] = imgColor; }

			//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
			result = texBuff_[i]->WriteToSubresource(0, nullptr, img, rowPitch, depthPitch);
			assert(SUCCEEDED(result));
			delete[] img;
		}
	}
}

void PostEffect::CreateSRV()
{
	HRESULT result;
	//SRV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	const UINT descriptorNum = 2;
	srvDescHeapDesc.NumDescriptors = descriptorNum;
	//SRV�p�f�X�N���v�^�q�[�v����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV_));
	assert(SUCCEEDED(result));

	//SRV�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	const UINT mipLevelNum = 1;
	srvDesc.Texture2D.MipLevels = mipLevelNum;
	for (int i = 0; i < descriptorNum; i++)
	{
		//�f�X�N���v�^�q�[�v��SRV�쐬
		spCommon_->GetDxCommon()->GetDevice()->CreateShaderResourceView(
		texBuff_[i].Get()/*�r���[�Ɗ֘A�t����o�b�t�@*/, &srvDesc,
		CD3DX12_CPU_DESCRIPTOR_HANDLE(
		descHeapSRV_->GetCPUDescriptorHandleForHeapStart(),i,
		spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	}
}

void PostEffect::CreateRTV()
{
	HRESULT result;
	//RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	const UINT descriptorNum = 2;
	rtvDescHeapDesc.NumDescriptors = descriptorNum;
	//RTV�p�f�X�N���v�^�q�[�v����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV_));
	assert(SUCCEEDED(result));

	//�����_�^�[�Q�b�g�r���[�ݒ�
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0; i < descriptorNum; i++)
	{
		//�f�X�N���v�^�q�[�v��RTV�쐬
		spCommon_->GetDxCommon()->GetDevice()->CreateRenderTargetView(
			texBuff_[i].Get(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(
				descHeapRTV_->GetCPUDescriptorHandleForHeapStart(),i,
				spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)));
	}
}

void PostEffect::CreateDepthBuffer()
{
	HRESULT result;

	//�[�x�o�b�t�@���\�[�X�ݒ�
	const CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		WinApp::WINDOW_WIDTH,
		WinApp::WINDOW_HEIGHT,
		resDescPreset_.arraysize, resDescPreset_.mipLevels, resDescPreset_.sampleCount,
		resDescPreset_.sampleQuality,D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		

	const CD3DX12_HEAP_PROPERTIES CHP = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	const CD3DX12_CLEAR_VALUE CCV = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	//�[�x�o�b�t�@����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateCommittedResource(
		&CHP,
		D3D12_HEAP_FLAG_NONE, &depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&CCV,
		IID_PPV_ARGS(&depthBuff_));
	assert(SUCCEEDED(result));
}

void PostEffect::CreateDSV()
{
	HRESULT result;

	//DSV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescHeapDesc{};
	dsvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	const UINT descriptorNum = 1;
	dsvDescHeapDesc.NumDescriptors = descriptorNum;
	//DSV�p�f�X�N���v�^�q�[�v����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateDescriptorHeap(
		&dsvDescHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));

	//�f�X�N���v�^�q�[�v��DSV�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;	//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	spCommon_->GetDxCommon()->GetDevice()->CreateDepthStencilView(
	depthBuff_.Get(), &dsvDesc, descHeapDSV_->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::CreateGraphicsPipelineState(const std::string& fileName)
{
	HRESULT result;
	//���_�V�F�[�_�[
	//�f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPathV = baseDirectory_ + fileName + "/" + fileName + directoryVS_;
	//Resources/shader/PostEffect/ Test / Test VS.hlsl

	const int cbMultiByte = -1;
	//���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSizeV = MultiByteToWideChar(CP_ACP, 0, fullPathV.c_str(), cbMultiByte, nullptr, 0);

	//���C�h������ɕϊ�
	std::vector<wchar_t> wfilePathV(filePathBufferSizeV);
	MultiByteToWideChar(CP_ACP, 0, fullPathV.c_str(), cbMultiByte, wfilePathV.data(), filePathBufferSizeV);

	//�s�N�Z���V�F�[�_�[
	//�f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPathP = baseDirectory_ + fileName + "/" + fileName + directoryPS_;

	//���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSizeP = MultiByteToWideChar(CP_ACP, 0, fullPathP.c_str(), cbMultiByte, nullptr, 0);

	//���C�h������ɕϊ�
	std::vector<wchar_t> wfilePathP(filePathBufferSizeP);
	MultiByteToWideChar(CP_ACP, 0, fullPathP.c_str(), cbMultiByte, wfilePathP.data(), filePathBufferSizeP);


	//�V�F�[�_�I�u�W�F�N�g
	ComPtr<ID3DBlob> vsBlob = nullptr;		//���_�V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> psBlob = nullptr;		//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ComPtr<ID3DBlob> errorBlob = nullptr;	//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�ǂݍ���
	result = D3DCompileFromFile(
		wfilePathV.data(),					//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�o����悤��
		"main", "vs_5_0",									//�G���g���[�|�C���g���A�V�F�[�_���f��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�N�p�ݒ�
		0,
		&vsBlob, &errorBlob);
	//�G���[����
	if (FAILED(result))
	{
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//�s�N�Z���V�F�[�_�ǂݍ���
	result = D3DCompileFromFile(
		wfilePathP.data(),					//�V�F�[�_�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,					//�C���N���[�h�o����悤��
		"main", "ps_5_0",									//�G���g���[�|�C���g���A�V�F�[�_���f��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,	//�f�o�b�N�p�ݒ�
		0,
		&psBlob, &errorBlob);

	//�G���[����
	if (FAILED(result))
	{
		//errorBlob����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�E�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}

	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
		"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//�O���t�B�b�N�X�p�C�v���C��
	//�ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};

	//�V�F�[�_�ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	//�T���v���}�X�N
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;	//�W���ݒ�

	//���X�^���C�U
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	//�J�����O���Ȃ�
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;	//�h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;	//�[�x�N���b�s���O��L����
	//�f�v�X�X�e���V���X�e�[�g
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//��ɏ㏑��
	//�u�����h�X�e�[�g
	const int defaultRenderTargetNum = 0;
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[defaultRenderTargetNum];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	//RGBA�S�Ẵ`�����l����`��

	//�u�����h���ʐݒ�(����{�����œ���)
	blenddesc.BlendEnable = false;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�f�X�g�̒l��0���g��
	//����������(�A���t�@�u�����f�B���O�@�f�t�H���g)
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l
	/*
	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;					//���Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;					//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_ONE;					//�f�X�g�̃A���t�@�l

	//���Z����
	blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;		//���Z
	blenddesc.SrcBlend = D3D12_BLEND_ONE;					//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_ONE;					//�f�X�g�̃A���t�@�l

	//�F���]
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;					//���Z
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;		//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_ZERO;					//�f�X�g�̃A���t�@�l

	*/
	//�[�x�o�b�t�@�̃t�H�[�}�b�g
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//���_���C�A�E�g
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	//�v���~�e�B�u�`��
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//���̑�
	const UINT renderTargetNum = 1;
	const UINT sampleDescCount = 1;

	pipelineDesc.NumRenderTargets = renderTargetNum;								//�`��Ώۂ�1��
	pipelineDesc.RTVFormats[defaultRenderTargetNum] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0�`255�w���RGBA
	pipelineDesc.SampleDesc.Count = sampleDescCount;								//1�s�N�Z���ɂ�1��T���v�����O

	//�f�X�N���v�^�����W�̐ݒ�
	const UINT descriptorNum = 1;
	enum DescriptoeRangeNum
	{
		DRN_SRV0 = 0,
		DRN_SRV1=1,
		DRN_Num=2,
	};

	D3D12_DESCRIPTOR_RANGE descriptorRange[DRN_Num]{};
	descriptorRange[DRN_SRV0].NumDescriptors = descriptorNum;								//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange[DRN_SRV0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[DRN_SRV0].BaseShaderRegister = DRN_SRV0;							//�e�N�X�`�����W�X�^0��
	descriptorRange[DRN_SRV0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	descriptorRange[DRN_SRV1].NumDescriptors = descriptorNum;								//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange[DRN_SRV1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[DRN_SRV1].BaseShaderRegister = DRN_SRV1;							//�e�N�X�`�����W�X�^1��
	descriptorRange[DRN_SRV1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//���[�g�p�����[�^�ݒ�
	const UINT CBDM_Register = 0;//�}�e���A���萔�o�b�t�@�̃��W�X�^
	const UINT CBDT_Register = 1;//���W�萔�o�b�t�@�̃��W�X�^

	D3D12_ROOT_PARAMETER rootParams[RPI_Num] = {};
	//�萔�o�b�t�@0��
	rootParams[RPI_ConstBuffMaterial].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//�萔�o�b�t�@�r���[(���)
	rootParams[RPI_ConstBuffMaterial].Descriptor.ShaderRegister = CBDM_Register;						//�萔�o�b�t�@�ԍ�
	rootParams[RPI_ConstBuffMaterial].Descriptor.RegisterSpace = 0;							//�f�t�H���g�l
	rootParams[RPI_ConstBuffMaterial].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[RPI_TexBuff0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[RPI_TexBuff0].DescriptorTable.pDescriptorRanges = &descriptorRange[DRN_SRV0];		//�f�X�N���v�^�����W
	rootParams[RPI_TexBuff0].DescriptorTable.NumDescriptorRanges = descriptorNum;						//�f�X�N���v�^�����W��
	rootParams[RPI_TexBuff0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@1��
	rootParams[RPI_ConstBuffTransform].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//�萔�o�b�t�@�r���[(���)
	rootParams[RPI_ConstBuffTransform].Descriptor.ShaderRegister = CBDT_Register;						//�萔�o�b�t�@�ԍ�
	rootParams[RPI_ConstBuffTransform].Descriptor.RegisterSpace = 0;							//�f�t�H���g�l
	rootParams[RPI_ConstBuffTransform].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^1��
	rootParams[RPI_TexBuff1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;			//���
	rootParams[RPI_TexBuff1].DescriptorTable.pDescriptorRanges = &descriptorRange[DRN_SRV1];				//�f�X�N���v�^�����W
	rootParams[RPI_TexBuff1].DescriptorTable.NumDescriptorRanges = descriptorNum;					//�f�X�N���v�^�����W��
	rootParams[RPI_TexBuff1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	
	//�e�N�X�`���T���v���[�̐ݒ�
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;					//���J��Ԃ�(�^�C�����O)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;					//�c�J��Ԃ�(�^�C�����O)
	//���[�g�V�O�l�`��
	//�ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;		//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);			//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	const UINT staticSamplersNum = 1;
	rootSignatureDesc.NumStaticSamplers = staticSamplersNum;

	// ���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = spCommon_->GetDxCommon()->GetDevice()->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature_.Get();

	//�p�C�v���C���X�e�[�g����
	result = spCommon_->GetDxCommon()->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));

}

void PostEffect::Draw([[maybe_unused]] ID3D12GraphicsCommandList* cmdList)
{
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�
	spCommon_->GetDxCommon()->GetCommandList()->SetPipelineState(pipelineState_.Get());
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
	//�v���~�e�B�u�`��̐ݒ�R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	
	//�f�X�N���v�^�q�[�v�̔z����Z�b�g����R�}���h
	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV_.Get() };
	spCommon_->GetDxCommon()->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//�e�N�X�`���R�}���h
	spCommon_->GetDxCommon()->GetCommandList()->
		SetGraphicsRootDescriptorTable(RPI_TexBuff0, CD3DX12_GPU_DESCRIPTOR_HANDLE(
		descHeapSRV_->GetGPUDescriptorHandleForHeapStart(),TBC_TexBuff0,
			spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	spCommon_->GetDxCommon()->GetCommandList()->
		SetGraphicsRootDescriptorTable(RPI_TexBuff1, CD3DX12_GPU_DESCRIPTOR_HANDLE(
			descHeapSRV_->GetGPUDescriptorHandleForHeapStart(), TBC_TexBuff1,
			spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	const UINT viewsNum = 1;
	spCommon_->GetDxCommon()->GetCommandList()->IASetVertexBuffers(0, viewsNum, &vbView_);

	//�萔�o�b�t�@�r���[(CBV�̐ݒ�R�}���h)
	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(RPI_ConstBuffMaterial, constBuffMaterialPost_->GetGPUVirtualAddress());

	spCommon_->GetDxCommon()->GetCommandList()->SetGraphicsRootConstantBufferView(RPI_ConstBuffTransform, constBuffTransformPost_->GetGPUVirtualAddress());
	//�`��R�}���h
	const UINT instanceCount = 1;
	spCommon_->GetDxCommon()->GetCommandList()->DrawInstanced(_countof(verticesPost_), instanceCount, 0, 0);

}

void PostEffect::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int CRBNum = 2;
	for (int i = 0; i < CRBNum; i++)
	{
		CD3DX12_RESOURCE_BARRIER CRB = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(),
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		//���\�[�X�o���A�ύX(�V�F�[�_�[���\�[�X���`��\)
		const UINT barrierNum = 1;
		cmdList->ResourceBarrier(barrierNum,&CRB);
	}
	//RTV�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	const int RTVNum = 2;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHs[RTVNum];
	for (int i = 0; i < RTVNum; i++)
	{
		rtvHs[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			descHeapRTV_->GetCPUDescriptorHandleForHeapStart(), i,
			spCommon_->GetDxCommon()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}
	//DSV�p�f�X�N���v�^�q�[�v�̃n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		descHeapDSV_->GetCPUDescriptorHandleForHeapStart();
	//�����_�[�^�[�Q�b�g���Z�b�g
	cmdList->OMSetRenderTargets(RTVNum, rtvHs, false, &dsvH);
	
	CD3DX12_VIEWPORT viewports[RTVNum];
	CD3DX12_RECT scissorRects[RTVNum];
	for (int i = 0; i < RTVNum; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f,WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
		scissorRects[i] = CD3DX12_RECT(0, 0, WinApp::WINDOW_WIDTH, WinApp::WINDOW_HEIGHT);
	}
	//�r���[�|�[�g�ݒ�
	cmdList->RSSetViewports(RTVNum, viewports);
	//�V�U�[�ݒ�
	cmdList->RSSetScissorRects(RTVNum, scissorRects);
	for (int i = 0; i < RTVNum; i++)
	{
		//�S��ʃN���A
		cmdList->ClearRenderTargetView(rtvHs[i], clearColor_, 0, nullptr);
	}
	//�[�x�o�b�t�@�N���A
	const float depthNum = 1.0f;
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, depthNum, 0, 0, nullptr);
}

void PostEffect::PostDraw(ID3D12GraphicsCommandList* cmdList)
{
	const int CRBNum = 2;
	for (int i = 0; i < CRBNum; i++)
	{
		CD3DX12_RESOURCE_BARRIER CRB = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		//���\�[�X�o���A�ύX(�`��\���V�F�[�_�[���\�[�X)
		const UINT barrierNum = 1;
		cmdList->ResourceBarrier(barrierNum, &CRB);
	}
}
