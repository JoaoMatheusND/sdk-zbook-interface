/*************************************************************************
 * @file zbook_io_uart.h
 *
 * @brief UART backend of the Zbook header pins.
 *
 * @details Routes a pin to its UART interface and enables serial communication.
 *			Used by the pin-function factory (zbook_pin.h).
 *
 * @note It does not track which function a pin holds.
 *
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 13/10/2026
 *
 * @copyright Centro de Inovação EDGE 2026. Todos os direitos reservados.
 *
 ***********************************************************************/
#ifndef ZBOOK_IO_UART_H
#define ZBOOK_IO_UART_H

#include "zbook_pin_cfg.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct zbook_pin;

/**
 * @brief Per-pin UART state kept between init and deinit.
 */
struct zbook_uart_ctx {
	enum zbook_pin_header_uart_role role; /**< Configured role (TX or RX). */
	uint32_t baudrate;                    /**< Configured baud rate. */
};

/**
 * @brief Configures a header pin as one line (TX or RX) of a UART link.
 *
 * @param[in] pin Pin handle; its gpio field is the header pin GPIO number.
 * @param[in] cfg Function config; only cfg->cfg.uart is used. Role, baud rate and framing.
 *
 * @retval 0 Success: the pin is configured as a UART line.
 * @retval -EINVAL Error: NULL argument or invalid configuration.
 */
int zbook_io_uart_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg);

/**
 * @brief Stops the pin's UART line and returns the pin to an unused state.
 *
 * @param[in] pin Pin handle; its gpio field is the header pin GPIO number.
 */
void zbook_io_uart_deinit(struct zbook_pin *pin);

#ifdef __cplusplus
}
#endif

#endif /* ZBOOK_IO_UART_H */