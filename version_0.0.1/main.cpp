#include "mainHead.h"

int main()
{
	mainHead mainHead;
	int choice;
	mainHead.initialData();
	faceData faceData;
	initalData();

	while (1)
	{
		std::cout << "Input 1 to build database index, 2 to test image,3 to exit: ";
		std::cin >> choice;

		if (1 == choice)
		{
			FileStorage file;
			file.open("data//name.xml", FileStorage::WRITE);
			string str;
			cout << "Please input the source file name:";
			cin >> str;
			file << "name" << str;
			file.release();
			faceData.initalImage(str);
		}
		else if (2 == choice)
		{
			mainHead.Video();
		}
		else if (3 == choice)
		{
			cout << "exit" << endl;
			return 0;
		}

		else
		{
			cout << "Input Num is error!" << endl;
		}
	}
	return 0;
}
int mainHead::Video()
{

	VideoCapture capture;
//  capture.open(videoName);//读入视频流
	capture.open(0);
	/*********读取模块数据***********/
	FileStorage fs;
	fs.open("data//parameters.yml", FileStorage::READ);
	TLD tld;


	if (!capture.isOpened())
	{
		cout << "Camera is failed" << endl;
		system("pause");
		return -1;
	}
	else
	{
		FileStorage file;
		string str;
		Mat srcImg;
		file.open("data//name.xml", FileStorage::READ);
		if (!file.isOpened() || !fs.isOpened())
		{
			cout << "The nameFile didn't be imported" << endl;
			exit(-1);
		}
		else
		{
			/******数据初始化**********/
			file["name"] >> str;
			srcImg = imread(str);
			file.release();
			tld.read(fs.getFirstTopLevelNode());

		}

		Mat frame;
		Mat last_gray;
		Mat first;
		if (fromfile)
		{
			//	waitKey(100);
			capture >> frame;
			resize(frame, frame, Size(320, 240));
			cvtColor(frame, last_gray, CV_RGB2GRAY);
			frame.copyTo(first);
		}
		else
		{
			capture.set(CV_CAP_PROP_FRAME_WIDTH, 320 * scale);
			capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240 * scale);
		}
	GETBOUNDINGBOX:
		while (!gotBB)
		{

			if (!fromfile)
			{
				capture >> frame;
			}
			else
			{
				first.copyTo(frame);
			}
			if (!frame.empty())
			{
				resize(frame, frame, Size(320, 240));
				cvtColor(frame, last_gray, CV_RGB2GRAY);
				faceData faceData;
				frame = faceData.detectFace(frame, gallery_fea);//人脸识别

				file.open("data//faceFeature.xml", FileStorage::READ);
				if (!file.isOpened() || !fs.isOpened())
				{
					cout << "The nameFile didn't be imported" << endl;
					file.release();
					exit(-1);
				}
				file["x"] >> box.x;
				file["y"] >> box.y;
				file["width"] >> box.width;
				file["height"] >> box.height;
				file["gotBB"] >> gotBB;
				file.release();

				drawBox(frame, box, color);
				imshow("TLD", frame);
				if (waitKey(33) == 'q')
					return 0;
			}
		}

		//判断识别框的大小
		if (min(box.width, box.height) < (int)fs.getFirstTopLevelNode()["min_win"])
		{
			cout << "Bounding box too small, try again." << endl;
			gotBB = false;
			goto GETBOUNDINGBOX;
		}
		//Remove callback
		printf("Initial Bounding Box = x:%d y:%d h:%d w:%d\n", box.x, box.y, box.width, box.height);
		//Output file
		FILE  *bb_file = fopen("bounding_boxes.txt", "w");
		//TLD initialization
		tld.init(last_gray, box, bb_file);

		///Run-time
		Mat current_gray;
		BoundingBox pbox;
		vector<Point2f> pts1;
		vector<Point2f> pts2;
		bool status = true;
		int frames = 1;
		int detections = 1;

	REPEAT:
		while (capture.read(frame))
		{
			resize(frame, frame, Size(320, 240));
			clock_t starttime = clock();
			cvtColor(frame, current_gray, CV_RGB2GRAY);
			//Process Frame
			tld.processFrame(last_gray, current_gray, pts1, pts2, pbox, status, tl, bb_file);
			//Draw Points
			if (status)
			{
				drawBox(frame, pbox, color);
				detections++;
			}
			//Display
			imshow("TLD", frame);
			cout << "Total time:" << clock() - starttime << "ms" << endl;
			//swap points and images
			swap(last_gray, current_gray);
			pts1.clear();
			pts2.clear();
			frames++;
			printf("Detection rate: %d/%d\n", detections, frames);
			if (waitKey(33) == 'q')
				break;
		}
		if (rep)
		{
			rep = false;
			tl = false;
			fclose(bb_file);
			bb_file = fopen("final_detector.txt", "w");
			capture.release();
			capture.open(video);
			goto REPEAT;
		}
		fclose(bb_file);
	}
	return 0;
}
/**************初始化数据*******************/
void mainHead::initialData()
{

	FileStorage file;
	file.open("data//data.xml", FileStorage::READ);
	if (!file.isOpened())
	{
		cout << "File didn't  be open" << endl;
	}
	else
	{
		char fileName[100];
		for (int i = 0; i < Num; i++)
		{
			sprintf(fileName, "NO%d", i);
			file[fileName] >> gallery_fea[i];
		}
		file.release();
	}
	file.open("data//faceNum.xml", FileStorage::WRITE);
	file << "faceNum" << 0;
	file.release();

	/**********初始化目标矩阵坐标值************/
	file.open("data//faceFeature.xml", FileStorage::WRITE);
	file << "x" << 0;
	file << "y" << 0;
	file << "width" << 0;
	file << "height" << 0;
	file << "gotBB" << false;
	file.release();
}

mainHead::mainHead()
{

}
mainHead::~mainHead()
{

}