#include "ViewProjection.h"
#include <cassert>
#include "WinApp.h"

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
	farZ = 1000.0f;

	//行列
	matView_ = XMMatrixIdentity();
	matProjection_ = XMMatrixIdentity();

	
}
