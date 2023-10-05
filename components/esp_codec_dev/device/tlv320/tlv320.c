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
    ESP_LOGI(TAG, "%s %d(0x%x) to register %d(0x%x)", __FUNCTION__, value, value, reg, reg);
    return codec->ctrl_if->write_reg(codec->ctrl_if, reg, sizeof(uint8_t), &value, sizeof(uint8_t));
}

static int _tlv320_read_reg(audio_codec_tlv320_t *codec, int reg, int *value)
{
    *value = 0;
    return codec->ctrl_if->read_reg(codec->ctrl_if, reg, sizeof(uint8_t), value, sizeof(uint8_t));
}

static int _tlv320_write_reg_check(audio_codec_tlv320_t *codec, int reg, int value) {
    int ret = ESP_CODEC_DEV_OK;
    int value_check;

    ret |= tlv320_write_reg(codec, reg, value);
    ESP_LOGD(TAG, "%s ret 0x%x", __FUNCTION__, ret);
    if(ret != ESP_CODEC_DEV_OK)
    {
        ESP_LOGE(TAG, "%s write reg %d with %d (0x%x)", __FUNCTION__, reg, value, value);
        return ret;
    }

    ret |= _tlv320_read_reg(codec, reg, &value_check);
    ESP_LOGD(TAG, "%s ret 0x%x", __FUNCTION__, ret);
    if(ret != ESP_CODEC_DEV_OK)
    {
        ESP_LOGE(TAG, "%s read reg %d", __FUNCTION__, reg);
        return ret;
    }

    if (value_check != value) {
        ESP_LOGE(TAG, "%s Mismatch in value writen (%d)(0x%x) vs value read (%d)(0x%x) to register (%d)",
                    __FUNCTION__, value, value, value_check, value_check, reg);
    }

    return ret;
}

static int current_page = -1;

static int tlv320_read_reg(audio_codec_tlv320_t *codec, int page, int reg, int *value)
{
    ESP_LOGI(TAG, "%s page %d, reg %d, value %d(0x%x)", __FUNCTION__, page, reg, *value, *value);

    if(page != current_page)
    {
        int ret = _tlv320_write_reg_check(codec, TLV320_CONTROL_PAGE_CONTROL_REGISTER, page);
        if(ret != ESP_CODEC_DEV_OK)
        {
            ESP_LOGE(TAG, "%s _tlv320_write_reg_check failed to write %d(0x%x) to register %d(0x%x)",
                __FUNCTION__, page, page, TLV320_CONTROL_PAGE_CONTROL_REGISTER, TLV320_CONTROL_PAGE_CONTROL_REGISTER);
                return ret;
        }

        current_page = page;
    }

    *value = 0;
    return codec->ctrl_if->read_reg(codec->ctrl_if, reg, sizeof(uint8_t), value, sizeof(uint8_t));
}

