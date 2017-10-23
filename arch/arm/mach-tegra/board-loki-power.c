/*
 * arch/arm/mach-tegra/board-loki-power.c
 *
 * Copyright (c) 2013-2014 NVIDIA Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/resource.h>
#include <linux/io.h>
#include <linux/regulator/machine.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/regulator/tegra-dfll-bypass-regulator.h>
#include <linux/power/power_supply_extcon.h>
#include <linux/tegra-fuse.h>
<<<<<<< HEAD
#include <linux/wakelock.h>
#include <linux/gpio.h>
#include <linux/system-wakeup.h>
#include <linux/syscore_ops.h>
=======
#include <linux/tegra-pmc.h>
#include <linux/pinctrl/pinconf-tegra.h>
#include <linux/gpio.h>
#include <linux/system-wakeup.h>
#include <linux/syscore_ops.h>
#include <linux/delay.h>
#include <linux/tegra-pmc.h>
>>>>>>> update/master

#include <mach/irqs.h>
#include <mach/edp.h>

#include <linux/pid_thermal_gov.h>
#include <linux/tegra_soctherm.h>

#include <asm/mach-types.h>

#include "pm.h"
#include "board.h"
#include "tegra-board-id.h"
#include "board-common.h"
#include "board-loki.h"
#include "board-pmu-defines.h"
#include "devices.h"
#include "iomap.h"
#include "tegra-board-id.h"
#include <linux/platform/tegra/dvfs.h>
#include <linux/platform/tegra/tegra_cl_dvfs.h>

<<<<<<< HEAD
#define PMC_CTRL                0x0
#define PMC_CTRL_INTR_LOW       (1 << 17)

static struct wake_lock wakelock;

=======
>>>>>>> update/master
void tegra13x_vdd_cpu_align(int step_uv, int offset_uv);

static void loki_reset_gamepad(void)
{
	int ret = 0;
	int gpio_gamepad_rst = TEGRA_GPIO_PQ7;

<<<<<<< HEAD
PALMAS_REGS_PDATA(smps123, 650,  1400, NULL, 0, 1, 1, NORMAL,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(smps45, 700,  1250, NULL, 0, 0, 0, NORMAL,
		0, PALMAS_EXT_CONTROL_NSLEEP, 0, 2500, 0);
PALMAS_REGS_PDATA(smps6, 3300,  3300, NULL, 1, 1, 1, NORMAL,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(smps7, 1350,  1350, NULL, 1, 1, 1, NORMAL,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(smps7_a01, 1500,  1500, NULL, 1, 1, 1, NORMAL,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(smps8, 1800,  1800, NULL, 1, 1, 1, NORMAL,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(smps9, 2800,  2800, NULL, 0, 0, 1, FAST,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(smps10_out1, 5000,  5000, NULL, 1, 1, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo1, 1050,  1050, palmas_rails(smps8), 0, 0, 1, 0,
		0, PALMAS_EXT_CONTROL_NSLEEP, 0, 0, 0);
PALMAS_REGS_PDATA(ldo1_a01, 1050,  1050, palmas_rails(smps8), 0, 1, 1, 0,
		0, PALMAS_EXT_CONTROL_NSLEEP, 0, 0, 0);
PALMAS_REGS_PDATA(ldo2, 2800,  3100, palmas_rails(smps6), 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo3, 1200,  1200, palmas_rails(smps8), 0, 1, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo4, 1800,  1800, palmas_rails(smps6), 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo5, 1200,  1200, palmas_rails(smps8), 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo5_a01, 1100,  1100, palmas_rails(smps8), 1, 1, 1, 0,
		0, PALMAS_EXT_CONTROL_NSLEEP, 0, 0, 0);
PALMAS_REGS_PDATA(ldo6, 2800,  2800, palmas_rails(smps6), 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo8, 900,  900, NULL, 1, 1, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldo9, 1800,  3300, palmas_rails(smps6), 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldoln, 2800, 3300, NULL, 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(ldousb, 2300,  3300, NULL, 0, 0, 1, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(regen1, 3300,  3300, NULL, 0, 0, 0, 0,
		0, 0, 0, 0, 0);
PALMAS_REGS_PDATA(regen2, 5000,  5000, NULL, 0, 1, 0, 0,
		0, PALMAS_EXT_CONTROL_NSLEEP, 0, 0, 0);


#define PALMAS_REG_PDATA(_sname) &reg_idata_##_sname
static struct regulator_init_data *loki_reg_data[PALMAS_NUM_REGS] = {
	NULL,
	PALMAS_REG_PDATA(smps123),
	NULL,
	PALMAS_REG_PDATA(smps45),
	NULL,
	PALMAS_REG_PDATA(smps6),
	PALMAS_REG_PDATA(smps7),
	PALMAS_REG_PDATA(smps8),
	PALMAS_REG_PDATA(smps9),
	NULL,
	PALMAS_REG_PDATA(smps10_out1),
	PALMAS_REG_PDATA(ldo1),
	PALMAS_REG_PDATA(ldo2),
	PALMAS_REG_PDATA(ldo3),
	PALMAS_REG_PDATA(ldo4),
	PALMAS_REG_PDATA(ldo5),
	PALMAS_REG_PDATA(ldo6),
	NULL,
	PALMAS_REG_PDATA(ldo8),
	PALMAS_REG_PDATA(ldo9),
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	PALMAS_REG_PDATA(ldoln),
	PALMAS_REG_PDATA(ldousb),
	PALMAS_REG_PDATA(regen1),
	PALMAS_REG_PDATA(regen2),
	NULL,
	NULL,
	NULL,
};
=======
	struct board_info bi;
	tegra_get_board_info(&bi);
>>>>>>> update/master

	if (bi.board_id == BOARD_E2548 ||
		(bi.board_id == BOARD_P2530 && bi.fab == 0xA0))
		gpio_gamepad_rst = TEGRA_GPIO_PK0;

<<<<<<< HEAD
static struct iio_map palmas_adc_iio_maps[] = {
	PALMAS_GPADC_IIO_MAP(NULL, NULL, NULL),
};

static struct iio_map palmas_adc_iio_maps_p2530_loki[] = {
	PALMAS_GPADC_IIO_MAP(IN1, "generic-adc-thermal.0", "thermistor"),
	PALMAS_GPADC_IIO_MAP(IN3, "generic-adc-thermal.1", "tdiode"),
	PALMAS_GPADC_IIO_MAP(IN4, "generic-adc-thermal.2", "tbat"),
	PALMAS_GPADC_IIO_MAP(NULL, NULL, NULL),
};

static struct iio_map palmas_adc_iio_maps_p2530_foster[] = {
	PALMAS_GPADC_IIO_MAP(IN1, "generic-adc-thermal.0", "thermistor"),
	PALMAS_GPADC_IIO_MAP(IN3, "generic-adc-thermal.1", "tdiode"),
	PALMAS_GPADC_IIO_MAP(NULL, NULL, NULL),
};

struct palmas_adc_auto_conv_property palmas_adc_auto_conv1_data = {
	.adc_channel_number = PALMAS_ADC_CH_IN4, /* Tbat */
	/* Shutdown if ADC auto conversion is below 952(>60C). */
	.adc_low_threshold = 952, /* 60C */
	.adc_shutdown = true,
};

