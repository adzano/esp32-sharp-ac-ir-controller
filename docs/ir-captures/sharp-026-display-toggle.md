# Sharp AC — DISPLAY toggle

## Kondisi remote

- Tombol yang ditekan: `DISPLAY`
- Layar remote tidak berubah

## Hasil tangkapan

```text
AA 5A CF 10 00 61 20 00 0F 80 04 E0 41
```

Frame SHARP_AC 104-bit diterima tanpa overflow. Library menampilkan `Econo:
Toggle` untuk byte perintah khusus `0x04`. Namun tombol fisik remote bertuliskan
DISPLAY, sehingga pada remote ini frame tersebut dicatat sebagai **DISPLAY
toggle**: kemungkinan untuk menyalakan atau mematikan tampilan/lampu pada unit
indoor, bukan mengubah LCD remote.

Respons unit AC belum diuji, sehingga fungsi fisik dan arah toggle (on atau off)
belum dapat dipastikan.
