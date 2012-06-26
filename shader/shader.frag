precision lowp float;

varying vec3 vv3colour;

uniform sampler2D textured;
varying vec2 vTexCoord;

void main() 
{
	gl_FragColor = vec4(vv3colour, 1.0) * texture2D(textured, vTexCoord);
}
