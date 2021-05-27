using System.Diagnostics;
using System.Windows.Input;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GreenTea.Services;
using SkiaSharp;
using Xamarin.Forms;

namespace GreenTea.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        private IVisionService visionService;
        private SKBitmap preview1;

        public ICommand StartCommand => new RelayCommand(Start);
        public ICommand TakePictureCommand => new RelayCommand(TakePicture);

        public MainViewModel()
        {
            // TODO: Use dependency injection
            visionService = DependencyService.Get<IVisionService>();
        }

        private SKBitmap image;
        public SKBitmap Image
        {
            get => image;
            set => Set(nameof(Image), ref image, value);
        }

        private void Start()
        {
            var cameraService = DependencyService.Get<ICameraService>();
            cameraService.OnVideoCapture = OnVideoCapture;
            cameraService.StartVideoCapture();
        }

        private void OnVideoCapture(byte[] encodedBytes)
        {
            var preview = SKBitmap.Decode(encodedBytes);
            if (!usePreview2)
            {
                visionService.SetPreview1(preview);
                preview1 = preview;
            }
            else
                visionService.SetPreview2(preview);

            Image = visionService.StitchPreview();
        }

        private bool usePreview2;
        private void TakePicture()
        {
            usePreview2 = true;
        }
    }
}
