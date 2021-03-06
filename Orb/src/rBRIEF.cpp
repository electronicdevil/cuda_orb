#pragma once


#include "BRIEF.h"
#include <opencv2/core.hpp>
#include <iostream>
#include <vector>

#define M_PI 3.14159265358979323846
using namespace std;
using namespace cv;

namespace ty
{
	rBRIEF::rBRIEF() : rBRIEF(BRIEF_DEFAULT_WINDOW_SIZE)
	{

	}
	rBRIEF::rBRIEF(int S) : rBRIEF(S, BRIEF_DEFAULT_TEST_COUNT)
	{

	}
	rBRIEF::rBRIEF(int S, int count)
	{
		generateLUT(BRIEF::GenerateBinaryTests(count, S),sBRIEF_DEFAULT_LUT_SIZE);
	}
	rBRIEF::rBRIEF(vector<BRIEF::BinaryTest>& tests)
	{
		generateLUT(tests, sBRIEF_DEFAULT_LUT_SIZE);
	}
	rBRIEF::~rBRIEF()
	{

	}
	void rBRIEF::generateLUT(vector<BRIEF::BinaryTest> tests, const int angleCount)
	{
		double delta = 2 * M_PI / angleCount;
		double ang = 0;
		for (int i = 0; i <angleCount; ++i, ang += delta)
		{
			double _sin = sin(ang);
			double _cos = cos(ang);
			vector<BRIEF::BinaryTest> test;
			for (vector<BRIEF::BinaryTest>::iterator p = tests.begin(); p != tests.end(); ++p)
			{
				test.push_back(p->Rotate(_cos,_sin));
			}
			lut.push_back(test);
		}
	}

	std::vector<BRIEF::BinaryTest> rBRIEF::operator [](int i) const
	{
		return std::vector<BRIEF::BinaryTest>(lut[i]);
	}

	std::vector<BRIEF::Feature> rBRIEF::extractFeatures(uint8_t** image, std::vector<Keypoint>& positions) const
	{
		std::vector<BRIEF::Feature> features = std::vector<BRIEF::Feature>(positions.size());
		int length = positions.size();
		#pragma omp parallel for
		for (int i = 0; i < length; ++i)
		{
			vector<BRIEF::BinaryTest> tests = lut[positions[i].z];
			vector<Keypoint>::iterator p = positions.begin() + i;
			Feature f; int bitpos = 0;
			for (int k = 0; k < BRIEF_DEFAULT_TEST_COUNT; ++k)
			{
				vector<BinaryTest>::const_iterator t = tests.begin() + k;
				int x1 = p->x + t->x1; int y1 = p->y + t->y1;
				int x2 = p->x + t->x2; int y2 = p->y + t->y2;
				f.setbit(bitpos, image[y1][x1] > image[y2][x2]);
				++bitpos;
			}
			f.position = p->position();
			features[i] = f;
		}
		return features;

	}

}