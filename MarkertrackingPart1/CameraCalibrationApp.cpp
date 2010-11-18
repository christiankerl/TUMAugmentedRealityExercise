/*
 * This file is part of the TUMAugmentedRealityExercise.
 *
 * (c) Christian Kerl <christian.kerl@in.tum.de>
 *
 * This source file is subject to the MIT license that is bundled
 * with this source code in the file LICENSE.
 */

#include "CameraCalibrationApp.h"

namespace TUMAugmentedRealityExercise
{
	CameraCalibrationApp::CameraCalibrationApp(void) : 
		running(true), 
		sampleChessboardImage(false), 
		sampleChessboardImageFolder(false), 
		calculateCalibration(false), 

		processor(), 
		window("CameraCalibrationApp", &processor),
		calibrator("./media")
	{
		this->video = new CameraVideoSource();

		this->fps.SetVideoSourceFPS(this->video->GetFPS());
	}


	CameraCalibrationApp::~CameraCalibrationApp(void)
	{
		delete this->video;
	}

	int CameraCalibrationApp::Run(int argc, char* argv[])
	{
		while(this->running)
		{
			fps.BeginProcessing();

			this->video->GetNextImage(this->buffer);

			if(this->sampleChessboardImage)
			{
				if(calibrator.SampleChessboardImage(this->buffer))
				{
					std::cout << "Sampled " << calibrator.GetSampledImageCount() << " chessboard image!" << std::endl;
				}
				else
				{
					std::cout << "Sampling chessboard image failed!" << std::endl;
				}

				this->sampleChessboardImage = false;
			}

			if(this->sampleChessboardImageFolder)
			{
				std::vector<std::string> images;
				images.push_back("chessboard00.bmp");
				images.push_back("chessboard01.bmp");
				images.push_back("chessboard02.bmp");
				images.push_back("chessboard03.bmp");
				images.push_back("chessboard04.bmp");
				images.push_back("chessboard05.bmp");
				images.push_back("chessboard06.bmp");
				images.push_back("chessboard07.bmp");
				images.push_back("chessboard08.bmp");
				images.push_back("chessboard09.bmp");
				images.push_back("chessboard10.bmp");
				images.push_back("chessboard11.bmp");
				images.push_back("chessboard12.bmp");
				images.push_back("chessboard13.bmp");
				images.push_back("chessboard14.bmp");
				images.push_back("chessboard15.bmp");
				images.push_back("chessboard16.bmp");
				images.push_back("chessboard17.bmp");
				images.push_back("chessboard18.bmp");
				images.push_back("chessboard19.bmp");
				images.push_back("chessboard20.bmp");

				calibrator.SampleChessboardImages(images);

				this->sampleChessboardImageFolder = false;
			}

			if(this->calculateCalibration)
			{
				calibrator.CalculateCalibration();

				this->calculateCalibration = false;
			}

			this->window.update(this->buffer);

			fps.EndProcessing();

			this->HandleKeyboardInput(cv::waitKey(fps.GetMsUntilNextFrame()));
		}

		return 0;
	}

	void CameraCalibrationApp::HandleKeyboardInput(int key)
	{
		switch(key)
		{
		case 13:   // ENTER
			this->calculateCalibration = true;
			break;
		case 0x20: // SPACE
			this->sampleChessboardImage = true;
			break;
		case 27:   // ESCAPE
			this->running = false;
			break;
		}
	}
}
