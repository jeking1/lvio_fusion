#ifndef lvio_fusion_MAPPING_H
#define lvio_fusion_MAPPING_H

#include "lvio_fusion/backend.h"
#include "lvio_fusion/common.h"
#include "lvio_fusion/frontend.h"
#include "lvio_fusion/lidar/lidar.hpp"
#include "lvio_fusion/lidar/association.h"
#include "lvio_fusion/map.h"
#include "lvio_fusion/visual/camera.hpp"

namespace lvio_fusion
{

class Mapping
{
public:
    typedef std::shared_ptr<Mapping> Ptr;

    Mapping();

    void SetLidar(Lidar::Ptr lidar) { lidar_ = lidar; }

    void SetCamera(Camera::Ptr camera) { camera_ = camera; }

    void SetMap(Map::Ptr map) { map_ = map; }

    void SetFeatureAssociation(FeatureAssociation::Ptr association) { association_ = association; }

    void SetFrontend(Frontend::Ptr frontend) { frontend_ = frontend; }

    void SetBackend(Backend::Ptr backend) { backend_ = backend; }

    void Optimize(Frames &active_kfs);

    PointRGBCloud GetGlobalMap();

    double head = 0;
    std::mutex mutex;

private:
    void MappingLoop();

    void BuildProblem(Frames &active_kfs, ceres::Problem &problem);

    void AddToWorld(const PointICloud &in, Frame::Ptr frame, PointRGBCloud &out);

    void BuildGlobalMap(Frames &active_kfs);

    Map::Ptr map_;
    FeatureAssociation::Ptr association_;
    Frontend::Ptr frontend_;
    Backend::Ptr backend_;

    std::thread thread_;
    std::mutex running_mutex_, pausing_mutex_;
    std::condition_variable running_;
    std::condition_variable pausing_;
    std::condition_variable started_;
    std::map<double, PointRGBCloud> pointclouds_;
    
    Lidar::Ptr lidar_;
    Camera::Ptr camera_;
};

} // namespace lvio_fusion

#endif // lvio_fusion_MAPPING_H
