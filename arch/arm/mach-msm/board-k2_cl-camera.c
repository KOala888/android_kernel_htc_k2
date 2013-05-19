/* Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <asm/mach-types.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/io.h>
#include <mach/msm_iomap.h>
#include <mach/board.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "board-8930.h"
#include "board-k2_cl.h"

#include <linux/spi/spi.h>

#include "board-mahimahi-flashlight.h"
#ifdef CONFIG_MSM_CAMERA_FLASH
#include <linux/htc_flashlight.h>
#endif

#define CAM_PIN_GPIO_V_CAM_D1V8_EN	MSM_V_CAM_D1V8_EN
#define CAM_PIN_GPIO_V_CAM2_D1V2_EN	MSM_V_CAM2_D1V2_EN
#define CAM_PIN_GPIO_V_CAMIO_1V8_EN	MSM_V_CAMIO_1V8_EN

#define CAM_PIN_GPIO_V_RAW_1V8_EN	MSM_V_RAW_1V8_EN
#define CAM_PIN_GPIO_V_RAW_1V2_EN	MSM_V_RAW_1V2_EN
#define CAM_PIN_GPIO_RAW_RSTN	MSM_RAW_RSTN
#define CAM_PIN_GPIO_RAW_INTR0	MSM_RAW_INTR0
#define CAM_PIN_GPIO_RAW_INTR1	MSM_RAW_INTR1
#define CAM_PIN_GPIO_CAM_MCLK0	MSM_CAM_MCLK0
#define CAM_PIN_GPIO_CAM_MCLK1	MSM_CAM_MCLK1	/*CAMIF_MCLK*/

#define CAM_PIN_GPIO_CAM_I2C_DAT	MSM_CAM_I2C_SDA	/*CAMIF_I2C_DATA*/
#define CAM_PIN_GPIO_CAM_I2C_CLK	MSM_CAM_I2C_SCL	/*CAMIF_I2C_CLK*/

#define CAM_PIN_GPIO_MCAM_SPI_CLK	MSM_MCAM_SPI_CLK
#define CAM_PIN_GPIO_MCAM_SPI_CS0	MSM_MCAM_SPI_CS0
#define CAM_PIN_GPIO_MCAM_SPI_DI	MSM_MCAM_SPI_DI
#define CAM_PIN_GPIO_MCAM_SPI_DO	MSM_MCAM_SPI_DO
#define CAM_PIN_GPIO_CAM_PWDN		MSM_CAM_PWDN
#define CAM_PIN_GPIO_CAM_VCM_PD		MSM_CAM_VCM_PD
#define CAM_PIN_GPIO_CAM2_RSTz		MSM_CAM2_RSTz
#define CAM_PIN_GPIO_CAM2_STANDBY	0
#define CAM_PIN_GPIO_CAM2_SEL		22
#define CAM_PIN_GPIO_CAM_ID			MSM_CAM_ID


static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend - I(L) 8MA*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_DOWN,
		.dir = GPIOMUX_IN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 1 - A FUNC1 8MA*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 2 - O(L) 8MA*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_OUT_LOW,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 3 - A FUNC1 8MA*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 4 - A FUNC2 8MA*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 5 - I(L) 4MA*/
		.drv = GPIOMUX_DRV_4MA,
		.pull = GPIOMUX_PULL_DOWN,
		.dir = GPIOMUX_IN,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 6 - A FUNC2 2MA*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 7 - I(NP) 2MA*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_IN,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 8 - I(L) 2MA*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
		.dir = GPIOMUX_IN,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 9 - O(H) 2MA*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_OUT_HIGH,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 10 - O(L) 2MA*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
		.dir = GPIOMUX_OUT_LOW,
	},
};


static struct msm_gpiomux_config msm8930_cam_common_configs[] = {
	{
		.gpio = CAM_PIN_GPIO_CAM_MCLK0,	// 5
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1], /*A FUNC1 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[2], /*O(L) 8MA*/
		},
	},
	{
		.gpio = CAM_PIN_GPIO_CAM_MCLK1,	// 4
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[4], /*A FUNC2 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[2], /*O(L) 8MA*/
		},
	},

	/* I2C gsbi 4 */
	{
		.gpio = CAM_PIN_GPIO_CAM_I2C_DAT,	// 20
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3], /*A FUNC1 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = CAM_PIN_GPIO_CAM_I2C_CLK,	// 21
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3], /*A FUNC1 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = CAM_PIN_GPIO_RAW_INTR0,	// 69
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[7], /*I(NP) 2MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[8], /*I(L) 2MA*/
		},
	},
	{
		.gpio = CAM_PIN_GPIO_RAW_INTR1,	// 50
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[7], /*I(NP) 2MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[8], /*I(L) 2MA*/
		},
	},
	/* gpio config for Rawchip SPI - gsbi10 */
	{
		.gpio      = CAM_PIN_GPIO_MCAM_SPI_CLK,
		.settings = {
			[GPIOMUX_ACTIVE] = &cam_settings[4], /*A FUNC2 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[2], /*O(L) 8MA*/
		},
	},
	{
		.gpio      = CAM_PIN_GPIO_MCAM_SPI_CS0,
		.settings = {
			[GPIOMUX_ACTIVE] = &cam_settings[6], /*A FUNC2 2MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[10], /* O(L) 2MA*/
		},
	},
	{
		.gpio      = CAM_PIN_GPIO_MCAM_SPI_DI,
		.settings = {
			[GPIOMUX_ACTIVE] = &cam_settings[4], /*A FUNC2 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[0], /* I(L) 8MA*/
		},
	},
	{
		.gpio      = CAM_PIN_GPIO_MCAM_SPI_DO,
		.settings = {
			[GPIOMUX_ACTIVE] = &cam_settings[4], /*A FUNC2 8MA*/
			[GPIOMUX_SUSPENDED] = &cam_settings[2], /*O(L) 8MA*/
		},
	},
};

#ifdef CONFIG_MSM_CAMERA
#define VFE_CAMIF_TIMER1_GPIO 3
#define VFE_CAMIF_TIMER2_GPIO 1

#ifdef CONFIG_MSM_CAMERA_FLASH
/*
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = VFE_CAMIF_TIMER1_GPIO,
	._fsrc.ext_driver_src.led_flash_en = VFE_CAMIF_TIMER2_GPIO,
};
*/
#endif
#endif

static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};


