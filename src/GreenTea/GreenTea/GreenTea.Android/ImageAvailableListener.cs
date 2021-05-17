using System;
using Android.Media;

namespace GreenTea.Droid
{
    public class ImageAvailableListener : Java.Lang.Object, ImageReader.IOnImageAvailableListener
    {
        public Action<ImageReader> ImageAvailable { get; set; }

        public void OnImageAvailable(ImageReader reader)
        {
            ImageAvailable?.Invoke(reader);
        }
    }
}