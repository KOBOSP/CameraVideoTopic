#include "ros/ros.h"
#include "std_msgs/String.h"
#include <serial/serial.h>
#include <sensor_msgs/Imu.h>
#include <sstream>

using namespace std;
serial::Serial ser; //声明串口对象

int main(int argc, char **argv) {
    //初始化节点
    ros::init(argc, argv, "PubIMU");
    //声明节点句柄
    ros::NodeHandle nh;
    //发布主题, 消息格式使用sensor_msg::Imu标准格式（topic名称，队列长度）
    ros::Publisher IMU_read_pub = nh.advertise<sensor_msgs::Imu>("imu_data", 1000);

    //打开串口
    try {
        //设置串口属性，并打开串口
        ser.setPort("/dev/ttyUSB0");
        ser.setBaudrate(115200);
        serial::Timeout to = serial::Timeout::simpleTimeout(1000);
        ser.setTimeout(to);
        ser.open();
    }
    catch (serial::IOException &e) {
        ROS_ERROR_STREAM("Unable to open port ");
        return -1;
    }

    //检测串口是否已经打开，并给出提示信息
    if (ser.isOpen()) {
        ROS_INFO_STREAM("Serial Port initialized");
    } else {
        return -1;
    }
    bool bFindStart = false;
    uint8_t data_size = 36;//4 444 444 4 2 2
    int32_t data_imu = 0, data_last_cntr = 0;
    unsigned char tmpdata[data_size] = {0};
    ros::Rate loop_rate(200);
    while (ros::ok()) {
        if (data_size < ser.available()) {//ser.available(当串口没有缓存时，这个函数会一直等到有缓存才返回字符数
            while (true) {
                if (tmpdata[0] == '$' && tmpdata[1] == 0
                    && tmpdata[2] == 0 && tmpdata[3] == 0
                    && tmpdata[34] == '#' && tmpdata[35] == 0) {
                    break;
                } else {
                    for (uint8_t i = 0; i < data_size - 1; i++) {
                        tmpdata[i] = tmpdata[i + 1];
                    }
                    ser.read((unsigned char *) &tmpdata[data_size - 1], 1);
                }
            }
            ser.read((unsigned char *) tmpdata, data_size);
            sensor_msgs::Imu imu_data;//打包IMU数据
            imu_data.header.stamp = ros::Time::now();
            imu_data.header.frame_id = "base_link";

            data_imu = tmpdata[32] | (tmpdata[33] << 8);
            imu_data.orientation.y = (float) data_imu;//cntr
            if (data_last_cntr != data_imu) {
                if (data_imu - data_last_cntr != 1) {
                    cout << "data_imu:" << data_imu << ", data_last_cntr:" << data_last_cntr << endl;
                }
                data_last_cntr = data_imu;
                data_imu = tmpdata[4] | (tmpdata[5] << 8) | tmpdata[6] << 16 | (tmpdata[7] << 24);
                imu_data.angular_velocity.x = (float) data_imu * 0.025 / 65536.0;
                data_imu = tmpdata[8] | (tmpdata[9] << 8) | tmpdata[10] << 16 | (tmpdata[11] << 24);
                imu_data.angular_velocity.y = (float) data_imu * 0.025 / 65536.0;
                data_imu = tmpdata[12] | (tmpdata[13] << 8) | tmpdata[14] << 16 | (tmpdata[15] << 24);
                imu_data.angular_velocity.z = (float) data_imu * 0.025 / 65536.0;
                data_imu = tmpdata[16] | (tmpdata[17] << 8) | tmpdata[18] << 16 | (tmpdata[19] << 24);
                imu_data.linear_acceleration.x = (float) data_imu * 2.45 / 1000.0 / 65536.0;
                data_imu = tmpdata[20] | (tmpdata[21] << 8) | tmpdata[22] << 16 | (tmpdata[23] << 24);
                imu_data.linear_acceleration.y = (float) data_imu * 2.45 / 1000.0 / 65536.0;
                data_imu = tmpdata[24] | (tmpdata[25] << 8) | tmpdata[26] << 16 | (tmpdata[27] << 24);
                imu_data.linear_acceleration.z = (float) data_imu * 2.45 / 1000.0 / 65536.0;
                data_imu = tmpdata[28] | (tmpdata[29] << 8) | tmpdata[30] << 16 | (tmpdata[31] << 24);
                imu_data.orientation.x = (float) data_imu * 0.1;//temp
                IMU_read_pub.publish(imu_data);//发布topic}
            }
        }
        //处理ROS的信息，比如订阅消息,并调用回调函数
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}