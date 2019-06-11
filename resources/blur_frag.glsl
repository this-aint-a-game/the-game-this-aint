#version 330 core

// Reference: https://learnopengl.com/Advanced-Lighting/Bloom

out vec4 color;

in vec2 texCoord;
uniform sampler2D texBuf;
uniform float offset[5] = float[]( 0.0, 1.0, 2.0, 3.0, 4.0 );
uniform float weight[5] = float[]( 0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162 );
uniform bool horizontal;

void main()
{
    vec2 tex_offset = 1.0 / textureSize(texBuf, 0); // gets size of single texel
    vec3 result = texture(texBuf, texCoord).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 10; ++i)
        {
            result += texture(texBuf, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(texBuf, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 10; ++i)
        {
            result += texture(texBuf, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(texBuf, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    color = vec4(result, 1.0);
}
