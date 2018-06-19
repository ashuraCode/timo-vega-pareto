#ifndef ASHURAWIN_H
#define ASHURAWIN_H

#include <QtCore>
#include <QMainWindow>
#include <QListWidgetItem>
#include "Widgets/inequalitiespanel.h"
#include "Solver/vega.h"
#include "Widgets/plot3d.h"


namespace Ui {
class AshuraWin;
}

class AshuraWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit AshuraWin(QWidget *parent = 0);
    ~AshuraWin();

    static AshuraWin *getInstance();

protected:
    virtual void 	closeEvent(QCloseEvent * event);

private slots:
    void on_solve_clicked();
    void on_equalities_textChanged();
    void on_action3Dplot_triggered(bool checked);
    void hidePlot();
    void hideZadania(bool);
    void createPareto();
    void on_actionZapisz_zadanie_triggered();
    void openTask(QString name);
    void zadanieRefresh();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_actionAutorzy_triggered();

private:
    static AshuraWin *m_instance;
    Ui::AshuraWin *ui;
    Vega m_vega;
    InequalitiesPanel *m_inequalitiesPanel;
    Plot3D m_plot;

};

#endif // ASHURAWIN_H
