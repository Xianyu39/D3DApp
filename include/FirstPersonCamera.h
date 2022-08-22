#if !defined(FIRSTPERSONCAMERA_H)
#define FIRSTPERSONCAMERA_H

#include "Camera.h"

using namespace DirectX;

class FirstPersonCamera :public Camera {
protected:
    XMFLOAT3 target = {};
    float dist = 0.f;
    float minDist = 0.f, maxDist = 0.f;
public:
    FirstPersonCamera() = default;
    ~FirstPersonCamera() override;
    // 设置摄像机位置
    void SetPosition(float x, float y, float z);
    void SetPosition(const XMFLOAT3& pos);
    // 设置摄像机的朝向
    void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
    void LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up);
    // 平移
    void Strafe(float d);
    // 直行(平面移动)
    void Walk(float d);
    // 前进(朝前向移动)
    void MoveForward(float d);
    // 上下观察
    // 正rad值向上观察
    // 负rad值向下观察
    void Pitch(float rad);
    // 左右观察
    // 正rad值向左观察
    // 负rad值向右观察
    void RotateY(float rad);
};

#endif // FIRSTPERSONCAMERA_H
