#ifndef PLOT3D_H
#define PLOT3D_H

#include <QtCore>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtWidgets/QSlider>
#include <QStackedWidget>
#include <QtCharts/QScatterSeries>
#include <QSpinBox>
#include <Solver/vega.h>


namespace Ui {
class Plot3D;
}
using namespace QtDataVisualization;
QT_CHARTS_USE_NAMESPACE

class Plot3D : public QWidget
{
    Q_OBJECT

public:
    explicit Plot3D(QWidget *parent=NULL);
    ~Plot3D();
    bool set3Dlist(QList<QVector3D> &data);

    std::vector<Vega::Osobnik> &pareto();

public Q_SLOTS:
    void toggleMode3D();
    void toggleMode2D();

    void changeTheme(int theme);

    void setBlackToYellowGradient();
    void setGreenToRedGradient();

    void setPointSize(int);
    void setColorMarker();

    bool set2Dlist(std::vector<Vega::Osobnik> *data, float offset = 0.5);

    void clear();

    void show2DPareto();

private:
    Q3DScatter *m_graph;
    QScatterDataProxy *m_proxy;
    QScatter3DSeries *m_series;

    QChart *m_chart;
    QChartView *m_chartView;

    void setAxisXRange(float min, float max);
    void setAxisZRange(float min, float max);

protected:
    virtual void hideEvent(QHideEvent *event);

signals:
    void widgetClosed();

private:
    Ui::Plot3D *ui;
    QStackedWidget *m_stackedWid;
    QStackedWidget *m_stackedOpt;
    QSpinBox *m_pointSize;
    std::vector<Vega::Osobnik> m_dataSeries1;
    std::vector<Vega::Osobnik> m_dataSeries2;
    bool m_firstSeries;

    void paretoFront(std::vector<Vega::Osobnik> &in, std::vector<Vega::Osobnik> &out);
};

#endif
