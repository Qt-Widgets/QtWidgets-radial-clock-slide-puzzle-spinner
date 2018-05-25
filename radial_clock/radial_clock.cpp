/****************************************************************************
**
** Copyright (C) 2018 Brian Hill
** All rights reserved.
**
** License Agreement
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** Software Author: Brian Hill <brian.hill@glowfish.ca>
**
** Summary: Top class for the radial clock widget
**
****************************************************************************/

#include "radial_clock.h"

#include <QPainter>
#include <QTimer>
#include <QDateTime>
#include <QMouseEvent>
#include <QToolTip>
#include <math.h>

#include <iostream>

/************************************************************************
** A loop for each defined TimeCode value.
************************************************************************/
#define ForEachTimeCode(tc) for(TimeCode tc = TimeCode(0); tc < InvalidTimeCode;tc = TimeCode(tc+1))

/************************************************************************
** Constants
************************************************************************/
const QString RadialClock::cTimeCodes[InvalidTimeCode] =
  // String                    TimeCode
{ "Seconds Left In Minute", // SecondOfMinute
  "Minutes Left In Hour",   // MinuteOfHour
  "Hours Left In Day",      // HourOfDay
  "Days Left In Week",      // DayOfWeek
  "Days Left In Month",     // DayOfMonth
  "Days Left In Year",      // DayOfYear
  "Months Left In Year" };  // MonthOfYear

// Define the backup color 'black'.
const QColor RadialClock::cBlack = QColor(Qt::black);

namespace {
    QString boolToStr(bool v)
    {
        return v ? "true" : "false";
    }
}

/************************************************************************
** Constructor/Destructor
************************************************************************/
RadialClock::RadialClock(QWidget *parent) :
    QWidget(parent),
    m_blip(true),
    m_seconds(true),
    m_minutes(true),
    m_hours(true),
    m_weekDays(true),
    m_monthDays(true),
    m_yearDays(true),
    m_months(true),
    m_rainbow(false)
{
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));
    m_timer->start(250);

    m_outer_color.setRed(255);
    m_inner_color.setBlue(255);
}

RadialClock::~RadialClock()
{
    delete m_timer;
}

QString RadialClock::describe() const
{
    QString props;
    props += "blip: " + boolToStr(blip()) + "\n";
    props += "seconds: " + boolToStr(seconds()) + "\n";
    props += "minutes: " + boolToStr(minutes()) + "\n";
    props += "hours: " + boolToStr(hours()) + "\n";
    props += "weekDays: " + boolToStr(weekDays()) + "\n";
    props += "monthDays: " + boolToStr(monthDays()) + "\n";
    props += "yearDays: " + boolToStr(yearDays()) + "\n";
    props += "months: " + boolToStr(months()) + "\n";
    props += "rainbow: " + boolToStr(rainbow()) + "\n";
    props += "outerColor: " + outerColor().name() + "\n";
    props += "innerColor: " + innerColor().name() + "\n";
    return props;
}

/************************************************************************
** TimeCode related member functions.
************************************************************************/
bool RadialClock::isDay(const TimeCode &tc)
{
    switch(tc) {
        case DayOfWeek:
        case DayOfMonth:
        case DayOfYear:
            return true;
        default:
            return false;
    }
}

void RadialClock::related(const TimeCode &tc, std::vector<TimeCode> &family)
{
    switch(tc) {
        case DayOfWeek:
        case DayOfMonth:
        case DayOfYear:
            family.push_back(DayOfWeek);
            family.push_back(DayOfMonth);
            family.push_back(DayOfYear);
            break;
        default:
            family.push_back(tc);
            break;
    }
}

RadialClock::TimeCode RadialClock::relatedTo(const TimeCode &tc)
{
    switch(tc) {
        case SecondOfMinute:
            return InvalidTimeCode;
            break;
        case MinuteOfHour:
            return SecondOfMinute;
            break;
        case HourOfDay:
            return MinuteOfHour;
            break;
        case DayOfWeek:
            return HourOfDay;
            break;
        case DayOfMonth:
            return HourOfDay;
            break;
        case DayOfYear:
            return HourOfDay;
            break;
        case MonthOfYear:
            return DayOfMonth;
            break;
        default:
            return InvalidTimeCode;
    }
}

