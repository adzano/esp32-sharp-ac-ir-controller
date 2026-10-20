# Sharp AC — JET dimatikan

## Kondisi remote

- Mode: AUTO
- Kecepatan kipas: AUTO
- Indikator ayun vertikal masih terlihat (tiga garis)
- Tombol yang ditekan: `JET` untuk kedua kali

## Hasil tangkapan

```text
AA 5A CF 10 00 71 20 00 0F 80 01 E0 01
```

Decoder membaca perintah khusus ini sebagai `Turbo: Off`.

Nilai ayun vertikal mentahnya adalah `7`. Pada protokol Sharp, nilai ini adalah
perintah *toggle* ayun terus-menerus, bukan posisi louver tetap. Karena ayun
sudah diaktifkan sebelum pengujian JET, tangkapan ini belum membuktikan bahwa
tombol JET mengaktifkan ayun. Hasil yang pasti adalah: JET mengubah Turbo dari
On menjadi Off.
