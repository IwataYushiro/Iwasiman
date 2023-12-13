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
UINT Model::descriptorHandleIncrementSize_;

const std::string Model::BASE_DIRECTORY = "Resources/";

Model::~Model()
{
	for (auto m : meshes_)
	{
		delete m;
	}
	meshes_.clear();

	for (auto m: materials_)
	{
		delete m.second;
	}
	materials_.clear();
}

void Model::StaticInitialize(ID3D12Device* device)
{
	Model::device_ = device;
	//���b�V��������
	Mesh::StaticInitialize(device_);
}

//OBJ�t�@�C������3D���f����ǂݍ���
std::unique_ptr<Model> Model::LoadFromOBJ(const std::string& modelName, bool smoothing)
{
	//�V����Model�^�̃C���X�^���X�̃��������m��
	std::unique_ptr<Model> model = std::make_unique<Model>();
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
	size_t materialCount = materials_.size();
	// �f�X�N���v�^�q�[�v�𐶐�	
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_���猩����悤��
	descHeapDesc.NumDescriptors = (UINT)materialCount; // �V�F�[�_�[���\�[�X�r���[=�}�e���A����
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descHeap_));//����
	if (FAILED(result)) {
		assert(0);
	}

	// �f�X�N���v�^�T�C�Y���擾
	descriptorHandleIncrementSize_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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
		const int32_t startKeyNum = 0;
		if (key[startKeyNum] == '\t')
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
			const int32_t offsetPos1Num = 1;
			pos1 = material->textureFilename.rfind('\\');
			if (pos1!=string::npos)
			{
				material->textureFilename = material->textureFilename.substr(
					pos1 + offsetPos1Num, material->textureFilename.size() - pos1 - offsetPos1Num);
			}

			pos1 = material->textureFilename.rfind('/');
			if (pos1 != string::npos)
			{
				material->textureFilename = material->textureFilename.substr(
					pos1 + offsetPos1Num, material->textureFilename.size() - pos1 - offsetPos1Num);
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
	string directoryPath = BASE_DIRECTORY + name_ + "/";
	for (auto& m : materials_)
	{
		Material* material = m.second;
		D3D12_CPU_DESCRIPTOR_HANDLE CDHSRV = descHeap_->GetCPUDescriptorHandleForHeapStart();
		CDHSRV.ptr += (descriptorHandleIncrementSize_ * texIndex);
		
		D3D12_GPU_DESCRIPTOR_HANDLE GDHSRV = descHeap_->GetGPUDescriptorHandleForHeapStart();
		GDHSRV.ptr += (descriptorHandleIncrementSize_ * texIndex);
		//�e�N�X�`�������̏ꍇ
		if (material->textureFilename.size() <= 0)
		{
			directoryPath = BASE_DIRECTORY;
		}

		//�e�N�X�`���ǂݍ���
		material->LoadTexture(directoryPath, CDHSRV, GDHSRV);
		texIndex++;
	}
}

// �`��
void Model::Draw(ID3D12GraphicsCommandList* cmdList) const {
	// nullptr�`�F�b�N
	assert(device_);
	assert(cmdList);

	if (descHeap_)
	{
		// �f�X�N���v�^�q�[�v�̔z��
		ID3D12DescriptorHeap* ppHeaps[] = { descHeap_.Get() };
		cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}
	for (auto& mesh : meshes_)
	{
		mesh->Draw(cmdList);
	}
	
}

