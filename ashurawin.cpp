#include "ashurawin.h"
#include "ui_ashurawin.h"
#include <QVBoxLayout>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

AshuraWin *AshuraWin::m_instance=NULL;


AshuraWin::AshuraWin(QWidget *parent) :
    QMainWindow(parent),
    m_inequalitiesPanel(new InequalitiesPanel),
    ui(new Ui::AshuraWin)
{
    ui->setupUi(this);

    m_instance = this;

    QDir::setCurrent(QApplication::applicationDirPath());

    m_plot.setVisible(false);

    ui->inequalitiesArea->setWidget(m_inequalitiesPanel);

    ui->graniceTable->setColumnWidth(0,80);
    ui->graniceTable->setColumnWidth(1,80);
    ui->graniceTable->setColumnWidth(2,80);

    zadanieRefresh();

    connect(ui->Zadania, SIGNAL(visibilityChanged(bool)), this, SLOT(hideZadania(bool)));
    connect(&m_plot, SIGNAL(widgetClosed()), this, SLOT(hidePlot()));
    connect(&m_vega, SIGNAL(animate(std::vector<Vega::Osobnik>*,float)), &m_plot, SLOT(set2Dlist(std::vector<Vega::Osobnik>*,float)));
    connect(&m_vega, SIGNAL(solvingFinished()), this, SLOT(createPareto()));
}

AshuraWin::~AshuraWin()
{
    delete ui;
}

AshuraWin *AshuraWin::getInstance()
{
    return m_instance;
}

void AshuraWin::closeEvent(QCloseEvent *event)
{
    m_plot.close();
}

void AshuraWin::on_solve_clicked()
{
    try {
        m_plot.clear();
        ui->action3Dplot->setChecked(true);
        m_plot.setVisible(true);

        m_vega.setRozmiarPopulacji(ui->dwN->value());
        m_vega.setMaxLiczbaPokolen(ui->dwT->value());
        m_vega.setPrawdKrzyzowania(ui->dwPC->value());
        m_vega.setPrawdMutacji(ui->dwPM->value());

        for (int i=0; i<ui->graniceTable->rowCount(); i++) {
            std::string variable = ui->graniceTable->item(i, 0)->text().toStdString();
            double lower = ui->graniceTable->item(i, 1)->text().toDouble();
            double upper = ui->graniceTable->item(i, 2)->text().toDouble();

            m_vega.setLimit(variable, lower, upper);
        }

        m_vega.setOgraniczenia(m_inequalitiesPanel->createFormula());
//        m_vega.computeOgraniczenia();
        m_vega.createPopulation();

        QStringList sList = ui->equalities->toPlainText().split('\n');
        std::list<std::string> code;
        for (auto &s : sList) code.push_back(s.toStdString());
        m_vega.setFunkcjaCelu(code);

        m_vega.setAnimationTime(ui->animationTime->value());
        ui->solve->setDisabled(true);
        m_vega.start();
    } catch(...) {

    }
}

void AshuraWin::on_equalities_textChanged()
{
    QStringList listaInstrukcji = ui->equalities->toPlainText().split("\n");
    Evaluator parser;
    std::list<std::string> listOfInstruction;

    for (auto s : listaInstrukcji)
        listOfInstruction.push_back(s.toStdString());

    try {
        std::string error = parser.compile(listOfInstruction);
        if (!error.empty()) throw error;
        client::code_gen::Formula::VariableMap vm = parser.getFormula().variables();
        QStringList w_tabeli;

        for (int i=0; i<ui->graniceTable->rowCount(); i++) {
            w_tabeli.push_back(ui->graniceTable->item(i, 0)->text());
        }

        for (auto &s: vm) {
            if (s.first[0]!='f') {
                QList<QTableWidgetItem*> lista = ui->graniceTable->findItems(s.first.c_str(), Qt::MatchExactly);
                if (lista.empty()) {
                    ui->graniceTable->insertRow( ui->graniceTable->rowCount() );
                    ui->graniceTable->setItem( ui->graniceTable->rowCount()-1, 0,
                                             new QTableWidgetItem(s.first.c_str()));
                    ui->graniceTable->setItem( ui->graniceTable->rowCount()-1, 1,
                                             new QTableWidgetItem(QString::number(0)));
                    ui->graniceTable->setItem( ui->graniceTable->rowCount()-1, 2,
                                             new QTableWidgetItem(QString::number(10)));
                }
                w_tabeli.removeAll(s.first.c_str());
            }
        }

        for (auto &s: w_tabeli) {
            QList<QTableWidgetItem*> lista = ui->graniceTable->findItems(s, Qt::MatchExactly);

            for (auto &t: lista) {
                ui->graniceTable->removeRow(t->row());
            }
        }
    } catch (std::string &err) {
//        QMessageBox::information(NULL, "Uwaga", err.c_str());
    }

    m_plot.clear();
}

void AshuraWin::on_action3Dplot_triggered(bool checked)
{
    m_plot.setVisible(checked);
}

void AshuraWin::hidePlot()
{
    ui->action3Dplot->setChecked(false);
}

void AshuraWin::hideZadania(bool b)
{
    ui->actionZadania->setChecked(b);
}

