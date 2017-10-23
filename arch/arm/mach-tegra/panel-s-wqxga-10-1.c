/*
 * arch/arm/mach-tegra/panel-s-wqxga-10-1.c
 *
 * Copyright (c) 2012-2014, NVIDIA CORPORATION. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mach/dc.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/regulator/consumer.h>

#include "board.h"
#include "board-panel.h"
#include "devices.h"
#include "gpio-names.h"

#define DSI_PANEL_RESET		1

static bool reg_requested;
static struct regulator *avdd_lcd_3v3;
static struct regulator *vdd_lcd_bl;
static struct regulator *vdd_lcd_bl_en;
static struct regulator *dvdd_lcd_1v8;
<<<<<<< HEAD
static struct regulator *vdd_ds_1v8;

#define en_vdd_bl	TEGRA_GPIO_PG0
#define lvds_en		TEGRA_GPIO_PG3

static struct tegra_dc_sd_settings dsi_s_wqxga_10_1_sd_settings = {
	.enable = 1, /* enabled by default. */
	.use_auto_pwm = false,
	.hw_update_delay = 0,
	.bin_width = -1,
	.aggressiveness = 5,
	.use_vid_luma = false,
	.phase_in_adjustments = 0,
	.k_limit_enable = true,
	.k_limit = 200,
	.sd_window_enable = false,
	.soft_clipping_enable = true,
	/* Low soft clipping threshold to compensate for aggressive k_limit */
	.soft_clipping_threshold = 128,
	.smooth_k_enable = true,
	.smooth_k_incr = 4,
	/* Default video coefficients */
	.coeff = {5, 9, 2},
	.fc = {0, 0},
	/* Immediate backlight changes */
	.blp = {1024, 255},
	/* Gammas: R: 2.2 G: 2.2 B: 2.2 */
	/* Default BL TF */
	.bltf = {
			{
				{57, 65, 73, 82},
				{92, 103, 114, 125},
				{138, 150, 164, 178},
				{193, 208, 224, 241},
			},
		},
	/* Default LUT */
	.lut = {
			{
				{255, 255, 255},
				{199, 199, 199},
				{153, 153, 153},
				{116, 116, 116},
				{85, 85, 85},
				{59, 59, 59},
				{36, 36, 36},
				{17, 17, 17},
				{0, 0, 0},
			},
		},
	.sd_brightness = &sd_brightness,
	.use_vpulse2 = true,
};

static tegra_dc_bl_output dsi_s_wqxga_10_1_bl_output_measured = {
	0, 1, 2, 3, 4, 5, 6, 7,
	8, 9, 11, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 21, 22,
	23, 24, 25, 26, 27, 28, 29, 30,
	31, 32, 32, 33, 34, 35, 36, 37,
	38, 39, 40, 41, 42, 43, 43, 44,
	45, 46, 47, 48, 49, 50, 51, 52,
	53, 54, 54, 55, 56, 57, 58, 59,
	60, 61, 62, 63, 63, 64, 65, 66,
	67, 68, 69, 70, 71, 72, 73, 74,
	75, 76, 77, 78, 79, 80, 80, 81,
	82, 83, 84, 85, 86, 87, 88, 89,
	90, 91, 92, 93, 94, 95, 96, 97,
	98, 99, 100, 101, 102, 103, 104, 105,
	106, 107, 108, 109, 110, 111, 112, 113,
	114, 115, 116, 117, 118, 119, 120, 121,
	122, 123, 124, 125, 126, 127, 128, 129,
	130, 131, 132, 133, 134, 135, 136, 137,
	138, 140, 141, 142, 143, 144, 145, 146,
	147, 148, 149, 150, 151, 152, 153, 154,
	155, 156, 157, 158, 159, 160, 161, 162,
	163, 164, 165, 166, 167, 168, 169, 170,
	171, 172, 173, 174, 175, 177, 178, 179,
	180, 181, 182, 183, 184, 185, 186, 187,
	188, 189, 190, 191, 192, 193, 194, 195,
	196, 197, 198, 200, 201, 202, 203, 204,
	205, 206, 207, 208, 209, 210, 211, 212,
	213, 214, 215, 217, 218, 219, 220, 221,
	222, 223, 224, 225, 226, 227, 228, 229,
	230, 231, 232, 234, 235, 236, 237, 238,
	239, 240, 241, 242, 243, 244, 245, 246,
	248, 249, 250, 251, 252, 253, 254, 255,
};

#if DC_CTRL_MODE & TEGRA_DC_OUT_ONE_SHOT_MODE
static u8 fbuf_mode_sel[] = {0x10, 0x00, 0x2A}; /* left-right */
#else
static u8 fbuf_mode_sel[] = {0x10, 0x00, 0x17}; /* odd-even, DRAM through */
#endif

