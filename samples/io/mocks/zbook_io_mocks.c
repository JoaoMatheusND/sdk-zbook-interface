/*************************************************************************
 * @file zbook_io_mocks.c
 *
 * @brief Mock implementation of the Zbook I/O backends for the samples.
 *
 * @details Provides zbook_io_{gpio,pwm,adc,uart}_*() without touching any
 * hardware: the state lives in the pin context and every call is logged.
 * Lets the samples build and run while the real backends are not linked in.
 *
 * @note Do not build it together with the real backends (duplicate symbols):
 * drop it from the sample's CMakeLists.txt once they are available.
 *
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 20/10/2026
 *
 * @copyright Centro de Inovação EDGE 2026. Todos os direitos reservados.
 *
 ***********************************************************************/

#include "io/zbook_pin.h"

#include <errno.h>
#include <string.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zbook_io_mocks, LOG_LEVEL_INF);

#define MOCK_ADC_NATIVE_BITS 12
#define MOCK_ADC_STEP        257

/* The mocks only need to remember the last GPIO level; pin->ctx has no field for it. */
static uint8_t gpio_level[ZBOOK_PIN_HEADER_AMOUNT];

static uint8_t *level_of(const struct zbook_pin *pin)
{
	switch (pin->gpio) {
	case ZBOOK_PIN_HEADER_IO_01:
		return &gpio_level[0];
	case ZBOOK_PIN_HEADER_IO_39:
		return &gpio_level[1];
	case ZBOOK_PIN_HEADER_IO_45:
		return &gpio_level[2];
	default:
		return &gpio_level[3];
	}
}

/* GPIO */

int zbook_io_gpio_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg)
{
	if (pin == NULL || cfg == NULL) {
		return -EINVAL;
	}

	pin->ctx.gpio.direction = cfg->cfg.gpio.direction;
	pin->ctx.gpio.open_drain = cfg->cfg.gpio.state == ZBOOK_PIN_HEADER_STATE_OPEN_DRAIN;
	*level_of(pin) = cfg->cfg.gpio.state == ZBOOK_PIN_HEADER_STATE_HIGH
				 ? ZBOOK_PIN_HEADER_STATE_HIGH
				 : ZBOOK_PIN_HEADER_STATE_LOW;

	LOG_INF("[mock] GPIO%d init: %s", pin->gpio,
		pin->ctx.gpio.direction == ZBOOK_PIN_HEADER_DIRECTION_OUTPUT ? "output" : "input");

	return 0;
}

void zbook_io_gpio_deinit(struct zbook_pin *pin)
{
	LOG_INF("[mock] GPIO%d deinit", pin->gpio);
}

int zbook_io_gpio_write(struct zbook_pin *pin, const void *buf, size_t len)
{
	if (len != 1) {
		return -EINVAL;
	}

	if (pin->ctx.gpio.direction != ZBOOK_PIN_HEADER_DIRECTION_OUTPUT) {
		return -ENOTSUP;
	}

	*level_of(pin) = *(const uint8_t *)buf;
	LOG_INF("[mock] GPIO%d write: %u", pin->gpio, *level_of(pin));

	return 0;
}

int zbook_io_gpio_read(struct zbook_pin *pin, void *buf, size_t len)
{
	if (len != 1) {
		return -EINVAL;
	}

	*(uint8_t *)buf = *level_of(pin);
	LOG_INF("[mock] GPIO%d read: %u", pin->gpio, *level_of(pin));

	return 0;
}

/* PWM */

int zbook_io_pwm_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg)
{
	if (pin == NULL || cfg == NULL || cfg->cfg.pwm.frequency == 0 ||
	    cfg->cfg.pwm.duty_cycle > 100) {
		return -EINVAL;
	}

	pin->ctx.pwm.polarity = cfg->cfg.pwm.polarity;
	pin->ctx.pwm.wrap = 0;

	LOG_INF("[mock] GPIO%d PWM init: %u Hz, duty %u%%", pin->gpio, cfg->cfg.pwm.frequency,
		cfg->cfg.pwm.duty_cycle);

	return 0;
}

void zbook_io_pwm_deinit(struct zbook_pin *pin)
{
	LOG_INF("[mock] GPIO%d PWM deinit", pin->gpio);
}

int zbook_io_pwm_set(struct zbook_pin *pin, const void *buf, size_t len)
{
	if (len != 1 || *(const uint8_t *)buf > 100) {
		return -EINVAL;
	}

	LOG_INF("[mock] GPIO%d PWM duty: %u%%", pin->gpio, *(const uint8_t *)buf);

	return 0;
}

/* ADC */

int zbook_io_adc_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg)
{
	if (pin == NULL || cfg == NULL || cfg->cfg.adc.resolution > MOCK_ADC_NATIVE_BITS) {
		return -EINVAL;
	}

	pin->ctx.adc.channel = pin->gpio - 40;
	pin->ctx.adc.resolution =
		cfg->cfg.adc.resolution != 0 ? cfg->cfg.adc.resolution : MOCK_ADC_NATIVE_BITS;

	LOG_INF("[mock] GPIO%d ADC init: %u bits", pin->gpio, pin->ctx.adc.resolution);

	return 0;
}

void zbook_io_adc_deinit(struct zbook_pin *pin)
{
	LOG_INF("[mock] GPIO%d ADC deinit", pin->gpio);
}

int zbook_io_adc_read(struct zbook_pin *pin, void *buf, size_t len)
{
	static uint16_t raw;

	if (len != sizeof(uint16_t)) {
		return -EINVAL;
	}

	/* Ramp through the full scale so consecutive samples differ. */
	raw = (raw + MOCK_ADC_STEP) & (BIT(MOCK_ADC_NATIVE_BITS) - 1);

	uint16_t sample = raw >> (MOCK_ADC_NATIVE_BITS - pin->ctx.adc.resolution);

	memcpy(buf, &sample, sizeof(sample));

	return 0;
}

/* UART */

int zbook_io_uart_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg)
{
	if (pin == NULL || cfg == NULL || cfg->cfg.uart.baudrate == 0) {
		return -EINVAL;
	}

	pin->ctx.uart.role = cfg->cfg.uart.role;
	pin->ctx.uart.baudrate = cfg->cfg.uart.baudrate;

	LOG_INF("[mock] GPIO%d UART init: %s, %u baud", pin->gpio,
		pin->ctx.uart.role == ZBOOK_PIN_HEADER_UART_ROLE_TX ? "TX" : "RX",
		pin->ctx.uart.baudrate);

	return 0;
}

void zbook_io_uart_deinit(struct zbook_pin *pin)
{
	LOG_INF("[mock] GPIO%d UART deinit", pin->gpio);
}
