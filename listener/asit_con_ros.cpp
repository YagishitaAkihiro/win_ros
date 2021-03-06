
#include "stdafx.h"
#include <windows.h>
#include <time.h>
#include <cstdlib>
#include <array>
#include <cmath>
#include <sstream>
#include <map>
#include <ros/ros.h>
#include <std_msgs/String.h>
/*
** 共有メモリ
*/
const wchar_t mutexName[](L"STER_DISPLAY_MUTEX");
static HANDLE hMutex;
const wchar_t shareName[](L"STER_DISPLAY_SHARE");
static HANDLE hShare;
const unsigned int shareSize(50);
typedef std::array<float, 16> Matrix;
static Matrix *matrix;

void callback(const std_msgs::String::ConstPtr& msg)
{
	ROS_INFO("I will angles: [%s]", msg->data.c_str());
}

static void killed(void)
{
  UnmapViewOfFile(matrix);
  CloseHandle(hShare);
  CloseHandle(hMutex);
}

int main()
{
  std::map<std::string,std::string> remappings;
  remappings["__master"]  = "http://192.168.139.124:11311";
  remappings["__hostname"]= "192.168.139.101";
  ros::init(remappings,"win_listener");
  atexit(killed);

  ros::NodeHandle n;

  hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, mutexName);

  if (hMutex)
  {
	  // ファイルマッピングオブジェクトを作成する
	 hShare = OpenFileMapping(FILE_MAP_WRITE, FALSE, shareName);
     if (hShare){
        matrix = static_cast<Matrix*>(MapViewOfFile(hShare, FILE_MAP_WRITE, 0, 0, shareSize * sizeof(Matrix)));
        ros::Subscriber sub = n.subscribe("angle_data", 1, callback);
  ros::spin();
  return 0;
     }
  }
}