struct palmas_adc_auto_conv_property palmas_adc_auto_conv0_data = {
	.adc_channel_number = PALMAS_ADC_CH_IN1, /* Tboard */
	/* Shutdown if ADC auto conversion is below (>105C). */
	.adc_low_threshold = 765, /* 105C */
	.adc_shutdown = true,
};

static struct palmas_gpadc_platform_data palmas_adc_pdata = {
	/* If ch3_dual_current is true, it will measure ch3 input signal with
	 * ch3_current and the next current of ch3_current.
	 * So this system will use 400uA and 800uA for ch3 measurement. */
	.ch3_current = 400,	/* 0uA, 10uA, 400uA, 800uA */
	.ch3_dual_current = true,
	.extended_delay = true,
	.iio_maps = palmas_adc_iio_maps,
};
=======
	ret = gpio_request(gpio_gamepad_rst, "GAMEPAD_RST");
	if (ret < 0) {
		pr_err("%s: gpio_request failed %d\n", __func__, ret);
		return;
	}

	ret = gpio_direction_output(gpio_gamepad_rst, 1);
	if (ret < 0) {
		gpio_free(gpio_gamepad_rst);
		pr_err("%s: gpio_direction_output failed %d\n", __func__, ret);
		return;
	}
>>>>>>> update/master

	gpio_set_value(gpio_gamepad_rst, 0);
	udelay(20);
	gpio_set_value(gpio_gamepad_rst, 1);
	gpio_free(gpio_gamepad_rst);

	pr_info("%s: done\n", __func__);

	return;
}

