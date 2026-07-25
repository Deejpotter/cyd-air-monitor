# Troubleshooting

Use the interactive guide below — pick your board and symptom for targeted fixes.

<TroubleshootingGuide />

## Common mistakes

### Wrong PlatformIO env

The most frequent issue. Symptoms:

- Blank or garbled display
- Touch completely dead
- UI layout clipped or rotated wrong

**Fix:** [Board Explorer](/boards/) → copy the correct `pio run -e ...` command.

### Resistive vs capacitive JC2432

| You have | Flash |
|----------|-------|
| JC2432W328**R** (resistive) | `jc2432w328r` |
| JC2432W328**C** (capacitive) | `jc2432w328c` |

### DHT11 on wrong GPIO

Each env uses a different pin — see [Getting Started](/guide/getting-started#4-wire-the-dht11).

### Editing `.pio/` directly

Template copies are overwritten every build. Edit `template files/` instead.

## Build / upload issues

| Problem | Fix |
|---------|-----|
| Permission denied on serial port | `sudo usermod -aG dialout $USER`, then **new terminal** — or `newgrp dialout` in the current shell |
| `.pio/build` owned by root | `sudo chown -R $USER:$USER .pio` after sudo builds |
| JC4827 library error | Use `moononournation/GFX Library for Arduino@1.4.9` (not `Arduino_GFX`) |

## Still stuck?

1. Capture `pio device monitor` output from boot
2. Note exact board model / silkscreen text
3. Open a [GitHub issue](https://github.com/Deejpotter/cyd-air-monitor/issues) with env name and symptoms

## Resources

- [ESP32-Cheap-Yellow-Display community hub](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)
- [LVGL docs](https://docs.lvgl.io/)
- [Display & touch tuning](/guide/display-touch)
