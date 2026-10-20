# Sharp AC — Sleep aktif

## Kondisi remote

- Mode: AUTO
- Kecepatan kipas: AUTO
- Indikator Sleep (ikon bulan) muncul
- Indikator ayun vertikal masih terlihat
- Tombol yang ditekan: `SLEEP`

## Hasil tangkapan

```text
AA 5A CF 10 00 61 20 00 0F 80 10 E0 11
```

Frame SHARP_AC 104-bit diterima tanpa overflow. Decoder library tidak memiliki
label Sleep untuk profil remote ini, sehingga ia hanya menampilkan state umum
dan tidak menamai fitur tersebut. Namun, foto layar mengonfirmasi ikon Sleep
aktif, sementara byte perintah khusus berubah menjadi `0x10`. Karena itu frame
ini dicatat sebagai **Sleep On** untuk remote ini.

Durasi atau perilaku temperatur Sleep belum diuji dan tidak boleh diasumsikan.
