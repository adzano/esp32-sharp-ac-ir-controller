# Capture Sharp 013 — DRY + AUTO

Foto remote menunjukkan mode DRY dan fan AUTO. Decoder berhasil mengenali satu frame bersih.

```text
Protocol: SHARP_AC (104 bits)
Power: On
Mode: 3 (Dry)
Temperature state: 15C
Fan: 2 (Auto)
Overflow: false
State: AA 5A CF 10 00 31 23 00 08 80 00 E0 11
```

Catatan: layar remote tidak menampilkan angka suhu ketika DRY. Nilai 15°C berasal dari state protokol yang didecode, bukan konfirmasi setpoint yang terlihat di layar. Beberapa frame sebelumnya tidak lolos decode karena penekanan sebelumnya/jeda monitor dan tidak dicatat sebagai pemetaan.
