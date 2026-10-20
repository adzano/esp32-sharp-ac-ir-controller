# Setup ESP32 pada Mac

## Konfigurasi

- Tool firmware: Arduino CLI lokal di `.tools/bin/arduino-cli`.
- Konfigurasi lokal: `arduino-cli.local.yaml` (diabaikan Git); buat dari `arduino-cli.example.yaml`. Path di dalamnya khusus lokasi proyek pada Mac ini.
- Paket dan unduhan disimpan di `.tools/` (diabaikan Git).
- Port terdeteksi: `/dev/cu.usbserial-0001` melalui Silicon Labs CP2102.
- Pengguna telah mengizinkan penggantian program lama pada ESP32.
- Program awal: `firmware/serial_check/serial_check.ino`, baud rate 115200, tanpa Wi-Fi dan tanpa akses GPIO eksternal.

## Perintah dari folder proyek

```sh
.tools/bin/arduino-cli core list --config-file arduino-cli.local.yaml
.tools/bin/arduino-cli compile --config-file arduino-cli.local.yaml --fqbn esp32:esp32:esp32 --build-path build/serial_check firmware/serial_check
.tools/bin/arduino-cli upload --config-file arduino-cli.local.yaml --fqbn esp32:esp32:esp32 --port /dev/cu.usbserial-0001 --input-dir build/serial_check firmware/serial_check
.tools/bin/arduino-cli monitor --config-file arduino-cli.local.yaml --port /dev/cu.usbserial-0001 --config baudrate=115200
```

Target generik `esp32:esp32:esp32` akan diverifikasi melalui identifikasi chip saat komunikasi dengan board. Tutup serial monitor sebelum upload. Nama port dapat berubah saat perangkat/adaptor diganti.

## Referensi

- [Panduan Arduino CLI](https://docs.arduino.cc/arduino-cli/getting-started/)
- [Instalasi paket ESP32 dari Espressif](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html)

## Hasil pengujian

- Arduino CLI: 1.5.2-rc.1, commit fef6e48df (versi yang dikembalikan unduhan resmi `latest` saat setup).
- Core Espressif: esp32:esp32 3.3.12; esptool 5.3.1.
- Target generik esp32:esp32:esp32 cocok dengan hasil deteksi ESP32-D0WD-V3 revisi v3.1, flash 4 MB.
- Build berhasil: 269.716 byte program, 22.116 byte variabel global.
- Upload berhasil, hash data terverifikasi; firmware berjalan setelah reset otomatis.
- Serial pada 115200 baud menampilkan identitas chip dan heartbeat setiap 2 detik. Monitor ditutup setelah pengujian.
- Tidak memasang driver USB tambahan. Akses serial dari lingkungan Codex memerlukan izin di luar sandbox.
- Uji cabut-pasang USB belum dilakukan. Kompatibilitas library IR dengan core ini belum diuji; versi dapat disesuaikan pada tahap IR bila diperlukan.
