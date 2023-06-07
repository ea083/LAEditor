#version 330 core
out vec4 FragColor;

in vec3 FragPos;

uniform vec3 cameraPos;
uniform float radius;

void main()
{
    FragColor = vec4(0.0f);
    float lineWidth = 1.0;
    float majorWidth = 0.5;
    float minorWidth = majorWidth / 10.0;
    vec2 derivative = fwidth(FragPos.xz);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    float xLineWidth = lineWidth * minimumx;
    float zLineWidth = lineWidth * minimumz;
    // Minor line marks
    float xMod = mod(FragPos.x, minorWidth);
    float zMod = mod(FragPos.z, minorWidth);
    if(xMod > -xLineWidth && xMod < xLineWidth){
        FragColor = vec4(vec3(0.2), 1.0f);
        FragColor.a = 0.5 * 1/radius;
    }
    if(zMod > -zLineWidth && zMod < zLineWidth){
        FragColor = vec4(vec3(0.2), 1.0f);
        FragColor.a = 0.5 * 1/radius;
    }
    // Major line marks
    xMod = mod(FragPos.x, majorWidth);
    zMod = mod(FragPos.z, majorWidth);
    if(xMod > -xLineWidth && xMod < xLineWidth){
        FragColor = vec4(vec3(0.2), 1.0f);
    }
    if(zMod > -zLineWidth && zMod < zLineWidth){
        FragColor = vec4(vec3(0.2), 1.0f);
    }
    // Axis Line Marks
    if(FragPos.x > -xLineWidth && FragPos.x < xLineWidth){
        FragColor.x = 1.0f;
    }
    if(FragPos.z > -zLineWidth && FragPos.z < zLineWidth){
        FragColor.z = 1.0f;
    }
    FragColor.a *= (50-distance(cameraPos, FragPos))/100;
    if(FragColor.a < 0.1)
        discard;
}