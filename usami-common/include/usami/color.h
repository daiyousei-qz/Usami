#pragma once
#include <cstdint>
#include "usami/math/math.h"

namespace usami
{
    struct ColorRGBA
    {
        union
        {
            struct
            {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };

            uint32_t value = 0;
        };

        constexpr ColorRGBA()
        {
        }
        constexpr ColorRGBA(uint32_t cc)
        {
            value = cc;
        }
        constexpr ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
    };

    using SpectrumRGB = Vec3f;

    inline bool InvalidSpectrum(const SpectrumRGB& s) noexcept
    {
        auto subzero  = s[0] < 0 || s[1] < 0 || s[2] < 0;
        auto inf_test = isinf(s[0]) || isinf(s[1]) || isinf(s[2]);
        auto nan_test = isnan(s[0]) || isnan(s[1]) || isnan(s[2]);

        return subzero || inf_test || nan_test;
    }

    inline float GammaCorrect(float u, float gamma) noexcept
    {
        return Pow(u, 1.f / gamma);
    }

    inline float ToneMap_Reinhard(float u) noexcept
    {
        constexpr float middle_gray = 1.f;

        float u_ = u * middle_gray;
        return u_ / (1.f + u_);
    }

    inline float ToneMap_Aces(float u) noexcept
    {
        constexpr float a = 2.51f;
        constexpr float b = 0.03f;
        constexpr float c = 2.43f;
        constexpr float d = 0.59f;
        constexpr float e = 0.14f;

        return (u * (a * u + b)) / (u * (c * u + d) + e);
    }

    inline float Linear2sRGB(float u) noexcept
    {
        if (u <= 0.0031308f)
        {
            return 12.92f * u;
        }
        else
        {
            float v = 1.055f * pow(u, 1.f / 2.4f) - 0.055f;
            return Clamp(v, 0.f, 1.f);
        }
    }

    inline float sRGB2Linear(float u) noexcept
    {
        if (u <= 0.04045f)
        {
            return u * (1.f / 12.92f);
        }
        else
        {
            float v = Pow((u + 0.055f) * (1.f / 1.055f), 2.4f);
            return Clamp(v, 0.f, 1.f);
        }
    }

    inline SpectrumRGB GammaCorrect(const SpectrumRGB& s, float gamma)
    {
        return SpectrumRGB{GammaCorrect(s[0], gamma), GammaCorrect(s[1], gamma),
                           GammaCorrect(s[2], gamma)};
    }

    inline SpectrumRGB ToneMap_Reinhard(SpectrumRGB color) noexcept
    {
        return {ToneMap_Reinhard(color[0]), ToneMap_Reinhard(color[1]), ToneMap_Reinhard(color[2])};
    }

    inline SpectrumRGB ToneMap_Aces(SpectrumRGB color) noexcept
    {
        return {ToneMap_Aces(color[0]), ToneMap_Aces(color[1]), ToneMap_Aces(color[2])};
    }

    inline SpectrumRGB Linear2sRGB(SpectrumRGB color) noexcept
    {
        return {Linear2sRGB(color[0]), Linear2sRGB(color[1]), Linear2sRGB(color[2])};
    }
    inline SpectrumRGB sRGB2Linear(SpectrumRGB color) noexcept
    {
        return {sRGB2Linear(color[0]), sRGB2Linear(color[1]), sRGB2Linear(color[2])};
    }

