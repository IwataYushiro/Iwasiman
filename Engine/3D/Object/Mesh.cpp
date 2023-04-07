#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh()
{
	
}

Mesh::~Mesh()
{
	delete input_;
}

void Mesh::Initialize(WinApp* winApp, DirectXCommon* dxCommon)
{
	HRESULT result;
	this->winApp_ = winApp;
	this->dxCommon_ = dxCommon;
	input_ = new Input();
	//�V���O���g���C���X�^���X���擾
	input_->Initialize(winApp_);

#pragma region �`�揉��������
	for (int i = 0; i < VerticesCount_; i++)
	{
		//�O�p�`����ƂɌv�Z
		//�O�p�`�̃C���f�b�N�X�����o���A�ꎞ�I�ȕϐ��ɓ����
		unsigned short Index0 = indices[i * 3 + 0];
		unsigned short Index1 = indices[i * 3 + 1];
		unsigned short Index2 = indices[i * 3 + 2];
		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices_[Index0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices_[Index1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices_[Index2].pos);
		//p0=>p1,p0=>p2�x�N�g�����v�Z(�x�N�g�����Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//���K��(������1��)
		normal = XMVector3Normalize(normal);
		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices_[Index0].normal, normal);
		XMStoreFloat3(&vertices_[Index1].normal, normal);
		XMStoreFloat3(&vertices_[Index2].normal, normal);
	}
	//���_�f�[�^�S�̂̃T�C�Y = ���_�f�[�^����̃T�C�Y * ���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));

	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};		//�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB;//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//���_�o�b�t�@�̐���
	
	result =dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));
	//���\�[�X�ݒ�
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeIB;//�C���f�b�N�X��񂪓��镪�̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�C���f�b�N�X�o�b�t�@�̐���
	
	result = dxCommon_->GetDevice()->CreateCommittedResource(
		&heapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&indexBuff));
	assert(SUCCEEDED(result));

	//�C���f�b�N�X�o�b�t�@���}�b�s���O
	uint16_t* indexMap = nullptr;
	result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	assert(SUCCEEDED(result));
	//�S�C���f�b�N�X�ɑ΂���
	for (int i = 0; i < _countof(indices); i++)
	{
		indexMap[i] = indices[i];
	}
	//�}�b�s���O����
	indexBuff->Unmap(0, nullptr);


	//�萔�o�b�t�@�̐ݒ�
	CreateConstBufferMaterial3d(&material3d_, dxCommon_->GetDevice());

	//�I�u�W�F�N�g�̏�����
	for (int i = 0; i < _countof(object3ds_); i++)
	{
		CreateConstBufferObject3d(&object3ds_[i], dxCommon_->GetDevice());
		//�ȉ��e�q�\���̃T���v��
		SetObject3ds(i);
	}

	//�l���������ނƎ����I�ɓ]�������
	material3d_.constMapMaterial->color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//�P�ʍs�����
	//���s���e�ϊ�
	//constMapTransform->mat = XMMatrixOrthographicOffCenterLH(0, windowWidth, windowHeight, 0, 0, 1);

	//�������e�ϊ��s��̌v�Z
	matprojection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)winApp_->window_width / winApp_->window_height,
		0.1f, 1000.0f
	);

	//�r���[�ϊ��s��

	//���_
	eye = { 0.0f, 0.0f, -100.0f };
	//�����_
	target = { 0.0f, 0.0f, 0.0f };
	//������x�N�g��
	up = { 0.0f, 1.0f, 0.0f };

	matview = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	this->isSpace = false;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/texture.png",	//Resources�t�H���_��texture.png
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//�~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	//�e�N�X�`���o�b�t�@�ݒ�

	// �q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;							//��
	textureResourceDesc.Height = (UINT16)metadata.height;				//����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�̐���
	
	result = dxCommon_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff));
	assert(SUCCEEDED(result));



	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texbuff->WriteToSubresource(
			(UINT)i,
			nullptr,							//�S�̈�փR�s�[
			img->pixels,						//���f�[�^�A�h���X
			(UINT)img->rowPitch,				//1���C���T�C�Y
			(UINT)img->slicePitch				//1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	TexMetadata metadata2{};
	ScratchImage scratchImg2{};
	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/reimu.png",	//Resources�t�H���_��texture.png
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);

	assert(SUCCEEDED(result));

	ScratchImage mipChain2{};
	//�~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg2.GetImages(), scratchImg2.GetImageCount(), scratchImg2.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain2);
	if (SUCCEEDED(result))
	{
		scratchImg2 = std::move(mipChain2);
		metadata2 = scratchImg2.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata2.format = MakeSRGB(metadata2.format);

	//�e�N�X�`���o�b�t�@�ݒ�
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;							//��
	textureResourceDesc2.Height = (UINT16)metadata2.height;				//����
	textureResourceDesc2.DepthOrArraySize = (UINT16)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT16)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�̐���

	result = dxCommon_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texbuff2));
	assert(SUCCEEDED(result));



	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata2.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img2 = scratchImg2.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		result = texbuff2->WriteToSubresource(
			(UINT)i,
			nullptr,							//�S�̈�փR�s�[
			img2->pixels,						//���f�[�^�A�h���X
			(UINT)img2->rowPitch,				//1���C���T�C�Y
			(UINT)img2->slicePitch				//1���T�C�Y
		);
		assert(SUCCEEDED(result));
	}
	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	//�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	

	//�ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dxCommon_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));

	//SRV�q�[�v�̃n���h�����擾
	srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};				//�ݒ�\����
	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	dxCommon_->GetDevice()->CreateShaderResourceView(texbuff.Get(), &srvDesc, srvHandle);
	//1�n���h����i�߂�
	incrementSize = dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//�V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};				//�ݒ�\����
	srvDesc2.Format = resDesc.Format;
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	//2D�e�N�X�`��
	srvDesc2.Texture2D.MipLevels = resDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	dxCommon_->GetDevice()->CreateShaderResourceView(texbuff2.Get(), &srvDesc2, srvHandle);

	//�[�x�o�b�t�@
	dxCommon_->InitializeDepthBuffer();

	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));

	//�q���������
	vertBuff->Unmap(0, nullptr);

	//���_�o�b�t�@�r���[�̍쐬
	// GPU���z�A�h���X
	vdView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	// ���_�o�b�t�@�̃T�C�Y
	vdView.SizeInBytes = sizeVB;
	// ���_1���̃f�[�^�T�C�Y
	vdView.StrideInBytes = sizeof(vertices_[0]);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	idView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	idView.Format = DXGI_FORMAT_R16_UINT;
	idView.SizeInBytes = sizeIB;

