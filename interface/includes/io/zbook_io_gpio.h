/*************************************************************************
 * @file zbook_io_gpio.h
 *
 * @brief GPIO backend of the Zbook header pins.
 *
 * @details Configures a pin as a plain GPIO (direction, pull, initial/open-drain state)
 *			and drives or samples it. Used by the pin-function factory (zbook_pin.h).
 *
 * @note It does not track which function a pin holds.
 *
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 09/10/2026
 *
 * @copyright Centro de Inovação EDGE 2026. Todos os direitos reservados.
 *
 ***********************************************************************/
#ifndef ZBOOK_IO_GPIO_H
#define ZBOOK_IO_GPIO_H

#include "zbook_pin_cfg.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct zbook_pin;

/**
 * @brief Per-pin GPIO state kept between init, read/write and deinit.
 */
struct zbook_gpio_ctx {
	enum zbook_pin_header_direction direction; /**< Configured direction. */
	bool open_drain; /**< HIGH releases the line instead of driving it. */
};

/**
 * @brief Configures a header pin as a GPIO.
 *
 * @param[in] pin Pin handle; its gpio field is the header pin GPIO number.
 * @param[in] cfg Function config; only cfg->cfg.gpio is used. Direction, pull and state.
 *
 * @retval 0 Success: the pin is configured as a GPIO.
 * @retval -EINVAL Error: @p pin or @p cfg is NULL, or @p cfg has an out-of-range field.
 */
int zbook_io_gpio_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg);

/**
 * @brief Returns the pin to an unused (input, no pull, no function) state.
 *
 * @param[in] pin Pin handle; its gpio field is the header pin GPIO number.
 */
void zbook_io_gpio_deinit(struct zbook_pin *pin);

/**
 * @brief Drives an output pin.
 *
 * @param[in] pin Pin handle; its gpio field is the header pin GPIO number.
 * @param[in] buf One byte: an enum zbook_pin_header_state.
 * @param[in] len Must be 1.
 *
 * @retval 0 Success: the pin is driven to the specified state.
 * @retval -EINVAL Error: @p len != 1 or the state is out of range.
 * @retval -ENOTSUP Error: the pin is configured as input.
 */
int zbook_io_gpio_write(struct zbook_pin *pin, const void *buf, size_t len);

/**
 * @brief Samples a pin.
 *
 * @param[in] pin Pin handle; its gpio field is the header pin GPIO number.
 * @param[out] buf One byte: ZBOOK_PIN_HEADER_STATE_LOW or ZBOOK_PIN_HEADER_STATE_HIGH.
 * @param[in] len Must be 1.
 *
 * @retval 0 Success: the pin is sampled.
 * @retval -EINVAL Error: @p len != 1.
 */
int zbook_io_gpio_read(struct zbook_pin *pin, void *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* ZBOOK_IO_GPIO_H */
