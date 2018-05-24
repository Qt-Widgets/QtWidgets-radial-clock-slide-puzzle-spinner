#include "board.h"
#include "needle.h"

#include <QPainter>
#include <iostream>

/************************************************************************
** Constants
************************************************************************/
// Define the backup label 'Unknown'.
const QString Board::cUnknown("Unknown");

// Define the backup color 'black'.
const QColor Board::cBlack = QColor(Qt::black);

Board::Board(int radius, const tagList &tags)
    : m_innerRadius(0),
      m_outerRadius(radius),
      m_center(Qt::white),
      m_tags(tags)
{
    init();
}

Board::Board(int radius)
    : m_innerRadius(0),
      m_outerRadius(radius),
      m_center(Qt::white)
{
    init();
}

Board::Board(int outerRadius, int innerRadius, const QColor &color)
    : m_innerRadius(innerRadius),
      m_outerRadius(outerRadius),
      m_center(color)
{
    init();
}

Board::Board(int outerRadius, const QColor &color)
    : m_innerRadius(outerRadius/2),
      m_outerRadius(outerRadius),
      m_center(color)
{
    init();
}

void Board::init() {
    setActive(false);
    setEnabled(false);
}

Board::~Board()
{
}

QRectF square(int R)
{
    int D = 2*R;
    return QRectF(-R, -R, D, D);
}

QRectF Board::boundingRect() const {
    return square(outerRadius());
}

const QString &Board::value(qreal a) {
    int angle = int(a) % Needle::cFullCircle;
    QMap<int, int>::iterator it = m_regions.lowerBound(angle);
    if (it == m_regions.end()) {
        it = m_regions.begin();
    }
    return tag(it.value());
}

void Board::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF border = square(outerRadius());

    const QBrush &brush = painter->background();
    const QPen &pen = painter->pen();

    int N = num();
    int slice = int((Needle::cFullCircle + 0.0)/N);
    int startAngle = 90;
    m_regions.clear();

    for(int i = 0; i < N; i++)
    {
        const QColor &c = color(i);
        painter->setPen(c);
        painter->setBrush(c);
        painter->drawPie(border, startAngle*16, slice*16);
        startAngle += slice;
        m_regions.insert(startAngle % Needle::cFullCircle, i);
    }

    if(innerRadius() > 0) {
        painter->setPen(center());
        painter->setBrush(center());
        painter->drawEllipse(square(innerRadius()));

    }
}