#pragma region ���_�V�F�[�_�[
	
				   
	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shader/BasicVS.hlsl",								//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				//�C���N���[�h�\�ɂ���
		"main", "vs_5_0",								//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
		0,
		&vsBlob, &errorBlob);

	//�G���[�Ȃ�
	if (FAILED(result))
	{
		//errorBlod����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize()),

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�D�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
#pragma endregion

#pragma region �s�N�Z���V�F�[�_�[
	//�s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shader/BasicPS.hlsl",								//�V�F�[�_�[�t�@�C����
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,				//�C���N���[�h�\�ɂ���
		"main", "ps_5_0",								//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,//�f�o�b�N�p�ݒ�
		0,
		&psBlob, &errorBlob);
	//�G���[�Ȃ�
	if (FAILED(result))
	{
		//errorBlod����G���[���e��string�^�ɃR�s�[
		std::string error;
		error.resize(errorBlob->GetBufferSize()),

			std::copy_n((char*)errorBlob->GetBufferPointer(),
				errorBlob->GetBufferSize(),
				error.begin());
		error += "\n";
		//�G���[���e���o�̓E�B���h�D�ɕ\��
		OutputDebugStringA(error.c_str());
		assert(0);
	}
#pragma endregion

#pragma region ���_���C�A�E�g
	//���_���C�A�E�g
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		//xyz���W(1�s�ŏ������������₷��)
		{
			"POSITION",										//�Z�}���e�B�b�N��
			0,												//�����Z�}���e�B�b�N������������Ƃ��Ɏg���C���f�b�N�X(0�ł���)
			DXGI_FORMAT_R32G32B32_FLOAT,					//�v�f���ƃr�b�g����\��(XYZ��3��float�^�Ȃ̂�R32G32B32_FLOAT)
			0,												//���̓X���b�g�C���f�b�N�X(0�ł���)
			D3D12_APPEND_ALIGNED_ELEMENT,					//�f�[�^�̃I�t�Z�b�g�l(D3D12_APPEND_ALIGNED_ELEMENT���Ǝ����ݒ�)
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,		//���̓f�[�^���(�W����D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
			0												//��x�ɕ`�悷��C���X�^���X��(0�ł���)
		},//���W�ȊO�ɐF�A�e�N�X�`��UV����n���ꍇ�͂���ɑ�����
		//�@���x�N�g��		
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},
		//uv���W(1�s�ŏ������������₷��)
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,						//uv��float����Ȃ̂Ńt�H�[�}�b�g��RG����
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}


	};
