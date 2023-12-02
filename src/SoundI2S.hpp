#include <Arduino.h>
#include <driver/i2s.h>

class EspSD;

class SoundI2S{
public:
    const int I2S_MODE_PIN = 15;
    const int I2S_DAT = 16;
    const int I2S_BITCLK = 17;
    const int I2S_FRAMECLK = 18;

    const int i2s_num = 0;
    const uint32_t sample_rate = 16000;
    const int bits_per_sample = 16;

    const int LDO_EN_PIN = 12;
    volatile bool shouldStop = false;
    volatile bool soundJobDone = false;
private:

    float volume = 0.2f;

    TaskHandle_t playTaskHandle = NULL;

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_BITCLK, // Bit Clock
        .ws_io_num = I2S_FRAMECLK, // Word Select
        .data_out_num = I2S_DAT, // Data Out
        .data_in_num = -1   //unused
    };
public:
    void begin();
    void test();
    void on();
    void off();
    void play(const String &filename, EspSD &sdCard);
    void stop();

    float getVolume() const;
    void setVolume(float value);
};

struct PlayTaskParams {
    SoundI2S* soundI2S;
    String filename;
    EspSD* sdCard;
};