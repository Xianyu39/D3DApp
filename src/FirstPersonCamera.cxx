#include "FirstPersonCamera.h"

using namespace DirectX;

FirstPersonCamera::~FirstPersonCamera() = default;
// ���������λ��
void FirstPersonCamera::SetPosition(float x, float y, float z) {
    transform.SetPosition({ x,y,z });
}
void FirstPersonCamera::SetPosition(const XMFLOAT3& pos) {
    transform.SetPosition(pos);
}
// ����������ĳ���
void FirstPersonCamera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up) {
    SetPosition(pos);
    transform.LookAt(target, up);
}
void FirstPersonCamera::LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up) {
    LookAt(pos, to, up);
}
// ƽ��
void FirstPersonCamera::Strafe(float d) {
    transform.Translate(GetRightAxis(), d);
}
// ֱ��(ƽ���ƶ�)
void FirstPersonCamera::Walk(float d) {
    auto rightVec = transform.GetRightAxisXM();
    auto frontVec = XMVector3Normalize(XMVector3Cross(rightVec, g_XMIdentityR1));
    XMFLOAT3 front;
    XMStoreFloat3(&front, frontVec);
    transform.Translate(front, d);
}
// ǰ��(��ǰ���ƶ�)
void FirstPersonCamera::MoveForward(float d) {
    transform.Translate(GetLookAxis(), d);
}
// ���¹۲�
// ��radֵ���Ϲ۲�
// ��radֵ���¹۲�
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
// ���ҹ۲�
// ��radֵ����۲�
// ��radֵ���ҹ۲�
void FirstPersonCamera::RotateY(float rad) {
    XMFLOAT3 rotation = transform.GetRotation();
    rotation.y = XMScalarModAngle(rotation.y + rad);
    transform.SetRotation(rotation);
}