#pragma endregion

#pragma region �O���t�B�b�N�X�p�C�v���C���ݒ�
	//�O���t�B�b�N�X�p�C�v���C���ݒ�
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	//�V�F�[�_�[�̐ݒ�
	pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();
	//�T���v���}�X�N�̐ݒ�
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;			//�W���ݒ�
	//���X�^���C�U�̐ݒ�
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;	//�J�����O����(���Ȃ��Ƃ���BACK��NONE�ɂ���)
	pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	//�|���S�����h��Ԃ�
	pipelineDesc.RasterizerState.DepthClipEnable = true;			//�[�x�N���b�s���O��L����
	//�u�����h�X�e�[�g�̐ݒ�
	//pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	//	= D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA�S�Ẵ`�����l����`��
	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;//�[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	//�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA�S�Ẵ`�����l����`��
	//�u�����h���ʐݒ�(����{�����œ���)
	blenddesc.BlendEnable = true;					//�u�����h��L���ɂ���
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;	//���Z
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;		//�\�[�X�̒l��100���g��
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;	//�e�X�g�̒l��0���g��
	//����������(�A���t�@�u�����f�B���O�@�f�t�H���g)
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;			//���Z
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;		//�\�[�X�̃A���t�@�l
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l

	//���_���C�A�E�g�̐ݒ�
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	//�}�`�̌`��̐ݒ�
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	
	//���̑��̐ݒ�
	pipelineDesc.NumRenderTargets = 1;								//�`��Ώۂ�1��
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;	//0�`255�w���RGBA
	pipelineDesc.SampleDesc.Count = 1;								//1�s�N�Z���ɂ�1�T���v�����O

	
	//�f�X�N���v�^�����W�̐ݒ�
	D3D12_DESCRIPTOR_RANGE descriptorRange{};
	descriptorRange.NumDescriptors = 1;								//��x�̕`��Ɏg���e�N�X�`����1���Ȃ̂�1
	descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange.BaseShaderRegister = 0;							//�e�N�X�`�����W�X�^0��
	descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//���[�g�p�����[�^�̐ݒ�
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	//�萔�o�b�t�@0��
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//�萔�o�b�t�@�r���[(���)
	rootParams[0].Descriptor.ShaderRegister = 0;						//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;							//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//�S�ẴV�F�[�_���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;			//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//�S�ẴV�F�[�_���猩����
	//�萔�o�b�t�@1��
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//�萔�o�b�t�@�r���[(���)
	rootParams[2].Descriptor.ShaderRegister = 1;						//�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;							//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//�S�ẴV�F�[�_���猩����

	//�e�N�X�`���T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//���J��Ԃ�(�^�C�����O)
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//�c�J��Ԃ�(�^�C�����O)
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;					//���s�J��Ԃ�(�^�C�����O)
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	//�{�[�_�[�̎��͍�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;					//�S�ă��j�A���
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;									//�~�b�v�}�b�v�ő�l
	samplerDesc.MinLOD = 0.0f;												//�~�b�v�}�b�v�ŏ��l
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;			//�s�N�Z���V�F�[�_�[����̂ݎg�p�\

	// ���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;						//���[�g�p�����[�^�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);			//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	result = dxCommon_->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	
	// �p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature.Get();
