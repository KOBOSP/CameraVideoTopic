#include "ros/ros.h"
//#include "CameraVideoTopic/NavSatFix.h"
#include "sensor_msgs/NavSatFix.h"
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <stdio.h>

using namespace ros;
using namespace std;

int KeyNum;
bool bSaveLoc;
FILE *pFile;

void SubscribeTopicCallBack(const sensor_msgs::NavSatFix::ConstPtr &msg) {
    ROS_INFO("SubscribeTopic:[latitude:%lf  longitude:%lf  altitude:%lf covariance_type:%d]",
             msg->latitude, msg->longitude, msg->altitude, msg->position_covariance_type);
    ROS_INFO("SubscribeTopic:[position_covariance:%lf %lf %lf %lf %lf %lf %lf %lf %lf]",
             msg->position_covariance[0], msg->position_covariance[1], msg->position_covariance[2],
             msg->position_covariance[3], msg->position_covariance[4], msg->position_covariance[5],
             msg->position_covariance[6], msg->position_covariance[7], msg->position_covariance[8]);
    cout << "bSaveLoc" << bSaveLoc << endl;
    if (bSaveLoc) {
        fprintf(pFile, "% 13.10lf, % 13.10lf, % 13.10lf, %d,\n",
                msg->latitude, msg->longitude, msg->altitude, msg->position_covariance_type);
        fprintf(pFile, "%lf, %lf, %lf, \n%lf, %lf, %lf, \n%lf, %lf, %lf,\n",
                msg->position_covariance[0], msg->position_covariance[1], msg->position_covariance[2],
                msg->position_covariance[3], msg->position_covariance[4], msg->position_covariance[5],
                msg->position_covariance[6], msg->position_covariance[7], msg->position_covariance[8]);
        bSaveLoc = false;
    }
}

int main(int argc, char **argv) {

    pFile = fopen("/home/kobosp/SLMAC/CameraVideoTopic/example.txt", "w");
    if(!pFile){
        return -1;
    }
    cv::namedWindow("SubscribeTopic");

    init(argc, argv, "SubscribeTopic");
    NodeHandle node;
    Subscriber sbu = node.subscribe("/mavros/global_position/raw/fix", 100, SubscribeTopicCallBack);
    ros::Rate loop_rate(60);
    while (ros::ok()) {
        KeyNum = cv::waitKey(2);//https://www.asciitable.com/
        if (KeyNum == 'r' || KeyNum == 'R') {
            bSaveLoc = true;
        } else if (KeyNum == 'q' || KeyNum == 'Q') {
            break;
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    fclose(pFile);
    cv::destroyWindow("SubscribeTopic");

    return 0;
}