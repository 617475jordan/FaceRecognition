#ifndef _FACEFILE_H_
#define _FACEFILE_H_

#include <iostream>    
#include <cstdint>  
#include <fstream>  
#include <string>  
#include <cmath>  
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>   
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur  
#include "recognizer.h"
#include <face_alignment.h>
#include <face_identification.h>
#include <face_detection.h>
#include <conio.h>
#include <time.h.>
#include "rtcprgb2gray.h"
#include "FaceRecognition.h"
#include "FeatureGroup.h"
using namespace cv;
using namespace std;

class faceData
{
public:
	int initalImage(string ImageName);
	void rotateImage(const cv::Mat src, const cv::Mat& img_rotate, double degree);
public:
	#define Num 2048 //特征点个数
	Mat detectFace(Mat  probe_img_color, float gallery_fea[Num]);
	string ImageName = "src//test.png";
	std::vector<Feature> features;
#define InputImageName "srceenshot:%d"
};
void initalData();
#endif