#ifndef _MAINHEAD_H_
#define _MAINHEAD_H_
#include <face_alignment.h>
#include "faceFile.h"
#include <conio.h>
#include <time.h>
#include <face_alignment.h>
#include <face_identification.h>
#include <face_detection.h>
#include <opencv2/gpu/gpu.hpp>
#include "TLD.h"
#include "tld_utils.h"
class mainHead
{
public:
	void initialData();
	int Video();
	mainHead();
	~mainHead();
public:
#define scale 1//放大系数
#define Num 2048 //特征点个数
#define videoName "video//social.mp4"
#define imageName "src//2.jpg"
	float gallery_fea[Num];

public:
	Rect box;
	bool drawing_box = false;
	bool gotBB = false;
	bool tl = true;
	bool rep = false;
	bool fromfile = false;
	string video;
	Scalar color = Scalar(255, 0, 0);
};
#endif