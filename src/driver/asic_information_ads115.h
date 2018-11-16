#ifndef _LOCAL_ASIC_INFORMATION_ADS115_H_
#define _LOCAL_ASIC_INFORMATION_ADS115_H_

#define ADS1115_NUMBER_OF_CHANNELS						4

#define ADS1115_OPERATION_TIMEOUT_MS						250
#define ADS1115_STARTUP_TIME_MS							25

#define ADS1115_BUS_ADDRESS_01							0x48

#define ADS1115_CONVERSION_REG_ADDR						0x00
#define ADS1115_CONVERSION_RESET_VALUE						0x00

#define ADS1115_CONFIG_REG_ADDR							0x01
#define ADS1115_CONFIG_REG_RESET_VALUE						0x8583

#define ADS1115_CFG_SINGLE_SHOT_OPERATION					(1 << 7)

#define ADS1115_CFG_CHANNEL_INDEX_0						(4 << 4)
#define ADS1115_CFG_CHANNEL_INDEX_1						(5 << 4)
#define ADS1115_CFG_CHANNEL_INDEX_2						(6 << 4)
#define ADS1115_CFG_CHANNEL_INDEX_3						(7 << 4)

#define ADS1115_CFG_GAIN_6144							(0 << 1)
#define ADS1115_CFG_GAIN_4096							(1 << 1)
#define ADS1115_CFG_GAIN_2048							(2 << 1)
#define ADS1115_CFG_GAIN_1024							(3 << 1)
#define ADS1115_CFG_GAIN_0512							(4 << 1)
#define ADS1115_CFG_GAIN_0256							(5 << 1)
#define ADS1115_CFG_GAIN_0256_01						(6 << 1)
#define ADS1115_CFG_GAIN_0256_02						(7 << 1)

#define ADS1115_DATARATE_SPS_08							(0 << 5)
#define ADS1115_DATARATE_SPS_16							(1 << 5)
#define ADS1115_DATARATE_SPS_32							(2 << 5)
#define ADS1115_DATARATE_SPS_64							(3 << 5)
#define ADS1115_DATARATE_SPS_128						(4 << 5)
#define ADS1115_DATARATE_SPS_250						(5 << 5)
#define ADS1115_DATARATE_SPS_475						(6 << 5)
#define ADS1115_DATARATE_SPS_860						(7 << 5)

#define ADS1115_OP_MODE_SINGLE_SHOT						(1 << 0)

#define ADS1115_SET_SINGLE_SHOT_MODE(p_cfg)					p_cfg[1] |= (1 << 7)
#define ADS1115_SET_CHANNEL_INDEX(p_cfg, index)					p_cfg[1] = (p_cfg[1] & ~(0x70)) | (index & 0x70)
#define ADS1115_SET_GAIN(p_cfg, gain)						p_cfg[1] = (p_cfg[1] & ~(0x0E)) | (gain & 0x0E)
#define ADS1115_SET_OP_MODE(p_cfg, mode)					p_cfg[1] = (p_cfg[1] & ~(0x01)) | (mode & 0x01)

#define ADS1115_SET_DATARATE(p_cfg, datarate)					p_cfg[2] = (p_cfg[2] & ~(0xE0)) | (datarate & 0xE0)

#define ADS1115_BUILD_WRITE_CMD(p_buf, reg_addr, value)				p_buf[0] = reg_addr; p_buf[1] = value
#define ADS1115_WRITE_COMMAND_LENGTH						2

#define ADS1115_BUILD_CONFIG_CMD(p_buf)						p_buf[0] = ADS1115_CONFIG_REG_ADDR; p_buf[1] = 0; p_buf[2] = 0
#define ADS1115_CONFIG_COMMAND_LENGTH						3

#define ADS1115_BUILD_MEASUREMENT_CMD(p_buf)					p_buf[0] = ADS1115_CONVERSION_REG_ADDR
#define ADS1115_MEASUREMENT_COMMAND_LENGTH					1

#define ADS1115_BUILD_STATUS_CMD(p_buffer)					p_buf[0] = ADS1115_CONFIG_REG_ADDR
#define ADS1115_STATUS_COMMAND_LENGTH						1
#define ADS1115_STATUS_ANSWER_LENGTH						2

#define ADS1115_MEASUREMENT_ANSWER_LENGTH					2

#define ADS1115_MEASUREMENT_IS_READY(p_buf)					((p_buf[0] & 0x80) != 0 ? 1 : 0)

#endif // _LOCAL_ASIC_INFORMATION_ADS115_H_




