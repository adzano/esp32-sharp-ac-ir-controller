# Catatan pengujian

## 18 Oktober 2026 — USB, upload, dan serial

Konfigurasi: ESP32-D0WD-V3 revisi v3.1, CP2102, flash 4 MB; port `/dev/cu.usbserial-0001`; Arduino CLI 1.5.2-rc.1; core ESP32 3.3.12; target `esp32:esp32:esp32`. Modul IR belum dihubungkan oleh langkah pengujian ini. Pengguna telah mengizinkan penggantian program lama.

| Skenario | Hasil yang diharapkan | Hasil aktual | Kesimpulan |
| --- | --- | --- | --- |
| Deteksi USB | Bridge dan port tersedia | CP2102, /dev/cu.usbserial-0001 | Lulus |
| Identifikasi chip | ESP32 dan ukuran flash terbaca | ESP32-D0WD-V3 v3.1, 4 MB | Lulus |
| Build serial_check | Kompilasi selesai | 269.716 byte program, 22.116 byte global | Lulus |
| Upload | Firmware tertulis dan diverifikasi | Hash terverifikasi, reset otomatis | Lulus |
| Jalankan program | Identitas chip dan heartbeat terbaca | 2 core, 4.194.304 byte flash, heartbeat tiap 2 detik | Lulus |
| Cabut-pasang USB | Firmware kembali berjalan | Belum dilakukan | Tertunda |
| Receiver/transmitter IR | Sinyal diterima/dikirim | Belum dilakukan | Tahap berikutnya |

Cuplikan output serial:

```text
SMARTHOME_SERIAL_CHECK ready
Chip: ESP32-D0WD-V3 rev 301, cores: 2, flash: 4194304 bytes
SMARTHOME_OK uptime_ms=1012
SMARTHOME_OK uptime_ms=3012
SMARTHOME_OK uptime_ms=5012
SMARTHOME_OK uptime_ms=7012
```

Angka revisi 301 pada API Arduino sesuai dengan v3.1 yang dilaporkan esptool. Keberhasilan ini membuktikan alur firmware dan komunikasi serial; belum membuktikan fungsi IR atau kendali AC.

## Uji awal firmware receiver

Pengguna meminta melanjutkan tanpa pemeriksaan foto tambahan. Wiring VCC→3V3, GND→GND, DATA→GPIO27 mengikuti pemasangan pengguna; jalur fisik belum terverifikasi penuh dari foto.

- IRremoteESP8266 2.9.0 berhasil dipasang dan dibangun dengan core ESP32 3.3.12.
- Firmware `firmware/ir_capture/ir_capture.ino` berhasil diunggah dan hash terverifikasi.
- Ukuran program 446.752 byte; variabel global 22.756 byte.
- Serial menampilkan `IR_CAPTURE_READY gpio=27 baud=115200 buffer=1024 timeout_ms=50` dan `IR_WAITING captures=0 input=1`.
- Input HIGH pada saat idle bukan bukti receiver/wiring berfungsi; menunggu sinyal remote untuk verifikasi.
- Firmware menampilkan protokol, state AC jika didukung, raw timing dalam format source, dan penanda overflow. Belum ada transmisi IR.
- Serial monitor dibiarkan aktif untuk percobaan remote berikutnya; tutup sebelum upload lain.

### Capture pertama berhasil

Pengguna menekan remote satu kali. Capture id=1 pada uptime 73962 ms dikenali sebagai SHARP_AC 104 bit, overflow=0. Decoder menampilkan power On, Cool, 19°C; fan=5 UNKNOWN. State dan raw timing tersimpan di `docs/ir-captures/sharp-001.md`. Penerimaan sinyal pertama terbukti; pencocokan layar remote dan pengujian berulang masih diperlukan. Lampu indikator saja tidak dipakai sebagai dasar kesimpulan.

Pembaruan: foto dan konfirmasi pengguna membuktikan layar remote COOL, 19°C sesuai decode. Uji perubahan suhu dan konsistensi penerimaan masih menunggu langkah berikutnya.

