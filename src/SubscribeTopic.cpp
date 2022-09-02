#include "ros/ros.h"
//#include "CameraVideoTopic/NavSatFix.h"
#include "sensor_msgs/NavSatFix.h"

using namespace ros;

//void SubscribeTopicCallBack(const CameraVideoTopic::NavSatFix::ConstPtr &msg){
//    ROS_INFO("SubscribeTopic:[latitude:%lf  longitude:%lf  altitude:%lf covariance_type:%d]",
//             msg->latitude, msg->longitude, msg->altitude, msg->position_covariance_type);
//    ROS_INFO("SubscribeTopic:[position_covariance:%lf %lf %lf %lf %lf %lf %lf %lf %lf]",
//             msg->position_covariance[0], msg->position_covariance[1], msg->position_covariance[2],
//             msg->position_covariance[3], msg->position_covariance[4], msg->position_covariance[5],
//             msg->position_covariance[6], msg->position_covariance[7], msg->position_covariance[8]);
//}

void SubscribeTopicCallBack(const sensor_msgs::NavSatFix::ConstPtr &msg){
    ROS_INFO("SubscribeTopic:[latitude:%lf  longitude:%lf  altitude:%lf covariance_type:%d]",
             msg->latitude, msg->longitude, msg->altitude, msg->position_covariance_type);
    ROS_INFO("SubscribeTopic:[position_covariance:%lf %lf %lf %lf %lf %lf %lf %lf %lf]",
             msg->position_covariance[0], msg->position_covariance[1], msg->position_covariance[2],
             msg->position_covariance[3], msg->position_covariance[4], msg->position_covariance[5],
             msg->position_covariance[6], msg->position_covariance[7], msg->position_covariance[8]);
}

int main(int argc, char **argv){
    init(argc , argv , "SubscribeTopic");
    NodeHandle node;
    Subscriber sbu = node.subscribe("/mavros/global_position/raw/fix",100, SubscribeTopicCallBack);

    ros::spin();
    return 0;
}