#include "Console Game Engine.hpp"
#include <random>

class PerlinNoiseGenerator1D : public CGE::BaseGameEngine {

    bool userCreate() override {
        m_appName = L"Perlin Noise Generator";

        // Init 1D
        m_outputLength1D = m_screenWidth;
        m_noiseSeed1D = std::make_unique<float[]>(m_outputLength1D);
        m_perlinNoise1D = std::make_unique<float[]>(m_outputLength1D);

        // Init 2D
        m_outputWidth2D = m_screenWidth;
        m_outputHeight2D = m_screenHeight;
        m_noiseSeed2D = std::make_unique<float[]>(m_outputWidth2D * m_outputHeight2D);
        m_perlinNoise2D = std::make_unique<float[]>(m_outputWidth2D * m_outputHeight2D);

        // Initializing noise seeds
        createSeed(m_noiseSeed1D, m_outputLength1D);
        createSeed(m_noiseSeed2D, m_outputWidth2D * m_outputHeight2D);

        // Beginning values
        m_numOctaves = 1;
        m_interpolateMethod = InterpolationMethod::Linear;
        m_scalingCoeff = 2.0f;
        m_scalingCoeffDelta = 0.05f;
        m_perlinMode = Mode::m1D;

        return true;
    }

    void handleInput() {
        if (getKey(L'Z').isPressed) {
            switch (m_perlinMode) {
            case Mode::m1D:
                createSeed(m_noiseSeed1D, m_outputLength1D);
                break;

            case Mode::m2D:
                createSeed(m_noiseSeed2D, m_outputWidth2D * m_outputHeight2D);
                break;

            default:
                break;
            }
        }
        if (getKey(L'I').isPressed) {
            m_interpolateMethod = static_cast<InterpolationMethod>((m_interpolateMethod + 1) % InterpolationMethod::NumMethods);
        }
        if (getKey(VK_SPACE).isPressed) {
            // Between 1 and 8
            ++m_numOctaves;
            if (m_numOctaves > 8) {
                m_numOctaves = 1;
            }
        }
        if (getKey(VK_UP).isPressed) {
            m_scalingCoeff += m_scalingCoeffDelta;
        }
        if (getKey(VK_DOWN).isPressed) {
            m_scalingCoeff -= m_scalingCoeffDelta;
            if (m_scalingCoeff < m_scalingCoeffDelta) {
                m_scalingCoeff = m_scalingCoeffDelta;
            }
        }
        if (getKey(L'1').isPressed) {
            m_perlinMode = Mode::m1D;
        }
        if (getKey(L'2').isPressed) {
            m_perlinMode = Mode::m2D;
        }
    }

    bool userUpdate(float elapsedTime) override {
        fill(0, 0, m_screenWidth, m_screenHeight, CGE::Pixel::Empty);

        handleInput();

        switch (m_perlinMode) {
        case Mode::m1D:
            perlinNoise1D(m_outputLength1D, m_numOctaves);
            draw1DNoise();
            break;

        case Mode::m2D:
            perlinNoise2D(m_outputWidth2D, m_outputHeight2D, m_numOctaves);
            draw2DNoise();
            break;

        default:
            break;
        }
        
        return true;
    }

    void createSeed(std::unique_ptr<float[]> &data, int length) {
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_real_distribution<> dist(0.0f, 1.0f);

        for (int i = 0; i < length; ++i) {
            data[i] = (float)(dist(rng));
        }
    }

    float linearInterpolation(float a, float b, float x) {
        return (1.0f - x) * a + x * b;
    }

    float cosineInterpolation(float a, float b, float x) {
        float ft = x * 3.1415927f;
        float f = (1 - std::cosf(ft)) * 0.5f;
        return (1.0f - f) * a + f * b;
    }