    // uniformly map RGB values into linear space, NOTE alpha channel is dropped
    inline Vec3f ColorToSpectrumRGB_Linear(ColorRGBA color) noexcept
    {
        static constexpr std::array<float, 256> mapping = {
            0.000000f, 0.003922f, 0.007843f, 0.011765f, 0.015686f, 0.019608f, 0.023529f, 0.027451f,
            0.031373f, 0.035294f, 0.039216f, 0.043137f, 0.047059f, 0.050980f, 0.054902f, 0.058824f,
            0.062745f, 0.066667f, 0.070588f, 0.074510f, 0.078431f, 0.082353f, 0.086275f, 0.090196f,
            0.094118f, 0.098039f, 0.101961f, 0.105882f, 0.109804f, 0.113725f, 0.117647f, 0.121569f,
            0.125490f, 0.129412f, 0.133333f, 0.137255f, 0.141176f, 0.145098f, 0.149020f, 0.152941f,
            0.156863f, 0.160784f, 0.164706f, 0.168627f, 0.172549f, 0.176471f, 0.180392f, 0.184314f,
            0.188235f, 0.192157f, 0.196078f, 0.200000f, 0.203922f, 0.207843f, 0.211765f, 0.215686f,
            0.219608f, 0.223529f, 0.227451f, 0.231373f, 0.235294f, 0.239216f, 0.243137f, 0.247059f,
            0.250980f, 0.254902f, 0.258824f, 0.262745f, 0.266667f, 0.270588f, 0.274510f, 0.278431f,
            0.282353f, 0.286275f, 0.290196f, 0.294118f, 0.298039f, 0.301961f, 0.305882f, 0.309804f,
            0.313725f, 0.317647f, 0.321569f, 0.325490f, 0.329412f, 0.333333f, 0.337255f, 0.341176f,
            0.345098f, 0.349020f, 0.352941f, 0.356863f, 0.360784f, 0.364706f, 0.368627f, 0.372549f,
            0.376471f, 0.380392f, 0.384314f, 0.388235f, 0.392157f, 0.396078f, 0.400000f, 0.403922f,
            0.407843f, 0.411765f, 0.415686f, 0.419608f, 0.423529f, 0.427451f, 0.431373f, 0.435294f,
            0.439216f, 0.443137f, 0.447059f, 0.450980f, 0.454902f, 0.458824f, 0.462745f, 0.466667f,
            0.470588f, 0.474510f, 0.478431f, 0.482353f, 0.486275f, 0.490196f, 0.494118f, 0.498039f,
            0.501961f, 0.505882f, 0.509804f, 0.513725f, 0.517647f, 0.521569f, 0.525490f, 0.529412f,
            0.533333f, 0.537255f, 0.541176f, 0.545098f, 0.549020f, 0.552941f, 0.556863f, 0.560784f,
            0.564706f, 0.568627f, 0.572549f, 0.576471f, 0.580392f, 0.584314f, 0.588235f, 0.592157f,
            0.596078f, 0.600000f, 0.603922f, 0.607843f, 0.611765f, 0.615686f, 0.619608f, 0.623529f,
            0.627451f, 0.631373f, 0.635294f, 0.639216f, 0.643137f, 0.647059f, 0.650980f, 0.654902f,
            0.658824f, 0.662745f, 0.666667f, 0.670588f, 0.674510f, 0.678431f, 0.682353f, 0.686275f,
            0.690196f, 0.694118f, 0.698039f, 0.701961f, 0.705882f, 0.709804f, 0.713725f, 0.717647f,
            0.721569f, 0.725490f, 0.729412f, 0.733333f, 0.737255f, 0.741176f, 0.745098f, 0.749020f,
            0.752941f, 0.756863f, 0.760784f, 0.764706f, 0.768627f, 0.772549f, 0.776471f, 0.780392f,
            0.784314f, 0.788235f, 0.792157f, 0.796078f, 0.800000f, 0.803922f, 0.807843f, 0.811765f,
            0.815686f, 0.819608f, 0.823529f, 0.827451f, 0.831373f, 0.835294f, 0.839216f, 0.843137f,
            0.847059f, 0.850980f, 0.854902f, 0.858824f, 0.862745f, 0.866667f, 0.870588f, 0.874510f,
            0.878431f, 0.882353f, 0.886275f, 0.890196f, 0.894118f, 0.898039f, 0.901961f, 0.905882f,
            0.909804f, 0.913725f, 0.917647f, 0.921569f, 0.925490f, 0.929412f, 0.933333f, 0.937255f,
            0.941176f, 0.945098f, 0.949020f, 0.952941f, 0.956863f, 0.960784f, 0.964706f, 0.968627f,
            0.972549f, 0.976471f, 0.980392f, 0.984314f, 0.988235f, 0.992157f, 0.996078f, 1.000000f,
        };

        return Vec3f{mapping[color.r], mapping[color.g], mapping[color.b]};
    }

    // NOTE colors ranged outside [0, 1] will be trimed
    // make sure to perform tune map for a larger dynamic range
    inline ColorRGBA SpectrumRGBToColor_Linear(Vec3f color) noexcept
    {
        Vec3f cc = (color * 255.f).Clamp(0, 255);
        return ColorRGBA{static_cast<uint8_t>(cc[0]), static_cast<uint8_t>(cc[1]),
                         static_cast<uint8_t>(cc[2])};
    }

