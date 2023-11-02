#pragma once
#include "Sprite.h"

/*

*	PostEffect.h

*	ポストエフェクト

*/
class PostEffect
{
public: // メンバ関数
    //エイリアステンプレートとかで色々省略
    template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
    using XMFLOAT2 = DirectX::XMFLOAT2;
    using XMFLOAT3 = DirectX::XMFLOAT3;
    using XMFLOAT4 = DirectX::XMFLOAT4;
    using XMMATRIX = DirectX::XMMATRIX;

public://構造体類
    //マテリアル
    struct ConstBufferDataMaterial
    {
        XMFLOAT4 color;
    };

    //座標
    struct ConstBufferDataTransform
    {
        XMMATRIX mat;	//3D変換行列
    };

    //頂点番号
    enum VertexNumber
    {
        LB,		//左下
        LT,		//左上
        RB,		//右下
        RT,		//右上
        verticesCount,//要素数
    };

    //頂点データ構造体
    struct Vertex
    {
        XMFLOAT3 pos;		//xyz座標
        XMFLOAT2 uv;		//uv座標
    };
private://静的メンバ変数
    //ベースディレクトリ
    static const std::string baseDirectory_;
    //頂点シェーダー
    static const std::string directoryVS_;
    //ピクセルシェーダー
    static const std::string directoryPS_;
    //画面クリアカラー
    static const float clearcolor_[4];

public:
    //コンストラクタ
    PostEffect();
    //初期化(スプライト基盤、使用ポストエフェクトパス)
    void Initialize(SpriteCommon* spCommon, const std::string& fileName="None");
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
    //パイプライン生成(ポストエフェクトファイルパス)
    void CreateGraphicsPipelineState(const std::string& fileName);

    //描画(コマンドリスト)
    void Draw(ID3D12GraphicsCommandList* cmdList);
    //描画前処理(コマンドリスト)
    void PreDraw(ID3D12GraphicsCommandList* cmdList);

    //描画後処理(コマンドリスト)
    void PostDraw(ID3D12GraphicsCommandList* cmdList);

private:
    //頂点データ
    Vertex verticesPost_[verticesCount] = {
        {{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},	//左下
        {{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},	//左上
        {{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},	//右下
        {{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},	//右上
    };

    //定数バッファのGPUリソースのポインタ
    ComPtr<ID3D12Resource> constBuffMaterialPost_ = nullptr;
    //マッピング用のポインタ
    ConstBufferDataMaterial* constMapMaterialPost_ = nullptr;
    //定数バッファのGPUリソースのポインタ
    ComPtr<ID3D12Resource> constBuffTransformPost_ = nullptr;
    //マッピング用のポインタ
    ConstBufferDataTransform* constMapTransformPost_ = nullptr;

    //スプライト基盤
    SpriteCommon* spCommon_ = nullptr;
    //頂点バッファ
    ComPtr<ID3D12Resource> vertBuff_;
    //頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    //テクスチャバッファ
    ComPtr<ID3D12Resource> texBuff_[2];
    //SRV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapSRV_;
    //深度バッファ
    ComPtr<ID3D12Resource> depthBuff_;
    //RTV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
    //DSV用デスクリプタヒープ
    ComPtr<ID3D12DescriptorHeap> descHeapDSV_;
    //グラフィックスパイプライン
    ComPtr<ID3D12PipelineState> pipelineState_;
    //ルートシグネチャ
    ComPtr<ID3D12RootSignature> rootSignature_;
  
    //色(RGBA)
    XMFLOAT4 color_ = { 1.0f,1.0f,1.0f,1.0f };
public:
    //カラーセット
    void SetColor(const XMFLOAT4& color) { color_ = color; }
    //カラーゲット
    const XMFLOAT4& GetColor()const { return color_; }
};
