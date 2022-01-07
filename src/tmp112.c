/*
 * Copyright (c) 2016 Firmwave
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <drivers/sensor.h>

void thread_tmp112(void *p1, void *p2, void *p3)
{
	int ret;
	struct sensor_value temp_value;
	struct sensor_value attr;

	const struct device *dev = (const struct device *)p1;
	struct k_mutex *temp_mutex = (struct k_mutex *)p2;
	int32_t *temp = (int32_t *)p3;

	attr.val1 = 150;
	attr.val2 = 0;
	ret = sensor_attr_set(dev, SENSOR_CHAN_AMBIENT_TEMP,
			      SENSOR_ATTR_FULL_SCALE, &attr);
	if (ret) {
		printk("sensor_attr_set failed ret %d\n", ret);
		return;
	}

	attr.val1 = 8;
	attr.val2 = 0;
	ret = sensor_attr_set(dev, SENSOR_CHAN_AMBIENT_TEMP,
			      SENSOR_ATTR_SAMPLING_FREQUENCY, &attr);
	if (ret) {
		printk("sensor_attr_set failed ret %d\n", ret);
		return;
	}

	while (1) {
		ret = sensor_sample_fetch(dev);
		if (ret) {
			printk("sensor_sample_fetch failed ret %d\n", ret);
			return;
		}

		ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_value);
		if (ret) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		if (k_mutex_lock(temp_mutex, K_MSEC(2000)) == 0) {
			*temp = temp_value.val1 * 1E6 + temp_value.val2;
			float ftemp = (*temp + 5000)/10000.0;
			*temp = ftemp;
			k_mutex_unlock(temp_mutex);
		} else {
			printk("Unable to lock mutex\n");
		}
		k_sleep(K_MSEC(2000));
	}
}
