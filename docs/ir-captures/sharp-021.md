# Capture Sharp 021 — toggle ayun vertikal

Foto remote menunjukkan ikon tiga garis arah hembusan.

```text
Protocol: SHARP_AC (104 bits)
Power: On
Mode: 0 (Auto)
Temperature state: 15C
Fan: 2 (Auto)
Raw Swing(V): 7
Overflow: false
State: AA 5A CF 10 00 31 20 00 0F 80 06 E0 31
```

Meski output decoder menulis `Highest`, konstanta protokol Sharp pada versi library ini menyatakan raw Swing(V) 7 sebagai **Toggle Constant swinging on/off**. Foto ikon tiga garis selaras dengan perintah toggle ayun vertikal. Ini adalah perintah toggle, sehingga state fisik ayun tidak dapat dipastikan hanya dari frame berikutnya tanpa pengamatan louver.
