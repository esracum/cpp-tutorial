# 🛸 Otonom İHA İniş Alanı Tespit Sistemi (C++ & Qt & YOLOv8)

Bu proje, insansız hava araçlarının (İHA) otonom iniş süreçlerini desteklemek amacıyla geliştirilmiş yüksek performanslı bir nesne tespit sistemidir. 
Drone kamerasından alınan simüle edilmiş görüntüler üzerinde **YOLOv8** mimarisi kullanılarak güvenli iniş bölgeleri ve engeller gerçek zamanlı olarak tespit edilmektedir.



## 🛠 Teknik Özellikler

* **Dil:** C++
* **Arayüz Framework:** Qt Creator (Qt 6.x)
* **Görüntü İşleme:** OpenCV
* **Sinir Ağı Modeli:** YOLOv8 (.onnx / .engine)
* **Veri Seti:** Teknofest Otonom İniş Veri Seti

## 🚀 Öne Çıkan Özellikler

* **Düşük Gecikme:** C++ ve OpenCV kullanımı sayesinde yüksek FPS değerlerinde nesne tespiti.
* **Görsel Arayüz (GUI):** Kullanıcının tespitleri anlık olarak izleyebileceği, tespit koordinatlarını ve güven skorlarını görebileceği kapsamlı Qt arayüzü.
* **Otonom Simülasyon:** Gerçek uçuş verilerini simüle eden video akışları üzerinde test edilmiş ve optimize edilmiştir.
* **Hata Toleransı:** Zorlu hava koşulları ve farklı ışık açılarını içeren kapsamlı bir veri seti ile eğitilmiştir.

## Veri Seti Bilgisi

Model, Kaggle üzerinde paylaşılan ve TEKNOFEST yarışmalarına uygun olarak hazırlanan **Autonomous Drone Landing Dataset** kullanılarak eğitilmiştir.
* **Erişim:** [Kaggle - Autonomous Drone Landing Dataset](https://www.kaggle.com/datasets/esracum/autonomous-drone-landing-dataset-teknofest)

## Demo ve Anlatım

Projenin çalışma mantığı, kod yapısı ve uygulama çıktılarını içeren detaylı teknik sunuma aşağıdaki bağlantıdan ulaşabilirsiniz:

* 📺 [YouTube - Proje Detaylı İnceleme](https://youtu.be/gc_lxAZrdpk?si=e0kYey9psDwQnDam)

## Kurulum

1.  **Gereksinimleri Yükleyin:**
    * Qt Creator & Desktop C++ Kit
    * OpenCV 4.10 (C++ kütüphanesi)
    * Model dosyasını (`best.onnx`) projenin klasörüne ekleyin.

2.  **Projeyi Derleyin:**
    ```bash
    git clone [https://github.com/kullaniciadi/proje-adi.git](https://github.com/kullaniciadi/proje-adi.git)
    cd proje-adi
    # Qt Creator ile açıp derleyin.
    ```

3.  **Ek Dosyalar:**
    Projenin kaynak kodlarına ve ek dokümantasyonuna [buradan (Google Drive)](https://drive.google.com/file/d/1i2oqM57JmhO8JOq1JG39bQD7KbZXbuDy/view?usp=sharing) erişebilirsiniz.

