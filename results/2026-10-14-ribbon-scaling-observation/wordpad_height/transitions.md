| tab | ladder | direction | at (fixed) | last px of old state | first px of new state | logical | from | to | what changed |
|---|---|---|---|---|---|---|---|---|---|
| Home | height | shrinking | 1600 | 238 | 237 | 119 | S1 | S2 | command area hidden |
| Home | height | growing | 1600 | 237 | 238 | 118.5 | S2 | S1 | command area shown |
| Home | height | shrinking | 800 | 238 | 237 | 119 | S3 | S2 | command area hidden |
| Home | height | growing | 800 | 237 | 238 | 118.5 | S2 | S3 | command area shown |
| Home | height | shrinking | 600 | 238 | 237 | 119 | S4 | S2 | command area hidden |
| Home | height | growing | 600 | 237 | 238 | 118.5 | S2 | S4 | command area shown |

States (signature: group{item:widthxheight} in physical pixels):

- **S1** (height shrinking client 1600x1400): `Clipboard{Paste:84x136;Cut:83x44;Copy:98x44} | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph{Decrease indent:46x44;Increase indent:44x44;Lists:70x44;Change line spacing:64x44;Align text left:46x44;Align text center:48x44;Align text right:48x44;Justify text:44x44;Paragraph:48x44} | Insert{Picture:84x136;Date and time:94x136;Insert object:84x136} | Editing{Find:90x44;Replace:123x44;Select all:133x44}`
- **S2** (height shrinking client 1600x220): `HIDDEN`
- **S3** (height shrinking client 800x1400): `Clipboard[collapsed] | Font{Font family:240x51;Font size:80x51;Grow font:46x44;Shrink font:44x44;Bold:46x44;Italic:48x44;Underline:48x44;Strikethrough:48x44;Subscript:48x44;Superscript:44x44;Text color:68x44;Text highlight color:66x44} | Paragraph[collapsed] | Insert[collapsed] | Editing[collapsed]`
- **S4** (height shrinking client 600x1400): `Clipboard[collapsed] | Font[collapsed] | Paragraph[collapsed] | Insert[collapsed] | Editing[collapsed]`
