#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;
uniform vec3 tintColor;  

void main()
{
    vec4 tex = texture(texture1, TexCoord);
    //FragColor = vec4(tex.rgb * tintColor, tex.a);
    FragColor = vec4(TexCoord, 0.0, 1.0);

}
