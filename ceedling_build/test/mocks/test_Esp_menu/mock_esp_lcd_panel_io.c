#include <string.h>
#include <stdlib.h>
#include <setjmp.h>
#include "cmock.h"
#include "mock_esp_lcd_panel_io.h"


static struct mock_esp_lcd_panel_ioInstance
{
  unsigned char placeHolder;
} Mock;

extern int GlobalExpectCount;
extern int GlobalVerifyOrder;

void mock_esp_lcd_panel_io_Verify(void)
{
}

void mock_esp_lcd_panel_io_Init(void)
{
  mock_esp_lcd_panel_io_Destroy();
}

void mock_esp_lcd_panel_io_Destroy(void)
{
  CMock_Guts_MemFreeAll();
  memset(&Mock, 0, sizeof(Mock));
  GlobalExpectCount = 0;
  GlobalVerifyOrder = 0;
}