static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 96215040,
		.ib  = 378224640,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 650000000,
		.ib  = 1361968128,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 207747072,
		.ib  = 489756672,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 60318720,
		.ib  = 150796800,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 147045888,
		.ib  = 588183552,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 263678976,
		.ib  = 659197440,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 650000000,
		.ib  = 1271531520,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 239708160,
		.ib  = 599270400,
	},
};

static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

#ifdef CONFIG_RAWCHIP
static int msm8930_use_ext_1v2(void)
{
	return 1;
}

static int msm8930_rawchip_vreg_on(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__);

	/* V_RAW_1V8 */
	rc = gpio_request(CAM_PIN_GPIO_V_RAW_1V8_EN, "V_RAW_1V8_EN");
	pr_info("[CAM] rawchip 1v8 gpio_request, %d rc(%d)\n", CAM_PIN_GPIO_V_RAW_1V8_EN, rc);
	if (rc < 0) //{
		pr_err("[CAM] rawchip on (\"gpio %d\", 1.8V) FAILED", CAM_PIN_GPIO_V_RAW_1V8_EN);
		//goto enable_rawchip_1v8_fail;
	//}
	gpio_direction_output(CAM_PIN_GPIO_V_RAW_1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_RAW_1V8_EN);
	mdelay(1);

	/* V_RAW_1V2 */
	rc = gpio_request(CAM_PIN_GPIO_V_RAW_1V2_EN, "_V_RAW_1V2_EN");
	pr_info("[CAM] rawchip 1v2 gpio_request, %d rc(%d)\n", CAM_PIN_GPIO_V_RAW_1V2_EN, rc);
	if (rc < 0) {
		pr_err("rawchip on (\"gpio %d\", 1.2V) FAILED", CAM_PIN_GPIO_V_RAW_1V2_EN);
		goto enable_rawchip_1v2_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_RAW_1V2_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_RAW_1V2_EN);

	return rc;

enable_rawchip_1v2_fail:
	rc = gpio_request(CAM_PIN_GPIO_V_RAW_1V8_EN, "V_RAW_1V8_EN");
	if (rc < 0)
		pr_err("rawchip off (\"gpio %d\", 1.8V) FAILED", CAM_PIN_GPIO_V_RAW_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_RAW_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_RAW_1V8_EN);
	}
//enable_rawchip_1v8_fail:
	return rc;

}

static int msm8930_rawchip_vreg_off(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__);

	rc = gpio_request(CAM_PIN_GPIO_V_RAW_1V2_EN, "V_RAW_1V2_EN");
	pr_info("[CAM] rawchip 1v2 gpio_request, %d\n", CAM_PIN_GPIO_V_RAW_1V2_EN);
	if (rc < 0)
		pr_err("rawchip off (\"gpio %d\", 1.2V) FAILED", CAM_PIN_GPIO_V_RAW_1V2_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_RAW_1V2_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_RAW_1V2_EN);
	}
	udelay(50);

	rc = gpio_request(CAM_PIN_GPIO_V_RAW_1V8_EN, "V_RAW_1V8_EN");
	pr_info("[CAM] rawchip 1v8 gpio_request, %d\n", CAM_PIN_GPIO_V_RAW_1V8_EN);
	if (rc < 0)
		pr_err("rawchip off (\"gpio %d\", 1.8V) FAILED", CAM_PIN_GPIO_V_RAW_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_RAW_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_RAW_1V8_EN);
	}

	return rc;
}

static struct msm_camera_rawchip_info k2_cl_msm_rawchip_board_info = {
	.rawchip_reset	= CAM_PIN_GPIO_RAW_RSTN,
	.rawchip_intr0	= CAM_PIN_GPIO_RAW_INTR0,
	.rawchip_intr1	= CAM_PIN_GPIO_RAW_INTR1,
	.rawchip_spi_freq = 27, /* MHz, should be the same to spi max_speed_hz */
	.rawchip_mclk_freq = 24, /* MHz, should be the same as cam csi0 mclk_clk_rate */
	.camera_rawchip_power_on = msm8930_rawchip_vreg_on,
	.camera_rawchip_power_off = msm8930_rawchip_vreg_off,
	.rawchip_use_ext_1v2 = msm8930_use_ext_1v2,
};

struct platform_device k2_cl_msm_rawchip_device = {
	.name	= "rawchip",
	.dev	= {
		.platform_data = &k2_cl_msm_rawchip_board_info,
	},
};
#endif

static uint16_t msm_cam_gpio_tbl[] = {
	CAM_PIN_GPIO_CAM_MCLK0, /*CAMIF_MCLK*/
	CAM_PIN_GPIO_CAM_MCLK1,
#if 0
	CAM_PIN_GPIO_CAM_I2C_DAT, /*CAMIF_I2C_DATA*/
	CAM_PIN_GPIO_CAM_I2C_CLK, /*CAMIF_I2C_CLK*/
#endif
	CAM_PIN_GPIO_RAW_INTR0,
	CAM_PIN_GPIO_RAW_INTR1,
	CAM_PIN_GPIO_MCAM_SPI_CLK,
	CAM_PIN_GPIO_MCAM_SPI_CS0,
	CAM_PIN_GPIO_MCAM_SPI_DI,
	CAM_PIN_GPIO_MCAM_SPI_DO,
};

static struct msm_camera_gpio_conf gpio_conf = {
	.cam_gpiomux_conf_tbl = NULL,
	.cam_gpiomux_conf_tbl_size = 0,
	.cam_gpio_tbl = msm_cam_gpio_tbl,
	.cam_gpio_tbl_size = ARRAY_SIZE(msm_cam_gpio_tbl),
};

#if defined(CONFIG_S5K4E5YX) || defined(CONFIG_S5K6A1GX) || defined(CONFIG_OV5693) || defined(CONFIG_S5K6A2YA)
static struct regulator *reg_8038_l17;
static struct regulator *reg_8038_l8;
static struct regulator *reg_8038_l2;

static int camera_sensor_power_enable(char *power, unsigned volt, struct regulator **sensor_power)
{
	int rc;

	if (power == NULL)
		return -ENODEV;

	*sensor_power = regulator_get(NULL, power);

	if (IS_ERR(*sensor_power)) {
		pr_err("[CAM] %s: Unable to get %s\n", __func__, power);
		return -ENODEV;
	}

	if (strcmp(power, "8038_l17") == 0) {
		regulator_set_optimum_mode(*sensor_power, 10000);
	}

	if (volt != 1800000) {
		rc = regulator_set_voltage(*sensor_power, volt, volt);
		if (rc < 0) {
			pr_err("[CAM] %s: unable to set %s voltage to %d rc:%d\n",
					__func__, power, volt, rc);
			regulator_put(*sensor_power);
			*sensor_power = NULL;
			return -ENODEV;
		}
	}

	rc = regulator_enable(*sensor_power);
	if (rc < 0) {
		pr_err("[CAM] %s: Enable regulator %s failed\n", __func__, power);
		regulator_put(*sensor_power);
		*sensor_power = NULL;
		return -ENODEV;
	}

	return rc;
}

