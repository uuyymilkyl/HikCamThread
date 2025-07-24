#ifndef MonItor_h
#define Monitor_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <thread>
#include <MvCameraControl.h>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

// 监视相机回调函数
typedef void (*MMonitorCamCall)(void* _pMain,cv::Mat _Mat);



class MMonitorCam
{
private:
    /* data */
public:
    MMonitorCam(/* args */);
    ~MMonitorCam();

    /*为采集循环开启线程*/
    void OpenCVCamGrabThread(MMonitorCamCall _pFunC);
    /*开始采集循环*/
    static void OpenMVCamGrabbing(void* _pMain, MMonitorCamCall _pFunC );

    int  MVCamInit();
    bool MVCamGrab();

    bool Convert2Mat(MV_CC_PIXEL_CONVERT_PARAM* pstImageInfo, unsigned char* p_Data);
    int RGB2BGR(unsigned char* pRgbData, unsigned int nWidth, unsigned int nHeight);

public:

    int m_nRet;
    void* m_handle;
    unsigned char * m_pData;
    unsigned char * m_pDataForRGB;

    bool m_bIsCamOpen;
    cv::Mat m_save_img;
    
    MV_FRAME_OUT_INFO_EX m_pFrameInfo;
    MV_CC_PIXEL_CONVERT_PARAM m_stConvertParam;
    MVCC_INTVALUE m_stParam;

    MMonitorCamCall m_CallBack;  ///< 回调函数
    void* m_pMain;               ///< 调用的函数指针
    std::thread m_hCamGrabThread; 
    std::mutex m_mtLock;         ///< 同步锁

};



#endif //Monitor_h