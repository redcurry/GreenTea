using System;
using Android.Hardware.Camera2;

namespace GreenTea.Droid
{
    public class CameraCaptureCallback : CameraCaptureSession.CaptureCallback
    {
        public Action<CameraCaptureSession, CaptureRequest, TotalCaptureResult> CaptureCompleted { get; set; }
        public Action<CameraCaptureSession, CaptureRequest, CaptureResult> CaptureProgressed { get; set; }

        public override void OnCaptureCompleted(CameraCaptureSession session, CaptureRequest request, TotalCaptureResult result)
        {
            CaptureCompleted?.Invoke(session, request, result);
        }

        public override void OnCaptureProgressed(CameraCaptureSession session, CaptureRequest request, CaptureResult partialResult)
        {
            CaptureProgressed?.Invoke(session, request, partialResult);
        }
    }
}