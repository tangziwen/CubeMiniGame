#extension GL_ARB_separate_shader_objects : enable

layout(set = 0, binding = 0) uniform UniformBufferObjectMat {
    vec2 TU_winSize;
    float TU_outlineWidth;
    float TU_depthEpsilon;
} t_shaderUnifom;

layout(set = 0, binding = 1) uniform sampler2D RT_sceneColor;
layout(set = 0, binding = 2) uniform sampler2D RT_outlineMask;
layout(set = 0, binding = 3) uniform sampler2D RT_outlineDepth;
layout(set = 0, binding = 4) uniform sampler2D RT_sceneDepth;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 v_texcoord;
layout(location = 0) out vec4 out_Color;

bool isVisibleOutlineSample(vec2 uv)
{
    vec4 mask = texture(RT_outlineMask, uv);
    if(mask.a <= 0.001)
    {
        return false;
    }

    float outlineDepth = texture(RT_outlineDepth, uv).r;
    float sceneDepth = texture(RT_sceneDepth, uv).r;
    return outlineDepth <= sceneDepth + t_shaderUnifom.TU_depthEpsilon;
}

void main()
{
    vec4 sceneColor = texture(RT_sceneColor, v_texcoord);
    vec4 centerMask = texture(RT_outlineMask, v_texcoord);
    if(centerMask.a > 0.001)
    {
        out_Color = sceneColor;
        return;
    }

    vec2 texel = 1.0 / t_shaderUnifom.TU_winSize;
    int radius = int(clamp(t_shaderUnifom.TU_outlineWidth, 1.0, 4.0));
    vec4 outlineColor = vec4(0.0);
    bool hasOutline = false;

    for(int y = -4; y <= 4; ++y)
    {
        for(int x = -4; x <= 4; ++x)
        {
            if(abs(x) > radius || abs(y) > radius)
            {
                continue;
            }
            if(x == 0 && y == 0)
            {
                continue;
            }

            vec2 sampleUv = v_texcoord + vec2(x, y) * texel;
            if(sampleUv.x < 0.0 || sampleUv.y < 0.0 || sampleUv.x > 1.0 || sampleUv.y > 1.0)
            {
                continue;
            }
            if(isVisibleOutlineSample(sampleUv))
            {
                outlineColor = texture(RT_outlineMask, sampleUv);
                hasOutline = true;
            }
        }
    }

    out_Color = hasOutline ? vec4(outlineColor.rgb, 1.0) : sceneColor;
}
