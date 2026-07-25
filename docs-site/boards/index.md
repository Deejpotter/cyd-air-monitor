# Boards

Every supported panel has a dedicated feature page — hardware, firmware, pins, build defaults, and flash commands in one place.

<BoardCardGrid />

## Quick identification

| You have… | Env | Touch |
|-----------|-----|-------|
| Dual USB CYD, ESP32, 2.8″ | [`2432s028r`](/boards/2432s028r) | Resistive XPT2046 |
| JC2432 **R**, resistive film | [`jc2432w328r`](/boards/jc2432w328r) | Resistive XPT2046 |
| JC2432 **C**, glass capacitive | [`jc2432w328c`](/boards/jc2432w328c) | CST820 I²C |
| Guition JC4827, 4.3″, ESP32-S3 | [`jc4827w543r`](/boards/jc4827w543r) | Resistive XPT2046 |

::: warning JC2432 — check R vs C
**R** and **C** look identical in photos. The resistive env will show a working display and **dead touch** on a capacitive board.
:::

## Also available

- [Compare all boards](/boards/compare) — side-by-side specs  
- [Gotchas](/boards/gotchas) — common pitfalls by board  
- [External links](/boards/references) — pinouts, repos, libraries  
- [Flash guide](/guide/getting-started) — first upload and serial monitor
