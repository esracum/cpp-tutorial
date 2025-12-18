# C++  Rehberi

Bu proje; C++ programlama diline dair teknik notları, en iyi pratikleri ve araç kullanımlarını içeren **sürekli güncellenen ve kapsamı genişleyen** bir başvuru kaynağıdır.


## İçindekiler
1. [G++ ile Temel Derleme](#g-ile-temel-derleme)
2. [Derleme Sürecinin Teorisi](#derleme-sürecinin-teorisi)
3. [Popüler C++ Derleyicileri](#popüler-c-derleyicileri)
4. [Basit Bir Uygulama Örneği](#basit-bir-uygulama-örneği)
5. [CMake ile Proje Yönetimi](#cmake-ile-proje-yönetimi)

---

## G++ ile Temel Derleme

Linux sistemlerde standart olan GCC (GNU Compiler Collection) derleyicisinin C++ modülü olan `g++` için temel komutlar ve kurulum adımları aşağıdadır.

### Kurulum
Debian/Ubuntu tabanlı sistemlerde kurulum için:
```bash
sudo apt-get install g++
```

### Sık Kullanılan Komutlar

Versiyon kontrolü:
```bash
g++ --version
```

Kılavuzu (manual) açmak için:
```bash
man g++
```

Standart bir derleme ve çalıştırma işlemi:
```bash
g++ main.cpp -o program && ./program
```
* `main.cpp`: Derlenecek kaynak dosya.
* `-o program`: Çıktı dosyasının adı (Windows'ta .exe olabilir).
* `&& ./program`: Derleme hatasız tamamlanırsa programı hemen çalıştırır.

Hataları yakalamak için (Tüm uyarıları hata olarak kabul et):
```bash
g++ -Werror main.cpp -o program && ./program
```

Belirli bir C++ standardı (örneğin C++17) ile derlemek:
```bash
g++ -std=c++17 main.cpp
```

---

## Derleme Sürecinin Teorisi

Derleme (Compilation), insan tarafından okunabilir kaynak kodunun (.cpp), bilgisayarın çalıştırabileceği makine koduna dönüştürülmesidir. Bu süreç dört ana aşamadan oluşur:

1.  **Ön İşleme (Preprocessing):**
    * Kodunuzdaki `#include` ve `#define` gibi direktifler işlenir.
    * Kütüphaneler koda dahil edilir ve kaynak kod derlemeye hazır hale getirilir.

2.  **Derleme (Compilation):**
    * Ön işlemden geçen kod, Assembly diline çevrilir.
    * Yazım hataları (syntax errors) bu aşamada tespit edilir.

3.  **Birleştirme (Assembly):**
    * Assembly kodu, makine diline (nesne dosyaları, .o veya .obj) dönüştürülür.

4.  **Bağlama (Linking):**
    * Sizin oluşturduğunuz nesne dosyaları ile C++ standart kütüphaneleri (örn: `std::cout`) birleştirilir.
    * Sonuç olarak tek bir çalıştırılabilir dosya (executable) oluşturulur.

---

## Popüler C++ Derleyicileri

* **GCC (g++):** Linux dünyasında standarttır, açık kaynaklıdır ve çok yaygındır.
* **Clang:** Modern, hızlı ve GCC ile uyumludur. LLVM projesinin bir parçasıdır.
* **MSVC (Microsoft Visual C++):** Windows platformunda Visual Studio ile birlikte gelen derleyicidir.

---

## Basit Bir Uygulama Örneği

Aşağıdaki kodu `merhaba.cpp` adıyla kaydedin:

```cpp
#include <iostream>

int main() {
    // Ekrana "Merhaba, Dünya!" yazdırır
    std::cout << "Merhaba, Dünya!" << std::endl;
    return 0;
}
```

### Derleme ve Çalıştırma

1.  **Derleme:**
    ```bash
    g++ merhaba.cpp -o merhaba
    ```

2.  **Çalıştırma:**
    * Linux/macOS: `./merhaba`
    * Windows: `merhaba.exe`

---

## CMake ile Proje Yönetimi

Tek bir dosyayı `g++` ile derlemek kolaydır, ancak dosya sayısı arttığında yönetim zorlaşır. Bu noktada CMake devreye girer.

### CMake Nedir?
CMake bir derleyici değildir. Derleme sürecini otomatize eden bir **Build System Generator** (İnşa Sistemi Oluşturucu) aracıdır. `CMakeLists.txt` dosyasındaki kuralları okuyarak, işletim sistemine uygun (Makefile, Visual Studio Projesi vb.) derleme dosyalarını üretir.

### Neden Kullanılır?
* **Platform Bağımsızlık:** Aynı kodun Windows, Linux ve macOS üzerinde derlenebilmesini sağlar.
* **Bağımlılık Yönetimi:** OpenCV, Boost gibi harici kütüphaneleri projeye eklemeyi kolaylaştırır.
* **Ölçeklenebilirlik:** Büyük projeleri yönetilebilir parçalara ayırır.

### Proje Yapısı

Örnek bir proje ağacı:

```text
projem/
├── CMakeLists.txt
└── src/
    └── merhaba.cpp
```

### CMakeLists.txt Dosyası
Projenin ana dizininde (`projem/`) oluşturulması gereken yapılandırma dosyası:

```cmake
# CMake'in minimum gerekli sürümü
cmake_minimum_required(VERSION 3.10)

# Proje adı ve versiyonu
project(MerhabaProjesi VERSION 1.0)

# C++ Standartı (C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Çalıştırılabilir dosya oluşturma
# "merhaba": Çıktı dosyasının adı
# "src/merhaba.cpp": Kaynak kodun yolu
add_executable(merhaba src/merhaba.cpp)
```

### CMake ile Derleme Adımları

Temiz bir kurulum için derleme dosyalarını `build` klasöründe tutmak en iyi pratiktir (Out-of-source build).

1.  **Build klasörünü oluşturun ve içine girin:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Yapılandırma (CMake'i çalıştırın):**
    Bu komut, bir üst dizindeki (`..`) `CMakeLists.txt` dosyasını okur.
    ```bash
    cmake ..
    ```

3.  **Derleme (Make'i çalıştırın):**
    CMake tarafından oluşturulan Makefile okunur ve asıl derleme gerçekleşir.
    ```bash
    make
    ```

4.  **Çalıştırma:**
    İşlem başarılıysa `build` klasörü içinde oluşan dosyayı çalıştırabilirsiniz.
    ```bash
    ./merhaba
    ```


