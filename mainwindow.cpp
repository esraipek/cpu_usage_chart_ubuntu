#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <fstream>
#include <sstream>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChartView>
#include <QTimer>
#include <QProgressBar>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , chart(new QChart())
    , series(new QLineSeries())
    , timer(new QTimer(this))
    , pb(new QProgressBar(this))
{
    ui->setupUi(this);

    // Setup the layout and widgets
    QVBoxLayout* layout = new QVBoxLayout();

    QChartView *chartView = new QChartView(chart, this);
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("CPU Kullanımı");
    chart->axisY()->setRange(0, 100);

    QValueAxis *axisX = new QValueAxis();
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Zaman");
    chart->setAxisX(axisX, series);

    connect(timer, &QTimer::timeout, this, &MainWindow::cpu_goster);
    connect(timer, &QTimer::timeout, this, &MainWindow::ram_goster);
    timer->start(1000);

    layout->addWidget(chartView);
    layout->addWidget(pb);

    // Create a central widget and set the layout
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Initialize CPU and RAM data
    onceki = getCPU();
    onceki_ram = getRAM();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete series;
    delete chart;
    delete timer;
    delete pb;
}

CPU MainWindow::getCPU() {
    ifstream file("/proc/stat");
    std::string satir;
    CPU al;
    if (getline(file, satir)) {
        stringstream ss(satir);
        std::string cpu_adi;
        ss >> cpu_adi;
        ss >> al.guest;
        ss >> al.guest_nice;
        ss >> al.idle;
        ss >> al.iowait;
        ss >> al.irq;
        ss >> al.nice;
        ss >> al.softirq;
        ss >> al.steal;
        ss >> al.system;
        ss >> al.user;
    }
    return al;
}

double MainWindow::isle(const CPU &onceki, const CPU &simdiki) {
    long onceki_bos_olan = onceki.idle + onceki.iowait;
    long gerisi_o = onceki.user + onceki.system + onceki.steal + onceki.nice + onceki.irq + onceki.softirq;
    long topla_o = onceki_bos_olan + gerisi_o;

    long simdiki_bos_olan = simdiki.idle + simdiki.iowait;
    long gerisi_s = simdiki.user + simdiki.system + simdiki.nice + simdiki.softirq + simdiki.irq + simdiki.steal;
    long topla_s = simdiki_bos_olan + gerisi_s;

    long cikar = topla_s - topla_o;
    long cikar_bos = simdiki_bos_olan - onceki_bos_olan;

    return (100 * (cikar - cikar_bos) / cikar);
}

void MainWindow::cpu_goster() {
    CPU cpu_alindi = getCPU();
    double cpu_kullanimi = isle(onceki, cpu_alindi);
    onceki = cpu_alindi;

    series->append(series->count(), cpu_kullanimi);
    if (series->count() > 50) {
        series->remove(0);
    }

    chart->axisX()->setRange(0, series->count());
}

RAM MainWindow::getRAM() {
    ifstream file("/proc/meminfo");
    std::string satir;
    RAM al;

    while (getline(file, satir)) {
        if (satir.find("MemTotal:") == 0) {
            stringstream ss(satir.substr(9));
            ss >> al.total;
        } else if (satir.find("MemFree:") == 0) {
            stringstream ss(satir.substr(8));
            ss >> al.free;
        } else if (satir.find("MemAvailable:") == 0) {
            stringstream ss(satir.substr(13));
            ss >> al.available;
        }
    }

    al.used = al.total - al.free;
    return al;
}

void MainWindow::ram_goster() {
    RAM ram_al = getRAM();
    double yuzde = (100.0 * ram_al.used) / ram_al.total;

    pb->setValue(static_cast<int>(yuzde));
}
