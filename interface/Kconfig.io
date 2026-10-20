menu "ZBook Interface - I/O"

config ZBOOK_INTERFACE_IO
    bool "Enable ZBook Interface I/O"
    default n
    help
      Enable the ZBook Interface I/O module.

config ZBOOK_INTERFACE_IO_PINS
    bool "Enable ZBook Interface I/O - runtime pin functions"
    depends on ZBOOK_INTERFACE_IO
    help
      Enable the header pin factory: assign a function (GPIO, UART, PWM, ADC)
      to a header pin at runtime and have it released/reconfigured on change.

    menu "ZBook Interface I/O - runtime pin functions"
      depends on ZBOOK_INTERFACE_IO_PINS

    config ZBOOK_INTERFACE_IO_ADC
      bool "Enable ZBook Interface I/O - ADC"
      default n
      select ADC
      help
        Enable the ZBook Interface I/O - ADC module. This config must be enable using "select CONFIG_ZBOOK_INTERFACE_IO_ADC" in the ADC module Kconfig.

    config ZBOOK_INTERFACE_IO_PWM
      bool "Enable ZBook Interface I/O - PWM"
      select PICOSDK_USE_PWM
      default n
      help
        Enable the ZBook Interface I/O - PWM module. This config must be enable using "select CONFIG_ZBOOK_INTERFACE_IO_PWM" in the PWM module Kconfig.
        

    config ZBOOK_INTERFACE_IO_UART
      bool "Enable ZBook Interface I/O - UART"
      default n
      help
        Enable the ZBook Interface I/O - ADC module. This config must be enable using "select CONFIG_ZBOOK_INTERFACE_IO_UART" in the UART module Kconfig.
        

    config ZBOOK_INTERFACE_IO_GPIO
      bool "Enable ZBook Interface I/O - GPIO"
      default y
    endmenu

endmenu

if ZBOOK_INTERFACE_IO_PINS

module = ZBOOK_INTERFACE_IO_PINS
module-str = zbook pins
source "subsys/logging/Kconfig.template.log_config"

endif # ZBOOK_INTERFACE_IO_PINS
