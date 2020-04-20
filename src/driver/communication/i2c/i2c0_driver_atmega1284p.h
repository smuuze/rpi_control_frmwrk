#ifndef _DRIVER_SPECIFIC_I2C
#define _DRIVER_SPECIFIC_I2C


/// Bitrate of I2C bus - needs to be looked over for correct setting before letting loose
#define f_cpu	7372800		// 14745600 without prescaling

#if defined f_cpu && f_cpu < 3600000
	#define I2C_BITRATE()	(TWBR = 0x0A)			// if f_cpu < 3,6 MHz, f_i2c should be reduced too
#elif defined f_cpu && f_cpu == 7372800
		#define I2C_BITRATE()	(TWBR = 0x1C)		// f_cpu = 14,7456 / 2	MHz = 7,3728 MHz ; f_i2c = 0,1 MHz
	#elif defined f_cpu && f_cpu == 14745600
		#define I2C_BITRATE()	(TWBR = 0x0A)		// f_cpu = 14,7456 MHz ; f_i2c = 0,4 MHz
	#else
		#define	I2C_BITRATE()	(TWBR = 0x0A)		// standard minimum setting if not otherwise modified
#endif

//----- BITRATE SETTINGS ---------------

/*
 * TWBR = 0.5 * (f_cpu / f_scl - 16) * 1 / (4^TWPS)
 */

#define I2C_DRIVER_BITRATE_1KHZ_PRESCALER_64			57
#define I2C_DRIVER_BITRATE_10KHZ_PRESCALER_1			90
#define I2C_DRIVER_BITRATE_100KHZ

//----- BITRATE PRESCALER ---------------

#define I2C_DRIVER_BITRATE_PRESCALER_1				0x00
#define I2C_DRIVER_BITRATE_PRESCALER_4				0x01
#define I2C_DRIVER_BITRATE_PRESCALER_16				0x02
#define I2C_DRIVER_BITRATE_PRESCALER_64				0x03


//----- General status codes -----
#define I2C_STATE_WAKEUP					0x01	// -not used yet, since only master mode is implemented- status before the decision of master or slavemode is done
#define I2C_STATE_MASTER_START_COMPLETE			0x08	// Start sent
#define I2C_STATE_MASTER_REPEATED_START_COMPLETE		0x10	// Repeated Start sent
#define I2C_STATE_MASTER_ARBITRATION_LOST			0x38	// Arbitration lost

//----- master transmitter mode (MTM) -----
#define I2C_STATE_MT_SLAW_ACK_RECEIVED			0x18	// SLA+W sent, ACK received
#define I2C_STATE_MT_SLAW_NACK_RECEIVED			0x20	// SLA+W sent, NACK received
#define I2C_STATE_MT_DATA_ACK_RECEIVED			0x28	// DATA sent, ACK received
#define I2C_STATE_MT_DATA_NACK_RECEIVED			0x30	// DATA sent, NACK received

//----- master receiver mode (MRM) -----
#define I2C_STATE_MR_SLAR_ACK_RECEIVED			0x40	// SLA+R sent, ACK received
#define I2C_STATE_MR_SLAR_NACK_RECEIVED			0x48	// SLA+R sent, NACK received
#define I2C_STATE_MR_DATA_BYTE_ACK_RECEIVED		0x50	// DATA received, ACK received
#define I2C_STATE_MR_DATA_BYTE_NACK_RECEIVED		0x58	// DATA received, NACK received

///----- !! status codes for slave modes are not implemented !! -----

//----- Miscellaneous States -----
#define LOCAL_BUS_ERROR		0x00 	// Illegal START or STOP condition has been detected
#define I2C_NO_STATE	0xF8 	// No relevant information available
#define SUCCESSFUL_TRANSFER	0xFF

//----- CONTROL WORDS ---------------

#define I2C_DRIVER_SEND_START_CONDITION()		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA) | (i2c_cfg_control_reg)

#define I2C_DRIVER_SEND_SLAVE_ADDRESS(addr)		TWDR = addr;	\
							TWCR = (1 << TWINT) | (1 << TWEN) | (i2c_cfg_control_reg)

#define I2C_DRIVER_SEND_DATA_BYTE(byte)			TWDR = byte;	\
							TWCR = (1 << TWINT) | (1 << TWEN) | (i2c_cfg_control_reg)

#define I2C_DRIVER_SEND_STOP_CONDITION()		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO) | (i2c_cfg_control_reg)

#define I2C_DRIVER_SEND_ACK()				TWCR = (1 << TWINT) | (1 << TWEN) | (1<<TWEA) | (i2c_cfg_control_reg)

