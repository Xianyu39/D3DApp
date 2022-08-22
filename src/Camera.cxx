#include "Camera.h"

using namespace DirectX;

Camera::~Camera(){}

XMVECTOR Camera::GetPositionXM() const {
    return transform.GetPositionXM();
}
XMFLOAT3 Camera::GetPosition() const {
    return transform.GetPosition();
}

XMMATRIX Camera::GetWorldToLocal()const{
    return transform.GetWorldToLocalMatrixXM();
}
XMMATRIX Camera::GetLocalToWorld()const{
    return transform.GetLocalToWorldMatrixXM();
}

//
// ��ȡ�������ת
//

// ��ȡ��X����ת��ŷ���ǻ���
float Camera::GetRotationX() const {
    return transform.GetRotation().x;
}
// ��ȡ��Y����ת��ŷ���ǻ���
float Camera::GetRotationY() const {
    return transform.GetRotation().y;
}

//
// ��ȡ�����������������
//

XMVECTOR Camera::GetRightAxisXM() const {
    return transform.GetRightAxisXM();
}
XMFLOAT3 Camera::GetRightAxis() const {
    return transform.GetRightAxis();
}
XMVECTOR Camera::GetUpAxisXM() const {
    return transform.GetUpAxisXM();
}
XMFLOAT3 Camera::GetUpAxis() const {
    return transform.GetUpAxis();
}
XMVECTOR Camera::GetLookAxisXM() const {
    return transform.GetForwardAxisXM();
}
XMFLOAT3 Camera::GetLookAxis() const {
    return transform.GetForwardAxis();
}

//
// ��ȡ����
//

XMMATRIX Camera::GetViewXM() const {
    return transform.GetWorldToLocalMatrixXM();
}
XMMATRIX Camera::GetProjXM() const {
    return XMMatrixPerspectiveFovLH(fov, aspect, zNear, zFar);
}
XMMATRIX Camera::GetViewProjXM() const {
    return GetViewXM() * GetProjXM();
}

// ��ȡ�ӿ�
D3D11_VIEWPORT Camera::GetViewPort() const {
    return viewport;
}


// ������׶��
void Camera::SetFrustum(float fovY, float aspect, float nearZ, float farZ) {
    fov = fovY;
    this->aspect = aspect;
    this->zNear = nearZ;
    this->zFar = farZ;
}

// �����ӿ�
void Camera::SetViewPort(const D3D11_VIEWPORT& viewPort) {
    this->viewport = viewPort;
}
void Camera::SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth) {
    viewport.Height = height;
    viewport.Width = width;
    viewport.MaxDepth = maxDepth;
    viewport.MinDepth = minDepth;
    viewport.TopLeftX = topLeftX;
    viewport.TopLeftY = topLeftY;
}