static void loki_board_resume(void)
{
	int wakeup_irq = get_wakeup_reason_irq();

	/* if wake up by LID open, reset gamepad uC */
	if (gpio_to_irq(TEGRA_GPIO_PS0) == wakeup_irq)
		loki_reset_gamepad();

	return;
}

static struct syscore_ops loki_pm_syscore_ops = {
	.resume = loki_board_resume,
};

static void loki_reset_gamepad(void)
{
	int ret = 0;
	int gpio_gamepad_rst = TEGRA_GPIO_PQ7;

	struct board_info bi;
	tegra_get_board_info(&bi);

	if (bi.board_id == BOARD_E2548 ||
		(bi.board_id == BOARD_P2530 && bi.fab == 0xA0))
		gpio_gamepad_rst = TEGRA_GPIO_PK0;

	ret = gpio_request(gpio_gamepad_rst, "GAMEPAD_RST");
	if (ret < 0) {
		pr_err("%s: gpio_request failed %d\n", __func__, ret);
		return;
	}

	ret = gpio_direction_output(gpio_gamepad_rst, 1);
	if (ret < 0) {
		gpio_free(gpio_gamepad_rst);
		pr_err("%s: gpio_direction_output failed %d\n", __func__, ret);
		return;
	}

	gpio_set_value(gpio_gamepad_rst, 0);
	udelay(20);
	gpio_set_value(gpio_gamepad_rst, 1);
	gpio_free(gpio_gamepad_rst);

	pr_info("%s: done\n", __func__);

	return;
}

static void loki_board_resume(void)
{
	int wakeup_irq = get_wakeup_reason_irq();

	/* if wake up by LID open, reset gamepad uC */
	if (gpio_to_irq(TEGRA_GPIO_PS0) == wakeup_irq)
		loki_reset_gamepad();

	return;
}

static struct syscore_ops loki_pm_syscore_ops = {
	.resume = loki_board_resume,
};

static struct tegra_suspend_platform_data loki_suspend_data = {
	.cpu_timer      = 3500,
	.cpu_off_timer  = 300,
	.suspend_mode   = TEGRA_SUSPEND_LP0,
	.core_timer     = 0x157e,
	.core_off_timer = 2000,
	.corereq_high   = true,
	.sysclkreq_high = true,
	.cpu_lp2_min_residency = 1000,
	.min_residency_crail = 20000,
};

int __init loki_suspend_init(void)
{
	struct board_info bi;
	tegra_get_board_info(&bi);

	/* reduce cpu_power_good_time for loki ffd fab a3 or higher */
	if (bi.board_id == BOARD_P2530 && bi.fab >= 0xa3)
		loki_suspend_data.cpu_timer = 500;

	tegra_init_suspend(&loki_suspend_data);

<<<<<<< HEAD
	/* disable system-suspend on foster, stay awake */
	if (bi.board_id == BOARD_P2530 && bi.sku == BOARD_SKU_FOSTER) {
		wake_lock_init(&wakelock, WAKE_LOCK_SUSPEND, "foster-staywake");
		wake_lock(&wakelock);
	}

	register_syscore_ops(&loki_pm_syscore_ops);
=======
	register_syscore_ops(&loki_pm_syscore_ops);

>>>>>>> update/master
	return 0;
}

static struct power_supply_extcon_plat_data extcon_pdata = {
	.extcon_name = "tegra-udc",
	.y_cable_extcon_name = "tegra-otg",
};

static struct platform_device power_supply_extcon_device = {
	.name	= "power-supply-extcon",
	.id	= -1,
	.dev	= {
		.platform_data = &extcon_pdata,
	},
};

