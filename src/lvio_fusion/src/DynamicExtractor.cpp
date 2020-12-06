#include "lvio_fusion/DynamicExtractor.h"

using namespace std;
using namespace cv;
using namespace dnn;

namespace lvio_fusion 
{
    DynamicExtractor::DynamicExtractor(const string &strModelPath, int maxUsage, bool useOpticalFlow,
                                       float confThreshold, float maskThreshold) : maxUsage(maxUsage),
                                                                                   confThreshold(confThreshold),
                                                                                   maskThreshold(maskThreshold),
                                                                                   useOpticalFlow(useOpticalFlow){
        cout<<"maks中初始化开始"<<endl;
        maskUsage = 0;
        string textGraph = strModelPath + "mask_rcnn_inception_v2_coco_2018_01_28.pbtxt";
        string modelWeights = strModelPath +  "frozen_inference_graph.pb";
        string classesFile = strModelPath + "mscoco_labels.names";
        string dynamicClassFile = strModelPath + "dynamic.txt";

        // Load names of classes
        ifstream ifs(classesFile.c_str());
        string line;
        while (getline(ifs, line)) classes.push_back(line);

        // load names of dynamic classes
        ifstream ifs2(dynamicClassFile.c_str());
        while (getline(ifs2, line)) dynamicClasses.insert(line);

        // Load the network
        net = readNetFromTensorflow(modelWeights, textGraph);
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        // should be able to use intel GPU
        net.setPreferableTarget(DNN_TARGET_OPENCL);
        cout<<"maks中初始化结束"<<endl;

    }

    Mat DynamicExtractor::extractMask(const Mat &frame) {
        cout<<"maks中extract开始"<<endl;
        //Mat blob;
        // Create 4D blob from a frame as input
        cout<<"maks中blobFromImage开始"<<endl;
        //imshow("maks中blobFromImage开始", frame);
        //waitKey(0);
        Mat blob = blobFromImage(frame, 1.0, Size(frame.cols, frame.rows), Scalar(), true, false);
        cout<<"maks中setInput开始"<<endl;
        //cout<<frame.cols<<endl;
        //cout<<frame.rows<<endl;
        //cout<<blob.size()<<endl;
        net.setInput(blob);

        // Runs the forward pass to get output from the output layers
        std::vector<String> outNames{"detection_out_final", "detection_masks"};
        vector<Mat> outs;
        cout<<"maks中net.forward开始"<<endl;
        net.forward(outs, outNames);
        cout<<"maks中net.forward结束"<<endl;
        Mat outDetections = outs[0];
        Mat outMasks = outs[1];

        // Output size of masks is NxCxHxW where
        // N - number of detected boxes
        // C - number of classes (excluding background)
        // HxW - segmentation shape
        const int numClasses = outMasks.size[1];

        // Output size of Detection size is 1 * 1 * numDetections * 7
        // 7 refers to classId, score, and detection box information
        const int numDetections = outDetections.size[2];
        // reshape to channel = 1, row = num of detections
        // now outDetection size is numDetections * 7
        cout<<"mask里面的reshape开始"<<endl;
        outDetections = outDetections.reshape(1, outDetections.total() / 7);
        cout<<"mask里面的reshape结束"<<endl;

        // aggregate binary mask of dynamic objects into dynamic_mask
        // dynamic part should be zero
        Mat dynamic_mask = Mat(frame.size(), CV_8U, Scalar(255));
        Mat mat_zeros = Mat::zeros(frame.size(), CV_8U);
        for (int i = 0; i < numDetections; ++i) {
            float score = outDetections.at<float>(i, 2);
            if (score > confThreshold) {
                // Extract class id
                int classId = static_cast<int>(outDetections.at<float>(i, 1));

                // Extract bounding box
                int left = static_cast<int>(frame.cols * outDetections.at<float>(i, 3));
                int top = static_cast<int>(frame.rows * outDetections.at<float>(i, 4));
                int right = static_cast<int>(frame.cols * outDetections.at<float>(i, 5));
                int bottom = static_cast<int>(frame.rows * outDetections.at<float>(i, 6));

                left = max(0, min(left, frame.cols - 1));
                top = max(0, min(top, frame.rows - 1));
                right = max(0, min(right, frame.cols - 1));
                bottom = max(0, min(bottom, frame.rows - 1));
                Rect box = Rect(left, top, right - left + 1, bottom - top + 1);

                // Extract the mask for the object
                Mat objectMask(outMasks.size[2], outMasks.size[3], CV_32F, outMasks.ptr<float>(i, classId));
                // Resize the mask, threshold, color and apply it on the image
                resize(objectMask, objectMask, Size(box.width, box.height));
                // threshold mask into binary 255/0 mask
                Mat mask = (objectMask > maskThreshold);
                mask.convertTo(mask, CV_8U);

                if (is_dynamic(classId)) {
                    // copy ones into the corresponding mask region
                    mat_zeros(box).copyTo(dynamic_mask(box), mask);
                }
            }
        }
        cout<<"maks中extract结束"<<endl;
        return dynamic_mask;
    }

    void DynamicExtractor::extractMask(const Mat &frame, Mat &dynamic_mask) {
        //// if maskUsage <= masUsage, resuse prevMask
        //if (prevMask.empty() || maskUsage >= maxUsage) {
        //    prevFrame = frame.clone();
        //    prevMask = extractMask(frame);
        //    dynamic_mask = prevMask.clone();
        //    maskUsage = 0;
        //} else if (useOpticalFlow) {
        //    cv::Mat flow;
        //    calcOpticalFlowFarneback(frame, prevFrame, flow, 0.5, 3, 20, 3, 5, 1.2, 0);
        //    propagate_mask(prevMask, dynamic_mask, flow);
        //} else {
        //    dynamic_mask = prevMask.clone();
        //}
        //maskUsage++;
		dynamic_mask = extractMask(frame);
    }
}