static int camera_sensor_power_disable(struct regulator *sensor_power)
{

	int rc;
	if (sensor_power == NULL)
		return -ENODEV;

	if (IS_ERR(sensor_power)) {
		pr_err("[CAM] %s: Invalid requlator ptr\n", __func__);
		return -ENODEV;
	}

	rc = regulator_disable(sensor_power);
	if (rc < 0)
		pr_err("[CAM] %s: disable regulator failed\n", __func__);

	regulator_put(sensor_power);
	sensor_power = NULL;
	return rc;
}

static int msm8930_csi_vreg_on(void)
{
	pr_info("%s\n", __func__);
	return camera_sensor_power_enable("8038_l2", 1200000, &reg_8038_l2);
}

static int msm8930_csi_vreg_off(void)
{
	pr_info("%s\n", __func__);
	return camera_sensor_power_disable(reg_8038_l2);
}

struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
		.ioclk.mclk_clk_rate = 24000000,
		.ioclk.vfe_clk_rate  = 228570000,
		.csid_core = 0,
		.camera_csi_on = msm8930_csi_vreg_on,
		.camera_csi_off = msm8930_csi_vreg_off,
		.cam_bus_scale_table = &cam_bus_client_pdata,
		.csid_core = 0,
		.is_csiphy = 1,
		.is_csid   = 1,
		.is_ispif  = 1,
		.is_vpe    = 1,
	},
	{
		.ioclk.mclk_clk_rate = 24000000,
		.ioclk.vfe_clk_rate  = 228570000,
		.csid_core = 1,
		.camera_csi_on = msm8930_csi_vreg_on,
		.camera_csi_off = msm8930_csi_vreg_off,
		.cam_bus_scale_table = &cam_bus_client_pdata,
		.csid_core = 1,
		.is_csiphy = 1,
		.is_csid   = 1,
		.is_ispif  = 1,
		.is_vpe    = 1,
	},
};
#endif

#ifdef CONFIG_S5K4E5YX
static int msm8930_s5k4e5yx_vreg_on(void)
{
	int rc;
	pr_info("[CAM] %s\n", __func__);

	/* VCM */
	rc = camera_sensor_power_enable("8038_l17", 2800000, &reg_8038_l17);
	pr_info("[CAM] vcm sensor_power_enable(\"8038_l17\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l17\", 2.8V) FAILED %d\n", rc);
		goto enable_s5k4e5yx_vcm_fail;
	}
	udelay(50);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
		goto enable_s5k4e5yx_digital_fail;
	}

	gpio_tlmm_config(
		GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);
	gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	udelay(50);

	/* analog */
	rc = camera_sensor_power_enable("8038_l8", 2800000, &reg_8038_l8);
	pr_info("[CAM] analog sensor_power_enable(\"8038_l8\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l8\", 2.8V) FAILED %d\n", rc);
		goto enable_s5k4e5yx_analog_fail;
	}
	udelay(50);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
		goto enable_s5k4e5yx_io_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	udelay(50);

	return rc;

enable_s5k4e5yx_io_fail:
	camera_sensor_power_disable(reg_8038_l8);
enable_s5k4e5yx_analog_fail:
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
enable_s5k4e5yx_digital_fail:
	camera_sensor_power_disable(reg_8038_l17);
enable_s5k4e5yx_vcm_fail:
	return rc;
}

static int msm8930_s5k4e5yx_vreg_off(void)
{
	int rc = 0;

	pr_info("[CAM] %s\n", __func__);

	/* analog */
	rc = camera_sensor_power_disable(reg_8038_l8);
	pr_info("[CAM] sensor_power_disable(\"8038_l8\") == %d\n", rc);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable\(\"8038_l8\") FAILED %d\n", rc);
	udelay(50);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_tlmm_config(
			GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
	udelay(900);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	}
	udelay(50);

	/* VCM */
	if (reg_8038_l17 != NULL)
		regulator_set_optimum_mode(reg_8038_l17, 9000);
	rc = camera_sensor_power_disable(reg_8038_l17);
	pr_info("[CAM] sensor_power_disable(\"8038_l17\") == %d\n", rc);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable\(\"8038_l17\") FAILED %d\n", rc);

	return rc;
}

#ifdef CONFIG_S5K4E5YX_ACT
static struct i2c_board_info s5k4e5yx_actuator_i2c_info = {
	I2C_BOARD_INFO("s5k4e5yx_act", 0x0C),
};

static struct msm_actuator_info s5k4e5yx_actuator_info = {
	.board_info     = &s5k4e5yx_actuator_i2c_info,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = CAM_PIN_GPIO_CAM_VCM_PD,
	.vcm_enable     = 1,
};
#endif

static struct msm_camera_csi_lane_params s5k4e5yx_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_s5k4e5yx_board_info = {
	.mount_angle = 90,
	.mirror_flip = CAMERA_SENSOR_NONE,
	.sensor_reset_enable = 0,
	.sensor_reset	= 0,
	.sensor_pwd	= CAM_PIN_GPIO_CAM_PWDN,
	.vcm_pwd	= CAM_PIN_GPIO_CAM_VCM_PD,
	.vcm_enable	= 1,
	.csi_lane_params = &s5k4e5yx_csi_lane_params,
};