/************************ LOKI CL-DVFS DATA *********************/
#define LOKI_CPU_VDD_MAP_SIZE		33
#define LOKI_CPU_VDD_MIN_UV		703000
#define LOKI_CPU_VDD_STEP_UV		19200
#define LOKI_CPU_VDD_STEP_US		80
#define LOKI_CPU_VDD_BOOT_UV		1000000

#ifdef CONFIG_ARCH_TEGRA_HAS_CL_DVFS
/* loki board parameters for cpu dfll */
static struct tegra_cl_dvfs_cfg_param loki_cl_dvfs_param = {
	.sample_rate = 50000,

	.force_mode = TEGRA_CL_DVFS_FORCE_FIXED,
	.cf = 10,
	.ci = 0,
	.cg = 2,

	.droop_cut_value = 0xF,
	.droop_restore_ramp = 0x0,
	.scale_out_ramp = 0x0,
};

/* loki dfll bypass device for legacy dvfs control */
static struct regulator_consumer_supply loki_dfll_bypass_consumers[] = {
	REGULATOR_SUPPLY("vdd_cpu", NULL),
};
DFLL_BYPASS(loki,
	    LOKI_CPU_VDD_MIN_UV, LOKI_CPU_VDD_STEP_UV, LOKI_CPU_VDD_BOOT_UV,
	    LOKI_CPU_VDD_MAP_SIZE, LOKI_CPU_VDD_STEP_US, -1);

static struct tegra_cl_dvfs_platform_data loki_cl_dvfs_data = {
	.dfll_clk_name = "dfll_cpu",
	.pmu_if = TEGRA_CL_DVFS_PMU_PWM,
	.u.pmu_pwm = {
		.pwm_rate = 12750000,
		.min_uV = LOKI_CPU_VDD_MIN_UV,
		.step_uV = LOKI_CPU_VDD_STEP_UV,
		.pwm_bus = TEGRA_CL_DVFS_PWM_1WIRE_BUFFER,
		.out_gpio = TEGRA_GPIO_PU6,
		.out_enable_high = false,
#ifdef CONFIG_REGULATOR_TEGRA_DFLL_BYPASS
		.dfll_bypass_dev = &loki_dfll_bypass_dev,
#endif
	},

	.cfg_param = &loki_cl_dvfs_param,
};

static void loki_suspend_dfll_bypass(void)
{
/* tristate external PWM buffer */
<<<<<<< HEAD
	if (loki_cl_dvfs_data.u.pmu_pwm.out_gpio)
=======
	if (gpio_is_valid(loki_cl_dvfs_data.u.pmu_pwm.out_gpio))
>>>>>>> update/master
		__gpio_set_value(loki_cl_dvfs_data.u.pmu_pwm.out_gpio, 1);
}

