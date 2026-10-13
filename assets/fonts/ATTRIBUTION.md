# Bundled UI font assets

Portsmouth Rapids is the repository owner's own typeface. GUI.Forms bundles
two Portsmouth Rapids 1.0 faces and uses them for all control and title roles:

- `PortsmouthRapids.ttf` — regular, SHA-256
  `88988bea222852c30e08a3629d9f929baacfec4844c85bb978dcc68e6f4d4add`
- `PortsmouthRapids-Bold.ttf` — bold, SHA-256
  `f97d702778f5933b4ae138064a348499a6dba90d9e2e6bf75a6ab81a6730c03a`

Licence: MIT, copyright the repository owner; the exact text is retained as
`LICENSE-PortsmouthRapids.txt`. Embedded metadata identifies the family as
`Portsmouth Rapids`, version 1.0. The current faces were supplied 2026-08-06
with the interface-symbol rebuild. Full source provenance is retained in the
owner's Portsmouth source project (`portsmouth/PROJECT.md`).

The 2026-08-06 faces add the protected `← ↑ → ▼` navigation/disclosure set
requested by GUI.Forms while retaining the previous 330 cmap entries, glyph
IDs, advances, and outlines exactly. The editable construction and acceptance
reports live in the cited Portsmouth source project.

## The rest of the Portsmouth 1.0 family

On 2026-10-13 the owner asked for every Portsmouth face to be included and
confirmed that the MIT grant in `LICENSE-PortsmouthRapids.txt` covers them
all. They come from the owner's published 1.0 downloads
(`Portsmouth-Family-1.0.zip`, `Portsmouth-Rapids-Family-1.0.zip` and
`Portsmouth-Mono-1.0.zip` in `falseywinchnet/paymenottowork` at `3dbc684`,
`public/instruments/portsmouth/assets/downloads/`), built on 2026-08-06 in the
same run as the two faces above, which are byte-identical to that release.
Every face embeds version 1.0 and the same 349-entry cmap.

| File | Family / style | SHA-256 |
|---|---|---|
| `PortsmouthRapids-Light.ttf` | Portsmouth Rapids Light | `1afe07a8d89be75a5a9fb5befca4938009422e6719a0d6d7b117d95d8345e61a` |
| `PortsmouthRapids-Italic.ttf` | Portsmouth Rapids Italic | `cd806b808e80df27b16694295ad06066c7479c6f0699e22b3c1e3cf04ec9e375` |
| `PortsmouthRapids-BoldItalic.ttf` | Portsmouth Rapids Bold Italic | `00ab54b1cc0c46c0706bb51653d8a96c3c5a56e23c2030826b0918ff78a387d1` |
| `Portsmouth.ttf` | Portsmouth Regular | `c3dcc717d26c7f42c83d87550a1b2f4b4c3b51d0d776d780bdd883018ead0f81` |
| `Portsmouth-Light.ttf` | Portsmouth Light | `8e78f093987c736dc32376dd9b112408855b31701ac878bf21650c1d72aa63dc` |
| `Portsmouth-Italic.ttf` | Portsmouth Italic | `0781e11d005c75b6942653b07ad65c2e05d50ab901ee2919b5038cb176a17682` |
| `Portsmouth-Bold.ttf` | Portsmouth Bold | `e709c37667842e2aae50af7e6a000ed4eabee378fc543c35140e49af176d6800` |
| `Portsmouth-BoldItalic.ttf` | Portsmouth Bold Italic | `7cdfaea4929b298ed01e40ac64e34c440436026195939b7897ec3828726c3ea9` |
| `PortsmouthMono.ttf` | Portsmouth Mono Regular (fixed pitch, 575 units) | `6129c819de5046e4ec97c7ee8d915bf195d106b98680d9b17901741a1d7314ad` |

The release's `Portsmouth-Regular-1.0.ttf` (an earlier build under the
separate family name "Portsmouth Regular") and its `.ttc` collections are not
copied; the individual faces above carry the same styles.

These faces are installed with the SDK's font directory. The hosts register
only the two faces above for `FontRole::control`; the others are available to
applications and to font substitution (Portsmouth Mono is the planned
substitute for Consolas, `planning/stages/F-font-substitution/`).

The font-pack specimen also contains Carlito and Cousine at upstream commit
and byte identity:

- Carlito commit `3a810cab78ebd6e2e4eed42af9e8453c4f9b850a`; regular
  `f6418f708baede9789daef5d458c0f53d2a888af9820e8062934e504fedc6595`, bold
  `bb5d20f79b82599ec72983597437373a80f2d2085fa91fc144fd74e876a594db`, italic
  `0b019225e58d702bfedcbd35c21696769f8ee115cb6343f84c2f240312450d1c`, and
  bold italic `b32928186c119599e03ca6a1ffc680fdcb7fac95772f4b95d989cf6cd3861517`.
- Cousine commit `c0fbdb438443968c884a5c13f5f9bee916a7f89b`; regular
  `5a57f0184000371cb22fe3fcea4c500354cab1a69efaf7beaf3e6eca6ecfefea`, bold
  `331215ec6445f41e98d8971251bb6237e722907f4101faae990583accfe79545`, italic
  `afb869eee8643d09915c3286edb61dad66278ceb1511c79b474ce21548421bcd`, and
  bold italic `7f804549c941ceaac0a9635389ed09c4bbc86e4bfe37589191dacd1b5b0e26a5`.

Both families are licensed under SIL OFL 1.1; exact license copies are retained
as `OFL-Carlito.txt` and `OFL-Cousine.txt`. Carlito is the declared lead body
**CANDIDATE** from the consumer typography plan, and Cousine is a bounded
monospace specimen. Their use here measures the bundled-font pipeline; it does
not silently elevate either candidate into the final M9 typography decision.

Two bounded, role-independent fallback faces complete the mixed-script
Gallery specimen:

- Noto Sans CJK JP Regular from `notofonts/noto-cjk` commit
  `f8d157532fbfaeda587e826d4cd5b21a49186f7c`, file
  `Sans/OTF/Japanese/NotoSansCJKjp-Regular.otf`, SHA-256
  `68a3fc98800b2a27b371f2fb79991daf3633bd89309d4ffaa6946fd587f375b5`.
- Noto Emoji Regular 1.05 monochrome from `googlefonts/noto-emoji` commit
  `9a5261d871451f9b5183c93483cbd68ed916b1e9`, file
  `fonts/NotoEmoji-Regular.ttf`, SHA-256
  `415dc6290378574135b64c808dc640c1df7531973290c4970c51fdeb849cb0c5`.

Both are licensed under SIL OFL 1.1. Exact upstream license copies are retained
as `OFL-NotoSansCJKjp.txt` and `OFL-NotoEmoji.txt`, each SHA-256
`6a73f9541c2de74158c0e7cf6b0a58ef774f5a780bf191f2d7ec9cc53efe2bf2`.
The files are reproducibly acquired and verified by
`third_party/fetch_showcase_fallback_fonts.sh`. They are **DECIDED** only for
the Gallery font pack and deterministic cross-script fallback proof;
File Manager's final locale/font-pack policy remains an M9 decision.

**REJECTED for this CPU renderer:** current COLRv1 Noto Emoji shaped correctly
but painted no pixels in live Skia/FreeType dogfood. The older upstream
monochrome face is deliberately bounded to fallback proof until GUI.Drawing
admits and measures a color-font raster policy.

Noto Sans Arabic, Hebrew, Devanagari, Bengali and Gurmukhi are unmodified Noto Project font files under SIL OFL 1.1. See OFL-Noto.txt and language-font-provenance.json for source URLs and byte hashes.
