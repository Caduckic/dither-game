#version 460

// Input vertex attributes (from vertex shader)
// in vec3 fragPosition;
in vec2 fragTexCoord;
// in vec4 fragColor;
// in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform float threshold;
uniform vec2 renderRes;

// Output fragment color
out vec4 finalColor;

// Wacks8 palette
const int numColors = 8;
uniform vec3 palette[numColors];// = vec3[numColors](
//     vec3(0.039, 0.055, 0.2),    // DARK Blue
//     vec3(0.784, 0.137, 0.165),  // Red
//     vec3(0.945, 0.878, 0.22),   // Yellow
//     vec3(0.078, 0.588, 0.298),  // Green
//     vec3(0.549, 0.71, 0.929),   // Cyan
//     vec3(0.169, 0.396, 0.886),  // Blue
//     vec3(0.773, 0.137, 0.675),  // Purple
//     vec3(0.894, 0.859, 0.941)     // LIGHT Blue
// );

const int dither[8][8] = int[8][8](
    int[](0, 32, 8, 40, 2, 34, 10, 42),
    int[](48, 16, 56, 24, 50, 18, 58, 26),
    int[](12, 44,  4, 36, 14, 46,  6, 38),
    int[](60, 28, 52, 20, 62, 30, 54, 22),
    int[](3, 35, 11, 43,  1, 33,  9, 41),
    int[](51, 19, 59, 27, 49, 17, 57, 25),
    int[](15, 47,  7, 39, 13, 45,  5, 37),
    int[](63, 31, 55, 23, 61, 29, 53, 21)
);

float colorDistance(vec3 color1, vec3 color2) {
    return length(color1 - color2);
}

vec4 findClosestColor(vec3 color) {
    float minDist = 1e10;
    int closestIndex = 0;

    float brightness = (color.x + color.y + color.z) / 3.0;

    float brightnessCutoff = 0.2;
    
    for (int i = 0; i < numColors; ++i) {
        float dist = colorDistance(color, palette[i]);

        if (brightness < brightnessCutoff && i == 0) { // DARK Blue
            dist /= 5.0;
        }

        if (dist < minDist) {
            minDist = dist;
            closestIndex = i;
        }
    }

    return vec4(palette[closestIndex], 1.0);  // Return the closest color with full opacity
}

void main()
{
    // I have to flip the texture within the shader due to raylib not liking drawing inverted textures on web
    vec2 flippedTexCoord = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);

    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, flippedTexCoord);

    // float gray = dot(texelColor.rgb, vec3(0.2989, 0.5870, 0.1140));
    
    // Calculate the Bayer matrix index based on scaled texture coordinates
    ivec2 pixelPos = ivec2(flippedTexCoord * renderRes);
    // ivec2 pixelPos = ivec2(gl_FragCoord.xy);
    int bayerValue = dither[pixelPos.y % 8][pixelPos.x % 8];

    // float scaledGray = gray * 63.0;
    vec3 adjustedColor = texelColor.rgb;
    adjustedColor += (bayerValue - 32.0 + threshold) / 128.0;

    finalColor = findClosestColor(adjustedColor);
}