#define I2C_DRIVER_SEND_NACK()				TWCR = (1 << TWINT) | (1 << TWEN) | (i2c_cfg_control_reg)



#define I2C_DRIVER_GET_MODULE_STATUS()			(TWSR & 0xF8)

#define I2C_DRIVER_SET_DATA_REGISTER(byte)		TWDR = byte

#define I2C_DRIVER_GET_DATA_REGISTER()			TWDR


#define I2C_CFGMASK_IRQ_EN				(1 << TWIE)
#define I2C_CFGMASK_ACK_EN				(1 << TWEA)
#define I2C_CFGMASK_GENERAL_CALL_RECOGNITION		(1 << TWGCE)

#define I2C_BUILD_CFG()					static u8 i2c_cfg_control_reg;		\
							static u8 i2c_cfg_slaveaddr_reg;	\
							static u8 i2c_cfg_bitrate_reg;		\
							static u8 i2c_status_reg;


#define I2C_ENABLE_MASTER_MODE()
#define I2C_DISABLE_MASTER_MODE()

#define I2C_SET_PRESCALER(scaler)			i2c_status_reg |= (scaler & 0x03)
#define I2C_SET_BITRATE(bitrate)			i2c_cfg_bitrate_reg = bitrate

#define I2C_BITRATE_1_KHZ()				I2C_SET_PRESCALER(0);	\
							I2C_SET_BITRATE(0)
#define I2C_BITRATE_10_KHZ()				I2C_SET_PRESCALER(0);	\
							I2C_SET_BITRATE(0)
#define I2C_BITRATE_100_KHZ()				I2C_SET_PRESCALER(0);	\
							I2C_SET_BITRATE(0)
#define I2C_BITRATE_1_MHZ()				I2C_SET_PRESCALER(0);	\
							I2C_SET_BITRATE(0)

#define I2C_ENABLE_IRQ()				i2c_cfg_control_reg |= I2C_CFGMASK_IRQ_EN
#define I2C_DISABLE_IRQ()				i2c_cfg_control_reg &= ~I2C_CFGMASK_IRQ_EN

#define I2C_ENABLE_ACK_RESPONSE()			i2c_cfg_control_reg |= I2C_CFGMASK_ACK_EN
#define I2C_DISABLE_ACK_RESPONSE()			i2c_cfg_control_reg &= ~I2C_CFGMASK_ACK_EN

#define I2C_ENABLE_ANSWER_TO_GENERAL_CALL()		i2c_cfg_slaveaddr_reg |= I2C_CFGMASK_GENERAL_CALL_RECOGNITION
#define I2C_DISABLE_ANSWER_TO_GENERAL_CALL()		i2c_cfg_slaveaddr_reg &= ~I2C_CFGMASK_GENERAL_CALL_RECOGNITION

#define I2C_SET_SLAVE_ADDRESS(addr)			i2c_cfg_slaveaddr_reg |= (addr << 1)
#define I2C_DELETE_SLAVE_ADDRESS()			i2c_cfg_slaveaddr_reg &= ~(0xFE)

#define I2C_DRIVER_SET_BITARTE_1KHZ()			i2c_cfg_bitrate_reg = 0
#define I2C_DRIVER_SET_BITARTE_10KHZ()			i2c_cfg_bitrate_reg = 0
#define I2C_DRIVER_SET_BITARTE_100KHZ()			i2c_cfg_bitrate_reg = 0
#define I2C_DRIVER_SET_BITARTE_1MHZ()			i2c_cfg_bitrate_reg = 0

#define I2C_ENABLE_MODULE()				TWAR = i2c_cfg_slaveaddr_reg;							\
							TWBR = i2c_cfg_bitrate_reg;							\
							TWCR = i2c_cfg_control_reg | (1 << TWEN)
#define I2C_RESET_REGISTERS()				i2c_cfg_control_reg = 0;							\
							i2c_cfg_slaveaddr_reg = 0;							\
							i2c_cfg_bitrate_reg = 0;							\
							i2c_status_reg = 0;								\
							TWAR = 0;									\
							TWBR = 0;									\
							TWCR = 0
#define I2C_DISABLE_MODULE()				TWCR |= (1 << TWINT);								\
							TWCR &= (1 << TWEN)
#define I2C_RESET_MODULE()				TWCR &= ~((1 << TWSTO) | (1 << TWEN));						\
							TWCR |= (1 << TWEN);								\
							while ((TWCR & (1 << TWSTO)) != 0)

#define I2C_POWER_DOWN()
#define I2C_POWER_UP()


#endif // _DRIVER_SPECIFIC_I2C
