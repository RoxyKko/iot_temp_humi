/*** 
 * @Author: RoxyKko
 * @Date: 2023-03-24 19:10:08
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-03-25 21:31:06
 * @Description: sht20驱动
 */
#ifndef _I2C_SHT20_IOCTL_H_
#define _I2C_SHT20_IOCTL_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <gpiod.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2C_BUS                     "/dev/i2c-1"    // 设备名
#define SHT20_ADDR                  0x40            // i2c设备物理地址
#define CHIP_NAME                   "/dev/gpiochip0"     // GPIO NAME

#define CMD_TRIGGER_TEMP_HOLD       0xE3            // 温度触发保持         8'b1110’0011 = 0xE3
#define CMD_TRIGGER_HUMI_HOLD       0xE5            // 湿度触发保持         8'b1110’0101 = 0xE5
#define CMD_TRIGGER_TEMP_NOHOLD     0xF3            // 温度触发不保持       8'b1111’0011 = 0xF3
#define CMD_TRIGGER_HUMI_NOHOLD     0xF5            // 湿度触发不保持       8'b1111’0101 = 0xF5
#define CMD_WRITE_USER_REG          0xE6            // 写用户寄存器         8'b1110’0110 = 0xE6
#define CMD_READ_USER_REG           0xE7            // 读用户寄存器         8'b1110’0111 = 0xE7
#define CMD_SOFT_RESET              0xFE            // 软复位               8'b1111’1110 = 0xFE

#define SHT20_MEASURING_DELAY 15                    // 上升沿延迟 15ms

#define I2C_API_RDWR                                // 使用i2c用户空间驱动Read/Write API (linux/i2c-dev.h)
// #define I2C_API_IOCTL                               // 使用i2c用户空间驱动LOCTL API (linux/i2c-dev.h)

int sht2x_init(void);
int sht2x_softReset(int fd);
int sht2x_get_temp_humidity(int fd, float *temp, float *rh);
int sht2x_get_serialNumber(int fd, uint8_t *serialNumber, int size);

#endif