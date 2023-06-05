#pragma once
#include "Sprite.h"
//ポストエフェクト
class PostEffect :  public Sprite
{
public:
    //コンストラクタ
    PostEffect();
    //初期化
    void Initialize(SpriteCommon* spCommon, uint32_t textureIndex = UINT32_MAX);
    //テクスチャ生成
    void CreateTexture();
    // SRV生成
    void CreateSRV();
    // RTV生成
    void CreateRTV();
    // 深度バッファ生成
    void CreateDepthBuffer();
    // DSV生成
    void CreateDSV();
    
    //描画
    void Draw(ID3D12GraphicsCommandList* cmdList);

private:
    //テクスチャバッファ
    ComPtr<ID3D12Resource> texBuff;
    //SRV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapSRV;
    //深度バッファ
    ComPtr<ID3D12Resource> depthBuff;
    //RTV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapRTV;
    //DSV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};

