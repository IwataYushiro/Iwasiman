#pragma once
#include "ModelFbx.h"
#include "Camera.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXMath.h>
#include <string>
#include <Windows.h>
#include <wrl.h>

class ObjectFbx
{
protected://エイリアス
	//Microsoft::WRL::を省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public://静的メンバ関数
	//セッター
	static void SetDevice(ID3D12Device* device) { ObjectFbx::device_ = device; }
	static void SetCamera(Camera* camera) { ObjectFbx::camera_ = camera; }


private://静的メンバ変数
	//デバイス
	static ID3D12Device* device_;
	//カメラ
	static Camera* camera_;
};
