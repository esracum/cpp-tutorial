#include "objectdetect.h"
#include "./ui_objectdetect.h"
#include <QDebug>
#include <QDateTime>

ObjectDetect::ObjectDetect(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ObjectDetect)
{
    ui->setupUi(this);

    // --- 1. TASARIM VE TEMALANDIRMA ---
    QString hudStil = "border: 1px solid #33ff33; background-color: #0d1117; color: #33ff33; border-radius: 6px; font-weight: bold; font-family: Consolas;";
    ui->listWidget->setStyleSheet(hudStil);
    ui->lblRadar->setStyleSheet(hudStil);

    QString labelStili = "QLabel { color: #33ff33; font-weight: bold; font-size: 14px; background-color: #000000; padding: 4px; border: 1px solid #333; }";
    QString modeStili  = "QLabel { color: #ffffff; font-weight: bold; font-size: 18px; background-color: #000000; padding: 5px; border: 2px solid #33ff33; border-radius: 5px;}";


    // --- 2. SİSTEM VE KAMERA AYARLARI ---
    fpsSayaci.start();
    orjinalKayitIconu = ui->btnRecord->icon();
    ui->btnRecord->setIconSize(QSize(60, 60));

    // Kamera Çerçevesinin Tasarımı
    ui->labelKamera->setStyleSheet("border: 2px solid #0000ff; background-color: #000000; border-radius: 6px;");

    kayitTimer = new QTimer(this);
    connect(kayitTimer, &QTimer::timeout, this, &ObjectDetect::kayitAnimasyonuYap);
    sinifListesi = {"Vehicle", "UAP", "UAI", "Person"};


   // Gerçek bir drone olsaydı buraya kamera ekleyecektik ama biz şimdilik test için bir video ekledik.
   // Bu video github reposunda yok siz test için bir video ekleyebilirsiniz veya readme.md deki drivedan bu videoyu indirebilirsiniz.
    cap.open("simulasyon_videosu.mp4");
   // Model yolunu veriyoruz.
    std::string modelYolu = "best.onnx";

    try {
        net = cv::dnn::readNetFromONNX(modelYolu);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    } catch (const cv::Exception& e) {
        qDebug() << "Model Yukleme Hatasi:" << e.what();
    }

    if(cap.isOpened()) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ObjectDetect::kameraGuncelle);
        timer->start(30);
    }
}

ObjectDetect::~ObjectDetect()
{
    if(cap.isOpened()) cap.release();
    delete ui;
}

