#include "radialclockwidget.h"

#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <QMouseEvent>
#include <QToolTip>

#include <iostream>
#include <math.h>

#define secondsInMinute 60
#define minutesInHour 60
#define hoursInDay 24
#define daysInWeek 7
#define monthsInYear 12

// Define the strings corresponding to each radial time ring.
const std::string RadialClockWidget::cTimeCodes[InvalidTimeCode] =
{ "Seconds Left In Minute", "Minutes Left In Hour", "Hours Left In Day",
  "Days Left In Week", "Days Left In Month", "Days Left In Year",
  "Months Left In Year" };

// Define the backup color 'black'.
const QColor RadialClockWidget::cBlack = QColor(Qt::black);

QString RadialClockWidget::name() const {
    return "RadialClock";
}

QString RadialClockWidget::includeFile() const {
    return "radialclockwidget.h";
}

QString RadialClockWidget::group() const {
    return tr("Clock Display Widgets");
}

QIcon RadialClockWidget::icon() const {
    return QIcon(":/images/radialclock.png");
}

QString RadialClockWidget::toolTip() const {
    return tr("A radial clock widget");
}

QString RadialClockWidget::whatsThis() const {
    return tr("This widget represents a clock showing current time as a series of concentric shrinking arcs");
}

bool RadialClockWidget::isContainer() const {
    return false;
}

QWidget *RadialClockWidget::createWidget(QWidget *parent) {
    return new RadialClockWidget(parent);
}

void paintRing(QPainter &painter, const QColor &clr, int x, int y, int r, int t, int a)
{
    const QBrush &brush = painter.background();
    const QPen &pen = painter.pen();

    // Draw the outer ring
    int xp = x - r - t;
    int yp = y - r - t;
    int w = 2*(r+t);
    painter.setPen(clr);
    painter.setBrush(clr);
    painter.drawEllipse(xp, yp, w, w);

    // Draw the inner ring
    xp = x - r;
    yp = y - r;
    w = 2*r;
    painter.setPen(brush.color());
    painter.setBrush(brush);
    painter.drawEllipse(xp, yp, w, w);

    // Draw the mask shape
    if (a > 0)
    {
        w = 2*(r + t) + 2;
        QRectF border(x-r-t-1, y-r-t-1, w, w);
        painter.drawPie(border, 90*16, -a*16);
    }

    painter.setPen(pen);
}

void RadialClockWidget::paintRings(QPainter &painter, int x, int y, int thick, int space, int base, const angleVector &angles)
{
    const QBrush &brush = painter.background();
    const QPen &pen = painter.pen();
    regions.clear();

    int N = angles.size();
    int R = (N-1)*thick + (N-1)*space + base;
    for(angleVector::const_reverse_iterator it(angles.rbegin());
        it != angles.rend(); it++)
    {
        regions.insert(std::make_pair(R, it->first));
        regions.insert(std::make_pair(R+thick, it->first));
        paintRing(painter, getColor(it->first), x, y, R, thick, it->second);
        R -= thick + space;
    }

    painter.setBrush(brush);
    painter.setPen(pen);
}

RadialClockWidget::RadialClockWidget(QWidget *parent) :
    QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(250);

    setWindowTitle("Radial Clock");
}

void RadialClockWidget::paintEvent(QPaintEvent *)
{
    QDateTime dtime = QDateTime::currentDateTime();
    QDate date = dtime.date();
    QTime time = dtime.time();
    std::string delim = ((time.second()) % 2 == 0) ? ":" : " ";
    std::string pattern = "hh"+delim+"mm"+delim+"ss";

    QString date_text = date.toString("ddd d, MMM");
    QString time_text = time.toString(pattern.c_str());

    int W = width();
    int H = height();
    int S = std::min(W, H);
    int cx = W/2;
    int cy = H/2;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    angleVector angles;
    int second = time.second();
    bool secondBlip = (time.msec() > 800);
    addAngle(angles, SecondOfMinute, second, secondsInMinute, secondBlip);
    int minute = time.minute();
    bool minuteBlip = (secondBlip && second == (secondsInMinute-1));
    addAngle(angles, MinuteOfHour, minute, minutesInHour, minuteBlip);
    int hour = time.hour();
    bool hourBlip = (minuteBlip && minute == (minutesInHour-1));
    addAngle(angles, HourOfDay, hour, hoursInDay, hourBlip);
    int wday = date.dayOfWeek();
    bool dayBlip = (hourBlip && hour == (hoursInDay-1));
    int ndays = date.daysInMonth();
    addAngle(angles, DayOfWeek, wday, daysInWeek, dayBlip);
    int day = date.day();
    addAngle(angles, DayOfMonth, date.day(), date.daysInMonth(), dayBlip);
    int month = date.month();
    bool monthBlip = (dayBlip && day == ndays);
    addAngle(angles, MonthOfYear, month, monthsInYear, monthBlip);
    addAngle(angles, DayOfYear, date.dayOfYear(), date.daysInYear(), dayBlip);

    int N = angles.size();
    int base = S/100 * 20;
    int space = std::max(1, S/100);
    int thick = (S/2 - base - (N-1)*space)/N;

    paintRings(painter, cx, cy, thick, space, base, angles);

    QRectF textBorder(cx-S/2, cy-S/2, S, S);
    painter.setPen(cBlack);
    painter.drawText(textBorder, Qt::AlignCenter, date_text + QString("\n") + time_text );
}

void RadialClockWidget::mousePressEvent(QMouseEvent *event)
{
    if (regions.empty())
    {
       return;
    }
    //std::cout << "Press: " << event->x() << "," << event->y() << std::endl;
    int W = width();
    int H = height();
    int cx = W/2;
    int cy = H/2;
    int dx = abs(cx - event->x());
    int dy = abs(cy - event->y());
    int dist = int(sqrt(pow(dx,2) + pow(dy,2)));
    std::map<int, TimeCode>::const_iterator it = regions.lower_bound(dist);
    // If inside the smallest ring.
    if (it == regions.begin())
    {
        if (it->second > dist)
        {
            return;
        }
    } else if (it->second != dist) {
        it--;
    }

    // Convert to time code.
    if (it->first > dist)
    {
        return;
    }

    TimeCode tc = it->second;
    it++;
    if (it == regions.end())
    {
        return;
    }

    if (it->second != tc)
    {
        return;
    }
    const std::string &msg = cTimeCodes[tc];
    QToolTip::showText(event->globalPos(), QString(msg.c_str()));
}
