/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "task_management.h"


/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS   1000
#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void taskRun(ZContext *context)
{
	int ret;
	bool led_state = true;
	while (1) {
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0) {
			return;
		}

		led_state = !led_state;
		printf("LED state: %s\n", led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
}

error_e taskStart(ZContext *context)
{
   //Create a task
   context->taskId = zCreateTask("Blinky", (ZTaskCode) taskRun, context,
      &context->taskParams);

   //Unable to create the task?
   if(context->taskId == Z_INVALID_TASK_ID)
      return ERROR_OUT_OF_RESOURCES;

   //Successful processing
   return NO_ERROR;
}

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	printf("Hello World! %s\n", CONFIG_BOARD_TARGET);
	ZContext context;
	error_e error = taskStart(&context);
	if (error != NO_ERROR) {
		printf("taskStart error");
		return 0;
	}
	return 0;
}