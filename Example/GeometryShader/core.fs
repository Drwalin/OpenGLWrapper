#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture1;

void main()
{
	//FragColor = vec4( 0.6, 0.6, 1.0, 1.0 );
	//FragColor = vec4( 1.0, texCoord.x, texCoord.y, texCoord.y );
	FragColor = texture( ourTexture1, texCoord.xy ) * vec4( 1.0, texCoord.x, texCoord.y, texCoord.y );
}