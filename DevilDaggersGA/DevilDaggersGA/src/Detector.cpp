#include "Detector.hpp"
#include "Constants.hpp"
#include <sstream>
#include <iomanip>

cv::dnn::Net Detector::net1;
cv::dnn::Net Detector::net2;
cv::Mat Detector::gameOverTemplate;

void Detector::Init()
{
	gameOverTemplate = cv::imread("samples/score_submitted.png", cv::IMREAD_COLOR);
	LoadDetectorModels();
	AllocateSkullData();
}

std::vector<cv::Point> Detector::DetectSkulls(cv::Mat& frame, SkullType skullType)
{
	// Determine which type of skull we want to detect
	cv::dnn::Net* pNet;
	cv::Scalar color;
	switch (skullType)
	{
		case SkullType::SKULL1:
		default:
			pNet = &net1;
			color = { 0.0, 255.0, 0.0 };
			break;
		case SkullType::SKULL2:
			pNet = &net2;
			color = { 0.0, 0.0, 255.0 };
			break;
	}
	cv::dnn::Net& net = *pNet;

	using namespace Constants;

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
	std::vector<cv::Point> points;
	for (int i = 0; i < transposed.rows; ++i)
	{
		float* data = transposed.ptr<float>(i);

		// Skip weak predictions
		float confidence = data[4];
		if (confidence < CONFIDENCE_THRESHOLD) continue;

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
		boxes.push_back(skullBox);
		confidences.push_back(confidence);
	}

	// Prune overlapping boxes
	std::vector<int> indices;
	cv::dnn::NMSBoxes(boxes, confidences, CONFIDENCE_THRESHOLD, NMS_THRESHOLD, indices);

	// Draw leftover boxes
	for (int i : indices)
	{
		cv::Rect const& skullBox = boxes[i];
		float confidence = confidences[i];

		// Draw box
		cv::rectangle(frame, skullBox, color, 2);

		// Draw label
		std::ostringstream label_ss;
		label_ss << std::fixed << std::setprecision(3) << confidence;
		std::string label = "Conf: " + label_ss.str();

		int baseline = 0;
		cv::Size label_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 2.0, 2, &baseline);

		cv::putText(frame, label,
			cv::Point(skullBox.x, skullBox.y - baseline),
			cv::FONT_HERSHEY_SIMPLEX, 1.0,
			cv::Scalar(255, 255, 255), 2);

		float cx = skullBox.x + skullBox.width / 2.f;
		float cy = skullBox.y + skullBox.height / 2.f;
		points.emplace_back(cv::Point(cx, cy));
	}

    return points;
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

	return { ul.x, ul.y, lr.x, lr.y };
}

void Detector::ComputeGameOverROI(DXCam::Region const& region)
{
}
