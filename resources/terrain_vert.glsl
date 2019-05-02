#version  330 core

//References:
//http://www.davidcornette.com/glsl/noise.html
//http://mrl.nyu.edu/~perlin/doc/oscar.html#noise
//https://www.redblobgames.com/maps/terrain-from-noise/
//https://blogs.igalia.com/itoral/2016/10/13/opengl-terrain-renderer-rendering-the-terrain-mesh/

layout(location = 0) in vec4 vertPos;

//uniform sampler2D permTexture;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 lightPos;
uniform vec3 cameraPos;

out vec3 fragNor;
out vec3 iLightVec;
out vec3 ViewVec;
out vec3 HVec;
out vec4 pos;
out float water;

void main()
{
    vec3 vertNor = vec3(0, 1, 0);

    gl_Position = P * V * M * vec4(vertPos.xyz, 1.0f);
    pos =  M * vec4(vertPos.xyz, 1.0f);

    water = 0.0;

    fragNor = normalize(vertNor);
    iLightVec = normalize(lightPos - (V * M * vertPos).xyz);
    ViewVec = normalize(cameraPos - (V * M * vec4(normalize(fragNor), 1.0)).xyz);
    HVec = normalize(iLightVec + ViewVec);
}
