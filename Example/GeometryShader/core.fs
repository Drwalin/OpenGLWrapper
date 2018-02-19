#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture1;

void main()
{
	//FragColor = texture( ourTexture1, texCoord );
	//FragColor = vec4( 0.6, 0.6, 1.0, 1.0 );
	FragColor = vec4( texCoord.x, texCoord.y, 1.0, texCoord.y );
}