/* Andrew_Cheng linear led 20111205 MB */
//150 mA FL_MODE_FLASH_LEVEL1
//200 mA FL_MODE_FLASH_LEVEL2
//300 mA FL_MODE_FLASH_LEVEL3
//400 mA FL_MODE_FLASH_LEVEL4
//500 mA FL_MODE_FLASH_LEVEL5
//600 mA FL_MODE_FLASH_LEVEL6
//700 mA FL_MODE_FLASH_LEVEL7
static struct camera_led_est msm_camera_sensor_s5k4e5yx_led_table[] = {
	/*{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL1,
		.current_ma = 150,
		.lumen_value = 150,
		.min_step = 50,
		.max_step = 70
	},*/
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL2,
		.current_ma = 200,
		.lumen_value = 250,//245,//240,   //mk0118
		.min_step = 29,//23,  //mk0210
		.max_step = 128				
	},
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL3,
		.current_ma = 300,
		.lumen_value = 350,
		.min_step = 27,
		.max_step = 28			
	},
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL4,
		.current_ma = 400,
		.lumen_value = 440,
		.min_step = 25,
		.max_step = 26
	},
	/*{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL5,
		.current_ma = 500,
		.lumen_value = 500,
		.min_step = 23,//25,
		.max_step = 29//26,
	},*/
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL6,
		.current_ma = 600,
		.lumen_value = 625,
		.min_step = 23,
		.max_step = 24
	},

	/*{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL7,
		.current_ma = 700,
		.lumen_value = 700,
		.min_step = 21,
		.max_step = 22
	},*/
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH,
		.current_ma = 750,
		.lumen_value = 745,//725,   //mk0217  //mk0221
		.min_step = 0,
		.max_step = 22    //mk0210
	},
	{
		.enable = 2,
		.led_state = FL_MODE_FLASH_LEVEL2,
		.current_ma = 200,
		.lumen_value = 250,//245,
		.min_step = 0,
		.max_step = 270
	},
	{
		.enable = 0,
		.led_state = FL_MODE_OFF,
		.current_ma = 0,
		.lumen_value = 0,
		.min_step = 0,
		.max_step = 0
	},
	{
		.enable = 0,
		.led_state = FL_MODE_TORCH,
		.current_ma = 150,
		.lumen_value = 150,
		.min_step = 0,
		.max_step = 0
	},
	{
		.enable = 2,     //mk0210
		.led_state = FL_MODE_FLASH,
		.current_ma = 750,
		.lumen_value = 745,//725,   //mk0217   //mk0221
		.min_step = 271,
		.max_step = 317    //mk0210
	},
	{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL5,
		.current_ma = 500,
		.lumen_value = 500,
		.min_step = 25,
		.max_step = 26
	},
	{
		.enable = 0,//3,  //mk0210
		.led_state = FL_MODE_FLASH,
		.current_ma = 750,
		.lumen_value = 750,//740,//725,
		.min_step = 271,
		.max_step = 325
	},
	{
		.enable = 0,
		.led_state = FL_MODE_TORCH_LEVEL_2,
		.current_ma = 200,
		.lumen_value = 75,
		.min_step = 0,
		.max_step = 40
	},
};
		

static struct camera_led_info msm_camera_sensor_s5k4e5yx_led_info = {
	.enable = 1,
	.low_limit_led_state = FL_MODE_TORCH,
	.max_led_current_ma = 750,  //mk0210
	.num_led_est_table = ARRAY_SIZE(msm_camera_sensor_s5k4e5yx_led_table),
};

static struct camera_flash_info msm_camera_sensor_s5k4e5yx_flash_info = {
	.led_info = &msm_camera_sensor_s5k4e5yx_led_info,
	.led_est_table = msm_camera_sensor_s5k4e5yx_led_table,
};

static struct camera_flash_cfg msm_camera_sensor_s5k4e5yx_flash_cfg = {
	.low_temp_limit		= 5,
	.low_cap_limit		= 15,
	.flash_info			= &msm_camera_sensor_s5k4e5yx_flash_info,
};
/* Andrew_Cheng linear led 20111205 ME */

#ifdef CONFIG_MSM_CAMERA_FLASH
int msm8930_flashlight_control(int mode)
{
	pr_info("[CAM] %s, linear led, mode=%d", __func__, mode);
#ifdef CONFIG_FLASHLIGHT_TPS61310
	return tps61310_flashlight_control(mode);
#else
	return 0;
#endif
}

static struct msm_camera_sensor_flash_src msm_camera_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_CURRENT_DRIVER,
	.camera_flash = msm8930_flashlight_control,
};
#endif

static struct msm_camera_sensor_flash_data flash_s5k4e5yx = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= &msm_camera_flash_src,
#endif

};

static struct msm_camera_sensor_info msm_camera_sensor_s5k4e5yx_data = {
	.sensor_name	= "s5k4e5yx",
	.camera_power_on = msm8930_s5k4e5yx_vreg_on,
	.camera_power_off = msm8930_s5k4e5yx_vreg_off,
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_s5k4e5yx,
	.sensor_platform_info = &sensor_s5k4e5yx_board_info,
	.gpio_conf = &gpio_conf,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
#ifdef CONFIG_S5K4E5YX_ACT
	.actuator_info = &s5k4e5yx_actuator_info,
#endif
	.use_rawchip = 1,
	.flash_cfg = &msm_camera_sensor_s5k4e5yx_flash_cfg, /* Andrew_Cheng linear led 20111205 */
};

#endif	//CONFIG_S5K4E5YX

