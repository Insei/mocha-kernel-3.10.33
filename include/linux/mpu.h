/*
* Copyright (C) 2012 Invensense, Inc.
* Copyright (c) 2013-2014 NVIDIA CORPORATION.  All rights reserved.
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
*/

#ifndef __MPU_H_
#define __MPU_H_

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/ioctl.h>
#endif

/* Mount maxtices for mount orientation.
 * MTMAT_XXX_CCW_YYY
 *     XXX : mount position. TOP for top and BOT for bottom.
 *     YYY : couter-clockwise rotation angle in degree.
 */
#define MTMAT_TOP_CCW_0		{  1,  0,  0,  0,  1,  0,  0,  0,  1 }
#define MTMAT_TOP_CCW_90	{  0, -1,  0,  1,  0,  0,  0,  0,  1 }
#define MTMAT_TOP_CCW_180	{ -1,  0,  0,  0, -1,  0,  0,  0,  1 }
#define MTMAT_TOP_CCW_270	{  0,  1,  0, -1,  0,  0,  0,  0,  1 }
#define MTMAT_BOT_CCW_0		{ -1,  0,  0,  0,  1,  0,  0,  0, -1 }
#define MTMAT_BOT_CCW_90	{  0, -1,  0, -1,  0,  0,  0,  0, -1 }
#define MTMAT_BOT_CCW_180	{  1,  0,  0,  0, -1,  0,  0,  0, -1 }
#define MTMAT_BOT_CCW_270	{  0,  1,  0,  1,  0,  0,  0,  0, -1 }

enum secondary_slave_type {
	SECONDARY_SLAVE_TYPE_NONE,
	SECONDARY_SLAVE_TYPE_ACCEL,
	SECONDARY_SLAVE_TYPE_COMPASS,
	SECONDARY_SLAVE_TYPE_PRESSURE,

	SECONDARY_SLAVE_TYPE_TYPES
};

enum ext_slave_id {
	ID_INVALID = 0,
	GYRO_ID_MPU3050,
	GYRO_ID_MPU6050A2,
	GYRO_ID_MPU6050B1,
	GYRO_ID_MPU6050B1_NO_ACCEL,
	GYRO_ID_ITG3500,

	ACCEL_ID_LIS331,
	ACCEL_ID_LSM303DLX,
	ACCEL_ID_LIS3DH,
	ACCEL_ID_KXSD9,
	ACCEL_ID_KXTF9,
	ACCEL_ID_BMA150,
	ACCEL_ID_BMA222,
	ACCEL_ID_BMA250,
	ACCEL_ID_ADXL34X,
	ACCEL_ID_MMA8450,
	ACCEL_ID_MMA845X,
	ACCEL_ID_MPU6050,

	COMPASS_ID_AK8963,
	COMPASS_ID_AK8975,
	COMPASS_ID_AK8972,
	COMPASS_ID_AMI30X,
	COMPASS_ID_AMI306,
	COMPASS_ID_YAS529,
	COMPASS_ID_YAS530,
	COMPASS_ID_HMC5883,
	COMPASS_ID_LSM303DLH,
	COMPASS_ID_LSM303DLM,
	COMPASS_ID_MMC314X,
	COMPASS_ID_HSCDTD002B,
	COMPASS_ID_HSCDTD004A,
	COMPASS_ID_MLX90399,
	COMPASS_ID_AK09911,

	PRESSURE_ID_BMP085,
	PRESSURE_ID_BMP280,
};

#define INV_PROD_KEY(ver, rev) (ver * 100 + rev)

#define REGULATOR_CONFIG_ANA	(0x01)
#define REGULATOR_CONFIG_I2C	(0x02)

/**
 * struct mpu_platform_data - Platform data for the mpu driver
 * @int_config:		Bits [7:3] of the int config register.
 * @level_shifter:	0: VLogic, 1: VDD
 * @orientation:	Orientation matrix of the gyroscope
 * @sec_slave_type:     secondary slave device type, can be compass, accel, etc
 * @sec_slave_id:       id of the secondary slave device
 * @secondary_i2c_address: secondary device's i2c address
 * @secondary_orientation: secondary device's orientation matrix
 * @key:                key for MPL library.
 * @regulator_config:   0: no regulator, Bit0:vdd_ana, Bit1:vdd_i2c
 *
 * Contains platform specific information on how to configure the MPU3050 to
 * work on this platform.  The orientation matricies are 3x3 rotation matricies
 * that are applied to the data to rotate from the mounting orientation to the
 * platform orientation.  The values must be one of 0, 1, or -1 and each row and
 * column should have exactly 1 non-zero value.
 */
