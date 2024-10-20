#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QTimer>
#include <QProgressBar>
#include <QString>
#include <QtCharts/QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

struct CPU {
    long user;
    long nice;
    long system;
    long idle;
    long iowait;
    long irq;
    long softirq;
    long steal;
    long guest;
    long guest_nice;
};

struct RAM {
    long total;
    long free;
    long available;
    long used;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void cpu_goster();
    void ram_goster();

private:
    Ui::MainWindow *ui;
    CPU getCPU();
    double isle(const CPU &onceki, const CPU &simdiki);
    QChart *chart;
    QLineSeries *series;
    QTimer *timer;
    CPU onceki;
    RAM onceki_ram;
    QProgressBar *pb;
    RAM getRAM();
};

#endif // MAINWINDOW_H
