/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <kernel.h>
#include <drivers/sensor.h>
#include <bluetooth/bluetooth.h>

#include "tmp112.h"

#define MANUF_LEN               9
#define THREAD_STACKSIZE        1024
#define THREAD_PRIORITY         7

#define DEVICE_NAME             CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN         (sizeof(DEVICE_NAME) - 1)

static int32_t temperature;
K_MUTEX_DEFINE(temp_mutex);

K_THREAD_STACK_DEFINE(thread_tmp112_stack_area, THREAD_STACKSIZE);
static struct k_thread thread_tmp112_data;

#define ADV_PARAM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
					     BT_GAP_ADV_SLOW_INT_MIN, \
					     BT_GAP_ADV_SLOW_INT_MAX, NULL)

static uint8_t manuf_data[MANUF_LEN] = { 
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, manuf_data, MANUF_LEN)
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(ADV_PARAM, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}
}

void main(void)
{
	int err;

	printk("Starting Blerry test\n");

	const struct device *dev = DEVICE_DT_GET_ANY(ti_tmp112);

	__ASSERT(dev != NULL, "Failed to get device binding");
	__ASSERT(device_is_ready(dev), "Device %s is not ready", dev->name);
	printk("device is %p, name is %s\n", dev, dev->name);

	k_thread_create(&thread_tmp112_data, thread_tmp112_stack_area,
			K_THREAD_STACK_SIZEOF(thread_tmp112_stack_area),
			thread_tmp112, (void *)dev, (void *)&temp_mutex, (void*)&temperature,
			THREAD_PRIORITY, 0, K_FOREVER);
	k_thread_start(&thread_tmp112_data);

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
	printk("Bluetooth initialized\n");

	for (;;) {
		if (k_mutex_lock(&temp_mutex, K_MSEC(100)) == 0) {
			manuf_data[0] = (uint8_t)((uint32_t)temperature & 0xff);
			manuf_data[1] = (uint8_t)(((uint32_t)temperature >> 8) & 0xff);
			k_mutex_unlock(&temp_mutex);
		}
		err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
		if (err) {
			printk("Failed to update advertising data (err %d)\n", err);
		}
		k_sleep(K_MSEC(BT_GAP_ADV_SLOW_INT_MIN));
	}
}
