# INTERACTION & E-PAPER — XOS-DL v1.0 (PROPOSED)

## 1. Navigation map

```text
Home
 ├─ Continue Reading → Reader
 ├─ Recent Book → Reader
 ├─ Library → Library List → Book → Reader
 ├─ Add Books → Import / Files chooser (availability checked)
 └─ Settings → Settings index → detail
Reader
 ├─ Left/Right tap OR mapped hardware key → page backward/forward (proposed)
 ├─ Center tap → contextual controls: TOC / type / progress / back
 └─ Back → previous screen or Library; reading position preserved
```

No persistent bottom app dock; **Home one-tap utilities are not a global tab bar**. Library, Files and Settings use a consistent visible back action. Both hardware-key and touch users must be able to navigate every essential flow. Where CrossPoint does not expose a required behavior, label it `requires verification` rather than silently inventing it.

## 2. Touch ergonomics

- Browser hit target size ≥48×48px; primary ≥56px. Adjacent hit zones separated by ≥8px where practicable.
- Do not mistake Apple's 44-point guidance for X4Pro display pixels. Physical validation of GT911 edges and button mappings is mandatory before native release.
- Repeat operations (page turn) require accidental-touch protection and direct visible feedback as appropriate, but never block reading with constant chrome.

## 3. E-paper redraw contract

- **Stable screen:** no animation, fake refreshing content, decorative progress shimmer, or clock updates unless deliberately needed.
- **Text/list change:** mark dirty region; request partial redraw only if verified supported and acceptable ghosting. Full refresh policy is an engineering decision after panel testing.
- **Mode change:** opaque transition; no translucent overlays that require complex compositing; clear stale pixels fully on close.
- **Invert mode:** independent feasibility task. No promise of native 1-bit invert or power savings.
- **Book cover:** pre-render/crop in correct aspect ratio; never fake real user cover via generated generic imagery in production.
- **Status data:** always sourced from real firmware runtime; stale/unknown values are hidden or explicitly marked unknown.

## 4. Keyboard and accessibility

- Search fields should not obscure the result list permanently; keyboard entry path must be tested in CrossPoint.
- Hardware focus visible via 2px stroke/inversion and consistent ordering: top actions → content → secondary actions.
- Destructive actions require separate confirmation; Back must not discard reading state.
- Empty, disabled and error states receive the same level of design detail as populated screens.

## 5. Browser vs firmware boundary

`specimen.html` illustrates **intent only**. It does not implement EPUB pagination, SD access, keyboard, real battery, touch calibration, waveform or reading persistence. Agent reports must separate **DESIGNED**, **BROWSER-TESTED**, **SIMULATOR-VERIFIED** and **DEVICE-VERIFIED**.
