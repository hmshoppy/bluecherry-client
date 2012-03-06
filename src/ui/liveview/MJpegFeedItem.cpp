#include "MJpegFeedItem.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

MJpegFeedItem::MJpegFeedItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    this->setFlag(QGraphicsItem::ItemHasNoContents, false);
}

void MJpegFeedItem::setStream(const QSharedPointer<LiveStream> &stream)
{
    if (stream == m_stream)
        return;

    if (m_stream)
        m_stream->disconnect(this);

    m_stream = stream;

    if (m_stream)
    {
        connect(m_stream.data(), SIGNAL(updated()), SLOT(updateFrame()));
        connect(m_stream.data(), SIGNAL(streamSizeChanged(QSize)), SLOT(updateFrameSize()));
        connect(m_stream.data(), SIGNAL(stateChanged(int)), SLOT(streamStateChanged(int)));
        connect(m_stream.data(), SIGNAL(pausedChanged(bool)), SIGNAL(pausedChanged(bool)));
        m_stream->start();
    }
    else
        emit errorTextChanged(tr("No<br>Video"));

    emit pausedChanged(isPaused());
    emit connectedChanged(isConnected());
    emit intervalChanged(interval());

    updateFrameSize();
    updateFrame();
}

void MJpegFeedItem::clear()
{
    setStream(QSharedPointer<LiveStream>());
}

bool MJpegFeedItem::isPaused() const
{
    return /*m_stream ? m_stream->isPaused() :*/ false;
}

void MJpegFeedItem::setPaused(bool paused)
{
    //if (m_stream)
    //    m_stream->setPaused(paused);
}

bool MJpegFeedItem::isConnected() const
{
    //return m_stream ? (m_stream->state() >= LiveStream::Streaming || m_stream->state() == LiveStream::Paused) : false;
    return true;
}

int MJpegFeedItem::interval() const
{
    return /*m_stream ? m_stream->interval() :*/ 1;
}

int MJpegFeedItem::fps() const
{
    return m_stream ? qRound(m_stream->receivedFps()) : 0;
}

void MJpegFeedItem::setInterval(int interval)
{
    if (m_stream)
    {
        //m_stream->setInterval(interval);
        emit intervalChanged(interval);
    }
}

void MJpegFeedItem::clearInterval()
{
//    m_stream->clearInterval();
    emit intervalChanged(interval());
}

void MJpegFeedItem::paint(QPainter *p, const QStyleOptionGraphicsItem *opt, QWidget *widget)
{
    Q_UNUSED(widget);

    if (!m_stream)
        return;

    QImage frame = m_stream->currentFrame();

    if (!frame.isNull())
    {
        p->save();
        //p->setRenderHint(QPainter::SmoothPixmapTransform);
        p->setCompositionMode(QPainter::CompositionMode_Source);

#if 0//def MJPEGFRAME_IS_PIXMAP
        p->drawPixmap(opt->rect, frame);
#else
        p->drawImage(opt->rect, frame);
#endif

        p->restore();
    }
    else
        p->fillRect(opt->rect, Qt::black);
}

void MJpegFeedItem::streamStateChanged(int state)
{
    Q_ASSERT(m_stream);

    emit connectedChanged(isConnected());

    switch (state)
    {
    case LiveStream::Error:
        emit errorTextChanged(tr("<span style='color:#ff0000;'>Error</span>"));
        //setToolTip(m_stream->errorMessage());
        break;
    case LiveStream::StreamOffline:
        emit errorTextChanged(tr("Offline"));
        break;
    case LiveStream::NotConnected:
        emit errorTextChanged(tr("Disconnected"));
        break;
    case LiveStream::Connecting:
        emit errorTextChanged(tr("Connecting..."));
        break;
    default:
        emit errorTextChanged(QString());
        break;
    }
}
