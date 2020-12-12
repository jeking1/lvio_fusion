#ifndef lvio_fusion_FRONTEND_H
#define lvio_fusion_FRONTEND_H

#include "lvio_fusion/common.h"
#include "lvio_fusion/frame.h"
#include "lvio_fusion/imu/imu.hpp"
#include "lvio_fusion/imu/initializer.h"
#include "lvio_fusion/map.h"
#include "lvio_fusion/visual/camera.hpp"
#include "lvio_fusion/DynamicExtractor.h"


namespace lvio_fusion
{

class Backend;
class DynamicExtractor;

enum class FrontendStatus
{
    BUILDING,
    INITIALIZING,
    TRACKING_GOOD,
    TRACKING_BAD,
    TRACKING_TRY,
    LOST
};

enum Flag
{
    None = 0,
    Mono = 1,
    Stereo = 1 << 1,
    RGBD = 1 << 2,
    IMU = 1 << 3,
    Laser = 1 << 4,
    GNSS = 1 << 5,
    Semantic = 1 << 6,
};

class Frontend
{
public:
    typedef std::shared_ptr<Frontend> Ptr;

    Frontend(int num_features, int init, int tracking, int tracking_bad, int need_for_keyframe, bool use_dy=false, int num_new_freature_skip=1, std::string dynamic_model_path="");

    bool AddFrame(Frame::Ptr frame);

    void AddImu(double time, Vector3d acc, Vector3d gyr);

    void SetMap(Map::Ptr map) { map_ = map; }

    void SetBackend(std::shared_ptr<Backend> backend) { backend_ = backend; }

    void SetCameras(Camera::Ptr left, Camera::Ptr right)
    {
        camera_left_ = left;
        camera_right_ = right;
    }

    void SetImu(Imu::Ptr imu)
    {
        imu_ = imu;
    }

    void UpdateCache();

    int flags = Flag::None;
    FrontendStatus status = FrontendStatus::BUILDING;
    Frame::Ptr current_frame;
    Frame::Ptr last_frame;
    Frame::Ptr current_key_frame;
    SE3d relative_i_j;
    std::mutex mutex;

private:
    bool Track();

    bool Reset();

    int TrackLastFrame();

    bool InitFramePoseByPnP();

    void CreateKeyframe(bool need_new_features = true);

    bool InitMap();

    int DetectNewFeatures();

    int TriangulateNewPoints();

    // data
    Map::Ptr map_;
    std::weak_ptr<Backend> backend_;
    std::unordered_map<unsigned long, Vector3d> position_cache_;
    SE3d last_frame_pose_cache_;

    Camera::Ptr camera_left_;
    Camera::Ptr camera_right_;
    Imu::Ptr imu_;

    // params
    int num_features_;
    int num_features_init_;
    int num_features_tracking_;
    int num_features_tracking_bad_;
    int num_features_needed_for_keyframe_;

	DynamicExtractor* mask_extractor;
	bool use_dynamic;
};

} // namespace lvio_fusion

#endif // lvio_fusion_FRONTEND_H
