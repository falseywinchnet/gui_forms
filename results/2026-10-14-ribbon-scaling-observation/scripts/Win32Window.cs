using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;

// Window helpers for the ribbon scaling observation. Every call takes a window
// handle the caller obtained from a process it started; nothing enumerates or
// touches other processes' windows except EnumWindows filtering by process id.
public static class RibbonObserveWin32
{
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }

    [StructLayout(LayoutKind.Sequential)]
    public struct POINT { public int X, Y; }

    [StructLayout(LayoutKind.Sequential)]
    public struct WINDOWPLACEMENT
    {
        public int length; public int flags; public int showCmd;
        public POINT ptMinPosition; public POINT ptMaxPosition; public RECT rcNormalPosition;
    }

    public delegate bool EnumWindowsProc(IntPtr hwnd, IntPtr lParam);

    [DllImport("user32.dll")] static extern bool EnumWindows(EnumWindowsProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out uint pid);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hwnd);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr hwnd, System.Text.StringBuilder s, int n);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetWindowText(IntPtr hwnd, System.Text.StringBuilder s, int n);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hwnd, out RECT r);
    [DllImport("user32.dll")] static extern bool GetClientRect(IntPtr hwnd, out RECT r);
    [DllImport("user32.dll")] static extern bool ClientToScreen(IntPtr hwnd, ref POINT p);
    [DllImport("user32.dll")] static extern bool SetWindowPos(IntPtr hwnd, IntPtr after, int x, int y, int cx, int cy, uint flags);
    [DllImport("user32.dll")] static extern bool GetWindowPlacement(IntPtr hwnd, ref WINDOWPLACEMENT p);
    [DllImport("user32.dll")] static extern bool SetWindowPlacement(IntPtr hwnd, ref WINDOWPLACEMENT p);
    [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint flags);
    [DllImport("user32.dll")] static extern uint GetDpiForWindow(IntPtr hwnd);
    [DllImport("user32.dll")] static extern bool PostMessage(IntPtr hwnd, uint msg, IntPtr w, IntPtr l);
    [DllImport("user32.dll")] static extern bool IsWindow(IntPtr hwnd);

    const uint SWP_NOZORDER = 0x0004, SWP_NOACTIVATE = 0x0010, SWP_NOOWNERZORDER = 0x0200;

    public static List<IntPtr> VisibleTopLevelWindows(int processId)
    {
        List<IntPtr> found = new List<IntPtr>();
        EnumWindows(delegate (IntPtr h, IntPtr l) {
            uint pid;
            GetWindowThreadProcessId(h, out pid);
            if (pid == (uint)processId && IsWindowVisible(h)) found.Add(h);
            return true;
        }, IntPtr.Zero);
        return found;
    }

    public static string ClassOf(IntPtr h) { var s = new System.Text.StringBuilder(256); GetClassName(h, s, 256); return s.ToString(); }
    public static string TitleOf(IntPtr h) { var s = new System.Text.StringBuilder(512); GetWindowText(h, s, 512); return s.ToString(); }
    public static bool Exists(IntPtr h) { return IsWindow(h); }
    public static uint Dpi(IntPtr h) { return GetDpiForWindow(h); }

    public static int[] WindowRect(IntPtr h) { RECT r; GetWindowRect(h, out r); return new int[] { r.Left, r.Top, r.Right - r.Left, r.Bottom - r.Top }; }
    public static int[] ClientRect(IntPtr h)
    {
        RECT r; GetClientRect(h, out r);
        POINT p = new POINT(); ClientToScreen(h, ref p);
        return new int[] { p.X, p.Y, r.Right, r.Bottom };
    }

    // Resizes so that the client area is exactly width x height, keeping the
    // window's top-left corner. The frame size is measured, not computed.
    public static int[] SetClientSize(IntPtr h, int width, int height)
    {
        int[] w = WindowRect(h); int[] c = ClientRect(h);
        int frameW = w[2] - c[2], frameH = w[3] - c[3];
        SetWindowPos(h, IntPtr.Zero, w[0], w[1], width + frameW, height + frameH, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        return ClientRect(h);
    }

    public static void SetWindowRect(IntPtr h, int x, int y, int width, int height)
    {
        SetWindowPos(h, IntPtr.Zero, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    }

    [DllImport("user32.dll")] static extern IntPtr GetParent(IntPtr hwnd);

    // The child window of `main` that contains `inner` (the ribbon's dock),
    // as x, y, width, height and visible (1 or 0).
    public static int[] DockOf(IntPtr main, IntPtr inner)
    {
        IntPtr cur = inner;
        while (cur != IntPtr.Zero)
        {
            IntPtr parent = GetParent(cur);
            if (parent == main)
            {
                int[] r = WindowRect(cur);
                return new int[] { r[0], r[1], r[2], r[3], IsWindowVisible(cur) ? 1 : 0 };
            }
            cur = parent;
        }
        return new int[] { 0, 0, 0, 0, 0 };
    }

    public static WINDOWPLACEMENT Placement(IntPtr h) { WINDOWPLACEMENT p = new WINDOWPLACEMENT(); p.length = Marshal.SizeOf(p); GetWindowPlacement(h, ref p); return p; }
    public static void Restore(IntPtr h, WINDOWPLACEMENT p) { p.length = Marshal.SizeOf(p); SetWindowPlacement(h, ref p); }
    public static void Close(IntPtr h) { PostMessage(h, 0x0010, IntPtr.Zero, IntPtr.Zero); }

    // -- MSAA (the NetUI ribbon exposes its controls through IAccessible) -----

    [DllImport("oleacc.dll")]
    static extern int AccessibleObjectFromWindow(IntPtr hwnd, uint objectId, ref Guid iid,
        [MarshalAs(UnmanagedType.IUnknown)] out object accessible);
    [DllImport("oleacc.dll")]
    static extern int AccessibleChildren(Accessibility.IAccessible container, int start, int count,
        [Out] object[] children, out int obtained);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)]
    static extern IntPtr FindWindowEx(IntPtr parent, IntPtr after, string cls, string title);

    public class AccNode
    {
        public int Depth; public string Name; public int Role; public int State;
        public int X, Y, W, H; public string DefaultAction;
    }

    [DllImport("user32.dll")] static extern IntPtr SetThreadDpiAwarenessContext(IntPtr context);

    // Makes this thread per-monitor DPI aware (v2), so every size and
    // position below is in physical pixels.
    public static void UsePhysicalPixels() { SetThreadDpiAwarenessContext(new IntPtr(-4)); }

    // Every descendant window of `root` with class `cls`, depth-first.
    public static List<IntPtr> FindDescendantWindows(IntPtr root, string cls)
    {
        List<IntPtr> found = new List<IntPtr>();
        CollectDescendants(root, cls, found);
        return found;
    }

    static void CollectDescendants(IntPtr root, string cls, List<IntPtr> found)
    {
        IntPtr child = FindWindowEx(root, IntPtr.Zero, null, null);
        while (child != IntPtr.Zero)
        {
            if (ClassOf(child) == cls) found.Add(child);
            CollectDescendants(child, cls, found);
            child = FindWindowEx(root, child, null, null);
        }
    }

    // The first descendant window of `root` with class `cls` (depth-first).
    public static IntPtr FindDescendantWindow(IntPtr root, string cls)
    {
        IntPtr child = FindWindowEx(root, IntPtr.Zero, null, null);
        while (child != IntPtr.Zero)
        {
            if (ClassOf(child) == cls) return child;
            IntPtr deeper = FindDescendantWindow(child, cls);
            if (deeper != IntPtr.Zero) return deeper;
            child = FindWindowEx(root, child, null, null);
        }
        return IntPtr.Zero;
    }

    // Walks the IAccessible tree of a window's client object to `maxDepth`.
    public static List<AccNode> AccTree(IntPtr hwnd, int maxDepth)
    {
        List<AccNode> rows = new List<AccNode>();
        Guid iid = new Guid("618736E0-3C3D-11CF-810C-00AA00389B71");
        object obj;
        if (AccessibleObjectFromWindow(hwnd, 0xFFFFFFFC, ref iid, out obj) != 0 || obj == null) return rows;
        Walk((Accessibility.IAccessible)obj, 0, 0, maxDepth, rows);
        return rows;
    }

    // Runs the default action of the first node with this name and role (a
    // ribbon tab's "Switch"). Returns whether one was found.
    public static bool DoDefaultAction(IntPtr hwnd, string name, int role)
    {
        Guid iid = new Guid("618736E0-3C3D-11CF-810C-00AA00389B71");
        object obj;
        if (AccessibleObjectFromWindow(hwnd, 0xFFFFFFFC, ref iid, out obj) != 0 || obj == null) return false;
        return Act((Accessibility.IAccessible)obj, name, role, 0);
    }

    static bool Act(Accessibility.IAccessible acc, string name, int role, int depth)
    {
        string n = null; int r = 0;
        try { n = acc.get_accName(0); } catch { }
        try { object o = acc.get_accRole(0); if (o is int) r = (int)o; } catch { }
        if (n == name && r == role) { acc.accDoDefaultAction(0); return true; }
        if (depth > 12) return false;
        int count = 0;
        try { count = acc.accChildCount; } catch { return false; }
        if (count <= 0) return false;
        object[] children = new object[count];
        int obtained;
        if (AccessibleChildren(acc, 0, count, children, out obtained) != 0) return false;
        for (int i = 0; i < obtained; i++)
        {
            Accessibility.IAccessible childAcc = children[i] as Accessibility.IAccessible;
            if (childAcc != null) { if (Act(childAcc, name, role, depth + 1)) return true; }
            else
            {
                string cn = null; int cr = 0;
                try { cn = acc.get_accName(children[i]); } catch { }
                try { object o = acc.get_accRole(children[i]); if (o is int) cr = (int)o; } catch { }
                if (cn == name && cr == role) { acc.accDoDefaultAction(children[i]); return true; }
            }
        }
        return false;
    }

    static void Record(Accessibility.IAccessible acc, object childId, int depth, List<AccNode> rows)
    {
        AccNode n = new AccNode(); n.Depth = depth;
        try { n.Name = acc.get_accName(childId); } catch { }
        try { object r = acc.get_accRole(childId); if (r is int) n.Role = (int)r; } catch { }
        try { object s = acc.get_accState(childId); if (s is int) n.State = (int)s; } catch { }
        try { n.DefaultAction = acc.get_accDefaultAction(childId); } catch { }
        try { int x, y, w, h; acc.accLocation(out x, out y, out w, out h, childId); n.X = x; n.Y = y; n.W = w; n.H = h; } catch { }
        rows.Add(n);
    }

    static void Walk(Accessibility.IAccessible acc, object self, int depth, int maxDepth, List<AccNode> rows)
    {
        Record(acc, self, depth, rows);
        if (depth >= maxDepth) return;
        int count = 0;
        try { count = acc.accChildCount; } catch { return; }
        if (count <= 0) return;
        object[] children = new object[count];
        int obtained;
        if (AccessibleChildren(acc, 0, count, children, out obtained) != 0) return;
        for (int i = 0; i < obtained; i++)
        {
            Accessibility.IAccessible childAcc = children[i] as Accessibility.IAccessible;
            if (childAcc != null) Walk(childAcc, 0, depth + 1, maxDepth, rows);
            else Record(acc, children[i], depth + 1, rows);
        }
    }

    // Captures the window itself (PrintWindow, full content), cropped to the
    // top `cropHeight` pixels of the window (the title bar and ribbon), as PNG.
    public static void CaptureTop(IntPtr h, string path, int cropHeight)
    {
        int[] w = WindowRect(h);
        using (Bitmap full = new Bitmap(w[2], w[3], PixelFormat.Format32bppArgb))
        {
            using (Graphics g = Graphics.FromImage(full))
            {
                IntPtr dc = g.GetHdc();
                PrintWindow(h, dc, 2); // PW_RENDERFULLCONTENT
                g.ReleaseHdc(dc);
            }
            int ch = Math.Min(cropHeight, w[3]);
            using (Bitmap crop = full.Clone(new Rectangle(0, 0, w[2], ch), PixelFormat.Format24bppRgb))
            {
                crop.Save(path, ImageFormat.Png);
            }
        }
    }
}
