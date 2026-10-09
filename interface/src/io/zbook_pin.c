/*************************************************************************
 * @file zbook_pin.c
 *
 * @brief Implementation of the Zbook header pin-function factory.
 * @author João Matheus Nascimento Dias (joao.dias@edge.ufal.br)
 * @version 0.1
 * @date 08/10/2026
 *
 * @copyright Centro de Inovação EDGE 2026. Todos os direitos reservados.
 *
 ***********************************************************************/

#include "io/zbook_pin.h"

#include "io/zbook_io_gpio.h"

#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

LOG_MODULE_REGISTER(zbook_pins, CONFIG_ZBOOK_INTERFACE_IO_PINS_LOG_LEVEL);

struct zbook_pin_backend {
	int (*init)(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg);
	void (*deinit)(struct zbook_pin *pin);
	int (*read)(struct zbook_pin *pin, void *buf, size_t len);
	int (*write)(struct zbook_pin *pin, const void *buf, size_t len);
};

/**
 * @brief Array of Zbook header pins.
 *
 */
static struct zbook_pin pins[ZBOOK_PIN_HEADER_AMOUNT] = {
	{.lock = Z_MUTEX_INITIALIZER(pins[0].lock), .gpio = ZBOOK_PIN_HEADER_IO_01}, /**< GPIO1  */
	{.lock = Z_MUTEX_INITIALIZER(pins[1].lock), .gpio = ZBOOK_PIN_HEADER_IO_39}, /**< GPIO39 */
	{.lock = Z_MUTEX_INITIALIZER(pins[2].lock), .gpio = ZBOOK_PIN_HEADER_IO_45}, /**< GPIO45 */
	{.lock = Z_MUTEX_INITIALIZER(pins[3].lock), .gpio = ZBOOK_PIN_HEADER_IO_46}, /**< GPIO46 */
};

/**
 * @brief Macro to create a bitmask for a given function.
 *
 */
#define CAP(_fn) BIT(_fn)

/**
 * @brief Macro to define the capabilities of a Zbook header pin.
 *
 */
#define CAPS_COMMON                                                                                \
	(CAP(ZBOOK_PIN_HEADER_FUNCTION_GPIO) | CAP(ZBOOK_PIN_HEADER_FUNCTION_UART) |               \
	 CAP(ZBOOK_PIN_HEADER_FUNCTION_PWM))

/**
 * @brief Macro to define the capabilities of a Zbook header pin with ADC support.
 *
 */
#define CAPS_ADC (CAPS_COMMON | CAP(ZBOOK_PIN_HEADER_FUNCTION_ADC))

static const uint8_t pin_caps[ZBOOK_PIN_HEADER_AMOUNT] = {
	CAPS_COMMON, /* GPIO1  */
	CAPS_COMMON, /* GPIO39 */
	CAPS_ADC,    /* GPIO45 */
	CAPS_ADC,    /* GPIO46 */
};

static struct zbook_pin *pin_lookup(enum zbook_pin_header gpio)
{
	for (size_t i = 0; i < ARRAY_SIZE(pins); i++) {
		if (pins[i].gpio == gpio) {
			return &pins[i];
		}
	}

	return NULL;
}

static inline bool handle_valid(const struct zbook_pin *handle)
{
	return handle >= pins && handle < pins + ARRAY_SIZE(pins);
}

static int gpio_backend_init(struct zbook_pin *pin, const struct zbook_pin_header_config *cfg)
{
	return zbook_io_gpio_init(pin->gpio, &cfg->gpio, &pin->ctx.gpio);
}

static void gpio_backend_deinit(struct zbook_pin *pin)
{
	zbook_io_gpio_deinit(pin->gpio);
}

static int gpio_backend_write(struct zbook_pin *pin, const void *buf, size_t len)
{
	return zbook_io_gpio_write(pin->gpio, &pin->ctx.gpio, buf, len);
}

static int gpio_backend_read(struct zbook_pin *pin, void *buf, size_t len)
{
	return zbook_io_gpio_read(pin->gpio, &pin->ctx.gpio, buf, len);
}

/**
 * @brief Array of backends for each pin function.
 *
 * @note Currently, only GPIO has a backend implemented. UART, PWM, and ADC backends are not yet
 * implemented.
 */
