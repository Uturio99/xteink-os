import os
import sys
import zipfile

sys.stdout.reconfigure(encoding='utf-8')

ZIP_PATH = os.path.abspath("docs/design/prototype/xteink-os-m1-unified-ui-review.zip")

README_CONTENT = """XTEInk OS — M1 Unified UI/UX Collection Review Package
======================================================
Target Device: XTEINK X4 Pro (480x800 Portrait E-Paper)
Milestone: M1 Acceptance Patch (Tasks 1–5 & Fixes P1-A / P1-B Complete)

INSPECTION INSTRUCTIONS:
1. Open 'prototype.html' in Google Chrome, Microsoft Edge, or Safari:
   - Genuine TrueType Noto Serif and Noto Sans fonts are bundled offline in 'fonts/' with SIL Open Font License 1.1 for 100% reproducible typography across all host environments.
   - Use top bar buttons (FR-01 through FR-15) to jump directly between frames.
   - Click 'Journey A' through 'Journey E' to trigger automated user journey playback.
   - In FR-05 (Library), test sorting by 'Gần đây', 'Tên sách (A-Z)' (Vietnamese alphabetical sorting), and 'Tiến độ' (reading progress descending).
   - In FR-06 (Search), test clearing search via '(×)': restores the full 6-book catalogue, updates status to 'Toàn bộ danh mục (6 cuốn)', and retains input focus; try accented ('chiến tranh') and unaccented ('chien tranh') queries to see accurate highlighted relevance.
   - In FR-08 (Reader), press Arrow Left / Arrow Right to turn pages; press Space to toggle HUD (FR-09).
   - In FR-09 (Reader HUD), notice the underlying reading prose remains fully visible and legible under the opaque control bars; tapping the central reading area returns to FR-08.
   - In FR-11 (Typography Settings), observe aligned defaults (19px / 1.65); changing font size, family, line height, or margins updates both FR-08 and FR-09 identically while preserving chapter/page reading state.
   - In the top toolbar, use '[Chế độ: Tệp hợp lệ / Lỗi định dạng]' to toggle FR-13 between valid EPUB and unsupported file fallback.
2. Open 'gallery.html' for a 15-frame contact sheet with reconciled design metadata.
3. Inspect 'captures/' for exact native 480x800 captures and 'captures/binary/' for 1-bit legibility evidence.
4. Read 'specs/UNIFIED_UI_COLLECTION.md' and audit reports in 'audits/' for complete design contracts and QA proof.
"""

def build_zip():
    print(f"Building review zip package at: {ZIP_PATH}")
    if os.path.exists(ZIP_PATH):
        os.remove(ZIP_PATH)

    with zipfile.ZipFile(ZIP_PATH, 'w', compression=zipfile.ZIP_DEFLATED) as z:
        # 1. Prototype & Gallery HTML
        z.write("docs/design/prototype/unified-ui-prototype.html", arcname="prototype.html")
        z.write("docs/design/prototype/gallery.html", arcname="gallery.html")

        # 2. Fonts
        fonts_dir = "docs/design/prototype/fonts"
        for fname in sorted(os.listdir(fonts_dir)):
            fpath = os.path.join(fonts_dir, fname)
            if os.path.isfile(fpath):
                z.write(fpath, arcname=f"fonts/{fname}")

        # 3. Reference Captures
        captures_dir = "docs/design/prototype/captures"
        for i in range(1, 16):
            fname = f"FR-{i:02d}.png"
            fpath = os.path.join(captures_dir, fname)
            z.write(fpath, arcname=f"captures/{fname}")

        # 4. Before Remediation Captures
        before_dir = os.path.join(captures_dir, "before_remediation")
        if os.path.exists(before_dir):
            for i in range(1, 16):
                fname = f"FR-{i:02d}.png"
                fpath = os.path.join(before_dir, fname)
                if os.path.exists(fpath):
                    z.write(fpath, arcname=f"captures/before_remediation/{fname}")

        # 5. Binary 1-bit Captures
        binary_dir = os.path.join(captures_dir, "binary")
        if os.path.exists(binary_dir):
            for i in range(1, 16):
                fname = f"FR-{i:02d}_binary.png"
                fpath = os.path.join(binary_dir, fname)
                if os.path.exists(fpath):
                    z.write(fpath, arcname=f"captures/binary/{fname}")

        # 6. Specs
        z.write("docs/design/specs/UNIFIED_UI_COLLECTION.md", arcname="specs/UNIFIED_UI_COLLECTION.md")

        # 7. Audits
        z.write("docs/superpowers/audits/M1-UI-COLLECTION-RECONCILIATION.md", arcname="audits/M1-UI-COLLECTION-RECONCILIATION.md")
        z.write("docs/superpowers/audits/M1-UI-COLLECTION-QA.md", arcname="audits/M1-UI-COLLECTION-QA.md")
        z.write("docs/superpowers/audits/M1-UI-COLLECTION-REVIEW.md", arcname="audits/M1-UI-COLLECTION-REVIEW.md")

        # 8. README.txt
        z.writestr("README.txt", README_CONTENT)

    print("Zip package build completed successfully.")
    
    # Verify contents
    with zipfile.ZipFile(ZIP_PATH, 'r') as z:
        names = z.namelist()
        print(f"Total entries in archive: {len(names)}")
        print("Archive structure summary:")
        for prefix in ["prototype.html", "gallery.html", "fonts/", "captures/FR", "captures/before_remediation/", "captures/binary/", "specs/", "audits/", "README.txt"]:
            matching = [n for n in names if n.startswith(prefix)]
            print(f"  - {prefix}: {len(matching)} item(s)")

if __name__ == "__main__":
    build_zip()
