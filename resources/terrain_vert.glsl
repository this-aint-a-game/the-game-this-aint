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
uniform vec3 cameraPos;

out float water;
out vec4 pos;

void main()
{
    water = 0.0;

    gl_Position = P * V * M * vec4(vertPos.xyz, 1.0f);
    pos =  M * vec4(vertPos.xyz, 1.0f);


}
