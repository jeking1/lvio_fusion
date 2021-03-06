#ifndef lvio_fusion_AGENT_H
#define lvio_fusion_AGENT_H

#include "lvio_fusion/adapt/problem.h"
#include "lvio_fusion/adapt/weights.h"

namespace lvio_fusion
{

class Core
{
public:
    virtual void UpdateWeights(adapt::Problem &problem, Weights &weights){};
};

class Agent
{
public:
    static void SetCore(Core *core)
    {
        Agent::instance_ = new Agent(core);
    }

    static Agent* Instance()
    {
        return instance_;
    }

    void UpdateWeights(adapt::Problem &problem, Weights &weights)
    {
        // if (problem.num_types[ProblemType::LidarPlaneErrorRPZ] < 200)
        // {
        //     weights.lidar_ground[0] *= 1000;
        //     weights.lidar_ground[1] *= 1000;
        // }
        // if (problem.num_types[ProblemType::LidarPlaneErrorYXY] < 200)
        // {
        //     weights.lidar_surf[0] *= 1000;
        //     weights.lidar_surf[1] *= 1000;
        //     weights.lidar_surf[2] *= 1000;
        // }
        core_->UpdateWeights(problem, weights);
    }

private:
    Agent(Core* core)
    {
        core_ = core;
    }
    Agent(const Agent &);
    Agent & operator = (const Agent &);

    Core *core_;
    static Agent *instance_;
};

} // namespace lvio_fusion

#endif // lvio_fusion_AGENT_H
