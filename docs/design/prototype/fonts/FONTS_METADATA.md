# Typography Assets & Licensing Metadata

## 1. Provenance & Included Files
- `NotoSerif-Regular.ttf` — Noto Serif Regular (TrueType)
- `NotoSerif-Bold.ttf` — Noto Serif Bold (TrueType)
- `NotoSerif-Italic.ttf` — Noto Serif Italic (TrueType)
- `NotoSans-Regular.ttf` — Noto Sans Regular (TrueType)
- `NotoSans-Bold.ttf` — Noto Sans Bold (TrueType)
- `NotoSans-Italic.ttf` — Noto Sans Italic (TrueType)

Source: Sourced directly from `lib/EpdFont/builtinFonts/source/NotoSerif/` and `lib/EpdFont/builtinFonts/source/NotoSans/` within the `xteink-os` repository.

## 2. Redistribution License
These font files are distributed under the **SIL Open Font License, Version 1.1** (OFL-1.1).
The complete legal text of the license is preserved in [`OFL.txt`](OFL.txt).
Copyright (c) 2012, Google Inc. (https://fonts.google.com/noto).

## 3. Engineering Classification
**BROWSER REFERENCE FONTS ONLY**:
These bundled TrueType assets are provided to establish a 100% reproducible, offline-capable typography environment for browser prototyping, visual auditing, and automated regression testing. They eliminate cross-platform font substitution differences between Chromium environments.

They are **NOT** claimed as proven firmware-identical rendering. On-device embedded rendering is governed by the FreeInk/EpdFont vector font rasterizer with dedicated glyph caching in PSRAM, which is subject to physical hardware rasterization and verification in Milestones M4 and M7.
