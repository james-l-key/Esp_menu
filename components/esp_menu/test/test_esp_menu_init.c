// Basic Unity test for esp_menu initialization
#include "unity.h"
#include "esp_err.h"
#include "esp_menu.h"

// Integration test: requires configured hardware via menuconfig
TEST_CASE("esp_menu_init returns OK or already initialized", "[esp_menu][hardware]")
{
	esp_err_t err = esp_menu_init();
	TEST_ASSERT_TRUE(err == ESP_OK || err == ESP_ERR_INVALID_STATE);

	/* Attempt to cleanup to satisfy leak checks */
	(void)esp_menu_deinit();
}

