PlatformIO copy templates helper

Used by **cyd-air-monitor** (and derived from [cyd-lvgl-template](https://github.com/Deejpotter/cyd-lvgl-template)). This pre-build script copies display configuration into `.pio/libdeps/<env>/`.

Per-environment overrides

Place board-specific files under folders named for the PlatformIO environment, for example:

    template files/2432s028r/
    template files/jc2432w328r/
    template files/jc2432w328c/

The script will prefer files from that folder for the env with the same name.

Files copied by default

- `User_Setup.h` -> copied into the `TFT_eSPI` library folder under `.pio/libdeps/<env>/TFT_eSPI/` when found in `template files/` or per-env folder.
- `lv_conf.h` -> copied to the `.pio/libdeps/<env>/` root (next to the `lvgl` library folder) — this mirrors the LVGL configuration requirement.

Behavior

- Existing files are overwritten by the templates (no `.bak` is created).
- The script logs what it copies and whether anything is skipped.

If you want to add more templates, update `scripts/copy_template.py` FILES_TO_COPY mapping.
