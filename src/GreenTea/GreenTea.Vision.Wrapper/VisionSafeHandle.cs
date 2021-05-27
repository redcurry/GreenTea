using System;
using Microsoft.Win32.SafeHandles;

namespace GreenTea.Vision.Wrapper
{
    public class VisionSafeHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        public VisionSafeHandle() : base(true) { }

        public IntPtr Ptr => handle;

        protected override bool ReleaseHandle()
        {
            //VisionInterop.FreeVision(handle);
            return true;
        }
    }
}
