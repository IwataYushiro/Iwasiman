#include "Model.h"
#include <fstream>
#include <sstream>
#include <cassert>

using namespace DirectX;
using namespace std;

/*

*	Model.cpp

*	3D���f��(.obj)

*/

//�ÓI�����o�ϐ��̎���
ID3D12Device* Model::device_ = nullptr;
// �f�X�N���v�^�T�C�Y
UINT Model::descriptorHandleIncrementSize;

const std::string Model::baseDirectory = "Resources/";

Model::~Model()
{
	for (auto m : meshes)
	{
		delete m;
	}
	meshes.clear();

	for (auto m: materials)
	{
		delete m.second;
	}
	materials.clear();
}

void Model::StaticInitialize(ID3D12Device* device)
{
	Model::device_ = device;
	//���b�V��������
	Mesh::StaticInitialize(device_);
}

//OBJ�t�@�C������3D���f����ǂݍ���
Model* Model::LoadFromOBJ(const std::string& modelName, bool smoothing)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	Model* model = new Model();
	//OBJ�t�@�C������̃f�[�^�ǂݍ���
	model->LoadFromOBJInternal(modelName, smoothing);

	//�f�X�N���v�^�q�[�v����
	model->InitializeDescriptorHeap();
	//�e�N�X�`���ǂݍ���
	model->LoadTextures();
	
	return model;
}

//�f�X�N���v�^�q�[�v
void Model::InitializeDescriptorHeap() {

	HRESULT result = S_FALSE;

	//�}�e���A����
	size_t materialCount = materials.size();
	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = (UINT)materialCount; // �V�F�[�_�[���\�[�X�r���[=�}�e���A����
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap));//����
	if (FAILED(result)) {
		assert(0);
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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

	Material* material = nullptr;

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

		//�擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl")
		{
			//���Ƀ}�e���A��������ꍇ
			if (material)
			{
				//�}�e���A�����R���e�i�ɓo�^
				AddMaterial(material);
			}
			//�V�����}�e���A���̐���
			material = Material::Create();
			//�}�e���A�����ǂݍ���
			line_stream >> material->name;
		}
		//�擪������Ka�Ȃ�A���r�G���g�F
		if (key == "Ka")
		{
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;
		}
		//�擪������Kd�Ȃ�f�B�t���[�Y�F
		if (key == "Kd")
		{
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;
		}
		//�擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks")
		{
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;
		}
		////�擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd")
		{
			//�e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material->textureFilename;
			//�t���p�X����t�@�C�������o��
			size_t pos1;
			pos1 = material->textureFilename.rfind('\\');
			if (pos1!=string::npos)
			{
				material->textureFilename = material->textureFilename.substr(
					pos1 + 1, material->textureFilename.size() - pos1 - 1);
			}

			pos1 = material->textureFilename.rfind('/');
			if (pos1 != string::npos)
			{
				material->textureFilename = material->textureFilename.substr(
					pos1 + 1, material->textureFilename.size() - pos1 - 1);
			}
		}
	}
	//�t�@�C�������
	file.close();

	if (material)
	{
		AddMaterial(material);
	}
}

void Model::LoadTextures()
{
	int texIndex = 0;
	string directoryPath = baseDirectory + name + "/";
	for (auto& m : materials)
	{
		Material* material = m.second;
		D3D12_CPU_DESCRIPTOR_HANDLE CDHSRV = descHeap->GetCPUDescriptorHandleForHeapStart();
		CDHSRV.ptr += (descriptorHandleIncrementSize * texIndex);
		
		D3D12_GPU_DESCRIPTOR_HANDLE GDHSRV = descHeap->GetGPUDescriptorHandleForHeapStart();
		GDHSRV.ptr += (descriptorHandleIncrementSize * texIndex);
		//�e�N�X�`�������̏ꍇ
		if (material->textureFilename.size() <= 0)
		{
			directoryPath = baseDirectory;
		}

		//�e�N�X�`���ǂݍ���
		material->LoadTexture(directoryPath, CDHSRV, GDHSRV);
		texIndex++;
	}
}

// �`��
void Model::Draw(ID3D12GraphicsCommandList* cmdList) {
	// nullptr�`�F�b�N
	assert(device_);
	assert(cmdList);

	if (descHeap)
	{
		// �f�X�N���v�^�q�[�v�̔z��
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}
	for (auto& mesh : meshes)
	{
		mesh->Draw(cmdList);
	}
	
}

