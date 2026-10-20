# Steering — DIY Smart Home IoT

Tanggal acuan: 18 Oktober 2026
Status: setup ESP32 pada Mac serta uji build, upload, dan serial berhasil. Pengujian IR belum dimulai; uji cabut-pasang USB masih perlu dilakukan.

## 1. Tujuan dan fungsi dokumen

Membangun sistem smart home pribadi untuk mengendalikan AC Sharp dan BARDI Smart LED melalui satu dashboard/PWA di iPhone. Target akhirnya mencakup kontrol melalui jaringan lokal dan internet, jadwal, serta automasi yang dapat diperluas.

Proyek ini sekaligus menjadi sarana belajar ESP32, komunikasi infrared, firmware, MQTT, integrasi perangkat, dan aplikasi IoT. Pengembangan dilakukan bertahap dengan hasil yang dapat diuji pada setiap tahap.

Dokumen ini adalah acuan ruang lingkup, keputusan, urutan kerja, dan kriteria keberhasilan. Dasarnya adalah rangkuman diskusi sebelumnya serta pembaruan bahwa IR receiver dan IR transmitter sudah tiba. Rencana teknis yang belum dibuktikan diberi status kandidat atau perlu verifikasi.

## 2. Kondisi awal

| Perangkat / sumber daya | Status dan catatan |
| --- | --- |
| ESP32 | Chip ESP32-D0WD-V3 revisi v3.1, dual-core, flash 4 MB, bridge USB Silicon Labs CP2102. Model/revisi PCB belum pasti. Firmware lama telah diganti dengan program serial_check atas izin pengguna; upload dan serial berhasil. |
| IR receiver | Modul besar bertuliskan IR Receiver memiliki label DATA, VCC, GND. Ada modul kecil tambahan yang tampak seperti receiver; model dan spesifikasi keduanya belum terverifikasi. Lihat docs/hardware.md. |
| IR transmitter | Modul bertuliskan IR Transmitter, dengan label DAT, VCC, GND. Model, driver, dan tegangan belum terverifikasi. |
| Breadboard dan jumper | Sudah dimiliki. |
| AC Sharp | AC konvensional dengan remote infrared; model unit AC belum dicatat. |
| Remote Sharp | CRMC-B094JBEZ, menggunakan 2 baterai AAA menurut konteks awal. |
| BARDI Smart LED | Sudah digunakan melalui aplikasi BARDI; model dan fitur belum dicatat. |
| MacBook Air M4 | Komputer pengembangan saat ini. Versi macOS dan tool yang tersedia belum diperiksa. |
| iPhone 15 | Target akses dashboard/PWA. |
| Wi-Fi rumah | Tersedia. |
| VPS Linux | Tersedia untuk tahap backend/cloud. |

Pengguna telah memiliki pengalaman Node.js, Next.js, Linux, Nginx, deployment VPS, dan infrastruktur cloud. Panduan hardware dan firmware tetap perlu menjelaskan fungsi komponen serta alasan setiap langkah.

## 3. Prioritas dan batas MVP pertama

**MVP pertama selesai ketika ESP32 di Mac dapat membaca remote Sharp, mengirim perintah IR yang kompatibel, dan perubahan pada AC telah diamati langsung.**

Ruang lingkup saat ini:

1. Identifikasi ESP32, receiver, transmitter, serta kebutuhan kabel dan daya.
2. Siapkan alur build, upload, dan serial monitor pada Mac.
3. Rekam sinyal remote asli secara berulang dan identifikasi protokolnya.
4. Kirim perintah IR dari ESP32 dan verifikasi respons AC.
5. Simpan konfigurasi, firmware, wiring, dan hasil uji agar dapat diulang.

Backend, MQTT, VPS, dashboard, integrasi BARDI, OTA, dan automasi belum menjadi pekerjaan implementasi pada MVP pertama. Tidak ada kebutuhan membongkar AC atau mengubah instalasi listriknya.

## 4. Prinsip kerja