static int tlv320_write_reg_check(audio_codec_tlv320_t *codec, int page, int reg, int value)
{
    ESP_LOGI(TAG, "%s page %d, reg %d, value %d(0x%x)", __FUNCTION__, page, reg, value, value);

    if(page != current_page)
    {
        int ret = _tlv320_write_reg_check(codec, TLV320_CONTROL_PAGE_CONTROL_REGISTER, page);
        if(ret != ESP_CODEC_DEV_OK)
        {
            ESP_LOGE(TAG, "%s _tlv320_write_reg_check failed to write %d(0x%x) to register %d(0x%x)",
                __FUNCTION__, page, page, TLV320_CONTROL_PAGE_CONTROL_REGISTER, TLV320_CONTROL_PAGE_CONTROL_REGISTER);
                return ret;
        }

        current_page = page;
    }

    int ret = _tlv320_write_reg_check(codec, reg, value);
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

    // Initiate SW Reset
    ret |= tlv320_write_reg(codec, TLV320_CONTROL_PAGE_CONTROL_REGISTER, TLV320_PAGE_0);
    ret |= tlv320_write_reg(codec, TLV320_SOFTWARE_RESET_PAGE0_REG01, 0x01);

    //////////////////
    // Internal clock configuration
    //
    // Note: NDAC, MDAC, and DOSR settings have to meet several requirements expressed in the
    //       datasheet as equations in order for the codec to work correctly.
    //
    // TODO: calculations for clock registers are based on the output audio sample rate and has to be adjusted
    //       as the output audio sample rate changes.
    //
    // Note: The NDAC, MDAC, and DOSR are configured for 22050 output sample rate (and should work at 22050 and above, 44100 etc)

    // Program and power up NDAC
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_NDAC_VAL_PAGE0_REG11, 0x82);

    // Program and power up MDAC
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_MDAC_VAL_PAGE0_REG12, 0x81);

    // Program OSR value
    // DOSR = 128, DOSR(9:8) = 0, DOSR(7:0) = 128
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_DOSR_VAL_MSB_PAGE0_REG13, 0x00);
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_DOSR_VAL_LSB_PAGE0_REG14, 0x80);

    // mode is i2s, wordlength is 16, slave mode
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_CODEC_INTERFACE_CONTROL_PAGE0_REG27, 00);

    // Program the processing block to be used
    // Select DAC DSP Processing Block PRB_P16
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_PROCESSING_BLOCK_MINIDSP_SELECTION_PAGE0_REG60, 0x04);
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_8, TLV320_DAC_COEFFICIENT_RAM_CONTROL_PAGE8_REG01, 0x04);


    /////////////////////////
    // Program analog blocks

    // Program common-mode voltage of 1.35V
    // Not necessary to set this as the default is 1.35V
//    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_HEADPHONE_DRIVERS_PAGE1_REG31, 0x04);

    // Program headphone-specific depop settings (in case headphone driver is used)
    // De-pop, Power on = 800 ms, Step time = 4 ms
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_HP_OUTPUT_DRIVERS_POP_REMOVAL_SETTINGS_PAGE1_REG33, 0x4e);

    // Program routing of DAC output ot the output amplifier (headphone/lineout or speaker)
    // DAC routed to mixer
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_DAC_OUTPUT_MIXER_ROUTING_PAGE1_REG35, 0x40);

    // Unmute and set gain of output driver
    // Unmute HPOUT, set gain = 0db
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_HPOUT_DRIVER_PAGE1_REG40, 0x06);

    // Unmute Class-D, set gain = 24db (max class-D gain)
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_CLASS_D_OUTPUT_DRIVER_DRIVER_PAGE1_REG42, 0x1C);

    // HPOUT powered up + short circuit detection
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_HEADPHONE_DRIVERS_PAGE1_REG31, 0x86);

    // Power-up Class-D drivers
    // D7   R/W 0 - powered down, 1 - powered up
    // D6-1 R/W 000 011 - write only this value
    // D0   R   0 - short circuit not detected, 1 - short circuit detected
    //
    // 1000 011Z - powered on, reserved, don't care on lower bit -> 0x86
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_CLASS_D_SPEAKER_AMP_PAGE1_REG32, 0x86);

    // Analog volume control to class-D amplifier max gain
    // D7     0 - analog volume output is not routed to class-D, 1 - left channel analog control output is routed to class-D amplifier
    // D6-D0  Analog volume control at maximum (0dB attenuation)
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_ANALOG_VOLUME_TO_CLASS_D_OUTPUT_DRIVER_PAGE1_REG38, 0x80);



    // Powerup DAC (soft step disable)
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_DATA_PATH_SETUP_PAGE0_REG63, 0x94);

    // DAC gain = 0 dB
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_VOLUME_CONTROL_PAGE0_REG65, 0x0);

    // Unmute digital volume control
    // Unmute DAC
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_MUTE_CONTROL_PAGE0_REG64, 0x04);

    if (ret != 0) {
        ESP_LOGI(TAG, "open fail");
        return ESP_CODEC_DEV_WRITE_FAIL;
    }

    codec->is_open = true;
    ESP_LOGI(TAG, "open success");
    return ESP_CODEC_DEV_OK;
}

