﻿#include "FbxLoader.h"
#include <cassert>
//静的メンバ変数の実体
const std::string FbxLoader::baseDirectory = "Resources/";

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}

void FbxLoader::Initialize(ID3D12Device* device)
{
    //再初期化チェック
    assert(fbxManager_ == nullptr);
    //引数からメンバ変数に代入
    this->device_ = device;

    //FBXマネージャー生成
    fbxManager_ = FbxManager::Create();

    //FBXマネージャーの入出力
    FbxIOSettings* ios = FbxIOSettings::Create(fbxManager_, IOSROOT);
    fbxManager_->SetIOSettings(ios);

    //FBXインポーター生成
    fbxImporter_ = FbxImporter::Create(fbxManager_, "");
}

void FbxLoader::Finalize()
{
    //インスタンス放棄
    fbxImporter_->Destroy();
    fbxManager_->Destroy();
}

void FbxLoader::LoadModelFromFile(const string& modelName)
{
    //モデルと同名のフォルダから読み込む
    const string directoryPath = baseDirectory + modelName + "/";
    //拡張子.fbxを追加
    const string filename = modelName + ".fbx";
    //連結してフルパスを得る
    const string fullPath = directoryPath + filename;
    //ファイル名を指定してFBXファイルを読み込む
    if (!fbxImporter_->Initialize(fullPath.c_str(),-1,
        fbxManager_->GetIOSettings()))
    {
        assert(0);
    }
    //シーン生成
    FbxScene* fbxScene = FbxScene::Create(fbxManager_, "fbxScene");

    //ファイルからロードしたFBXの情報をシーンにインポート
    fbxImporter_->Import(fbxScene);
}
