#pragma once
#include "Material.h"
#include <DirectXMath.h>
#include <Windows.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>

/*

*	Mesh.h

*	�`��(���b�V��)

*/
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
	void AddVertex(const VertexPosNormalUv& vertex) { vertices_.emplace_back(vertex); }
	//���_�C���f�b�N�X�ǉ�
	void AddIndex(unsigned short index) { indices_.emplace_back(index); }
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
	const std::string& GetName() { return name_; }
	//���O�Z�b�g
	void SetName(const std::string& name) { this->name_ = name; }
	//�}�e���A���Q�b�g
	Material* GetMaterial() { return material_; }
	//�}�e���A���Z�b�g
	void SetMaterial(Material* material) { this->material_ = material; }
	//���_�f�[�^���Q�b�g
	inline size_t GetVertexCount() { return vertices_.size(); }
	//���_�o�b�t�@�Q�b�g
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }
	//�C���f�b�N�X�o�b�t�@�Q�b�g
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }
	//���_�z��Q�b�g
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }
	//�C���f�b�N�X�z��Q�b�g
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private://�����o�ϐ�
	//���O
	std::string name_;
	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices_;
	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;
	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_;
	//�}�e���A��
	Material* material_ = nullptr;
	//���_�A�C���f�b�N�X�o�b�t�@�̃}�b�s���O
	VertexPosNormalUv* vertMap_ = nullptr;
	unsigned short* indexmap_ = nullptr;
	//���_�X���[�W���O�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;

};
