#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D mainTexture;
uniform vec4 color;

void main()
{
	vec4 texColor = texture(mainTexture, TexCoord);

	// Handle magenta transparency (RGB: 255, 0, 255)
	if (texColor.r >= 0.95 && texColor.b >= 0.95 && texColor.g <= 0.05)
		discard;

	FragColor = texColor * color;
}