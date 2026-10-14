| tab | ladder | direction | at (fixed) | last px of old state | first px of new state | logical | from | to | what changed |
|---|---|---|---|---|---|---|---|---|---|
| Home | width | shrinking | 1200 | 1343 | 1342 | 671.5 | S1 | S2 | Clipboard: Cut 83x44->44x44, Copy 98x44->44x44 |
| Home | width | shrinking | 1200 | 1289 | 1288 | 644.5 | S2 | S3 | Insert collapses to its button |
| Home | width | shrinking | 1200 | 1093 | 1092 | 546.5 | S3 | S4 | Editing collapses to its button |
| Home | width | shrinking | 1200 | 1026 | 1025 | 513 | S4 | S5 | Paragraph collapses to its button |
| Home | width | shrinking | 1200 | 874 | 873 | 437 | S5 | S6 | Clipboard collapses to its button |
| Home | width | shrinking | 1200 | 717 | 716 | 358.5 | S6 | S7 | Font collapses to its button |
| Home | width | shrinking | 1200 | 480 | 479 | 240 | S7 | S8 | scroll buttons appear |
| Home | width | growing | 1200 | 498 | 499 | 249 | S8 | S7 | scroll buttons disappear |
| Home | width | growing | 1200 | 716 | 717 | 358 | S7 | S6 | Font expands from its button |
| Home | width | growing | 1200 | 873 | 874 | 436.5 | S6 | S5 | Clipboard expands from its button |
| Home | width | growing | 1200 | 1025 | 1026 | 512.5 | S5 | S4 | Paragraph expands from its button |
| Home | width | growing | 1200 | 1092 | 1093 | 546 | S4 | S3 | Editing expands from its button |
| Home | width | growing | 1200 | 1288 | 1289 | 644 | S3 | S2 | Insert expands from its button |
| Home | width | growing | 1200 | 1342 | 1343 | 671 | S2 | S1 | Clipboard: Cut 44x44->83x44, Copy 44x44->98x44 |
| View | width | shrinking | 1200 | 711 | 710 | 355.5 | S9 | S10 | Zoom: Zoom in 84x136->128x44, Zoom out 84x136->142x44, 100% 84x136->101x44 |
| View | width | shrinking | 1200 | 601 | 600 | 300.5 | S10 | S11 | Zoom collapses to its button |
| View | width | shrinking | 1200 | 525 | 524 | 262.5 | S11 | S12 | Show or hide collapses to its button |
| View | width | shrinking | 1200 | 449 | 448 | 224.5 | S12 | S13 | Settings collapses to its button |
| View | width | growing | 1200 | 448 | 449 | 224 | S13 | S12 | Settings expands from its button |
| View | width | growing | 1200 | 524 | 525 | 262 | S12 | S11 | Show or hide expands from its button |
| View | width | growing | 1200 | 600 | 601 | 300 | S11 | S10 | Zoom expands from its button |
| View | width | growing | 1200 | 710 | 711 | 355 | S10 | S9 | Zoom: Zoom in 128x44->84x136, Zoom out 142x44->84x136, 100% 101x44->84x136 |

States (signature: group{item:widthxheight} in physical pixels):

- **S1** (width shrinking client 2760x1200): `Clipboard{Paste:84x136;Cut:83x44;Copy:98x44} | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph{Decrease indent:46x44;Increase indent:44x44;Lists:70x44;Change line spacing:64x44;Align text left:46x44;Align text center:48x44;Align text right:48x44;Justify text:44x44;Paragraph:48x44} | Insert{Picture:84x136;Date and time:94x136;Insert object:84x136} | Editing{Find:90x44;Replace:123x44;Select all:133x44}`
- **S2** (width shrinking client 1320x1200): `Clipboard{Paste:84x136;Cut:44x44;Copy:44x44} | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph{Decrease indent:46x44;Increase indent:44x44;Lists:70x44;Change line spacing:64x44;Align text left:46x44;Align text center:48x44;Align text right:48x44;Justify text:44x44;Paragraph:48x44} | Insert{Picture:84x136;Date and time:94x136;Insert object:84x136} | Editing{Find:90x44;Replace:123x44;Select all:133x44}`
- **S3** (width shrinking client 1280x1200): `Clipboard{Paste:84x136;Cut:44x44;Copy:44x44} | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph{Decrease indent:46x44;Increase indent:44x44;Lists:70x44;Change line spacing:64x44;Align text left:46x44;Align text center:48x44;Align text right:48x44;Justify text:44x44;Paragraph:48x44} | Insert[collapsed] | Editing{Find:90x44;Replace:123x44;Select all:133x44}`
- **S4** (width shrinking client 1080x1200): `Clipboard{Paste:84x136;Cut:44x44;Copy:44x44} | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph{Decrease indent:46x44;Increase indent:44x44;Lists:70x44;Change line spacing:64x44;Align text left:46x44;Align text center:48x44;Align text right:48x44;Justify text:44x44;Paragraph:48x44} | Insert[collapsed] | Editing[collapsed]`
- **S5** (width shrinking client 1000x1200): `Clipboard{Paste:84x136;Cut:44x44;Copy:44x44} | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph[collapsed] | Insert[collapsed] | Editing[collapsed]`
- **S6** (width shrinking client 840x1200): `Clipboard[collapsed] | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph[collapsed] | Insert[collapsed] | Editing[collapsed]`
- **S7** (width shrinking client 680x1200): `Clipboard[collapsed] | Font[collapsed] | Paragraph[collapsed] | Insert[collapsed] | Editing[collapsed]`
- **S8** (width shrinking client 440x1200): `Clipboard[collapsed] | Font[collapsed] | Paragraph[collapsed] | Insert[collapsed] | Editing[collapsed] | SCROLL`
- **S9** (width shrinking client 2760x1200): `Zoom{Zoom in:84x136;Zoom out:84x136;100%:84x136} | Show or hide{Ruler:98x44;Status bar:142x44} | Settings{Word wrap:169x44;Measurement units:247x44}`
- **S10** (width shrinking client 680x1200): `Zoom{Zoom in:128x44;Zoom out:142x44;100%:101x44} | Show or hide{Ruler:98x44;Status bar:142x44} | Settings{Word wrap:169x44;Measurement units:247x44}`
- **S11** (width shrinking client 600x1200): `Zoom[collapsed] | Show or hide{Ruler:98x44;Status bar:142x44} | Settings{Word wrap:169x44;Measurement units:247x44}`
- **S12** (width shrinking client 520x1200): `Zoom[collapsed] | Show or hide[collapsed] | Settings{Word wrap:169x44;Measurement units:247x44}`
- **S13** (width shrinking client 440x1200): `Zoom[collapsed] | Show or hide[collapsed] | Settings[collapsed]`
