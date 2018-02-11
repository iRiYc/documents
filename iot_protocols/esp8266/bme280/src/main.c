#include "mgos.h"
#include "mgos_gpio.h"
#include "mgos_i2c.h"
#include "mgos_bme280.h"

static const int led = 2;
static struct mgos_bme280 *s_bme = NULL;

static void bme_timer_cb(void *arg) {
  mgos_gpio_toggle(led);
  double t = mgos_bme280_read_temperature(s_bme);
  double p = mgos_bme280_read_pressure(s_bme);
  double h = mgos_bme280_read_humidity(s_bme);

  if (t <= 0) {
    LOG(LL_INFO, ("Failed to read data from sensor\n"));
    return;
  }
  LOG(LL_INFO, ("Pressure: %f, Temperature: %f *C, Humidity: %f %%", p, t, h));
  (void) arg;
}

/*static void i2c_scanner() {
  uint8_t addr = 0;
  struct mgos_i2c * c = mgos_i2c_get_global();
  
  for (; addr < 0x7F; ++addr) {
	  uint8_t ret = mgos_i2c_read_reg_b(c, addr, 0xD0);
	  if (ret != 0xFF) {
		  LOG(LL_INFO, ("Found in 0x%02x address. Ret: 0x%02x\n", addr, ret));
	  }
  }
  mgos_msleep(500);
}*/

enum mgos_app_init_result mgos_app_init(void) {
  if (!mgos_gpio_set_pull(led, MGOS_GPIO_PULL_UP) ||
      !mgos_gpio_set_mode(led, MGOS_GPIO_MODE_OUTPUT)) {
    return MGOS_APP_INIT_ERROR;
  }
  if ((s_bme = mgos_bme280_i2c_create(0x76)) == NULL) {
	return MGOS_APP_INIT_ERROR;
  }
  mgos_set_timer(1000 /* ms */, true /* repeat */, bme_timer_cb, NULL);
  return MGOS_APP_INIT_SUCCESS;
}
