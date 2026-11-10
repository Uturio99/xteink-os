# Native typography bake-off

The experiment runs in a fresh copy of `/home/uturio/noveos-font-gate-check`. It reuses that checkout's V2 Home composition, `GfxRenderer`, font converter, Unicode host test, simulator and screenshot command. A small isolated overlay inserts five fixed recent-book records from `scene-content.json`; fake paths have no backing files, and no production source or data is changed.

The five existing Home roles remain 8 px regular, 10 px bold, 11 px regular, 13 px regular and 21 px regular. Variable sources pin `opsz` to the role size clamped to its supported range, and `wght` to 400 or 700. Positions, spacing and scene content remain fixed; wrapping and clipping differences are evidence and must not be tuned per font.

Exact source repositories, commits, files, releases and OFL licenses are in `manifest.json`. `prepare.sh` downloads each TTF and OFL file from the pinned commit and records SHA-256 values in `run-manifest.json`. Upstream metadata reports Newsreader axes `opsz 6–72`, `wght 200–800`; Literata `opsz 7–72`, `wght 200–900`; Noto Serif v2.015 supplies variable `wdth`/`wght` sources.

## Safe Windows-to-WSL sync

Run `sync_bakeoff.ps1` from Windows PowerShell. It refuses an existing destination, copies the verified checkout to `/home/uturio/noveos-font-bakeoff`, then adds this artifact directory only if absent. It normalizes text files to LF and verifies hashes. It never writes into `noveos-font-gate-check`.

## Ubuntu commands

```bash
cd /home/uturio/noveos-font-bakeoff/docs/design/experiments/native-typography-bakeoff
./prepare.sh
./render.sh noto-serif
./render.sh newsreader
./render.sh literata
```

Each render regenerates the five existing serif headers in the isolated copy, runs its existing `VietnameseFontCoverageTest`, builds with `/home/uturio/noveos-simulator/.venv/bin/pio run -e simulator_x4_pro`, and captures the native framebuffer under `captures/<candidate>/`. The same injected Home scene and fixed long NFC/NFD titles are used for every run. Do not build firmware-size variants before PO selects a candidate.

The user reports a previous 13/13 Noto Serif Unicode pass and successful native Noto Serif specimen; these are historical reports, not results of this bake-off. All candidate Unicode runs, simulator builds, screenshots, clipping review and visual comparison are **NOT TESTED**. PO approval is pending.
