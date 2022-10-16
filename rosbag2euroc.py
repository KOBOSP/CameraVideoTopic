#!/usr/bin/env python3

import sys
import argparse
import os
import subprocess, yaml
import errno

import cv2

import roslib
import rosbag
import rospy
from std_msgs.msg import String
from cv_bridge import CvBridge, CvBridgeError

CAM_FOLDER_NAME = 'cam'
IMU_FOLDER_NAME = 'imu'
DATA_CSV = 'data.csv'
SENSOR_YAML = 'sensor.yaml'
BODY_YAML = 'body.yaml'

# Can get this from ros topic
CAM_SENSOR_YAML = dict(
    sensor_type= "camera",
    comment= "VI-Sensor cam0 (MT9M034)",
    T_BS= dict(
        cols= 4,
        rows= 4,
        data= [-0.9999695, -0.00269689, 0.00732996, 0.06271645,
               0.00270479, -0.99999577, 0.00106847, 0.00076847,
               0.00732704, 0.00108826, 0.99997256, 0.0047303,
               0.0, 0.0, 0.0, 1.0]
    ),
    rate_hz= 50,
    resolution= [1280, 800],
    camera_model= "pinhole",
    intrinsics= [493.683689207663, 493.3864619293556, 583.9934736726879, 460.2509615566427],
    distortion_model= "radial-tangential",
    distortion_coefficients= [0.20643903564540966, -0.16861648341047722, -0.004528025028118068, 0.004199063024472363]
)

# Can get this from ros topic
IMU_SENSOR_YAML = dict(
    sensor_type= "imu",
    comment= "VI-Sensor IMU (ADIS16448)",
    T_BS= dict(
        cols= 4,
        rows= 4,
        data= [1.0, 0.0, 0.0, 0.0,
               0.0, 1.0, 0.0, 0.0,
               0.0, 0.0, 1.0, 0.0,
               0.0, 0.0, 0.0, 1.0]
    ),
    rate_hz= 1000,
    gyroscope_noise_density= 0.004360717329633302,
    gyroscope_random_walk= 5.298612456148996e-05,
    accelerometer_noise_density= 0.028786453120295194,
    accelerometer_random_walk= 0.00033865467510342566,
)

def get_rosbag_metadata(rosbag_path):
    assert(os.path.exists(rosbag_path))
    # This subprocess will only work if ROS is sourced...
    return yaml.load(subprocess.Popen(['rosbag', 'info', '--yaml', rosbag_path],
                                      stdout=subprocess.PIPE).communicate()[0])

def mkdirs_without_exception(path):
    try:
       os.makedirs(path)
    except OSError as e:
        if e.errno == errno.EEXIST:
            print("The directory {} already exists.".format(path))
        else:
            print(e)
            raise  # raises the error again

def setup_dataset_dirs(rosbag_path, output_path, camera_topics, imu_topics):
    # Create base folder
    dirname = os.path.split(rosbag_path)[-1].split(".", 1)[0] + '/mav0'
    base_path = os.path.join(output_path, dirname)
    mkdirs_without_exception(base_path)

    # Create folder for camera topics
    cam_folder_paths = []
    for i in range(len(camera_topics)):
        cam_folder_paths.append(os.path.join(base_path, CAM_FOLDER_NAME + repr(i)))
        current_cam_folder_path = cam_folder_paths[-1]
        mkdirs_without_exception(current_cam_folder_path)

        # Create data folder
        mkdirs_without_exception(os.path.join(current_cam_folder_path, 'data'))

        # Create data.csv file
        with open(os.path.join(current_cam_folder_path, DATA_CSV), 'w+') as outfile:
            outfile.write('#timestamp [ns],filename')

        # Create sensor.yaml file
        with open(os.path.join(current_cam_folder_path, SENSOR_YAML), 'w+') as outfile:
            outfile.write("%YAML:1.0\n")
            CAM_SENSOR_YAML['comment'] = CAM_FOLDER_NAME + repr(i)
            yaml.dump(CAM_SENSOR_YAML, outfile, default_flow_style=True)


    # Create folder for imu topics
    imu_folder_paths = []
    for i in range(len(imu_topics)):
        imu_folder_paths.append(os.path.join(base_path, IMU_FOLDER_NAME + repr(i)))
        current_imu_folder_path = imu_folder_paths[-1]
        mkdirs_without_exception(current_imu_folder_path)

        # Create data.csv file
        with open(os.path.join(current_imu_folder_path, DATA_CSV), 'w+') as outfile:
            outfile.write("#timestamp [ns],w_RS_S_x [rad s^-1],w_RS_S_y [rad s^-1],w_RS_S_z [rad s^-1],a_RS_S_x [m s^-2],a_RS_S_y [m s^-2],a_RS_S_z [m s^-2]")

        # Create sensor.yaml file
        with open(os.path.join(current_imu_folder_path, SENSOR_YAML), 'w+') as outfile:
            outfile.write("%YAML:1.0\n")
            IMU_SENSOR_YAML['comment'] = IMU_FOLDER_NAME + repr(i)
            yaml.dump(IMU_SENSOR_YAML, outfile, default_flow_style=True)

    # Create body.yaml file
    with open(os.path.join(base_path, BODY_YAML), 'w+') as outfile:
        outfile.write("%YAML:1.0\n")
        body_yaml = dict(comment = 'Automatically generated dataset using Rosbag2Euroc, using rosbag: {}'.format(rosbag_path))
        yaml.dump(body_yaml, outfile, default_flow_style=True)

    return cam_folder_paths, imu_folder_paths

