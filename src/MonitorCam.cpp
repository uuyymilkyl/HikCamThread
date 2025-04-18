
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "MvCameraControl.h"
#include "MonitorCam.h"


static unsigned int g_nPayloadSize;

MMonitorCam::MMonitorCam()
{
    m_bIsCamOpen = false;
    m_save_img = cv::Mat::zeros(400, 400, CV_8UC3);
}

MMonitorCam::~MMonitorCam()
{
}

void MMonitorCam::OpenCVCamGrabThread( MMonitorCamCall _pFunC)
{
    m_CallBack = _pFunC;
	m_pMain = this;

    m_hCamGrabThread = std::thread(OpenMVCamGrabbing,m_pMain,m_CallBack);
	m_hCamGrabThread.detach(); 


}

void MMonitorCam::OpenMVCamGrabbing(void* _pMain,  MMonitorCamCall _pFunC)
{
    MMonitorCam* j = (MMonitorCam*)_pMain;

    while (1)
    {

        // 相机打开后，进入图像采集
        if(!j->MVCamGrab())
        {
           j->m_CallBack(j->m_pMain,j->m_save_img); // 采集失败回传一张黑色图像
            continue;
        }
        j->m_CallBack(j->m_pMain,j->m_save_img);

    }

}

int MMonitorCam::MVCamInit()
{
    m_nRet = MV_OK;
    m_handle = NULL;
	m_pData = NULL;        
    
    


    // 初始化SDK 
	m_nRet = MV_CC_Initialize();
	if (MV_OK != m_nRet)
	{
		printf("Initialize SDK fail! nRet [0x%x]\n", m_nRet);
		return -1;
	}

	MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 枚举设备
    m_nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_GENTL_CAMERALINK_DEVICE | MV_GENTL_CXP_DEVICE | MV_GENTL_XOF_DEVICE, &stDeviceList);
    if (MV_OK != m_nRet)
    {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", m_nRet);
        return -1;
    }
    if (stDeviceList.nDeviceNum > 0)
    {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++)
        {
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (NULL == pDeviceInfo)
            {
                break;
            } 
           // PrintDeviceInfo(pDeviceInfo);            
        }  
    } 
    else
    {
        printf("Find No Devices!\n");
        return -1;
    }
    unsigned int nIndex = 0;  // 设置相机设备的索引
    if (nIndex >= stDeviceList.nDeviceNum)
    {
        printf("Intput error!\n");
        return -1;
    }
    // 选择设备并创建句柄
    // select device and create handle
    m_nRet = MV_CC_CreateHandle(&m_handle, stDeviceList.pDeviceInfo[nIndex]);
    if (MV_OK != m_nRet)
    {
        printf("MV_CC_CreateHandle fail! nRet [%x]\n", m_nRet);
        return -1;
    }
    // 打开设备
    // open device
    m_nRet = MV_CC_OpenDevice(m_handle);
    if (MV_OK != m_nRet)
    {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", m_nRet);
        return -1;
    }
	usleep(1000);

	m_nRet = MV_CC_SetEnumValue(m_handle, "TriggerMode", 0);
    if (MV_OK != m_nRet)
    {
        printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", m_nRet);
        return -1;
    }

    // ch:获取数据包大小 | en:Get payload size
    memset(&m_stParam, 0, sizeof(MVCC_INTVALUE));
    m_nRet = MV_CC_GetIntValue(m_handle, "PayloadSize", &m_stParam);
    if (MV_OK != m_nRet)
    {
        printf("Get PayloadSize fail! nRet [0x%x]\n", m_nRet);
        return -1;
    }	

	m_nRet = MV_CC_StartGrabbing(m_handle);
    if (m_nRet != MV_OK)
	{
        printf("开始取流失败");
		
		return -1;
	}

    if(MV_OK == m_nRet)
	{
        m_bIsCamOpen = true;
        return 0;
    }


    return -1;
}

