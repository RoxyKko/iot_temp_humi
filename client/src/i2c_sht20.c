/*
 * @Author: RoxyKko
 * @Date: 2023-03-24 18:50:49
 * @LastEditors: RoxyKko
 * @LastEditTime: 2023-04-04 17:35:27
 * @Description: sht20驱动
 */
#include <i2c_sht20.h>

/**
 * @name: static inline void msleep(unsigned long ms)
 * @description: 休眠函数
 * @param {unsigned long} ms
 * @return {*}
 */
static inline void msleep(unsigned long ms)
{
    struct timespec cSleep;
    unsigned long ulTmp;

    cSleep.tv_sec = ms / 1000;

    if (cSleep.tv_sec == 0)
    {
        ulTmp = ms * 10000;
        cSleep.tv_nsec = ulTmp * 100;
    }
    else
    {
        cSleep.tv_nsec = 0;
    }
    nanosleep(&cSleep, 0);
}

/**
 * @name: static inline void dump_buf(const char *prompt, uint8_t *buf, int len)
 * @description: 打印缓冲区
 * @param {char} *prompt 提示
 * @param {uint8_t} *buf 缓冲区
 * @param {int} len 长度
 * @return {*}
 */
static inline void dump_buf(const char *prompt, uint8_t *buf, int size)
{
    int i;

    if (!buf)
    {
        return;
    }

    if (prompt)
    {
        printf("%s", prompt);
    }

    for (i = 0; i < size; i++)
    {
        printf("%02x ", buf[i]);
    }
    printf("\n");

    return;
}

/**
 * @name: int sht2x_init(void)
 * @description: 初始化sht20
 * @return fd 返回文件描述符
 */
int sht2x_init(void)
{
    int fd;

    if ((fd = open("/dev/i2c-1", O_RDWR)) < 0)
    {
        printf("i2c device open failed: %s\n", strerror(errno));
        return -1;
    }

    if (sht2x_softReset(fd) < 0)
    {
        printf("sht2x softReset failed!\n");
        return -2;
    }

    return fd;
}

/**
 * @name: int sht2x_softReset(int fd)
 * @description: sht20软件复位
 * @param {int} fd
 * @return {*}
 */
int sht2x_softReset(int fd)
{
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data sht2x_data;
    uint8_t buf[2];

    if (fd < 0)
    {
        printf("%s line [%d] %s() get invalid input arguments\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    msg.addr = 0x40;
    msg.flags = 0; // write
    msg.len = 1;
    msg.buf = buf;
    msg.buf[0] = CMD_SOFT_RESET;

    sht2x_data.nmsgs = 1;
    sht2x_data.msgs = &msg;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }

    msleep(50);
    return 0;
}

/**
 * @name: int sht2x_get_temp_humidity(int fd, float *temp, float *rh)
 * @description: 从sht20获取温度和湿度并打印
 * @param {int} fd  文件描述符
 * @param {float} *temp 温度参数
 * @param {float} *rh   湿度参数
 * @return {*}
 */
int sht2x_get_temp_humidity(int fd, float *temp, float *rh)
{
    struct i2c_msg msg;
    struct i2c_rdwr_ioctl_data sht2x_data;
    uint8_t buf[4];

    if (fd < 0)
    {
        printf("%s line [%d] %s() get invalid input arguments\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    /*+------------------------------------------+
     *| measure and get temperature |
     *+------------------------------------------+*/

    msg.addr = 0x40;
    msg.flags = 0; // write
    msg.len = 1;
    msg.buf = buf;
    msg.buf[0] = CMD_TRIGGER_TEMP_NOHOLD; /* trigger temperature without hold I2C bus */

    sht2x_data.nmsgs = 1;
    sht2x_data.msgs = &msg;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }

    msleep(85);

    memset(buf, 0, sizeof(buf));
    msg.addr = 0x40;
    msg.flags = I2C_M_RD; // write
    msg.len = 3;
    msg.buf = buf;

    sht2x_data.nmsgs = 1;
    sht2x_data.msgs = &msg;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }

    // dump_buf("Temperature sample data: ", buf, 3);
    *temp = 175.72 * (((((int)buf[0]) << 8) + buf[1]) / 65536.0) - 46.85;

    /*+------------------------------------------+
     *| measure and get relative humidity |
     *+------------------------------------------+*/

    msg.addr = 0x40;
    msg.flags = 0; // write
    msg.len = 1;
    msg.buf = buf;
    msg.buf[0] = CMD_TRIGGER_HUMI_NOHOLD; /* trigger humidity without hold I2C bus */

    sht2x_data.nmsgs = 1;
    sht2x_data.msgs = &msg;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }

    msleep(29);

    memset(buf, 0, sizeof(buf));
    msg.addr = 0x40;
    msg.flags = I2C_M_RD; // write
    msg.len = 3;
    msg.buf = buf;

    sht2x_data.nmsgs = 1;
    sht2x_data.msgs = &msg;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }
    // dump_buf("Relative humidity sample data: ", buf, 3);

    *rh = 125 * (((((int)buf[0]) << 8) + buf[1]) / 65536.0) - 6;
    return 0;
}