//OBJ�t�@�C������3D���f����ǂݍ���(����J)
void Model::LoadFromOBJInternal(const std::string& modelName,bool smoothing) {
	//�t�@�C���X�g���[��
	std::ifstream file;
	//obj�t�@�C�����J��
	const string filename = modelName + ".obj";					
	const string directoryPath = baseDirectory + modelName + "/";

	file.open(directoryPath + filename);//"Resources/triangle_mat/triangle_mat.obj"
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}
	name = modelName;
	//���b�V������
	Mesh* mesh = new Mesh();
	int indexCountTex = 0;
	//int indexCountNoTex = 0;

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
			string fileNameMat;
			line_stream >> fileNameMat;
			//�}�e���A���ǂݍ���
			LoadMaterial(directoryPath, fileNameMat);
		}
		//�擪������g�Ȃ�O���[�v�̊J�n
		if (key == "g")
		{
			if (mesh->GetName().size()>0)
			{
				//���_�@���̕��ςɂ��G�b�W�̕�����
				if (smoothing)
				{
					mesh->CalculateSmoothedVertexNormals();
				}
				//�R���e�i�ɓo�^
				meshes.emplace_back(mesh);
				//���̃��b�V������
				mesh = new Mesh();
				indexCountTex = 0;
			}

			//�O���[�v���ǂݍ���
			string groupName;
			line_stream >> groupName;

			mesh->SetName(groupName);
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
		// �擪������usemtl�Ȃ�}�e���A�������蓖�Ă�
		if (key == "usemtl") 
		{
			if (mesh->GetMaterial() == nullptr)
			{
				//�}�e���A�����ǂݍ���
				string materialName;
				line_stream >> materialName;

				//�}�e���A�����Ō��������蓖�Ă�
				auto itr = materials.find(materialName);
				if (itr!=materials.end())
				{
					mesh->SetMaterial(itr->second);
				}
			}
		}
		//�擪������f�Ȃ�|���S��(�O�p�`)
		if (key == "f")
		{
			//�C���f�b�N�X�J�E���g
			int faceIndexCount = 0;
			//���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			string index_string;
			while (getline(line_stream, index_string, ' '))
			{
				//���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexTexcoord, indexNormal;
				//���_�ԍ�
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				//�X���b�V�����΂�
				index_stream.seekg(1, ios_base::cur);
				//�}�e���A���A�e�N�X�`��������ꍇ
				if (material && material->textureFilename.size() > 0)
				{
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);//�X���b�V�����΂�
					index_stream >> indexNormal;

					//���_�f�[�^�̒ǉ�
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					//�G�b�W�������f�[�^�̒ǉ�
					if (smoothing)
					{
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else
				{
					char c;
					index_stream >> c;
					// �X���b�V����A���Ȃ璸�_�ԍ��̂�
					if (c == '/')
					{
						//���_�f�[�^�ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0.0f,0.0f,1.0f };
						vertex.uv = { 0.0f,0.0f };
						mesh->AddVertex(vertex);
						//�G�b�W�������f�[�^�̒ǉ�
						if (smoothing)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
					else
					{
						index_stream.seekg(-1, ios_base::cur);//�q�g�}�X���h��
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur);//�X���b�V�����΂�
						index_stream >> indexNormal;
						//���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0.0f,0.0f };
						mesh->AddVertex(vertex);
						//�G�b�W�������f�[�^�̒ǉ�
						if (smoothing)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}
				//�C���f�b�N�X�f�[�^�̒ǉ�
				if (faceIndexCount >= 3)
				{
					// �l�p�`�|���S����4�_�ڂȂ̂ŁA
					// �l�p�`��0,1,2,3�̓� 2,3,0�ŎO�p�`���\�z����
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex - 1));
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex));
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex - 3));
				}
				else
				{
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex));
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	//�t�@�C�������
	file.close();

	//���_�@���̕��ςɂ��G�b�W�̕�����
	if (smoothing)
	{
		mesh->CalculateSmoothedVertexNormals();
	}
	//���b�V���R���e�i�ɓo�^
	meshes.emplace_back(mesh);

	//���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes)
	{
		//�Ȃ��ꍇ
		if (m->GetMaterial() == nullptr)
		{
			if (defaultMaterial == nullptr)
			{
				//�f�t�H���g�̃}�e���A���𐶐�
				defaultMaterial = Material::Create();
				defaultMaterial->name = "no material";
				materials.emplace(defaultMaterial->name, defaultMaterial);
			}
			//������Z�b�g
			m->SetMaterial(defaultMaterial);
		}
	}

	//���b�V���̃o�b�t�@����
	for (auto& m : meshes)
	{
		m->CreateBuffers();
	}
	//�}�e���A�����l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials)
	{
		m.second->Update();
	}
}