#ifdef CONFIG_S5K6A1GX
static int msm8930_s5k6a1gx_vreg_on(void)
{
	int rc;
	pr_info("[CAM] %s\n", __func__);

	/* VCM */
	rc = camera_sensor_power_enable("8038_l17", 2800000, &reg_8038_l17);
	pr_info("[CAM] sensor_power_enable(\"8038_l17\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l17\", 2.8V) FAILED %d\n", rc);
		goto enable_s5k6a1gx_vcm_fail;
	}
	udelay(50);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
		goto enable_s5k6a1gx_digital_fail;
	}
	gpio_tlmm_config(
		GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);
	gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);


	/* VCM PD */ /* TODO */
	rc = gpio_request(CAM_PIN_GPIO_CAM_VCM_PD, "CAM_VCM_PD");
	pr_info("[CAM] vcm pd gpio_request, %d\n", CAM_PIN_GPIO_CAM_VCM_PD);
	if (rc < 0 && rc != -EBUSY) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM_VCM_PD);
		goto enable_s5k6a1gx_vcm_pd;
	} else
		rc = 0;
	gpio_direction_output(CAM_PIN_GPIO_CAM_VCM_PD, 1);
	gpio_free(CAM_PIN_GPIO_CAM_VCM_PD);

	udelay(500);

	/* analog */
	rc = camera_sensor_power_enable("8038_l8", 2800000, &reg_8038_l8);
	pr_info("[CAM] sensor_power_enable(\"8038_l8\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l8\", 2.8V) FAILED %d\n", rc);
		goto enable_s5k6a1gx_analog_fail;
	}
	udelay(50);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
		goto enable_s5k6a1gx_io_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	udelay(50);

	/* reset pin */
	rc = gpio_request(CAM_PIN_GPIO_CAM2_RSTz, "s5k6a1gx");
	pr_info("[CAM] reset pin gpio_request, %d\n", CAM_PIN_GPIO_CAM2_RSTz);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_RSTz);
		goto enable_s5k6a1gx_rst_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_CAM2_RSTz, 1);
	gpio_free(CAM_PIN_GPIO_CAM2_RSTz);
	udelay(50);

	/* 2nd cam digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM2_D1V2_EN, "V_CAM2_D1V2_EN");
	pr_info("[CAM] 2nd cam digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
		goto enable_s5k6a1gx_2nd_digital_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_CAM2_D1V2_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	msleep(1);

	return rc;

enable_s5k6a1gx_2nd_digital_fail:
	rc = gpio_request(CAM_PIN_GPIO_CAM2_RSTz, "s5k6a1gx");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_RSTz);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM2_RSTz, 0);
		gpio_free(CAM_PIN_GPIO_CAM2_RSTz);
	}
enable_s5k6a1gx_rst_fail:
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	}

enable_s5k6a1gx_io_fail:
	camera_sensor_power_disable(reg_8038_l8);
enable_s5k6a1gx_analog_fail:
	rc = gpio_request(CAM_PIN_GPIO_CAM_VCM_PD, "CAM_VCM_PD");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM_VCM_PD);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM_VCM_PD, 0);
		gpio_free(CAM_PIN_GPIO_CAM_VCM_PD);
	}
enable_s5k6a1gx_vcm_pd:
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
enable_s5k6a1gx_digital_fail:
	camera_sensor_power_disable(reg_8038_l17);
enable_s5k6a1gx_vcm_fail:

	return rc;
}

static int msm8930_s5k6a1gx_vreg_off(void)
{

	int rc = 0;
	pr_info("[CAM] %s\n", __func__);

	/* analog */
	pr_info("[CAM] sensor_power_disable(\"8038_l8\") == %d\n", rc);
	rc = camera_sensor_power_disable(reg_8038_l8);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable(\"reg_8038_l8\") FAILED %d\n", rc);

	/* reset pin */
	rc = gpio_request(CAM_PIN_GPIO_CAM2_RSTz, "s5k6a1gx");
	pr_info("[CAM] reset pin gpio_request, %d\n", CAM_PIN_GPIO_CAM2_RSTz);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_RSTz);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM2_RSTz, 0);
		gpio_free(CAM_PIN_GPIO_CAM2_RSTz);
	}
	udelay(50);

	/* 2nd digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM2_D1V2_EN, "V_CAM2_D1V2_EN");
	pr_info("[CAM] 2nd cam digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAM2_D1V2_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	}
	msleep(1);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_tlmm_config(
			GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
	udelay(50);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	}

	/* VCM PD */ /* TODO */
	rc = gpio_request(CAM_PIN_GPIO_CAM_VCM_PD, "CAM_VCM_PD");
	pr_info("[CAM] vcm pd gpio_request, %d\n", CAM_PIN_GPIO_CAM_VCM_PD);
	if (rc < 0 && rc != -EBUSY) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM_VCM_PD);
	} else {
		gpio_direction_output(CAM_PIN_GPIO_CAM_VCM_PD, 0);
		gpio_free(CAM_PIN_GPIO_CAM_VCM_PD);
		rc = 0;
	}
	udelay(50);

	/* VCM */
	pr_info("[CAM] sensor_power_disable(\"reg_8038_l17\") == %d\n", rc);
	rc = camera_sensor_power_disable(reg_8038_l17);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable(\"reg_8038_l17\") FAILED %d\n", rc);

	return rc;
}

static struct msm_camera_csi_lane_params s5k6a1gx_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_s5k6a1gx_board_info = {
	.mount_angle = 270,
	.mirror_flip = CAMERA_SENSOR_NONE,
	.sensor_reset_enable = 0,
	.sensor_reset	= CAM_PIN_GPIO_CAM2_RSTz,
	.sensor_pwd	= CAM_PIN_GPIO_CAM2_STANDBY,
	.vcm_pwd	= CAM_PIN_GPIO_CAM_VCM_PD,
	.vcm_enable	= 0,
	.csi_lane_params = &s5k6a1gx_csi_lane_params,
};

static struct msm_camera_sensor_flash_data flash_s5k6a1gx = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k6a1gx_data = {
	.sensor_name	= "s5k6a1gx",
	.sensor_reset	= CAM_PIN_GPIO_CAM2_RSTz,
	.sensor_pwd	= CAM_PIN_GPIO_CAM2_STANDBY,
	.vcm_pwd	= 0,
	.vcm_enable	= 0,
	.camera_power_on = msm8930_s5k6a1gx_vreg_on,
	.camera_power_off = msm8930_s5k6a1gx_vreg_off,
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_s5k6a1gx,
	.sensor_platform_info = &sensor_s5k6a1gx_board_info,
	.gpio_conf = &gpio_conf,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.use_rawchip = 0,
};
#endif	//CONFIG_S5K6A1GX

#ifdef CONFIG_OV5693_ACT
static struct i2c_board_info ov5693_actuator_i2c_info = {
	I2C_BOARD_INFO("ov5693_act", 0x0C),
};

static struct msm_actuator_info ov5693_actuator_info = {
	.board_info     = &ov5693_actuator_i2c_info,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = CAM_PIN_GPIO_CAM_VCM_PD,
	.vcm_enable     = 1,
};
#endif

#ifdef CONFIG_OV5693
static int msm8930_ov5693_vreg_on(void)
{
	int rc;
	pr_info("[CAM] %s\n", __func__);

	/* VCM */
	rc = camera_sensor_power_enable("8038_l17", 2800000, &reg_8038_l17);
	pr_info("[CAM] vcm sensor_power_enable(\"8038_l17\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l17\", 2.8V) FAILED %d\n", rc);
		goto enable_ov5693_vcm_fail;
	}
	udelay(50);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
		goto enable_ov5693_digital_fail;
	}

	gpio_tlmm_config(
		GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);
	gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	udelay(50);

	/* analog */
	rc = camera_sensor_power_enable("8038_l8", 2800000, &reg_8038_l8);
	pr_info("[CAM] analog sensor_power_enable(\"8038_l8\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l8\", 2.8V) FAILED %d\n", rc);
		goto enable_ov5693_analog_fail;
	}
	mdelay(2);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
		goto enable_ov5693_io_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	udelay(50);

	return rc;