Capture kedua setelah instruksi suhu 20°C: diterima, tetapi UNKNOWN, overflow=0, raw 209 interval. Detail tersimpan di docs/ir-captures/sharp-002.md. Belum boleh menyatakan perubahan suhu terbaca. Langkah berikutnya: ulangi dengan arah/jarak remote stabil; belum mengubah toleransi decoder.

### Pengulangan dan penyesuaian toleransi

- Capture 003: SHARP_AC 104 bit, 19°C/Cool, state identik dengan capture 001, tanpa overflow.
- Capture 004: UNKNOWN, raw 211 interval lengkap, tanpa overflow. Raw tersimpan bersama capture 003 di docs/ir-captures/.
- Analisis diagnostik offline terhadap capture 004 (bit dipisahkan berdasarkan space >1000 µs, LSB-first) menghasilkan AA5ACF10C5315200088004E0A1. Checksum dihitung 0xA, sama dengan nibble checksum yang diterima. Ini bukan hasil decode firmware dan belum diperlakukan sebagai validasi suhu end-to-end.
- Mark capture 004 berkisar 374–454 µs. Library memakai nominal 470 µs, mark excess 50 µs, toleransi default 25%. Pulsa pendek diduga menjadi penyebab penolakan; penyebab distorsi fisik belum dipastikan.
- Toleransi firmware dinaikkan menjadi 35% untuk uji berikutnya; validasi checksum protokol tetap aktif. Build berhasil (446.804 byte). Menunggu uji ulang remote untuk membuktikan hasil perubahan ini.

### Hasil uji toleransi 35%

Upload dan startup firmware berhasil. Dua capture berikutnya dikenali sebagai SHARP_AC 104 bit tanpa overflow: 19°C pada uptime 36719 ms (state AA5ACF10C4315200088004E0B1), lalu 20°C pada uptime 40779 ms (AA5ACF10C5315200088004E0A1). Keduanya On/Cool; fan masih 5 UNKNOWN. Raw dan state disimpan sebagai sharp-005.txt dan sharp-006.txt (ID serial kembali ke 1 dan 2 setelah reboot).

Perubahan suhu 19→20°C berhasil dibaca pada percobaan ini. Belum dapat memisahkan pengaruh toleransi dari variasi kualitas sinyal; perlu pengulangan. Berikutnya ulangi pasangan 19→20°C dua kali dengan jeda 2 detik antar tombol, tanpa perubahan mode/fan.

### Konsistensi suhu terverifikasi pada setup saat ini

### Pemetaan fan pertama

Setelah satu tekanan tombol fan, foto menunjukkan COOL 20°C dan empat batang indikator kipas. Capture serial ID 7 (uptime 188827 ms) dikenali SHARP_AC 104 bit tanpa overflow, fan=7 High, state AA5ACF10C5317200088005E091. Sebelumnya fan=5 UNKNOWN dengan foto awal tampak tiga batang. Pemetaan sementara: empat batang → raw fan 7 (High); raw fan 5 belum diberi nama kecepatan. Kecepatan fisik kipas AC belum diverifikasi. Capture lengkap: sharp-011.txt. Langkah berikutnya: satu tekanan fan lagi dan catat indikator layar untuk memetakan urutan.

### Fan AUTO terpetakan

Pengguna mengatur remote kembali ke fan AUTO pada COOL 20°C. Karena tombol ditekan beberapa kali cepat, lima frame awal pada serial monitor tidak lolos decode dan tidak digunakan untuk pemetaan. Frame akhir (serial ID 6, uptime 47962 ms) bersih: SHARP_AC 104 bit, overflow=0, fan=2 (Auto), state AA5ACF10C5312200088005E0C1. Foto layar remote mengonfirmasi AUTO. Capture lengkap: sharp-012.txt.

Pemetaan valid saat ini: AUTO → raw fan 2; empat batang → raw fan 7 (High). Tiga batang awal tetap raw fan 5 tanpa nama kecepatan yang terverifikasi.

### Mode DRY terpetakan

