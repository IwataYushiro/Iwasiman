#pragma once
#include "Sprite.h"
//ポストエフェクト
class PostEffect :  public Sprite
{
public:
    //コンストラクタ
    PostEffect();
    //初期化
    void Initialize(SpriteCommon* spCommon);
    //更新
    void Update();
    //頂点バッファ生成
    void CreateVertexBuffer();
    //定数バッファ生成
    void CreateConstBuffer();
    //マテリアル
    void CreateConstBufferMaterialPost();
    //座標
    void CreateConstBufferTransformPost();
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
    //パイプライン生成
    void CreateGraphicsPipelineState();
    
    //描画
    void Draw(ID3D12GraphicsCommandList* cmdList);
    //描画前処理
    void PreDraw(ID3D12GraphicsCommandList* cmdList);

    //描画後処理
    void PostDraw(ID3D12GraphicsCommandList* cmdList);

private:
    //頂点データ
    Vertex verticesPost[verticesCount] = {
        {{-0.5f,-0.5f,0.0f},{0.0f,1.0f}},	//左下
        {{-0.5f,+0.5f,0.0f},{0.0f,0.0f}},		//左上
        {{+0.5f,-0.5f,0.0f},{1.0f,1.0f}},	//右下
        {{+0.5f,+0.5f,0.0f},{1.0f,0.0f}},	//右上
    };
    //定数バッファのGPUリソースのポインタ
    ComPtr<ID3D12Resource> constBuffMaterialPost = nullptr;
    //マッピング用のポインタ
    ConstBufferDataMaterial* constMapMaterialPost = nullptr;

    //定数バッファのGPUリソースのポインタ
    ComPtr<ID3D12Resource> constBuffTransformPost = nullptr;
    //マッピング用のポインタ
    ConstBufferDataTransform* constMapTransformPost = nullptr;

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
    //グラフィックスパイプライン
    ComPtr<ID3D12PipelineState> pipelineState;
    //ルートシグネチャ
    ComPtr<ID3D12RootSignature> rootSignature;
    //画面クリアカラー
    static const float clearcolor[4];

};

