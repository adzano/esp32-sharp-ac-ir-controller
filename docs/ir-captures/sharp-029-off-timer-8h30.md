# Sharp AC — OFF timer 8 jam 30 menit

## Kondisi remote

- Tombol yang ditekan: `OFF` timer
- Layar menunjukkan ikon timer OFF dan `8.5 h`

## Hasil tangkapan

```text
AA 5A CF 10 00 81 20 88 0F 80 DE E0 D1
```

Frame SHARP_AC 104-bit diterima tanpa overflow. Decoder membaca `Off Timer:
08:30`, sesuai tampilan remote. Nilai khusus `0xDE` merepresentasikan timer
dengan tambahan setengah jam. Timer aktif pada remote dan perlu dibatalkan dengan
`CANCEL` setelah pengujian.

## Verifikasi pembatalan

Setelah `CANCEL`, frame kembali ke:

```text
AA 5A CF 10 00 81 20 00 0F 80 C0 E0 21
```

Frame diterima tanpa overflow dan decoder tidak lagi menampilkan `Off Timer`.
Pembatalan OFF timer pada remote terverifikasi.