Setelah satu tekanan MODE dari COOL/AUTO, foto remote menunjukkan DRY dan fan AUTO. Capture valid SHARP_AC 104 bit tanpa overflow menunjukkan mode=3 (Dry), fan=2 (Auto), temperature state=15°C, state AA5ACF1000312300088000E011. Layar DRY tidak menampilkan suhu; jangan menganggap nilai state 15°C sebagai setpoint yang terlihat. Detail: sharp-013.md.

### Tampilan FAN terpetakan dengan batas decoder

Setelah MODE lagi, foto remote menunjukkan FAN dengan dua batang kipas. Frame SHARP_AC 104 bit tanpa overflow memiliki state AA5ACF1003313400088000E041. Library menafsirkan state ini sebagai mode 0 Auto, 18°C, fan 3 Medium. Karena tampilan remote pasti FAN, ketidakcocokan itu dicatat sebagai batas interpretasi library untuk profile ini; label layar menjadi sumber kebenaran. Detail: sharp-014.md.

### Mode AUTO terpetakan

Setelah MODE lagi, foto remote menunjukkan AUTO dan fan AUTO. Frame SHARP_AC 104 bit tanpa overflow sesuai dengan interpretasi decoder: mode=0 Auto, fan=2 Auto, temperature state=15°C, state AA5ACF1000312000088000E021. Layar tidak menampilkan suhu pada mode AUTO. Detail: sharp-015.md.

### Posisi louver vertikal pertama terpetakan

Saat tombol arah hembusan ditekan sekali dari AUTO/fan AUTO, frame SHARP_AC 104 bit tanpa overflow berubah menjadi Swing(V)=1 High, state AA5ACF1000312000098006E051. Foto mendukung adanya indikator arah hembusan. Posisi High sudah terpetakan; swing otomatis dan posisi lain belum diuji. Detail: sharp-016.md.

### Louver berhenti pada posisi terakhir

Tekanan tombol arah hembusan berikutnya menghasilkan SHARP_AC 104 bit tanpa overflow dengan Swing(V)=2 Off, state AA5ACF10003120000A8006E061. Definisi protokol library menjelaskan nilai Off sebagai menghentikan gerak dan kembali ke posisi tetap terakhir. Indikator layar masih mirip, sehingga state IR menjadi pembeda utama. Detail: sharp-017.md.

### Posisi louver vertikal Middle

Tekanan tombol arah hembusan ketiga menghasilkan SHARP_AC 104 bit tanpa overflow, Swing(V)=3 Middle, state AA5ACF10003120000B8006E071. Detail: sharp-018.md.

### Posisi louver vertikal Low

Tekanan tombol arah hembusan keempat menghasilkan SHARP_AC 104 bit tanpa overflow, Swing(V)=4 Low, state AA5ACF10003120000C8006E001. Pengguna mengamati arah hembusan bergerak sedikit lebih ke bawah. Detail: sharp-019.md.

### Posisi louver 5

Tekanan tombol arah hembusan kelima menghasilkan SHARP_AC 104 bit tanpa overflow, Swing(V)=5 (decoder UNKNOWN), state AA5ACF10003120000D8006E011. Pengguna mengamati arah hembusan bergerak makin ke bawah. Nilai tersebut dicatat sebagai posisi louver 5, lebih rendah dari Low, tanpa menebak nama resmi. Detail: sharp-020.md.

### Toggle ayun vertikal

Tekanan tombol arah hembusan keenam menghasilkan SHARP_AC 104 bit tanpa overflow, raw Swing(V)=7, state AA5ACF10003120000F8006E031. Output decoder menyebut Highest, tetapi konstanta protokol library menyebut raw value 7 sebagai perintah toggle ayun konstan on/off. Foto ikon tiga garis selaras dengan perintah tersebut. Ini perintah toggle, bukan state yang bisa dianggap konfirmasi ayun sedang aktif. Detail: sharp-021.md.

### JET / Turbo on terpetakan

Saat tombol JET ditekan dan ikon JET muncul di layar, frame SHARP_AC 104 bit tanpa overflow menampilkan Turbo On, state AA5ACF1000612000088001E061. Frame khusus ini menampilkan Power `-`, artinya field power tidak diubah, bukan AC mati. Detail: sharp-022.md.

### JET / Turbo off terpetakan

