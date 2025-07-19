#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include "Detector.hpp"
#include "Constants.hpp"
#include "Skull.hpp"

cv::dnn::Net Detector::net1;
cv::dnn::Net Detector::net2;
int Detector::regionArea;
int Detector::leftBoundaryX;
int Detector::middleBoundaryX;
int Detector::rightBoundaryX;

void Detector::Init()
{
	LoadDetectorModels();
	AllocateSkullData();
}

std::vector<Skull> Detector::DetectSkulls(cv::Mat& frame, SkullType skullType)
{
	using namespace Constants;

	// Determine which type of skull we want to detect
	cv::dnn::Net* pNet;
	cv::Scalar color;
	float confidenceThreshold;
	switch (skullType)
	{
		case SkullType::SKULL1:
		default:
			confidenceThreshold = SKULL1_CONFIDENCE_THRESHOLD;
			pNet = &net1;
			color = { 0.0, 255.0, 0.0 };
			break;
		case SkullType::SKULL2:
			pNet = &net2;
			confidenceThreshold = SKULL2_CONFIDENCE_THRESHOLD;
			color = { 0.0, 0.0, 255.0 };
			break;
	}
	cv::dnn::Net& net = *pNet;

	// Letterbox
	float scale = std::min(
		F_YOLO_IMGSZ / frame.cols,
		F_YOLO_IMGSZ / frame.rows
	);

	// Resize image to 640 x 640
	cv::Mat resized;
	cv::resize(frame, resized, cv::Size(), scale, scale);

	// Letterboxing
	int pad_x = (I_YOLO_IMGSZ - resized.cols) / 2;
	int pad_y = (I_YOLO_IMGSZ - resized.rows) / 2;
	cv::Mat letterbox;
	cv::copyMakeBorder(
		resized, letterbox,
		pad_y, I_YOLO_IMGSZ - resized.rows - pad_y,
		pad_x, I_YOLO_IMGSZ - resized.cols - pad_x,
		cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114)
	);

	// Set the input image
	cv::Mat blob = cv::dnn::blobFromImage(letterbox, 1.0f / 255.0f, cv::Size(I_YOLO_IMGSZ, I_YOLO_IMGSZ), cv::Scalar(), true, false);
	net.setInput(blob);

	// Run the detection algorithm
	std::vector<cv::Mat> outputs;
	net.forward(outputs, net.getUnconnectedOutLayersNames());

	cv::Mat output = outputs[0]; // [1, 5, 8400]
	cv::Mat reshaped = output.reshape(1, 5); // [1, 5, 8400] -> [5, 8400]
	cv::Mat transposed;
	cv::transpose(reshaped, transposed); // [5, 8400] -> [8400, 5]

	// Create boxes around detections
	std::vector<cv::Rect> boxes;
	std::vector<float> confidences;
	for (int i = 0; i < transposed.rows; ++i)
	{
		float* data = transposed.ptr<float>(i);

		// Skip weak predictions
		float confidence = data[4];
		if (confidence < confidenceThreshold) continue;

		float x = data[0]; // Center X (normalized: 0 to 1)
		float y = data[1]; // Center Y
		float w = data[2]; // Box width
		float h = data[3]; // Box height

		float left = x - w / 2.0f;
		float top  = y - h / 2.0f;

		left -= pad_x;
		top  -= pad_y;

		left /= scale;
		top  /= scale;
		w    /= scale;
		h    /= scale;

		int final_left = static_cast<int>(std::max(0.0f, left));
		int final_top = static_cast<int>(std::max(0.0f, top));
		int final_w = static_cast<int>(std::min(w, static_cast<float>(frame.cols - final_left)));
		int final_h = static_cast<int>(std::min(h, static_cast<float>(frame.rows - final_top)));

		cv::Rect skullBox(final_left, final_top, final_w, final_h);
		boxes.push_back(std::move(skullBox));
		confidences.push_back(confidence);
	}

	// Prune overlapping boxes
	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, NMS_THRESHOLD, indices);

	// Draw leftover boxes
	std::vector<Skull> skulls;
	for (int i : indices)
	{
		cv::Rect const& skullBox = boxes[i];
		int cx = skullBox.x + skullBox.width  / 2;
		int cy = skullBox.y + skullBox.height / 2;
		skulls.emplace_back(Skull{ cv::Point{ cx, cy }, skullBox.area(), skullType });
	}

    return skulls;
}

void Detector::LoadDetectorModels()
{
	net1 = cv::dnn::readNetFromONNX("models/skull1.onnx");
	net1.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net1.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

	net2 = cv::dnn::readNetFromONNX("models/skull2.onnx");
	net2.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net2.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
}

void Detector::AllocateSkullData()
{
}

DXCam::Region Detector::ComputeGameRegion(HWND hwnd)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	POINT ul{ rect.left, rect.top }, lr{ rect.right, rect.bottom };
	ClientToScreen(hwnd, &ul);
	ClientToScreen(hwnd, &lr);

	DXCam::Region region{ ul.x, ul.y, lr.x, lr.y };
	ComputeXBoundaries(region);
	regionArea = region.get_width() * region.get_height();

	return region;
}

void Detector::ComputeXBoundaries(DXCam::Region const& region)
{
	int width = region.get_width();
	leftBoundaryX = width / 4;
	middleBoundaryX = width / 2;
	rightBoundaryX = width * 3 / 4;
}

bool Detector::IsAgentDead(cv::Mat const& frame)
{
	// Check for red screen
	cv::Mat frameHSV;
	cv::cvtColor(frame, frameHSV, cv::COLOR_BGR2HSV);
	cv::Mat redMask1, redMask2;
	cv::inRange(frameHSV, cv::Scalar(0.0, 250.0, 12.0),  cv::Scalar(0.0, 255.0, 30.0), redMask1);
	cv::inRange(frameHSV, cv::Scalar(0.0, 250.0, 120.0), cv::Scalar(0.0, 255.0, 128.0), redMask2);
	cv::Mat redMask = redMask1 | redMask2;
	float redPercentage = static_cast<float>(cv::countNonZero(redMask)) / static_cast<float>(redMask.total());
	return redPercentage > Constants::GAME_OVER_RED_THRESHOLD;
}
