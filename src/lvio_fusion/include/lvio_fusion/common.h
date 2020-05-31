#ifndef lvio_fusion_COMMON_H
#define lvio_fusion_COMMON_H

// std
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

// define the commonly included file to avoid a long include list
#include <Eigen/Core>
#include <Eigen/Geometry>

using namespace Eigen;

// for Sophus
#include <sophus/se3.hpp>
#include <sophus/so3.hpp>

typedef Sophus::SE3d SE3;
typedef Sophus::SO3d SO3;

// opencv
#include <opencv2/core/core.hpp>

// PCL
#include <pcl/common/common_headers.h>

typedef pcl::PointXYZI PointI;
typedef typename pcl::PointCloud<PointI> PointCloudI;
typedef pcl::PointXYZRGB PointRGB;
typedef typename pcl::PointCloud<PointRGB> PointCloudRGB;

// glog
#include <glog/logging.h>

#endif // lvio_fusion_COMMON_H