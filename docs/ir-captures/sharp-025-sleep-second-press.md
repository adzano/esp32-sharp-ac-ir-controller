# Sharp AC — Sleep dimatikan

## Hasil tangkapan

```text
AA 5A CF 10 00 71 20 00 0F 80 10 E0 01
```

Frame SHARP_AC 104-bit diterima tanpa overflow setelah tombol `SLEEP` ditekan
kedua kali. Frame ini berbeda dari Sleep aktif pada byte perintah dan checksum.

Pengguna mengonfirmasi ikon bulan sudah hilang setelah tekanan kedua, sehingga
frame ini dipetakan sebagai **Sleep Off**. Pada kondisi uji ini, tombol SLEEP
berfungsi sebagai toggle antara Sleep On dan Sleep Off.
