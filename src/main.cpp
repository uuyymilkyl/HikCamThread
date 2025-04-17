#include "MonitorCam.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>


static void cvMMonitorCallFunc(void* _pMain,cv::Mat _inMat)
{
    cv::imshow("Display Window", _inMat);
    cv::waitKey(1);
}

int main(){

    MMonitorCam MMCam;
;
    MMCam.OpenCVCamGrabThread(cvMMonitorCallFunc);

    while(true){
        sleep(10);
    }

}