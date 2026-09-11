/*******************************************************************
 * @file zbook_uart.c
 *
 * @brief Implementation of the Zbook UART protocol interface.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 21/08/2026
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/misc/pio_rpi_pico/pio_rpi_pico.h>
#include <zephyr/drivers/uart.h>

#include <hardware/clocks.h>
#include <hardware/pio.h>