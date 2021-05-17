using Android.Content;
using Android.Views;
using System;
using System.Collections.Generic;
using System.Linq;
using Android;
using Android.Content.PM;
using Android.Graphics;
using Android.Hardware.Camera2;
using Android.Media;
using AndroidX.Core.Content;
using GreenTea.Droid;
using Xamarin.Forms;
using Image = Android.Media.Image;

[assembly: Dependency(typeof(CameraService))]
namespace GreenTea.Droid
{
    public class CameraService : ICameraService
    {
        public Action<byte[]> OnVideoCapture { get; set; }

        public void StartVideoCapture()
        {
            if (CheckCameraPermission() != Permission.Granted) return;

            var cameraId = GetBackCameraId();
            var cameraManager = GetCameraManager();
            cameraManager.OpenCamera(cameraId, CreateCameraStateCallback(), null);
        }

        private Permission CheckCameraPermission()
        {
            return ContextCompat.CheckSelfPermission(Android.App.Application.Context, Manifest.Permission.Camera);
        }

        private string GetBackCameraId()
        {
            var cameraManager = GetCameraManager();
            var cameraIdList = cameraManager.GetCameraIdList();

            foreach (var cameraId in cameraIdList)
            {
                var characteristics = cameraManager.GetCameraCharacteristics(cameraId);
                var cameraFace = (int)characteristics.Get(CameraCharacteristics.LensFacing);
                if (cameraFace == (int)LensFacing.Back)
                    return cameraId;
            }

            return null;
        }

        private CameraManager GetCameraManager()
        {
            return (CameraManager)Android.App.Application.Context.GetSystemService(Context.CameraService);
        }

        private CameraStateCallback CreateCameraStateCallback()
        {
            return new CameraStateCallback
            {
                Opened = cameraDevice =>
                {
                    var imageReader = CreateImageReader();
                    cameraDevice.CreateCaptureSession(
                        GetOutputs(imageReader),
                        CreateCaptureStateSessionCallback(cameraDevice, imageReader),
                        null);
                }
            };
        }

        private ImageReader CreateImageReader()
        {
            // Create ImageReader, which gives access to the latest video frame
            var imageReader = ImageReader.NewInstance(640*2, 480*2, ImageFormatType.Jpeg, 2);
            imageReader.SetOnImageAvailableListener(CreateImageAvailableListener(), null);
            return imageReader;
        }

        private ImageAvailableListener CreateImageAvailableListener()
        {
            return new ImageAvailableListener
            {
                ImageAvailable = reader =>
                {
                    var image = reader.AcquireLatestImage();
                    if (image != null)
                    {
                        var imageBytes = CopyBytesFromImage(image);
                        OnVideoCapture(imageBytes);
                        image.Close();
                    }
                }
            };
        }

        private byte[] CopyBytesFromImage(Image image)
        {
            var buffer = image.GetPlanes()?[0].Buffer;
            var bytes = new byte[buffer?.Remaining() ?? 0];
            buffer?.Get(bytes);
            return bytes;
        }

        private List<Surface> GetOutputs(ImageReader imageReader)
        {
            return new[] {imageReader.Surface}.ToList();
        }

        private CaptureStateSessionCallback CreateCaptureStateSessionCallback(CameraDevice cameraDevice, ImageReader imageReader)
        {
            return new CaptureStateSessionCallback
            {
                Ready = session =>
                {
                    session.SetRepeatingRequest(CreateCaptureRequest(cameraDevice, imageReader), null, null);
                }
            };
        }

        private CaptureRequest CreateCaptureRequest(CameraDevice cameraDevice, ImageReader imageReader)
        {
            var builder = cameraDevice.CreateCaptureRequest(CameraTemplate.Preview);
            builder.AddTarget(imageReader.Surface);
            return builder.Build();
        }
    }
}