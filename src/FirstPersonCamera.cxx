#include "FirstPersonCamera.h"

using namespace DirectX;

FirstPersonCamera::~FirstPersonCamera() = default;
// 设置摄像机位置
void FirstPersonCamera::SetPosition(float x, float y, float z) {
    transform.SetPosition({ x,y,z });
}
void FirstPersonCamera::SetPosition(const XMFLOAT3& pos) {
    transform.SetPosition(pos);
}
// 设置摄像机的朝向
void FirstPersonCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up) {
    SetPosition(pos);
    transform.LookAt(target, up);
}
void FirstPersonCamera::LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up) {
    LookAt(pos, to, up);
}
// 平移
void FirstPersonCamera::Strafe(float d) {
    transform.Translate(GetRightAxis(), d);
}
// 直行(平面移动)
void FirstPersonCamera::Walk(float d) {
    auto rightVec = transform.GetRightAxisXM();
    auto frontVec = XMVector3Normalize(XMVector3Cross(rightVec, g_XMIdentityR1));
    XMFLOAT3 front;
    XMStoreFloat3(&front, frontVec);
    transform.Translate(front, d);
}
// 前进(朝前向移动)
void FirstPersonCamera::MoveForward(float d) {
    transform.Translate(GetLookAxis(), d);
}
// 上下观察
// 正rad值向上观察
// 负rad值向下观察
void FirstPersonCamera::Pitch(float rad) {
    auto rollPitchYaw = transform.GetRotation();
    rollPitchYaw.x += rad;
    if (rollPitchYaw.x > XM_PI * 7 / 18) {
        rollPitchYaw.x = XM_PI * 7 / 18;
    }
    if (rollPitchYaw.x < -XM_PI * 7 / 18) {
        rollPitchYaw.x = -XM_PI * 7 / 18;
    }
    transform.SetRotation(rollPitchYaw);
}
// 左右观察
// 正rad值向左观察
// 负rad值向右观察
void FirstPersonCamera::RotateY(float rad) {
    XMFLOAT3 rotation = transform.GetRotation();
    rotation.y = XMScalarModAngle(rotation.y + rad);
    transform.SetRotation(rotation);
}