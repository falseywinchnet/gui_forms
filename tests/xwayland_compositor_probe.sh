#!/bin/sh
# Stage T5, phase C4: the X11 compositor probe under XWayland (the owner's
# concern beside T5-Q9). Starts a headless Weston with its Xwayland, runs
# the probe given as $1 against Xwayland's display, and stops Weston.
#
#   sh tests/xwayland_compositor_probe.sh <gui_forms_x11_compositor_probe>
#
# The probe's lines record what an X11 client sees there: whether
# _NET_WM_CM_S0 is owned, whether the 32-bit visual is offered, the XWAYLAND
# extension and the compositing of premultiplied alpha. When Weston or its
# Xwayland cannot be started, one line says the case was not exercised and
# why, with the end of Weston's log; that is a measurement, not a failure.
set -u

probe=$1
runtime=$(mktemp -d)
chmod 700 "$runtime"
log="$runtime/weston.log"

not_exercised() {
    printf '{"probe":"x11_compositor","xwayland_case":"not exercised","reason":"%s"}\n' "$1"
    if [ -f "$log" ]; then tail -n 20 "$log"; fi
}

if ! command -v weston > /dev/null 2>&1; then
    not_exercised "weston is not installed"
    rm -rf "$runtime"
    exit 0
fi

# Weston's headless backend needs no display of its own: the X server the
# test runs under (Xvfb) is left alone.
env -u DISPLAY -u WAYLAND_DISPLAY XDG_RUNTIME_DIR="$runtime" \
    weston --backend=headless --renderer=pixman --xwayland --socket=t5-wayland --idle-time=0 \
    --log="$log" &
weston_pid=$!

# Weston names Xwayland's display in its log once it listens on it.
display=""
attempt=0
while [ $attempt -lt 100 ]; do
    if [ -f "$log" ]; then
        display=$(sed -n 's/.*listening on display \(:[0-9][0-9]*\).*/\1/p' "$log" | head -n 1)
    fi
    if [ -n "$display" ]; then break; fi
    if ! kill -0 "$weston_pid" 2> /dev/null; then break; fi
    attempt=$((attempt + 1))
    sleep 0.1
done

status=0
if [ -z "$display" ]; then
    not_exercised "weston did not report an Xwayland display"
else
    printf '{"probe":"x11_compositor","xwayland_case":"exercised","display":"%s"}\n' "$display"
    # The versions, so a server crash can be matched against upstream.
    printf '{"probe":"x11_compositor","weston":"%s","xwayland":"%s"}\n' \
        "$(weston --version 2>&1 | head -n 1)" "$(Xwayland -version 2>&1 | grep -i version | head -n 1)"
    DISPLAY="$display" WAYLAND_DISPLAY=t5-wayland XDG_RUNTIME_DIR="$runtime" XDG_SESSION_TYPE=wayland \
        GUI_FORMS_X11_PROBE_STEPS=1 timeout 45 "$probe"
    status=$?
    if [ $status -ne 0 ]; then tail -n 30 "$log"; fi
fi

kill "$weston_pid" 2> /dev/null
wait "$weston_pid" 2> /dev/null
rm -rf "$runtime"
exit $status
