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
** Summary: Header for the radial clock widget
**
****************************************************************************/

#ifndef RADIAL_CLOCK_H
#define RADIAL_CLOCK_H

#include <QWidget>
#include <QToolTip>
#include <map>
#include <QtDesigner/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT RadialClock : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool blip READ blip WRITE setBlip);
    Q_PROPERTY(bool seconds READ seconds WRITE setSeconds);
    Q_PROPERTY(bool minutes READ minutes WRITE setMinutes);
    Q_PROPERTY(bool hours READ hours WRITE setHours);
    Q_PROPERTY(bool weekDays READ weekDays WRITE setWeekDays);
    Q_PROPERTY(bool monthDays READ monthDays WRITE setMonthDays);
    Q_PROPERTY(bool yearDays READ yearDays WRITE setYearDays);
    Q_PROPERTY(bool months READ months WRITE setMonths);
    Q_PROPERTY(QColor outerColor READ outerColor WRITE setOuterColor);
    Q_PROPERTY(QColor innerColor READ innerColor WRITE setInnerColor);
    Q_PROPERTY(bool rainbow READ rainbow WRITE setRainbow);

public:
    explicit RadialClock(QWidget *parent = 0);
    ~RadialClock();

    /************************************************************************
    ** Encapsulated Properties
    ** - blip -- Whether to precursor a tick of the clock with a slight back-tick.
    ** - seconds -- Whether to show the seconds counting down
    ** - minutes -- Whether to show the minutes counting down
    ** - hours -- Whether to show the hours counting down
    ** - weekDays -- Whether to show the days of the week counting down
    ** - monthDays -- Whether to show the days of the month counting down
    ** - yearDays -- Whether to show the days of the year counting down
    ** - months -- Whether to show the months of the year counting down
    ** - innerColor -- The color of the inner ring
    ** - outerColor -- The color of the outer ring
    ** Note: all in-between rings will be colored on a gradient between INNER and OUTER
    ** - rainbow -- An override to color the rings based on the colors of the rainbow
    ************************************************************************/
    bool blip() const { return m_blip; }
    void setBlip(bool b) { m_blip = b; }

    bool seconds() const { return m_seconds; }
    void setSeconds(bool s) { m_seconds = s; }

    bool minutes() const { return m_minutes; }
    void setMinutes(bool m) { m_minutes = m; }

    bool hours() const { return m_hours; }
    void setHours(bool h) { m_hours = h; }

    bool weekDays() const { return m_weekDays; }
    void setWeekDays(bool d) { m_weekDays = d; }

    bool monthDays() const { return m_monthDays; }
    void setMonthDays(bool d) { m_monthDays = d; }

    bool yearDays() const { return m_yearDays; }
    void setYearDays(bool d) { m_yearDays = d; }

    bool months() const { return m_months; }
    void setMonths(bool m) { m_months = m; }

    QColor outerColor() const { return m_outer_color; }
    void setOuterColor(const QColor &c) { m_outer_color = c;
                                          m_colors.clear(); }

    QColor innerColor() const { return m_inner_color; }
    void setInnerColor(const QColor &c) { m_inner_color = c;
                                          m_colors.clear(); }

    bool rainbow() const { return m_rainbow; }
    void setRainbow(bool r) { m_rainbow = r; }

    QString describe() const;

    /************************************************************************
    ** TimeCode - An enumerated type to differentiate the different rings
    ** of the clock face.
    ************************************************************************/
    enum TimeCode {
        SecondOfMinute = 0,
        MinuteOfHour = 1,
        HourOfDay = 2,
        DayOfWeek = 3,
        DayOfMonth = 4,
        DayOfYear = 5,
        MonthOfYear = 6,
        InvalidTimeCode = 7
    };
    static const QString cTimeCodes[InvalidTimeCode];

    static bool isDay(const TimeCode &tc);
    static void related(const TimeCode &tc, std::vector<TimeCode> &family);
    static TimeCode relatedTo(const TimeCode &tc);
    int stages() const;
    bool display(const TimeCode &tc) const;
    static int value(const QDateTime &dtime, const TimeCode &tc);
    static int limit(const QDateTime &dtime, const TimeCode &tc);
    static int blipLimit(const QDateTime &dtime, const TimeCode &tc);
    void blips(const QDateTime &dtime, std::map<TimeCode, bool> &blips) const;

    /************************************************************************
    ** Color Processing.
    ************************************************************************/
    static const QColor cBlack;

    void processColors();
    const QColor &getColor(TimeCode tc) const;
    void getColors(const std::vector<QColor> &reference, int s, std::vector<QColor> &colors);

protected:
    /************************************************************************
    ** Widget Callbacks
    ************************************************************************/
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private:
    typedef std::vector<std::pair<TimeCode, int> > angleVector;

    /************************************************************************
    ** Internal Variables.
    ************************************************************************/
    QTimer *m_timer;
    bool m_blip;
    bool m_seconds;
    bool m_minutes;
    bool m_hours;
    bool m_weekDays;
    bool m_monthDays;
    bool m_yearDays;
    bool m_months;
    QColor m_outer_color;
    QColor m_inner_color;
    bool m_rainbow;
    std::map<TimeCode, QColor> m_colors;
    std::map<int, TimeCode> m_regions;

    void addAngle(angleVector &angles, TimeCode tc, int value, int ref, bool blip = false);
    void paintRing(QPainter &painter, const QColor &clr, int x, int y, int r, int t, int a);
    void paintRings(QPainter &painter, int x, int y, int t, int s, int b, const angleVector &angles);

signals:

public slots:

};

#endif // RADIAL_CLOCK_H


