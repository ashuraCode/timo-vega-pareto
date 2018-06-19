#include "plot3d.h"
#include "ui_plot3d.h"
#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <QtGui/QImage>
#include <QtCore/qmath.h>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QComboBox>
#include <QScreen>
#include <QPainter>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QSpacerItem>
#include <QColorDialog>


Plot3D::Plot3D(QWidget *parent) :
    QWidget(parent),
    m_firstSeries(true),
    ui(new Ui::Plot3D)
{
    ui->setupUi(this);

    m_graph = new Q3DScatter();

    if (!m_graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        close();
        return;
    }

    m_graph->setSelectionMode(QAbstract3DGraph::SelectionItem);

    m_chart = new QChart;
    m_chartView = new QChartView(m_chart);

    m_stackedWid = new QStackedWidget;
    m_stackedOpt = new QStackedWidget;

    m_stackedWid->addWidget(QWidget::createWindowContainer(m_graph));
    m_stackedWid->addWidget(m_chartView);

    QSize screenSize = m_graph->screen()->size();
    m_stackedWid->setMinimumSize(QSize(screenSize.width() / 2, screenSize.height() / 1.6));
    m_stackedWid->setMaximumSize(screenSize);
    m_stackedWid->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stackedWid->setFocusPolicy(Qt::StrongFocus);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(m_stackedWid, 1);
    hLayout->addLayout(vLayout);
    vLayout->setAlignment(Qt::AlignTop);

    setWindowTitle(QStringLiteral("Plot"));

    QGroupBox *selectionGroupBox = new QGroupBox(QStringLiteral("Typy prezentacji"));

    QRadioButton *modeItemRB = new QRadioButton(this);
    modeItemRB->setText(QStringLiteral("Widok 3D"));
    modeItemRB->setChecked(false);

    QRadioButton *modeSliceRowRB = new QRadioButton(this);
    modeSliceRowRB->setText(QStringLiteral("Widok 2D"));
    modeSliceRowRB->setChecked(false);

    QVBoxLayout *selectionVBox = new QVBoxLayout;
    selectionVBox->addWidget(modeItemRB);
    selectionVBox->addWidget(modeSliceRowRB);
    selectionGroupBox->setLayout(selectionVBox);

    QComboBox *themeList = new QComboBox(this);
    themeList->addItem(QStringLiteral("Qt"));
    themeList->addItem(QStringLiteral("Primary Colors"));
    themeList->addItem(QStringLiteral("Digia"));
    themeList->addItem(QStringLiteral("Stone Moss"));
    themeList->addItem(QStringLiteral("Army Blue"));
    themeList->addItem(QStringLiteral("Retro"));
    themeList->addItem(QStringLiteral("Ebony"));
    themeList->addItem(QStringLiteral("Isabelle"));

    QGroupBox *colorGroupBox = new QGroupBox(QStringLiteral("Kolorystyka"));

    QLinearGradient grBtoY(0, 0, 1, 100);
    grBtoY.setColorAt(1.0, Qt::black);
    grBtoY.setColorAt(0.67, Qt::blue);
    grBtoY.setColorAt(0.33, Qt::red);
    grBtoY.setColorAt(0.0, Qt::yellow);
    QPixmap pm(24, 100);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grBtoY));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientBtoYPB = new QPushButton(this);
    gradientBtoYPB->setIcon(QIcon(pm));
    gradientBtoYPB->setIconSize(QSize(24, 100));

    QLinearGradient grGtoR(0, 0, 1, 100);
    grGtoR.setColorAt(1.0, Qt::darkGreen);
    grGtoR.setColorAt(0.5, Qt::yellow);
    grGtoR.setColorAt(0.2, Qt::red);
    grGtoR.setColorAt(0.0, Qt::darkRed);
    pmp.setBrush(QBrush(grGtoR));
    pmp.drawRect(0, 0, 24, 100);
    QPushButton *gradientGtoRPB = new QPushButton(this);
    gradientGtoRPB->setIcon(QIcon(pm));
    gradientGtoRPB->setIconSize(QSize(24, 100));

    QHBoxLayout *colorHBox = new QHBoxLayout;
    colorHBox->addWidget(gradientBtoYPB);
    colorHBox->addWidget(gradientGtoRPB);
    colorGroupBox->setLayout(colorHBox);

    vLayout->addWidget(selectionGroupBox);

    vLayout->addWidget(m_stackedOpt);

    QVBoxLayout *v1Layout = new QVBoxLayout();
    v1Layout->addWidget(new QLabel(QStringLiteral("Motyw")));
    v1Layout->addWidget(themeList);
    v1Layout->addWidget(colorGroupBox);
    v1Layout->addItem(new QSpacerItem(10, 1000, QSizePolicy::Minimum, QSizePolicy::Maximum));
    QWidget *wid = new QWidget;
    wid->setLayout(v1Layout);
    m_stackedOpt->addWidget(wid);

    QVBoxLayout *v2Layout = new QVBoxLayout();
    m_pointSize = new QSpinBox;
    m_pointSize->setValue(5);
    v2Layout->addWidget(new QLabel(QStringLiteral("Rozmiar")));
    v2Layout->addWidget(m_pointSize);
    QPushButton *sKolor = new QPushButton(QStringLiteral("Kolor"));
    v2Layout->addWidget(sKolor);
    QPushButton *paretoBut = new QPushButton(QStringLiteral("Tylko pareto"));
    v2Layout->addWidget(paretoBut);
    v2Layout->addItem(new QSpacerItem(10, 1000, QSizePolicy::Minimum, QSizePolicy::Maximum));
    wid = new QWidget;
    wid->setLayout(v2Layout);
    m_stackedOpt->addWidget(wid);

    modeItemRB->setChecked(true);
    themeList->setCurrentIndex(2);

    m_proxy = new QScatterDataProxy();
    m_series = new QScatter3DSeries(m_proxy);
    m_series->setItemLabelFormat(QStringLiteral("@xTitle: @xLabel @yTitle: @yLabel @zTitle: @zLabel"));
    m_series->setMeshSmooth(true);
    m_series->setItemSize(0.05);
    m_graph->setTitle("Zbiór rozwiązań Pareto");

    m_graph->addSeries(m_series);
    setBlackToYellowGradient();
    m_chart->setTitle(QStringLiteral("Zbiór wszystkich osobników"));
    m_chart->legend()->hide();

    m_graph->axisX()->setLabelFormat("%.2f");
    m_graph->axisZ()->setLabelFormat("%.2f");
    m_graph->axisX()->setLabelAutoRotation(30);
    m_graph->axisY()->setLabelAutoRotation(90);
    m_graph->axisZ()->setLabelAutoRotation(30);

    QObject::connect(modeItemRB,  SIGNAL(toggled(bool)),
                     this, SLOT(toggleMode3D()));
    QObject::connect(modeSliceRowRB,  SIGNAL(toggled(bool)),
                     this, SLOT(toggleMode2D()));
    QObject::connect(themeList, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(changeTheme(int)));
    QObject::connect(gradientBtoYPB, SIGNAL(pressed()),
                     this, SLOT(setBlackToYellowGradient()));
    QObject::connect(gradientGtoRPB, SIGNAL(pressed()),
                     this, SLOT(setGreenToRedGradient()));
    QObject::connect(m_pointSize, SIGNAL(valueChanged(int)),
                     this, SLOT(setPointSize(int)));
    QObject::connect(sKolor, SIGNAL(clicked()),
                     this, SLOT(setColorMarker()));
    QObject::connect(paretoBut, SIGNAL(clicked()),
                     this, SLOT(show2DPareto()));
}