Saat tombol JET ditekan kembali, frame valid SHARP_AC 104 bit tanpa overflow adalah AA5ACF10007120000F8001E001. Decoder membaca Turbo Off. Indikator ayun vertikal tetap terlihat pada layar remote, sementara nilai ayun mentah `7` adalah perintah toggle ayun pada protokol Sharp. Karena ayun sudah diaktifkan sebelum JET diuji, hasil ini belum membuktikan bahwa JET mengubah ayun. Detail: sharp-023-jet-off.md.

### Sleep aktif terpetakan

Satu tekanan tombol SLEEP menghasilkan frame SHARP_AC 104 bit tanpa overflow: AA5ACF10006120000F8010E011. Foto remote memperlihatkan ikon bulan aktif. Decoder library tidak menamai Sleep untuk profil ini, tetapi byte perintah khusus `0x10` dan konfirmasi layar remote cukup untuk mencatat frame sebagai Sleep On. Durasi serta perilaku Sleep belum diuji. Detail: sharp-024-sleep-on.md.

### Sleep dimatikan

Tekanan SLEEP kedua diterima tanpa overflow sebagai AA5ACF10007120000F8010E001. Pengguna mengonfirmasi ikon bulan hilang, sehingga frame dipetakan sebagai Sleep Off. Detail: sharp-025-sleep-second-press.md.

### DISPLAY terpetakan sebagai toggle unit indoor

Tekanan DISPLAY menghasilkan SHARP_AC 104 bit tanpa overflow: AA5ACF10006120000F8004E041. LCD remote tidak berubah, sebagaimana wajar bila perintah ditujukan ke tampilan/lampu unit indoor. Library melabelinya `Econo: Toggle`, tetapi label tombol fisik menjadi sumber kebenaran untuk remote ini: DISPLAY toggle. Respons fisik AC belum diuji. Detail: sharp-026-display-toggle.md.

### CANCEL timer terpetakan

Tekanan CANCEL menghasilkan SHARP_AC 104 bit tanpa overflow: AA5ACF10008120000F80C0E021. Byte perintah `0xC0` termasuk kategori timer pada protokol Sharp. Decoder tidak menunjukkan timer aktif, selaras dengan fungsi pembatalan. LCD remote tidak berubah karena tidak ada timer yang terlihat aktif. Detail: sharp-027-cancel-timer.md.

### ON timer 5 jam terpetakan

Tekanan tombol ON timer menghasilkan SHARP_AC 104 bit tanpa overflow: AA5ACF10008120C50F80C0E0B1. Foto remote menampilkan ON 5,0 h dan decoder membaca On Timer 05:00. Timer kini tersetel; perlu dibatalkan dengan CANCEL setelah pengujian. Detail: sharp-028-on-timer-5h.md.

### Pembatalan ON timer terverifikasi

Saat ON timer 05:00 aktif, CANCEL menghasilkan SHARP_AC 104 bit tanpa overflow: AA5ACF10008120400F80C0E061. Decoder tidak lagi menunjukkan timer dan pengguna mengonfirmasi layar remote kembali kosong. Pembatalan pada remote terverifikasi; efek unit AC belum diuji. Detail: sharp-027-cancel-timer.md.

### OFF timer 8 jam 30 menit terpetakan

Tekanan tombol OFF timer menghasilkan SHARP_AC 104 bit tanpa overflow: AA5ACF10008120880F80DEE0D1. Foto remote menunjukkan OFF 8,5 h dan decoder membaca Off Timer 08:30. Timer masih aktif dan perlu dibatalkan dengan CANCEL setelah pengujian. Detail: sharp-029-off-timer-8h30.md.

### Pembatalan OFF timer terverifikasi

Setelah CANCEL, frame kembali menjadi AA5ACF10008120000F80C0E021 tanpa overflow dan decoder tidak lagi menampilkan Off Timer. OFF timer pada remote berhasil dibatalkan. Detail: sharp-029-off-timer-8h30.md.

## Integrasi OLED dan transmitter

