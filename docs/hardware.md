# Inventaris hardware dan hasil identifikasi

Tanggal: 18 Oktober 2026. Sumber: dua foto yang diberikan pengguna. Identifikasi visual bukan pengganti spesifikasi listrik atau pemeriksaan board.

## ESP32

- Board hitam dengan modul berpelindung logam, konektor Micro-USB, serta tombol RST dan BOOT.
- Label GPIO terlihat di kedua sisi board.
- Bentuknya menyerupai board ESP32 development dengan 38 pin; nama model/revisi belum dipastikan.
- Tulisan modul dan chip dekat konektor USB belum cukup jelas untuk identifikasi pasti. Jangan menetapkan driver USB berdasarkan foto ini saja.
- Kabel USB terlihat terpasang pada foto, tetapi koneksi ke Mac dan kemampuan transfer data kabel belum dikonfirmasi.
- Pengguna mengizinkan penggantian firmware lama. Program serial_check telah diunggah dan diuji.
- Identifikasi lewat esptool: ESP32-D0WD-V3 revisi v3.1, crystal 40 MHz, flash 4 MB. Output program mengonfirmasi 2 core dan flash 4.194.304 byte. Nama/revisi PCB masih belum dipastikan.

## Modul infrared

## OLED tambahan

OLEDQT 0.969D dengan empat pin GND, VCC, SCL, SDA telah dipasang pada GND bersama dan suplai 3,3 V. Pemindaian I2C mendeteksi satu perangkat di alamat `0x3C` dengan SDA pada GPIO21 dan SCL pada GPIO22. Firmware status memakai driver SSD1306 128×64 sebagai uji awal; tampilan fisik perlu tetap dikonfirmasi pengguna. Jangan memberi VCC OLED 5 V tanpa bukti bahwa jalur SDA/SCL aman untuk level 3,3 V ESP32, karena banyak modul menarik I2C ke tegangan VCC.

Calon penggunaan: status receiver, protokol IR terakhir, dan pengaturan terakhir yang terbaca (bukan konfirmasi kondisi fisik AC).

### Inventaris IR

| Posisi pada foto kedua | Yang terlihat | Yang belum diketahui |
| --- | --- | --- |
| Kiri atas | Tulisan IR Transmitter; LED bening; pin berlabel DAT, VCC, GND dari kiri ke kanan dengan orientasi foto tersebut | Model, tegangan suplai/input, driver, dan kebutuhan carrier |
| Bawah | Tulisan IR Receiver; pin berlabel GND, VCC, DATA dari atas ke bawah dengan orientasi foto tersebut | Model komponen penerima, frekuensi, tegangan suplai/output |
| Kanan atas | Modul kecil dengan komponen berkapsul logam dan lensa gelap; label minus di sisi kiri header dan S di sisi kanan | Tampak seperti receiver IR, tetapi model, fungsi pasti, dan pin tengah perlu konfirmasi |

Receiver untuk uji awal dipilih modul besar berlabel IR Receiver. Spesifikasi listing telah dibaca; identitas chip pada barang fisik tetap mengikuti klaim penjual, belum diverifikasi dari marking komponen.

### Verifikasi tautan produk

