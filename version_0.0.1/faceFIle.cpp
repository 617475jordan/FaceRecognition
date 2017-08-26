#include "faceFile.h"
#include "face_identification.h"


seeta::FaceDetection detector("model//seeta_fd_frontal_v1.0.bin");
seeta::FaceAlignment point_detector("model//seeta_fa_v1.1.bin");
seeta::FaceIdentification face_recognizer("model//seeta_fr_v1.0.bin");

void initalData()
{
	detector.SetMinFaceSize(20);
	detector.SetScoreThresh(4.f);
	detector.SetImagePyramidScaleFactor(0.8f);
	detector.SetWindowStep(4, 4);

}

//逆时针旋转图像degree角度（原尺寸）    
void faceData::rotateImage(const cv::Mat src, const cv::Mat& img_rotate, double degree)
{
	//旋转中心为图像中心    
	Point2f center;
	center.x = float(src.cols / 2.0 + 0.5);
	center.y = float(src.rows / 2.0 + 0.5);
	int length = 0;
	length = sqrt(src.cols*src.cols + src.rows*src.rows);
	//计算二维旋转的仿射变换矩阵  
	Mat M = getRotationMatrix2D(center, degree, 1);
	warpAffine(src, img_rotate, M, Size(length, length));//仿射变换  
}
Mat  faceData::detectFace(Mat  probe_img_color, float gallery_fea[Num])
{
	FileStorage file;
	int faceNum;
	
	Mat out = probe_img_color;
	file.open("data//faceNum.xml", FileStorage::READ);
	if (!file.isOpened())
	{
		cout << "The nameFile didn't be imported" << endl;
		exit(-1);
	}
	else
	{
		file["faceNum"] >> faceNum;
		file.release();
		//		namedWindow(str, WINDOW_AUTOSIZE);
		//		imshow(str, srcImg);
	}
	file.open("data//faceFeature.xml", FileStorage::READ);
	double max_k;
	file["max"] >> max_k;
	file.release();
	int width = probe_img_color.cols;
	int height = probe_img_color.rows;
	cv::Mat probe_img_gray;
//	rtcprgb2Gray rtcprgb2Gray;
//	probe_img_gray = rtcprgb2Gray.rtcprgb2gray(probe_img_color);
	cv::cvtColor(probe_img_color, probe_img_gray, CV_BGR2GRAY);
	
    seeta::ImageData probe_img_data_color(width, height, 3);
	probe_img_data_color.data = probe_img_color.data;

	seeta::ImageData probe_img_data_gray(width, height, 1);
	probe_img_data_gray.data = probe_img_gray.data;

	std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);//release 50ms
	int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

	if ( probe_face_num == 0)
	{
	//	std::cout << "Faces are not detected." << endl;;
		file.open("data//faceNum.xml", FileStorage::WRITE);
		file << "faceNum" << probe_face_num;
		file.release();
		//return 0;
		return probe_img_color;
	}

	Rect face_rect;
	Mat img;
	Mat maxsimilarity;
	double max = 0;
	char fileName[100];
	for (int i = 0; i < probe_face_num; i++)
	{
		sprintf(fileName, InputImageName, i);
		face_rect.x = probe_faces[i].bbox.x;
		face_rect.y = probe_faces[i].bbox.y;
		face_rect.width = probe_faces[i].bbox.width;
		face_rect.height = probe_faces[i].bbox.height;
	//	rectangle(probe_img_color, face_rect, CV_RGB(255, 0, 0), 1, 8, 0);
		
		
		seeta::FacialLandmark probe_points[5];
		point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[i], probe_points);
		/**************特征点标志****************/
/*		for (int i = 0; i < 5; i++)
		{
			cv::circle(probe_img_color, cv::Point(probe_points[i].x, probe_points[i].y), 2,
				CV_RGB(0, 255, 0));
		}
*/
		double outdata;
		float sim;//相似值比较
