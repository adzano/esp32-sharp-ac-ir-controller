# Capture Sharp 014 — tampilan FAN

Foto remote menunjukkan mode **FAN** dan dua batang indikator kipas.

```text
Protocol: SHARP_AC (104 bits)
Power: On
Display mode observed: FAN
Decoder mode: 0 (Auto)
Decoder temperature state: 18C
Decoder fan: 3 (Medium)
Overflow: false
State: AA 5A CF 10 03 31 34 00 08 80 00 E0 41
```

Interpretasi mode dari decoder tidak cocok dengan label FAN pada layar remote. Frame diterima lengkap; karena itu label layar dipakai sebagai sumber kebenaran untuk mode ini. Nilai mode/temperature/fan hasil decoder dicatat sebagai representasi library, bukan bukti bahwa remote menampilkan AUTO atau 18°C.
