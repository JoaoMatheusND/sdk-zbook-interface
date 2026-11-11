/*******************************************************************
 * @file main.c
 *
 * @brief I2C target fixture for validating zbook_i2c's external GPIO header.
 * @author Matheus Macário dos Santos (matheus.macario@edge.ufal.br)
 * @version 0.1
 * @date 18/09/2026
 *
 * @copyright Copyright (c) Centro de Inovacao EDGE - 2026
 *
 *******************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/gpio.h>

#define TARGET_ADDR 0x50

static const struct device *const i2c_dev = DEVICE_DT_GET(DT_NODELABEL(i2c0));

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

static uint8_t tx_byte = 0xA5;
static uint8_t rx_buf[32];

static int target_buf_read_requested(struct i2c_target_config *config, uint8_t **ptr, uint32_t *len)
{
	ARG_UNUSED(config);

	printk("I2C TARGET: address matched, master reading -- sending 0x%02x\n", tx_byte);
	*ptr = &tx_byte;
	*len = sizeof(tx_byte);
	return 0;
}

static void target_buf_write_received(struct i2c_target_config *config, uint8_t *ptr, uint32_t len)
{
	ARG_UNUSED(config);

	printk("I2C TARGET: address matched, master wrote %u byte(s):", len);
	for (uint32_t i = 0; i < len && i < sizeof(rx_buf); i++) {
		printk(" 0x%02x", ptr[i]);
	}
	printk("\n");
}

static const struct i2c_target_callbacks target_callbacks = {
	.buf_read_requested = target_buf_read_requested,
	.buf_write_received = target_buf_write_received,
};

static struct i2c_target_config target_cfg = {
	.address = TARGET_ADDR,
	.callbacks = &target_callbacks,
};

int main(void)
{
	printk("zbook i2c target fixture (custom target, addr 0x%02x)\n", TARGET_ADDR);

	if (!device_is_ready(i2c_dev)) {
		printk("i2c0 device not ready\n");
		return 0;
	}

	if (i2c_target_register(i2c_dev, &target_cfg) < 0) {
		printk("failed to register i2c target\n");
		return 0;
	}

	printk("i2c target registered at 0x%02x -- staying up\n", TARGET_ADDR);

	if (gpio_is_ready_dt(&led0)) {
		gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	}

	return 0;
}