static u8 __maybe_unused mipi_if_sel[] = {0x10, 0x01, 0x01}; /* cmd mode */
static u8 __maybe_unused smode[] = {0x10, 0x07, 0x07}; /* vsync mode */

static struct tegra_dsi_cmd dsi_s_wqxga_10_1_init_cmd[] = {
	DSI_CMD_LONG(DSI_GENERIC_LONG_WRITE, fbuf_mode_sel),
	DSI_DLY_MS(20),
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_NO_OP, 0x0),
	DSI_DLY_MS(20),

#if DC_CTRL_MODE & TEGRA_DC_OUT_ONE_SHOT_MODE
	DSI_CMD_LONG(DSI_GENERIC_LONG_WRITE, mipi_if_sel),
	DSI_DLY_MS(20),
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_NO_OP, 0x0),
	DSI_DLY_MS(20),
#endif

#if !(DC_CTRL_MODE & TEGRA_DC_OUT_ONE_SHOT_MODE)
	DSI_CMD_LONG(DSI_GENERIC_LONG_WRITE, smode),
	DSI_DLY_MS(20),
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_NO_OP, 0x0),
	DSI_DLY_MS(20),
#endif

#if DC_CTRL_MODE & TEGRA_DC_OUT_ONE_SHOT_MODE
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM,
			DSI_DCS_SET_TEARING_EFFECT_ON, 0x0),
	DSI_DLY_MS(20),
#endif
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_EXIT_SLEEP_MODE, 0x0),
	DSI_DLY_MS(120),
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_SET_DISPLAY_ON, 0x0),
	DSI_DLY_MS(20),
	DSI_SEND_FRAME(1),
	DSI_DLY_MS(120),
};

static struct tegra_dsi_cmd dsi_s_wqxga_10_1_suspend_cmd[] = {
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_SET_DISPLAY_OFF, 0x0),
	DSI_DLY_MS(50),
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM, DSI_DCS_ENTER_SLEEP_MODE, 0x0),
	DSI_DLY_MS(200),
#if DC_CTRL_MODE & TEGRA_DC_OUT_ONE_SHOT_MODE
	DSI_CMD_SHORT(DSI_DCS_WRITE_0_PARAM,
			DSI_DCS_SET_TEARING_EFFECT_OFF, 0x0),
	DSI_DLY_MS(20),
#endif
};

static struct tegra_dsi_out dsi_s_wqxga_10_1_pdata = {
	.controller_vs = DSI_VS_1,

	.n_data_lanes = 8,

#if DC_CTRL_MODE & TEGRA_DC_OUT_ONE_SHOT_MODE
	.video_data_type = TEGRA_DSI_VIDEO_TYPE_COMMAND_MODE,
	.ganged_type = TEGRA_DSI_GANGED_SYMMETRIC_LEFT_RIGHT,
	.suspend_aggr = DSI_HOST_SUSPEND_LV2,
	.refresh_rate = 61,
	.rated_refresh_rate = 60,
	.te_polarity_low = true,
#else
	.ganged_type = TEGRA_DSI_GANGED_SYMMETRIC_EVEN_ODD,
	.video_data_type = TEGRA_DSI_VIDEO_TYPE_VIDEO_MODE,
	.video_burst_mode = TEGRA_DSI_VIDEO_NONE_BURST_MODE,
	.refresh_rate = 60,
#endif

	.pixel_format = TEGRA_DSI_PIXEL_FORMAT_24BIT_P,
	.virtual_channel = TEGRA_DSI_VIRTUAL_CHANNEL_0,

	.panel_reset = DSI_PANEL_RESET,
	.power_saving_suspend = true,
	.video_clock_mode = TEGRA_DSI_VIDEO_CLOCK_TX_ONLY,
	.dsi_init_cmd = dsi_s_wqxga_10_1_init_cmd,
	.n_init_cmd = ARRAY_SIZE(dsi_s_wqxga_10_1_init_cmd),
	.dsi_suspend_cmd = dsi_s_wqxga_10_1_suspend_cmd,
	.n_suspend_cmd = ARRAY_SIZE(dsi_s_wqxga_10_1_suspend_cmd),
	.bl_name = "pwm-backlight",
	.lp00_pre_panel_wakeup = true,
	.ulpm_not_supported = true,
	.no_pkt_seq_hbp = true,
};
=======
static u16 en_panel_rst;
>>>>>>> update/master

