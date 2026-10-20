# Capture 002 — percobaan suhu 20°C

Pengguna menyatakan sudah menjalankan instruksi menaikkan suhu dari 19°C ke 20°C. Tidak ada foto layar baru. Sinyal diterima tetapi decoder menghasilkan UNKNOWN; suhu 20°C belum berhasil divalidasi melalui decode.

```text
CAPTURE_BEGIN id=2 uptime_ms=410677 overflow=0
Protocol  : UNKNOWN
Code      : 0xCF7E8096 (105 Bits)
uint16_t rawData[209] = {3844, 1860,  476, 482,  450, 1418,  448, 486,  446, 1424,  446, 486,  446, 1446,  422, 486,  446, 1448,  422, 486,  446, 1448,  420, 486,  446, 1448,  422, 1446,  422, 512,  420, 1450,  418, 510,  422, 1446,  422, 1446,  420, 1448,  420, 1448,  420, 512,  422, 512,  422, 1446,  420, 1448,  420, 512,  422, 512,  422, 510,  422, 510,  422, 1446,  420, 512,  422, 510,  422, 512,  422, 1446,  422, 512,  420, 1448,  420, 512,  422, 512,  420, 512,  420, 1448,  420, 1448,  420, 1448,  420, 512,  420, 512,  420, 512,  420, 1448,  398, 1496,  374, 532,  420, 512,  398, 534,  398, 1496,  374, 534,  398, 534,  398, 1498,  374, 534,  396, 1498,  372, 558,  374, 560,  374, 558,  374, 560,  372, 558,  374, 558,  374, 560,  372, 560,  374, 560,  372, 560,  372, 560,  374, 560,  374, 1546,  324, 582,  348, 584,  348, 584,  348, 584,  350, 584,  348, 584,  348, 584,  348, 584,  348, 584,  348, 610,  326, 606,  328, 1594,  262, 644,  302, 606,  326, 1596,  258, 648,  302, 630,  302, 606,  324, 632,  302, 630,  302, 630,  302, 632,  300, 632,  302, 632,  302, 630,  302, 1620,  210, 1684,  160, 3552,  210, 696,  262, 646,  302, 632,  302, 630,  302, 1644,  160, 720,  262, 1660,  160};  // UNKNOWN CF7E8096

CAPTURE_END
```

Tidak ada overflow. Raw berisi 209 interval (capture pertama 211), dengan perubahan durasi pulsa menuju akhir frame. Penyebab belum dipastikan; ulangi pengujian dengan posisi remote stabil sebelum mengubah firmware atau toleransi decoder.