- Selesaikan satu jalur kerja yang terbukti sebelum menambah lapisan berikutnya.
- Dahulukan rangkaian dan firmware minimal; tambah abstraksi ketika dibutuhkan.
- Pisahkan fakta, asumsi, dan hasil pengujian. Jangan menganggap nama produk atau bentuk modul sebagai bukti pinout.
- Verifikasi kompatibilitas board, framework, dan library sebelum menetapkan versinya; catat versi yang berhasil digunakan.
- Jangan menetapkan GPIO, tegangan suplai, atau wiring sebelum model perangkat dan dokumentasinya diperiksa.
- Setiap pengujian memiliki tujuan, langkah, hasil yang diharapkan, hasil aktual, serta kesimpulan.
- Perubahan status AC harus dibuktikan lewat pengamatan; keberhasilan pengiriman firmware atau perintah bukan bukti bahwa AC menerima sinyal.
- Simpan kredensial Wi-Fi, MQTT, dan API di konfigurasi lokal yang tidak masuk version control. Hindari menyalinnya ke log atau dokumentasi.

## 5. Keputusan dan hal yang belum final

| Topik | Arah kerja | Status |
| --- | --- | --- |
| Pengendali IR | ESP32 yang sudah dimiliki | Ditetapkan; model perlu identifikasi |
| Kontrol AC | Meniru perintah remote melalui IR eksternal | Ditetapkan |
| Library IR | IRremoteESP8266 | Kandidat; kompatibilitas board dan remote perlu diuji |
| Framework / tool firmware | Arduino framework; Arduino IDE atau PlatformIO | Dipilih saat persiapan Mac berdasarkan kompatibilitas dan kemudahan uji |
| Backend | Node.js | Arah awal, implementasi setelah IR terbukti |
| Messaging | MQTT; Mosquitto sebagai kandidat broker | Arah awal |
| Frontend | Next.js dengan target PWA di iPhone | Arah awal |
| Akses internet | VPS, HTTPS, autentikasi, MQTT TLS | Rencana tahap lanjutan |
| Kontrol lokal saat internet mati | Jalur lokal yang dapat digunakan tanpa VPS | Kebutuhan; desain belum dipilih |
| Integrasi BARDI | Evaluasi Tuya Cloud atau integrasi lokal berdasarkan perangkat | Belum diputuskan |
| Home Assistant | Alternatif hub untuk dievaluasi bila membantu | Bukan dependensi MVP |

Contoh GPIO 4 dan suplai 3,3 V pada diskusi lama adalah ilustrasi, bukan wiring yang sudah disetujui. Receiver yang datang juga belum boleh diasumsikan sebagai TSOP38438 atau VS1838B.

## 6. Rencana bertahap dan kriteria selesai

### Tahap 0 — Identifikasi hardware dan persiapan Mac

Langkah kerja:

1. Catat tulisan/model ESP32, konektor USB, serta chip USB-to-serial bila ada. Gunakan foto sisi depan dan belakang jika model belum jelas.
2. Catat model receiver/transmitter, label setiap pin, dan spesifikasi dari penjual atau produsen.
3. Periksa kabel USB yang mendukung data dan adaptor yang digunakan dengan Mac.
4. Periksa versi macOS, tool pengembangan yang sudah tersedia, serta perubahan perangkat USB/port serial ketika ESP32 disambungkan tanpa modul IR.
5. Tentukan kebutuhan driver berdasarkan identitas board dan hasil deteksi. Jangan memasang driver secara spekulatif hanya karena board pernah digunakan di Windows.
6. Tentukan tool firmware dan board target yang sesuai, lalu catat versi paket/framework dan library.
7. Sebelum upload pertama, pastikan apakah firmware/data lama perlu dipertahankan: upload firmware baru dapat mengganti program yang ada.
8. Build dan upload firmware sederhana yang menghasilkan pesan serial. Uji LED bawaan hanya jika pin serta keberadaannya diketahui.

Kriteria selesai:

- Board teridentifikasi dan port koneksinya diketahui.
- Upload firmware berhasil dan output serial terbaca.
- Setelah cabut-pasang USB, board dapat digunakan kembali dengan langkah yang terdokumentasi.
- Tidak ada kebutuhan bergantung pada konfigurasi laptop Windows lama.

### Tahap 1 — Menerima sinyal remote Sharp

Langkah kerja:

