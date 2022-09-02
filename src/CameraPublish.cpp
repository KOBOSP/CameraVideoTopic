#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>

int main(int argc, char **argv) {
    ros::init(argc, argv, "CameraPublish");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher PubLeft = it.advertise("/camera/left/image_raw", 1);
    image_transport::Publisher PubRight = it.advertise("/camera/right/image_raw", 1);

    cv::VideoCapture cap;
    cv::Mat ImgOri, ImgLeft, ImgRight;
    int deviceID = 0;
    int apiID = cv::CAP_ANY;
    cap.open(deviceID + apiID);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 60);
    if (!cap.isOpened()) {
        std::cerr << "ERROR! Unable to open camera" << std::endl;
        return -1;
    }

    ros::Rate loop_rate(60);
    sensor_msgs::ImagePtr msg;
    while (nh.ok()) {
        cap.read(ImgOri);
        if (ImgOri.empty()) {
            continue;
        }
        ImgLeft = ImgOri(cv::Rect(0, 0,
                                  ImgOri.size().width / 2, ImgOri.size().height));
        cv::flip(ImgLeft, ImgLeft, -1);
        msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", ImgLeft).toImageMsg();
        PubLeft.publish(msg);
        ImgRight = ImgOri(cv::Rect(ImgOri.size().width / 2, 0,
                                   ImgOri.size().width / 2, ImgOri.size().height));
        cv::flip(ImgRight, ImgRight, -1);
        msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", ImgRight).toImageMsg();
        PubRight.publish(msg);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
