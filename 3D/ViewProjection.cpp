#include "ViewProjection.h"
#include <cassert>
#include <cmath>
#include "WinApp.h"
#include "DirectXCommon.h"

using namespace DirectX;

void ViewProjection::Initialize()
{
	//各ベクトル
	eye_ = { 0.0f, 0.0f, -100.0f };
	target_ = { 0.0f, 0.0f, 0.0f };
	up_ = { 0.0f,1.0f,0.0f };

	//視野角
	angle_ = XMConvertToRadians(45.0f);
	//アスペクト比
	aspect_ = (float)WinApp::window_width / WinApp::window_height;
	//ニアクリップ(手前側の深度限界)
	nearZ_ = 0.1f;
	//ファークリップ(奥側の深度限界)
	farZ_ = 1000.0f;

	//行列
	matView_ = XMMatrixIdentity();
	matProjection_ = XMMatrixIdentity();
	matBillboard_ = XMMatrixIdentity();
	matBillboardY_ = XMMatrixIdentity();

	//定数バッファ生成
	CreateConstBuffer();
	//マッピング
	MapingCB();
}

void ViewProjection::CreateConstBuffer()
{
	HRESULT result;

	D3D12_HEAP_PROPERTIES cbHeapProp{};		//ヒープ設定
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPUへの転送用
	//リソース設定
	D3D12_RESOURCE_DESC cbResourseDesc{};
	cbResourseDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourseDesc.Width = (sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff;//256バイトアラインメント
	cbResourseDesc.Height = 1;
	cbResourseDesc.DepthOrArraySize = 1;
	cbResourseDesc.MipLevels = 1;
	cbResourseDesc.SampleDesc.Count = 1;
	cbResourseDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファの生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&cbResourseDesc, //リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void ViewProjection::MapingCB()
{
	HRESULT result;

	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

void ViewProjection::Update()
{
	//行列計算
	UpdateView();
	UpdateProjection();

	//合成
	matViewProjection_ = matView_ * matProjection_;
}

void ViewProjection::UpdateView()
{
	//視点座標
	XMVECTOR eyePos = XMLoadFloat3(&eye_);
	//注視点座標
	XMVECTOR targetPos = XMLoadFloat3(&target_);
	//上方向ベクトル
	XMVECTOR upVec = XMLoadFloat3(&up_);

	//カメラZ軸(視線)
	XMVECTOR axisZ = XMVectorSubtract(targetPos, eyePos);
	//ベクトルが0だと向き不明なので除外
	assert(!XMVector3Equal(axisZ, XMVectorZero()));
	assert(!XMVector3IsInfinite(axisZ));
	assert(!XMVector3Equal(upVec, XMVectorZero()));
	assert(!XMVector3IsInfinite(upVec));
	//正規化
	axisZ = XMVector3Normalize(axisZ);

	//X軸(右)
	XMVECTOR axisX;
	//X軸は上方向→Z軸の外積で決まる
	axisX = XMVector3Cross(upVec, axisZ);
	//正規化
	axisX = XMVector3Normalize(axisX);

	//Y軸(上方向)
	XMVECTOR axisY;
	//Y軸はZ軸→X軸の外積で決まる
	axisY = XMVector3Cross(axisZ, axisX);

	//カメラ回転行列
	XMMATRIX matCameraRot;
	//カメラ座標→ワールド座標系の変換行列
	matCameraRot.r[0] = axisX;
	matCameraRot.r[1] = axisY;
	matCameraRot.r[2] = axisZ;
	matCameraRot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	//転置して逆行列を計算
	matView_ = XMMatrixTranspose(matCameraRot);

	//視点座標 * -1
	XMVECTOR reverseEyePos = XMVectorNegate(eyePos);
	//カメラの位置からワールド原点へのベクトル
	XMVECTOR tX = XMVector3Dot(axisX, reverseEyePos);
	XMVECTOR tY = XMVector3Dot(axisY, reverseEyePos);
	XMVECTOR tZ = XMVector3Dot(axisZ, reverseEyePos);
	//一つのベクトルにまとめる
	XMVECTOR trans = XMVectorSet(tX.m128_f32[0], tY.m128_f32[1], tZ.m128_f32[2], 1.0f);
	//ビュー行列に平行移動成分を指定
	matView_.r[3] = trans;

}

void ViewProjection::UpdateProjection()
{
	//透視投影による射影行列
	matProjection_ = XMMatrixPerspectiveFovLH(
		angle_, aspect_, nearZ_, farZ_
	);
}

void ViewProjection::Trans()
{
}