//		if (faceNum != probe_face_num)
//		{
			float probe_fea[Num];
			clock_t starttime = clock();
			face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);//release 140ms
			cout << "time:" << clock() - starttime << "ms" << endl;
			sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);
			cout << fileName << endl;
			cout << "The similarity of two faces is(0.0-1.0):" << abs(sim) << endl;
			outdata = abs(sim);
	//	}

		if (outdata >= 0.65)
		{

			int tempx0 = probe_points[0].x < probe_points[3].x ? probe_points[0].x : probe_points[3].x;
			int tempx1 = probe_points[1].x > probe_points[4].x ? probe_points[1].x : probe_points[4].x;
			int nfaceW = (int)((tempx1 - tempx0)*2.2);
			int tempy0 = probe_points[0].y > probe_points[1].y ? probe_points[0].y : probe_points[1].y;
			int tempy1 = probe_points[3].y > probe_points[4].y ? probe_points[3].y : probe_points[4].y;
			int nfaceH = (int)((tempy1 - tempy0)*2.2);
			int x0 = probe_points[2].x - nfaceW / 2;
			int y0 = probe_points[2].y - nfaceW / 2;
			int w0 = nfaceW;
			int h0 = nfaceW;
			face_rect.x = x0;
			face_rect.y = y0;
			face_rect.width = w0;
			face_rect.height = h0;

			if (x0 < 0 || y0 < 0 || w0 < 0 || h0< 0)
			{
				return probe_img_color;
			//	return -1;
			}
			if (x0 + w0 > probe_img_color.cols || y0 + h0 > probe_img_color.rows)
			{
				return probe_img_color;
			//	return -1;
			}

			Mat img_rect(probe_img_color, face_rect);  //截取脸部  
			
			//	circle(img_rect, cvPoint(img_rect.cols / 2, img_rect.rows / 2), 2, CV_RGB(0, 255, 0), CV_FILLED);
			//  cout << "img_rect : " << img_rect.cols << ", " << img_rect.rows << endl;
			//	for (int i = 0; i < 5; i++)
			//	{
			//cout << i << ": " << points[i].x << ", " << points[i].y << endl;
			//		cv::circle(img, cvPoint(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
			//	}

			//-------计算角度--------  
			int x1 = probe_points[0].x;
			int y1 = probe_points[0].y;
			int x2 = probe_points[1].x;
			int y2 = probe_points[1].y;
			//cout << "0: " << x1 << ", " << y1 << endl;
			//cout << "1: " << x2 << ", " << y2 << endl;
			double dX = (double)(x2 - x1);
			double dY = (double)(y2 - y1);
			float Angle = (float)(atan(dY / dX) * 180 / 3.1415926);
			cout << "nAngle:" << Angle << endl;

			//--------------------------------  
			int length = 0;
			length = sqrt(img_rect.cols*img_rect.cols + img_rect.rows*img_rect.rows);
			cv::Mat tempImg(length, length, img_rect.type());//临时图像，大小和输出图像一样大  
			int ROI_x = length / 2 - img_rect.cols / 2;//ROI矩形左上角的x坐标    
			int ROI_y = length / 2 - img_rect.rows / 2;//ROI矩形左上角的y坐标    
			Rect ROIRect(ROI_x, ROI_y, img_rect.cols, img_rect.rows);//ROI矩形    
			Mat tempImgROI2(tempImg, ROIRect);//tempImg的中间部分  
			img_rect.copyTo(tempImgROI2);//将原图复制到tempImg的中心

			/*		float k = 0;
					k = 240.0 / tempImg.cols;
					resize(tempImg, tempImg, Size(k*tempImg.cols, k*tempImg.rows));
					namedWindow(fileName, WINDOW_AUTOSIZE);
					imshow(fileName, tempImg);
					waitKey(1);*/
			//		tempImg.release();

			cv::Mat dst_image;
			Point2f center(float(length / 2.0), float(length / 2.0));//旋转中心  
			Mat M = getRotationMatrix2D(center, (double)Angle, 1);//计算旋转的仿射变换矩阵  
			warpAffine(tempImg, dst_image, M, Size(length, length));//仿射变换  
			//		cout << "dst_image : " << dst_image.cols << ", " << dst_image.rows << endl;
			//circle(dst_image, cvPoint(dst_image.cols / 2, dst_image.rows / 2), 1, CV_RGB(255, 255, 255), CV_FILLED);
			if (max <= sim&&sim>max_k)
			{
				img = img_rect;
				file.open("data//faceFeature.xml", FileStorage::WRITE);
				file << "x" << abs(face_rect.x);
				file << "y" << abs(face_rect.y);
				file << "width" << face_rect.width;
				file << "height" << face_rect.height;
				file << "gotBB" << "true";
				file << "max" << sim;
				file.release();
				cout << "This is suspect " << endl;
			}
			namedWindow("dst_image", WINDOW_AUTOSIZE);
			imshow("dst_image", dst_image);
		}
		//-----------------------  
	}
	file.open("data//faceNum.xml", FileStorage::WRITE);
	file << "faceNum" << probe_face_num;
	file.release();
//	imwrite(filenam, probe_img_color);
	return out;
	//return probe_face_num;


}
int faceData::initalImage(string ImageName)
{
	cv::Mat gallery_img_color = imread(ImageName, 1);
	if (gallery_img_color.empty())
	{
		cout << "Don't find the images" << endl;
		return -1;
	}
	cv::Mat gallery_img_gray;
	cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);
//	rtcprgb2Gray rtcprgb2Gray;
//	gallery_img_gray = rtcprgb2Gray.rtcprgb2gray(gallery_img_color);
    seeta::ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
	gallery_img_data_color.data = gallery_img_color.data;

    seeta::ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
	gallery_img_data_gray.data = gallery_img_gray.data;
	// Detect faces
	std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
	int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

	if (gallery_face_num == 0 )
	{
		std::cout << "Faces are not detected.";
		system("pause");
	}
	// Detect 5 facial landmarks
	seeta::FacialLandmark gallery_points[5];
	point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);

	// Extract face identity feature
	float gallery_fea[Num];
	face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
	
	FileStorage file;
	file.open("data//data.xml", FileStorage::WRITE);
	char fileName[100];
	for (int i = 0; i < Num; i++)
	{
		sprintf(fileName, "NO%d", i);
		file << fileName << gallery_fea[i];
	}
	file.release();
	return 0;
}




