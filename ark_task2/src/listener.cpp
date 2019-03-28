#include "ros/ros.h"
#include "ark_task2/board_pose.h"
#include <sstream>
#include <iostream>

void chatterCallback(const tasks::board_pose::ConstPtr& msg)
{
  ROS_INFO("I heard: [%d,%d]", centre_pose->x, centre_pose->y);
}

bool InDanger(ark_task2::danger_region::Request  &req,
ark_task2::danger_region::Response &res)
	{
		stringstream ss;
		if(req.safety)
			ss << "Danger!!";
		else
			ss << "Peace....";
		res.out = ss.str();
		ROS_INFO("request: input=%d", (int)req.safety);
		ROS_INFO("sent response: %s", res.out);
		return true;
	}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "listener");

  ros::NodeHandle n;
  
  ros::ServiceServer service = n.advertiseService("danger_region", service_call);

  ros::Subscriber sub = n.subscribe("check_pose", 1000, chatterCallback);

  ros::spin();
  return 0;
}