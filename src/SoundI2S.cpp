#include "SoundI2S.hpp"
#include "EspSD.hpp"

void playTask(void *pvParameters);

void SoundI2S::begin() {
    pinMode(LDO_EN_PIN, OUTPUT);
    on();
    pinMode(I2S_MODE_PIN, OUTPUT);
    digitalWrite(I2S_MODE_PIN, HIGH);
    esp_err_t err = i2s_driver_install((i2s_port_t)i2s_num, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("I2S driver install failed: %s\n", esp_err_to_name(err));
    }
    err = i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
    if (err != ESP_OK) {
        Serial.printf("I2S set pin failed: %s\n", esp_err_to_name(err));
    }
}

void SoundI2S::test() {
    const int sample_count = 3084;
    int16_t samples[sample_count];
    double frequency = 1.0;
    const int repeat_count = 200;
    for (int j = 0; j < repeat_count; j++) {
        for (int i = 0; i < sample_count; i++) {
            samples[i] = (int16_t)(sin(2 * PI * frequency * i / sample_rate) * 32767)/5;
        }
        frequency += 10;
        size_t bytes_written = 0;
        esp_err_t err = i2s_write((i2s_port_t)i2s_num, samples, sample_count * sizeof(int16_t), &bytes_written, portMAX_DELAY);
        if (err != ESP_OK) {
            Serial.printf("I2S write failed: %s\n", esp_err_to_name(err));
            break;
        }
    }
}

void SoundI2S::on(){
    digitalWrite(LDO_EN_PIN, HIGH);
    delay(100);
}

void SoundI2S::off(){
    digitalWrite(LDO_EN_PIN, LOW);
    delay(100);
}

void SoundI2S::play(const String &filename, EspSD &sdCard) {
    if (playTaskHandle != NULL) {
        Serial.println("Sound system busy!");
        stop();
    }
    updateState(true, filename);
    PlayTaskParams *params = new PlayTaskParams();
    params->soundI2S = this;
    params->filename = filename;
    params->sdCard = &sdCard;
    xTaskCreatePinnedToCore(playTask, "PlayTask", 10000, params, 1, &playTaskHandle, 0);
}

void SoundI2S::stop() {
    if (playTaskHandle != NULL && shouldStop == false) {
        shouldStop = true;
        delay(100);
        playTaskHandle = NULL;
        shouldStop = false;
    }
    updateState(false, "");
}

float SoundI2S::getVolume() const {
    return volume;
}

void SoundI2S::setVolume(float value) {
    if (value < 0.0) value = 0.0;
    if (value > 1.0) value = 1.0;
    volume = value;
}

String SoundI2S::getState() const {
    return isPlayingState ? "playing" : "idle";
}

String SoundI2S::getCurrentTrack() const {
    return currentTrack;
}

void SoundI2S::updateState(bool playing, String track) {
    isPlayingState = playing;
    currentTrack = track;
}

void playTask(void *pvParameters) {
    PlayTaskParams *params = static_cast<PlayTaskParams *>(pvParameters);
    SoundI2S *soundI2S = params->soundI2S;
    EspSD &sdCard = *(params->sdCard);
    String filename = params->filename;

    soundI2S->shouldStop = false;

    File wavFile = SD.open("/" + filename, FILE_READ);
    if (!wavFile) {
        soundI2S->updateState(false, "");
        soundI2S->playTaskHandle = NULL;
        delete params;
        vTaskDelete(NULL);
        return;
    }

    char wavHeader[44];
    wavFile.readBytes(wavHeader, 44);

    int numChannels = wavHeader[22] | (wavHeader[23] << 8);
    int sampleRate = wavHeader[24] | (wavHeader[25] << 8) | (wavHeader[26] << 16) | (wavHeader[27] << 24);
    int bitsPerSample = wavHeader[34] | (wavHeader[35] << 8);

    soundI2S->on();
    i2s_set_clk((i2s_port_t)soundI2S->i2s_num, sampleRate, (i2s_bits_per_sample_t)bitsPerSample, numChannels == 1 ? I2S_CHANNEL_MONO : I2S_CHANNEL_STEREO);

    const int bufferSize = 1024;
    uint8_t buffer[bufferSize];

    while (wavFile.available() && !soundI2S->shouldStop) {
        int bytesRead = wavFile.read(buffer, bufferSize);
        size_t bytesWritten = 0;
        for (int i = 0; i < bytesRead && !soundI2S->shouldStop; i += 2) {
            int16_t sample = (int16_t)(buffer[i] | (buffer[i + 1] << 8));
            sample = (int16_t)(sample * soundI2S->getVolume());
            buffer[i] = sample & 0xFF;
            buffer[i + 1] = (sample >> 8) & 0xFF;
        }
        if (bytesRead > 0) {
            i2s_write((i2s_port_t)soundI2S->i2s_num, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
        }
    }
    
    soundI2S->off();
    wavFile.close();
    soundI2S->updateState(false, "");
    soundI2S->playTaskHandle = NULL;
    delete params;
    vTaskDelete(NULL);
}
