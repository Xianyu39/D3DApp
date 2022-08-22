#include "Transform.h"

using namespace DirectX;

// ��ȡ�������ű���
XMFLOAT3 Transform::GetScale() const{
    return scale;
}
// ��ȡ�������ű���
XMVECTOR Transform::GetScaleXM() const {
    return XMLoadFloat3(&scale);
}
// ��ȡ����ŷ����(������)
// ������Z-X-Y��˳����ת
XMFLOAT3 Transform::GetRotation() const {
    return rollPitchYaw;
}
// ��ȡ����ŷ����(������)
// ������Z-X-Y��˳����ת
XMVECTOR Transform::GetRotationXM() const {
    return XMLoadFloat3(&rollPitchYaw);
}
// ��ȡ����λ��
XMFLOAT3 Transform::GetPosition() const {
    return pos;
}
// ��ȡ����λ��
XMVECTOR Transform::GetPositionXM() const {
    return XMLoadFloat3(&pos);
}
// ��ȡ�ҷ�����
XMFLOAT3 Transform::GetRightAxis() const {
    XMFLOAT3 right;
    XMStoreFloat3(&right, GetRightAxisXM());
    return right;
}
// ��ȡ�ҷ�����
XMVECTOR Transform::GetRightAxisXM() const {
    return XMMatrixRotationRollPitchYawFromVector(GetRotationXM()).r[0];
}
// ��ȡ�Ϸ�����
XMFLOAT3 Transform::GetUpAxis() const {
    XMFLOAT3 up;
    XMStoreFloat3(&up, GetUpAxisXM());
    return up;
}
// ��ȡ�Ϸ�����
XMVECTOR Transform::GetUpAxisXM() const {
    return XMMatrixRotationRollPitchYawFromVector(GetRotationXM()).r[1];
}
// ��ȡǰ������
XMFLOAT3 Transform::GetForwardAxis() const {
    XMFLOAT3 forward;
    XMStoreFloat3(&forward, GetForwardAxisXM());
    return forward;
}
// ��ȡǰ������
XMVECTOR Transform::GetForwardAxisXM() const {
    return XMMatrixRotationRollPitchYawFromVector(GetRotationXM()).r[2];
}
// ��ȡ����任����
XMFLOAT4X4 Transform::GetLocalToWorldMatrix() const {
    XMFLOAT4X4 ans;
    XMStoreFloat4x4(&ans, GetLocalToWorldMatrixXM());
    return ans;
}
// ��ȡ����任����
XMMATRIX Transform::GetLocalToWorldMatrixXM() const {
    return XMMatrixScalingFromVector(GetScaleXM()) * XMMatrixRotationRollPitchYawFromVector(GetRotationXM()) * XMMatrixTranslationFromVector(GetPositionXM());
}

// ��ȡ������任����
XMFLOAT4X4 Transform::GetWorldToLocalMatrix() const {
    XMFLOAT4X4 ans;
    XMStoreFloat4x4(&ans, GetWorldToLocalMatrixXM());
    return ans;
}
// ��ȡ������任����
XMMATRIX Transform::GetWorldToLocalMatrixXM() const {
    return XMMatrixInverse(nullptr, GetLocalToWorldMatrixXM());
}

// ���ö������ű���
void Transform::SetScale(const XMFLOAT3& scale) {
    this->scale = scale;
}
// ���ö������ű���
void Transform::SetScale(float x, float y, float z) {
    this->scale = { x,y,z };
}

// ���ö���ŷ����(������)
// ������Z-X-Y��˳����ת
void Transform::SetRotation(const XMFLOAT3& eulerAnglesInRadian) {
    rollPitchYaw = eulerAnglesInRadian;
}
// ���ö���ŷ����(������)
// ������Z-X-Y��˳����ת
void Transform::SetRotation(float x, float y, float z) {
    rollPitchYaw = { x,y,z };
}

// ���ö���λ��
void Transform::SetPosition(const XMFLOAT3& position) {
    pos = position;
}
// ���ö���λ��
void Transform::SetPosition(float x, float y, float z) {
    pos = { x,y,z };
}

// ָ��ŷ������ת����
void Transform::Rotate(const XMFLOAT3& eulerAnglesInRadian) {
    auto xmRollPitchYaw = GetRotationXM();
    xmRollPitchYaw += XMLoadFloat3(&eulerAnglesInRadian);
    XMStoreFloat3(&rollPitchYaw, xmRollPitchYaw);
}
// ָ����ԭ��Ϊ����������ת
void Transform::RotateAxis(const XMFLOAT3& axis, float radian) {
    auto R = XMMatrixMultiply(XMMatrixRotationRollPitchYawFromVector(GetRotationXM()), XMMatrixRotationAxis(XMLoadFloat3(&axis), radian));
    XMFLOAT4X4 rMat;
    XMStoreFloat4x4(&rMat, R);
    rollPitchYaw = GetEulerAnglesFromRotationMatrix(rMat);
}
// ָ����pointΪ��ת����������ת
void Transform::RotateAround(const XMFLOAT3& point, const XMFLOAT3& axis, float radian) {
    auto coreVec = XMLoadFloat3(&point);
    auto posVec = GetPositionXM();
    auto rpyVec = GetRotationXM();

    XMMATRIX rt = XMMatrixRotationRollPitchYawFromVector(rpyVec) * XMMatrixTranslationFromVector(posVec - coreVec);
    rt *= XMMatrixRotationAxis(XMLoadFloat3(&axis), radian);
    rt *= XMMatrixTranslationFromVector(coreVec);
    
}

// ����ĳһ����ƽ��
void Transform::Translate(const XMFLOAT3& direction, float magnitude) {
    XMStoreFloat3(&pos, XMVectorMultiplyAdd(XMVector3Normalize(XMLoadFloat3(&direction)), XMVectorReplicate(magnitude), GetPositionXM()));
}

// �۲�ĳһ��
void Transform::LookAt(const XMFLOAT3& target, const XMFLOAT3& up) {
    XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up));
    XMMATRIX invView = XMMatrixInverse(nullptr, view);
    XMFLOAT4X4 rot;
    XMStoreFloat4x4(&rot, invView);
    rollPitchYaw = GetEulerAnglesFromRotationMatrix(rot);
}
// ����ĳһ����۲�
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