static void loki_resume_dfll_bypass(void)
{
/* enable PWM buffer operations */
<<<<<<< HEAD
	if (loki_cl_dvfs_data.u.pmu_pwm.out_gpio)
=======
	if (gpio_is_valid(loki_cl_dvfs_data.u.pmu_pwm.out_gpio))
>>>>>>> update/master
		__gpio_set_value(loki_cl_dvfs_data.u.pmu_pwm.out_gpio, 0);
}
static int __init loki_cl_dvfs_init(void)
{
	struct tegra_cl_dvfs_platform_data *data = NULL;
	struct board_info bi;

	tegra_get_board_info(&bi);
	if (bi.board_id == BOARD_P2530 && bi.fab >= 0xc0) {
<<<<<<< HEAD
		loki_cl_dvfs_data.u.pmu_pwm.out_gpio = 0;
=======
		loki_cl_dvfs_data.u.pmu_pwm.out_gpio =  -1;
>>>>>>> update/master
		loki_cl_dvfs_data.u.pmu_pwm.pwm_bus =
			TEGRA_CL_DVFS_PWM_1WIRE_DIRECT;
	}

	{
		data = &loki_cl_dvfs_data;

		data->u.pmu_pwm.pinctrl_dev = tegra_get_pinctrl_device_handle();
		if (!data->u.pmu_pwm.pinctrl_dev) {
			pr_err("%s: Tegra pincontrol driver not found\n",
				__func__);
			return -EINVAL;
		}

		data->u.pmu_pwm.pwm_pingroup =
			pinctrl_get_selector_from_group_name(
				data->u.pmu_pwm.pinctrl_dev, "dvfs_pwm_px0");
		if (data->u.pmu_pwm.pwm_pingroup < 0) {
			pr_err("%s: Tegra pin dvfs_pwm_px0 not found\n",
				__func__);
			return -EINVAL;
		}

		if (data->u.pmu_pwm.dfll_bypass_dev) {
			/* this has to be exact to 1uV level from table */
			loki_suspend_data.suspend_dfll_bypass =
				loki_suspend_dfll_bypass;
			loki_suspend_data.resume_dfll_bypass =
				loki_resume_dfll_bypass;
			platform_device_register(
				data->u.pmu_pwm.dfll_bypass_dev);
		} else {
			(void)loki_dfll_bypass_dev;
		}
	}

	data->flags = TEGRA_CL_DVFS_DYN_OUTPUT_CFG;
	tegra_cl_dvfs_device.dev.platform_data = data;
	platform_device_register(&tegra_cl_dvfs_device);
	return 0;
}
#else
static inline int loki_cl_dvfs_init()
{ return 0; }
#endif

int __init loki_rail_alignment_init(void)
{
#ifdef CONFIG_ARCH_TEGRA_13x_SOC
	tegra13x_vdd_cpu_align(LOKI_CPU_VDD_STEP_UV,
			LOKI_CPU_VDD_MIN_UV);
#else
	tegra12x_vdd_cpu_align(LOKI_CPU_VDD_STEP_UV,
			LOKI_CPU_VDD_MIN_UV);
#endif
	return 0;
}

int __init loki_regulator_init(void)
{
<<<<<<< HEAD
	void __iomem *pmc = IO_ADDRESS(TEGRA_PMC_BASE);
	u32 pmc_ctrl;
	int i;
	struct board_info bi;

	tegra_get_board_info(&bi);

	/* TPS65913: Normal state of INT request line is LOW.
	 * configure the power management controller to trigger PMU
	 * interrupts when HIGH.
	 */
	pmc_ctrl = readl(pmc + PMC_CTRL);
	writel(pmc_ctrl | PMC_CTRL_INTR_LOW, pmc + PMC_CTRL);
	for (i = 0; i < PALMAS_NUM_REGS ; i++) {
		pmic_platform.reg_data[i] = loki_reg_data[i];
		pmic_platform.reg_init[i] = loki_reg_init[i];
	}

	if (bi.board_id == BOARD_P2530 && bi.fab == 0xc0) {
		pmic_platform.reg_data[PALMAS_REG_SMPS123] = NULL;
		pmic_platform.reg_init[PALMAS_REG_SMPS123] = NULL;
	} else /* Set vdd_gpu init uV to 1V */
		reg_idata_smps123.constraints.init_uV = 1000000;
	reg_idata_smps9.constraints.enable_time = 250;


	if (bi.board_id == BOARD_P2530 && bi.fab >= 0xa1) {

		palmas_adc_pdata.auto_conversion_period_ms = 1000;
		palmas_adc_pdata.adc_auto_conv0_data =
					&palmas_adc_auto_conv0_data;

		if (bi.sku == BOARD_SKU_FOSTER) {
			pr_info("thermal: registering for foster\n");
			palmas_adc_pdata.iio_maps =
				palmas_adc_iio_maps_p2530_foster;
		} else if (bi.sku == BOARD_SKU_100 || bi.sku == BOARD_SKU_0) {
			pr_info("thermal: registering for loki\n");
			palmas_adc_pdata.iio_maps =
				palmas_adc_iio_maps_p2530_loki;
			palmas_adc_pdata.adc_auto_conv1_data =
					&palmas_adc_auto_conv1_data;
		} else {
			pr_err("palmas: Not a known SKU!\n");
		}
	}


	i2c_register_board_info(4, palma_device,
			ARRAY_SIZE(palma_device));
	if (bi.board_id == BOARD_P2530 && bi.fab >= 0xa1) {
		pmic_platform.reg_data[PALMAS_REG_SMPS7] =
			PALMAS_REG_PDATA(smps7_a01);
		pmic_platform.reg_init[PALMAS_REG_SMPS7] =
			PALMAS_REG_INIT_DATA(smps7_a01);
		pmic_platform.reg_data[PALMAS_REG_LDO1] =
			PALMAS_REG_PDATA(ldo1_a01);
		pmic_platform.reg_init[PALMAS_REG_LDO1] =
			PALMAS_REG_INIT_DATA(ldo1_a01);
		pmic_platform.reg_data[PALMAS_REG_LDO5] =
			PALMAS_REG_PDATA(ldo5_a01);
		pmic_platform.reg_init[PALMAS_REG_LDO5] =
			PALMAS_REG_INIT_DATA(ldo5_a01);
	}

=======
>>>>>>> update/master
	platform_device_register(&power_supply_extcon_device);

	loki_cl_dvfs_init();
	return 0;
}

