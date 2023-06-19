#pragma once
#include "Sprite.h"
//ポストエフェクト
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
    static const std::string baseDirectory;
    static const std::string DirectoryVS;
    static const std::string DirectoryPS;

public:
    //コンストラクタ
    PostEffect();
    //初期化
    void Initialize(SpriteCommon* spCommon, const std::string& fileName="Test");
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
    void CreateGraphicsPipelineState(const std::string& fileName);

    //描画
    void Draw(ID3D12GraphicsCommandList* cmdList);
    //描画前処理
    void PreDraw(ID3D12GraphicsCommandList* cmdList);

    //描画後処理
    void PostDraw(ID3D12GraphicsCommandList* cmdList);

private:
    //頂点データ
    Vertex verticesPost[verticesCount] = {
        {{-1.0f,-1.0f,0.0f},{0.0f,1.0f}},	//左下
        {{-1.0f,+1.0f,0.0f},{0.0f,0.0f}},		//左上
        {{+1.0f,-1.0f,0.0f},{1.0f,1.0f}},	//右下
        {{+1.0f,+1.0f,0.0f},{1.0f,0.0f}},	//右上
    };

    //定数バッファのGPUリソースのポインタ
    ComPtr<ID3D12Resource> constBuffMaterialPost = nullptr;
    //マッピング用のポインタ
    ConstBufferDataMaterial* constMapMaterialPost = nullptr;
    //定数バッファのGPUリソースのポインタ
    ComPtr<ID3D12Resource> constBuffTransformPost = nullptr;
    //マッピング用のポインタ
    ConstBufferDataTransform* constMapTransformPost = nullptr;

    //スプライト基盤
    SpriteCommon* spCommon_ = nullptr;
    //頂点バッファ
    ComPtr<ID3D12Resource> vertBuff;
    //頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView{};
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

    //色(RGBA)
    XMFLOAT4 color_ = { 1.0f,1.0f,1.0f,1.0f };
public:
    //色
    void SetColor(const XMFLOAT4& color) { color_ = color; }

    const XMFLOAT4& GetColor()const { return color_; }
};