bool MMonitorCam::MVCamGrab()
{
    //memset(&m_pFrameInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
	// //检查缓存的负载大小，使用全局变量前要做检查，避免因小失大
	// if (m_pData == NULL)
	// {
	// 	//Allocate memory failed.
	// 	return false;
	// }


	if(m_bIsCamOpen != true || m_handle == NULL)
	{
		MVCamInit();
		usleep(10);
		return false;
	}

	MV_FRAME_OUT_INFO_EX m_pImageInfo = {0};
    memset(&m_pFrameInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    m_pData = (unsigned char *)malloc(sizeof(unsigned char) * m_stParam.nCurValue);
    if (NULL == m_pData)
    {
        printf("pData is null\n");
        return false;
    }
    unsigned int nDataSize = m_stParam.nCurValue;

	MV_CC_PIXEL_CONVERT_PARAM stConvertParam ;

	//超时单帧获取一帧图像，超时机制可以获得较为平稳的图像
	m_nRet = MV_CC_GetOneFrameTimeout(m_handle, m_pData, nDataSize, &m_pFrameInfo, 1000);
	if (m_nRet == MV_OK)
	{
		m_pDataForRGB = (unsigned char*)malloc(m_pFrameInfo.nWidth * m_pFrameInfo.nHeight * 4+2048 );
		if (NULL == m_pDataForRGB)
		{
			return false;
		}
		unsigned int nDataSizeForRGB = m_pFrameInfo.nWidth * m_pFrameInfo.nHeight * 4+2048 ;

		//像素格式转换
		stConvertParam = { 0 };
		memset(&stConvertParam, 0, sizeof(MV_CC_PIXEL_CONVERT_PARAM));
		stConvertParam.nWidth = m_pFrameInfo.nWidth;                 //ch:图像宽 | en:image width
		stConvertParam.nHeight = m_pFrameInfo.nHeight;               //ch:图像高 | en:image height
		stConvertParam.pSrcData = m_pData;                            //ch:输入数据缓存 | en:input data buffer
		stConvertParam.nSrcDataLen = m_pFrameInfo.nFrameLen;         //ch:输入数据大小 | en:input data size
		stConvertParam.enSrcPixelType = m_pFrameInfo.enPixelType;    //ch:输入像素格式 | en:input pixel format
		stConvertParam.enDstPixelType = PixelType_Gvsp_RGB8_Packed; //ch:输出像素格式 | en:output pixel format
		stConvertParam.pDstBuffer = m_pDataForRGB;                    //ch:输出数据缓存 | en:output data buffer
		stConvertParam.nDstBufferSize = nDataSizeForRGB;            //ch:输出缓存大小 | en:output buffer size
		m_nRet = MV_CC_ConvertPixelType(m_handle, &stConvertParam);
		if (m_nRet != MV_OK)
		{
			printf("Pixel Type fail! nRet [0x%x]", m_nRet);
			free(m_pDataForRGB);
			return false;
		}

	}
	else
	{
		//No data
		printf("取证相机异常[0x%x]", m_nRet);
		//释放缓存
		//free(pData);
		//free(pDataForRGB);
		//初始化缓存
		//pData = NULL;
		//pDataForRGB = NULL;
		m_mtLock.unlock();
		return false;
	}


	bool bConvertRet = false;
	//将缓存内的数据进行转换获得Mat格式的图像
	bConvertRet = Convert2Mat(&stConvertParam, stConvertParam.pDstBuffer);
    cv::Mat Img = m_save_img;
    cv::imwrite("img.png",m_save_img);

	free(m_pDataForRGB);
	m_pDataForRGB = NULL;

	m_mtLock.unlock();
	return bConvertRet;
}

bool MMonitorCam::Convert2Mat(MV_CC_PIXEL_CONVERT_PARAM *pstImageInfo, unsigned char *p_Data)
{
    //定义一个Mat变量
	cv::Mat srcImage;
	//图片有Mono8格式：灰度(单通道)图片
	//还有RGB8格式： 彩色(三通道)图片
	if (pstImageInfo->enDstPixelType == PixelType_Gvsp_Mono8)
	{
		//根据缓存中的当前值来赋值给Mat，格式为Mono8，因为单通道图片没有RGB值，不需要进行RGB的转换
		srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC1, p_Data);
	}
	else if (pstImageInfo->enDstPixelType == PixelType_Gvsp_RGB8_Packed)
	{
		//因为三通道图片具有RGB值，而OpenCV的RGB值是反过来的，所以要做一个重新排列顺序的转换
		RGB2BGR(p_Data, pstImageInfo->nWidth, pstImageInfo->nHeight);
		//根据缓存中RGB转换后的当前值来赋值给Mat
		srcImage = cv::Mat(pstImageInfo->nHeight, pstImageInfo->nWidth, CV_8UC3, p_Data);
	}
	else
	{
		//无效的像素格式: unsupported pixel format
		printf("无效的像素格式");
		return false;
	}

	//对赋值后的Mat做检查
	if (NULL == srcImage.data)
	{
		return false;
	}

	//save converted image in a local file
	//将转换后的图像保存在全局变量save_img中
	try {
#if defined (VC9_COMPILE)	//VC9中的OpenCV的保存函数：cvSaveImage()
		cvSaveImage("MatImage.bmp", &(IplImage(srcImage)));
#else
		
		m_save_img = srcImage.clone();
#endif
	}
	catch (cv::Exception& ex) {
		//捕捉异常
		
		fprintf(stderr, "Exception saving image to bmp format: %s\n", ex.what());
		return false;
	}

	//清空Mat变量 内存控制操作
	srcImage.release();
	//返回正常值
	return true;
}

int MMonitorCam::RGB2BGR(unsigned char *pRgbData, unsigned int nWidth, unsigned int nHeight)
{
	if (NULL == pRgbData)
	{
		return MV_E_PARAMETER;
	}

	//遍历一遍RGB颜色值索引，将R(0)和B(2)的位置调换
	for (unsigned int j = 0; j < nHeight; j++)
	{
		for (unsigned int i = 0; i < nWidth; i++)
		{
			//在m行n列的图像矩阵中的RGB值 R、G、B 分别为：[m * (n * 3) + 0]、[m * (n * 3) + 1]、[m * (n * 3) + 2]，
			//原始格式中 R的值是在第一位,获得Red的值
			unsigned char red = pRgbData[j * (nWidth * 3) + i * 3];
			//将Blue的值赋值到Red
			pRgbData[j * (nWidth * 3) + i * 3] = pRgbData[j * (nWidth * 3) + i * 3 + 2];
			//将Red的值赋值到Blue
			pRgbData[j * (nWidth * 3) + i * 3 + 2] = red;
		}
	}
	//返回正常值
	return MV_OK;
}
