# Sharp AC — CANCEL timer

## Kondisi remote

- Tombol yang ditekan: `CANCEL`
- LCD remote tidak berubah

## Hasil tangkapan

```text
AA 5A CF 10 00 81 20 00 0F 80 C0 E0 21
```

Frame SHARP_AC 104-bit diterima tanpa overflow. Byte perintah khusus `0xC0`
sesuai kategori timer pada protokol Sharp. Decoder tidak menunjukkan timer aktif,
selaras dengan fungsi `CANCEL`: membatalkan timer yang sedang tersetel. LCD tidak
berubah bila tidak ada timer aktif untuk dibatalkan.

## Verifikasi pembatalan saat timer aktif

Setelah ON timer 05:00 disetel, tombol `CANCEL` menghasilkan frame berikut:

```text
AA 5A CF 10 00 81 20 40 0F 80 C0 E0 61
```

Frame diterima tanpa overflow. Decoder tidak lagi menampilkan `On Timer`, dan
pengguna mengonfirmasi layar remote kembali kosong. Pembatalan timer pada remote
terverifikasi. Efek terhadap unit AC belum diuji.