<<<<<<< HEAD
static int __init loki_fixed_regulator_init(void)
{
	struct board_info pmu_board_info;
	struct board_info bi;


	if ((!of_machine_is_compatible("nvidia,loki")) &&
		(!of_machine_is_compatible("nvidia,t132loki")) &&
		(!of_machine_is_compatible("nvidia,foster")))
		return 0;

	tegra_get_board_info(&bi);
	tegra_get_pmu_board_info(&pmu_board_info);

	if (bi.board_id == BOARD_P2530) {
		fixed_reg_en_vdd_hdmi_5v0_pdata.gpio = TEGRA_GPIO_PFF0;
		fixed_reg_en_vdd_hdmi_5v0_pdata.enable_high = false;
	}
	if (pmu_board_info.board_id == BOARD_E2545)
		return platform_add_devices(fixed_reg_devs_e2545,
			ARRAY_SIZE(fixed_reg_devs_e2545));
	return 0;
}

subsys_initcall_sync(loki_fixed_regulator_init);
int __init loki_edp_init(void)
{
	unsigned int regulator_mA;

	regulator_mA = get_maximum_cpu_current_supported();
	if (!regulator_mA)
		regulator_mA = 14000;

	pr_info("%s: CPU regulator %d mA\n", __func__, regulator_mA);
	tegra_init_cpu_edp_limits(regulator_mA);

	/* gpu maximum current */
	regulator_mA = 14000;
	pr_info("%s: GPU regulator %d mA\n", __func__, regulator_mA);

	tegra_init_gpu_edp_limits(regulator_mA);
	return 0;
}

=======
>>>>>>> update/master
static struct pid_thermal_gov_params soctherm_pid_params = {
	.max_err_temp = 9000,
	.max_err_gain = 1000,

	.gain_p = 1000,
	.gain_d = 0,

	.up_compensation = 20,
	.down_compensation = 20,
};

static struct thermal_zone_params soctherm_tzp = {
	.governor_name = "pid_thermal_gov",
	.governor_params = &soctherm_pid_params,
};

static struct tegra_thermtrip_pmic_data tpdata_palmas = {
	.reset_tegra = 1,
	.pmu_16bit_ops = 0,
	.controller_type = 0,
	.pmu_i2c_addr = 0x58,
	.i2c_controller_id = 4,
	.poweroff_reg_addr = 0xa0,
	.poweroff_reg_data = 0x0,
};