static int dalmore_dsi_regulator_get(struct device *dev)
{
	int err = 0;

	if (reg_requested)
		return 0;
	dvdd_lcd_1v8 = regulator_get(dev, "dvdd_lcd");
	if (IS_ERR(dvdd_lcd_1v8)) {
		pr_err("dvdd_lcd regulator get failed\n");
		err = PTR_ERR(dvdd_lcd_1v8);
		dvdd_lcd_1v8 = NULL;
		goto fail;
	}
	avdd_lcd_3v3 = regulator_get(dev, "avdd_lcd");
	if (IS_ERR(avdd_lcd_3v3)) {
		pr_err("avdd_lcd regulator get failed\n");
		err = PTR_ERR(avdd_lcd_3v3);
		avdd_lcd_3v3 = NULL;
		goto fail;
	}

	vdd_lcd_bl = regulator_get(dev, "vdd_lcd_bl");
	if (IS_ERR(vdd_lcd_bl)) {
		pr_err("vdd_lcd_bl regulator get failed\n");
		err = PTR_ERR(vdd_lcd_bl);
		vdd_lcd_bl = NULL;
		goto fail;
	}

	vdd_lcd_bl_en = regulator_get(dev, "vdd_lcd_bl_en");
	if (IS_ERR(vdd_lcd_bl_en)) {
		pr_err("vdd_lcd_bl_en regulator get failed\n");
		err = PTR_ERR(vdd_lcd_bl_en);
		vdd_lcd_bl_en = NULL;
		goto fail;
	}
	reg_requested = true;
	return 0;
fail:
	return err;
}

static int dsi_s_wqxga_10_1_postpoweron(struct device *dev)
{
	int err = 0;

	err = dalmore_dsi_regulator_get(dev);
	if (err < 0) {
		pr_err("dsi regulator get failed\n");
		goto fail;
	}

	err = tegra_panel_gpio_get_dt("s,wqxga-10-1", &panel_of);
	if (err < 0) {
		pr_err("dsi gpio request failed\n");
		goto fail;
	}

	/* If panel rst gpio is specified in device tree,
	 * use that.
	 */
	if (gpio_is_valid(panel_of.panel_gpio[TEGRA_GPIO_RESET]))
		en_panel_rst = panel_of.panel_gpio[TEGRA_GPIO_RESET];

	if (dvdd_lcd_1v8) {
		err = regulator_enable(dvdd_lcd_1v8);
		if (err < 0) {
			pr_err("dvdd_lcd regulator enable failed\n");
			goto fail;
		}
	}

	if (avdd_lcd_3v3) {
		err = regulator_enable(avdd_lcd_3v3);
		if (err < 0) {
			pr_err("avdd_lcd regulator enable failed\n");
			goto fail;
		}
	}

	/* panel ic requirement after vcc enable */
	msleep(260);

	if (vdd_lcd_bl) {
		err = regulator_enable(vdd_lcd_bl);
		if (err < 0) {
			pr_err("vdd_lcd_bl regulator enable failed\n");
			goto fail;
		}
	}

	if (vdd_lcd_bl_en) {
		err = regulator_enable(vdd_lcd_bl_en);
		if (err < 0) {
			pr_err("vdd_lcd_bl_en regulator enable failed\n");
			goto fail;
		}
	}

	msleep(20);
#if DSI_PANEL_RESET
	/* use platform data */
	gpio_direction_output(en_panel_rst, 1);
	usleep_range(1000, 5000);
	gpio_set_value(en_panel_rst, 0);
	usleep_range(1000, 5000);
	gpio_set_value(en_panel_rst, 1);
	msleep(20);
#endif

	return 0;
fail:
	return err;
}

static int dsi_s_wqxga_10_1_enable(struct device *dev)
{
	return 0;
}

static int dsi_s_wqxga_10_1_disable(struct device *dev)
{
	if (vdd_lcd_bl)
		regulator_disable(vdd_lcd_bl);

	if (vdd_lcd_bl_en)
		regulator_disable(vdd_lcd_bl_en);

	if (avdd_lcd_3v3)
		regulator_disable(avdd_lcd_3v3);

	if (dvdd_lcd_1v8)
		regulator_disable(dvdd_lcd_1v8);

	return 0;
}

static int dsi_s_wqxga_10_1_postsuspend(void)
{
	return 0;
}

static int dsi_s_wqxga_10_1_bl_notify(struct device *dev, int brightness)
{
	struct backlight_device *bl = NULL;
	struct pwm_bl_data *pb = NULL;
	int cur_sd_brightness = atomic_read(&sd_brightness);
	bl = (struct backlight_device *)dev_get_drvdata(dev);
	pb = (struct pwm_bl_data *)dev_get_drvdata(&bl->dev);

	/* SD brightness is a percentage */
	brightness = (brightness * cur_sd_brightness) / 255;

	/* Apply any backlight response curve */
	if (brightness > 255)
		pr_info("Error: Brightness > 255!\n");
	else if (pb->bl_measured)
		brightness = pb->bl_measured[brightness];

	return brightness;
}