enable_ov5693_io_fail:
	camera_sensor_power_disable(reg_8038_l8);
enable_ov5693_analog_fail:
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
enable_ov5693_digital_fail:
	camera_sensor_power_disable(reg_8038_l17);
enable_ov5693_vcm_fail:
	return rc;
}

static int msm8930_ov5693_vreg_off(void)
{
	int rc = 0;

	pr_info("[CAM] %s\n", __func__);

	/* analog */
	rc = camera_sensor_power_disable(reg_8038_l8);
	pr_info("[CAM] sensor_power_disable(\"8038_l8\") == %d\n", rc);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable\(\"8038_l8\") FAILED %d\n", rc);
	udelay(50);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_tlmm_config(
			GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
	udelay(900);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	}
	mdelay(1);

	/* VCM */
	if (reg_8038_l17 != NULL)
		regulator_set_optimum_mode(reg_8038_l17, 9000);
	rc = camera_sensor_power_disable(reg_8038_l17);
	pr_info("[CAM] sensor_power_disable(\"8038_l17\") == %d\n", rc);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable\(\"8038_l17\") FAILED %d\n", rc);

	return rc;
}

static struct msm_camera_csi_lane_params ov5693_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_ov5693_board_info = {
	.mount_angle = 90,
	.mirror_flip = CAMERA_SENSOR_MIRROR,
	.sensor_reset_enable = 0,
	.sensor_reset	= 0,
	.sensor_pwd	= CAM_PIN_GPIO_CAM_PWDN,
	.vcm_pwd	= CAM_PIN_GPIO_CAM_VCM_PD,
	.vcm_enable	= 1,
	.csi_lane_params = &ov5693_csi_lane_params,
};

/* Andrew_Cheng linear led 20111205 MB */
//150 mA FL_MODE_FLASH_LEVEL1
//200 mA FL_MODE_FLASH_LEVEL2
//300 mA FL_MODE_FLASH_LEVEL3
//400 mA FL_MODE_FLASH_LEVEL4
//500 mA FL_MODE_FLASH_LEVEL5
//600 mA FL_MODE_FLASH_LEVEL6
//700 mA FL_MODE_FLASH_LEVEL7
static struct camera_led_est msm_camera_sensor_ov5693_led_table[] = {
	/*{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL1,
		.current_ma = 150,
		.lumen_value = 150,
		.min_step = 50,
		.max_step = 70
	},*/
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL2,
		.current_ma = 200,
		.lumen_value = 250,//245,//240,   //mk0118
		.min_step = 29,//23,  //mk0210
		.max_step = 128
	},
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL3,
		.current_ma = 300,
		.lumen_value = 350,
		.min_step = 27,
		.max_step = 28
	},
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL4,
		.current_ma = 400,
		.lumen_value = 440,
		.min_step = 25,
		.max_step = 26
	},
	/*{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL5,
		.current_ma = 500,
		.lumen_value = 500,
		.min_step = 23,//25,
		.max_step = 29//26,
	},*/
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH_LEVEL6,
		.current_ma = 600,
		.lumen_value = 625,
		.min_step = 23,
		.max_step = 24
	},

	/*{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL7,
		.current_ma = 700,
		.lumen_value = 700,
		.min_step = 21,
		.max_step = 22
	},*/
	{
		.enable = 1,
		.led_state = FL_MODE_FLASH,
		.current_ma = 750,
		.lumen_value = 745,//725,   //mk0217  //mk0221
		.min_step = 0,
		.max_step = 22    //mk0210
	},
	{
		.enable = 2,
		.led_state = FL_MODE_FLASH_LEVEL2,
		.current_ma = 200,
		.lumen_value = 250,//245,
		.min_step = 0,
		.max_step = 270
	},
	{
		.enable = 0,
		.led_state = FL_MODE_OFF,
		.current_ma = 0,
		.lumen_value = 0,
		.min_step = 0,
		.max_step = 0
	},
	{
		.enable = 0,
		.led_state = FL_MODE_TORCH,
		.current_ma = 150,
		.lumen_value = 150,
		.min_step = 0,
		.max_step = 0
	},
	{
		.enable = 2,     //mk0210
		.led_state = FL_MODE_FLASH,
		.current_ma = 750,
		.lumen_value = 745,//725,   //mk0217   //mk0221
		.min_step = 271,
		.max_step = 317    //mk0210
	},
	{
		.enable = 0,
		.led_state = FL_MODE_FLASH_LEVEL5,
		.current_ma = 500,
		.lumen_value = 500,
		.min_step = 25,
		.max_step = 26
	},
	{
		.enable = 0,//3,  //mk0210
		.led_state = FL_MODE_FLASH,
		.current_ma = 750,
		.lumen_value = 750,//740,//725,
		.min_step = 271,
		.max_step = 325
	},
	{
		.enable = 0,
		.led_state = FL_MODE_TORCH_LEVEL_2,
		.current_ma = 200,
		.lumen_value = 75,
		.min_step = 0,
		.max_step = 40
	},
};

static struct camera_led_info msm_camera_sensor_ov5693_led_info = {
	.enable = 1,
	.low_limit_led_state = FL_MODE_TORCH,
	.max_led_current_ma = 750,  //mk0210
	.num_led_est_table = ARRAY_SIZE(msm_camera_sensor_ov5693_led_table),
};

static struct camera_flash_info msm_camera_sensor_ov5693_flash_info = {
	.led_info = &msm_camera_sensor_ov5693_led_info,
	.led_est_table = msm_camera_sensor_ov5693_led_table,
};

static struct camera_flash_cfg msm_camera_sensor_ov5693_flash_cfg = {
	.low_temp_limit		= 5,
	.low_cap_limit		= 15,
	.flash_info			= &msm_camera_sensor_ov5693_flash_info,
};
/* Andrew_Cheng linear led 20111205 ME */

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_data flash_ov5693 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= &msm_camera_flash_src,
#endif

};
#endif

static struct msm_camera_sensor_info msm_camera_sensor_ov5693_data = {
	.sensor_name	= "ov5693",
	.camera_power_on = msm8930_ov5693_vreg_on,
	.camera_power_off = msm8930_ov5693_vreg_off,
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_ov5693,
	.sensor_platform_info = &sensor_ov5693_board_info,
	.gpio_conf = &gpio_conf,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
#ifdef CONFIG_OV5693_ACT
	.actuator_info = &ov5693_actuator_info,
#endif
	.use_rawchip = 1,
	.flash_cfg = &msm_camera_sensor_ov5693_flash_cfg, /* Andrew_Cheng linear led 20111205 */
};

