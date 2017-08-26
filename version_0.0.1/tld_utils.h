#include <opencv2/opencv.hpp>
#pragma once



void drawBox(cv::Mat& image, CvRect box, cv::Scalar color = cvScalar(255,0,0), int thick=1); 

void drawPoints(cv::Mat& image, std::vector<cv::Point2f> points,cv::Scalar color=cvScalar(255,255,0));

cv::Mat createMask(const cv::Mat& image, CvRect box);

float median(std::vector<float> v);

std::vector<int> index_shuffle(int begin,int end);

