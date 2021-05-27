using System;

namespace GreenTea
{
    public interface ICameraService
    {
        // Action to execute on the latest video frame capture
        Action<byte[]> OnVideoCapture { get; set; }

        // Action to execute on the latest photo capture taken
        //Action<byte[]> OnPhotoCapture { get; set; }

        // Starts capturing video images (accessed via OnVideoCapture)
        void StartVideoCapture();

        //void TakePicture();
    }
}