    void perlinNoise1D(int count, int numOctaves) {
        // We take randomly generated noise and interpolate it
        // with different amplitudes and wave lengths
        // Sum of created noise waves gives 1D Perlin noise

        for (int x = 0; x < count; ++x) {
            float noise = 0.0f;
            float amplitude = 1.0f;
            float scaleAcc = 0.0f;

            for (int o = 0; o < numOctaves; ++o) {
                // For each octave wave length is half of previous octave
                // That means that frequency gets doubled for each octave
                int waveLength = count >> o;

                // Begin and end point of a wave that contains our x value
                int sample1 = (x / waveLength) * waveLength;
                int sample2 = (sample1 + waveLength) % count;
                
                // Normalising x value in a wave
                float xNorm = (float)(x - sample1) / (float)waveLength;

                // Interpolating to get value of noise wave that matches
                // our x value (xNorm in a wave between sample1 and sample2)
                float sample = 0.0f;
                switch (m_interpolateMethod) {
                case InterpolationMethod::Linear:
                    sample = linearInterpolation(m_noiseSeed1D[sample1], m_noiseSeed1D[sample2], xNorm);
                    break;

                case InterpolationMethod::Cosine:
                    sample = cosineInterpolation(m_noiseSeed1D[sample1], m_noiseSeed1D[sample2], xNorm);
                    break;

                default:
                    break;
                }

                // Sum calculated value with other values in previous octaves
                noise += sample * amplitude;

                // Save amplitude of a wave into accumulator variable
                scaleAcc += amplitude;

                // Decrease wave amplitude for next octave
                amplitude /= m_scalingCoeff;
            }
            // Saving normalised noise value into output
            if (scaleAcc) {
                m_perlinNoise1D[x] = noise / scaleAcc;
            }
        }
    }