#pragma endregion

#pragma region �p�C�v���C���X�e�[�g�̐���

	result = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
#pragma endregion
}

void Mesh::CreateConstBufferMaterial3d(Material3d* material, ID3D12Device* device) {

	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(material->cbdm) + 0xff) & ~0xff;//256�o�C�g�A���C�������g
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&material->constBuffMaterial));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = material->constBuffMaterial->Map(0, nullptr, (void**)&material->constMapMaterial);//�}�b�s���O
	assert(SUCCEEDED(result));
}

void Mesh::CreateConstBufferObject3d(Object3d* object, ID3D12Device* device) {

	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//�q�[�v�ݒ�
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(object->cbdt) + 0xff) & ~0xff;//256�o�C�g�A���C�������g
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&object->constBuffTransform));
	assert(SUCCEEDED(result));

	//�萔�o�b�t�@�̃}�b�s���O
	result = object->constBuffTransform->Map(0, nullptr, (void**)&object->constMapTransform);//�}�b�s���O
	assert(SUCCEEDED(result));
}
//�I�u�W�F�N�g�̏�����
void Mesh::SetObject3ds(int num)
{
	//�����V�[�h�𐶐�
	std::random_device seed_gen;
	//�����Z���k�E�c�C�X�^�[�̗����G���W���𐶐�
	std::mt19937_64 engine(seed_gen());
	//�����͈͂��w��
	std::uniform_real_distribution<float> dist(-25.0f, 25.0f);

	//�����G���W����ϐ��ɓn��
	float randPosX = dist(engine);
	float randPosY = dist(engine);
	float randPosZ = dist(engine);
	//�擪�ȊO�Ȃ�
	if (num > 0)
	{
		//1�O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�ɂ���//���ꂪ�Ȃ��ƒǏ]���Ȃ��Ȃ�
		//object3ds_[i].parent = &object3ds_[i - 1];
		//�e�I�u�W�F�N�g��9���̑傫��
		object3ds_[num].scale = { 0.5f,0.5f,0.5f };
		//�e�I�u�W�F�N�g�ɑ΂���Z�������30�x��]
		object3ds_[num].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };

		//�e�I�u�W�F�N�g�ɑ΂���Z��������-8.0f���炷
		object3ds_[num].position = { randPosX,randPosY,randPosZ };
	}
}


void Mesh::Update(ID3D12Device* device)
{

	input_->Update();

	//���_�𑀍�
	if (input_->PushKey(DIK_D) || input_->PushKey(DIK_A))
	{
		if (input_->PushKey(DIK_D)) { angle += XMConvertToRadians(1.0f); }
		else if (input_->PushKey(DIK_A)) { angle -= XMConvertToRadians(1.0f); }

		//angle���W�A������y������ɉ�]�A���a��-100
		eye.x = -100.0f * sinf(angle);
		eye.z = -100.0f * cosf(angle);

		matview = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	}
	if (input_->TriggerKey(DIK_SPACE))
	{
		if (!isSpace)
		{
			isSpace = true;
			srvHandle.ptr += incrementSize;
			srvGpuHandle.ptr += incrementSize;
		}
		else if(isSpace)
		{
			isSpace = false;
			srvHandle.ptr -= incrementSize;
			srvGpuHandle.ptr -= incrementSize;
		}

	}
	for (int i = 0; i < _countof(object3ds_); i++)
	{
		UpdateObject3d(&object3ds_[i], matview, matprojection);
	}
	//����
	ControlObject3d(&object3ds_[0]);
}

