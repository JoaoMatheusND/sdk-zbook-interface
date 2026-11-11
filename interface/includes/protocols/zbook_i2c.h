/*******************************************************************
 * @file zbook_i2c.h
 *
 * @brief Defines the interface for the zbook I2C bus.
 * @author Matheus Macário dos Santos (matheus.macario@edge.ufal.br)
 * @version 0.1
 * @date 18/09/2026
 *
 * @copyright Copyright (c) Centro de Inovacao EDGE - 2026
 *
 *******************************************************************/

#ifndef ZBOOK_I2C_H
#define ZBOOK_I2C_H

#include <stddef.h>
#include <stdint.h>

enum zbook_i2c_speed {
	ZBOOK_I2C_SPEED_STANDARD_100K, /**< 100 kHz (I2C_SPEED_STANDARD) */
	ZBOOK_I2C_SPEED_FAST_400K,     /**< 400 kHz (I2C_SPEED_FAST) */
	ZBOOK_I2C_SPEED_FAST_PLUS_1M,  /**< 1 MHz (I2C_SPEED_FAST_PLUS) */
};

/** @brief Runtime-configurable zbook I2C bus parameters. */
struct zbook_i2c_cfg {
	enum zbook_i2c_speed speed; /**< Bus clock speed. */
};

/**
 * @brief Check that the zbook I2C bus is ready to use.
 *
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_init(void);

/**
 * @brief Reconfigure the zbook I2C bus (clock speed).
 *
 * @param cfg Desired configuration. All fields are required.
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_configure(const struct zbook_i2c_cfg *cfg);

/**
 * @brief Write @p len bytes from @p buf to the I2C device at @p addr.
 *
 * @param[in] addr 7-bit address of the target device.
 * @param[in] buf Buffer to send.
 * @param[in] len Number of bytes to send.
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_write(uint16_t addr, const uint8_t *buf, size_t len);

/**
 * @brief Read @p len bytes into @p buf from the I2C device at @p addr.
 *
 * @param[in] addr 7-bit address of the target device.
 * @param[out] buf Buffer to receive into.
 * @param[in] len Number of bytes to receive.
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_read(uint16_t addr, uint8_t *buf, size_t len);

/**
 * @brief Write then read the I2C device at @p addr in a single transaction.
 *
 * @note Unlike zbook_i2c_write() followed by zbook_i2c_read(), the bus is not
 * released between the write and the read (repeated start).
 *
 * @param[in] addr 7-bit address of the target device.
 * @param[in] write_buf Buffer to send.
 * @param[in] write_len Number of bytes to send.
 * @param[out] read_buf Buffer to receive into.
 * @param[in] read_len Number of bytes to receive.
 * @return 0 on success, -errno on error.
 */
int zbook_i2c_write_read(uint16_t addr, const uint8_t *write_buf, size_t write_len,
			 uint8_t *read_buf, size_t read_len);

#endif /* ZBOOK_I2C_H */