struct mpu_platform_data {
	__u8 int_config;
	__u8 level_shifter;
	__s8 orientation[9];
	enum secondary_slave_type sec_slave_type;
	enum ext_slave_id sec_slave_id;
	__u16 secondary_i2c_addr;
	__s8 secondary_orientation[9];
	__u8 key[16];
	enum secondary_slave_type aux_slave_type;
	enum ext_slave_id aux_slave_id;
	__u16 aux_i2c_addr;

#ifdef CONFIG_DTS_INV_MPU_IIO
	int (*power_on)(struct mpu_platform_data *);
	int (*power_off)(struct mpu_platform_data *);
	struct regulator *vdd_ana;
	struct regulator *vdd_i2c;
	__u8 regulator_config;
#endif
};

<<<<<<< HEAD
=======
/**
 * Expected use of the nvi_mpu_ routines are as follows:
 * - nvi_input_mpu_dev_valid: Use to validate whether a device
 *      is connected to the MPU.
 * - nvi_input_mpu_port_alloc: Request a connection to the
 *      device. If successful, a port number will be returned to
 *      identify the connection.  The port number is then used
 *      for all further communication with the connection.
 * - nvi_input_mpu_port_free: Use to close the port connection.
 * - nvi_input_mpu_enable: Use to enable/disable a port.
 *      The enable and FIFO enable is disabled by default so
 *      this will be required after a port is assigned.
 * - nvi_input_mpu_delay_us: Use to set the sampling rate in
 *      microseconds.  The fastest rate of all the enabled MPU
 *      devices will be used that does not exceed the
 *      nvi_input_mpu_delay_ms setting of an enabled device.
 * - nvi_input_mpu_delay_ms: Use to change the port polling
 *      delay at runtime. There is only one HW delay so the
 *      delay used will be the longest delay of all the enabled
 *      ports. This is separate from the sampling rate
 *      (nvi_input_mpu_delay_us).  See function notes below.
 * - nvi_input_mpu_data_out: Use to change the data written at
 *      runtime for ports that are configured as I2C write
 *      transactions.
 * - nvi_input_mpu_bypass request/release: Use to
 *      connect/disconnect the MPU host from the device.  When
 *      bypass is enabled, the connection from the device to the
 *      MPU will then be connected to the host (that the MPU is
 *      connected to). This is a global connection switch
 *      affecting all ports so a mechanism is in place of
 *      whether the request is honored or not.  See the function
 *      notes for nvi_input_mpu_bypass_request.
 */

/**
 * Use to validate a device connected to the MPU I2C master.
 * The function works by doing a single byte read or write to
 * the device and detecting a NACK.  Typically, the call would
 * be set up to read a byte ID of the device.
 * @param struct nvi_mpu_port *nmp
 *           Only the following is needed in nmp:
 *           - addr
 *           - reg
 *           - ctrl
 *           - data_out if a write transaction
 * @param *val: pointer for read data.  Can be NULL if write.
 * @return int error
 *            Possible return value or errors are:
 *            - 0: device is connected to MPU.
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -ENODEV: The device is not connected to the MPU.
 *            - -EINVAL: Problem with input parameters.
 *            - -EIO: The device is connected but responded with
 *                 a NACK.
 */
int nvi_input_mpu_dev_valid(struct nvi_mpu_port *nmp, u8 *data);

/**
 * Request a port.
 * @param struct nvi_mpu_port *nmp
 *           - addr: device I2C address 6:0.
 *                7:7 = 0 if the port is to do writes.
 *                7:7 = 1 if the port is to do reads.
 *           - reg: the starting register to write or read.
 *           - ctrl: number of bytes to read.  Use 1 if port
 *                is configured to do writes.
 *           - data_out: only valid if port is configured to do
 *                writes.
 *           - delay: polling delay
 *           - handler: function to call when data is read. This
 *                should be NULL if the port is configured to do
 *                writes.
 *           - ext_driver: this pointer is passed in handler for
 *                use by external driver.  This should be NULL
 *                if the port is configured for writes.
 * @return int error/port id
 *            if return >= 0 then this is the port ID.  The ID
 *            will have a value of 0 to 3 (HW has 4 ports).
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -ENODEV: A port is not available.  The only way
 *                 to resolve this error is for a port to be
 *                 freed.
 *            - -EINVAL: Problem with input parameters.
 */
