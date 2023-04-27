#include "FbxLoader.h"

FbxLoader* FbxLoader::GetInstance()
{
    static FbxLoader instance;
    return &instance;
}
