using System;
using System.Runtime.InteropServices;

namespace GreenTea.Vision.Wrapper
{
    public static class VisionInterop
    {
#if ANDROID
        private const string DllName = "libGreenTeaVision.so";
#else
        private const string DllName = "__Internal";
#endif

        [DllImport(DllName, EntryPoint = "CreateVision")]
        internal static extern VisionSafeHandle CreateVision();

        [DllImport(DllName, EntryPoint = "FreeVision")]
        internal static extern void FreeVision(IntPtr ptr);

        [DllImport(DllName, EntryPoint = "SetPreview1")]
        internal static extern void SetPreview1(VisionSafeHandle handle, IntPtr preview1Ptr, int width, int height);

        [DllImport(DllName, EntryPoint = "SetPreview2")]
        internal static extern void SetPreview2(VisionSafeHandle handle, IntPtr preview2Ptr, int width, int height);

        [DllImport(DllName, EntryPoint = "StitchPreview")]
        internal static extern void StitchPreview(VisionSafeHandle handle, IntPtr outputPtr, int width, int height);
    }
}
