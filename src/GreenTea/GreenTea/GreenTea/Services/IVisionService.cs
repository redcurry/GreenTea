using SkiaSharp;

namespace GreenTea.Services
{
    public interface IVisionService
    {
        void SetPreview1(SKBitmap preview1);
        void SetPreview2(SKBitmap preview2);
        SKBitmap StitchPreview();
    }
}
