#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <ark_task2/board_pose.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

bool InDanger(int x, int y)
{
	if(80<x && x<120 && 80<y && y<120)
		return true;
	else
		return false;
}

int main(int argc,char **argv)
{
	ros::init(argc, argv, "talker");

	ros::NodeHandle n;				//creating node object

	ros::Publisher chatter_pub = n.advertise<ark_task2::board_pose>("check_pose",1000);		//topic name is chatter. we publish msg of type board_pose

	ros::Rate loop_rate(10);

	ark_task2::board_pose centre_pose;
	
	VideoCapture cap("new.avi");
	if(!cap.isOpened())
	{	
    		cout << "Error opening video stream or file" << endl;
    		return -1;
	}
	while(ros::ok())
	{
    	Mat frame,gray,board_img;
		cap>>frame;
				
		cvtColor(frame, gray, CV_BGR2GRAY );					
		
		Size patternsize(5,8); //interior number of corners

		vector<Point2f> corners; //this will be filled by the detected
		
		bool patternfound = findChessboardCorners( gray, patternsize, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+ CALIB_CB_FAST_CHECK); 
		
		if(patternfound)
			cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
		else
			break;
					
		centre_pose.x = (corners[22].x + corners[17].x)/2;
		centre_pose.y = (corners[22].y + corners[17].y)/2;
		ROS_INFO("%d %d", centre_pose.x, centre_pose.y);

		chatter_pub.publish(centre_pose);
		ros::ServiceClient client = n.serviceClient<ark_task2::danger_region.srv>("danger_region");

		ark_task2::danger_region srv;

		srv.request.safety = InDanger(int((corners[22].x + corners[17].x)/2),int((corners[22].y + corners[17].y)/2));
		

		if (client.call(srv))
  		{
    		ROS_INFO("Output: %s", srv.response.out);
 		}
  		else
  		{
    		ROS_ERROR("Failed to call service danger_region");
    		return 0;
		}

		ros::spinOnce();

		loop_rate.sleep();
	}
	
}

