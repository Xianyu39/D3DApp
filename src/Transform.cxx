#include "Transform.h"

using namespace DirectX;

// 获取对象缩放比例
XMFLOAT3 Transform::GetScale() const{
    return scale;
}
// 获取对象缩放比例
XMVECTOR Transform::GetScaleXM() const {
    return XMLoadFloat3(&scale);
}
// 获取对象欧拉角(弧度制)
// 对象以Z-X-Y轴顺序旋转
XMFLOAT3 Transform::GetRotation() const {
    return rollPitchYaw;
}
// 获取对象欧拉角(弧度制)
// 对象以Z-X-Y轴顺序旋转
XMVECTOR Transform::GetRotationXM() const {
    return XMLoadFloat3(&rollPitchYaw);
}
// 获取对象位置
XMFLOAT3 Transform::GetPosition() const {
    return pos;
}
// 获取对象位置
XMVECTOR Transform::GetPositionXM() const {
    return XMLoadFloat3(&pos);
}
// 获取右方向轴
XMFLOAT3 Transform::GetRightAxis() const {
    XMFLOAT3 right;
    XMStoreFloat3(&right, GetRightAxisXM());
    return right;
}
// 获取右方向轴
XMVECTOR Transform::GetRightAxisXM() const {
    return XMMatrixRotationRollPitchYawFromVector(GetRotationXM()).r[0];
}
// 获取上方向轴
XMFLOAT3 Transform::GetUpAxis() const {
    XMFLOAT3 up;
    XMStoreFloat3(&up, GetUpAxisXM());
    return up;
}
// 获取上方向轴
XMVECTOR Transform::GetUpAxisXM() const {
    return XMMatrixRotationRollPitchYawFromVector(GetRotationXM()).r[1];
}
// 获取前方向轴
XMFLOAT3 Transform::GetForwardAxis() const {
    XMFLOAT3 forward;
    XMStoreFloat3(&forward, GetForwardAxisXM());
    return forward;
}
// 获取前方向轴
XMVECTOR Transform::GetForwardAxisXM() const {
    return XMMatrixRotationRollPitchYawFromVector(GetRotationXM()).r[2];
}
// 获取世界变换矩阵
XMFLOAT4X4 Transform::GetLocalToWorldMatrix() const {
    XMFLOAT4X4 ans;
    XMStoreFloat4x4(&ans, GetLocalToWorldMatrixXM());
    return ans;
}
// 获取世界变换矩阵
XMMATRIX Transform::GetLocalToWorldMatrixXM() const {
    return XMMatrixScalingFromVector(GetScaleXM()) * XMMatrixRotationRollPitchYawFromVector(GetRotationXM()) * XMMatrixTranslationFromVector(GetPositionXM());
}

// 获取逆世界变换矩阵
XMFLOAT4X4 Transform::GetWorldToLocalMatrix() const {
    XMFLOAT4X4 ans;
    XMStoreFloat4x4(&ans, GetWorldToLocalMatrixXM());
    return ans;
}
// 获取逆世界变换矩阵
XMMATRIX Transform::GetWorldToLocalMatrixXM() const {
    return XMMatrixInverse(nullptr, GetLocalToWorldMatrixXM());
}

// 设置对象缩放比例
void Transform::SetScale(const XMFLOAT3& scale) {
    this->scale = scale;
}
// 设置对象缩放比例
void Transform::SetScale(float x, float y, float z) {
    this->scale = { x,y,z };
}

// 设置对象欧拉角(弧度制)
// 对象将以Z-X-Y轴顺序旋转
void Transform::SetRotation(const XMFLOAT3& eulerAnglesInRadian) {
    rollPitchYaw = eulerAnglesInRadian;
}
// 设置对象欧拉角(弧度制)
// 对象将以Z-X-Y轴顺序旋转
void Transform::SetRotation(float x, float y, float z) {
    rollPitchYaw = { x,y,z };
}

// 设置对象位置
void Transform::SetPosition(const XMFLOAT3& position) {
    pos = position;
}
// 设置对象位置
void Transform::SetPosition(float x, float y, float z) {
    pos = { x,y,z };
}

// 指定欧拉角旋转对象
void Transform::Rotate(const XMFLOAT3& eulerAnglesInRadian) {
    auto xmRollPitchYaw = GetRotationXM();
    xmRollPitchYaw += XMLoadFloat3(&eulerAnglesInRadian);
    XMStoreFloat3(&rollPitchYaw, xmRollPitchYaw);
}
// 指定以原点为中心绕轴旋转
void Transform::RotateAxis(const XMFLOAT3& axis, float radian) {
    auto R = XMMatrixMultiply(XMMatrixRotationRollPitchYawFromVector(GetRotationXM()), XMMatrixRotationAxis(XMLoadFloat3(&axis), radian));
    XMFLOAT4X4 rMat;
    XMStoreFloat4x4(&rMat, R);
    rollPitchYaw = GetEulerAnglesFromRotationMatrix(rMat);
}
// 指定以point为旋转中心绕轴旋转
void Transform::RotateAround(const XMFLOAT3& point, const XMFLOAT3& axis, float radian) {
    auto coreVec = XMLoadFloat3(&point);
    auto posVec = GetPositionXM();
    auto rpyVec = GetRotationXM();

    XMMATRIX rt = XMMatrixRotationRollPitchYawFromVector(rpyVec) * XMMatrixTranslationFromVector(posVec - coreVec);
    rt *= XMMatrixRotationAxis(XMLoadFloat3(&axis), radian);
    rt *= XMMatrixTranslationFromVector(coreVec);
    
}

// 沿着某一方向平移
void Transform::Translate(const XMFLOAT3& direction, float magnitude) {
    XMStoreFloat3(&pos, XMVectorMultiplyAdd(XMVector3Normalize(XMLoadFloat3(&direction)), XMVectorReplicate(magnitude), GetPositionXM()));
}

// 观察某一点
void Transform::LookAt(const XMFLOAT3& target, const XMFLOAT3& up) {
    XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up));
    XMMATRIX invView = XMMatrixInverse(nullptr, view);
    XMFLOAT4X4 rot;
    XMStoreFloat4x4(&rot, invView);
    rollPitchYaw = GetEulerAnglesFromRotationMatrix(rot);
}
// 沿着某一方向观察
void Transform::LookTo(const XMFLOAT3& direction, const XMFLOAT3& up) {
    LookAt(direction, up);
}

XMFLOAT3 Transform::GetEulerAnglesFromRotationMatrix(const XMFLOAT4X4& rotationMatrix) {
    float c = sqrtf(1.f - rotationMatrix(2, 1) * rotationMatrix(2, 1));
    if (isnan(c)) { c = 0.f; }
    XMFLOAT3 rotation;
    rotation.x = atan2f(-rotationMatrix(2, 1), c);
    rotation.y = atan2f(rotationMatrix(2, 0), rotationMatrix(2, 2));
    rotation.z = atan2f(rotationMatrix(0, 1), rotationMatrix(1, 1));
    return rotation;
}