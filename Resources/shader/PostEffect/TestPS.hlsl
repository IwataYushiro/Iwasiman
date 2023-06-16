#include "Test.hlsli"

Texture2D<float4> tex : register(t0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp      : register(s0);	//0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float cold = 0.2f;
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 gray = float4(grayScale - cold, grayScale, grayScale + cold, 1.0f);
    return gray;
}

/*
�x�[�X
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.rgb, 1.0f);

}

���x�ύX
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
     return float4(texcolor.rgb * 2.0f, 1.0f);

}
�΂����ύX
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(texcolor.r, texcolor.g * 2.0f, texcolor.b, 1.0f);

}

�F���](�l�K�|�W?)
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    return float4(float3(1.0f, 1.0f, 1.0f) - texcolor.rgb, 1.0f);

}

UV���炵(�T���v���[��D3D12_TEXTURE_ADDRESS_MODE_WRAP����H)
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv + float2(0.5f,0.0f));
    return float4(texcolor.rgb, 1.0f);

}

�O���[�X�P�[��
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 gray = float4(grayScale, grayScale, grayScale, 1.0f);
    return gray;

}
�����`���U�C�N
float4 main(VSOutput input) : SV_TARGET
{
    float density = 100;
    float4 col = tex.Sample(smp, floor(input.uv * density) / density);
}


�����`���U�C�N
float4 main(VSOutput input) : SV_TARGET
{
   
    float2 windowsSize = { 1280.0f, 720.0f };
    
    float density = 10.0f;
    float4 col = tex.Sample(smp, floor(input.uv * windowsSize / density) * density / windowsSize);
    return col;
}

�Z�s�A�J���[
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float sepia = 0.1f;
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 gray = float4(grayScale + sepia, grayScale, grayScale - sepia, 1.0f);
    return gray;
}

�R�[���h�J���[
float4 main(VSOutput input) : SV_TARGET
{
    float4 texcolor = tex.Sample(smp, input.uv);
    float cold = 0.2f;
    float grayScale = texcolor.r * 0.299f + texcolor.g * 0.587f + texcolor.b * 0.114f;
    float4 gray = float4(grayScale - cold, grayScale, grayScale + cold, 1.0f);
    return gray;
}

�K�E�V�A���u���[
float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    float totalWeight = 0.0f, _Sigma = 0.005f, _StepWidth = 0.001f;
    float4 col = float4(0.0f, 0.0f, 0.0f, 0.0f);

    for (float py = -_Sigma * 2.0f; py <= _Sigma * 2.0f; py += _StepWidth)
    {
				//[loop]�A[unroll]�͏����Ȃ��Ă���
        for (float px = -_Sigma * 2.0f; px <= _Sigma * 2.0f; px += _StepWidth)
        {
            float2 pickUV = input.uv + float2(px, py);
            float weight = Gaussian(input.uv, pickUV, _Sigma);
            col += saturate(tex.Sample(smp, pickUV)) * weight;
            totalWeight += weight;
        }

    }
    col.rgb = col.rgb / totalWeight;
    //col.a = 1.0f;
    return col;
}


*/