- OLED pada GPIO21/GPIO22 terdeteksi sebagai satu perangkat I2C di alamat `0x3C` dengan suplai 3,3 V dan GND bersama.
- Firmware `firmware/oled_status` berhasil dibangun dan diunggah. Firmware ini menginisialisasi transmitter pada GPIO26, tetapi tidak pernah memancarkan IR.
- Dengan transmitter terpasang, receiver menghasilkan frame UNKNOWN yang overflow secara berulang. Setelah tiga kabel transmitter dilepas, receiver idle stabil selama dua pemeriksaan 10 detik tanpa noise.
- Satu tombol remote setelah isolasi transmitter diterima sebagai frame valid Sharp AC 104-bit. Receiver dan OLED dapat digunakan bersamaan pada setup saat ini; transmitter menunggu pemeriksaan wiring/pinout lebih lanjut.

### Pengiriman IR ke AC berhasil

- Setelah wiring transmitter diperbaiki, DAT tetap pada GPIO26, GND disatukan, dan VCC transmitter dipindahkan ke VIN/5V ESP32. OLED serta receiver tetap memakai 3,3 V.
- Firmware `firmware/ir_tx_button_test` mengirim tiga salinan frame Sharp yang direkam dari remote: Power On, Cool, 19°C, hanya saat tombol BOOT ditekan.
- Dengan transmitter diarahkan ke penerima IR AC, unit berbunyi tiga kali. Pengguna juga berhasil memicu respons dari jarak hingga sekitar dua meter di depan unit.
- Respons beep membuktikan penerimaan perintah oleh AC. Tampilan atau kondisi fisik akhir AC belum dikonfirmasi pada pengujian ini.

### Kontrol OLED/BOOT v1 berhasil

- Firmware `firmware/ir_control_v1` menampilkan menu pada OLED dan memakai tombol BOOT: tekan singkat untuk memilih profil, tahan sekitar satu detik lalu lepas untuk mengirim.
- Empat profil yang berasal dari capture remote asli tersedia: Cool 19°C; Cool 20°C; Cool 20°C dengan fan Auto; serta Cool 20°C dengan fan High.
- Pengguna mengonfirmasi semua profil berhasil mengendalikan AC. Setiap perintah memakai tiga salinan frame untuk keandalan jangkauan.
- Power-off dan fitur lain yang belum direkam tidak disediakan pada firmware ini.

### Kontrol web lokal v1 berhasil

- Firmware `firmware/ir_web_control_v1` berhasil terhubung ke Wi-Fi rumah dan menyediakan halaman kontrol dari iPhone pada jaringan lokal.
- Keempat profil AC dapat dipilih dari halaman tersebut. Pada uji jarak dekat, seluruh perintah diterima AC.
- Jika transmitter digeser atau ditempatkan lebih jauh, penerimaan AC masih sesekali tidak konsisten. Ini membatasi keandalan pada kekuatan dan arah pancaran modul transmitter saat ini, bukan koneksi Wi-Fi atau logika kontrol.

### Kontrol MQTT v1 berhasil

- Backend, broker MQTT lokal, dan ESP32 terhubung. Keempat preset yang sama dengan Web UI telah diterbitkan melalui MQTT dan diterima firmware ESP32.
- Pengguna mengonfirmasi LED transmitter menyala tiga kali setiap tombol Web UI ditekan untuk seluruh empat preset.
- Respons fisik AC belum diuji pada tahap ini karena penggunaan listrik sedang tinggi. Hasil ini memverifikasi pengiriman command sampai transmitter, bukan penerimaan IR oleh unit AC.

Empat capture berikutnya (serial ID 3–6) semuanya SHARP_AC 104 bit tanpa overflow, urutan 19→20→19→20°C tepat. State untuk tiap suhu identik dengan capture sebelumnya. Dengan pasangan sebelumnya, total enam dari enam capture setelah penyesuaian berhasil, masing-masing tiga sampel per suhu. Raw tersimpan sebagai sharp-007.txt sampai sharp-010.txt. Ini memvalidasi penerimaan suhu pada posisi uji saat ini, bukan seluruh fitur/jangkauan. Fan tetap UNKNOWN; pengiriman ke AC belum diuji.
