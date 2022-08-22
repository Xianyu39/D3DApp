#if !defined(CAMERA_H)
#define CAMERA_H

#include <DirectXMath.h>
#include <d3d11.h>

#include "Transform.h"

using namespace DirectX;

class Camera {
protected:
    Transform transform;
    float zNear = 0.f, zFar = 0.f;
    float aspect;
    float fov;
    D3D11_VIEWPORT viewport;
    
public:
    Camera() = default;
    virtual ~Camera() = 0;
    //
    // ��ȡ�����λ��
    //

    XMVECTOR GetPositionXM() const;
    XMFLOAT3 GetPosition() const;

    //
    // ��ȡ�������ת
    //

    // ��ȡ��X����ת��ŷ���ǻ���
    float GetRotationX() const;
    // ��ȡ��Y����ת��ŷ���ǻ���
    float GetRotationY() const;

    //
    // ��ȡ�����������������
    //

    XMVECTOR GetRightAxisXM() const;
    XMFLOAT3 GetRightAxis() const;
    XMVECTOR GetUpAxisXM() const;
    XMFLOAT3 GetUpAxis() const;
    XMVECTOR GetLookAxisXM() const;
    XMFLOAT3 GetLookAxis() const;

    //
    // ��ȡ����
    //

    XMMATRIX GetViewXM() const;
    XMMATRIX GetProjXM() const;
    XMMATRIX GetViewProjXM() const;

    // ��ȡ�ӿ�
    D3D11_VIEWPORT GetViewPort() const;
    // ��ȡ����
    XMMATRIX GetWorldToLocal()const;
    XMMATRIX GetLocalToWorld()const;


    // ������׶��
    void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

    // �����ӿ�
    void SetViewPort(const D3D11_VIEWPORT& viewPort);
    void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
};

#endif // CAMERA_H