#endif	//CONFIG_OV5693

#ifdef CONFIG_S5K6A2YA
static int msm8930_s5k6a2ya_vreg_on(void)
{
	int rc;
	pr_info("[CAM] %s\n", __func__);

	/* MIPI Switch */
	rc = gpio_request(CAM_PIN_GPIO_CAM2_SEL, "CAM2_SEL");
	pr_info("[CAM] cam sel gpio_request, %d\n", CAM_PIN_GPIO_CAM2_SEL);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_SEL);
	}
	gpio_direction_output(CAM_PIN_GPIO_CAM2_SEL, 1);
	gpio_free(CAM_PIN_GPIO_CAM2_SEL);
	udelay(50);

	/* VCM */
	rc = camera_sensor_power_enable("8038_l17", 2800000, &reg_8038_l17);
	pr_info("[CAM] sensor_power_enable(\"8038_l17\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l17\", 2.8V) FAILED %d\n", rc);
		goto enable_s5k6a2ya_vcm_fail;
	}
	udelay(50);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
		goto enable_s5k6a2ya_digital_fail;
	}
	gpio_tlmm_config(
		GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
		GPIO_CFG_ENABLE);
	gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);


	/* VCM PD */
	rc = gpio_request(CAM_PIN_GPIO_CAM_VCM_PD, "CAM_VCM_PD");
	pr_info("[CAM] vcm pd gpio_request, %d\n", CAM_PIN_GPIO_CAM_VCM_PD);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM_VCM_PD);
		goto enable_s5k6a2ya_vcm_pd;
	}
	gpio_direction_output(CAM_PIN_GPIO_CAM_VCM_PD, 1);
	gpio_free(CAM_PIN_GPIO_CAM_VCM_PD);

	udelay(500);

	/* analog */
	rc = camera_sensor_power_enable("8038_l8", 2800000, &reg_8038_l8);
	pr_info("[CAM] sensor_power_enable(\"8038_l8\", 2.8V) == %d\n", rc);
	if (rc < 0) {
		pr_err("[CAM] sensor_power_enable(\"8038_l8\", 2.8V) FAILED %d\n", rc);
		goto enable_s5k6a2ya_analog_fail;
	}
	udelay(50);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
		goto enable_s5k6a2ya_io_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	udelay(50);

	/* 2nd cam digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM2_D1V2_EN, "V_CAM2_D1V2_EN");
	pr_info("[CAM] 2nd cam digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
		goto enable_s5k6a2ya_2nd_digital_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_V_CAM2_D1V2_EN, 1);
	gpio_free(CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	udelay(50);

	/* reset pin */
	rc = gpio_request(CAM_PIN_GPIO_CAM2_RSTz, "s5k6a2ya");
	pr_info("[CAM] reset pin gpio_request, %d\n", CAM_PIN_GPIO_CAM2_RSTz);
	if (rc < 0) {
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_RSTz);
		goto enable_s5k6a2ya_rst_fail;
	}
	gpio_direction_output(CAM_PIN_GPIO_CAM2_RSTz, 1);
	gpio_free(CAM_PIN_GPIO_CAM2_RSTz);
	mdelay(1);

	return rc;

enable_s5k6a2ya_2nd_digital_fail:
	rc = gpio_request(CAM_PIN_GPIO_CAM2_RSTz, "s5k6a2ya");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_RSTz);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM2_RSTz, 0);
		gpio_free(CAM_PIN_GPIO_CAM2_RSTz);
	}
enable_s5k6a2ya_rst_fail:
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	}

enable_s5k6a2ya_io_fail:
	camera_sensor_power_disable(reg_8038_l8);
enable_s5k6a2ya_analog_fail:
	rc = gpio_request(CAM_PIN_GPIO_CAM_VCM_PD, "CAM_VCM_PD");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM_VCM_PD);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM_VCM_PD, 0);
		gpio_free(CAM_PIN_GPIO_CAM_VCM_PD);
	}
enable_s5k6a2ya_vcm_pd:
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
enable_s5k6a2ya_digital_fail:
	camera_sensor_power_disable(reg_8038_l17);
enable_s5k6a2ya_vcm_fail:

	return rc;
}

static int msm8930_s5k6a2ya_vreg_off(void)
{
	int rc = 0;
	pr_info("[CAM] %s\n", __func__);

	/* reset pin */
	rc = gpio_request(CAM_PIN_GPIO_CAM2_RSTz, "s5k6a2ya");
	pr_info("[CAM] reset pin gpio_request, %d\n", CAM_PIN_GPIO_CAM2_RSTz);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_RSTz);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM2_RSTz, 0);
		gpio_free(CAM_PIN_GPIO_CAM2_RSTz);
	}
	udelay(50);

	/* analog */
	pr_info("[CAM] sensor_power_disable(\"8038_l8\") == %d\n", rc);
	rc = camera_sensor_power_disable(reg_8038_l8);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable(\"reg_8038_l8\") FAILED %d\n", rc);
	udelay(50);

	/* 2nd digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM2_D1V2_EN, "V_CAM2_D1V2_EN");
	pr_info("[CAM] 2nd cam digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAM2_D1V2_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM2_D1V2_EN);
	}
	msleep(1);

	/* digital */
	rc = gpio_request(CAM_PIN_GPIO_V_CAM_D1V8_EN, "V_CAM_D1V8_EN");
	pr_info("[CAM] digital gpio_request, %d\n", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAM_D1V8_EN);
	else {
		gpio_tlmm_config(
			GPIO_CFG(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		gpio_direction_output(CAM_PIN_GPIO_V_CAM_D1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAM_D1V8_EN);
	}
	udelay(50);

	/* IO */
	rc = gpio_request(CAM_PIN_GPIO_V_CAMIO_1V8_EN, "V_CAMIO_1V8_EN");
	pr_info("[CAM] cam io gpio_request, %d\n", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	else {
		gpio_direction_output(CAM_PIN_GPIO_V_CAMIO_1V8_EN, 0);
		gpio_free(CAM_PIN_GPIO_V_CAMIO_1V8_EN);
	}

	/* VCM PD */
	rc = gpio_request(CAM_PIN_GPIO_CAM_VCM_PD, "CAM_VCM_PD");
	pr_info("[CAM] vcm pd gpio_request, %d\n", CAM_PIN_GPIO_CAM_VCM_PD);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM_VCM_PD);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM_VCM_PD, 0);
		gpio_free(CAM_PIN_GPIO_CAM_VCM_PD);
	}
	udelay(50);

	/* VCM */
	pr_info("[CAM] sensor_power_disable(\"reg_8038_l17\") == %d\n", rc);
	rc = camera_sensor_power_disable(reg_8038_l17);
	if (rc < 0)
		pr_err("[CAM] sensor_power_disable(\"reg_8038_l17\") FAILED %d\n", rc);

	/* MIPI Switch */
	rc = gpio_request(CAM_PIN_GPIO_CAM2_SEL, "CAM2_SEL");
	pr_info("[CAM] cam sel gpio_request, %d\n", CAM_PIN_GPIO_CAM2_SEL);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed", CAM_PIN_GPIO_CAM2_SEL);
	else {
		gpio_direction_output(CAM_PIN_GPIO_CAM2_SEL, 0);
		gpio_free(CAM_PIN_GPIO_CAM2_SEL);
	}
	udelay(50);

	return rc;
}