1. Verifikasi dokumentasi receiver dan pilih GPIO yang sesuai untuk board aktual.
2. Catat wiring dalam tabel pin sebelum merangkai. Putuskan daya saat mengubah koneksi; pastikan level sinyal sesuai dengan masukan board.
3. Gunakan contoh penerimaan IR dari versi library yang dipilih sebagai titik awal.
4. Baca remote asli dan simpan hasil decode maupun raw timing yang tersedia.
5. Rekam beberapa keadaan remote yang terkontrol: power, suhu, mode, fan, dan swing bila tersedia. Untuk setiap sampel, catat seluruh tampilan remote, bukan hanya tombol terakhir.
6. Ulangi minimal tiga kali untuk setiap kasus penting agar konsistensi sinyal dapat dibandingkan.

Kriteria selesai:

- Receiver membaca remote secara konsisten dan hasilnya tersimpan.
- Protokol teridentifikasi, atau tersedia rekaman raw yang memadai untuk investigasi lanjutan.
- Pengaturan remote dan hasil capture dapat dipasangkan dengan jelas.

Model CRMC-B094JBEZ belum membuktikan protokol atau dukungan library. Bila decode gagal, periksa wiring, suplai, konfigurasi capture, dan dokumentasi sebelum menyimpulkan remote tidak didukung.

### Tahap 2 — Mengirim IR ke AC

Langkah kerja:

1. Verifikasi apakah transmitter menerima sinyal carrier langsung atau memakai pembangkit carrier sendiri, serta kebutuhan driver, suplai, dan level inputnya.
2. Tentukan wiring dan GPIO dari spesifikasi aktual. Jangan menggerakkan IR LED telanjang langsung dari GPIO tanpa rangkaian pembatas arus/driver yang sesuai.
3. Jika protokol didukung, gunakan representasi state AC dari library. Jika belum, evaluasi replay raw dengan parameter yang terdokumentasi dan uji keterbatasannya.
4. Uji perintah secara manual dan bertahap dari posisi dengan jalur pandang ke AC; hindari pengulangan power cepat.
5. Verifikasi power ON/OFF, perubahan suhu, dan setidaknya satu mode/fan yang didukung. Catat fitur lain sebagai belum diuji bila belum terbukti.
6. Catat jarak, arah transmitter, kondisi awal, pengaturan yang dikirim, dan respons AC. Ulangi untuk menilai keandalan pada posisi penempatan yang direncanakan.

Kriteria selesai MVP:

- AC merespons power dan perubahan pengaturan yang diuji secara konsisten.
- Firmware dapat dibangun dan diunggah ulang dengan konfigurasi yang tercatat.
- Wiring final tahap ini, hasil capture, hasil uji, dan batasan jangkauan tersimpan.
- Kegagalan atau fitur yang belum didukung tercatat; tidak disajikan sebagai fitur yang sudah bekerja.

### Tahap 3 — Kontrol melalui jaringan lokal

Bangun jalur perintah lokal minimal setelah MVP IR selesai. Pilih HTTP pada ESP32 atau broker/backend lokal berdasarkan kebutuhan. Definisikan validasi perintah, autentikasi, status koneksi, dan perilaku saat perangkat restart.

Kriteria selesai: perangkat dapat dikontrol melalui jaringan rumah dan pengujian dengan internet terputus membuktikan jalur lokal tetap dapat digunakan. MQTT broker yang hanya berada di VPS tidak memenuhi kebutuhan ini dengan sendirinya. Akses langsung dari PWA ke perangkat lokal juga perlu diuji pada iPhone sebelum desain ditetapkan.

### Tahap 4 — Integrasi BARDI

Identifikasi model lampu dan fitur aktualnya. Evaluasi kompatibilitas akun, API cloud, atau kontrol lokal berdasarkan dokumentasi dan pengujian perangkat tersebut. Pilih satu jalur terlebih dahulu.

Kriteria selesai: power dan fitur lampu yang tersedia dapat dikontrol melalui jalur terpilih; ketergantungan internet, kredensial, biaya layanan bila ada, dan batasannya terdokumentasi.

### Tahap 5 — Backend, dashboard, dan akses internet

Bangun backend Node.js, messaging bila diperlukan, lalu dashboard Next.js/PWA. Tambahkan akses dari luar rumah melalui VPS setelah autentikasi dan koneksi terenkripsi siap. ESP32 direncanakan membuka koneksi keluar ke layanan; jangan mengandalkan pembukaan port ESP32 ke internet.

