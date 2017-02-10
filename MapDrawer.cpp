#include "MapDrawer.h"
#include <pangolin/pangolin.h>
#include <mutex>
#include <iostream>

#include <cv.h>
#include <highgui.h>

using namespace std;

MapDrawer::MapDrawer(const std::string &strSettingPath,std::vector<cv::Mat>* poses):
mpPoses(poses)
{
    cv::FileStorage fSettings(strSettingPath, cv::FileStorage::READ);

    mKeyFrameSize = fSettings["Viewer.KeyFrameSize"];
    mKeyFrameLineWidth = fSettings["Viewer.KeyFrameLineWidth"];
    mGraphLineWidth = fSettings["Viewer.GraphLineWidth"];
    mPointSize = fSettings["Viewer.PointSize"];
    mCameraSize = fSettings["Viewer.CameraSize"];
    mCameraLineWidth = fSettings["Viewer.CameraLineWidth"];
}

void MapDrawer::DrawKeyFrames(const bool bDrawKF)
{
    unique_lock<mutex> lock(mMutexPose);
    const float &w = mKeyFrameSize;
    const float h = w*0.75;
    const float z = w*0.6;
    if(bDrawKF)
    {
        for(size_t i=0; i< mpPoses->size(); i++)
        {
            // KeyFrame* pKF = vpKFs[i];
            cv::Mat Twc = (*mpPoses)[i].t();

            glPushMatrix();
            glMultMatrixf(Twc.ptr<GLfloat>(0));
            glLineWidth(mKeyFrameLineWidth);
            glColor3f(0.0f,0.0f,1.0f);
            glBegin(GL_LINES);
            glVertex3f(0,0,0);
            glVertex3f(w,h,z);
            glVertex3f(0,0,0);
            glVertex3f(w,-h,z);
            glVertex3f(0,0,0);
            glVertex3f(-w,-h,z);
            glVertex3f(0,0,0);
            glVertex3f(-w,h,z);

            glVertex3f(w,h,z);
            glVertex3f(w,-h,z);

            glVertex3f(-w,h,z);
            glVertex3f(-w,-h,z);

            glVertex3f(-w,h,z);
            glVertex3f(w,h,z);

            glVertex3f(-w,-h,z);
            glVertex3f(w,-h,z);
            glEnd();

            glPopMatrix();

            if(i>0)
            {
              glColor3f(1.0f,0.0f,0.0f);
              glBegin(GL_LINES);
              glVertex3f((*mpPoses)[i].at<float>(0,3),(*mpPoses)[i].at<float>(1,3),(*mpPoses)[i].at<float>(2,3));
              glVertex3f((*mpPoses)[i-1].at<float>(0,3),(*mpPoses)[i-1].at<float>(1,3),(*mpPoses)[i-1].at<float>(2,3));
              glEnd();
            }
        }
    }
}

void MapDrawer::DrawCurrentCamera(pangolin::OpenGlMatrix &Twc)
{
    unique_lock<mutex> lock(mMutexPose);

    const float &w = mCameraSize;
    const float h = w*0.75;
    const float z = w*0.6;

    glPushMatrix();

#ifdef HAVE_GLES
        glMultMatrixf(Twc.m);
#else
        glMultMatrixd(Twc.m);
#endif

    glLineWidth(mCameraLineWidth);
    glColor3f(0.0f,1.0f,0.0f);
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(w,h,z);
    glVertex3f(0,0,0);
    glVertex3f(w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,-h,z);
    glVertex3f(0,0,0);
    glVertex3f(-w,h,z);

    glVertex3f(w,h,z);
    glVertex3f(w,-h,z);

    glVertex3f(-w,h,z);
    glVertex3f(-w,-h,z);

    glVertex3f(-w,h,z);
    glVertex3f(w,h,z);

    glVertex3f(-w,-h,z);
    glVertex3f(w,-h,z);
    glEnd();

    glPopMatrix();
}


void MapDrawer::SetCurrentCameraPose(const cv::Mat &Tcw)
{
    mCameraPose = Tcw.clone();
}

void MapDrawer::GetCurrentOpenGLCameraMatrix(pangolin::OpenGlMatrix &M)
{
    unique_lock<mutex> lock(mMutexPose);
    if(!(mpPoses->back().empty()))
    {
        cv::Mat Rwc(3,3,CV_32F);
        cv::Mat twc(3,1,CV_32F);
        {
          mCameraPose = mpPoses->back();
          Rwc = mCameraPose.rowRange(0,3).colRange(0,3);
          twc = mCameraPose.rowRange(0,3).col(3);
        }


        M.m[0] = Rwc.at<float>(0,0);
        M.m[1] = Rwc.at<float>(1,0);
        M.m[2] = Rwc.at<float>(2,0);
        M.m[3]  = 0.0;

        M.m[4] = Rwc.at<float>(0,1);
        M.m[5] = Rwc.at<float>(1,1);
        M.m[6] = Rwc.at<float>(2,1);
        M.m[7]  = 0.0;

        M.m[8] = Rwc.at<float>(0,2);
        M.m[9] = Rwc.at<float>(1,2);
        M.m[10] = Rwc.at<float>(2,2);
        M.m[11]  = 0.0;

        M.m[12] = twc.at<float>(0);
        M.m[13] = twc.at<float>(1);
        M.m[14] = twc.at<float>(2);
        M.m[15]  = 1.0;
    }
    else
        M.SetIdentity();
}
