# Capture Sharp 001

Sumber: serial monitor firmware ir_capture, IRremoteESP8266 2.9.0, GPIO27.
Pengguna melaporkan menekan tombol sekali; tombol dan tampilan remote belum dikonfirmasi.

Pembaruan: pengguna kemudian mengonfirmasi hasil dan mengirim foto layar remote yang menunjukkan COOL, 19°C. Dengan demikian suhu dan mode capture pertama cocok dengan layar. Ikon kipas dan beberapa batang terlihat, tetapi pemetaan fan=5 belum ditetapkan. Tombol yang menghasilkan capture belum dikonfirmasi secara eksplisit; kondisi fisik AC tetap belum diverifikasi.

```text
CAPTURE_BEGIN id=1 uptime_ms=73962 overflow=0
Protocol  : SHARP_AC
Code      : 0xAA5ACF10C4315200088004E0B1 (104 Bits)
AC_STATE: Model: 1 (A907), Power: On, Mode: 2 (Cool), Temp: 19C, Fan: 5 (UNKNOWN), Swing(V): 0 (N/A), Turbo: Off, Ion: Off, Econo: -, Clean: Off
```

```cpp
uint8_t state[13] = {0xAA, 0x5A, 0xCF, 0x10, 0xC4, 0x31, 0x52, 0x00, 0x08, 0x80, 0x04, 0xE0, 0xB1};
uint16_t rawData[211] = {3834, 1866, 466, 466, 466, 1404, 462, 494, 438, 1430, 436, 496, 436, 1430, 436, 494, 438, 1430, 438, 494, 438, 1430, 438, 494, 438, 1428, 438, 1430, 438, 494, 438, 1430, 438, 494, 438, 1430, 438, 1430, 438, 1430, 438, 1430, 438, 494, 438, 494, 442, 1426, 438, 1430, 438, 494, 438, 494, 440, 492, 438, 494, 438, 1428, 438, 494, 438, 494, 438, 494, 438, 494, 438, 492, 440, 1428, 438, 494, 438, 494, 444, 490, 424, 1442, 424, 1442, 426, 1442, 426, 506, 448, 482, 426, 506, 426, 1442, 426, 1442, 424, 508, 426, 506, 426, 506, 424, 1444, 426, 506, 426, 506, 426, 1442, 424, 506, 426, 1442, 424, 508, 426, 506, 424, 506, 426, 508, 426, 506, 426, 506, 426, 506, 426, 506, 426, 508, 424, 506, 426, 508, 424, 508, 426, 1442, 426, 508, 426, 506, 426, 506, 426, 508, 426, 506, 426, 506, 424, 506, 426, 506, 426, 506, 426, 506, 426, 506, 426, 1442, 426, 506, 426, 506, 426, 1442, 426, 506, 426, 506, 426, 506, 426, 506, 426, 506, 428, 506, 426, 506, 426, 506, 426, 506, 426, 506, 426, 1442, 426, 1442, 426, 1442, 426, 1442, 426, 506, 426, 506, 426, 506, 426, 1444, 422, 1442, 426, 508, 426, 1440, 426};
```

Kesimpulan: satu frame diterima dan dikenali sebagai SHARP_AC tanpa overflow. Label model A907 adalah interpretasi decoder, bukan identifikasi fisik remote. Fan 5 belum dipetakan decoder dan swing N/A belum membuktikan fitur tidak tersedia. Nilai suhu/mode/power perlu dicocokkan dengan layar remote. Kondisi fisik AC belum diverifikasi. Perlu capture berulang sebelum menyatakan penerimaan konsisten.