Plot3D::~Plot3D()
{
    delete m_proxy;
    delete m_series;
    delete m_graph;
    delete m_chart;
    delete m_chartView;
    delete m_stackedWid;
    delete m_stackedOpt;
    delete ui;
}

bool Plot3D::set2Dlist(std::vector<Vega::Osobnik> *data, float offset)
{
    QScatterDataArray *dataArray = new QScatterDataArray;
    QScatterSeries *dataSeries = new QScatterSeries;
    dataArray->reserve(data->size());

    for (int index = 0; index < data->size(); index++) {
        dataArray->push_back(QVector3D((*data)[index]["f1"], (*data)[index]["f2"], offset));
        dataSeries->append((*data)[index]["f1"], (*data)[index]["f2"]);
    }

    if (m_firstSeries) {
        m_dataSeries1.insert(m_dataSeries1.end(), data->begin(), data->end());

        paretoFront(m_dataSeries1, m_dataSeries2);

        m_firstSeries=false;
    } else {
        m_dataSeries2.insert(m_dataSeries2.end(), data->begin(), data->end());

        paretoFront(m_dataSeries2, m_dataSeries1);

        m_firstSeries=true;
    }

    if (m_firstSeries) {
        for (auto &s: m_dataSeries1) {
            dataArray->push_back(QVector3D(s["f1"], s["f2"], offset));
        }
    } else {
        for (auto &s: m_dataSeries2) {
            dataArray->push_back(QVector3D(s["f1"], s["f2"], offset));
        }
    }

    m_proxy->resetArray(dataArray);
    m_chart->addSeries(dataSeries);

    m_chart->createDefaultAxes();
    setPointSize(m_pointSize->value());
}

