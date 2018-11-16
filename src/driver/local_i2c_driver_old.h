#ifndef _LOCAL_I2C_DRIVER_H_
#define _LOCAL_I2C_DRIVER_H_


#ifndef LOCAL_I2C_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER
#define LOCAL_I2C_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER	64
#endif

#ifndef LOCAL_I2C_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER
#define LOCAL_I2C_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER	64
#endif

#ifndef LOCAL_I2C_DRIVER_MAX_NUMBER_START_RETRY	
#define LOCAL_I2C_DRIVER_MAX_NUMBER_START_RETRY 3
#endif

#define LOCAL_I2C_DRIVER_OP_MODE_MASTER			1
#define LOCAL_I2C_DRIVER_OP_MODE_SLAVE			2

#define LOCAL_I2C_DRIVER_RESPONSE_TO_BROADCAST_ENABLED	(1 << 0)
#define LOCAL_I2C_DRIVER_RESPONSE_TO_BROADCAST_DISABLED	(0)

/*!
 *
 */
typedef struct LOCAL_I2C_DRIVER_CFG {

	u8 op_mode;
	u8 bit_rate;
	u8 slave_own_address;
	u8 slave_broadcast_response;

} LOCAL_I2C_DRIVER_CFG;


/*!
 *
 */
void local_i2c_driver_init(LOCAL_I2C_DRIVER_CFG* p_i2c_cfg);


/*!
 *
 * @param addr
 */
void local_i2c_driver_set_address(u8 addr);


/*!
 *
 */
void local_i2c_driver_power_off(void);


/*!
 *
 * @param byte
 */
void local_i2c_driver_write_byte(u8 byte);


/*!
 *
 * @param word
 */
void local_i2c_driver_write_word(u16 word);


/*!
 *
 * @param long_integer
 */
void local_i2c_driver_write_long(u32 long_integer);


/*!
 *
 * @param num_bytes
 * @param p_buffer_from
 */
void local_i2c_driver_write_N_bytes(u8 num_bytes, u8* p_buffer_from);


/*!
 *
 * @return
 */
u8 local_i2c_driver_bytes_available(void);


/*!
 *
 * @return
 */
u8 local_i2c_driver_read_byte(void);


/*!
 *
 * @return
 */
u16 local_i2c_driver_read_word(void);


/*!
 *
 * @return
 */
u32 local_i2c_driver_read_long(void);


/*!
 *
 * @param num_bytes
 * @param p_buffer_to
 */
void local_i2c_driver_read_N_bytes(u8 num_bytes, u8* p_buffer_to);

#endif // _LOCAL_I2C_DRIVER_H_
