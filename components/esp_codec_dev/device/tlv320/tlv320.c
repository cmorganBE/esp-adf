#include "tlv320_codec.h"
#include "tlv320_reg.h"
#include "esp_log.h"
#include "es_common.h"
#include <math.h>

#define TAG     "TLV320_CODEC"

/* The volume register mapped to decibel table can get from codec data-sheet
   Volume control register 0x65 description:
       {0x00 - '0.0dB' ... 0x30 - '24.0db'}
       {0x81 - '-63.5dB' ... 0xFF = '-0.5dB'}
*/
const esp_codec_dev_vol_range_t vol_range = {
    .min_vol =
    {
        .vol = -127,
        .db_value = -63.5,
    },
    .max_vol =
    {
        .vol = 48,
        .db_value = 24.0,
    },
};

static int tlv320_write_reg(audio_codec_tlv320_t *codec, int reg, int value)
{
    return codec->ctrl_if->write_reg(codec->ctrl_if, reg, 1, &value, 1);
}

static int tlv320_read_reg(audio_codec_tlv320_t *codec, int reg, int *value)
{
    *value = 0;
    return codec->ctrl_if->read_reg(codec->ctrl_if, reg, 1, value, 1);
}

static int tlv320_write_reg_check(audio_codec_tlv320_t *codec, int reg, int value) {
    int ret = ESP_CODEC_DEV_OK;
    int value_check;

    ret |= tlv320_write_reg(codec, reg, value);
    ret |= tlv320_read_reg(codec, reg, &value_check);
    if (value_check != value) {
        ESP_LOGE(TAG, "Mismatch in value writen (%d) vs value read (%d) to register (%d)", value, value_check, reg);
    }

    return ret;
}

static int tlv320_open(const audio_codec_if_t *h, void *cfg, int cfg_size)
{
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    tlv320_codec_cfg_t *codec_cfg = (tlv320_codec_cfg_t *) cfg;
    if (codec == NULL || codec_cfg == NULL || cfg_size != sizeof(tlv320_codec_cfg_t) || codec_cfg->ctrl_if == NULL) {
        return ESP_CODEC_DEV_INVALID_ARG;
    }
    int ret = ESP_CODEC_DEV_OK;
    codec->ctrl_if = codec_cfg->ctrl_if;
    codec->gpio_if = codec_cfg->gpio_if;

    // Set register page to 0
    ret |= tlv320_write_reg_check(codec, TLV320_CONTROL_PAGE0_REG00, TLV320_PAGE_0);

    // Initiate SW Reset
    ret |= tlv320_write_reg(codec, TLV320_SOFTWARE_RESET_PAGE0_REG01, 0x01);

    // Program and power up NDAC
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_NDAC_VAL_PAGE0_REG11, 0x88);

    // Program and power up MDAC
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_MDAC_VAL_PAGE0_REG12, 0x82);

    // Program OSR value
    // DOSR = 128, DOSR(9:8) = 0, DOSR(7:0) = 128
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_DOSR_VAL_MSB_PAGE0_REG13, 0x00);
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_DOSR_VAL_LSB_PAGE0_REG14, 0x80);

    // mode is i2s, wordlength is 16, slave mode
    ret |= tlv320_write_reg_check(codec, TLV320_CODEC_INTERFACE_CONTROL_PAGE0_REG27, 00);

    // Program the processing block to be used
    // Select DAC DSP Processing Block PRB_P16
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_PROCESSING_BLOCK_MINIDSP_SELECTION_PAGE0_REG60, 0x10);
    ret |= tlv320_write_reg_check(codec, TLV320_CONTROL_PAGE0_REG00, TLV320_PAGE_8);
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_COEFFICIENT_RAM_CONTROL_PAGE8_REG01, 0x04);

    // Program analog blocks
    // Page 1 is selected
    ret |= tlv320_write_reg_check(codec, TLV320_CONTROL_PAGE8_REG00, TLV320_PAGE_1);

    // Program common-mode voltage (default = 1.35V)
    ret |= tlv320_write_reg_check(codec, TLV320_HEADPHONE_DRIVERS_PAGE1_REG31, 0x04);

    // Program headphone-specific depop settings (in case headphone driver is used)
    // De-pop, Power on = 800 ms, Step time = 4 ms
    ret |= tlv320_write_reg_check(codec, TLV320_HP_OUTPUT_DRIVERS_POP_REMOVAL_SETTINGS_PAGE1_REG33, 0x4e);

    // Program routing of DAC output ot the output amplifier (headphone/lineout or speaker)
    // DAC routed to HPOUT
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_OUTPUT_MIXER_ROUTING_PAGE1_REG35, 0x40);

    // Unmute and set gain of output driver
    // Unmute HPOUT, set gain = 0db
    ret |= tlv320_write_reg_check(codec, TLV320_HPOUT_DRIVER_PAGE1_REG40, 0x06);

    // Unmute Class-D, set gain = 18 db
    ret |= tlv320_write_reg_check(codec, TLV320_CLASS_D_OUTPUT_DRIVER_DRIVER_PAGE1_REG42, 0x1C);

    // HPOUT powered up
    ret |= tlv320_write_reg_check(codec, TLV320_HEADPHONE_DRIVERS_PAGE1_REG31, 0x82);

    // Power-up Class-D drivers
    ret |= tlv320_write_reg_check(codec, TLV320_CLASS_D_SPEAKER_AMP_PAGE1_REG32, 0xC6);

    // Enable HPOUT output analog volume, set = -9 dB
    ret |= tlv320_write_reg_check(codec, TLV320_ANALOG_VOLUME_TO_HPOUT_PAGE1_REG36, 0x92);

    // Enable Class-D output analog volume, set = -9 dB
    ret |= tlv320_write_reg_check(codec, TLV320_ANALOG_VOLUME_TO_CLASS_D_OUTPUT_DRIVER_PAGE1_REG38, 0x92);




    // Page 0 is selected
    ret |= tlv320_write_reg_check(codec, TLV320_CONTROL_PAGE1_REG00, TLV320_PAGE_0);

    // Powerup DAC (soft step disable)
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_DATA_PATH_SETUP_PAGE0_REG63, 0x94);

    // DAC gain = -22 dB
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_VOLUME_CONTROL_PAGE0_REG65, 0xD4);

    // Unmute digital volume control
    // Unmute DAC
    ret |= tlv320_write_reg_check(codec, TLV320_DAC_MUTE_CONTROL_PAGE0_REG64, 0x04);

    if (ret != 0) {
        return ESP_CODEC_DEV_WRITE_FAIL;
    }

    codec->is_open = true;
    return ESP_CODEC_DEV_OK;
}

