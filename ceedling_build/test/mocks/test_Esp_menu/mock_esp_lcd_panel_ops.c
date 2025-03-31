#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "mock_esp_lcd_panel_ops.h"


static struct mock_esp_lcd_panel_opsInstance
{
  unsigned char placeHolder;
} Mock;

extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void mock_esp_lcd_panel_ops_Verify(void)
{
}

void mock_esp_lcd_panel_ops_Init(void)
{
  mock_esp_lcd_panel_ops_Destroy();
}

void mock_esp_lcd_panel_ops_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