void Mesh::UpdateObject3d(Object3d* object, XMMATRIX& matview, XMMATRIX& matprojection)
{
	XMMATRIX matScale, matRot, matTrans;

	//�X�P�[�����O���v�Z
	matScale = XMMatrixScaling(object->scale.x, object->scale.y, object->scale.z);

	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(object->rotation.z);
	matRot *= XMMatrixRotationX(object->rotation.x);
	matRot *= XMMatrixRotationY(object->rotation.y);

	matTrans = XMMatrixTranslation(object->position.x, object->position.y, object->position.z);
	//���[���h�s�񍇐�
	object->matWorld = XMMatrixIdentity();
	object->matWorld *= matScale;	//�X�P�[�����O���f
	object->matWorld *= matRot;		//��]���f
	object->matWorld *= matTrans;	//���s�ړ����f

	//�e�I�u�W�F�N�g�������
	if (object->parent != nullptr)
	{
		//�e�I�u�W�F�N�g�̃��[���h�s����|���Z
		object->matWorld *= object->parent->matWorld;
	}

	//�萔�o�b�t�@�փf�[�^�]��
	object->constMapTransform->mat = object->matWorld * matview * matprojection;
}

//����
void Mesh::ControlObject3d(Object3d* object)
{
	if (input_->PushKey(DIK_UP)|| input_->PushKey(DIK_DOWN) || input_->PushKey(DIK_RIGHT) || input_->PushKey(DIK_LEFT))
	{
		if (input_->PushKey(DIK_UP)) { object->position.y += 1.0f; }
		else if (input_->PushKey(DIK_DOWN)) { object->position.y -= 1.0f; }
		if (input_->PushKey(DIK_RIGHT)) { object->position.x += 1.0f; }
		else if (input_->PushKey(DIK_LEFT)) { object->position.x -= 1.0f; }
	}
}

void Mesh::Draw()
{
	//�S���_�ɑ΂���
	for (int i = 0; i < _countof(vertices_); i++)
	{
		vertMap[i] = vertices_[i];		//���W���R�s�[
	}

	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dxCommon_->GetCommandList()->SetPipelineState(pipelineState.Get());
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	//���X�g���X�g���b�v��

	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, material3d_.constBuffMaterial->GetGPUVirtualAddress());
	//SRV�q�[�v�̐ݒ�R�}���h
	ID3D12DescriptorHeap* srvHeaps[] = { srvHeap.Get() };
	dxCommon_->GetCommandList()->SetDescriptorHeaps(_countof(srvHeaps), srvHeaps);
	// SRV�q�[�v�̐擪�n���h�����擾(SRV���w���Ă���͂�)

	// SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
	//�S�I�u�W�F�N�g�`��
	for (int i = 0; i < _countof(object3ds_); i++)
	{
		DrawObject3d(&object3ds_[i], vdView, idView, _countof(indices));
	}
	//�`��R�}���h
	//commandList->DrawInstanced(_countof(vertices), 1, 0, 0);	//�S�Ă̒��_���g���ĕ`��
}

void Mesh::DrawObject3d(Object3d* object,D3D12_VERTEX_BUFFER_VIEW& vdView, D3D12_INDEX_BUFFER_VIEW& idView, UINT numIndices)
{
	// ���_�o�b�t�@�r���[�̐ݒ�R�}���h
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vdView);
	// �C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	dxCommon_->GetCommandList()->IASetIndexBuffer(&idView);
	//�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, object->constBuffTransform->GetGPUVirtualAddress());
	//�C���f�b�N�X�o�b�t�@���g���ꍇ
	dxCommon_->GetCommandList()->DrawIndexedInstanced(numIndices, 1, 0, 0, 0);
}
