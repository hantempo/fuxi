precision mediump float;

varying float out_light_intensity;
varying vec3 out_color;

void main() 
{
    const vec3 light_color = vec3(0.92, 0.91, 0.98);
    //const vec3 ambient_color = light_color;
    //const float ambient_cont = 0.0;

    vec3 temp = light_color * out_light_intensity;
    //temp += ambient_color * ambient_cont;
	gl_FragColor = vec4(temp, 1.0);
    //gl_FragColor = vec4(out_color, 1.0);
}
