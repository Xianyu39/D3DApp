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
    // ���������λ��
    void SetPosition(float x, float y, float z);
    void SetPosition(const XMFLOAT3& pos);
    // ����������ĳ���
    void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
    void LookTo(const XMFLOAT3& pos, const XMFLOAT3& to, const XMFLOAT3& up);
    // ƽ��
    void Strafe(float d);
    // ֱ��(ƽ���ƶ�)
    void Walk(float d);
    // ǰ��(��ǰ���ƶ�)
    void MoveForward(float d);
    // ���¹۲�
    // ��radֵ���Ϲ۲�
    // ��radֵ���¹۲�
    void Pitch(float rad);
    // ���ҹ۲�
    // ��radֵ����۲�
    // ��radֵ���ҹ۲�
    void RotateY(float rad);
};

#endif // FIRSTPERSONCAMERA_H
