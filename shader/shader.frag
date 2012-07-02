precision mediump float;

varying float out_light_intensity;

void main() 
{
    const vec3 light_color = vec3(0.92, 0.91, 0.98);
    const vec3 ambient_color = light_color;
    const float ambient_percent = 0.2;

    vec3 temp = light_color * out_light_intensity * (1.0 - ambient_percent);
    temp += ambient_color * ambient_percent;
	gl_FragColor = vec4(temp, 1.0);
}
