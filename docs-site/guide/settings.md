# Settings & features

Tap the **gear icon** on the dashboard to open Settings.

## WiFi — two ways to connect

You can configure WiFi **on the panel** or **from a browser**. Both use the same saved credentials (NVS) — pick whichever is easier.

### Option A: On the touchscreen

**Settings → WiFi**

1. **Scan** — discovers nearby 2.4 GHz networks (non-blocking)
2. **SSID dropdown** — pick a network from the list
3. **Password** — tap the field; on-screen keyboard appears
4. **Connect** — saves credentials and connects
5. **Forget WiFi** — clears saved SSID/password from NVS

### Option B: From a phone or PC browser

**Settings → Browser Setup** on the panel, then:

1. Tap **Start Setup Hotspot** (creates WiFi network `CYD-Setup`)
2. On your phone or PC, join **CYD-Setup**
3. Open **http://192.168.4.1** in a browser (many phones open this automatically)
4. Use the **WiFi** tab to scan and connect to your home network

When already connected to your LAN, skip the hotspot and open **http://&lt;device-ip&gt;** instead (shown on the dashboard).

The browser UI also lets you change display rotation and touch calibration — same values as the touchscreen settings.

## Display rotation

**Settings → Rotate Display** cycles through 0°, 90°, 180°, and 270°. Touch coordinates follow the rotation automatically — useful when the panel is mounted upside-down or sideways without reflashing.

The chosen angle is saved to NVS and restored on boot.

## Web dashboard (LAN)

When WiFi is connected, the device serves a **local web UI** on port 80:

- **http://&lt;device-ip&gt;** — most reliable (shown on the CYD dashboard as `Browser: http://192.168.x.x`)
- **http://cydmon.local** — mDNS; needs your PC on the same LAN and Avahi/Bonjour support

::: tip If cydmon.local fails
Use the IP from the dashboard or serial monitor. Your PC must be on the **same WiFi network** as the board (2.4 GHz).
:::

The web UI mirrors touchscreen settings with more room for detail:

| Tab | What you can do |
|-----|-----------------|
| **Dashboard** | Live temp/humidity, board info, WiFi RSSI, uptime (auto-refreshes) |
| **WiFi** | Scan, connect, forget — same NVS credentials as the panel |
| **Display** | Set rotation 0°–270° — syncs with Settings → Rotate Display |
| **Touch** | View/edit resistive calibration (capacitive is read-only) |

Changes from the browser apply immediately on the device and survive reboot (NVS).

::: info Security
No login — intended for trusted home LAN only. Do not port-forward to the internet without adding authentication.
:::

### Persistence

Credentials are stored in ESP32 **NVS** via Arduino `Preferences`:

- Namespace: `cydmon`
- Auto-reconnect on boot when saved credentials exist

### Keyboard behaviour (touch WiFi screen)

The keyboard dismisses when you:

- Tap **OK** or **Cancel**
- Tap **Connect**, **Scan**, **Forget WiFi**, or **Back**
- Tap the SSID dropdown
- Defocus the password field

If the keyboard ever sticks open on an older build, flash the latest firmware — `hideKeyboard()` clears textarea focus and association.

## Touch Test screen

| Board type | Features |
|------------|----------|
| Resistive (2432S028R, JC2432W328R, JC4827) | Raw ADC, mapped X/Y, touch dot, min/max +/- buttons, **Save Cal** → NVS |
| Capacitive (JC2432W328C) | Live coords and dot — diagnostic only |

Saved resistive calibration overrides compile-time `TOUCH_X_MIN/MAX` defaults on next boot.

## NVS schema

Managed by `SettingsStore`:

- WiFi SSID, password, auto-connect flag
- Touch calibration (resistive): xMin, xMax, yMin, yMax, saved flag
- Schema version for future migrations

Use **Forget WiFi** in the app rather than erasing flash when rotating networks.
