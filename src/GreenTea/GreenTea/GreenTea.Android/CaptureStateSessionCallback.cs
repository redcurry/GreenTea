using System;
using Android.Hardware.Camera2;

namespace GreenTea.Droid
{
    public class CaptureStateSessionCallback : CameraCaptureSession.StateCallback
    {
        public Action<CameraCaptureSession> Failed { get; set; }
        public Action<CameraCaptureSession> Configured { get; set; }
        public Action<CameraCaptureSession> Ready { get; set; }

        public override void OnConfigured(CameraCaptureSession session)
        {
            Configured?.Invoke(session);
        }

        public override void OnConfigureFailed(CameraCaptureSession session)
        {
            Failed?.Invoke(session);
        }

        public override void OnReady(CameraCaptureSession session)
        {
            Ready?.Invoke(session);
        }
    }
}