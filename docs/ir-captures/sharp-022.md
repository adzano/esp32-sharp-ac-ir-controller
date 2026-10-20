# Capture Sharp 022 — JET / Turbo on

Foto remote menunjukkan ikon JET aktif.

```text
Protocol: SHARP_AC (104 bits)
Power: - (tidak diubah oleh frame khusus)
Mode: 0 (Auto)
Temperature state: 15C
Fan: 2 (Auto)
Turbo: On
Overflow: false
State: AA 5A CF 10 00 61 20 00 08 80 01 E0 61
```

Tombol JET mengirim frame khusus yang menyalakan Turbo. Tanda `Power: -` berarti field power tidak diubah oleh frame ini; bukan laporan bahwa AC mati.
