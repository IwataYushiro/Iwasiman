#pragma once
#include "Material.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>

//�`��(���b�V��)
class Mesh
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;
public://�T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		XMFLOAT3 pos; // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT2 uv;  // uv���W
	};
public:
	//�ÓI������
	static void StaticInitialize(ID3D12Device* device);

private://�ÓI�����o�ϐ�
	static ID3D12Device* device_;

public://�����o�֐�
	//���_�f�[�^�ǉ�
	void AddVertex(const VertexPosNormalUv& vertex) { vertices.emplace_back(vertex); }
	//���_�C���f�b�N�X�ǉ�
	void AddIndex(unsigned short index) { indices.emplace_back(index); }
	//�G�b�W�������f�[�^�ǉ�
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);
	//���������ꂽ���_�@���̒ǉ�
	void CalculateSmoothedVertexNormals();
	//�o�b�t�@����
	void CreateBuffers();
	//�`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

public://�A�N�Z�b�T�u����
	//���O�Q�b�g
	const std::string& GetName() { return name; }
	//���O�Z�b�g
	void SetName(const std::string& name) { this->name = name; }
	//�}�e���A���Q�b�g
	Material* GetMaterial() { return material; }
	//�}�e���A���Z�b�g
	void SetMaterial(Material* material) { this->material = material; }
	//���_�f�[�^���Q�b�g
	inline size_t GetVertexCount() { return vertices.size(); }
	//���_�o�b�t�@�Q�b�g
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }
	//�C���f�b�N�X�o�b�t�@�Q�b�g
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }
	//���_�z��Q�b�g
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }
	//�C���f�b�N�X�z��Q�b�g
	inline const std::vector<unsigned short>& GetIndices() { return indices; }

private://�����o�ϐ�
	//���O
	std::string name;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;
	//�}�e���A��
	Material* material = nullptr;
	//���_�A�C���f�b�N�X�o�b�t�@�̃}�b�s���O
	VertexPosNormalUv* vertMap = nullptr;
	unsigned short* indexmap = nullptr;
	//���_�X���[�W���O�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData;

};
