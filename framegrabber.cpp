#include "framegrabber.h"
#include <QRandomGenerator>
#include <QDebug>

QVideoFilterRunnable *FrameGrabber::createFilterRunnable()
{
    return new FilterRunnable(this);
}

FilterRunnable::FilterRunnable(FrameGrabber *frameGrabber)
    : m_frameGrabber(frameGrabber)
{
}

QImage FilterRunnable::imageWrapper(const QVideoFrame &frame){

    if (!frame.isReadable()) {
        qWarning("imageWrapper: No mapped image data available for read");
        return QImage();
    }

    QImage::Format fmt = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat());
    if (fmt != QImage::Format_Invalid)
        return QImage(frame.bits(), frame.width(), frame.height(), fmt);

    qWarning("imageWrapper: No matching QImage format");

    return QImage();
}

QVideoFrame FilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags){
    Q_UNUSED(surfaceFormat);
    Q_UNUSED(flags);

    input->map(QAbstractVideoBuffer::ReadOnly);

    QByteArray * imgBytes = new QByteArray((const char*)input->bits(), input->mappedBytes());

    if(!imgBytes->isEmpty() && !imgBytes->isNull()){
        // pass byte array to XNOR.ai class

    }

    FilterResult *r = new FilterResult;

    // clean last patterns detected
    r->m_bboxes.clear();
    r->m_class_ids.clear();
    r->m_confidence.clear();
    r->m_labels.clear();

    // populate bboxes with new patterns detected
    QRandomGenerator generator;

    generator.seed(++index); // changes seed to change values

    for(int i=0; i<generator.generateDouble()*(10); i++){
        BoundingBox bbox;

        bbox.x = (generator.generateDouble()*(SCREEN_WIDTH - MIN_WIDTH)/2);
        bbox.y = (generator.generateDouble()*(SCREEN_HEIGHT-MIN_HEIGHT)/2);
        bbox.height = (generator.generateDouble()*(SCREEN_HEIGHT-bbox.y-MIN_HEIGHT));
        bbox.width = (generator.generateDouble()*(SCREEN_WIDTH-bbox.x-MIN_WIDTH));
        bbox.confidence = (generator.bounded(100));
        bbox.label = (labels[generator.bounded(5)].toStdString());
        bbox.class_id = ((int)generator.bounded(10));

        r->m_bboxes.append(QRect(bbox.x, bbox.y, bbox.width, bbox.height));
        r->m_class_ids.append(bbox.class_id);
        r->m_confidence.append(bbox.confidence);
        r->m_labels.append(QString::fromStdString(bbox.label));
    }


    emit m_frameGrabber->finished(r);

    // free imgBytes
    delete imgBytes;
    imgBytes = NULL;

    return QVideoFrame();
}