int RadialClock::stages() const
{
    int c = 0;
    if (seconds()) c++;
    if (minutes()) c++;
    if (hours()) c++;
    if (weekDays()) c++;
    if (monthDays()) c++;
    if (yearDays()) c++;
    if (months()) c++;
    return c;
}

bool RadialClock::display(const TimeCode &tc) const
{
    switch(tc) {
        case SecondOfMinute:
            return seconds();
            break;
        case MinuteOfHour:
            return minutes();
            break;
        case HourOfDay:
            return hours();
            break;
        case DayOfWeek:
            return weekDays();
            break;
        case DayOfMonth:
            return monthDays();
            break;
        case DayOfYear:
            return yearDays();
            break;
        case MonthOfYear:
            return months();
            break;
        default:
            return false;
    }
}

int RadialClock::value(const QDateTime& dtime, const TimeCode &tc)
{
    switch(tc) {
        case SecondOfMinute:
            return dtime.time().second();
            break;
        case MinuteOfHour:
            return dtime.time().minute();
            break;
        case HourOfDay:
            return dtime.time().hour();
            break;
        case DayOfWeek:
            return dtime.date().dayOfWeek();
            break;
        case DayOfMonth:
            return dtime.date().day();
            break;
        case DayOfYear:
            return dtime.date().dayOfYear();
            break;
        case MonthOfYear:
            return dtime.date().month();
            break;
        default:
            return 0;
            break;
    }
}

int RadialClock::limit(const QDateTime& dtime, const TimeCode &tc)
{
    switch(tc) {
        case SecondOfMinute:
            return 60;
            break;
        case MinuteOfHour:
            return 60;
            break;
        case HourOfDay:
            return 24;
            break;
        case DayOfWeek:
            return 7;
            break;
        case DayOfMonth:
            return dtime.date().daysInMonth();
            break;
        case DayOfYear:
            return dtime.date().daysInYear();
            break;
        case MonthOfYear:
            return 12;
            break;
        default:
            return 0;
            break;
    }
}

int RadialClock::blipLimit(const QDateTime& dtime, const TimeCode &tc)
{
    switch(tc) {
        case SecondOfMinute:
        case MinuteOfHour:
        case HourOfDay:
            return limit(dtime, tc) - 1;
            break;
        default:
            return limit(dtime, tc);
            break;
    }
}

void RadialClock::blips(const QDateTime &dtime, std::map<TimeCode, bool> &blips) const
{
    const QTime &time = dtime.time();
    bool showBlip = (blip() && time.msec() > 800);
    blips.insert(std::make_pair(SecondOfMinute, showBlip));
    ForEachTimeCode(currentTC)
    {
        TimeCode relatedTC = relatedTo(currentTC);
        if (relatedTC == InvalidTimeCode) continue;
        showBlip = (blips[relatedTC] && value(dtime, relatedTC) == (blipLimit(dtime, relatedTC)));
        blips.insert(std::make_pair(currentTC, showBlip));
    }
}

/************************************************************************
** Color Processing.
************************************************************************/

const QColor &RadialClock::getColor(TimeCode tc) const
{
    std::map<TimeCode, QColor>::const_iterator it = m_colors.find(tc);
    return (it == m_colors.end()) ? cBlack : it->second;
}

QColor interpolateColor(const QColor &a, const QColor &b, double factor)
{
    QColor r = a;
    r.setRed(int(r.red() + factor * (b.red() - a.red())+0.5));
    r.setGreen(int(r.green() + factor * (b.green() - a.green())+0.5));
    r.setBlue(int(r.blue() + factor * (b.blue() - a.blue())+0.5));
    return r;
}

void RadialClock::getColors(const std::vector<QColor> &reference, int s, std::vector<QColor> &colors)
{
    size_t refNum = reference.size()-1;
    for(int i = 0; i < s; i++)
    {
        double p = (i + 0.0)/(s-1);
        int j1 = floor(p * refNum);
        int j2 = ceil(p * refNum);
        if (j1 == j2) {
            colors.push_back(reference[j1]);
        } else {
            double j1p = (j1+0.0)/(refNum);
            double j2p = (j2+0.0)/(refNum);
            double delta = j2p - j1p;
            double f = (p - j1p)/delta;
            colors.push_back(interpolateColor(reference[j1], reference[j2], f));
        }
    }
}

