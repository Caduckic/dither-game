#version 460

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform values
uniform sampler2D texture0;

// Output fragment color
out vec4 finalColor;

void main()
{
    // I have to flip the texture within the shader due to raylib not liking drawing inverted textures on web
    vec2 flippedTexCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);

    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, flippedTexCoord);

    // Define the target gray shade (exact match: rgba(0.5, 0.5, 0.5, 1.0))
    vec4 targetGray = vec4(0.5, 0.5, 0.5, 1.0);

    float threshold = 0.1;  // Allowing a small tolerance
    if (abs(texelColor.r - targetGray.r) < threshold &&
        abs(texelColor.g - targetGray.g) < threshold &&
        abs(texelColor.b - targetGray.b) < threshold) {
        finalColor = vec4(0.0, 0.0, 0.0, 0.0);  // Transparent
    } else {
        finalColor = texelColor;
    }

    // finalColor = texelColor;
}