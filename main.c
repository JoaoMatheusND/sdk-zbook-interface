/*******************************************************************
 * @file main.c
 *
 * @brief Entry point for the Zbook Interface application: brings up every
 *        interface (LDR, potentiometer, LED, buttons), exposes the `led`
 *        and `btn` shell commands, and periodically logs the sensor
 *        readings and button events.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 *
 * @copyright Copyright (c) 2026
 *
 *******************************************************************/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "actuators/zbook_buzzer.h"
#include "actuators/zbook_led.h"
#include "io/zbook_btn.h"
#include "sensors/zbook_ldr.h"
#include "sensors/zbook_potentiometer.h"

LOG_MODULE_REGISTER(main);

static const char *evt_name(enum zbook_btn_evt evt)
{
	switch (evt) {
	case ZBOOK_BTN_EVT_PRESSED:
		return "pressed";
	case ZBOOK_BTN_EVT_RELEASED:
		return "released";
	case ZBOOK_BTN_EVT_LONG_PRESSED:
		return "long-pressed";
	default:
		return "unknown";
	}
}

static void on_btn_evt(enum zbook_btn btn, enum zbook_btn_evt evt, void *user_data)
{
	ARG_UNUSED(user_data);

	LOG_INF("BTN%d %s", btn, evt_name(evt));
}

int main(void)
{
	int ret;
	uint16_t ldr_level;
	uint16_t pot_level;

	LOG_INF("Zbook Interface Application\n");

	ret = zbook_led_init();
	if (ret < 0) {
		LOG_ERR("zbook_led_init failed (%d)", ret);
		return ret;
	}

	ret = zbook_buzzer_init();
	if (ret < 0) {
		LOG_ERR("zbook_buzzer_init failed (%d)", ret);
		return ret;
	}

	ret = zbook_btn_init();
	if (ret < 0) {
		LOG_ERR("zbook_btn_init failed (%d)", ret);
		return ret;
	}

	ret = zbook_btn_reg_cb(ZBOOK_BTN_ALL, ZBOOK_BTN_EVT_ALL, on_btn_evt, NULL);
	if (ret < 0) {
		LOG_ERR("zbook_btn_reg_cb failed (%d)", ret);
		return ret;
	}

	ret = zbook_ldr_init();
	if (ret < 0) {
		LOG_ERR("zbook_ldr_init failed (%d)", ret);
		return ret;
	}

	ret = zbook_potentiometer_init();
	if (ret < 0) {
		LOG_ERR("zbook_potentiometer_init failed (%d)", ret);
		return ret;
	}

	while (1) {
		ret = zbook_ldr_read(&ldr_level);
		if (ret) {
			LOG_ERR("zbook_ldr_read failed (%d)", ret);
		} else {
			LOG_INF("Light level: %u%%", ldr_level);
		}

		ret = zbook_potentiometer_read(&pot_level);
		if (ret) {
			LOG_ERR("zbook_potentiometer_read failed (%d)", ret);
		} else {
			LOG_INF("Potentiometer level: %u%%", pot_level);
		}

		k_sleep(K_SECONDS(5));
	}

	return 0;
}