def rosbag_2_euroc(rosbag_path, output_path):
    # Check that the path to the rosbag exists.
    assert(os.path.exists(rosbag_path))
    bag = rosbag.Bag(rosbag_path)

    # Check that rosbag has the data we need to convert to Euroc dataset format.
    bag_metadata = get_rosbag_metadata(rosbag_path)
    camera_topics = []
    imu_topics = []
    for element in bag_metadata['topics']:
        if (element['type'] == 'sensor_msgs/Image'):
            camera_topics.append(element['topic'])
        elif (element['type'] == 'sensor_msgs/Imu'):
            imu_topics.append(element['topic'])

    # Check that it has one or two Image topics.
    if not camera_topics:
        print ("WARNING: there are no camera topics in this rosbag!")

    # Check that it has one, and only one, IMU topic.
    if imu_topics != 1:
        print ("WARNING: expected to have a single IMU topic, instead got: {} topic(s)".format(
            len(imu_topics)))

    # Build output folder.
    cam_folder_paths, imu_folder_paths = setup_dataset_dirs(rosbag_path, output_path, camera_topics, imu_topics)

    # Use a CvBridge to convert ROS images to OpenCV images so they can be saved.
    cv_bridge = CvBridge()

    # Convert image msg to Euroc dataset format.
    assert(len(camera_topics) == len(cam_folder_paths))
    for i, cam_topic in enumerate(camera_topics):
        print(f"Converting camera messages for topic: {cam_topic}")
        print(f"Storing results in: {cam_folder_paths[i]}")
        # Write data.csv file.
        with open(os.path.join(cam_folder_paths[i], DATA_CSV), 'a') as outfile:
            for _, msg, t in bag.read_messages(topics=[cam_topic]):
                image_filename = str(msg.header.stamp) + '.png'
                outfile.write('\n' + str(msg.header.stamp) + "," + image_filename)
                try:
                    cv_image = cv_bridge.imgmsg_to_cv2(msg, msg.encoding)
                    cv2.imwrite(os.path.join(cam_folder_paths[i], 'data/',
                                             image_filename),
                                cv_image)
                except CvBridgeError as e:
                    print(e)

    # Convert IMU msg to Euroc dataset format.
    assert(len(imu_topics) == len(imu_folder_paths))
    for i, imu_topic in enumerate(imu_topics):
        print(f"Converting IMU messages for topic: {imu_topic}")
        print(f"Storing results in: {imu_folder_paths[i]}")
        with open(os.path.join(imu_folder_paths[i], DATA_CSV), 'a') as outfile:
            for _, msg, t in bag.read_messages(topics=[imu_topic]):
                outfile.write(str(msg.header.stamp) + ',' +
                              str(msg.angular_velocity.x) + ',' +
                              str(msg.angular_velocity.y) + ',' +
                              str(msg.angular_velocity.z) + ',' +
                              str(msg.linear_acceleration.x) + ',' +
                              str(msg.linear_acceleration.y) + ',' +
                              str(msg.linear_acceleration.z))

    # TODO(TONI): Consider adding Lidar msgs (sensor_msgs/PointCloud2)
    # TODO(TONI): parse tf_static or cam_info and create the sensor.yaml files?

    # Close the rosbag.
    bag.close()

if __name__ == "__main__":
    # Parse rosbag path.
    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('rosbag_path', help='Path to the rosbag.')
    parser.add_argument('-o', '--output_path', help='Path to the output.', default='./')
    args = parser.parse_args()

    # Convert rosbag.
    print(f"Converting rosbag: \"{os.path.split(args.rosbag_path)[-1]}\" to EuRoC format.")
    print(f"Storing results in directory: {args.output_path}.")
    rosbag_2_euroc(args.rosbag_path, args.output_path)
    print("Done.")


