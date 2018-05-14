#ifndef RADIALCLOCKWIDGET_H
#define RADIALCLOCKWIDGET_H

#include "shapeitem.h"

#include <QWidget>
#include <QDesignerCustomWidgetInterface>
#include <QToolTip>
#include <map>

class RadialClockWidget : public QWidget,
                          public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)


public:
    explicit RadialClockWidget(QWidget *parent = 0);

    QString name() const;
    QString includeFile() const;
    QString group() const;
    QIcon icon() const;
    QString toolTip() const;
    QString whatsThis() const;
    bool isContainer() const;
    QWidget *createWidget(QWidget *parent);

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

    static const std::string cTimeCodes[InvalidTimeCode];
    static const QColor cBlack;
    static std::map<TimeCode, QColor> &populate()
    {
        static std::map<TimeCode, QColor> colors;
        colors.insert(std::make_pair(SecondOfMinute, QColor(Qt::red)));
        colors.insert(std::make_pair(MinuteOfHour, QColor(Qt::blue)));
        colors.insert(std::make_pair(HourOfDay, QColor(Qt::green)));
        colors.insert(std::make_pair(DayOfWeek, QColor(Qt::yellow)));
        colors.insert(std::make_pair(DayOfMonth, QColor(Qt::cyan)));
        colors.insert(std::make_pair(DayOfYear, QColor(Qt::magenta)));
        colors.insert(std::make_pair(MonthOfYear, QColor(Qt::black)));
        return colors;
    }

    static const QColor &getColor(TimeCode tc)
    {
        static std::map<TimeCode, QColor> &colors = populate();
        std::map<TimeCode, QColor>::const_iterator it = colors.find(tc);
        return (it == colors.end()) ? cBlack : it->second;
    }

protected:
   void paintEvent(QPaintEvent *);
   void mousePressEvent(QMouseEvent *);

private:
    std::map<int, TimeCode> regions;

    typedef std::vector<std::pair<TimeCode, int> > angleVector;
    void addAngle(angleVector &angles, TimeCode tc, int value, int ref, bool blip = false)
    {
        int angle = int(value/(ref+0.0)*360);
        if (blip && angle > 2) angle -= 2;
        angles.push_back(std::make_pair(tc, angle));
    }

    void paintRings(QPainter &painter, int x, int y, int t, int s, int b, const angleVector &angles);

    void createShapeItem(const QPainterPath &path, const QString &toolTip,
                         const QPoint &point, const QColor &color);
    int itemAt(const QPoint &pos);

    QList<ShapeItem> shapeItems;

signals:

public slots:

};

#endif // RADIALCLOCKWIDGET_H