void ObjectDetect::kameraGuncelle()
{
    QDateTime anlikZaman = QDateTime::currentDateTime();
    ui->labelDate->setText(anlikZaman.toString("dd.MM.yyyy HH:mm:ss"));

    qint64 gecenSure = fpsSayaci.elapsed();
    fpsSayaci.restart();
    double fps = (gecenSure > 0) ? 1000.0 / gecenSure : 0.0;
    ui->lcdFPS->display(int(fps));

    cap >> frame;
    if(frame.empty()) {
        cap.set(cv::CAP_PROP_POS_FRAMES, 0);
        return;
    }

    nesneleriTani(frame);
    if (termalModAktif) cv::applyColorMap(frame, frame, cv::COLORMAP_JET);

    if (kayitAktifMi && videoYazici.isOpened()) {
        videoYazici.write(frame);
    }

    cv::Mat displayFrame;
    cv::cvtColor(frame, displayFrame, cv::COLOR_BGR2RGB);
    qtImage = QImage((const unsigned char*) (displayFrame.data), displayFrame.cols, displayFrame.rows, displayFrame.step, QImage::Format_RGB888);
    ui->labelKamera->setPixmap(QPixmap::fromImage(qtImage).scaled(ui->labelKamera->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void ObjectDetect::nesneleriTani(cv::Mat &img) {
    cv::Mat blob;
    cv::dnn::blobFromImage(img, blob, 1.0/255.0, cv::Size(640, 640), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    try {
        net.forward(outputs, net.getUnconnectedOutLayersNames());
    } catch (...) { return; }

    if (outputs.empty()) return;

    cv::Mat &output = outputs[0];
    if (output.dims > 2) {
        output = output.reshape(1, output.size[1]);
        cv::transpose(output, output);
    }

    float *data = (float *)output.data;
    int rows = output.rows;
    int dimensions = output.cols;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    float x_factor = img.cols / 640.0;
    float y_factor = img.rows / 640.0;

    for (int i = 0; i < rows; ++i) {
        float confidence = 0; int class_id_index = -1;
        float *classes_scores = data + 4;
        if (dimensions > 4) {
             cv::Mat scores(1, sinifListesi.size(), CV_32FC1, classes_scores);
             cv::Point class_id; double maxClassScore;
             minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);
             if (maxClassScore > 0.45) { confidence = maxClassScore; class_id_index = class_id.x; }
        }
        if (confidence >= 0.45 && class_id_index != -1) {
            float x = data[0]; float y = data[1]; float w = data[2]; float h = data[3];
            boxes.push_back(cv::Rect(int((x - 0.5 * w) * x_factor), int((y - 0.5 * h) * y_factor), int(w * x_factor), int(h * y_factor)));
            confidences.push_back(confidence);
            class_ids.push_back(class_id_index);
        }
        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, 0.45, 0.45, nms_result);
    std::vector<cv::Scalar> colors = {cv::Scalar(0,255,255), cv::Scalar(0,0,255), cv::Scalar(255,0,0), cv::Scalar(0,255,0)};

    int aI = 0, aA = 0;
    for (int idx : nms_result) {
        cv::Rect box = boxes[idx];
        int clsId = class_ids[idx];
        float conf = confidences[idx];

        if (clsId == 3) aI++; else if (clsId == 0) aA++;
        cv::rectangle(img, box, colors[clsId % 4], 3);

        std::string labelText = sinifListesi[clsId] + " " + std::to_string(int(conf * 100)) + "%";
        cv::putText(img, labelText, cv::Point(box.x, box.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.7, colors[clsId % 4], 2);

        QString logEntry = QString("[%1] %2 | %%3")
                            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                            .arg(QString::fromStdString(sinifListesi[clsId]).leftJustified(8, ' '))
                            .arg(int(conf * 100));

        if(ui->listWidget->count() > 18) delete ui->listWidget->takeItem(0);
        ui->listWidget->addItem(logEntry);
        ui->listWidget->scrollToBottom();
    }
    ui->lcdInsan->display(aI); ui->lcdArac->display(aA);
    radarCiz(boxes, class_ids);
}

// --- KAYIT İŞLEMLERİ ---
void ObjectDetect::on_btnRecord_clicked() {
    if (!kayitAktifMi) {
        if (frame.empty()) return;


        //Buraya video kaydınızın nereye eklenmesini istiyorsanız o yolu vermelisiniz :D
        QString dosyaAdi = "Desktop/CPP/Object_Detect/Kayit_" + QDateTime::currentDateTime().toString("ddMMyy_HHmmss") + ".avi";
        videoYazici.open(dosyaAdi.toStdString(), cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(frame.cols, frame.rows), true);

        if (videoYazici.isOpened()) {
            kayitAktifMi = true;
            kayitTimer->start(500);

            // Kayıt Başladığında Çerçeveyi KIRMIZI Yap
            ui->labelKamera->setStyleSheet("border: 3px solid #ff0000; background-color: #000000; border-radius: 6px;");
            ui->listWidget->addItem(">>> KAYIT BASLATILDI");
        }
    } else {
        kayitAktifMi = false;
        videoYazici.release();
        kayitTimer->stop();

        ui->btnRecord->setStyleSheet("QPushButton { background-color: transparent; border: none; padding: 5px; }");
        ui->btnRecord->setIcon(orjinalKayitIconu);
        ui->btnRecord->setIconSize(QSize(60, 60));

        // [GÜNCELLEME] Kayıt Durduğunda Çerçeveyi MAVİYE Geri Döndür
        ui->labelKamera->setStyleSheet("border: 2px solid #0000ff; background-color: #000000; border-radius: 6px;");
        ui->listWidget->addItem(">>> KAYIT DURDURULDU");
    }
}

void ObjectDetect::kayitAnimasyonuYap() {
    animasyonDurumu = !animasyonDurumu;
    if(kayitAktifMi) {

        ui->btnRecord->setStyleSheet(animasyonDurumu ?
            "background-color: #ff0000; border: 3px solid #ffffff; border-radius: 35px; box-shadow: 0 0 15px #ff0000;" :
            "background-color: #880000; border: 3px solid #ff0000; border-radius: 35px;");
    }
}

void ObjectDetect::on_snapshot_clicked() {
    if (frame.empty()) return;

    //Buraya ekran görüntünüz nereye eklenmesini istiyorsanız o yolu vermelisiniz :D

    QString dosyaAdi = "/Desktop/CPP/Object_Detect/Foto_" + QDateTime::currentDateTime().toString("ddMMyy_HHmmss") + ".jpg";
    if (cv::imwrite(dosyaAdi.toStdString(), frame)) {
        ui->listWidget->addItem(">>> SNAPSHOT ALINDI");
    }
}

// --- RADAR ÇİZİMİ ---
void ObjectDetect::radarCiz(std::vector<cv::Rect> kutular, std::vector<int> siniflar) {
    int rSize = 300;
    cv::Mat radar = cv::Mat::zeros(rSize, rSize, CV_8UC3);
    cv::Point merkez(rSize/2, rSize/2);

    for(int i=0; i<=rSize; i+=50) cv::line(radar, cv::Point(i,0), cv::Point(i, rSize), cv::Scalar(0, 30, 0), 1);
    for(int i=0; i<=rSize; i+=50) cv::line(radar, cv::Point(0,i), cv::Point(rSize, i), cv::Scalar(0, 30, 0), 1);

    static float aci = 0; aci += 2.5; if(aci > 360) aci = 0;
    std::vector<cv::Point> pts; pts.push_back(merkez);
    for(int i=0; i<30; i++) {
        float rad = (aci - i) * CV_PI / 180.0;
        pts.push_back(cv::Point(merkez.x + (rSize/2 - 5) * cos(rad), merkez.y + (rSize/2 - 5) * sin(rad)));
    }
    cv::fillPoly(radar, std::vector<std::vector<cv::Point>>{pts}, cv::Scalar(0, 70, 0));

    for(int r : {int(rSize*0.15), int(rSize*0.3), int(rSize*0.45)}) {
        cv::circle(radar, merkez, r, cv::Scalar(0, 150, 0), 1, cv::LINE_AA);
        cv::putText(radar, std::to_string(r*2)+"m", cv::Point(merkez.x+2, merkez.y-r-2), cv::FONT_HERSHEY_SIMPLEX, 0.35, cv::Scalar(0, 180, 0), 1);
    }
    cv::circle(radar, merkez, rSize/2 - 2, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);

    for(size_t i=0; i<kutular.size(); i++) {
        int rX = (kutular[i].x + kutular[i].width/2 - 320) * (rSize/640.0 * 0.8) + merkez.x;
        int rY = (kutular[i].y + kutular[i].height/2 - 240) * (rSize/480.0 * 0.8) + merkez.y;

        if(cv::norm(cv::Point(rX, rY) - merkez) < (rSize/2 - 10)) {
            cv::Scalar c = (siniflar[i] == 3) ? cv::Scalar(0,0,255) : cv::Scalar(0,255,255);
            cv::circle(radar, cv::Point(rX, rY), 5, c, -1, cv::LINE_AA);
            cv::circle(radar, cv::Point(rX, rY), 8, c, 1, cv::LINE_AA);
        }
    }

    cv::drawMarker(radar, merkez, cv::Scalar(255,255,255), cv::MARKER_CROSS, 20, 2);
    cv::cvtColor(radar, radar, cv::COLOR_BGR2RGB);
    QImage radarImg((const unsigned char*) (radar.data), radar.cols, radar.rows, radar.step, QImage::Format_RGB888);
    ui->lblRadar->setAlignment(Qt::AlignCenter);
    ui->lblRadar->setPixmap(QPixmap::fromImage(radarImg).scaled(ui->lblRadar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ObjectDetect::on_btnEoir_clicked() { termalModAktif = !termalModAktif; }
void ObjectDetect::on_labelKamera_linkActivated(const QString &link) {}