    void perlinNoise2D(int width, int height, int numOctaves) {
        // We take randomly generated noise and interpolate it
        // with different amplitudes and wave lengths
        // At first we take two "slices" from our 2D noise
        // to get two 1D noises and then we interpolate between them

        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                float noise = 0.0f;
                float amplitude = 1.0f;
                float scaleAcc = 0.0f;

                for (int o = 0; o < numOctaves; ++o) {
                    // For each octave wave length is half of previous octave
                    // That means that frequency gets doubled for each octave
                    int waveLength = width >> o;

                    // Begin and end point of a wave
                    int sample1X = (x / waveLength) * waveLength;
                    int sample1Y = (y / waveLength) * waveLength;

                    int sample2X = (sample1X + waveLength) % width;
                    int sample2Y = (sample1Y + waveLength) % width;

                    // Normalising x and y value in a wave
                    float xNorm = (float)(x - sample1X) / (float)waveLength;
                    float yNorm = (float)(y - sample1Y) / (float)waveLength;

                    // Interpolating to get value of noise wave
                    // sample1 and sample2 - values from two 1D noise slices
                    // sample - value that we get interpolating between two slices
                    float sample1 = 0.0f;
                    float sample2 = 0.0f;
                    float sample = 0.0f;
                    switch (m_interpolateMethod) {
                    case InterpolationMethod::Linear:
                        // Sampling values in 1D noise
                        sample1 = linearInterpolation(
                            m_noiseSeed2D[sample1Y * width + sample1X], 
                            m_noiseSeed2D[sample1Y * width + sample2X], 
                            xNorm
                        );
                        sample2 = linearInterpolation(
                            m_noiseSeed2D[sample2Y * width + sample1X],
                            m_noiseSeed2D[sample2Y * width + sample2X],
                            xNorm
                        );

                        // Sampling value for 2D noise
                        sample = linearInterpolation(
                            sample1, 
                            sample2, 
                            yNorm
                        );
                        break;

                    case InterpolationMethod::Cosine:
                        // Sampling values in 1D noise
                        sample1 = cosineInterpolation(
                            m_noiseSeed2D[sample1Y * width + sample1X],
                            m_noiseSeed2D[sample1Y * width + sample2X],
                            xNorm
                        );
                        sample2 = cosineInterpolation(
                            m_noiseSeed2D[sample2Y * width + sample1X],
                            m_noiseSeed2D[sample2Y * width + sample2X],
                            xNorm
                        );

                        // Sampling value for 2D noise
                        sample = cosineInterpolation(
                            sample1,
                            sample2,
                            yNorm
                        );
                        break;

                    default:
                        break;
                    }

                    // Sum calculated value with other values in previous octaves
                    noise += sample * amplitude;

                    // Save amplitude of a wave into accumulator variable
                    scaleAcc += amplitude;

                    // Decrease wave amplitude for next octave
                    amplitude /= m_scalingCoeff;
                }
                // Saving normalised noise value into output
                if (scaleAcc) {
                    m_perlinNoise2D[y * width + x] = noise / scaleAcc;
                }
            }
        }
    }

    void draw1DNoise() {
        for (int x = 0; x < m_outputLength1D; ++x) {
            int y = (int)((m_screenHeight / 2.0f) - (m_screenHeight / 2.0f) * m_perlinNoise1D[x]);
            for (int f = y; f < m_screenHeight / 2; ++f) {
                draw((short)x, (short)f, CGE::Pixel::Solid, CGE::Color::FG_Green);
            }
        }
    }

    void draw2DNoise() {
        for (int x = 0; x < m_outputWidth2D; ++x) {
            for (int y = 0; y < m_outputHeight2D; ++y) {
                CHAR_INFO color = getColor(m_perlinNoise2D[y * m_outputWidth2D + x]);
                draw((short)x, (short)y, color.Char.UnicodeChar, color.Attributes);
            }
        }
    }

    CHAR_INFO getColor(float val) {
        CGE::baseColorType bgCol, fgCol;
        CGE::basePixelType pix;
        int pixelBw = (int)(16.0f * val);
        switch (pixelBw) {
        case 0:  bgCol = CGE::Color::BG_DarkBlue; fgCol = CGE::Color::FG_DarkBlue; pix = CGE::Pixel::Solid; break;

        case 1:  bgCol = CGE::Color::BG_DarkBlue; fgCol = CGE::Color::FG_Blue; pix = CGE::Pixel::Quarter; break;
        case 2:  bgCol = CGE::Color::BG_DarkBlue; fgCol = CGE::Color::FG_Blue; pix = CGE::Pixel::Half; break;
        case 3:  bgCol = CGE::Color::BG_DarkBlue; fgCol = CGE::Color::FG_Blue; pix = CGE::Pixel::ThreeQuarters; break;
        case 4:  bgCol = CGE::Color::BG_DarkBlue; fgCol = CGE::Color::FG_Blue; pix = CGE::Pixel::Solid; break;

        case 5:  bgCol = CGE::Color::BG_Blue; fgCol = CGE::Color::FG_Green; pix = CGE::Pixel::Quarter; break;
        case 6:  bgCol = CGE::Color::BG_Blue; fgCol = CGE::Color::FG_Green; pix = CGE::Pixel::Half; break;
        case 7:  bgCol = CGE::Color::BG_Blue; fgCol = CGE::Color::FG_Green; pix = CGE::Pixel::ThreeQuarters; break;
        case 8:  bgCol = CGE::Color::BG_Blue; fgCol = CGE::Color::FG_Green; pix = CGE::Pixel::Solid; break;

        case 9:  bgCol = CGE::Color::BG_Green; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::Quarter; break;
        case 10: bgCol = CGE::Color::BG_Green; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::Half; break;
        case 11: bgCol = CGE::Color::BG_Green; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::ThreeQuarters; break;
        case 12: bgCol = CGE::Color::BG_Green; fgCol = CGE::Color::FG_DarkGrey; pix = CGE::Pixel::Solid; break;

        case 13: bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::Quarter; break;
        case 14: bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::Half; break;
        case 15: bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::ThreeQuarters; break;
        case 16: bgCol = CGE::Color::BG_DarkGrey; fgCol = CGE::Color::FG_White; pix = CGE::Pixel::Solid; break;

        default: bgCol = CGE::Color::BG_Black; fgCol = CGE::Color::FG_Black; pix = CGE::Pixel::Solid; break;
        }

        CHAR_INFO c;
        c.Attributes = bgCol | fgCol;
        c.Char.UnicodeChar = pix;
        return c;
    }

    // 1D Noise parameters
    int m_outputLength1D;
    std::unique_ptr<float[]> m_noiseSeed1D;
    std::unique_ptr<float[]> m_perlinNoise1D;

    //2D Noise parameters
    int m_outputWidth2D;
    int m_outputHeight2D;
    std::unique_ptr<float[]> m_noiseSeed2D;
    std::unique_ptr<float[]> m_perlinNoise2D;

    int m_numOctaves;

    enum InterpolationMethod {
        Linear,
        Cosine,
        // add new members before NumMethods
        NumMethods
    };

    InterpolationMethod m_interpolateMethod;

    // Value used for decreasing amplitude of octaves
    float m_scalingCoeff;
    float m_scalingCoeffDelta;

    enum Mode {
        m1D,
        m2D
    };

    Mode m_perlinMode;

};

int main() {
    PerlinNoiseGenerator1D generator1D;
    if (generator1D.createConsole(256, 256, 3, 3)) {
        generator1D.start();
    }
}