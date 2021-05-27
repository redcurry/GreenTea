using GreenTea.Services;
using GreenTea.Vision.Wrapper;
using SkiaSharp;
using Xamarin.Forms;

[assembly: Dependency(typeof(VisionService))]
namespace GreenTea.Vision.Wrapper
{
    public class VisionService : IVisionService
    {
        private readonly VisionSafeHandle visionHandle;

        private int previewWidth;
        private int previewHeight;

        public VisionService()
        {
            visionHandle = VisionInterop.CreateVision();
        }

        public void SetPreview1(SKBitmap preview1)
        {
            var ptr = preview1.GetPixels();
            previewWidth = preview1.Width;
            previewHeight = preview1.Height;
            VisionInterop.SetPreview1(visionHandle, ptr, previewWidth, previewHeight);
        }

        public void SetPreview2(SKBitmap preview2)
        {
            var ptr = preview2.GetPixels();
            previewWidth = preview2.Width;
            previewHeight = preview2.Height;
            VisionInterop.SetPreview2(visionHandle, ptr, previewWidth, previewHeight);
        }

        public SKBitmap StitchPreview()
        {
            var output = CreateEmptyBitmap(previewWidth, previewHeight);
            var ptr = output.GetPixels();
            VisionInterop.StitchPreview(visionHandle, ptr, previewWidth, previewHeight);
            return output;
        }

        private SKBitmap CreateEmptyBitmap(int width, int height)
        {
            var bitmap = new SKBitmap(width, height, SKColorType.Rgba8888, SKAlphaType.Unpremul);
            return bitmap;
        }
    }
}