void AshuraWin::createPareto()
{
    std::vector<Vega::Osobnik> & osobniki = m_plot.pareto();

    ui->paretoTable->setColumnCount(osobniki.front().size());
    ui->paretoTable->setRowCount(osobniki.size());

    int itCol=0;
    for (auto &s: osobniki.front()) {
        ui->paretoTable->setHorizontalHeaderItem(itCol++, new QTableWidgetItem(s.first.c_str()));
    }

    int itRow=0;
    for (auto &o: osobniki) {
        int itCol=0;
        for (auto &s: o) {
            ui->paretoTable->setItem(itRow, itCol++, new QTableWidgetItem(QString::number(s.second)));
        }
        itRow++;
    }

    ui->solve->setEnabled(true);
}

QString serialize(QStringList stringList)
{
  QByteArray byteArray;
  QDataStream out(&byteArray, QIODevice::WriteOnly);
  out << stringList;
  return QString(byteArray.toBase64());
}

QStringList deserialize(QString serializedStringList)
{
  QStringList result;
  QByteArray byteArray = QByteArray::fromBase64(serializedStringList.toUtf8());
  QDataStream in(&byteArray, QIODevice::ReadOnly);
  in >> result;
  return result;
}

void AshuraWin::on_actionZapisz_zadanie_triggered()
{
    bool ok;
    QString plik = QInputDialog::getText(this, QStringLiteral("Zapis zadań"),
                                         QStringLiteral("Nazwa zadania:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);

    if (ok && !plik.isEmpty()) {
        plik += ".task";
        QFile scriptFile(plik);
        if( !scriptFile.open(QIODevice::WriteOnly) )
        {
          QString msg = QString( tr("SaveCurrentMacro - Unable to save current script to file: %1") ).arg(plik);
          QMessageBox::critical(this, tr("Macro Editor"), msg, QMessageBox::Ok );
          return;
        }

        QStringList doZapisu;
        QStringList tabela;
        doZapisu << ui->equalities->toPlainText();

        doZapisu << QString::number(ui->dwN->value());
        doZapisu << QString::number(ui->dwPC->value());
        doZapisu << QString::number(ui->dwPM->value());
        doZapisu << QString::number(ui->dwT->value());

        for (quint32 i=0; i < ui->graniceTable->rowCount(); i++) {
            for (quint32 j=0; j < ui->graniceTable->columnCount(); j++) {
                tabela << ui->graniceTable->item(i, j)->text();
            }
        }

        doZapisu << serialize(tabela);

        doZapisu << serialize(m_inequalitiesPanel->serialize());

        QTextStream outputStream(&scriptFile);
        outputStream << serialize(doZapisu);

        scriptFile.close();
    }

    zadanieRefresh();
}

void AshuraWin::openTask(QString name)
{
    if (!name.isEmpty()) {
        name += ".task";
        QFile scriptFile(name);
        if( !scriptFile.open(QIODevice::ReadOnly) )
        {
          QString msg = QString( tr("OpenCurrentMacro - Unable to save current script to file: %1") ).arg(name);
          QMessageBox::critical(this, tr("Macro Editor"), msg, QMessageBox::Ok );
          return;
        }

        QString odczytane = scriptFile.readAll();
        scriptFile.close();

        QStringList doOdczytu = deserialize(odczytane);
        QStringList tabela;

        odczytane = doOdczytu.front();
        doOdczytu.pop_front();
        ui->equalities->setPlainText(odczytane);

        odczytane = doOdczytu.front();
        doOdczytu.pop_front();
        ui->dwN->setValue(odczytane.toInt());
        odczytane = doOdczytu.front();
        doOdczytu.pop_front();
        ui->dwPC->setValue(odczytane.toDouble());
        odczytane = doOdczytu.front();
        doOdczytu.pop_front();
        ui->dwPM->setValue(odczytane.toDouble());
        odczytane = doOdczytu.front();
        doOdczytu.pop_front();
        ui->dwT->setValue(odczytane.toInt());

        tabela = deserialize(doOdczytu.front());
        doOdczytu.pop_front();

        for (quint32 i=0; i < ui->graniceTable->rowCount(); i++) {
            for (quint32 j=0; j < ui->graniceTable->columnCount(); j++) {
                ui->graniceTable->item(i, j)->setText(tabela.front());
                tabela.pop_front();
            }
        }

        tabela = deserialize(doOdczytu.front());
        m_inequalitiesPanel->deserialize(tabela);
        doOdczytu.pop_front();
    }
}

void AshuraWin::zadanieRefresh()
{
    QDir export_folder;
    export_folder.setNameFilters(QStringList()<<"*.task");
    QStringList fileList = export_folder.entryList();

    for (auto &s : fileList) {
        s.truncate(s.lastIndexOf('.'));
    }

    ui->listWidget->clear();
    ui->listWidget->addItems(fileList);
}

void AshuraWin::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    openTask(item->text());
}

void AshuraWin::on_actionAutorzy_triggered()
{
    QMessageBox::information(this, "Autorzy", QStringLiteral("AshuraCode"), QMessageBox::Ok);
}
