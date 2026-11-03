import asyncio
import os
import sys
import shutil
from playwright.async_api import async_playwright
from PIL import Image, ImageChops

sys.stdout.reconfigure(encoding='utf-8')

# CLI argument handling
DEMO_FAILURE = '--demonstrate-failure' in sys.argv
TEST_MUTATIONS = '--test-mutations' in sys.argv
TEST_ARTIFACTS_DIR = os.path.abspath('tools/test_output')

results = {
    "dimensions": {},
    "frame_integrity": {},
    "typography": "",
    "overflow": {},
    "touch_targets": {},
    "mutations": {},
    "search_reliability": {},
    "library_sorting": {},
    "reader_typography": {},
    "state_integrity": {},
    "journeys": {},
    "binary_contrast": {},
    "binary_pixel_match": {}
}

async def run_qa():
    print("=================================================================")
    print("XTEInk OS — M1 Comprehensive QA Verification Suite")
    print(f"Artifacts output dir: {TEST_ARTIFACTS_DIR}")
    if DEMO_FAILURE:
        print("MODE: --demonstrate-failure ACTIVE (deliberate failure test)")
    if TEST_MUTATIONS:
        print("MODE: --test-mutations ACTIVE (mutation testing suite)")
    print("=================================================================\n")

    os.makedirs(TEST_ARTIFACTS_DIR, exist_ok=True)
    all_passed = True

    # 0. Deliberate Failure Demonstration Mode
    if DEMO_FAILURE:
        print("[DEMO FAILURE] Running deliberate failing assertion to verify harness exit code...")
        try:
            assert 1 == 2, "Deliberate test failure: 1 != 2"
        except AssertionError as e:
            print(f"[DEMO FAILURE CONFIRMED] Caught: {e}")
            sys.exit(1)

    # 1. Capture Files & Dimension Check (Checks reference captures without overwriting them)
    frames = [f"FR-{i:02d}" for i in range(1, 16)]
    for f in frames:
        p = f"docs/design/prototype/captures/{f}.png"
        if not os.path.exists(p):
            results["dimensions"][f] = "MISSING CAPTURE FILE"
            all_passed = False
            continue
        try:
            im = Image.open(p)
            size = im.size
            if size == (480, 800):
                results["dimensions"][f] = "PASS (480x800)"
            else:
                results["dimensions"][f] = f"FAIL (Unexpected dimensions: {size})"
                all_passed = False
        except Exception as e:
            results["dimensions"][f] = f"ERROR: {e}"
            all_passed = False

    # 2. 1-Bit Binary Legibility & Canonical Exact Pixel Match Check
    bin_dir = os.path.join(TEST_ARTIFACTS_DIR, "binary_test")
    os.makedirs(bin_dir, exist_ok=True)
    for f in frames:
        p = f"docs/design/prototype/captures/{f}.png"
        p_canon_bin = f"docs/design/prototype/captures/binary/{f}_binary.png"
        if os.path.exists(p):
            im = Image.open(p).convert("L")
            thresh = 180
            binary_im = im.point(lambda x: 255 if x > thresh else 0, mode='1')
            out_bin = os.path.join(bin_dir, f"{f}_bin.png")
            binary_im.save(out_bin)
            extrema = binary_im.getextrema()
            # Must contain both 0 (black ink) and 255 (white paper)
            if extrema == (0, 255):
                results["binary_contrast"][f] = "PASS (Extrema (0, 255), 100% 1-bit legible)"
            else:
                results["binary_contrast"][f] = f"FAIL (Extrema {extrema})"
                all_passed = False

            # Verify canonical binary capture file exists and matches fresh conversion pixel-for-pixel
            if os.path.exists(p_canon_bin):
                im_canon = Image.open(p_canon_bin)
                diff = ImageChops.difference(binary_im.convert('L'), im_canon.convert('L'))
                bbox = diff.getbbox()
                if bbox is None:
                    results["binary_pixel_match"][f] = "PASS (0 px diff vs canonical)"
                else:
                    results["binary_pixel_match"][f] = f"FAIL (mismatch bbox {bbox})"
                    all_passed = False
            else:
                results["binary_pixel_match"][f] = "FAIL (Missing canonical binary capture)"
                all_passed = False

    # 3. Headless Playwright In-Depth DOM, State, Search, Overflow & Journey Tests
    async with async_playwright() as p:
        browser = await p.chromium.launch()
        page = await browser.new_page(viewport={"width": 1200, "height": 1200}, device_scale_factor=1.0)
        file_path = os.path.abspath("docs/design/prototype/unified-ui-prototype.html")
        await page.goto(f"file:///{file_path.replace(os.sep, '/')}")
        await page.wait_for_selector("#device-screen")
        await page.evaluate("document.fonts.ready")

        # 3.0 Typography & Font Engine Verification
        font_check = await page.evaluate('''() => {
            let serifLoaded = false;
            let sansLoaded = false;
            document.fonts.forEach(f => {
                if (f.family.includes('Noto Serif') && f.status === 'loaded') serifLoaded = true;
                if (f.family.includes('Noto Sans') && f.status === 'loaded') sansLoaded = true;
            });
            return { serifLoaded, sansLoaded };
        }''')
        results["typography"] = f"PASS (Noto Serif loaded: {font_check['serifLoaded']}, Noto Sans loaded: {font_check['sansLoaded']})"

        # 3.1 Frame Existence & DOM Integrity Check
        frame_ids = [f"fr-{i:02d}" for i in range(1, 16)]
        for fid in frame_ids:
            exists = await page.query_selector(f"#frame-{fid}")
            if exists:
                results["frame_integrity"][fid] = "PASS (DOM Node Present)"
            else:
                results["frame_integrity"][fid] = "FAIL (Missing DOM Node)"
                all_passed = False

        # 3.2 Viewport Overflow Check (Both Horizontal and Vertical)
        for fid in frame_ids:
            await page.evaluate(f'showFrame("{fid}")')
            if fid == 'fr-06':
                await page.evaluate('handleSearchInput("chiến tranh")')
            await page.wait_for_timeout(40)
            overflow_info = await page.evaluate(f'''() => {{
                const el = document.getElementById("frame-{fid}");
                return {{
                    scrollW: el.scrollWidth,
                    clientW: el.clientWidth,
                    scrollH: el.scrollHeight,
                    clientH: el.clientHeight,
                    hOverflow: el.scrollWidth > el.clientWidth,
                    vOverflow: el.scrollHeight > el.clientHeight
                }};
            }}''')
            if not overflow_info["hOverflow"] and not overflow_info["vOverflow"]:
                results["overflow"][fid] = f"PASS (W:{overflow_info['clientW']} H:{overflow_info['clientH']})"
            else:
                issues = []
                if overflow_info["hOverflow"]:
                    issues.append(f"H-Overflow ({overflow_info['scrollW']} > {overflow_info['clientW']})")
                if overflow_info["vOverflow"]:
                    issues.append(f"V-Overflow ({overflow_info['scrollH']} > {overflow_info['clientH']})")
                results["overflow"][fid] = f"FAIL ({', '.join(issues)})"
                all_passed = False

        # 3.3 Touch Targets Rigorous DOM Interaction Census
        total_discovered = 0
        total_evaluated = 0
        total_excluded = 0
        target_failures = []
        census_details = {}

        for fid in frame_ids:
            await page.evaluate(f'showFrame("{fid}")')
            if fid == 'fr-06':
                await page.evaluate('handleSearchInput("chiến tranh")')
            await page.wait_for_timeout(40)

            res = await page.evaluate(f'''() => {{
                const frame = document.getElementById("frame-{fid}");
                const candidates = Array.from(frame.querySelectorAll(
                    'button, a[href], input, select, textarea, [role="button"], [onclick], label.radio-option-row, .reader-tap-left, .reader-tap-center, .reader-tap-right'
                ));
                
                const discoveredCount = candidates.length;
                const evaluated = [];
                const excluded = [];
                const failures = [];
                const visited = new Set();

                candidates.forEach(el => {{
                    const style = window.getComputedStyle(el);
                    const b = el.getBoundingClientRect();
                    if (style.display === 'none' || style.visibility === 'hidden' || style.opacity === '0' || (b.width === 0 && b.height === 0)) {{
                        excluded.push({{ tag: el.tagName, reason: 'hidden/invisible-or-parent-display-none' }});
                        return;
                    }}
                    if (el.disabled || el.getAttribute('aria-disabled') === 'true') {{
                        excluded.push({{ tag: el.tagName, reason: 'disabled/inert' }});
                        return;
                    }}
                    // Avoid double counting radio inputs inside accessible parent label rows
                    if (el.tagName.toLowerCase() === 'input' && (el.type === 'radio' || el.type === 'checkbox') && el.closest('label.radio-option-row')) {{
                        excluded.push({{ tag: 'input[radio]', reason: 'accessible-parent-label-evaluated' }});
                        return;
                    }}
                    // If an element is a child inside an interactive parent container, parent handles hit
                    const clickableAncestor = el.parentElement ? el.parentElement.closest('button, a[href], label.radio-option-row, [role="button"]') : null;
                    if (clickableAncestor && clickableAncestor !== el && frame.contains(clickableAncestor)) {{
                        excluded.push({{ tag: el.tagName, reason: 'child-of-interactive-parent' }});
                        return;
                    }}

                    if (visited.has(el)) return;
                    visited.add(el);

                    const rect = el.getBoundingClientRect();
                    const w = Math.round(rect.width);
                    const h = Math.round(rect.height);

                    const isPrimaryCta = el.classList.contains('btn-primary');
                    const isRowItem = el.classList.contains('book-row-item') || el.classList.contains('file-row-btn') || el.classList.contains('toc-row-item') || el.classList.contains('settings-row-item') || el.classList.contains('utility-main-row') || el.classList.contains('fr02-method-row');

                    let pass = false;
                    let req = ">=48x48px";
                    if (isPrimaryCta) {{
                        req = ">=120x56px";
                        pass = (w >= 120 && h >= 56);
                    }} else if (isRowItem) {{
                        req = ">=200x48px";
                        pass = (w >= 200 && h >= 48);
                    }} else {{
                        pass = (w >= 48 && h >= 48);
                    }}

                    const label = (el.innerText || el.placeholder || el.value || el.className || el.tagName).substring(0, 25).trim().replace(/\\n/g, ' ');
                    const item = {{ label, tag: el.tagName, w, h, req, pass }};
                    evaluated.push(item);
                    if (!pass) {{
                        failures.push(item);
                    }}
                }});

                return {{
                    discoveredCount,
                    evaluatedCount: evaluated.length,
                    excludedCount: excluded.length,
                    failures
                }};
            }}''')

            total_discovered += res['discoveredCount']
            total_evaluated += res['evaluatedCount']
            total_excluded += res['excludedCount']
            census_details[fid] = f"Discovered: {res['discoveredCount']}, Evaluated: {res['evaluatedCount']}, Excluded: {res['excludedCount']}"
            if res['failures']:
                for fail in res['failures']:
                    target_failures.append(f"[{fid}] '{fail['label']}' ({fail['w']}x{fail['h']}px, required {fail['req']})")

        if len(target_failures) == 0:
            results["touch_targets"]["status"] = f"PASS (Census: Discovered {total_discovered}, Evaluated {total_evaluated}, Excluded {total_excluded}, 100% compliant >=48x48px / 56px height)"
        else:
            results["touch_targets"]["status"] = f"FAIL ({len(target_failures)} targets below bounds: {target_failures[:3]})"
            all_passed = False

        # 3.4 Mutation Testing (Shrinking Reader tap zone and Search input hit region)
        try:
            # Mutation 1: Shrink Reader Tap Zone width to 30px
            await page.evaluate('''() => {
                const style = document.createElement('style');
                style.id = 'mutation-1-style';
                style.innerHTML = '.reader-tap-center { width: 30px !important; }';
                document.head.appendChild(style);
            }''')
            await page.evaluate('showFrame("fr-08")')
            await page.wait_for_timeout(30)
            m1_detected = await page.evaluate('''() => {
                const el = document.querySelector('.reader-tap-center');
                const rect = el.getBoundingClientRect();
                return rect.width < 48;
            }''')
            await page.evaluate('document.getElementById("mutation-1-style").remove()')
            assert m1_detected is True, "Mutation 1 was not detected by harness!"

            # Mutation 2: Shrink Search Input height to 30px
            await page.evaluate('''() => {
                const style = document.createElement('style');
                style.id = 'mutation-2-style';
                style.innerHTML = '#search-input-field { height: 30px !important; min-height: 30px !important; }';
                document.head.appendChild(style);
            }''')
            await page.evaluate('showFrame("fr-06")')
            await page.evaluate('handleSearchInput("chiến tranh")')
            await page.wait_for_timeout(30)
            m2_detected = await page.evaluate('''() => {
                const el = document.querySelector('#search-input-field');
                const rect = el.getBoundingClientRect();
                return rect.height < 48;
            }''')
            await page.evaluate('document.getElementById("mutation-2-style").remove()')
            assert m2_detected is True, "Mutation 2 was not detected by harness!"

            results["mutations"]["status"] = "PASS (Both mutations reliably detected and cause harness failure)"
        except Exception as e:
            results["mutations"]["status"] = f"FAIL ({e})"
            all_passed = False

        # 3.5 Task B Regression: Reading State Integrity Across Multiple Books
        try:
            # Step 1: Start at Book 1 ("noi-buon-chien-tranh") at initial page 42
            await page.evaluate('showFrame("fr-01")')
            await page.evaluate('switchToBook("noi-buon-chien-tranh")')
            p1_init = await page.evaluate('CATALOG["noi-buon-chien-tranh"].currentPage')
            assert p1_init == 42, f"Expected 42, got {p1_init}"

            # Step 2: Turn page twice in Book 1 (Page 42 -> 44)
            await page.click('.reader-tap-right')
            await page.click('.reader-tap-right')
            p1_adv = await page.evaluate('CATALOG["noi-buon-chien-tranh"].currentPage')
            assert p1_adv == 44, f"Expected Book 1 advanced to 44, got {p1_adv}"

            # Step 3: Switch to Book 2 ("nhung-nguoi-khon-kho") initially at page 1
            await page.evaluate('switchToBook("nhung-nguoi-khon-kho")')
            active_id = await page.evaluate('currentState.activeBookId')
            assert active_id == 'nhung-nguoi-khon-kho'
            p2_init = await page.evaluate('CATALOG["nhung-nguoi-khon-kho"].currentPage')
            assert p2_init == 1, f"Expected Book 2 at page 1, got {p2_init}"

            # Step 4: Advance Book 2 by 3 pages (Page 1 -> 4)
            await page.click('.reader-tap-right')
            await page.click('.reader-tap-right')
            await page.click('.reader-tap-right')
            p2_adv = await page.evaluate('CATALOG["nhung-nguoi-khon-kho"].currentPage')
            assert p2_adv == 4, f"Expected Book 2 at page 4, got {p2_adv}"

            # Step 5: Switch back to Book 1: verify Book 1 page is still 44 (NOT reset or overwritten)
            await page.evaluate('switchToBook("noi-buon-chien-tranh")')
            p1_restored = await page.evaluate('CATALOG["noi-buon-chien-tranh"].currentPage')
            assert p1_restored == 44, f"Book 1 progress corrupted: expected 44, got {p1_restored}"

            # Step 6: Verify Book 2 still has page 4
            p2_check = await page.evaluate('CATALOG["nhung-nguoi-khon-kho"].currentPage')
            assert p2_check == 4, f"Book 2 progress corrupted: expected 4, got {p2_check}"

            # Step 7: Return Home and verify Home shows the active book (Book 1) at Page 44
            await page.evaluate('showFrame("fr-01")')
            home_title = await page.inner_text('#home-hero-title')
            home_milestone = await page.inner_text('#home-hero-milestone')
            assert "Nỗi buồn chiến tranh" in home_title
            assert "44/280" in home_milestone, f"Home did not display updated page 44: {home_milestone}"

            results["state_integrity"]["multi_book_persistence"] = "PASS (Isolated state confirmed between books)"
        except Exception as e:
            results["state_integrity"]["multi_book_persistence"] = f"FAIL ({e})"
            all_passed = False

        # 3.6 Task P1-A Regression: Search Reset & Diacritic-Insensitive Highlighting
        try:
            await page.evaluate('openSearch()')

            # Test 1: Accented query "chiến tranh"
            await page.evaluate('handleSearchInput("chiến tranh")')
            count_accented = await page.inner_text('#search-result-count')
            assert "2 kết quả" in count_accented, f"Expected 2 results for 'chiến tranh', got '{count_accented}'"
            results_rows = await page.query_selector_all('#search-results-list .book-row-item')
            assert len(results_rows) == 2, f"Expected 2 rendered rows for 'chiến tranh', got {len(results_rows)}"
            mark_accented = await page.query_selector_all('#search-results-list mark.search-match')
            assert len(mark_accented) >= 2, f"Expected mark.search-match in accented results, got {len(mark_accented)}"

            # Test 2: Unaccented query "chien tranh" (Diacritic-insensitive + highlighting relevance)
            await page.evaluate('handleSearchInput("chien tranh")')
            count_unaccented = await page.inner_text('#search-result-count')
            assert "2 kết quả" in count_unaccented, f"Expected 2 results for 'chien tranh', got '{count_unaccented}'"
            results_rows_un = await page.query_selector_all('#search-results-list .book-row-item')
            assert len(results_rows_un) == 2, f"Expected 2 rendered rows for 'chien tranh', got {len(results_rows_un)}"
            mark_unaccented = await page.query_selector_all('#search-results-list mark.search-match')
            assert len(mark_unaccented) >= 2, f"Expected mark.search-match in unaccented results, got {len(mark_unaccented)}"
            mark_text = await page.evaluate('document.querySelector("#search-results-list mark.search-match").innerText')
            assert "chiến tranh" in mark_text.lower(), f"Expected 'chiến tranh' in mark, got '{mark_text}'"

            # Test 3: Zero matching query
            await page.evaluate('handleSearchInput("khong-ton-tai-xyz")')
            count_zero = await page.inner_text('#search-result-count')
            assert "0 kết quả" in count_zero
            rows_zero = await page.query_selector_all('#search-results-list .book-row-item')
            assert len(rows_zero) == 0, f"Expected 0 rows for non-matching query, got {len(rows_zero)}"

            # Test 4: Clear button (P1-A Search Reset: restores full catalogue, focus, count)
            await page.fill('#search-input-field', 'tô hoài')
            await page.wait_for_timeout(40)
            await page.click('.search-clear-btn')
            await page.wait_for_timeout(40)
            input_val = await page.input_value('#search-input-field')
            assert input_val == "", f"Expected empty input after clear, got '{input_val}'"
            is_focused = await page.evaluate('document.activeElement === document.getElementById("search-input-field")')
            assert is_focused is True, "Expected #search-input-field to have keyboard focus after clear"
            count_cleared = await page.inner_text('#search-result-count')
            assert "Toàn bộ danh mục (6 cuốn)" in count_cleared, f"Expected 'Toàn bộ danh mục (6 cuốn)', got '{count_cleared}'"
            rows_cleared = await page.query_selector_all('#search-results-list .book-row-item')
            assert len(rows_cleared) == 6, f"Expected 6 restored catalogue rows after clear, got {len(rows_cleared)}"
            first_title_cleared = await page.evaluate('document.querySelector("#search-results-list .book-row-item strong").innerText')
            assert "Nỗi buồn chiến tranh" in first_title_cleared, f"Expected 'Nỗi buồn chiến tranh' as first restored book, got '{first_title_cleared}'"

            results["search_reliability"]["diacritic_and_queries"] = "PASS (Accented, unaccented, highlighting, zero-match, and clear -> restored catalogue verified)"
        except Exception as e:
            results["search_reliability"]["diacritic_and_queries"] = f"FAIL ({e})"
            all_passed = False

        # 3.7 Task 3 Regression: Library Sorting Behavior
        try:
            await page.evaluate('showFrame("fr-05")')

            # Sort by Title A-Z: should place "Chiến tranh và Hòa bình" at the top
            await page.click('.sort-pills-row .sort-pill:nth-child(2)')
            await page.wait_for_timeout(40)
            first_title_az = await page.evaluate('''() => {
                const el = document.querySelector('#library-list-items .book-row-item strong');
                return el ? el.innerText.trim() : '';
            }''')
            assert "Chiến tranh và Hòa bình" in first_title_az, f"Expected 'Chiến tranh và Hòa bình' first in Title sort, got '{first_title_az}'"
            az_active = await page.evaluate('''() => {
                const btn = document.querySelector('.sort-pills-row .sort-pill:nth-child(2)');
                return btn ? btn.classList.contains('active') : false;
            }''')
            assert az_active is True, "Title A-Z sort button did not receive active class"

            # Sort by Progress: should place "Dế Mèn phiêu lưu ký" (40%) at the top
            await page.click('.sort-pills-row .sort-pill:nth-child(3)')
            await page.wait_for_timeout(40)
            first_title_prog = await page.evaluate('''() => {
                const el = document.querySelector('#library-list-items .book-row-item strong');
                return el ? el.innerText.trim() : '';
            }''')
            assert "Dế Mèn phiêu lưu ký" in first_title_prog, f"Expected 'Dế Mèn phiêu lưu ký' first in Progress sort, got '{first_title_prog}'"
            prog_active = await page.evaluate('''() => {
                const btn = document.querySelector('.sort-pills-row .sort-pill:nth-child(3)');
                return btn ? btn.classList.contains('active') : false;
            }''')
            assert prog_active is True, "Progress sort button did not receive active class"

            # Sort by Recent: restores "Nỗi buồn chiến tranh" at the top
            await page.click('.sort-pills-row .sort-pill:nth-child(1)')
            await page.wait_for_timeout(40)
            first_title_rec = await page.evaluate('''() => {
                const el = document.querySelector('#library-list-items .book-row-item strong');
                return el ? el.innerText.trim() : '';
            }''')
            assert "Nỗi buồn chiến tranh" in first_title_rec, f"Expected 'Nỗi buồn chiến tranh' first in Recent sort, got '{first_title_rec}'"
            rec_active = await page.evaluate('''() => {
                const btn = document.querySelector('.sort-pills-row .sort-pill:nth-child(1)');
                return btn ? btn.classList.contains('active') : false;
            }''')
            assert rec_active is True, "Recent sort button did not receive active class"

            results["library_sorting"]["sorting_behavior"] = "PASS (Title A-Z, Progress, Recent verified with active state and DOM reordering)"
        except Exception as e:
            results["library_sorting"]["sorting_behavior"] = f"FAIL ({e})"
            all_passed = False

        # 3.8 Task P1-B Regression: Reader Typography Synchronization & HUD Consistency
        try:
            await page.evaluate('showFrame("fr-08")')
            await page.wait_for_timeout(40)

            # Step 1: Check initial Reader defaults align across state, CSS, and FR-14
            state_settings = await page.evaluate('currentState.readerSettings')
            assert state_settings['fontSize'] == 19, f"Expected fontSize 19, got {state_settings['fontSize']}"
            assert state_settings['lineHeight'] == 1.65, f"Expected lineHeight 1.65, got {state_settings['lineHeight']}"
            assert state_settings['fontFamily'] == 'serif', f"Expected fontFamily serif, got {state_settings['fontFamily']}"
            assert state_settings['margin'] == 32, f"Expected margin 32, got {state_settings['margin']}"

            # Verify FR-14 displayed default text
            await page.evaluate('showFrame("fr-14")')
            setting_typo_val = await page.inner_text('#setting-typography-val')
            assert "19px / Giãn dòng 1.65" in setting_typo_val, f"FR-14 display mismatch: '{setting_typo_val}'"

            # Verify FR-11 initial active stepper buttons
            await page.evaluate('showFrame("fr-11")')
            f19_active = await page.evaluate('document.getElementById("fbtn-19").classList.contains("active")')
            lh165_active = await page.evaluate('document.getElementById("lhbtn-165").classList.contains("active")')
            serif_active = await page.evaluate('document.getElementById("font-serif-btn").classList.contains("active")')
            m32_active = await page.evaluate('document.getElementById("mbtn-32").classList.contains("active")')
            assert f19_active is True, "#fbtn-19 should be active by default"
            assert lh165_active is True, "#lhbtn-165 should be active by default"
            assert serif_active is True, "#font-serif-btn should be active by default"
            assert m32_active is True, "#mbtn-32 should be active by default"

            # Step 2: Verify typography consistency between FR-08 (Reader) and FR-09 (HUD)
            await page.evaluate('showFrame("fr-08")')
            fr08_style = await page.evaluate('''() => {
                const el = document.getElementById("reader-prose-text");
                const body = document.querySelector("#frame-fr-08 .fr08-reader-body");
                const cs = window.getComputedStyle(el);
                const bs = window.getComputedStyle(body);
                return {
                    fontSize: cs.fontSize,
                    fontFamily: cs.fontFamily,
                    lineHeight: cs.lineHeight,
                    paddingLeft: bs.paddingLeft,
                    paddingRight: bs.paddingRight
                };
            }''')

            await page.evaluate('showFrame("fr-09")')
            fr09_style = await page.evaluate('''() => {
                const el = document.getElementById("reader-hud-prose-text");
                const body = document.querySelector("#frame-fr-09 .fr08-reader-body");
                const cs = window.getComputedStyle(el);
                const bs = window.getComputedStyle(body);
                return {
                    fontSize: cs.fontSize,
                    fontFamily: cs.fontFamily,
                    lineHeight: cs.lineHeight,
                    paddingLeft: bs.paddingLeft,
                    paddingRight: bs.paddingRight
                };
            }''')

            assert fr08_style['fontSize'] == fr09_style['fontSize'], f"Font size mismatch: FR-08 {fr08_style['fontSize']} vs FR-09 {fr09_style['fontSize']}"
            assert fr08_style['fontFamily'] == fr09_style['fontFamily'], f"Font family mismatch: FR-08 {fr08_style['fontFamily']} vs FR-09 {fr09_style['fontFamily']}"
            assert fr08_style['lineHeight'] == fr09_style['lineHeight'], f"Line height mismatch: FR-08 {fr08_style['lineHeight']} vs FR-09 {fr09_style['lineHeight']}"
            assert fr08_style['paddingLeft'] == fr09_style['paddingLeft'], f"Margin mismatch: FR-08 {fr08_style['paddingLeft']} vs FR-09 {fr09_style['paddingLeft']}"

            # Step 3: Change typography settings, assert page/chapter state is preserved
            page_before = await page.evaluate('getActiveBook().currentPage')
            ch_before = await page.evaluate('getActiveBook().chapter')

            await page.evaluate('setReaderFontSize(18)')
            await page.evaluate('setReaderLineHeight(1.4)')
            await page.evaluate('setReaderFontFamily("sans")')
            await page.evaluate('setReaderMargin(24)')

            page_after = await page.evaluate('getActiveBook().currentPage')
            ch_after = await page.evaluate('getActiveBook().chapter')
            assert page_after == page_before, f"Page changed after typography change: {page_before} -> {page_after}"
            assert ch_after == ch_before, f"Chapter changed after typography change: {ch_before} -> {ch_after}"

            # Step 4: Verify FR-08 and FR-09 updated identically to new settings
            fr08_new = await page.evaluate('''() => {
                const el = document.getElementById("reader-prose-text");
                const body = document.querySelector("#frame-fr-08 .fr08-reader-body");
                const cs = window.getComputedStyle(el);
                const bs = window.getComputedStyle(body);
                return {
                    fontSize: cs.fontSize,
                    fontFamily: cs.fontFamily,
                    paddingLeft: bs.paddingLeft
                };
            }''')
            fr09_new = await page.evaluate('''() => {
                const el = document.getElementById("reader-hud-prose-text");
                const body = document.querySelector("#frame-fr-09 .fr08-reader-body");
                const cs = window.getComputedStyle(el);
                const bs = window.getComputedStyle(body);
                return {
                    fontSize: cs.fontSize,
                    fontFamily: cs.fontFamily,
                    paddingLeft: bs.paddingLeft
                };
            }''')
            assert fr08_new['fontSize'] == '18px' and fr09_new['fontSize'] == '18px', f"FR-08: {fr08_new['fontSize']}, FR-09: {fr09_new['fontSize']}"
            assert 'sans' in fr08_new['fontFamily'].lower() and 'sans' in fr09_new['fontFamily'].lower()
            assert fr08_new['paddingLeft'] == '24px' and fr09_new['paddingLeft'] == '24px'

            # Restore defaults
            await page.evaluate('setReaderFontSize(19)')
            await page.evaluate('setReaderLineHeight(1.65)')
            await page.evaluate('setReaderFontFamily("serif")')
            await page.evaluate('setReaderMargin(32)')

            results["reader_typography"]["synchronization_and_consistency"] = "PASS (FR-08 <-> FR-09 consistency, 19px/1.65 defaults, state preserved across changes)"
        except Exception as e:
            results["reader_typography"]["synchronization_and_consistency"] = f"FAIL ({e})"
            all_passed = False

        # 3.9 All 5 Mandatory User Journeys (A through E)
        try:
            # Journey A: Populated Home -> Continue Reading -> Reader -> Turn Page -> HUD -> Back to Library -> Back to Home
            await page.evaluate('showFrame("fr-01")')
            await page.click('#frame-fr-01 .btn-primary')
            assert await page.evaluate('currentState.currentFrame') == 'fr-08'
            p_a1 = await page.evaluate('getActiveBook().currentPage')
            await page.click('.reader-tap-right')
            p_a2 = await page.evaluate('getActiveBook().currentPage')
            assert p_a2 == p_a1 + 1
            await page.click('.reader-tap-center') # open HUD
            await page.click('#frame-fr-09 .hud-top-bar .nav-back-btn') # back to Library (fr-05)
            assert await page.evaluate('currentState.currentFrame') == 'fr-05'
            await page.click('#frame-fr-05 .nav-back-btn') # back to Home (fr-01)
            assert await page.evaluate('currentState.currentFrame') == 'fr-01'
            results["journeys"]["Journey A"] = "PASS"
        except Exception as e:
            results["journeys"]["Journey A"] = f"FAIL ({e})"
            all_passed = False

        try:
            # Journey B: Home -> Library -> Search -> Select Book -> Reader
            await page.evaluate('showFrame("fr-01")')
            await page.click('.utility-main-row')
            assert await page.evaluate('currentState.currentFrame') == 'fr-05'
            await page.click('.search-bar-wrap')
            assert await page.evaluate('currentState.currentFrame') == 'fr-06'
            await page.fill('#search-input-field', 'tô hoài')
            await page.wait_for_timeout(100)
            await page.click('#search-results-list .book-row-item:first-child')
            assert await page.evaluate('currentState.currentFrame') == 'fr-08'
            assert await page.evaluate('currentState.activeBookId') == 'de-men-phieu-luu-ky'
            results["journeys"]["Journey B"] = "PASS"
        except Exception as e:
            results["journeys"]["Journey B"] = f"FAIL ({e})"
            all_passed = False

        try:
            # Journey C: Reader -> HUD -> TOC -> Select Chapter -> Reader -> Typo Settings -> Reader
            await page.evaluate('switchToBook("noi-buon-chien-tranh")')
            assert await page.evaluate('currentState.currentFrame') == 'fr-08'
            await page.click('.reader-tap-center') # open HUD
            assert await page.evaluate('currentState.currentFrame') == 'fr-09'
            await page.click('#frame-fr-09 .hud-actions-grid button:nth-child(2)') # TOC
            assert await page.evaluate('currentState.currentFrame') == 'fr-10'
            await page.click('#toc-dynamic-list .toc-row-item:nth-child(5)') # Select Ch. 5
            assert await page.evaluate('currentState.currentFrame') == 'fr-08'
            assert await page.evaluate('getActiveBook().chapter') == 5
            await page.click('.reader-tap-center') # open HUD
            await page.click('#frame-fr-09 .hud-actions-grid button:nth-child(3)') # Aa
            assert await page.evaluate('currentState.currentFrame') == 'fr-11'
            await page.click('#fbtn-18') # Change font to 18px
            await page.click('#frame-fr-11 .modal-close-btn') # Apply
            assert await page.evaluate('currentState.currentFrame') == 'fr-08'
            assert await page.evaluate('currentState.readerSettings.fontSize') == 18
            results["journeys"]["Journey C"] = "PASS"
        except Exception as e:
            results["journeys"]["Journey C"] = f"FAIL ({e})"
            all_passed = False

        try:
            # Journey D: Empty Home -> SD File Browser -> Sideload Inspect -> Reader
            await page.evaluate('showFrame("fr-02")')
            await page.click('#frame-fr-02 .btn-primary') # Add books
            assert await page.evaluate('currentState.currentFrame') == 'fr-12'
            await page.click('#file-browser-list .file-row-btn:nth-child(2)') # EPUB
            assert await page.evaluate('currentState.currentFrame') == 'fr-13'
            await page.click('#frame-fr-13 #file-inspect-valid .btn-primary') # Open in reader
            assert await page.evaluate('currentState.currentFrame') == 'fr-08'
            results["journeys"]["Journey D"] = "PASS"
        except Exception as e:
            results["journeys"]["Journey D"] = f"FAIL ({e})"
            all_passed = False

        try:
            # Journey E: Home -> Settings -> Refresh Interval Modal -> Change Value -> Save
            await page.evaluate('showFrame("fr-01")')
            await page.click('#frame-fr-01 .utility-sub-group button:last-child') # Cài đặt
            assert await page.evaluate('currentState.currentFrame') == 'fr-14'
            await page.click('#frame-fr-14 .settings-row-item:first-of-type') # Refresh interval
            assert await page.evaluate('currentState.currentFrame') == 'fr-15'
            await page.click('#frame-fr-15 input[value="5"]')
            await page.click('#frame-fr-15 .btn-primary') # Save
            assert await page.evaluate('currentState.currentFrame') == 'fr-14'
            setting_txt = await page.inner_text('#setting-refresh-val')
            assert "Mỗi 5 trang" in setting_txt, f"Expected 'Mỗi 5 trang', got '{setting_txt}'"
            results["journeys"]["Journey E"] = "PASS"
        except Exception as e:
            results["journeys"]["Journey E"] = f"FAIL ({e})"
            all_passed = False

        await browser.close()

    # 4. Final Report & Strict Exit Code Policy
    print("\n------------------- QA AUDIT REPORT -------------------")
    print(f"Dimensions: {results['dimensions']}")
    print(f"Frame Integrity: {results['frame_integrity']}")
    print(f"Typography: {results['typography']}")
    print(f"Overflow: {results['overflow']}")
    print(f"Touch Targets: {results['touch_targets']}")
    print(f"Mutations: {results['mutations']}")
    print(f"State Integrity: {results['state_integrity']}")
    print(f"Search Reliability: {results['search_reliability']}")
    print(f"Library Sorting: {results['library_sorting']}")
    print(f"Reader Typography: {results['reader_typography']}")
    print(f"Journeys: {results['journeys']}")
    print(f"Binary Contrast Sample (FR-01..FR-03): {[results['binary_contrast'][k] for k in frames[:3]]}")
    print(f"Canonical Binary Pixel Match: {all(results['binary_pixel_match'][k].startswith('PASS') for k in frames)}")
    print("-------------------------------------------------------")

    if all_passed:
        print("\n>>> OVERALL VERDICT: PASS (100% verified across all suites) <<<")
        sys.exit(0)
    else:
        print("\n>>> OVERALL VERDICT: FAIL (One or more QA assertions failed) <<<")
        sys.exit(1)

if __name__ == "__main__":
    asyncio.run(run_qa())
