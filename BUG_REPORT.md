-- 08/11/2026 --
1. underground waterway: at the last stage char stuck and wont move
2. dark forest loading screen still old one
3. when changed from base class to first class, the skill that level has already passed, the first class skill still unlearned
4. result screen ESC flow still a mess
5. quest char dialogue sound leaking issue

-- 11/11/2026: all five fixed, waiting for play-test (details: MODS.md "BUG_REPORT 2026-11-08, items 1-5") --
1. NPCTemplet row of the boss took June's script (data037 / data006)
2. June's Dark Forest + four Altera loading screens staged (data059 / data062)
3. job change (every advancement) and transcendence push the learned skills at once
4 + 5. one cause: a skipped quest scene stayed alive, ended "the scene" on every ESC and played its voice lines; its calls are dropped now