Kriteria selesai: iPhone dapat mengendalikan perangkat yang telah didukung dari jaringan rumah dan luar rumah, dengan status offline serta kegagalan perintah yang terlihat jelas.

### Tahap 6 — Automasi dan penguatan operasional

Tambahkan jadwal, timer, dan scene seperti Sleep, Work, atau Away setelah kendali manual stabil. Gunakan zona waktu Asia/Jakarta untuk jadwal pengguna. Tetapkan perilaku saat restart, internet mati, dan jadwal terlewat sebelum mengaktifkannya. Sensor, monitoring daya, Apple Shortcuts, multiroom, dan OTA adalah pengembangan berikutnya.

## 7. Model status dan keandalan

IR pada rancangan awal diperlakukan sebagai komunikasi tanpa konfirmasi dari AC. Bedakan:

- **State yang diminta:** pengaturan yang diinginkan pengguna.
- **Perintah terakhir dikirim:** pengaturan yang telah dipancarkan ESP32.
- **Kondisi teramati:** respons AC yang diverifikasi pengguna atau sensor yang relevan kelak.

Dashboard tidak boleh menampilkan perintah terakhir sebagai kepastian kondisi fisik AC. Remote asli dapat menyebabkan perbedaan state. Receiver mungkin membantu mengamati remote, tetapi penerimaan semua perintah belum dijamin.

Saat menambahkan jaringan, perintah perlu identitas, batas usia, serta penanganan duplikasi. Jangan replay perintah lama secara otomatis setelah reconnect. Hindari retained message pada topik perintah eksekusi; bedakan topik perintah dari pelaporan state. Strategi retry harus mempertimbangkan apakah perintah menetapkan state atau melakukan toggle.

## 8. Struktur proyek yang dituju

Struktur berikut adalah rencana; folder dibuat ketika tahapnya mulai dikerjakan.

```text
Smarthome/
  STEERING.md              # Acuan utama proyek
  docs/
    hardware.md            # Model, sumber spesifikasi, tegangan, dan pinout
    mac-setup.md           # Tool, versi, port, langkah upload, troubleshooting
    ir-captures/           # Rekaman remote dan metadata pengaturannya
    test-log.md            # Hasil uji dan batasan yang diketahui
    decisions.md           # Keputusan teknis dan alasan perubahan
  firmware/                # Firmware ESP32 sesuai tool yang dipilih
  backend/                 # Dibuat setelah fase hardware/local control
  web/                     # Dashboard/PWA
```

Format minimum catatan uji:

| Tanggal | Board / firmware | Wiring / konfigurasi | Skenario | Hasil yang diharapkan | Hasil aktual | Kesimpulan |
| --- | --- | --- | --- | --- | --- | --- |
| Belum diuji | — | — | — | — | — | — |

## 9. Informasi yang perlu dilengkapi

Untuk sesi berikutnya, yang paling diperlukan:

1. Foto atau tulisan model ESP32, termasuk sisi depan/belakang dan konektor USB.
2. Foto/label IR receiver dan transmitter yang benar-benar diterima, beserta tautan produk atau spesifikasinya bila tersedia.
3. Kabel/adaptor USB yang akan digunakan di Mac.
4. Apakah program lama di ESP32 perlu disimpan sebelum upload baru.

Dapat menyusul pada tahap terkait: model unit AC, jarak penempatan transmitter, model/fitur BARDI, pilihan integrasi lampu, lokasi broker/backend lokal, dan keputusan memakai Home Assistant.

## 10. Aturan melanjutkan pekerjaan

Lanjutkan dari penutupan Tahap 0 (uji cabut-pasang USB), lalu verifikasi spesifikasi receiver sebelum Tahap 1. Setup tool, build, upload, dan serial telah berhasil; lihat docs/mac-setup.md dan docs/test-log.md. Belum ada wiring atau pengujian IR.

Jangan melewati kriteria tahap hanya karena tahap selanjutnya sudah bisa ditulis sebagai kode. Jika asumsi berubah setelah hardware diperiksa, perbarui dokumen dan catatan keputusan. Sampaikan langkah praktis beserta alasan singkatnya agar pengguna dapat mengikuti dan memahami proses.

## 11. Referensi awal

