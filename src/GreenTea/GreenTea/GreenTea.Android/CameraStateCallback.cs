using System;
using Android.Hardware.Camera2;

namespace GreenTea.Droid
{
    public class CameraStateCallback : CameraDevice.StateCallback
    {
        public Action<CameraDevice> Disconnected { get; set; }
        public Action<CameraDevice, CameraError> Error { get; set; }
        public Action<CameraDevice> Opened { get; set; }

        public override void OnDisconnected(CameraDevice camera)
        {
            Disconnected?.Invoke(camera);
        }

        public override void OnError(CameraDevice camera, CameraError error)
        {
            Error?.Invoke(camera, error);
        }

        public override void OnOpened(CameraDevice camera)
        {
            Opened?.Invoke(camera);
        }
    }
}