/**
 * @name: int sht2x_get_serialNumber(int fd, uint8_t *serialNumber, int size)
 * @description: SHT20获取序列号函数
 * @param {int} fd  文件描述符
 * @param {uint8_t} *serialNumber 序列号参数
 * @param {int} size 序列号长度
 * @return {*}
 */
int sht2x_get_serialNumber(int fd, uint8_t *serialnumber, int size)
{
    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data sht2x_data;
    uint8_t sbuf[2];
    uint8_t rbuf[4];

    if (fd < 0)
    {
        printf("%s line [%d] %s() get invalid input arguments\n", __FILE__, __LINE__, __func__);
        return -1;
    }

    /*+------------------------------------------+
     *| Read SerialNumber from Location 1 |
     *+------------------------------------------+*/

    msgs[0].addr = 0x40;
    msgs[0].flags = 0; // write
    msgs[0].len = 2;
    msgs[0].buf = sbuf;

    msgs[0].buf[0] = 0xfa; /* command for readout on-chip memory */
    msgs[0].buf[1] = 0x0f; /* on-chip memory address */

    msgs[1].addr = 0x40;
    msgs[1].flags = I2C_M_RD; // write
    msgs[1].len = 4;
    msgs[1].buf = rbuf;

    sht2x_data.nmsgs = 2;
    sht2x_data.msgs = msgs;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }

    serialnumber[5] = rbuf[0]; /* Read SNB_3 */
    serialnumber[4] = rbuf[1]; /* Read SNB_2 */
    serialnumber[3] = rbuf[2]; /* Read SNB_1 */
    serialnumber[2] = rbuf[3]; /* Read SNB_0 */

    /*+------------------------------------------+
     *| Read SerialNumber from Location 2 |
     *+------------------------------------------+*/

    msgs[0].addr = 0x40;
    msgs[0].flags = 0; // write
    msgs[0].len = 2;
    msgs[0].buf = sbuf;

    msgs[0].buf[0] = 0xfc; /* command for readout on-chip memory */
    msgs[0].buf[1] = 0xc9; /* on-chip memory address */

    msgs[1].addr = 0x40;
    msgs[1].flags = I2C_M_RD; // write
    msgs[1].len = 4;
    msgs[1].buf = rbuf;

    sht2x_data.nmsgs = 2;
    sht2x_data.msgs = msgs;

    if (ioctl(fd, I2C_RDWR, &sht2x_data) < 0)
    {
        printf("%s() ioctl failure: %s\n", __func__, strerror(errno));
        return -2;
    }

    serialnumber[1] = rbuf[0]; /* Read SNC_1 */
    serialnumber[0] = rbuf[1]; /* Read SNC_0 */
    serialnumber[7] = rbuf[2]; /* Read SNA_1 */
    serialnumber[6] = rbuf[3]; /* Read SNA_0 */

    dump_buf("SHT2x Serial number: ", serialnumber, 8);
    return 0;
}