#include <DirectXTex.h>
#include "Model.h"
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <Windows.h>

using namespace DirectX;
using namespace std;

//�ÓI�����o�ϐ��̎���
ID3D12Device* Model::device = nullptr;

//OBJ�t�@�C������3D���f����ǂݍ���
Model* Model::LoadFromOBJ(const std::string& modelName, bool smoothing)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	Model* model = new Model();

	//�f�X�N���v�^�q�[�v����
	model->InitializeDescriptorHeap();

	//OBJ�t�@�C������̃f�[�^�ǂݍ���
	model->LoadFromOBJInternal(modelName, smoothing);

	//�ǂݍ��񂾃f�[�^�����Ɋe��o�b�t�@����
	model->CreateBuffers();

	return model;
}

//�f�X�N���v�^�q�[�v
void Model::InitializeDescriptorHeap() {

	HRESULT result = S_FALSE;

	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = 1; // �V�F�[�_�[���\�[�X�r���[1��
	result = device->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//����
	if (FAILED(result)) {
		assert(0);
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

//�}�e���A���ǂݍ���
void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	//�}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}

	//1�s���ǂݍ���
	string line;
	while (getline(file, line))
	{
		//1�s���̕�������X�g���[���ɕϊ�
		std::istringstream line_stream(line);

		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�擪�̃^�u�����͖���
		if (key[0] == '\t')
		{
			key.erase(key.begin());//�擪�̕������폜
		}

		//�擪������newlib�Ȃ�}�e���A����
		if (key == "newmtl")
		{
			//�}�e���A�����ǂݍ���
			line_stream >> material.name;
		}
		//�擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka")
		{
			line_stream >> material.ambient.x;
			line_stream >> material.ambient.y;
			line_stream >> material.ambient.z;
		}
		//�擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd")
		{
			line_stream >> material.diffuse.x;
			line_stream >> material.diffuse.y;
			line_stream >> material.diffuse.z;
		}
		//�擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks")
		{
			line_stream >> material.specular.x;
			line_stream >> material.specular.y;
			line_stream >> material.specular.z;
		}
		////�擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd")
		{
			//�e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material.textureFilename;
			//�e�N�X�`���ǂݍ���
			LoadTexture(directoryPath, material.textureFilename);
		}
	}
	//�t�@�C�������
	file.close();
}

// �`��
void Model::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial) {
	// nullptr�`�F�b�N
	assert(device);
	assert(cmdList);

	

	// �f�X�N���v�^�q�[�v�̔z��
	ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	if (material.textureFilename.size() > 0)
	{
		
	}
	
}

//OBJ�t�@�C������3D���f����ǂݍ���(����J)
void Model::LoadFromOBJInternal(const std::string& modelName,bool smoothing) {
	//�t�@�C���X�g���[��
	std::ifstream file;
	//obj�t�@�C�����J��
	const string filename = modelName + ".obj";					
	const string directoryPath = "Resources/" + modelName + "/";

	file.open(directoryPath + filename);//"Resources/triangle_mat/triangle_mat.obj"
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}

	vector<XMFLOAT3> positions;					//���_���W
	vector<XMFLOAT3> normals;					//�@���x�N�g��
	vector<XMFLOAT2> texcoords;					//�e�N�X�`��UV
	//1�s���ǂݍ���
	string line;
	while (getline(file, line))
	{
		//1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);
		//���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		//�擪������mtllib�Ȃ�}�e���A��
		if (key == "mtllib")
		{
			//�}�e���A���̃t�@�C�����ǂݍ���
			string filename;
			line_stream >> filename;
			//�}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}
		//�擪������v�Ȃ璸�_���W
		if (key == "v")
		{
			//X,Y,Z���W�ǂݍ���
			XMFLOAT3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;
			//���W�f�[�^�ɒǉ�
			positions.emplace_back(position);

		}
		//�擪������vt�Ȃ�e�N�X�`��
		if (key == "vt")
		{
			//U,V�����ǂݍ���
			XMFLOAT2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			//V�������]
			texcoord.y = 1.0f - texcoord.y;
			//�e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);

		}
		//�擪������vn�Ȃ�@���x�N�g��
		if (key == "vn")
		{
			//X,Y,Z�����ǂݍ���
			XMFLOAT3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;

			//�@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}

		//�擪������f�Ȃ�|���S��(�O�p�`)
		if (key == "f")
		{
			//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexTexcoord, indexNormal;
				index_stream >> indexPosition;
				//�X���b�V�����΂�
				index_stream.seekg(1, ios_base::cur);
				index_stream >> indexTexcoord;
				//�X���b�V�����΂�
				index_stream.seekg(1, ios_base::cur);
				index_stream >> indexNormal;

				//���_�f�[�^�̒ǉ�
				VertexPosNormalUv vertex{};
				vertex.pos = positions[indexPosition - 1];
				vertex.normal = normals[indexNormal - 1];
				vertex.uv = texcoords[indexTexcoord - 1];
				vertices.emplace_back(vertex);

				//�C���f�b�N�X�f�[�^�̒ǉ�

				//���_�C���f�b�N�X�ɒǉ�
				indices.emplace_back((unsigned short)indices.size());
			}
		}
	}
	//�t�@�C�������
	file.close();

}

