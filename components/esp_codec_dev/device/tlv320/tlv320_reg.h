#ifndef _TLV320_REG_H_
#define _TLV320_REG_H_


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Values taken from the TLV320AIC3120IRHBT Data Sheet
 * https://www.ti.com/product/TLV320AIC3120/part-details/TLV320AIC3120IRHBT
*/


#define TLV320_CONTROL_PAGE_CONTROL_REGISTER                                                              0x00

/**
 * PAGE 0 Defines
*/
#define TLV320_PAGE_0                                                                                     0x00

#define TLV320_SOFTWARE_RESET_PAGE0_REG01                                                                 0X01
#define TLV320_OT_FLAG_PAGE0_REG03                                                                        0X03
#define TLV320_CLOCK_GEN_MUXING_PAGE0_REG04                                                               0x04
#define TLV320_PLL_P_AND_R_VALUES_PAGE0_REG05                                                             0x05
#define TLV320_PLL_J_VALUE_PAGE0_REG06                                                                    0x06
#define TLV320_PLL_D_VALUE_MSB_PAGE0_REG07                                                                0x07
#define TLV320_PLL_D_VALUE_LSB_PAGE0_REG08                                                                0x08
#define TLV320_DAC_NDAC_VAL_PAGE0_REG11                                                                   0x0B
#define TLV320_DAC_MDAC_VAL_PAGE0_REG12                                                                   0x0C
#define TLV320_DAC_DOSR_VAL_MSB_PAGE0_REG13                                                               0x0D
#define TLV320_DAC_DOSR_VAL_LSB_PAGE0_REG14                                                               0x0E
#define TLV320_DAC_IDAC_VAL_PAGE0_REG15                                                                   0x0F
#define TLV320_DAC_MINIDSP_ENGINE_INTERPOLATION_PAGE0_REG16                                               0x10
#define TLV320_ADC_NADC_VAL_PAGE0_REG18                                                                   0x12
#define TLV320_ADC_MADC_VAL_PAGE0_REG19                                                                   0x13
#define TLV320_ADC_AOSR_VAL_PAGE0_REG20                                                                   0x14
#define TLV320_ADC_IADC_VAL_PAGE0_REG21                                                                   0x15
#define TLV320_ADC_MINIDSP_ENGINE_DECIMATION_PAGE0_REG22                                                  0x16
#define TLV320_CLKOUT_MUX_PAGE0_REG25                                                                     0x19
#define TLV320_CLKOUT_M_DIVIDER_VALUE_PAGE0_REG26                                                         0x1A
#define TLV320_CODEC_INTERFACE_CONTROL_PAGE0_REG27                                                        0x1B
#define TLV320_DATA_SLOT_OFFSET_PROGRAMMABILITY_PAGE0_REG28                                               0x1C
#define TLV320_CODEC_INTERFACE_CONTROL_2_PAGE0_REG29                                                      0x1D
#define TLV320_BCLK_N_DIVIDER_VALUE_PAGE0_REG30                                                           0x1E
#define TLV320_CODEC_SECONDARY_INTERFACE_CONTROL_1_PAGE0_REG31                                            0x1F
#define TLV320_CODEC_SECONDARY_INTERFACE_CONTROL_2_PAGE0_REG32                                            0x20
#define TLV320_CODEC_SECONDARY_INTERFACE_CONTROL_3_PAGE0_REG33                                            0x21
#define TLV320_CODEC_I2C_BUS_CONDITION_PAGE0_REG34                                                        0x22
#define TLV320_ADC_FLAG_REGISTER_PAGE0_REG36                                                              0x24
#define TLV320_DAC_FLAG_REGISTER_PAGE0_REG37                                                              0x25
#define TLV320_DAC_PGA_FLAG_REGISTER_PAGE0_REG38                                                          0x26
#define TLV320_OVERFLOW_FLAGS_PAGE0_REG39                                                                 0x27
#define TLV320_INTERRUPT_FLAGS_DAC_PAGE0_REG44                                                            0x2C
#define TLV320_INTERRUPT_FLAGS_ADC_PAGE0_REG45                                                            0x2D
#define TLV320_INTERRUPT_FLAGS_DAC_2_PAGE0_REG46                                                          0x2E
#define TLV320_INTERRUPT_FLAGS_ADC_2_PAGE0_REG47                                                          0x2F
#define TLV320_INT1_CONTROL_REGISTER_PAGE0_REG48                                                          0x30
#define TLV320_INT2_CONTROL_REGISTER_PAGE0_REG49                                                          0x31
#define TLV320_GPIO1_IN_OUT_PIN_CONTROL_PAGE0_REG51                                                       0x33
#define TLV320_DOUT_OUT_PIN_CONTROL_PAGE0_REG53                                                           0x35
#define TLV320_DIN_IN_PIN_CONTROL_PAGE0_REG54                                                             0x36
#define TLV320_DAC_PROCESSING_BLOCK_MINIDSP_SELECTION_PAGE0_REG60                                         0x3C
#define TLV320_ADC_PROCESSING_BLOCK_MINIDSP_SELECTION_PAGE0_REG61                                         0x3D
#define TLV320_PROGRAMMABLE_MINIDSP_INSTRUCTION_MODE_CONTROL_BITS_PAGE0_REG62                             0x3E
#define TLV320_DAC_DATA_PATH_SETUP_PAGE0_REG63                                                            0x3F
#define TLV320_DAC_MUTE_CONTROL_PAGE0_REG64                                                               0x40
#define TLV320_DAC_VOLUME_CONTROL_PAGE0_REG65                                                             0x41
#define TLV320_HEADSET_DETECTION_PAGE0_REG67                                                              0x43
#define TLV320_DRC_CONTROL_1_PAGE0_REG68                                                                  0x44
#define TLV320_DRC_CONTROL_2_PAGE0_REG69                                                                  0x45
#define TLV320_DRC_CONTROL_3_PAGE0_REG70                                                                  0x46
#define TLV320_BEEP_GENERATOR_PAGE0_REG71                                                                 0x47
#define TLV320_BEEP_LENGTH_MSB_PAGE0_REG73                                                                0x49
#define TLV320_BEEP_LENGTH_MIDDLE_BITS_PAGE0_REG74                                                        0x4A
#define TLV320_BEEP_LENGTH_LSB_PAGE0_REG75                                                                0x4B
#define TLV320_BEEP_SIN_MSB_PAGE0_REG76                                                                   0x4C
#define TLV320_BEEP_SIN_LSB_PAGE0_REG77                                                                   0x4D
#define TLV320_BEEP_COS_MSB_PAGE0_REG78                                                                   0x4E
#define TLV320_BEEP_COS_LSB_PAGE0_REG79                                                                   0x4F
#define TLV320_ADC_DIGITAL_MIC_PAGE0_REG81                                                                0x51
#define TLV320_ADC_DIGITAL_VOLUME_CONTROL_FINE_ADJUST_PAGE0_REG82                                         0x52
#define TLV320_ADC_DIGITAL_VOLUME_CONTROL_COARSE_ADJUST_PAGE0_REG83                                       0x53
#define TLV320_AGC_CONTROL_1_PAGE0_REG86                                                                  0x56
#define TLV320_AGC_CONTROL_1_PAGE0_REG87                                                                  0x57
#define TLV320_AGC_MAXIMUM_GAIN_PAGE0_REG88                                                               0x58
#define TLV320_AGC_ATTACK_TIME_PAGE0_REG89                                                                0x59
#define TLV320_AGC_DECAY_TIME_PAGE0_REG90                                                                 0x5A
#define TLV320_AGC_NOISE_DEBOUNCE_PAGE0_REG91                                                             0x5B
#define TLV320_AGC_SIGNAL_DEBOUNCE_PAGE0_REG92                                                            0x5C
#define TLV320_AGC_GAIN_APPLIED_READING_PAGE0_REG93                                                       0x5D
#define TLV320_ADC_DC_MEASUREMENT_1_PAGE0_REG102                                                          0x66
#define TLV320_ADC_DC_MEASUREMENT_2_PAGE0_REG103                                                          0x67
#define TLV320_ADC_DC_MEASUREMENT_OUTPUT1_PAGE0_REG104                                                    0x68
#define TLV320_ADC_DC_MEASUREMENT_OUTPUT2_PAGE0_REG105                                                    0x69
#define TLV320_ADC_DC_MEASUREMENT_OUTPUT3_PAGE0_REG106                                                    0x6A
#define TLV320_VOL_MICSET_PIN_SAR_ADC_VOLUME_CONTROL_PAGE0_REG116                                         0x74
#define TLV320_VOL_MICDET_PIN_GAIN_PAGE0_REG117                                                           0x75

