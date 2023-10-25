#include <ros/ros.h>
#include <stdio.h>
#include "std_msgs/String.h"
#include <sensor_msgs/TimeReference.h>

#include <sys/time.h>
#include <linux/hidraw.h>
#include "ch347_lib.h"

using namespace std;

typedef enum _CH347FUNCTYPE {
    FUNC_UART = 0,
    FUNC_SPI_I2C_GPIO,
    FUNC_JTAG_GPIO,
} CH347FUNCTYPE;

struct ch34x {
    int fd;
    char version[100];
    CHIP_TYPE chiptype;
    uint32_t dev_id;
    CH347FUNCTYPE functype;
};

static struct ch34x ch347device;

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("<<Usage: sudo %s [device]\n", argv[0]);
        return -1;
    }

    bool ret;

    /* open device */
    ch347device.fd = CH347OpenDevice(argv[1]);
    if (ch347device.fd < 0) {
        printf("<<CH347OpenDevice false.\n");
        return -1;
    }
    printf("<<Open device %s succeed, fd: %d\n", argv[1], ch347device.fd);

    if (strstr(argv[1], "ch34x_pis")) {
        ret = CH34xSetTimeout(ch347device.fd, 2000, 2000);
        if (ret == false) {
            printf("<<CH34xSetTimeout false.\n");
            return -1;
        }
    }

    switch (ch347device.functype) {
        case FUNC_UART: {
            printf("<<Open device in FUNC_UART, break\n");
            break;
        }
        case FUNC_SPI_I2C_GPIO: {
            //初始化节点
            ros::init(argc, argv, "PubPPS");
            //声明节点句柄
            ros::NodeHandle nh;
            //发布主题, 消息格式使用sensor_msg::Imu标准格式（topic名称，队列长度）
            ros::Publisher IMU_read_pub = nh.advertise<sensor_msgs::TimeReference>("/gpspps", 10);

            printf("<<Open device in FUNC_SPI_I2C_GPIO, continue\n");

            uint8_t oInBuffer[5] = {0};//[0]:原引脚方向,[1]:电平状态,[2]:IO方向0in1out,[3]:输出数据0低1高
            oInBuffer[2] = 0xfe, oInBuffer[3] = 0xfe;//1111110,1-7输出,0输入
            int LastSecond = 0, NowSecond, LastMS = 0, NowMS, PastMS, HaveFrameInSecond = 0, FpsCamTriger = 3;
            bool bGPSTriger = true;

            ros::Rate loop_rate(200);
            while (ros::ok()) {
                if (CH347GPIO_Get(ch347device.fd, &oInBuffer[0], &oInBuffer[1]) == false) {
                    printf("CH347GPIO_Get Return error\n");
                    return (false);
                } else {
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    NowSecond = tv.tv_sec;  //秒
                    NowMS = tv.tv_usec / 1000;  //毫秒
                    PastMS = ((NowMS - LastMS) >= 0 ? (NowMS - LastMS) : (NowMS - LastMS) + 1000);
                    if ((((oInBuffer[1] >> 0) & 1) && (NowSecond != LastSecond) && bGPSTriger) ||
                        (PastMS > 998 && !bGPSTriger)) {
                        printf("NowSecond:%d,LastSecond:%d,PastMS:%d\n", NowSecond, LastSecond, PastMS);
                        LastSecond = NowSecond;
                        LastMS = NowMS;
                        HaveFrameInSecond = 0;
                    }
                    if ((HaveFrameInSecond * (1000 / FpsCamTriger) < PastMS && HaveFrameInSecond < FpsCamTriger) ||
                        (HaveFrameInSecond == 0)) {
                        CH347GPIO_Set(ch347device.fd, 0xff, oInBuffer[2], oInBuffer[3]);
                        oInBuffer[3] = ~oInBuffer[3];
                        usleep(5000);//CamTrigerTime bigger=5ms>4ms
                        CH347GPIO_Set(ch347device.fd, 0xff, oInBuffer[2], oInBuffer[3]);
                        oInBuffer[3] = ~oInBuffer[3];
                        HaveFrameInSecond++;
                    }
                }

                //处理ROS的信息，比如订阅消息,并调用回调函数
                ros::spinOnce();
                loop_rate.sleep();
            }
            break;
        }
        case FUNC_JTAG_GPIO: {
            printf("<<Open device in FUNC_JTAG_GPIO, break\n");
            break;
        }
        default:
            break;
    }

    /* close the device */
    if (CH347CloseDevice(ch347device.fd)) {
        printf("<<Close device succeed.\n");
    }

    return 0;
}
