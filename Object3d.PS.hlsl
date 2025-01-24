#include "object3d.hlsli"

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

/*struct PixelShaderOutput{
    float32_t4 color : SV_TARGET0;
};*/

struct Material {
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
    float32_t shininess;
};
ConstantBuffer<Material> gMaterial : register(b0);
struct PixelShaderOutput {
    float32_t4 color : SV_TARGET0;
};

struct DirectionalLight {
    float32_t4 color; //!< ���C�g�̐F
    float32_t3 direction; //!< ���C�g�̌���
    float intensity; //!< �P�x
};
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct Camera {
    float32_t3 worldPosition;
};
ConstantBuffer<Camera> gCamera : register(b2);

PixelShaderOutput main(VertexShaderOutput input){
    PixelShaderOutput output;
    float32_t4 transformedUV = mul(float3x2_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    if (gMaterial.enableLighting != 0){ // Lighting����ꍇ
        float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
        
        // Phong Reflection Model
        // �v�Z�� R = reflect(L,N) specular = (V.R)n
        float32_t3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float32_t3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
    
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shininess); // ���ˋ��x
        
        // �g�U����
        float32_t3 diffuse = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.color.rgb * cos * gDirectionalLight.intensity;
        
        // ���ʔ���                                                                                      �� ���̂̋��ʔ��˂̐F�B�����ł͔��ɂ��Ă��� material�Őݒ�ł����肷��Ɨǂ�
        float32_t3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow * float32_t3(1.0f, 1.0f, 1.0f);
        
        // �g�U���� + ���ʔ���
        output.color.rgb = diffuse + specular;
        //output.color.rgb = gMaterial.color.rgb * textureColor.rgb * gDirectionalLight.intensity * cos;
        // �A���t�@�͍��܂Œʂ�
        output.color.a = gMaterial.color.a * textureColor.a;
        //output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    } else { // Lighting���Ȃ��ꍇ�B�O��܂łƓ����v�Z
        output.color = gMaterial.color * textureColor;
    }
    output.color = gMaterial.color * textureColor;
    if (output.color.a == 0.0)
    {
        discard;
    }
    return output;
    //output.color = gMaterial.color;
    //float4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    //float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    //output.color = gMaterial.color * textureColor;
    return output;
}