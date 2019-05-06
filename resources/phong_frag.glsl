#version 330 core 

//to send the color to a frame buffer
layout(location = 0) out vec4 color;

out vec4 outputColor;

in vec3 finalColor;

/* Very simple Diffuse shader with a directional light*/
void main()
{

	outputColor = vec4(finalColor, 1.0);
}
