#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <cv_bridge/cv_bridge.h>
#include <iostream>

using namespace std;

/*
roslaunch mavros px4.launch
sudo gedit /opt/ros/noetic/share/usb_cam/launch/usb_cam-test.launch
roslaunch usb_cam usb_cam-test.launch
rostopic hz /usb_cam/image_raw

 <launch>
  <node name="usb_cam" pkg="usb_cam" type="usb_cam_node" output="screen" >
    <param name="video_device" value="/dev/video0" />
    <param name="image_width" value="1280" />
    <param name="image_height" value="480" />
    <param name="pixel_format" value="mjpeg" />
    <param name="camera_frame_id" value="usb_cam" />
    <param name="io_method" value="mmap"/>
  </node>
  <node name="image_view" pkg="image_view" type="image_view" respawn="false" output="screen">
    <remap from="image" to="/usb_cam/image_raw"/>
    <param name="autosize" value="true" />
  </node>
</launch>
 ***-
 */

int main(int argc, char **argv) {
    ros::init(argc, argv, "CameraPublish");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher PubLeftColor = it.advertise("/leftcolor", 1);
//    image_transport::Publisher PubLeftGray = it.advertise("/leftgray", 1);
    image_transport::Publisher PubRightColor = it.advertise("/rightcolor", 1);
//    image_transport::Publisher PubRightGray = it.advertise("/rightgray", 1);

    cv::namedWindow("ImgOri");
    cv::startWindowThread();
    cv::VideoCapture cap(argv[1][0] - '0', cv::CAP_V4L2);
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cv::Mat ImgOri, ImgLeftColor, ImgRightColor, ImgLeftGray, ImgRightGray;

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 2560);
    cout << "CV_CAP_PROP_FRAME_WIDTH:" << cap.get(cv::CAP_PROP_FRAME_WIDTH) << endl;
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
    cout << "CAP_PROP_FRAME_HEIGHT:" << cap.get(cv::CAP_PROP_FRAME_HEIGHT) << endl;
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cout << "CAP_PROP_FOURCC:" << cap.get(cv::CAP_PROP_FOURCC) << endl;
    cap.set(cv::CAP_PROP_FPS, 30);
    cout << "CAP_PROP_FPS:" << cap.get(cv::CAP_PROP_FPS) << endl;

    ros::Rate loop_rate(1000);
    sensor_msgs::ImagePtr msg;
    while (nh.ok()) {
        cap.read(ImgOri);
        if (ImgOri.empty()) {
            continue;
        }
        std_msgs::Header hd;
        hd.stamp = ros::Time::now();
        hd.frame_id = "AR0144Stereo";
        ImgLeftColor = ImgOri(cv::Rect(ImgOri.size().width / 2, 0,
                                       ImgOri.size().width / 2, ImgOri.size().height));
        msg = cv_bridge::CvImage(hd, "bgr8", ImgLeftColor).toImageMsg();
        PubLeftColor.publish(msg);
//        cvtColor(ImgLeftColor, ImgLeftGray, cv::COLOR_RGB2GRAY);
//        msg = cv_bridge::CvImage(hd, "mono8", ImgLeftGray).toImageMsg();
//        PubLeftGray.publish(msg);


        ImgRightColor = ImgOri(cv::Rect(0, 0,
                                        ImgOri.size().width / 2, ImgOri.size().height));
        msg = cv_bridge::CvImage(hd, "bgr8", ImgRightColor).toImageMsg();
        PubRightColor.publish(msg);
//        cvtColor(ImgRightColor, ImgRightGray, cv::COLOR_RGB2GRAY);
//        msg = cv_bridge::CvImage(hd, "mono8", ImgRightGray).toImageMsg();
//        PubRightGray.publish(msg);
        cv::imshow("ImgOri", ImgOri);
        cv::waitKey(1);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