static const struct zbook_pin_backend backends[] = {
	[ZBOOK_PIN_HEADER_FUNCTION_GPIO] = {gpio_backend_init, gpio_backend_deinit,
					    gpio_backend_read, gpio_backend_write},
};

static const struct zbook_pin_backend *backend_for(enum zbook_pin_header_function function)
{
	if ((size_t)function >= ARRAY_SIZE(backends) || backends[function].init == NULL) {
		return NULL;
	}

	return &backends[function];
}

static void pin_teardown(struct zbook_pin *pin)
{
	if (!pin->configured) {
		return;
	}

	const struct zbook_pin_backend *old = backend_for(pin->function);

	if (old != NULL) {
		old->deinit(pin);
	}

	LOG_DBG("GPIO%u: function %d released", pin->gpio, pin->function);
	pin->configured = false;
}

bool zbook_pin_supports(enum zbook_pin_header gpio, enum zbook_pin_header_function function)
{
	for (size_t i = 0; i < ARRAY_SIZE(pins); i++) {
		if (pins[i].gpio == gpio) {
			return (unsigned int)function < 8 && (pin_caps[i] & CAP(function)) != 0;
		}
	}

	return false;
}

int zbook_pin_set_function(enum zbook_pin_header gpio, const struct zbook_pin_header_config *cfg,
			   struct zbook_pin **handle)
{
	struct zbook_pin *pin = pin_lookup(gpio);

	if (pin == NULL || cfg == NULL) {
		return -EINVAL;
	}

	if (!zbook_pin_supports(gpio, cfg->function)) {
		LOG_ERR("GPIO%u cannot do function %d", gpio, cfg->function);
		return -ENOTSUP;
	}

	const struct zbook_pin_backend *next = backend_for(cfg->function);

	if (next == NULL) {
		LOG_ERR("function %d has no backend", cfg->function);
		return -ENOTSUP;
	}

	k_mutex_lock(&pin->lock, K_FOREVER);

	pin_teardown(pin);

	int ret = next->init(pin, cfg);

	if (ret == 0) {
		pin->function = cfg->function;
		pin->configured = true;
		LOG_DBG("GPIO%u: function %d configured", gpio, cfg->function);

		if (handle != NULL) {
			*handle = pin;
		}
	} else {
		LOG_ERR("GPIO%u: function %d init failed (%d), pin left unconfigured", gpio,
			cfg->function, ret);
	}

	k_mutex_unlock(&pin->lock);

	return ret;
}

int zbook_pin_release(struct zbook_pin *handle)
{
	if (!handle_valid(handle)) {
		return -EINVAL;
	}

	k_mutex_lock(&handle->lock, K_FOREVER);
	pin_teardown(handle);
	k_mutex_unlock(&handle->lock);

	return 0;
}

int zbook_pin_get_function(enum zbook_pin_header gpio, enum zbook_pin_header_function *function)
{
	struct zbook_pin *pin = pin_lookup(gpio);

	if (pin == NULL || function == NULL) {
		return -EINVAL;
	}

	k_mutex_lock(&pin->lock, K_FOREVER);

	int ret = pin->configured ? 0 : -ENODATA;

	if (ret == 0) {
		*function = pin->function;
	}

	k_mutex_unlock(&pin->lock);

	return ret;
}

int zbook_pin_write(struct zbook_pin *handle, const void *buf, size_t len)
{
	if (!handle_valid(handle) || buf == NULL) {
		return -EINVAL;
	}

	k_mutex_lock(&handle->lock, K_FOREVER);

	if (!handle->configured) {
		k_mutex_unlock(&handle->lock);
		return -EINVAL;
	}

	const struct zbook_pin_backend *be = backend_for(handle->function);

	int ret = (be->write == NULL) ? -ENOTSUP : be->write(handle, buf, len);

	k_mutex_unlock(&handle->lock);

	return ret;
}

int zbook_pin_read(struct zbook_pin *handle, void *buf, size_t len)
{
	if (!handle_valid(handle) || buf == NULL) {
		return -EINVAL;
	}

	k_mutex_lock(&handle->lock, K_FOREVER);

	if (!handle->configured) {
		k_mutex_unlock(&handle->lock);
		return -EINVAL;
	}

	const struct zbook_pin_backend *be = backend_for(handle->function);

	int ret = (be->read == NULL) ? -ENOTSUP : be->read(handle, buf, len);

	k_mutex_unlock(&handle->lock);

	return ret;
}
