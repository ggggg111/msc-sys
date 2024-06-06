# Thesis-System

## About

Name: Gerard Marcos Freixas.

MSc. Thesis System: “Application for HAR Inference and
Visualization”.

Fudan University.

## Pre-requisites

1. Clone the repository by running ```git clone –recursive https://github.com/ggggg111/msc-sys```.
2. A Windows 11 64-bit machine with a NVIDIA GPU.
3. Download and install [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with the desktop development with C++ workflow. Click [here](https://learn.microsoft.com/en-us/cpp/build/vscpp-step-0-installation?view=msvc-170) for more information.
4. Download and install NVIDIA CUDA Toolkit. We use the 12.1.1 version. Click [here](https://developer.nvidia.com/cuda-12-1-1-download-archive) to download and install it.
5. Download the [SDL](https://www.libsdl.org) development libary. The exact version used can be downloaded [here](https://github.com/libsdl-org/SDL/releases/download/release-2.28.3/SDL2-devel-2.28.3-VC.zip).
6. Download the [LibTorch](https://pytorch.org/cppdocs/index.html) library. The exact debug version used can be downloaded [here](https://download.pytorch.org/libtorch/cu121/libtorch-win-shared-with-deps-debug-2.1.0%2Bcu121.zip), and the exact release version can be downloaded [here](https://download.pytorch.org/libtorch/cu121/libtorch-win-shared-with-deps-2.1.0%2Bcu121.zip).
7. Download the [OpenCV](https://opencv.org) library. The exact version used can be downloaded [here](https://github.com/opencv/opencv/releases/download/4.8.0/opencv-4.8.0-windows.exe).

## Build and setup

Place all `.dll` files from `SDL2-devel-2.28.3-VC/SDL2-2.28.3/lib/x64` and `SDL2_image-devel-2.6.3-VC/SDL2_image-2.6.3/lib/x64` to `Thesis-System/Thess-System/debug_workdir` and `Thesis-System/Thesis-System/release_workdir`.

Place all `.dll` files from `libtorch-win-shared-with-deps-debug-2.1.0+cu121/libtorch/lib` to `Thesis-System/Thesis-System/debug_workdir`.

Place all `.dll` files from `libtorch-win-shared-with-deps-2.1.0+cu121/libtorch/lib` to `Thesis-System/Thesis-System/release_workdir`.

Place the `opencv_world480d.dll` file from `opencv-4.8.0/build/x64/vc16/lib/bin` to `Thesis-System/Thesis-System/debug_workdir`.

Place the `opencv_world480.dll` file from `opencv-4.8.0/build/x64/vc16/lib/bin` to `Thesis-System/Thesis-System/release_workdir`.

Place the `opencv_videoio_ffmpeg480_64.dll` file from `opencv-4.8.0/build/bin` to `Thesis-System/Thesis-System/debug_workdir` and `Thesis-System/Thesis-System/release_workdir`.

The `.dll` files that are not needed are as follows: `cublasLt64_12.dll`, `cudnn_adv_infer64_8.dll`, `cudnn_adv_train64_8.dll`, `cudnn_cnn_infer64_8.dll`, `cudnn_ops_train64_8.dll`, `cufft64_11.dll`, `cusparse64_12.dll`, `cusolverMg64_11.dll`, `nvJitLink_120_0.dll`, `nvrtc64_120_0.dll`, `cublas64_12.dll`, and `cusolver64_11.dll`.

Modify the library paths from `Thesis-System/Thesis-System/Thesis-System.vcxproj` to the ones where the libraries are saved. You can also change them from Visual Studio, as well as the CUDA Toolkit include path.

Execute the following command with Python:

```shell
python Thesis-System/Thesis-System/vendor/icon-cpp-headers/GenerateIconFontCppHeaders.py
```

If there are missing packages, you can install them via pip. The script above uses the `requests`, `yaml`, `os`, `sys`, and `logging` packages.

## Prepare dataset skeleton samples

To generate skeleton samples, please check the `Utils` directory for more information, as well as its [README](./Utils/README.md).

## Additional info

We use YOLO-v8 to estimate the 2-dimensional human poses so that we can perform skeleton-based human action recognition. The detector used can be found and converted to TorchScript on [here](https://github.com/ultralytics/ultralytics). We already provide different versions.

## Releases

To run a release, simply execute the `.exe` file in the main folder.

## Additional Info

For any inquiries or questions, as well as if there are any problems setting up the project, don’t hesitate to open an issue.
