precision mediump float;

uniform sampler2D textured;
varying vec2 vTexCoord;

void main() 
{
	gl_FragColor = texture2D(textured, vTexCoord);
}