static int tlv320_set_vol(const audio_codec_if_t *h, float db_value)
{
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    if (codec == NULL) {
        return ESP_CODEC_DEV_INVALID_ARG;
    }
    if (codec->is_open == false) {
        return ESP_CODEC_DEV_WRONG_STATE;
    }

    uint8_t volume = esp_codec_dev_vol_calc_reg(&vol_range, db_value);
    int ret = tlv320_write_reg_check(codec, TLV320_DAC_VOLUME_CONTROL_PAGE0_REG65, volume);
    ESP_LOGD(TAG, "Set volume reg:%x db:%f", volume, db_value);
    return (ret == 0) ? ESP_CODEC_DEV_OK : ESP_CODEC_DEV_WRITE_FAIL;
}

static int tlv320_set_mute(const audio_codec_if_t *h, bool mute)
{
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    if (codec == NULL || codec->is_open == false) {
        return ESP_CODEC_DEV_INVALID_ARG;
    }
    int regv;
    int ret = tlv320_read_reg(codec, TLV320_DAC_MUTE_CONTROL_PAGE0_REG64, &regv);
    if (ret < 0) {
        return ESP_CODEC_DEV_READ_FAIL;
    }
    if (mute) {
        regv = 0x0C;
    } else {
        regv = 0x04;
    }
    return tlv320_write_reg_check(codec, TLV320_DAC_MUTE_CONTROL_PAGE0_REG64, (uint8_t) regv);
}

static int tlv320_set_reg(const audio_codec_if_t *h, int reg, int value)
{
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    if (codec == NULL) {
        return ESP_CODEC_DEV_INVALID_ARG;
    }
    if (codec->is_open == false) {
        return ESP_CODEC_DEV_WRONG_STATE;
    }
    return tlv320_write_reg_check(codec, reg, value);
}

static int tlv320_get_reg(const audio_codec_if_t *h, int reg, int *value)
{
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    if (codec == NULL) {
        return ESP_CODEC_DEV_INVALID_ARG;
    }
    if (codec->is_open == false) {
        return ESP_CODEC_DEV_WRONG_STATE;
    }
    return tlv320_read_reg(codec, reg, value);
}

const audio_codec_if_t *tlv320_codec_new(tlv320_codec_cfg_t *codec_cfg)
{
    if (codec_cfg == NULL || codec_cfg->ctrl_if == NULL) {
        ESP_LOGE(TAG, "Wrong codec config");
        return NULL;
    }
    if (codec_cfg->ctrl_if->is_open(codec_cfg->ctrl_if) == false) {
        ESP_LOGE(TAG, "Control interface not open yet");
        return NULL;
    }
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) calloc(1, sizeof(audio_codec_tlv320_t));
    if (codec == NULL) {
        CODEC_MEM_CHECK(codec);
        return NULL;
    }

    codec->ctrl_if = codec_cfg->ctrl_if;
    codec->base.open = tlv320_open;
    codec->base.set_vol = tlv320_set_vol;
    codec->base.mute = tlv320_set_mute;
    codec->base.set_reg = tlv320_set_reg;
    codec->base.get_reg = tlv320_get_reg;
    codec->hw_gain = esp_codec_dev_col_calc_hw_gain(&codec_cfg->hw_gain);
    do {
        int ret = codec->base.open(&codec->base, codec_cfg, sizeof(tlv320_codec_cfg_t));
        if (ret != 0) {
            ESP_LOGE(TAG, "Open fail");
            break;
        }
        return &codec->base;
    } while (0);
    if (codec) {
        free(codec);
    }
    return NULL;
}