void RadialClock::processColors()
{
    if (m_colors.empty() == false) return;
    std::vector<QColor> reference;
    if (rainbow()) {
        reference.push_back(QColor(255,0,0));    // Red
        reference.push_back(QColor(255,127,0));  // Orange
        reference.push_back(QColor(255,255,0));  // Yellow
        reference.push_back(QColor(0,255,0));    // Green
        reference.push_back(QColor(0,0,255));    // Blue
        reference.push_back(QColor(75,0,130));   // Indogo
        reference.push_back(QColor(148,0,211));  // Violet
    } else {
        reference.push_back(innerColor());
        reference.push_back(outerColor());
    }

    std::vector<QColor> colors;
    getColors(reference, stages(), colors);
    int index = 0;
    ForEachTimeCode(tc)
    {
        if (display(tc) == false) {
            continue;
        }
        m_colors.insert(std::make_pair(tc, colors[index++]));
    }
}

void RadialClock::paintEvent(QPaintEvent *)
{
    processColors();
    QDateTime dtime = QDateTime::currentDateTime();
    QDate date = dtime.date();
    QTime time = dtime.time();
    QString delim = ((time.second()) % 2 == 0) ? ":" : " ";
    QString pattern = "hh"+delim+"mm"+delim+"ss";

    QString date_text = date.toString("ddd d, MMM");
    QString time_text = time.toString(pattern);

    int W = width();
    int H = height();
    int S = std::min(W, H);
    int cx = W/2;
    int cy = H/2;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    std::map<TimeCode, bool> useBlips;
    blips(dtime, useBlips);

    angleVector angles;
    ForEachTimeCode(tc)
    {
        if (display(tc) == false) {
            continue;
        }
        addAngle(angles, tc, value(dtime, tc), limit(dtime, tc), useBlips[tc]);
    }

    int N = angles.size();
    int base = S/100 * 20;
    int space = std::max(1, S/100);
    int thick = (S/2 - base - (N-1)*space)/N;

    paintRings(painter, cx, cy, thick, space, base, angles);

    QRectF textBorder(cx-S/2, cy-S/2, S, S);
    painter.setPen(cBlack);
    painter.drawText(textBorder, Qt::AlignCenter, date_text + QString("\n") + time_text );
}

void RadialClock::mousePressEvent(QMouseEvent *event)
{
    if (m_regions.empty()) {
       return;
    }
    int W = width();
    int H = height();
    int cx = W/2;
    int cy = H/2;
    int dx = abs(cx - event->x());
    int dy = abs(cy - event->y());
    int dist = int(sqrt(pow(dx,2) + pow(dy,2)));
    std::map<int, TimeCode>::const_iterator it = m_regions.lower_bound(dist);
    // If inside the smallest ring.
    if (it == m_regions.begin()) {
        if (it->second > dist) {
            return;
        }
    } else if (it->second != dist) {
        it--;
    }

    // Convert to time code.
    if (it->first > dist) {
        return;
    }

    TimeCode tc = it->second;
    it++;
    if (it == m_regions.end()) {
        return;
    }

    if (it->second != tc) {
        return;
    }

    QToolTip::showText(event->globalPos(), cTimeCodes[tc]);
}

void RadialClock::addAngle(angleVector &angles, TimeCode tc, int value, int ref, bool blip)
{
    int angle = int(value/(ref+0.0)*360);
    if (blip && angle > 2) angle -= 2;
    angles.push_back(std::make_pair(tc, angle));
}

void RadialClock::paintRing(QPainter &painter, const QColor &clr, int x, int y, int r, int t, int a)
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
    if (a > 0) {
        w = 2*(r + t) + 2;
        QRectF border(x-r-t-1, y-r-t-1, w, w);
        painter.drawPie(border, 90*16, -a*16);
    }

    painter.setPen(pen);
}

void RadialClock::paintRings(QPainter &painter, int x, int y, int thick, int space, int base, const angleVector &angles)
{
    const QBrush &brush = painter.background();
    const QPen &pen = painter.pen();
    m_regions.clear();

    int N = angles.size();
    int R = (N-1)*thick + (N-1)*space + base;
    for(angleVector::const_reverse_iterator it(angles.rbegin());
        it != angles.rend(); it++)
    {
        m_regions.insert(std::make_pair(R, it->first));
        m_regions.insert(std::make_pair(R+thick, it->first));
        paintRing(painter, getColor(it->first), x, y, R, thick, it->second);
        R -= thick + space;
    }

    painter.setBrush(brush);
    painter.setPen(pen);
}
