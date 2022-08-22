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
    // 获取摄像机位置
    //

    XMVECTOR GetPositionXM() const;
    XMFLOAT3 GetPosition() const;

    //
    // 获取摄像机旋转
    //

    // 获取绕X轴旋转的欧拉角弧度
    float GetRotationX() const;
    // 获取绕Y轴旋转的欧拉角弧度
    float GetRotationY() const;

    //
    // 获取摄像机的坐标轴向量
    //

    XMVECTOR GetRightAxisXM() const;
    XMFLOAT3 GetRightAxis() const;
    XMVECTOR GetUpAxisXM() const;
    XMFLOAT3 GetUpAxis() const;
    XMVECTOR GetLookAxisXM() const;
    XMFLOAT3 GetLookAxis() const;

    //
    // 获取矩阵
    //

    XMMATRIX GetViewXM() const;
    XMMATRIX GetProjXM() const;
    XMMATRIX GetViewProjXM() const;

    // 获取视口
    D3D11_VIEWPORT GetViewPort() const;
    // 获取矩阵
    XMMATRIX GetWorldToLocal()const;
    XMMATRIX GetLocalToWorld()const;


    // 设置视锥体
    void SetFrustum(float fovY, float aspect, float nearZ, float farZ);

    // 设置视口
    void SetViewPort(const D3D11_VIEWPORT& viewPort);
    void SetViewPort(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
};

#endif // CAMERA_H