void Plot3D::clear()
{
    m_proxy->resetArray(NULL);
    m_chart->removeAllSeries();
}

bool Plot3D::set3Dlist(QList<QVector3D> &data)
{

}

std::vector<Vega::Osobnik> &Plot3D::pareto()
{
    if (m_firstSeries)
        return m_dataSeries1;

    return m_dataSeries2;
}

void Plot3D::toggleMode3D()
{
    m_stackedWid->setCurrentIndex(0);
    m_stackedOpt->setCurrentIndex(0);
}

void Plot3D::toggleMode2D()
{
    m_stackedWid->setCurrentIndex(1);
    m_stackedOpt->setCurrentIndex(1);
}

void Plot3D::setAxisXRange(float min, float max)
{
    m_graph->axisX()->setRange(min, max);
}

void Plot3D::setAxisZRange(float min, float max)
{
    m_graph->axisZ()->setRange(min, max);
}

void Plot3D::setPointSize(int ps)
{
    if (!m_chart->series().isEmpty()) {
        for (auto &s : m_chart->series()) {
            ((QScatterSeries*)s)->setMarkerSize(ps);
        }
    }
}

void Plot3D::setColorMarker()
{
    QColor kolor = QColorDialog::getColor();
    if (kolor.isValid()) {
        if (!m_chart->series().isEmpty()) {
            for (auto &s : m_chart->series()) {
                ((QScatterSeries*)s)->setBrush(kolor);
            }
        }
    }
}

void Plot3D::hideEvent(QHideEvent *event)
{
    emit widgetClosed();
    QWidget::hideEvent(event);
}

int compareOsobniki(Vega::Osobnik &o1, Vega::Osobnik &o2) {
    return o1["f1"] < o2["f1"];
}

void Plot3D::paretoFront(std::vector<Vega::Osobnik> &in, std::vector<Vega::Osobnik> &out)
{
    std::sort(in.begin(), in.end(), compareOsobniki);
    out.clear();

    double yVal = m_dataSeries1.front()["f2"];
    out.push_back(in.front());
    for (auto &s: in) {
        if (s["f2"]<yVal) {
            out.push_back(s);
            yVal = s["f2"];
        }
    }
}

void Plot3D::show2DPareto()
{
    QScatterSeries *dataSeries = new QScatterSeries;
    m_chart->removeAllSeries();
    int it=0;
    if (m_firstSeries) {
        for (auto &s: m_dataSeries1) {
            it++;
            dataSeries->append(s["f1"], s["f2"]);
            if (!(it%50)) {
                m_chart->addSeries(dataSeries);
                dataSeries = new QScatterSeries;
            }
        }
    } else {
        for (auto &s: m_dataSeries2) {
            it++;
            dataSeries->append(s["f1"], s["f2"]);
            if (!(it%50)) {
                m_chart->addSeries(dataSeries);
                dataSeries = new QScatterSeries;
            }
        }
    }
    m_chart->addSeries(dataSeries);
    m_chart->createDefaultAxes();
    setPointSize(m_pointSize->value());
}

void Plot3D::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}

void Plot3D::setBlackToYellowGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::black);
    gr.setColorAt(0.33, Qt::blue);
    gr.setColorAt(0.67, Qt::red);
    gr.setColorAt(1.0, Qt::yellow);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void Plot3D::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.5, Qt::yellow);
    gr.setColorAt(0.8, Qt::red);
    gr.setColorAt(1.0, Qt::darkRed);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    m_chart->legend()->setBrush(gr);
}