/**
 * PAGE 1 Defines
*/

#define TLV320_PAGE_1                                                                                     0x01

#define TLV320_HEADPHONE_AND_SPEAKER_AMPLIFIER_ERROR_CONTROL_PAGE1_REG30                                  0x1E
#define TLV320_HEADPHONE_DRIVERS_PAGE1_REG31                                                              0X1F
#define TLV320_CLASS_D_SPEAKER_AMP_PAGE1_REG32                                                            0x20
#define TLV320_HP_OUTPUT_DRIVERS_POP_REMOVAL_SETTINGS_PAGE1_REG33                                         0x21
#define TLV320_OUTPUT_DRIVER_PGA_RAMP_DOWN_PERIOD_CONTROL_PAGE1_REG34                                     0x22
#define TLV320_DAC_OUTPUT_MIXER_ROUTING_PAGE1_REG35                                                       0x23
#define TLV320_ANALOG_VOLUME_TO_HPOUT_PAGE1_REG36                                                         0x24
#define TLV320_ANALOG_VOLUME_TO_CLASS_D_OUTPUT_DRIVER_PAGE1_REG38                                         0x26
#define TLV320_HPOUT_DRIVER_PAGE1_REG40                                                                   0x28
#define TLV320_CLASS_D_OUTPUT_DRIVER_DRIVER_PAGE1_REG42                                                   0x2A
#define TLV320_HP_DRIVER_CONTROL_PAGE1_REG44                                                              0x2C
#define TLV320_MICBIAS_PAGE1_REG46                                                                        0x2E
#define TLV320_DELTA_SIGMA_MONO_ADC_CHANNEL_FINE_GAIN_INPUT_SELECTION_FOR_P_TERMINAL_PAGE1_REG48          0x30
#define TLV320_ADC_INPUT_SELECTION_FOR_M_TERMINAL_PAGE1_REG49                                             0x31
#define TLV320_INPUT_CM_SETTINGS_PAGE1_REG50                                                              0x32


