using System.Windows.Input;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using SkiaSharp;
using Xamarin.Forms;

namespace GreenTea.ViewModels
{
    public class MainViewModel : ViewModelBase
    {
        public ICommand StartCommand => new RelayCommand(Start);

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

        private void OnVideoCapture(byte[] imageBytes)
        {
            Image = SKBitmap.Decode(imageBytes);
        }
    }
}