static int dsi_s_wqxga_10_1_check_fb(struct device *dev, struct fb_info *info)
{
	struct platform_device *pdev = NULL;
	pdev = to_platform_device(bus_find_device_by_name(
		&platform_bus_type, NULL, "tegradc.0"));
	return info->device == &pdev->dev;
}

static struct pwm_bl_data_dt_ops  dsi_s_wqxga_10_1_pwm_bl_ops = {
	.notify = dsi_s_wqxga_10_1_bl_notify,
	.check_fb = dsi_s_wqxga_10_1_check_fb,
	.blnode_compatible = "s,wqxga-10-1-bl",
};

<<<<<<< HEAD
static struct platform_device __maybe_unused
		dsi_s_wqxga_10_1_bl_device = {
	.name	= "pwm-backlight",
	.id	= -1,
	.dev	= {
		.platform_data = &dsi_s_wqxga_10_1_bl_data,
	},
};

static struct platform_device __maybe_unused
			*dsi_s_wqxga_10_1_bl_devices[] __initdata = {
	&dsi_s_wqxga_10_1_bl_device,
};

static int __init dsi_s_wqxga_10_1_register_bl_dev(void)
{
	int err = 0;

	err = platform_add_devices(dsi_s_wqxga_10_1_bl_devices,
				ARRAY_SIZE(dsi_s_wqxga_10_1_bl_devices));
	if (err) {
		pr_err("disp1 bl device registration failed");
		return err;
	}
	return err;
}

static void dsi_s_wqxga_10_1_set_disp_device(
	struct platform_device *dalmore_display_device)
{
	disp_device = dalmore_display_device;
}

static void dsi_s_wqxga_10_1_dc_out_init(struct tegra_dc_out *dc)
{
	dc->dsi = &dsi_s_wqxga_10_1_pdata;
	dc->parent_clk = "pll_d_out0";
	dc->modes = dsi_s_wqxga_10_1_modes;
	dc->n_modes = ARRAY_SIZE(dsi_s_wqxga_10_1_modes);
	dc->enable = dsi_s_wqxga_10_1_enable;
	dc->postpoweron = dsi_s_wqxga_10_1_postpoweron;
	dc->disable = dsi_s_wqxga_10_1_disable;
	dc->postsuspend	= dsi_s_wqxga_10_1_postsuspend,
	dc->width = 216;
	dc->height = 135;
	dc->flags = DC_CTRL_MODE;
}

static void dsi_s_wqxga_10_1_fb_data_init(struct tegra_fb_data *fb)
{
	fb->xres = dsi_s_wqxga_10_1_modes[0].h_active;
	fb->yres = dsi_s_wqxga_10_1_modes[0].v_active;
}

static void
dsi_s_wqxga_10_1_sd_settings_init(struct tegra_dc_sd_settings *settings)
{
	*settings = dsi_s_wqxga_10_1_sd_settings;
	settings->bl_device_name = "pwm-backlight";
}

#ifdef CONFIG_TEGRA_DC_CMU
static void dsi_s_wqxga_10_1_cmu_init(struct tegra_dc_platform_data *pdata)
{
	pdata->cmu = &dsi_s_wqxga_10_1_cmu;
}
#endif

=======
>>>>>>> update/master
struct tegra_panel_ops dsi_s_wqxga_10_1_ops = {
	.enable = dsi_s_wqxga_10_1_enable,
	.disable = dsi_s_wqxga_10_1_disable,
	.postpoweron = dsi_s_wqxga_10_1_postpoweron,
	.postsuspend = dsi_s_wqxga_10_1_postsuspend,
	.pwm_bl_ops = &dsi_s_wqxga_10_1_pwm_bl_ops,
};
<<<<<<< HEAD

struct tegra_panel __initdata dsi_s_wqxga_10_1 = {
	.init_sd_settings = dsi_s_wqxga_10_1_sd_settings_init,
	.init_dc_out = dsi_s_wqxga_10_1_dc_out_init,
	.init_fb_data = dsi_s_wqxga_10_1_fb_data_init,
	.register_bl_dev = dsi_s_wqxga_10_1_register_bl_dev,
#ifdef CONFIG_TEGRA_DC_CMU
	.init_cmu_data = dsi_s_wqxga_10_1_cmu_init,
#endif
	.set_disp_device = dsi_s_wqxga_10_1_set_disp_device,
};
EXPORT_SYMBOL(dsi_s_wqxga_10_1);

=======
>>>>>>> update/master
