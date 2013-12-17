#pragma once


class QPointF;
class QVector3D;

class Camera;


class FlightNavigation
{
public:
    FlightNavigation(Camera * camera = nullptr);
    virtual ~FlightNavigation();

    void setCamera(Camera * camera);

    void setMouseSpeed(float speed);
    void setMoveSpeed(float speed);

public:
    void mouseMove(const QPointF & mouseDelta);
    void moveLeft();
    void moveRight();
    void moveForward();
    void moveBackward();

protected:
    const QVector3D unproject(const QVector3D & v);
    void move(const QVector3D & direction);

protected:
    Camera * m_camera;

    float m_mouseSpeed;
    float m_moveSpeed;

};