static struct soctherm_platform_data loki_soctherm_data = {
	.therm = {
		[THERM_CPU] = {
			.zone_enable = true,
			.passive_delay = 1000,
			.hotspot_offset = 6000,
			.num_trips = 3,
			.trips = {
				{
					.cdev_type = "tegra-shutdown",
					.trip_temp = 101000,
					.trip_type = THERMAL_TRIP_CRITICAL,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
				{
					.cdev_type = "tegra-heavy",
					.trip_temp = 98000,
					.trip_type = THERMAL_TRIP_HOT,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
				{
					.cdev_type = "tegra-balanced",
					.trip_temp = 88000,
					.trip_type = THERMAL_TRIP_PASSIVE,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
			},
			.tzp = &soctherm_tzp,
		},
		[THERM_GPU] = {
			.zone_enable = true,
			.passive_delay = 1000,
			.hotspot_offset = 6000,
			.num_trips = 3,
			.trips = {
				{
					.cdev_type = "tegra-shutdown",
					.trip_temp = 98000,
					.trip_type = THERMAL_TRIP_CRITICAL,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
				{
					.cdev_type = "tegra-heavy",
					.trip_temp = 95000,
					.trip_type = THERMAL_TRIP_HOT,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
				{
					.cdev_type = "tegra-balanced",
					.trip_temp = 85000,
					.trip_type = THERMAL_TRIP_PASSIVE,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
			},
			.tzp = &soctherm_tzp,
		},
		[THERM_MEM] = {
			.zone_enable = true,
			.num_trips = 1,
			.trips = {
				{
					.cdev_type = "tegra-shutdown",
					.trip_temp = 103000, /* = GPU shut */
					.trip_type = THERMAL_TRIP_CRITICAL,
					.upper = THERMAL_NO_LIMIT,
					.lower = THERMAL_NO_LIMIT,
				},
			},
		},
		[THERM_PLL] = {
			.zone_enable = true,
			.tzp = &soctherm_tzp,
		},
	},
	.throttle = {
		[THROTTLE_HEAVY] = {
			.priority = 100,
			.devs = {
				[THROTTLE_DEV_CPU] = {
					.enable = true,
					.depth = 80,
				},
				[THROTTLE_DEV_GPU] = {
					.enable = true,
					.throttling_depth = "heavy_throttling",
				},
			},
		},
		[THROTTLE_OC2] = {
			.throt_mode = BRIEF,
			.polarity = 1,
			.intr = false,
			.devs = {
				[THROTTLE_DEV_CPU] = {
					.enable = true,
					.depth = 30,
				},
				[THROTTLE_DEV_GPU] = {
					.enable = true,
					.throttling_depth = "heavy_throttling",
				},
			},
		},
	},
	.tshut_pmu_trip_data = &tpdata_palmas,
};

int __init loki_soctherm_init(void)
{
	/* do this only for supported CP,FT fuses */
	if ((tegra_fuse_calib_base_get_cp(NULL, NULL) >= 0) &&
	    (tegra_fuse_calib_base_get_ft(NULL, NULL) >= 0)) {
		tegra_platform_edp_init(
			loki_soctherm_data.therm[THERM_CPU].trips,
			&loki_soctherm_data.therm[THERM_CPU].num_trips,
			8000); /* edp temperature margin */
		tegra_platform_gpu_edp_init(
			loki_soctherm_data.therm[THERM_GPU].trips,
			&loki_soctherm_data.therm[THERM_GPU].num_trips,
			8000);
		tegra_add_cpu_vmax_trips(
			loki_soctherm_data.therm[THERM_CPU].trips,
			&loki_soctherm_data.therm[THERM_CPU].num_trips);
		tegra_add_tgpu_trips(
			loki_soctherm_data.therm[THERM_GPU].trips,
			&loki_soctherm_data.therm[THERM_GPU].num_trips);
		tegra_add_vc_trips(
			loki_soctherm_data.therm[THERM_CPU].trips,
			&loki_soctherm_data.therm[THERM_CPU].num_trips);
		tegra_add_core_vmax_trips(
			loki_soctherm_data.therm[THERM_PLL].trips,
			&loki_soctherm_data.therm[THERM_PLL].num_trips);
		tegra_add_cpu_vmin_trips(
			loki_soctherm_data.therm[THERM_CPU].trips,
			&loki_soctherm_data.therm[THERM_CPU].num_trips);
		tegra_add_gpu_vmin_trips(
			loki_soctherm_data.therm[THERM_GPU].trips,
			&loki_soctherm_data.therm[THERM_GPU].num_trips);
		/*PLL soctherm is being used for SOC vmin*/
		tegra_add_core_vmin_trips(
			loki_soctherm_data.therm[THERM_PLL].trips,
			&loki_soctherm_data.therm[THERM_PLL].num_trips);
	}

	return tegra_soctherm_init(&loki_soctherm_data);
}
