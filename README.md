# ESP32 Sharp AC IR Controller

Firmware ESP32 untuk mengontrol AC Sharp dari iPhone pada jaringan Wi-Fi lokal, memakai OLED I2C, receiver IR, dan transmitter IR. Perintah yang dikirim merupakan state asli yang direkam dari remote.

## Status saat ini

- Empat preset AC telah terbukti bekerja dari OLED/tombol BOOT dan halaman web lokal.
- ESP32 menyimpan kredensial Wi-Fi di memorinya sendiri, bukan di source code.
- Transmitter saat ini bekerja baik pada jarak dekat, namun arah dan jangkauan masih belum cukup konsisten untuk instalasi permanen.

## Hardware

| Komponen | Koneksi ESP32 |
| --- | --- |
| IR receiver | VCC → 3V3, GND → GND, DATA → GPIO27 |
| IR transmitter | VCC → VIN/5V, GND → GND bersama, DAT → GPIO26 |
| OLED SSD1306 I2C | VCC → 3V3, GND → GND, SDA → GPIO21, SCL → GPIO22 |

Gunakan GND yang sama untuk semua modul. OLED dan receiver tetap memakai 3,3 V. Jangan memasok OLED dari 5 V kecuali level I2C-nya telah dipastikan aman untuk ESP32.

## Firmware utama

[`firmware/ir_web_control_v1/ir_web_control_v1.ino`](firmware/ir_web_control_v1/ir_web_control_v1.ino) menyediakan:

- onboarding Wi-Fi melalui access point `Smarthome-Setup`;
- halaman kontrol lokal untuk empat preset AC;
- menu OLED dan kontrol cadangan dengan tombol BOOT;
- receiver IR untuk memantau remote asli.

Saat belum ada Wi-Fi tersimpan, hubungkan ponsel ke access point tersebut, lalu buka `http://192.168.4.1`. Setelah Wi-Fi rumah tersimpan, alamat IP perangkat muncul pada OLED.

## Menyiapkan Arduino CLI

Salin konfigurasi contoh menjadi konfigurasi lokal, lalu sesuaikan path jika diperlukan:

```sh
cp arduino-cli.example.yaml arduino-cli.local.yaml
```

Lalu pasang core ESP32 dan library yang diperlukan:

```sh
arduino-cli core update-index --config-file arduino-cli.local.yaml
arduino-cli core install esp32:esp32 --config-file arduino-cli.local.yaml
arduino-cli lib install IRremoteESP8266 U8g2 --config-file arduino-cli.local.yaml
```

Contoh build dan upload:

```sh
arduino-cli compile --config-file arduino-cli.local.yaml --fqbn esp32:esp32:esp32 firmware/ir_web_control_v1
arduino-cli upload --config-file arduino-cli.local.yaml --fqbn esp32:esp32:esp32 --port /dev/cu.usbserial-0001 firmware/ir_web_control_v1
```

Nama port USB berbeda pada setiap komputer. Tutup serial monitor sebelum upload.

## Dokumentasi

- [Acuan proyek](STEERING.md)
- [Wiring dan inventaris hardware](docs/hardware.md)
- [Catatan pengujian](docs/test-log.md)
- [Hasil capture remote](docs/ir-captures/)

## Catatan keamanan

Halaman kontrol ini ditujukan untuk jaringan lokal tepercaya. Jangan meneruskan port ESP32 ke internet. Untuk perangkat yang dipasang permanen, ganti password access point setup dan tambahkan autentikasi pada halaman kontrol.