static int tlv320_set_vol(const audio_codec_if_t *h, float db_value)
{
    ESP_LOGI(TAG, "%s volume to %f", __FUNCTION__, db_value);
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    if (codec == NULL) {
        return ESP_CODEC_DEV_INVALID_ARG;
    }
    if (codec->is_open == false) {
        return ESP_CODEC_DEV_WRONG_STATE;
    }

    uint8_t volume = esp_codec_dev_vol_calc_reg(&vol_range, db_value);
//    volume = 0; // 0dB attenuation in the dac
    int ret = tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_VOLUME_CONTROL_PAGE0_REG65, volume);
    ESP_LOGD(TAG, "Set volume reg:%x db:%f", volume, db_value);
    return (ret == 0) ? ESP_CODEC_DEV_OK : ESP_CODEC_DEV_WRITE_FAIL;
}

static int tlv320_set_mute(const audio_codec_if_t *h, bool mute)
{
    ESP_LOGI(TAG, "%s h %p, mute %d", __FUNCTION__, h, mute);
    audio_codec_tlv320_t *codec = (audio_codec_tlv320_t *) h;
    if (codec == NULL || codec->is_open == false) {
        ESP_LOGE(TAG, "%s invalid arg", __FUNCTION__);
        return ESP_CODEC_DEV_INVALID_ARG;
    }

#if 0
    int regv;
    int ret = tlv320_read_reg(codec, TLV320_DAC_MUTE_CONTROL_PAGE0_REG64, &regv);
    if (ret < 0) {
        return ESP_CODEC_DEV_READ_FAIL;
    }
#endif

    // mute at the class-D amplifier
    int reg42;
    int ret = tlv320_read_reg(codec, TLV320_PAGE_1, TLV320_CLASS_D_OUTPUT_DRIVER_DRIVER_PAGE1_REG42, &reg42);
    if(ret != ESP_CODEC_DEV_OK)
    {
        ESP_LOGE(TAG, "%s read reg 42", __FUNCTION__);
        return ret;
    }

    // mask off the lowest bit, its read-only from the register and indicates that gains are being adjusted
    // so we don't want to attempt to write it back as anything other than 0
    reg42 &= ~(0x1);

    int reg42_new = reg42;
    if(mute)
    {
        reg42_new &= ~(0x04); // clear the 2nd (D2) bit to mute
    } else {
        reg42_new |= 0x04; // set the 2nd (D2) bit to unmute
    }

    ESP_LOGI(TAG, "%s read %d(0x%x), writing %d(0x%x)", __FUNCTION__, reg42, reg42, reg42_new, reg42_new);

    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_1, TLV320_CLASS_D_OUTPUT_DRIVER_DRIVER_PAGE1_REG42, reg42_new);
    if(ret != ESP_CODEC_DEV_OK)
    {
        ESP_LOGE(TAG, "%s writing back reg 42", __FUNCTION__);
        return ret;
    }

    // mute at the dac
    // TODO: do we need this anymore? it looks like not
    int regv;
    if (mute) {
        regv = 0x04 & 0x08;
    } else {
        regv = 0x04;
    }
    ret |= tlv320_write_reg_check(codec, TLV320_PAGE_0, TLV320_DAC_MUTE_CONTROL_PAGE0_REG64, (uint8_t) regv);

    return ret;
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
    return _tlv320_write_reg_check(codec, reg, value);
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
    return _tlv320_read_reg(codec, reg, value);
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
        audio_codec_if_t *retval = &codec->base;
        ESP_LOGI(TAG, "%s %p", __FUNCTION__, retval);
        return retval;
    } while (0);
    if (codec) {
        free(codec);
    }
    return NULL;
}