static struct msm_camera_csi_lane_params s5k6a2ya_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_s5k6a2ya_board_info = {
	.mount_angle = 270,
	.mirror_flip = CAMERA_SENSOR_NONE,
	.sensor_reset_enable = 0,
	.sensor_reset	= CAM_PIN_GPIO_CAM2_RSTz,
	.sensor_pwd	= CAM_PIN_GPIO_CAM2_STANDBY,
	.vcm_pwd	= CAM_PIN_GPIO_CAM_VCM_PD,
	.vcm_enable	= 0,
	.csi_lane_params = &s5k6a2ya_csi_lane_params,
};

static struct msm_camera_sensor_flash_data flash_s5k6a2ya = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_sensor_info msm_camera_sensor_s5k6a2ya_data = {
	.sensor_name	= "s5k6a2ya",
	.sensor_reset	= CAM_PIN_GPIO_CAM2_RSTz,
	.sensor_pwd	= CAM_PIN_GPIO_CAM2_STANDBY,
	.vcm_pwd	= 0,
	.vcm_enable	= 0,
	.camera_power_on = msm8930_s5k6a2ya_vreg_on,
	.camera_power_off = msm8930_s5k6a2ya_vreg_off,
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_s5k6a2ya,
	.sensor_platform_info = &sensor_s5k6a2ya_board_info,
	.gpio_conf = &gpio_conf,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.use_rawchip = 1,
};
#endif	//CONFIG_S5K6A2YA

static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

#ifdef CONFIG_I2C
struct i2c_board_info k2_cl_camera_i2c_boardinfo[] = {

#ifdef CONFIG_OV5693
	{
	I2C_BOARD_INFO("ov5693", 0x20 >> 1),
	.platform_data = &msm_camera_sensor_ov5693_data,
	},
#endif

#ifdef CONFIG_S5K6A2YA
	{
	I2C_BOARD_INFO("s5k6a2ya", 0x6C >> 1),
	.platform_data = &msm_camera_sensor_s5k6a2ya_data,
	},
#endif

};

struct i2c_board_info k2_cl_camera_i2c_boardinfo_old[] = {

#ifdef CONFIG_S5K4E5YX
	{
	I2C_BOARD_INFO("s5k4e5yx", 0x20 >> 1),
	.platform_data = &msm_camera_sensor_s5k4e5yx_data,
	},
#endif

#ifdef CONFIG_S5K6A1GX
	{
	I2C_BOARD_INFO("s5k6a1gx", 0x6C >> 1),
	.platform_data = &msm_camera_sensor_s5k6a1gx_data,
	},
#endif
};

struct msm_camera_board_info k2_cl_camera_board_info = {0};
#endif


#define GPIO_IN_OUT(n) (MSM_TLMM_BASE + 0x1004 + (0x10 * n))


void __init k2_cl_init_cam(void)
{
	int rc = 0;
	int voltage = 0;

	pr_info("[CAM] msm8930_cam_common_configs");

	msm_gpiomux_install(msm8930_cam_common_configs,
			ARRAY_SIZE(msm8930_cam_common_configs));

	platform_device_register(&msm_camera_server);
	platform_device_register(&msm8960_device_csiphy0);
	platform_device_register(&msm8960_device_csiphy1);
	platform_device_register(&msm8960_device_csid0);
	platform_device_register(&msm8960_device_csid1);
	platform_device_register(&msm8960_device_ispif);
	platform_device_register(&msm8960_device_vfe);
	platform_device_register(&msm8960_device_vpe);

#ifdef CONFIG_I2C
	/* Pre-setting for i2c sensor device */
	rc = gpio_request(CAM_PIN_GPIO_CAM_ID, "MSM_CAM_ID");
	pr_info("[CAM] cam id gpio_request, %d\n", CAM_PIN_GPIO_CAM_ID);
	if (rc < 0)
		pr_err("[CAM] GPIO(%d) request failed\n", CAM_PIN_GPIO_CAM_ID);
	else {

		gpio_tlmm_config(
			GPIO_CFG(CAM_PIN_GPIO_CAM_ID, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		msleep(1);
		/* direction: GPIO_CFG_INPUT */
		voltage = readl(GPIO_IN_OUT(CAM_PIN_GPIO_CAM_ID)) & BIT(0);
		pr_info("[CAM] CAM ID voltage: %d\n", voltage);

		gpio_tlmm_config(
			GPIO_CFG(CAM_PIN_GPIO_CAM_ID, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
			GPIO_CFG_ENABLE);
		gpio_free(CAM_PIN_GPIO_CAM_ID);
	}

	if (voltage == 1) {
		pr_info("[CAM] use s5k4e5yx and s5k6a1gx\n");
		k2_cl_camera_board_info.board_info = k2_cl_camera_i2c_boardinfo_old;
		k2_cl_camera_board_info.num_i2c_board_info = ARRAY_SIZE(k2_cl_camera_i2c_boardinfo_old);
	} else {
		k2_cl_camera_board_info.board_info = k2_cl_camera_i2c_boardinfo;
		k2_cl_camera_board_info.num_i2c_board_info = ARRAY_SIZE(k2_cl_camera_i2c_boardinfo);
	}
#endif

}
