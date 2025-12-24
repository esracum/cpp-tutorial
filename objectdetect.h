#ifndef OBJECTDETECT_H
#define OBJECTDETECT_H
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QMainWindow>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QElapsedTimer>
#include <QProgressBar>
#include <QProcess>



QT_BEGIN_NAMESPACE
namespace Ui { class ObjectDetect; }
QT_END_NAMESPACE

class ObjectDetect : public QMainWindow
{
    Q_OBJECT

public:
    ObjectDetect(QWidget *parent = nullptr);
    ~ObjectDetect();



private slots:
    void kameraGuncelle();
    void on_btnRecord_clicked();
    void kayitAnimasyonuYap();
    void on_labelKamera_linkActivated(const QString &link);
    void on_snapshot_clicked();
    void on_btnEoir_clicked();


private:
    Ui::ObjectDetect *ui;



    void radarCiz(std::vector<cv::Rect> kutular, std::vector<int> siniflar);
        bool alarmDurumu = false;



    cv::VideoCapture cap;
    cv::Mat frame;
    cv::dnn::Net net;
    QTimer *timer;
    QImage qtImage;
    std::vector<std::string> sinifListesi;

    // Nesne Tanıma Fonksiyonu
    void nesneleriTani(cv::Mat &img);

    // Kayıt Değişkenleri
    cv::VideoWriter videoYazici;
    bool kayitAktifMi = false;
    QTimer *kayitTimer;
    bool animasyonDurumu = false;
    QIcon orjinalKayitIconu;

    // FPS Sayacı
    QElapsedTimer fpsSayaci;

    // Termal Mod
    bool termalModAktif = false;




};
#endif // OBJECTDETECT_H
