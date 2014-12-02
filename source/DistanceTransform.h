
#pragma once

#include <QSize>
#include <QImage>

// Signed Euclidean Distance Transform

class DistanceTransform
{
public:
    DistanceTransform(
        QImage source
    ,   unsigned short dstWidth
    ,   unsigned short dstHeight
    ,   float distScale = 0.0625f);

    virtual ~DistanceTransform();
    float * data();

protected:

    void sedt(unsigned char threshold);

protected:
    const QImage m_source;

    const QSize m_dstSize;
    const float m_distScale;

    float * m_sedt;
};
