import asyncio
import os
from playwright.async_api import async_playwright
from PIL import Image

async def main():
    async with async_playwright() as p:
        browser = await p.chromium.launch()
        page = await browser.new_page(viewport={'width': 1200, 'height': 1200}, device_scale_factor=1.0)
        file_path = os.path.abspath('docs/design/prototype/unified-ui-prototype.html')
        await page.goto(f'file:///{file_path.replace(os.sep, "/")}')
        await page.wait_for_selector('#device-screen')
        
        # Ensure genuine TrueType fonts are completely loaded before capturing
        await page.evaluate('document.fonts.ready')
        await page.wait_for_timeout(300)
        
        frames = [
            ('fr-01', 'FR-01'), ('fr-02', 'FR-02'), ('fr-03', 'FR-03'),
            ('fr-04', 'FR-04'), ('fr-05', 'FR-05'), ('fr-06', 'FR-06'),
            ('fr-07', 'FR-07'), ('fr-08', 'FR-08'), ('fr-09', 'FR-09'),
            ('fr-10', 'FR-10'), ('fr-11', 'FR-11'), ('fr-12', 'FR-12'),
            ('fr-13', 'FR-13'), ('fr-14', 'FR-14'), ('fr-15', 'FR-15')
        ]
        
        os.makedirs('docs/design/prototype/captures', exist_ok=True)
        os.makedirs('docs/design/prototype/captures/binary', exist_ok=True)
        
        elem = await page.query_selector('#device-screen')
        box = await elem.bounding_box()
        
        for fid, fname in frames:
            await page.evaluate(f'showFrame("{fid}")')
            if fid == 'fr-06':
                await page.evaluate('handleSearchInput("chiến tranh")')
            await page.evaluate('document.fonts.ready')
            await page.wait_for_timeout(200)
            
            out_path = f'docs/design/prototype/captures/{fname}.png'
            await page.screenshot(
                path=out_path,
                clip={'x': round(box['x']), 'y': round(box['y']), 'width': 480, 'height': 800}
            )
            im = Image.open(out_path)
            print(f'{fname}: {im.size} saved to {out_path}')
            assert im.size == (480, 800), f'{fname} size mismatch: {im.size}'

            # Generate 1-bit canonical binary representation (Threshold 180)
            im_l = im.convert('L')
            binary_im = im_l.point(lambda x: 255 if x > 180 else 0, mode='1')
            bin_path = f'docs/design/prototype/captures/binary/{fname}_binary.png'
            binary_im.save(bin_path)
            print(f'{fname}_binary: saved to {bin_path}')
            
        await browser.close()
        print('All 15 frames & canonical 1-bit binary images regenerated with genuine Noto Serif/Sans typography!')

if __name__ == '__main__':
    asyncio.run(main())