- [Transmitter 38 kHz](https://shopee.co.id/product/906610/28863872700): judul halaman terbaca sebagai Digital 38KHz Infrared Transmitter Module 38 KHz, tetapi deskripsi terhalang halaman login. Tegangan, driver, dan input carrier masih belum diverifikasi. Tautan pendek pengguna: https://s.shopee.co.id/5fpcm6G4jm.
- [Receiver TSOP38438](https://shopee.co.id/product/906610/26064025272): penjual menyebut TSOP38438, 38 kHz, suplai modul 3,3–5,5 V, output data biner. Dipasangkan sementara dengan modul besar pada foto berdasarkan label dan bentuk. Tautan pengguna: https://s.shopee.co.id/BUgE0noCV.
- [Receiver KY-022](https://shopee.co.id/product/906610/18346335802): penjual menyebut receiving head 1838, 37,9 kHz dan 2,7–5,5 V. Dipasangkan sementara dengan modul kecil pada foto. Pin tengah tetap perlu verifikasi sebelum modul ini dipakai. Tautan pengguna: https://s.shopee.co.id/Lo6QLPPCl.

Datasheet [Vishay TSOP382/384](https://www.vishay.com/docs/82491/tsop382.pdf) mencakup suplai chip 2,0–5,5 V. Rentang chip berbeda dari spesifikasi modul penjual; untuk pengujian ini gunakan 3,3 V yang termasuk keduanya. Klaim kompatibilitas Sharp pada listing belum membuktikan kompatibilitas remote AC pengguna.

### Wiring receiver untuk pengujian pertama

Cabut USB sebelum merangkai. Gunakan modul besar berlabel IR Receiver, bukan transmitter atau KY-022. Ikuti tulisan pada PCB, bukan urutan kabel berdasarkan orientasi foto.

| Pin receiver | Pin ESP32 |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| DATA | G27 (GPIO27) |

GPIO27 dipilih sebagai input digital berdasarkan [dokumentasi GPIO Espressif](https://documentation.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html). Gunakan suplai 3,3 V agar level sinyal sesuai ESP32; jangan memakai pin 5V untuk rangkaian ini. Pastikan pin berbeda tidak tersambung pada satu strip breadboard. Transmitter dan KY-022 belum dihubungkan.

Status: receiver telah diuji ulang dengan OLED terpasang dan transmitter dilepas. Idle stabil tanpa noise selama dua pemeriksaan 10 detik, lalu satu tombol remote diterima sebagai frame valid 104-bit. Setelah transmitter di-wiring ulang, receiver tetap stabil. Transmitter memakai DAT→GPIO26, GND bersama, dan VCC→VIN/5V ESP32; OLED serta receiver tetap memakai 3,3 V. Pengiriman state Sharp yang direkam berhasil memicu tiga beep pada AC dari jarak hingga sekitar dua meter di depan unit. Firmware `firmware/ir_control_v1` kemudian berhasil mengendalikan empat profil AC melalui OLED dan tombol BOOT. Kontrol dari firmware web juga terbukti bekerja dari jarak dekat; jarak/arah yang lebih longgar masih kadang gagal, sehingga modul transmitter saat ini belum ideal untuk pemasangan permanen.

## Pemeriksaan awal Mac

Pemeriksaan read-only pada 18 Oktober 2026 hanya menemukan:

```text
/dev/cu.Bluetooth-Incoming-Port
/dev/cu.debug-console
```

Belum terlihat port serial yang dapat dikaitkan dengan ESP32. Pemeriksaan informasi USB tidak mengembalikan rincian perangkat pada sesi ini. Hasil tersebut belum membuktikan driver hilang atau board rusak; perlu memastikan sambungan langsung, kabel data, dan membandingkan deteksi sebelum/sesudah board dipasang.

Pada pemeriksaan awal belum dilakukan instalasi driver, pemasangan tool firmware, upload, atau wiring IR. Pembaruan: tool firmware kini sudah dipasang, upload dan serial berhasil; driver tambahan serta wiring IR tidak dilakukan.

## Langkah berikutnya

### Pembaruan setelah USB disambungkan

Mac sekarang mendeteksi `CP2102 USB to UART Bridge Controller` dari `Silicon Labs` (USB VID 0x10C4, PID 0xEA60), dan port `/dev/cu.usbserial-0001` telah muncul. Identitas ini adalah bridge USB-to-serial, bukan identifikasi model ESP32. Kabel/adaptor sudah berhasil membawa koneksi data USB. Tidak perlu memasang driver tambahan pada tahap ini karena port sudah tersedia; komunikasi serial dan upload masih belum diuji.

Langkah koneksi USB dan persiapan firmware sudah selesai, termasuk izin penggantian program lama, build, upload, serta pembacaan serial. Daftar berikut merekam urutan pemeriksaan awal; pekerjaan tersisa adalah uji cabut-pasang USB dan verifikasi spesifikasi IR.

1. Hubungkan ESP32 saja ke Mac menggunakan kabel data; biarkan modul IR belum terhubung.
2. Periksa ulang perangkat USB dan port serial setelah tersambung.
3. Jika belum terdeteksi, periksa kabel/adaptor dan identitas chip USB sebelum menentukan kebutuhan driver.
4. Minta tautan produk/spesifikasi modul IR, atau foto belakang bila tersedia, untuk melengkapi identifikasi sebelum wiring.
5. Pastikan kebutuhan mempertahankan firmware lama sebelum upload pertama.
