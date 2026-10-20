# Capture Sharp 017 — louver berhenti pada posisi terakhir

Foto remote masih menunjukkan AUTO dan fan AUTO dengan indikator arah hembusan.

```text
Protocol: SHARP_AC (104 bits)
Power: On
Mode: 0 (Auto)
Temperature state: 15C
Fan: 2 (Auto)
Swing(V): 2 (Off)
Overflow: false
State: AA 5A CF 10 00 31 20 00 0A 80 06 E0 61
```

Pada definisi protokol Sharp dari library, Swing(V) Off berarti menghentikan gerak louver dan kembali ke posisi tetap terakhir. Ini bukan bukti bahwa indikator layar harus menghilang.
