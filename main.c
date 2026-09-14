/*******************************************************************
 * @file main.c
 *
 * @brief Entry point for the Zbook Interface application.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main);

int main(void)
{
	LOG_INF("Zbook Interface Application\n");

	return 0;
}