//OBJ�t�@�C������3D���f����ǂݍ���(����J)
void Model::LoadFromOBJInternal(const std::string& modelName,const bool smoothing) {
	//�t�@�C���X�g���[��
	std::ifstream file;
	//obj�t�@�C�����J��
	const string filename = modelName + ".obj";					
	const string directoryPath = BASE_DIRECTORY + modelName + "/";

	file.open(directoryPath + filename);//"Resources/triangle_mat/triangle_mat.obj"
	//�t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail())
	{
		assert(0);
	}
	name_ = modelName;
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
				meshes_.emplace_back(mesh);
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
			const float reverseCalculation = 1.0f - texcoord.y;
			texcoord.y = reverseCalculation;
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
				auto itr = materials_.find(materialName);
				if (itr!=materials_.end())
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
			//�X���b�V���̃X�L�b�v���邽�߂̃I�t�Z�b�g
			const int32_t seekSkipSlashOffset = 1;
			//���_�C���f�b�N�X�̃I�t�Z�b�g
			const int32_t indexOffset = 1;
			//�f�t�H���g�̖@���x�N�g����UV
			struct DefaultNormalAndUV
			{
				const XMFLOAT3 normal = { 0.0f,0.0f,1.0f };
				const XMFLOAT2 uv = { 0.0f,0.0f };
			};
			DefaultNormalAndUV defaultNormalAndUV;

			while (getline(line_stream, index_string, ' '))
			{
				//���_�C���f�b�N�X1���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexTexcoord, indexNormal;
				//���_�ԍ�
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				//�X���b�V�����΂�
				index_stream.seekg(seekSkipSlashOffset, ios_base::cur);
				//�}�e���A���A�e�N�X�`��������ꍇ
				if (material && material->textureFilename.size() > 0)
				{
					index_stream >> indexTexcoord;
					index_stream.seekg(seekSkipSlashOffset, ios_base::cur);//�X���b�V�����΂�
					index_stream >> indexNormal;

					//���_�f�[�^�̒ǉ�
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - indexOffset];
					vertex.normal = normals[indexNormal - indexOffset];
					vertex.uv = texcoords[indexTexcoord - indexOffset];
					mesh->AddVertex(vertex);
					//�G�b�W�������f�[�^�̒ǉ�
					if (smoothing)
					{
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - indexOffset);
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
						vertex.pos = positions[indexPosition - indexOffset];
						vertex.normal = defaultNormalAndUV.normal;
						vertex.uv = defaultNormalAndUV.uv;
						mesh->AddVertex(vertex);
						//�G�b�W�������f�[�^�̒ǉ�
						if (smoothing)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - indexOffset);
						}
					}
					else
					{
						index_stream.seekg(-seekSkipSlashOffset, ios_base::cur);//�q�g�}�X���h��
						index_stream >> indexTexcoord;
						index_stream.seekg(seekSkipSlashOffset, ios_base::cur);//�X���b�V�����΂�
						index_stream >> indexNormal;
						//���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - indexOffset];
						vertex.normal = normals[indexNormal - indexOffset];
						vertex.uv = defaultNormalAndUV.uv;
						mesh->AddVertex(vertex);
						//�G�b�W�������f�[�^�̒ǉ�
						if (smoothing)
						{
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - indexOffset);
						}
					}
				}
				//�C���f�b�N�X�f�[�^�̒ǉ�
				//3���_��
				const int inPoligonVertexTriangle = 3;
				if (faceIndexCount >= inPoligonVertexTriangle)
				{
					
					// �l�p�`�|���S����4�_�ڂȂ̂ŁA
					// �l�p�`��0,1,2,3�̓� 2,3,0�ŎO�p�`���\�z����
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex - TIN_Index2));
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex));
					mesh->AddIndex(static_cast<unsigned short>(indexCountTex - TIN_Index0));
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
	meshes_.emplace_back(mesh);

	//���b�V���̃}�e���A���`�F�b�N
	for (auto& m : meshes_)
	{
		//�Ȃ��ꍇ
		if (m->GetMaterial() == nullptr)
		{
			if (defaultMaterial_ == nullptr)
			{
				//�f�t�H���g�̃}�e���A���𐶐�
				defaultMaterial_= Material::Create();
				defaultMaterial_->name = "no material";
				materials_.emplace(defaultMaterial_->name, defaultMaterial_);
			}
			//������Z�b�g
			m->SetMaterial(defaultMaterial_);
		}
	}

	//���b�V���̃o�b�t�@����
	for (auto& m : meshes_)
	{
		m->CreateBuffers();
	}
	//�}�e���A�����l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials_)
	{
		m.second->Update();
	}
}

