#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include <string>
#include<iostream>
using std::string;
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "face_detection.h"
#include "face_alignment.h"
#include "face_identification.h"

class Detector : public seeta::FaceDetection{
public:
	Detector(const char * model_name);
};

class FaceRecognition{
public:
	FaceRecognition();
	float* NewFeatureBuffer();
	float FeatureCompare(float* feat1, float* feat2);
	int GetFeatureDims();
	bool GetFeature(string filename, float* feat);
	~FaceRecognition();
public:
	Detector* detector;
	seeta::FaceAlignment* point_detector;
	seeta::FaceIdentification* face_recognizer;
	
};

#endif // !FACERECOGNITION_H
