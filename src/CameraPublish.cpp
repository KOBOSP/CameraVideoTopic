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
 *
 */


int main(int argc, char **argv) {
    ros::init(argc, argv, "CameraPublish");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher PubLeft = it.advertise("/camera/left/image_raw", 1);
    image_transport::Publisher PubRight = it.advertise("/camera/right/image_raw", 1);

    int deviceID = argv[1][0]-'0';
    cv::VideoCapture cap(deviceID,cv::CAP_V4L2);
    /*
#if 0
	inputVideo.open(0, cv::CAP_V4L2);
#else
	inputVideo.open(0, cv::CAP_GSTREAMER);
     */
    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
        return -1;
    }
    cv::Mat ImgOri, ImgLeft, ImgRight;


    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cout<<"CV_CAP_PROP_FRAME_WIDTH:"<<cap.get(cv::CAP_PROP_FRAME_WIDTH)<<endl;
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cout<<"CAP_PROP_FRAME_HEIGHT:"<<cap.get(cv::CAP_PROP_FRAME_HEIGHT)<<endl;
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));
    cout<<"CAP_PROP_FOURCC:"<<cap.get(cv::CAP_PROP_FOURCC)<<endl;
    cap.set(cv::CAP_PROP_FPS, 30);
    cout<<"CAP_PROP_FPS:"<<cap.get(cv::CAP_PROP_FPS)<<endl;

    ros::Rate loop_rate(30);
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
