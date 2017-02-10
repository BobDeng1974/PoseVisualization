#include "Viewer.h"
#include <iostream>
#include <thread>
#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

int main()
{
  std::vector<Mat> vPose;
  MapDrawer mapDrawer("../conf.yaml",&vPose);
  Viewer viewer(&mapDrawer,"../conf.yaml");

  FILE *fp = fopen("../CameraTrajectory.txt","r");
  float currPose[16];

  thread t(&Viewer::Run,&viewer);

  while( fscanf(fp,"%f",&currPose[0]) != EOF )
  {
      for (size_t i = 1; i < 12; i++)
      {
        fscanf(fp,"%f",&currPose[i]);
      }
      float temp;
      for (size_t i = 0; i < 4; i++) {
        /* code */
        fscanf(fp,"%f",&temp);
      }
      currPose[12] = 0; currPose[13] = 0; currPose[14] = 0; currPose[15] = 1;
      cv::Mat m(4,4,CV_32FC1,currPose);
      {
        unique_lock<mutex> lock(mapDrawer.mMutexPose);
        vPose.push_back(m.clone());
      }

      usleep(10000);
  }

  fclose(fp);


  t.join();
  return 0;
}
