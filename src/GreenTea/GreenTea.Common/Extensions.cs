using System;
using System.Runtime.InteropServices;

namespace GreenTea.Common
{
    public static class Extensions
    {
        public static IntPtr CopyToUnmanagedArray(this byte[] bytes)
        {
            var data = Marshal.AllocHGlobal(sizeof(byte) * bytes.Length);
            Marshal.Copy(bytes, 0, data, bytes.Length);
            return data;
        }

        public static byte[] CopyToByteArray(this IntPtr ptr, int length)
        {
            var bytes = new byte[length];
            Marshal.Copy(ptr, bytes, 0, length);
            return bytes;
        }

        // Get an IntPtr to the given bytes without copying
        public static IntPtr ToIntPtr(this byte[] bytes, out GCHandle freeMe)
        {
            freeMe = GCHandle.Alloc(bytes, GCHandleType.Pinned);
            return freeMe.AddrOfPinnedObject();
        }
    }
}
