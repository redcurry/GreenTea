using System;
using System.ComponentModel;
using GreenTea.ViewModels;
using SkiaSharp;
using SkiaSharp.Views.Forms;
using Xamarin.Forms;

namespace GreenTea
{
    public partial class MainPage : ContentPage
    {
        private bool isInitialized;

        public MainPage()
        {
            InitializeComponent();
        }

        public MainViewModel ViewModel => (MainViewModel)BindingContext;

        private void MainPage_OnAppearing(object sender, EventArgs e)
        {
            if (isInitialized) return;

            ViewModel.PropertyChanged += ViewModel_PropertyChanged;
            ViewModel.StartCommand.Execute(null);
            isInitialized = true;
        }

        private void ViewModel_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == nameof(ViewModel.Image))
                CanvasView.InvalidateSurface();
        }

        private void SKCanvasView_OnPaintSurface(object sender, SKPaintSurfaceEventArgs e)
        {
            var canvas = e.Surface.Canvas;
            canvas.Clear();

            if (ViewModel.Image != null)
                canvas.DrawBitmap(ViewModel.Image, new SKPoint());
        }
    }
}