/**
 * PAGE 3 Defines
*/

#define TLV320_PAGE_3                                                                                     3

#define TLV320_TIMER_CLOCK_MCLK_DIVIDER_PAGE3_REG16                                                       0x10


/**
 * PAGE 8 Defines
*/

#define TLV320_PAGE_8                                                                                     8

#define TLV320_DAC_COEFFICIENT_RAM_CONTROL_PAGE8_REG01                                                    0x01

/**
 * Refer to documentation for:
 * - PAGE 4: ADC Digital Filter Coefficients
 * - PAGE 5: ADC Programmable Coefficients RAM
 * - PAGE 8: DAC Programmable Coefficients RAM Buffer A (1:63)
 * - PAGE 9: DAC Programmable Coefficients RAM Buffer A (65:127)
 * - PAGE 10: DAC Programmable Coefficients RAM Buffer A (129:191)
 * - PAGE 11: DAC Programmable Coefficients RAM Buffer A (193:255)
 * - PAGE 12: DAC Programmable Coefficients RAM Buffer B (1:63)
 * - PAGE 13: DAC Programmable Coefficients RAM Buffer B (65:127)
 * - PAGE 14: DAC Programmable Coefficients RAM Buffer B (129:191)
 * - PAGE 15: DAC Programmable Coefficients RAM Buffer B (193:255)
 * - PAGE 32: ADC DSP Engine Instruction RAM (0:31)
 * - PAGE 33 - 43: ADC DSP Engine Instruction RAM (32:63)
 * - PAGE 64: DAC DSP Engine Instruction RAM (0:31)
 * - PAGE 65 - 95: DAC DSP Engine Instuction RAM (32:63)
*/


#ifdef __cplusplus
}
#endif
#endif