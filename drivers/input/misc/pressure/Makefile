#
# Makefile for stand-alone pressure input device drivers
#

GCOV_PROFILE := y

subdir-ccflags-y := -Werror
obj-$(CONFIG_NVI_INPUT_BMPx80) += inv-bmp180.o
inv-bmp180-objs += bmp180.o

obj-$(CONFIG_BMP280)     += bmp280_core.o BMP280.o
obj-$(CONFIG_BMP280_I2C) += bmp280_i2c.o
obj-$(CONFIG_BMP280_SPI) += bmp280_spi.o