int nvi_input_mpu_port_alloc(struct nvi_mpu_port *nmp);

/**
 * Remove a port.
 * @param port
 * @return int error
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -EINVAL: Problem with input parameters.
 */
int nvi_input_mpu_port_free(int port);

/**
 * Enable/disable a port.
 * @param port
 * @param enable
 * @return int error
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -EINVAL: Problem with input parameters.
 */
int nvi_input_mpu_enable(int port, bool enable, bool fifo_enable);

/**
 * Use to change the ports sampling delay in microseconds. The
 * hardware only supports one sampling rate so the shortest time
 * is used among all enabled ports, accelerometer, and gyro. If
 * the requested rate is longer than the actual rate and the
 * port is configured for reads, the data will be reported at
 * the requested rate skipping the data polled at the faster
 * rate.  Setting this to zero causes other enabled devices to
 * determine the sampling rate.  If there are no other enabled
 * devices, then the MPU default rate is used.
 * @param port
 * @param delay_us
 * @return int error
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -EINVAL: Problem with input parameters.
 */
int nvi_input_mpu_delay_us(int port, unsigned long delay_us);

/**
 * Use to change the ports polling delay in milliseconds.
 * A delay value of 0 disables the delay for that port.  The
 * hardware only supports one delay value so the largest request
 * of all the enabled ports is used. The polling delay is in
 * addition to the sampling delay (nvi_input_mpu_delay_us).
 * This is typically used to guarantee a delay after an I2C
 * write to a device to allow the device to process the request
 * and be read by another port before another write at the
 * sampling delay.
 *
 * @param port
 * @param delay_ms
 * @return int error/delay used or 0 if request is to disable
 *            and is successful.
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -EINVAL: Problem with input parameters.
 */
int nvi_input_mpu_delay_ms(int port, u8 delay_ms);

/**
 * Use to change the data written to the sensor.
 * @param port
 * @param data_out is the new data to be written
 * @return int error
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 *            - -EINVAL: Problem with input parameters.
 */
int nvi_input_mpu_data_out(int port, u8 data_out);

/**
 * Enable/disable the MPU bypass mode.  When enabled, the MPU
 * will connect its auxiliary I2C ports to the host.  This is
 * typically used to initialize a device that requires more I2C
 * transactions than the automated port polling can offer.
 * EVERY nvi_input_mpu_bypass_request call must be balanced with
 * a nvi_input_mpu_bypass_release call! A bypass request does
 * not need a following ~enable call.  The release call will
 * automatically handle the correct bypass enable setting. The
 * request locks the bypass setting if successful.  The release
 * unlocks and restores the setting if need be.  Although odd,
 * the purpose of the request call with the enable cleared to
 * false is to allow an external driver to access its device
 * that would normally conflict with a device behind the MPU.
 * Note that this call must not be a permanent solution, i.e.
 * delayed or no release call. When the MPU is in a shutdown
 * state the return error will be
 * -EPERM and bypass will be enabled to allow access from the
 * host to the devices connected to the MPU for their own
 * shutdown needs.
 * @param enable
 * @return int error: calls that return with an error must not
 *            be balanced with a release call.
 *            Possible errors are:
 *            - -EAGAIN: MPU is not initialized yet.
 *            - -EPERM: MPU is shutdown.  MPU API won't be
 *                 available until a system restart.
 *            - -EBUSY: MPU is busy with another request.
 */
int nvi_input_mpu_bypass_request(bool enable);

/**
 * See the nvi_input_mpu_bypass_request notes.
 * @return int 0: Always returns 0.  The call return should be
 *         void but for backward compatibility it returns 0.
 */
int nvi_input_mpu_bypass_release(void);

/**
 * Register the sysfs node from secondary i2c to mpu so that
 * it can be managed together
 */
int nvi_input_mpu_sysfs_register(struct kobject *target, char *name);

>>>>>>> update/master
#endif	/* __MPU_H_ */