    // map RGB values in sRGB space into linear space, NOTE alpha channel is dropped
    inline Vec3f ColorToSpectrumRGB_sRGB(ColorRGBA color) noexcept
    {
        static constexpr std::array<float, 256> mapping = {
            0.000000f, 0.000304f, 0.000607f, 0.000911f, 0.001214f, 0.001518f, 0.001821f, 0.002125f,
            0.002428f, 0.002732f, 0.003035f, 0.003347f, 0.003677f, 0.004025f, 0.004391f, 0.004777f,
            0.005182f, 0.005605f, 0.006049f, 0.006512f, 0.006995f, 0.007499f, 0.008023f, 0.008568f,
            0.009134f, 0.009721f, 0.010330f, 0.010960f, 0.011612f, 0.012286f, 0.012983f, 0.013702f,
            0.014444f, 0.015209f, 0.015996f, 0.016807f, 0.017642f, 0.018500f, 0.019382f, 0.020289f,
            0.021219f, 0.022174f, 0.023153f, 0.024158f, 0.025187f, 0.026241f, 0.027321f, 0.028426f,
            0.029557f, 0.030713f, 0.031896f, 0.033105f, 0.034340f, 0.035601f, 0.036889f, 0.038204f,
            0.039546f, 0.040915f, 0.042311f, 0.043735f, 0.045186f, 0.046665f, 0.048172f, 0.049707f,
            0.051269f, 0.052861f, 0.054480f, 0.056128f, 0.057805f, 0.059511f, 0.061246f, 0.063010f,
            0.064803f, 0.066626f, 0.068478f, 0.070360f, 0.072272f, 0.074214f, 0.076185f, 0.078187f,
            0.080220f, 0.082283f, 0.084376f, 0.086500f, 0.088656f, 0.090842f, 0.093059f, 0.095307f,
            0.097587f, 0.099899f, 0.102242f, 0.104616f, 0.107023f, 0.109462f, 0.111932f, 0.114435f,
            0.116971f, 0.119538f, 0.122139f, 0.124772f, 0.127438f, 0.130136f, 0.132868f, 0.135633f,
            0.138432f, 0.141263f, 0.144128f, 0.147027f, 0.149960f, 0.152926f, 0.155926f, 0.158961f,
            0.162029f, 0.165132f, 0.168269f, 0.171441f, 0.174647f, 0.177888f, 0.181164f, 0.184475f,
            0.187821f, 0.191202f, 0.194618f, 0.198069f, 0.201556f, 0.205079f, 0.208637f, 0.212231f,
            0.215861f, 0.219526f, 0.223228f, 0.226966f, 0.230740f, 0.234551f, 0.238398f, 0.242281f,
            0.246201f, 0.250158f, 0.254152f, 0.258183f, 0.262251f, 0.266356f, 0.270498f, 0.274677f,
            0.278894f, 0.283149f, 0.287441f, 0.291771f, 0.296138f, 0.300544f, 0.304987f, 0.309469f,
            0.313989f, 0.318547f, 0.323143f, 0.327778f, 0.332452f, 0.337164f, 0.341914f, 0.346704f,
            0.351533f, 0.356400f, 0.361307f, 0.366253f, 0.371238f, 0.376262f, 0.381326f, 0.386429f,
            0.391572f, 0.396755f, 0.401978f, 0.407240f, 0.412543f, 0.417885f, 0.423268f, 0.428690f,
            0.434154f, 0.439657f, 0.445201f, 0.450786f, 0.456411f, 0.462077f, 0.467784f, 0.473531f,
            0.479320f, 0.485150f, 0.491021f, 0.496933f, 0.502886f, 0.508881f, 0.514918f, 0.520996f,
            0.527115f, 0.533276f, 0.539479f, 0.545724f, 0.552011f, 0.558340f, 0.564712f, 0.571125f,
            0.577580f, 0.584078f, 0.590619f, 0.597202f, 0.603827f, 0.610496f, 0.617207f, 0.623960f,
            0.630757f, 0.637597f, 0.644480f, 0.651406f, 0.658375f, 0.665387f, 0.672443f, 0.679542f,
            0.686685f, 0.693872f, 0.701102f, 0.708376f, 0.715694f, 0.723055f, 0.730461f, 0.737910f,
            0.745404f, 0.752942f, 0.760525f, 0.768151f, 0.775822f, 0.783538f, 0.791298f, 0.799103f,
            0.806952f, 0.814847f, 0.822786f, 0.830770f, 0.838799f, 0.846873f, 0.854993f, 0.863157f,
            0.871367f, 0.879622f, 0.887923f, 0.896269f, 0.904661f, 0.913099f, 0.921582f, 0.930111f,
            0.938686f, 0.947307f, 0.955973f, 0.964686f, 0.973445f, 0.982251f, 0.991102f, 1.000000f,
        };

        return Vec3f{mapping[color.r], mapping[color.g], mapping[color.b]};
    }

    // NOTE colors ranged outside [0, 1] will be trimed
    // make sure to perform tune map for a larger dynamic range
    inline ColorRGBA SpectrumRGBToColor_sRGB(Vec3f color) noexcept
    {
        Vec3f cc = (Linear2sRGB(color) * 255.f).Clamp(0, 255);
        return ColorRGBA{static_cast<uint8_t>(cc[0]), static_cast<uint8_t>(cc[1]),
                         static_cast<uint8_t>(cc[2])};
    }
} // namespace usami
