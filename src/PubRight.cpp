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
    cv::namedWindow("ImgRightView");
    cv::startWindowThread();
    ros::init(argc, argv, "PubRight");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);
    image_transport::Publisher PubLeft = it.advertise("/rightcolor", 10);

    cv::VideoCapture CapLeft(argv[1][0] - '0', cv::CAP_V4L2);
    if (!CapLeft.isOpened()) {
        cerr << "ERROR! Unable to open camera " << argv[1][0] << "\n";
        return -1;
    }
    int32_t HaveFrameInSecond = 0, FpsCamTriger = 20, LastSecond, LastMS = 0, NowMS, PastMS;

    CapLeft.set(cv::CAP_PROP_FRAME_WIDTH, 1920);
    cout << "CV_CAP_PROP_FRAME_WIDTH:" << CapLeft.get(cv::CAP_PROP_FRAME_WIDTH) << endl;
    CapLeft.set(cv::CAP_PROP_FRAME_HEIGHT, 1080);
    cout << "CAP_PROP_FRAME_HEIGHT:" << CapLeft.get(cv::CAP_PROP_FRAME_HEIGHT) << endl;
    CapLeft.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    cout << "CAP_PROP_FOURCC:" << CapLeft.get(cv::CAP_PROP_FOURCC) << endl;
    CapLeft.set(cv::CAP_PROP_BACKLIGHT, 2);
    cout << "CAP_PROP_FOURCC:" << CapLeft.get(cv::CAP_PROP_BACKLIGHT) << endl;
    CapLeft.set(cv::CAP_PROP_FPS, FpsCamTriger);
    cout << "CAP_PROP_FPS:" << CapLeft.get(cv::CAP_PROP_FPS) << endl;

    ros::Rate loop_rate(1000);
    sensor_msgs::ImagePtr MsgLeft;
    cv::Mat ImgLeft, ImgLeftSmall;
    while (ros::ok()) {
        CapLeft.read(ImgLeft);
        if (!ImgLeft.empty()) {
            std_msgs::Header hd;

            hd.stamp = ros::Time::now();
            NowMS = hd.stamp.nsec / 1000000;
            PastMS = ((NowMS - LastMS) > 0 ? (NowMS - LastMS) : (NowMS - LastMS) + 1000);
            if ((PastMS > 1000 / FpsCamTriger * 2) ||
                ((PastMS > 1000 / FpsCamTriger) && HaveFrameInSecond >= FpsCamTriger - 1)) {
                cout << "LastMS:" << LastMS << " NowMS:" << NowMS << " PastMS:" << PastMS << endl;
                HaveFrameInSecond = 0;
                LastSecond = hd.stamp.sec;
            }
            LastMS = NowMS;
            hd.stamp.sec = LastSecond;
            hd.stamp.nsec = (1000000000) / FpsCamTriger * HaveFrameInSecond;


            hd.frame_id = to_string(HaveFrameInSecond++);
            MsgLeft = cv_bridge::CvImage(hd, "bgr8", ImgLeft).toImageMsg();
            PubLeft.publish(MsgLeft);
            cv::resize(ImgLeft, ImgLeftSmall, cv::Size(640, 360));
            cv::imshow("ImgRightView", ImgLeftSmall);
            cv::waitKey(1);
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}
