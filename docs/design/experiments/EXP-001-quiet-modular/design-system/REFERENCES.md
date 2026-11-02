# VERIFIED UI REFERENCE LIBRARY — XOS-DL v1.0

**Purpose:** Exact, traceable visual references so design agents cannot invent vague "Apple vibes." These are external copyrighted examples; **link to them, do not copy source images into a public repository**. Sources verified 2026-11-02. All references are for **principles, not pixel-for-pixel cloning**.

## R1 — Apple HIG: layout and visual hierarchy (PRIMARY BEHAVIORAL AUTHORITY)

- **Official document:** https://developer.apple.com/design/human-interface-guidelines/layout
- **Official principles:** https://developer.apple.com/design/human-interface-guidelines/design-principles
- **Learn:** Put essential content near the leading/top scan path; align related material; use negative space as grouping; keep controls discoverable and give secondary information progressive disclosure.
- **Do not copy:** iOS-specific Liquid Glass, tab bars, iPhone safe area or SF glyphs as if they were hardware specifications for X4 Pro.
- **Design test:** Can the user tell the one main action within 2 seconds without reading every label?

## R2 — Apple Books / Reading Now (PRIMARY COMPOSITION REFERENCE)

- **Original Apple newsroom:** https://www.apple.com/newsroom/2018/06/apple-books-all-new-for-iphone-and-ipad-celebrates-reading/
- **Illustration (Apple-hosted):** https://www.apple.com/newsroom/images/product/apple-books/apple-books_reading-now_06122018_big.jpg.large_2x.jpg
- **Learn:** A reading-first primary object, clear next action, then a lightweight 'recently opened' rhythm. Typography establishes section ranking and books are content, not dashboard widgets.
- **Do not copy:** the full-width iPhone tab bar, color recommendations, sales modules or its exact spacing. This 2018 UI is *historical*, not an assertion about the current Apple Books UI.
- **Design test:** Home's reading object must outweigh the sum of secondary destinations.

## R3 — DuRoBo Krono / actual launcher (PRIMARY DEDICATED-DEVICE REFERENCE)

- **Actual device photo / screen:** https://www.androidcentral.com/wearables/durobo-krono-e-reader
- **Image URL:** https://cdn.mos.cms.futurecdn.net/xBhBX6AjNtPEEJfavgr3SE.jpg
- **Additional manufacturer's UI context:** https://www.durobo.com/blogs/news/kronos-3-refresh-modes-explained-clarity-speed-quality
- **Learn:** Single dominant status/clock area, simple vertical direct-access commands, wide calm margins, a dedicated-device feeling and clear content/utility distinction.
- **Do not copy:** the clock launcher as a mandatory Home, Krono's exact list labels (Read/Notes/Music/etc.), hardware rotary control behavior, proprietary iconography or overall physical shape.
- **Design test:** Could the launcher be understood in monochrome by someone who has never used this device?

## R4 — Kobo Clara BW / actual e-reader library (FUNCTIONAL INDEX REFERENCE)

- **Official product page:** https://au.kobobooks.com/products/kobo-clara-bw
- **Official product illustration:** https://au.kobobooks.com/cdn/shop/files/1-Spa-Colour-UK-AUZ-Device_Front_1080x1080_bb6d10b7-d702-4552-9ec5-5d8bc94791b3.png?v=1757384316&width=1080
- **Additional real library photo:** https://moddedbear.com/kobo-clara-bw-review/
- **Learn:** Covers as navigable content; clear progress/status annotations; a dedicated reader's library can prioritize scanning over ornamental cards.
- **Do not copy:** shopping/sponsored suggestions, Kobo's persistent navigation model, color covers or density without adapting to 480×800.
- **Design test:** Users can search, sort and reopen an item without decoding icons or duplicate badges.

## R5 — Apple UI Design Tips / touch and clarity (CONSTRAINT REFERENCE)

- **Official:** https://developer.apple.com/design/tips/
- **Learn:** Target size, text contrast, readable labels, grouping and proximity of controls to affected content.
- **Important translation note:** Apple's 44 **points** are not 44 X4 Pro **pixels**. XOS-DL uses its own provisional 48px minimum / 56px primary regions and requires physical validation.
- **Do not copy:** iOS screenshot scale, automatic assumption of Retina density or platform-specific typography choices.

## R6 — User-supplied modular iPhone UI photograph (MOOD REFERENCE ONLY)

- **Origin:** Provided directly by Product Owner in the current conversation (not an external official design source); retain in personal conversation context, **do not automatically commit a copy**.
- **Learn:** Deliberate blank space, grouped contextual actions, hierarchy that reads in one glance.
- **Do not copy:** translucent gray cards, blurred surfaces, grid of tiny cards or unrelated task app functionality. The photo is visual inspiration, not XTEInk feature evidence.

## Explicit cross-reference rule for agent output

Each new screen must name at most two applicable reference IDs and identify one feature borrowed and one feature intentionally rejected. **A vague phrase such as 'Apple/Krono feel' is not evidence.** Explain why the screen's hierarchy/spacing follows R1–R5, then demonstrate it in the rendered 480×800 screenshot.
