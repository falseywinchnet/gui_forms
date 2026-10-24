// GUI.Forms browser host, page side (Emscripten --js-library). It owns the
// canvas, forwards DOM input to the gf_web_* exports in web_host.cpp, copies
// presented rectangles with putImageData and runs the tick schedule.
addToLibrary({
  $GFWeb__deps: ['$stringToNewUTF8', 'free'],
  $GFWeb: {
    canvas: null,
    context: null,
    frame: null,
    rafPending: false,
    timer: 0,
    timerDue: 0,
    width: 0,
    height: 0,
    scale: 1,
    codes: {
      KeyA: 0x04, KeyB: 0x05, KeyC: 0x06, KeyD: 0x07, KeyE: 0x08, KeyF: 0x09, KeyG: 0x0a,
      KeyH: 0x0b, KeyI: 0x0c, KeyJ: 0x0d, KeyK: 0x0e, KeyL: 0x0f, KeyM: 0x10, KeyN: 0x11,
      KeyO: 0x12, KeyP: 0x13, KeyQ: 0x14, KeyR: 0x15, KeyS: 0x16, KeyT: 0x17, KeyU: 0x18,
      KeyV: 0x19, KeyW: 0x1a, KeyX: 0x1b, KeyY: 0x1c, KeyZ: 0x1d,
      Digit1: 0x1e, Digit2: 0x1f, Digit3: 0x20, Digit4: 0x21, Digit5: 0x22, Digit6: 0x23,
      Digit7: 0x24, Digit8: 0x25, Digit9: 0x26, Digit0: 0x27,
      Enter: 0x28, NumpadEnter: 0x28, Escape: 0x29, Backspace: 0x2a, Tab: 0x2b, Space: 0x2c,
      Minus: 0x2d, Equal: 0x2e, BracketLeft: 0x2f, BracketRight: 0x30, Backslash: 0x31,
      Semicolon: 0x33, Quote: 0x34, Backquote: 0x35, Comma: 0x36, Period: 0x37, Slash: 0x38,
      F1: 0x3a, F2: 0x3b, F3: 0x3c, F4: 0x3d, F5: 0x3e, F6: 0x3f, F7: 0x40, F8: 0x41,
      F9: 0x42, F10: 0x43, F11: 0x44, F12: 0x45,
      Home: 0x4a, PageUp: 0x4b, Delete: 0x4c, End: 0x4d, PageDown: 0x4e,
      ArrowRight: 0x4f, ArrowLeft: 0x50, ArrowDown: 0x51, ArrowUp: 0x52,
      NumpadSubtract: 0x56, NumpadAdd: 0x57
    },
    cursors: ['default', 'text', 'pointer', 'crosshair', 'ew-resize', 'ns-resize',
              'wait', 'not-allowed', 'nwse-resize', 'nesw-resize'],
    modifiers(event) {
      return (event.shiftKey ? 1 : 0) | (event.ctrlKey ? 2 : 0) | (event.altKey ? 4 : 0) |
             (event.metaKey ? 8 : 0);
    },
    tick() {
      GFWeb.rafPending = false;
      _gf_web_tick();
    },
    measure() {
      const rect = GFWeb.canvas.getBoundingClientRect();
      const scale = Math.min(Math.max(window.devicePixelRatio || 1, 1), 2);
      return [Math.max(1, Math.round(rect.width)), Math.max(1, Math.round(rect.height)), scale];
    },
    resize() {
      const [width, height, scale] = GFWeb.measure();
      if (width === GFWeb.width && height === GFWeb.height && scale === GFWeb.scale) return;
      GFWeb.width = width; GFWeb.height = height; GFWeb.scale = scale;
      _gf_web_resize(width, height, scale);
    },
    point(event) {
      const rect = GFWeb.canvas.getBoundingClientRect();
      return [event.clientX - rect.left, event.clientY - rect.top];
    },
    install() {
      const canvas = GFWeb.canvas;
      canvas.tabIndex = 0;
      canvas.style.touchAction = 'none';
      canvas.style.outline = 'none';
      const pointer = (kind) => (event) => {
        const [x, y] = GFWeb.point(event);
        if (kind === 1) canvas.focus({preventScroll: true});
        _gf_web_pointer(kind, x, y, event.button, GFWeb.modifiers(event), 0, 0,
                        event.pointerId, event.pointerType === 'touch' ? 1 : 0);
        if (kind === 1 || kind === 2) event.preventDefault();
      };
      canvas.addEventListener('pointermove', (event) => {
        const events = event.getCoalescedEvents ? event.getCoalescedEvents() : [event];
        const last = events.length ? events[events.length - 1] : event;
        pointer(0)(last);
      });
      canvas.addEventListener('pointerdown', pointer(1));
      canvas.addEventListener('pointerup', pointer(2));
      canvas.addEventListener('pointercancel', pointer(6));
      canvas.addEventListener('pointerenter', pointer(3));
      canvas.addEventListener('pointerleave', pointer(4));
      canvas.addEventListener('contextmenu', (event) => event.preventDefault());
      canvas.addEventListener('wheel', (event) => {
        const [x, y] = GFWeb.point(event);
        const unit = event.deltaMode === 1 ? 40 : event.deltaMode === 2 ? GFWeb.height : 1;
        _gf_web_pointer(5, x, y, 0, GFWeb.modifiers(event), -event.deltaX * unit,
                        -event.deltaY * unit, 1, 0);
        event.preventDefault();
      }, {passive: false});
      const key = (down) => (event) => {
        const usage = GFWeb.codes[event.code] || 0;
        const handled = usage ? _gf_web_key(down ? 1 : 0, usage, event.repeat ? 1 : 0,
                                            GFWeb.modifiers(event)) : 0;
        let text = false;
        if (down && !handled && !event.ctrlKey && !event.metaKey && !event.altKey &&
            event.key && [...event.key].length === 1) {
          const pointer = stringToNewUTF8(event.key);
          _gf_web_text(pointer);
          _free(pointer);
          text = true;
        }
        const navigation = usage >= 0x4a && usage <= 0x52 || usage === 0x2c || usage === 0x2b;
        if (handled || text || navigation || usage === 0x3a) event.preventDefault();
      };
      canvas.addEventListener('keydown', key(true));
      canvas.addEventListener('keyup', key(false));
      canvas.addEventListener('focus', () => _gf_web_focus(1));
      canvas.addEventListener('blur', () => _gf_web_focus(0));
      document.addEventListener('paste', (event) => {
        const text = event.clipboardData ? event.clipboardData.getData('text/plain') : '';
        const pointer = stringToNewUTF8(text);
        _gf_web_paste(pointer);
        _free(pointer);
      });
      document.addEventListener('visibilitychange', () => _gf_web_occluded(document.hidden ? 1 : 0));
      new ResizeObserver(() => GFWeb.resize()).observe(canvas);
      window.addEventListener('resize', () => GFWeb.resize());
      window.addEventListener('pagehide', () => _gf_web_close());
    }
  },

  gf_web_js_attach__deps: ['$GFWeb'],
  gf_web_js_attach: (width, height, scale) => {
    const id = Module['guiFormsCanvas'] || 'gui-forms-canvas';
    GFWeb.canvas = typeof id === 'string' ? document.getElementById(id) : id;
    if (!GFWeb.canvas) throw new Error('GUI.Forms: no canvas element "' + id + '"');
    GFWeb.context = GFWeb.canvas.getContext('2d', {alpha: false, desynchronized: true});
    const [w, h, s] = GFWeb.measure();
    GFWeb.width = w; GFWeb.height = h; GFWeb.scale = s;
    HEAPF64[width >> 3] = w;
    HEAPF64[height >> 3] = h;
    HEAPF64[scale >> 3] = s;
    GFWeb.install();
  },

  gf_web_js_schedule__deps: ['$GFWeb'],
  gf_web_js_schedule: (milliseconds) => {
    if (milliseconds <= 16) {
      if (GFWeb.timer) { clearTimeout(GFWeb.timer); GFWeb.timer = 0; }
      if (!GFWeb.rafPending) {
        GFWeb.rafPending = true;
        requestAnimationFrame(GFWeb.tick);
      }
      return;
    }
    const due = performance.now() + milliseconds;
    if (GFWeb.rafPending || (GFWeb.timer && GFWeb.timerDue <= due)) return;
    if (GFWeb.timer) clearTimeout(GFWeb.timer);
    GFWeb.timerDue = due;
    GFWeb.timer = setTimeout(() => {
      GFWeb.timer = 0;
      if (!GFWeb.rafPending) {
        GFWeb.rafPending = true;
        requestAnimationFrame(GFWeb.tick);
      }
    }, milliseconds - 8);
  },

  gf_web_js_present__deps: ['$GFWeb'],
  gf_web_js_present: (pixels, stride, left, top, right, bottom, width, height) => {
    const canvas = GFWeb.canvas;
    if (canvas.width !== width || canvas.height !== height) {
      canvas.width = width;
      canvas.height = height;
    }
    const columns = right - left, rows = bottom - top;
    if (columns <= 0 || rows <= 0) return;
    const bytes = columns * 4 * rows;
    if (!GFWeb.frame || GFWeb.frame.data.length < bytes || GFWeb.frame.width !== columns ||
        GFWeb.frame.height !== rows) {
      GFWeb.frame = new ImageData(columns, rows);
    }
    // The raster is premultiplied RGBA and the window opaque, so its bytes
    // are already the straight RGBA putImageData expects. The heap may be a
    // SharedArrayBuffer, which ImageData cannot wrap, hence the copy.
    const target = GFWeb.frame.data;
    const rowBytes = columns * 4;
    for (let row = 0; row < rows; ++row) {
      const start = pixels + (top + row) * stride + left * 4;
      target.set(HEAPU8.subarray(start, start + rowBytes), row * rowBytes);
    }
    GFWeb.context.putImageData(GFWeb.frame, left, top);
  },

  gf_web_js_set_title__deps: ['$UTF8ToString'],
  gf_web_js_set_title: (title) => { document.title = UTF8ToString(title); },

  gf_web_js_set_cursor__deps: ['$GFWeb'],
  gf_web_js_set_cursor: (kind) => {
    if (GFWeb.canvas) GFWeb.canvas.style.cursor = GFWeb.cursors[kind] || 'default';
  },

  gf_web_js_pointer_capture__deps: ['$GFWeb'],
  gf_web_js_pointer_capture: (captured, pointerId) => {
    try {
      if (captured) GFWeb.canvas.setPointerCapture(pointerId);
      else GFWeb.canvas.releasePointerCapture(pointerId);
    } catch (error) {}
  },

  gf_web_js_write_clipboard__deps: ['$UTF8ToString'],
  gf_web_js_write_clipboard: (text) => {
    const value = UTF8ToString(text);
    if (navigator.clipboard) navigator.clipboard.writeText(value).catch(() => {});
  },

  gf_web_js_full_screen__deps: ['$GFWeb'],
  gf_web_js_full_screen: () => {
    if (document.fullscreenElement) document.exitFullscreen().catch(() => {});
    else (GFWeb.canvas.parentElement || GFWeb.canvas).requestFullscreen().catch(() => {});
  },

  gf_web_js_set_visible__deps: ['$GFWeb'],
  gf_web_js_set_visible: (visible) => {
    if (GFWeb.canvas) GFWeb.canvas.style.visibility = visible ? 'visible' : 'hidden';
  },

  gf_web_js_screen: (width, height) => {
    HEAPF64[width >> 3] = screen.width;
    HEAPF64[height >> 3] = screen.height;
  }
});
