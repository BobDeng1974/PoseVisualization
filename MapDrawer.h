#ifndef MAPDRAWER_H
#define MAPDRAWER_H

#include<pangolin/pangolin.h>

#include<mutex>
#include<vector>
#include<string>

#include <cv.h>
#include <highgui.h>

class MapDrawer
{
public:
    MapDrawer( const std::string &strSettingPath, std::vector<cv::Mat>* poses);

    void DrawKeyFrames(const bool bDrawKF);
    void DrawCurrentCamera(pangolin::OpenGlMatrix &Twc);
    void SetCurrentCameraPose(const cv::Mat &Tcw);
    void GetCurrentOpenGLCameraMatrix(pangolin::OpenGlMatrix &M);
    std::mutex mMutexPose;

private:

    float mKeyFrameSize;
    float mKeyFrameLineWidth;
    float mGraphLineWidth;
    float mPointSize;
    float mCameraSize;
    float mCameraLineWidth;
    cv::Mat mCameraPose;
    std::vector<cv::Mat>* mpPoses;   //added pose vector
};

#endif // MAPDRAWER_H
