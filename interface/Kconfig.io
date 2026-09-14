menu "ZBook Interface - I/O"

config BTN
	bool
	default y
	select INPUT

config BTN_LONG_PRESS_MS
	int "Long-press threshold in milliseconds"
	depends on BTN
	default 1000
	help
	  Minimum hold duration, in milliseconds, for a button press to be
	  reported as ZBOOK_BTN_EVT_LONG_PRESSED.

endmenu

module = BTN
module-str = btn
source "subsys/logging/Kconfig.template.log_config"