- Rangkuman diskusi yang diberikan pengguna pada 18 Oktober 2026.
- Pembaruan pengguna: IR receiver dan transmitter telah tiba; ESP32 belum pernah disiapkan pada Mac ini.
- Kandidat library: [IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266). Dukungan board, protokol, versi, serta contoh penggunaannya harus diperiksa saat implementasi; belum diverifikasi dalam sesi dokumentasi ini.

## 12. Riwayat

| Tanggal | Perubahan |
| --- | --- |
| 2026-10-18 | Dokumen awal; status komponen IR diperbarui menjadi sudah tiba; persiapan ESP32 pada Mac ditempatkan sebelum pengujian IR. |
| 2026-10-18 | Identifikasi visual dari dua foto ditambahkan ke docs/hardware.md. Pemeriksaan awal Mac belum menunjukkan port serial ESP32; belum ada instalasi atau upload. |
| 2026-10-18 | Setelah disambungkan, Mac mendeteksi bridge Silicon Labs CP2102 dan port /dev/cu.usbserial-0001. Belum melakukan upload; driver tambahan tidak diperlukan pada tahap deteksi ini. |
| 2026-10-18 | Pengguna mengizinkan penggantian firmware. Arduino CLI dan core ESP32 dipasang secara lokal di proyek; program serial_check berhasil dibangun, diunggah, dan menghasilkan heartbeat serial. |
| 2026-10-18 | Listing receiver besar menyebut TSOP38438 (3,3–5,5 V); receiver kecil KY-022/1838. Wiring uji receiver besar ditetapkan VCC→3V3, GND→GND, DATA→G27. Belum dirangkai/diuji; spesifikasi transmitter masih terhalang login Shopee. |
| 2026-10-18 | Pengguna memasang receiver dan meminta lanjut tanpa pemeriksaan foto tambahan. Firmware ir_capture dengan IRremoteESP8266 2.9.0 berhasil dibangun/diunggah; serial READY, menunggu uji remote. |
| 2026-10-18 | Capture remote pertama berhasil: SHARP_AC 104 bit tanpa overflow. Decoder membaca On/Cool/19°C, fan belum dikenali. Belum ada konfirmasi layar remote, uji pengulangan, atau pengiriman ke AC. |
| 2026-10-18 | Foto mengonfirmasi Cool/19°C. Setelah beberapa UNKNOWN, toleransi IR disetel 35%; pasangan 19→20°C berhasil didecode SHARP_AC 104 bit tanpa overflow. Uji konsistensi dan pemetaan fan masih berlangsung; transmitter belum diuji. |
| 2026-10-18 | Uji konsistensi suhu lulus pada posisi sekarang: enam dari enam capture setelah penyesuaian berhasil, tiga sampel per suhu (19°C dan 20°C). Pemetaan fan dan uji transmitter belum selesai. |
| 2026-10-19 | Fan AUTO pada Cool 20°C berhasil dipetakan sebagai raw fan 2 (Auto); high dengan empat batang sebagai raw fan 7. Frame tombol cepat yang tidak lolos decode tidak dipakai sebagai data. Uji transmitter belum dimulai. |
| 2026-10-19 | Mode DRY dengan fan AUTO berhasil dipetakan: mode 3, fan 2. Protokol memuat temperature state 15°C walau layar DRY tidak menampilkan angka. |
| 2026-10-19 | Tampilan remote FAN (dua batang) ditangkap sebagai state AA5ACF1003313400088000E041 tanpa overflow. Decoder menginterpretasikan state sebagai Auto/18°C/Medium, sehingga label layar diperlakukan sebagai sumber kebenaran untuk mode ini. |
| 2026-10-19 | Mode AUTO + fan AUTO berhasil dipetakan: mode 0, fan 2, dengan temperature state 15°C yang tidak tampil di layar. Siklus mode dasar kini mencakup Cool, Dry, Fan, dan Auto. |
| 2026-10-19 | Tombol arah hembusan dipetakan untuk posisi pertama: Swing(V) 1 / High. Posisi berikutnya dan swing otomatis masih perlu diuji. |
| 2026-10-19 | Tekanan arah hembusan kedua dipetakan sebagai Swing(V) 2 / Off: louver berhenti dan kembali ke posisi tetap terakhir menurut definisi protokol library. |
| 2026-10-19 | Tekanan arah hembusan ketiga dipetakan sebagai Swing(V) 3 / Middle. |
| 2026-10-19 | Tekanan arah hembusan keempat dipetakan sebagai Swing(V) 4 / Low; pengguna mengamati arah sedikit lebih ke bawah. |
| 2026-10-19 | Tekanan arah hembusan kelima menghasilkan Swing(V) 5 (belum diberi nama decoder); pengguna mengamati posisi lebih rendah dari Low. |
| 2026-10-19 | Tekanan arah hembusan keenam menghasilkan raw Swing(V) 7. Konstanta protokol library mengartikan nilai ini sebagai toggle ayun vertikal on/off; decoder menamainya Highest secara tidak tepat. |
| 2026-10-19 | Tombol JET berhasil dipetakan sebagai frame khusus Turbo On. Field Power tidak diubah oleh frame ini. |
| 2026-10-19 | Tekanan JET kedua berhasil dipetakan sebagai Turbo Off. Indikator ayun yang tetap terlihat tidak membuktikan JET mengubah ayun, karena ayun sudah aktif sebelum pengujian. |
| 2026-10-19 | Tombol SLEEP menghasilkan frame khusus dengan byte `0x10`; foto mengonfirmasi ikon bulan aktif. Decoder library tidak memberi label Sleep pada profil remote ini. |
| 2026-10-19 | Tekanan SLEEP kedua menghasilkan frame Sleep Off; pengguna mengonfirmasi ikon bulan hilang. Pada kondisi uji ini, Sleep bekerja sebagai toggle. |
| 2026-10-19 | Tombol DISPLAY menghasilkan frame khusus `0x04`. Library menyebutnya Econo Toggle, tetapi tombol fisik menunjukkan perintah DISPLAY untuk unit indoor; respons AC belum diuji. |
| 2026-10-19 | Tombol CANCEL menghasilkan frame timer khusus `0xC0`; sesuai fungsi untuk membatalkan timer. Efek pada unit AC belum diuji dengan timer aktif. |
| 2026-10-19 | Tombol ON timer berhasil dipetakan dengan setelan 05:00, sesuai layar remote dan decoder. Timer perlu dibatalkan sesudah pengujian. |
| 2026-10-19 | CANCEL ketika ON timer 05:00 aktif berhasil menghapus indikator timer dan field On Timer pada hasil decode. |
| 2026-10-19 | Tombol OFF timer berhasil dipetakan dengan setelan 08:30, sesuai layar remote dan decoder. Timer perlu dibatalkan sesudah pengujian. |
| 2026-10-19 | CANCEL setelah OFF timer aktif mengembalikan frame tanpa timer; pembatalan OFF timer terverifikasi. |
| 2026-10-19 | OLED I2C terdeteksi di 0x3C pada GPIO21/GPIO22 dan firmware status diunggah. Receiver stabil serta menerima Sharp AC saat transmitter dilepas; transmitter memicu noise receiver saat terpasang dan ditunda untuk pemeriksaan wiring/pinout. |
| 2026-10-19 | Wiring transmitter diperbaiki: GPIO26 untuk data, GND bersama, dan VIN/5V untuk VCC transmitter. Frame Sharp Power On/Cool/19°C berhasil diterima AC, ditandai tiga beep, hingga sekitar dua meter di depan unit. |
| 2026-10-19 | Firmware `ir_control_v1` dengan OLED dan tombol BOOT berhasil mengendalikan empat profil AC yang berasal dari capture remote asli. Tahap MVP pengiriman IR telah terbukti untuk profil tersebut. |
| 2026-10-20 | Firmware `ir_web_control_v1` berhasil dikendalikan dari iPhone melalui Wi-Fi lokal dan semua profil diterima AC pada uji jarak dekat. Kegagalan sesekali pada jarak/arah lebih jauh menunjukkan batas pancaran transmitter saat ini; peningkatan hardware transmitter perlu diprioritaskan sebelum pemasangan permanen. |
| 2026-10-20 | Firmware `ir_mqtt_control_v1` menerima empat preset dari backend melalui broker MQTT lokal. Seluruh preset memicu LED transmitter tiga kali per klik Web UI; uji penerimaan fisik oleh AC ditunda karena penggunaan listrik tinggi. |
